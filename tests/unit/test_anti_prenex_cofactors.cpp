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
#include "anti_prenex/normalisers/simplify.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using tb = tau_term_bdd<node_t>;
using th = term_handle<node_t>;

tref bf(const char* s) {
	tref t = tau::get(s, parse_bf());
	REQUIRE(t != nullptr);
	return t;
}
/// The trimmed `variable` node of a one-variable term — the shape
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

/// Semantic equality of two terms: equal ROBDDs (leaves opaque), both
/// finished first.
bool same_function(tref a, tref b) {
	return tb::build_bdd(th::convert_to_tau_terms(a))
		== tb::build_bdd(th::convert_to_tau_terms(b));
}

/// Is there a node of this kind anywhere in `n`?
bool holds(tref n, size_t nt) {
	return tau::get(n).find_top([nt](tref m) {
		return tree<node_t>::get(m).is(nt); }) != nullptr;
}

/// The term of a one-atom body, prepared.
tref prepared(tref atom) {
	return sides(ap::prepare_terms<node_t>(atom)).first;
}

/// The witness `f₁′` a pin record carries.
tref witness_of(const ap::cof_entry& e) {
	REQUIRE(e.pin);
	return e.witness;
}

} // namespace

TEST_SUITE("anti_prenex/shared/cofactors") {

// --- COF ---------------------------------------------------------------------------

TEST_CASE("C1: a strict pin — the spelled shape x + t = 0") {
	tref x = vr("x");
	tref f = prepared(eq0(lxor(bf("x"), bf("t"))));
	REQUIRE(th::is_bdd_backed(f));

	const ap::cof_entry e = ap::cof<node_t>(f, x);
	CHECK(e.usable);
	CHECK(e.pin);
	// STRICT: the residual folds to `0`, so the pinning conjunct says
	// nothing beyond the pin itself.
	REQUIRE(e.p != nullptr);
	CHECK(tau::get(e.p).equals_0());
	CHECK(e.strict);
	// the cofactors are `t` and `t′`, and the witness is the lower end
	// `f₁′`, the complement of `f₁`'s BDD
	CHECK(same_function(e.f0, bf("t")));
	CHECK(same_function(e.f1, bf("t'")));
	CHECK(same_function(witness_of(e), bf("t")));
	CHECK(tb::build_bdd(e.witness) == tb::bdd_not(tb::build_bdd(e.f1)));
}

TEST_CASE("C1b: STRICT by an open node test — a residual the leaf product cannot see") {
	tref x = vr("x");
	// `x′·A ∪ x·B` with `A = r ∪ s` and `B = r′·s′·(r′ ∪ t)`, which is
	// `A′` spelled otherwise: the union and the product are single leaves,
	// `1` and `0` only once the sweep has been through them.
	tref f = prepared(eq0(bf(
		"x' & (r(a) | s(a)) | x & r(a)' & s(a)' & (r(a)' | t(a))")));
	REQUIRE(th::is_bdd_backed(f));
	const ap::cof_entry e = ap::cof<node_t>(f, x);
	const tb::ref b0 = tb::build_bdd(e.f0), b1 = tb::build_bdd(e.f1);
	REQUIRE(tb::bdd_or(b0, b1) != tb::T);
	REQUIRE(tb::bdd_and(b0, b1) != tb::F);
	CHECK(e.usable);
	CHECK(e.pin);
	CHECK(e.strict);
	// `p` is carried: recorded as the strict test returned it
	REQUIRE(e.p != nullptr);
	CHECK(tau::get(e.p).equals_0());
	CHECK(tb::build_bdd(e.witness) == tb::bdd_not(b1));
}

TEST_CASE("C2: the WEAK pin (y + t) ∪ G = 0 keeps its residual") {
	tref y = vr("y");
	const ap::block P{ y };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	// `G` is a product, so no variable of it pins on its own; the
	// cofactors of `y` are `t ∪ G` and `t′ ∪ G`, complementary only where
	// `G = 0`.
	const tref G = land(bf("c"), bf("d"));
	tref f = prepared(eq0(lor(lxor(bf("y"), bf("t")), G)));
	REQUIRE(th::is_bdd_backed(f));

	const ap::cof_entry e = ap::cof<node_t>(f, y);
	CHECK(e.usable);
	CHECK(e.pin);
	REQUIRE(e.p != nullptr);
	CHECK(!tau::get(e.p).equals_0());               // weak, not strict
	CHECK(!e.strict);
	// the residual is `G` and the witness is `t·G′` (§3's hidden pin)
	CHECK(same_function(e.p, G));
	CHECK(same_function(witness_of(e), land(bf("t"), lneg(G))));
}

TEST_CASE("C3: what does not pin, and the LEAF HAZARD") {
	tref x = vr("x");
	// `x·a = 0`: the cofactors are `0` and `a`, whose union is not 1.
	tref f = prepared(eq0(land(bf("x"), bf("a"))));
	const ap::cof_entry e = ap::cof<node_t>(f, x);
	CHECK(e.usable);
	CHECK(!e.pin);
	REQUIRE(e.p != nullptr);
	CHECK(same_function(e.f0, tau::_0(find_ba_type<node_t>(f))));
	CHECK(same_function(e.f1, bf("a")));
	// no pin: no witness, nothing strict
	CHECK(e.witness == nullptr);
	CHECK(!e.strict);

	// `x·a ∪ r(x)`: `x` is a decision variable AND sits inside a
	// reference argument, where no cofactor reaches it (§1). The record
	// says so rather than answering about the occurrences it saw.
	tref h = prepared(eq0(lor(land(bf("x"), bf("a")), bf("r(x)"))));
	REQUIRE(th::is_bdd_backed(h));
	const ap::cof_entry hz = ap::cof<node_t>(h, x);
	CHECK(!hz.usable);
	CHECK(!hz.pin);
	CHECK(hz.p == nullptr);
	CHECK(hz.witness == nullptr);
}

TEST_CASE("C4: child selection passes through a kept chain in a leaf") {
	tref x = vr("x"), y = vr("y");
	// `∀y (y ∪ r(a))` holds `a` free in a reference, so the slide leaves it
	// ONE LEAF of the term's BDD (§1) — a kept emission of an inner block
	// looks exactly like this.
	tref chain = tb::build_functional_quantifiers({{ y, tb::all }},
		lor(bf("y"), bf("r(a)")));
	REQUIRE(tau::get(chain).child_is(tau::bf_fall));
	tref f = prepared(eq0(lor(land(bf("x"), chain),
		land(lneg(bf("x")), bf("b")))));
	REQUIRE(th::is_bdd_backed(f));

	const ap::cof_entry e = ap::cof<node_t>(f, x);
	// `x` is gone from both cofactors, and the chain rode out whole on
	// the one that selected it.
	CHECK(e.usable);
	CHECK(!e.pin);
	CHECK(ap::carries_functional_quantifier<node_t>(e.f1));
	CHECK(same_function(e.f0, bf("b")));
}

TEST_CASE("C5: the record is a pure function of (f, x), cached under TAU_CACHE") {
	tref x = vr("x");
	tref f = prepared(eq0(lor(land(bf("x"), bf("ma")),
		land(lneg(bf("x")), bf("mb")))));
	// The table is a cache: it does not exist in Debug, so the answer is
	// the same either way and only the entry is build-type specific.
	CHECK(ap::find<node_t, ap::table::cof_memo>({ f, x }) == nullptr);
	const ap::cof_entry first = ap::cof<node_t>(f, x);
	const ap::cof_entry again = ap::cof<node_t>(f, x);
	CHECK(first.usable == again.usable);
	CHECK(first.pin == again.pin);
	CHECK(first.strict == again.strict);
	CHECK(same(first.f0, again.f0));
	CHECK(same(first.f1, again.f1));
#ifdef TAU_CACHE
	CHECK(ap::find<node_t, ap::table::cof_memo>({ f, x }) != nullptr);
#else
	CHECK(ap::find<node_t, ap::table::cof_memo>({ f, x }) == nullptr);
#endif
}

TEST_CASE("C6: ONE pin recipe — the plain regime's match agrees (D9)") {
	tref y = vr("y");
	const tref G = land(bf("c"), bf("d"));
	// The plain regime takes its cofactors by SUBSTITUTION; here they are
	// written out, and `pin_from_cofactors` must say what `find_pin_for`
	// says about the same atom.
	auto weak = ap::find_pin_for<node_t>(
		eq0(lor(lxor(bf("y"), bf("t")), G)), y);
	REQUIRE(weak.has_value());
	const ap::cof_entry we = ap::pin_from_cofactors<node_t>(
		lor(bf("t"), G), lor(lneg(bf("t")), G), y);
	CHECK(we.usable);
	CHECK(we.pin);
	CHECK(tau::get(we.p).equals_0() == weak->strict);
	CHECK(we.strict == weak->strict);
	CHECK(same_function(witness_of(we), weak->witness));

	auto strict = ap::find_pin_for<node_t>(eq0(lxor(bf("y"), bf("t"))), y);
	REQUIRE(strict.has_value());
	const ap::cof_entry se = ap::pin_from_cofactors<node_t>(
		bf("t"), lneg(bf("t")), y);
	CHECK(se.pin);
	CHECK(tau::get(se.p).equals_0() == strict->strict);
	CHECK(se.strict);
	CHECK(strict->strict);
	CHECK(same_function(witness_of(se), strict->witness));

	// The backed regime's match is `COF`'s record on the same atom.
	tref atom = ap::prepare_terms<node_t>(
		eq0(lor(lxor(bf("y"), bf("t")), G)));
	auto backed = ap::find_pin_for<node_t>(atom, y, ap::regime::backed);
	REQUIRE(backed.has_value());
	const ap::cof_entry be = ap::cof<node_t>(sides(atom).first, y);
	CHECK(be.pin);
	CHECK(backed->witness == be.witness);
	CHECK(backed->strict == be.strict);
	CHECK(same_function(be.witness, weak->witness));
}

#ifdef TAU_CACHE
TEST_CASE("C7: the plain match and COF keep apart records under one key (Release)") {
	tref x = vr("x");
	// `r(x) = 0`: its term is the single leaf `r(x)`, one node in both
	// regimes. The plain match substitutes into the reference, so `x` is
	// gone from both cofactors and its record is usable, with no pin.
	const tref f = bf("r(x)");
	CHECK(!ap::find_pin_for<node_t>(eq0(f), x).has_value());
	const ap::cof_entry* plain =
		ap::find<node_t, ap::table::plain_cof_memo>({ f, x });
	REQUIRE(plain != nullptr);
	CHECK(plain->usable);
	CHECK(!plain->pin);
	// COF on the same key: a restrict never reaches into a leaf, so `x`
	// hides there and COF's own record is not usable.
	const ap::cof_entry e = ap::cof<node_t>(f, x);
	CHECK(!e.usable);
	CHECK(e.p == nullptr);
	const ap::cof_entry* backed =
		ap::find<node_t, ap::table::cof_memo>({ f, x });
	REQUIRE(backed != nullptr);
	CHECK(!backed->usable);
}
#endif

// --- DISCHARGE, discharging mode ---------------------------------------------------

TEST_CASE("D1: ∀_X f = 0 and ∃_X h ≠ 0 for one variable") {
	tref x = vr("x");
	const ap::block P{ x };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	tref f = prepared(eq0(lor(land(bf("x"), bf("a")),
		land(lneg(bf("x")), bf("b")))));

	// positive: `∃x. f = 0` is `∀_x f = 0`, i.e. `a·b = 0`
	tref pos = ap::discharge<node_t>(f, false, P, c);
	CHECK(is_child<node_t>(pos, tau::bf_eq));
	CHECK(tau::get(sides(pos).second).equals_0());
	CHECK(same_function(sides(pos).first,
		ap::quantify_over<node_t>(tb::all, f, P)));
	CHECK(same_function(sides(pos).first, land(bf("a"), bf("b"))));

	// negative: `∃x. f ≠ 0` is `∃_x f ≠ 0`, i.e. `¬(a ∪ b = 0)` — under a
	// formula negation, never `bf_neq` (invariant 4)
	tref neg = ap::discharge<node_t>(f, true, P, c);
	REQUIRE(tau::get(neg).child_is(tau::wff_neg));
	tref inner = ap::atom_of<node_t>(neg);
	CHECK(is_child<node_t>(inner, tau::bf_eq));
	CHECK(!holds(neg, tau::bf_neq));
	CHECK(same_function(sides(inner).first,
		ap::quantify_over<node_t>(tb::ex, f, P)));
	CHECK(same_function(sides(inner).first, lor(bf("a"), bf("b"))));

	// an empty block quantifies nothing and the atom is the term's own
	CHECK(same_function(sides(ap::discharge<node_t>(f, false, {}, c)).first,
		f));
}

TEST_CASE("D2: a whole block of two, and a sub-block that leaves one behind") {
	tref x = vr("x"), y = vr("y");
	const ap::block P{ x, y };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	// `a ∪ x·b ∪ y·c`, whose two quantifications below are both
	// non-constant: `∀_{x,y} f = a` and `∀_x f = a ∪ y·c`.
	tref f = prepared(eq0(lor(lor(bf("a"), land(bf("x"), bf("b"))),
		land(bf("y"), bf("cc")))));
	REQUIRE(th::is_bdd_backed(f));

	// |X| = 2: ONE quantification, never 4 terms
	tref two = ap::discharge<node_t>(f, false, P, c);
	CHECK(same_function(sides(two).first,
		ap::quantify_over<node_t>(tb::all, f, P)));
	CHECK(same_function(sides(two).first, bf("a")));
	CHECK(!ap::fv_meets<node_t>(sides(two).first, P));

	// a SUB-BLOCK: what is left still depends on `y`, and the term stays
	// BDD-backed
	const ap::block X{ x };
	tref one = ap::discharge<node_t>(f, false, X, c);
	tref l = sides(one).first;
	CHECK(th::is_bdd_backed(l));
	CHECK(same_function(l, ap::quantify_over<node_t>(tb::all, f, X)));
}

// --- DISCHARGE, keep mode ----------------------------------------------------------

TEST_CASE("D3: a whole-block keep emission is decided at its emission") {
	tref x = vr("x"), y = vr("y");
	const ap::block P{ x, y };
	auto plain = ap::ctx<node_t>::for_component(P, 0, false);
	auto keep = ap::ctx<node_t>::for_component(P, 0, true);
	trefs asked;
	const ap::keep_functional_fn<node_t> keep_all = [&asked](tref n) {
		asked.push_back(n);
		return true;
	};
	keep.kf = keep_all;
	tref f = prepared(eq0(lor(lor(bf("a"), land(bf("x"), bf("b"))),
		land(bf("y"), bf("cc")))));
	REQUIRE(th::is_bdd_backed(f));

	// `kf` is asked ONCE, on the canonical chain over the stored BDD as
	// built (§1 ctx table)
	tref kept = ap::discharge<node_t>(f, false, P, keep);
	REQUIRE(asked.size() == 1);
	CHECK(asked[0] == tb::build_functional_quantifiers(
		{{ x, tb::all }, { y, tb::all }}, f));
	// its yes keeps the chain SLID to P's levels: `a`, `b` and `cc` sort
	// above `x` and `y` and stay decision structure, and every piece over
	// `x`, `y` is closed and folds, so the emission is the discharging
	// mode's own answer, `a`
	tref discharged = ap::discharge<node_t>(f, false, P, plain);
	CHECK(same_function(sides(kept).first, sides(discharged).first));
	CHECK(same_function(sides(kept).first, bf("a")));

	// a piece holding a free variable in a leaf stays a chain:
	// `c ∪ x·y·r(b)` keeps ∀x ∀y (x·y·r(b)) under `c′`, one leaf
	tref g = prepared(eq0(lor(bf("c"), land(land(bf("x"), bf("y")),
		bf("r(b)")))));
	REQUIRE(th::is_bdd_backed(g));
	tref kg = sides(ap::discharge<node_t>(g, false, P, keep)).first;
	CHECK(asked.size() == 2);
	CHECK(ap::carries_functional_quantifier<node_t>(kg));
	CHECK(!ap::fv_meets<node_t>(kg, P));         // X-free, the block is bound
	REQUIRE(th::is_bdd_backed(kg));
	const tb::ref kb = th::convert_to_handle(kg).get();
	CHECK(same(tb::get_var(kb), vr("c")));
	CHECK(tb::get_high(kb) == tb::T);
	REQUIRE(tb::leaf(tb::get_low(kb)));
	CHECK(ap::strip_chain<node_t>(tb::get_var_term(tb::get_low(kb)))
		.first.size() == 2);

	// a chain `kf` does not keep is quantified at once
	auto keep_none = ap::ctx<node_t>::for_component(P, 0, true);
	CHECK(ap::discharge<node_t>(f, false, P, keep_none) == discharged);
}

TEST_CASE("D4: a sub-block with block variables pending below its levels is quantified at once") {
	tref x = vr("x"), y = vr("y");
	tref f = prepared(eq0(lor(land(land(bf("x"), bf("y")), bf("a")),
		land(lneg(bf("x")), bf("b")))));
	REQUIRE(th::is_bdd_backed(f));
	const ap::block P{ x, y };
	auto plain = ap::ctx<node_t>::for_component(P, 0, false);
	auto keep = ap::ctx<node_t>::for_component(P, 0, true);
	size_t asked = 0;
	const ap::keep_functional_fn<node_t> keep_all = [&asked](tref) {
		return ++asked, true;
	};
	keep.kf = keep_all;

	// the sub-block `{x}` with `y` pending BELOW its levels: quantified at
	// once, whatever `kf` would answer, so no chain hides `y` (§1)
	tref low = ap::discharge<node_t>(f, false, ap::block{ x }, keep);
	CHECK(asked == 0);
	CHECK(low == ap::discharge<node_t>(f, false, ap::block{ x }, plain));
	CHECK(!ap::carries_functional_quantifier<node_t>(sides(low).first));

	// the sub-block `{y}` with `x` pending ABOVE: slid, ∀y (x·y·a ∪ x′·b) =
	// x′·b, and `x` stays a decision variable, which `COF` reads
	tref high = sides(ap::discharge<node_t>(f, false, ap::block{ y }, keep))
		.first;
	CHECK(asked == 1);
	CHECK(same_function(high, land(lneg(bf("x")), bf("b"))));
	const ap::cof_entry e = ap::cof<node_t>(high, x);
	CHECK(e.usable);
}

} // TEST_SUITE
