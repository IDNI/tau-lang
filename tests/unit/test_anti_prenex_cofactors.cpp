// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 3 unit tests for src/anti_prenex/shared/cofactors.h: §6 COF and §7
// DISCHARGE. Spec: anti_prenex.md §6 (COF, cof_memo), §7 (DISCHARGE), §1
// (the term representation, the leaf hazard, cache scope), §3 (TRY_WITNESS's
// pin test, shared with the plain regime).
//
// Everything runs on PREPARED terms: a block, the component's ctx and its
// order, and `prepare_terms` over it — the production spelling. Never assert
// on to_str() of a BDD-backed term; identity is trefs and BDD refs, and
// semantic equality is `same_function`.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/shared/cofactors.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using tb = tau_term_bdd<node_t>;
using th = term_handle<node_t>;
using order_t = ap::var_order<node_t>;

tref bf(const char* s) {
	tref t = tau::get(s, parse_bf());
	REQUIRE(t != nullptr);
	return t;
}
/// The trimmed `variable` node of a one-variable term — the shape order keys,
/// blocks and free-variable sets hold.
tref vr(const char* s) { return tau::trim(bf(s)); }

tref eq0(tref t)          { return build_bf_eq_0<node_t>(t); }
tref land(tref l, tref r) { return build_bf_and<node_t>(l, r); }
tref lor(tref l, tref r)  { return build_bf_or<node_t>(l, r); }
tref lneg(tref t)         { return build_bf_neg<node_t>(t); }
tref lxor(tref l, tref r) { return build_bf_xor<node_t>(l, r); }

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// The `l = r` sides of an atom, through no negation.
std::pair<tref, tref> sides(tref atom) {
	const tau& t = tau::get(atom);
	REQUIRE(t.is(tau::wff));
	REQUIRE(t[0].has_child());
	return { tau::trim_right_sibling(t[0].first()),
		tau::trim_right_sibling(t[0].second()) };
}

/// Semantic equality of two terms over the variables `vs`: equal ROBDDs
/// under one order over all of them (leaves opaque), both finished first.
bool same_function(tref a, tref b, const ap::block& vs) {
	order_t o;
	for (size_t i = 0; i < vs.size(); ++i)
		o.emplace(vs[i], int_t(vs.size() - i));
	return tb::build_bdd(th::convert_to_tau_terms(a), o)
		== tb::build_bdd(th::convert_to_tau_terms(b), o);
}

/// Is there a node of this kind anywhere in `n`?
bool holds(tref n, size_t nt) {
	return tau::get(n).find_top([nt](tref m) {
		return tree<node_t>::get(m).is(nt); }) != nullptr;
}

/// §5's component setup, and the term of a one-atom body prepared over it.
tref prepared(tref atom, const ap::block& P, const order_t& o) {
	return sides(ap::prepare_terms<node_t>(atom, P, o)).first;
}

/// The witness `f₁′` a consumer of `COF` builds for itself (§6: there is no
/// slot for it in the record).
tref witness_of(const ap::cof_entry& e, const order_t& o) {
	return ap::simplify_term<node_t>(build_bf_neg<node_t>(e.f1), o);
}

} // namespace

TEST_SUITE("anti_prenex/shared/cofactors") {

// --- COF ---------------------------------------------------------------------------

TEST_CASE("C1: a strict pin — the spelled shape x + t = 0") {
	tref x = vr("x"), t = vr("t");
	const ap::block P{ x };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	tref f = prepared(eq0(lxor(bf("x"), bf("t"))), P, c.order);
	REQUIRE(th::is_bdd_backed(f));

	const ap::cof_entry e = ap::cof<node_t>(f, x, c);
	CHECK(e.usable);
	CHECK(e.pin);
	// STRICT: the residual folds to `0`, so the pinning conjunct says
	// nothing beyond the pin itself.
	REQUIRE(e.p != nullptr);
	CHECK(tau::get(e.p).equals_0());
	// the cofactors are `t` and `t′`, and the witness is the lower end `f₁′`
	CHECK(same_function(e.f0, bf("t"), { t }));
	CHECK(same_function(e.f1, bf("t'"), { t }));
	CHECK(same_function(witness_of(e, c.order), bf("t"), { t }));
}

TEST_CASE("C2: the WEAK pin (y + t) ∪ G = 0 keeps its residual") {
	tref y = vr("y"), t = vr("t"), cc = vr("c"), d = vr("d");
	const ap::block P{ y };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	// `G` is a product, so no variable of it pins on its own; the
	// cofactors of `y` are `t ∪ G` and `t′ ∪ G`, complementary only where
	// `G = 0`.
	const tref G = land(bf("c"), bf("d"));
	tref f = prepared(eq0(lor(lxor(bf("y"), bf("t")), G)), P, c.order);
	REQUIRE(th::is_bdd_backed(f));

	const ap::cof_entry e = ap::cof<node_t>(f, y, c);
	CHECK(e.usable);
	CHECK(e.pin);
	REQUIRE(e.p != nullptr);
	CHECK(!tau::get(e.p).equals_0());               // weak, not strict
	// the residual is `G` and the witness is `t·G′` (§3's hidden pin)
	CHECK(same_function(e.p, G, { t, cc, d }));
	CHECK(same_function(witness_of(e, c.order), land(bf("t"), lneg(G)),
		{ t, cc, d }));
}

TEST_CASE("C3: what does not pin, and the LEAF HAZARD") {
	tref x = vr("x"), a = vr("a");
	const ap::block P{ x };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	// `x·a = 0`: the cofactors are `0` and `a`, whose union is not 1.
	tref f = prepared(eq0(land(bf("x"), bf("a"))), P, c.order);
	const ap::cof_entry e = ap::cof<node_t>(f, x, c);
	CHECK(e.usable);
	CHECK(!e.pin);
	REQUIRE(e.p != nullptr);
	CHECK(same_function(e.f0, tau::_0(find_ba_type<node_t>(f)), { a }));
	CHECK(same_function(e.f1, bf("a"), { a }));

	// `x·a ∪ r(x)`: `x` is a decision variable AND sits inside a
	// reference argument, where no cofactor reaches it (§1). The record
	// says so rather than answering about the occurrences it saw.
	tref h = prepared(eq0(lor(land(bf("x"), bf("a")), bf("r(x)"))), P,
		c.order);
	REQUIRE(th::is_bdd_backed(h));
	const ap::cof_entry hz = ap::cof<node_t>(h, x, c);
	CHECK(!hz.usable);
	CHECK(!hz.pin);
	CHECK(hz.p == nullptr);
}

TEST_CASE("C4: child selection passes through a kept chain in a leaf") {
	tref x = vr("x"), y = vr("y");
	const ap::block P{ x };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	// `∀y (y ∪ a)` binds no decision variable, so the library keeps it as
	// ONE LEAF of the term's BDD (§1) — a kept emission of an inner block
	// looks exactly like this.
	tref chain = tb::build_functional_quantifiers({{ y, tb::all }},
		lor(bf("y"), bf("a")));
	REQUIRE(tau::get(chain).child_is(tau::bf_fall));
	tref f = prepared(eq0(lor(land(bf("x"), chain),
		land(lneg(bf("x")), bf("b")))), P, c.order);
	REQUIRE(th::is_bdd_backed(f));

	const ap::cof_entry e = ap::cof<node_t>(f, x, c);
	// `x` is gone from both cofactors, and the chain rode out whole on
	// the one that selected it.
	CHECK(e.usable);
	CHECK(!e.pin);
	CHECK(ap::carries_functional_quantifier<node_t>(e.f1));
	CHECK(same(e.f0, bf("b")));
}

TEST_CASE("C5: the record is a pure function of (f, x), cached under TAU_CACHE") {
	tref x = vr("x");
	const ap::block P{ x };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	tref f = prepared(eq0(lor(land(bf("x"), bf("ma")),
		land(lneg(bf("x")), bf("mb")))), P, c.order);
	// The table is a cache: it does not exist in Debug, so the answer is
	// the same either way and only the entry is build-type specific.
	CHECK(ap::find<node_t, ap::table::cof_memo>({ f, x }) == nullptr);
	const ap::cof_entry first = ap::cof<node_t>(f, x, c);
	const ap::cof_entry again = ap::cof<node_t>(f, x, c);
	CHECK(first.usable == again.usable);
	CHECK(first.pin == again.pin);
	CHECK(same(first.f0, again.f0));
	CHECK(same(first.f1, again.f1));
#ifdef TAU_CACHE
	CHECK(ap::find<node_t, ap::table::cof_memo>({ f, x }) != nullptr);
#else
	CHECK(ap::find<node_t, ap::table::cof_memo>({ f, x }) == nullptr);
#endif
}

TEST_CASE("C6: ONE pin recipe — the plain regime's match agrees (D9)") {
	tref y = vr("y"), t = vr("t"), cc = vr("c"), d = vr("d");
	const tref G = land(bf("c"), bf("d"));
	// The plain regime takes its cofactors by SUBSTITUTION; here they are
	// written out, and `pin_from_cofactors` must say what `find_pin_for`
	// says about the same atom.
	auto weak = ap::find_pin_for<node_t>(
		eq0(lor(lxor(bf("y"), bf("t")), G)), y);
	REQUIRE(weak.has_value());
	const ap::cof_entry we = ap::pin_from_cofactors<node_t>(
		lor(bf("t"), G), lor(lneg(bf("t")), G), y, {});
	CHECK(we.usable);
	CHECK(we.pin);
	CHECK(tau::get(we.p).equals_0() == weak->strict);
	CHECK(same_function(witness_of(we, {}), weak->witness, { t, cc, d }));

	auto strict = ap::find_pin_for<node_t>(eq0(lxor(bf("y"), bf("t"))), y);
	REQUIRE(strict.has_value());
	const ap::cof_entry se = ap::pin_from_cofactors<node_t>(
		bf("t"), lneg(bf("t")), y, {});
	CHECK(se.pin);
	CHECK(tau::get(se.p).equals_0() == strict->strict);
	CHECK(strict->strict);
	CHECK(same_function(witness_of(se, {}), strict->witness, { t }));
}

// --- DISCHARGE, discharging mode ---------------------------------------------------

TEST_CASE("D1: ∀_X f = 0 and ∃_X h ≠ 0 for one variable") {
	tref x = vr("x"), a = vr("a"), b = vr("b");
	const ap::block P{ x };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	tref f = prepared(eq0(lor(land(bf("x"), bf("a")),
		land(lneg(bf("x")), bf("b")))), P, c.order);

	// positive: `∃x. f = 0` is `∀_x f = 0`, i.e. `a·b = 0`
	tref pos = ap::discharge<node_t>(f, false, P, c);
	CHECK(is_child<node_t>(pos, tau::bf_eq));
	CHECK(tau::get(sides(pos).second).equals_0());
	CHECK(same_function(sides(pos).first,
		ap::quantify_over<node_t>(tb::all, f, P, c.order), { a, b }));
	CHECK(same_function(sides(pos).first, land(bf("a"), bf("b")), { a, b }));

	// negative: `∃x. f ≠ 0` is `∃_x f ≠ 0`, i.e. `¬(a ∪ b = 0)` — under a
	// formula negation, never `bf_neq` (invariant 4)
	tref neg = ap::discharge<node_t>(f, true, P, c);
	REQUIRE(tau::get(neg).child_is(tau::wff_neg));
	tref inner = ap::atom_of<node_t>(neg);
	CHECK(is_child<node_t>(inner, tau::bf_eq));
	CHECK(!holds(neg, tau::bf_neq));
	CHECK(same_function(sides(inner).first,
		ap::quantify_over<node_t>(tb::ex, f, P, c.order), { a, b }));
	CHECK(same_function(sides(inner).first, lor(bf("a"), bf("b")), { a, b }));

	// an empty block quantifies nothing and the atom is the term's own
	CHECK(same_function(sides(ap::discharge<node_t>(f, false, {}, c)).first,
		f, { x, a, b }));
}

TEST_CASE("D2: a whole block of two, and a sub-block that leaves one behind") {
	tref x = vr("x"), y = vr("y"), a = vr("a"), b = vr("b"), cc = vr("cc");
	const ap::block P{ x, y };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	// `a ∪ x·b ∪ y·c`, whose two quantifications below are both
	// non-constant: `∀_{x,y} f = a` and `∀_x f = a ∪ y·c`.
	tref f = prepared(eq0(lor(lor(bf("a"), land(bf("x"), bf("b"))),
		land(bf("y"), bf("cc")))), P, c.order);
	REQUIRE(th::is_bdd_backed(f));

	// |X| = 2: ONE quantification, never 4 terms
	tref two = ap::discharge<node_t>(f, false, P, c);
	CHECK(same_function(sides(two).first,
		ap::quantify_over<node_t>(tb::all, f, P, c.order), { a, b, cc }));
	CHECK(same_function(sides(two).first, bf("a"), { a }));
	CHECK(!th::is_bdd_backed(sides(two).first));  // X-free, hence plain

	// a SUB-BLOCK: what is left still depends on `y`, so the term stays
	// BDD-backed over `P ∖ X`
	const ap::block X{ x };
	tref one = ap::discharge<node_t>(f, false, X, c);
	tref l = sides(one).first;
	CHECK(th::is_bdd_backed(l));
	CHECK(same_function(l, ap::quantify_over<node_t>(tb::all, f, X, c.order),
		{ y, a, b, cc }));
}

// --- DISCHARGE, keep mode ----------------------------------------------------------

TEST_CASE("D3: a whole-block keep emission holds its BDD until the close") {
	tref x = vr("x"), y = vr("y"), a = vr("a"), b = vr("b"), cc = vr("cc");
	const ap::block P{ x, y };
	auto plain = ap::ctx<node_t>::for_component(P, 0, false);
	auto keep = ap::ctx<node_t>::for_component(P, 0, true);
	tref atom = eq0(lor(lor(bf("a"), land(bf("x"), bf("b"))),
		land(bf("y"), bf("cc"))));
	tref f = prepared(atom, P, plain.order);
	REQUIRE(th::is_bdd_backed(f));

	tref kept = ap::discharge<node_t>(f, false, P, keep);
	// the emission is the canonical chain over the STORED BDD, which
	// `SIMPLIFY_ATOM` leaves alone (§1: nothing spells a stored BDD out
	// before the component's close)
	tref l = sides(kept).first;
	REQUIRE(tau::get(l).child_is(tau::bf_fall));
	auto [prefix, body] = ap::strip_chain<node_t>(l);
	CHECK(prefix.size() == 2);
	CHECK(th::is_bdd_backed(body));
	CHECK(!ap::fv_meets<node_t>(l, P));      // X-free, the block is bound

	// the CLOSE resolves it with one quantification of that stored BDD,
	// and what comes out is the discharging mode's own answer
	tref closed = th::convert_to_tau_terms(
		ap::resolve_functional_quantifiers_bdd<node_t>(kept, keep.order,
			ap::keep_no_functional<node_t>));
	tref discharged = ap::discharge<node_t>(f, false, P, plain);
	CHECK(same_function(sides(closed).first, sides(discharged).first,
		{ a, b, cc }));
	CHECK(same_function(sides(closed).first, bf("a"), { a }));
}

TEST_CASE("D4: a settled sub-block's emission hides the variables it carries") {
	tref x = vr("x"), y = vr("y");
	const ap::block P{ x, y };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	tref f = prepared(eq0(lor(land(land(bf("x"), bf("y")), bf("a")),
		land(lneg(bf("x")), bf("b")))), P, c.order);
	REQUIRE(th::is_bdd_backed(f));
	// `∀y (BDD over {x, y})`, the shape the settle move emits in keep
	// mode: `x` is still inside the stored BDD, and no slide puts it back
	// on the outside (Q5).
	tref sub = tb::build_functional_quantifiers({{ y, tb::all }}, f);
	REQUIRE(tau::get(sub).child_is(tau::bf_fall));
	REQUIRE(ap::fv_meets<node_t>(sub, ap::block{ x }));

	// so `COF` on it refuses to answer about `x` (§1 leaf hazard)
	const ap::cof_entry e = ap::cof<node_t>(sub, x, c);
	CHECK(!e.usable);
	CHECK(!e.pin);
	CHECK(e.p == nullptr);
}

} // TEST_SUITE
