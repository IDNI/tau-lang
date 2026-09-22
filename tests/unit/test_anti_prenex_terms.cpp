// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 0 unit tests for src/anti_prenex/foundations/terms.h (package B).
// Spec: anti_prenex.md.
//
// Never assert on to_str() of a BDD-backed term: BDD_ID is an unreachable
// production of the grammar. Identity is checked through trefs (hash-consed)
// and through BDD refs (canonical under one order).

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/foundations/terms.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using tb   = tau_term_bdd<node_t>;
using th   = term_handle<node_t>;
using order_t = ap::var_order<node_t>;

tref bf(const char* s) {
	tref t = tau::get(s, parse_bf());
	REQUIRE(t != nullptr);
	return t;
}
tref wff(const char* s) {
	tref t = tau::get(s, parse_wff());
	REQUIRE(t != nullptr);
	return t;
}
/// The trimmed `variable` node of a one-variable term — the shape order keys,
/// blocks and free-variable sets hold.
tref vr(const char* s) { return tau::trim(bf(s)); }

/// `P[i] ↦ |P| - i` (§5): inner → LOWER rank.
order_t order_of(const ap::block& P) {
	order_t o;
	for (size_t i = 0; i < P.size(); ++i) o.emplace(P[i], int_t(P.size() - i));
	return o;
}

/// The BDD ref of a term under `o` (a BDD-backed term's own ref).
tb::ref ref_of(tref t, const order_t& o) {
	if (th::is_bdd_backed(t))
		return th::U.find(th::key_of(t))->second.get();
	return tb::build_bdd(t, o);
}

/// Semantic equality of two terms over the variables `vs`: equal ROBDDs
/// under one order over all of them (leaves opaque). Both are finished
/// first, since a BDD-backed term's own ref belongs to its own order.
bool same_function(tref a, tref b, const ap::block& vs) {
	order_t o = order_of(vs);
	return tb::build_bdd(th::convert_to_tau_terms(a), o)
		== tb::build_bdd(th::convert_to_tau_terms(b), o);
}

/// The prefix of the chain node a `keep_functional_fn` is handed (fwd.h),
/// outermost first with the kinds: what §3 says `keep` reads off that node.
tb::quants prefix_of(tref chain) { return ap::strip_chain<node_t>(chain).first; }

/// The `l = r` atom of a prepared body (the body IS the atom here).
std::pair<tref, tref> sides(tref atom) {
	const tau& t = tau::get(atom);
	REQUIRE(t.is(tau::wff));
	REQUIRE(t[0].has_child());
	return { tau::trim_right_sibling(t[0].first()),
		tau::trim_right_sibling(t[0].second()) };
}

bool has_bdd_id(tref n) {
	return tau::get(n).find_top([](tref m) {
		return tree<node_t>::get(m).is(tau::BDD_ID); }) != nullptr;
}

} // namespace

TEST_SUITE("anti_prenex/foundations/terms") {

// 1. prepare_terms ------------------------------------------------------------

TEST_CASE("prepare_terms: decision variables are exactly P, leaves hold the rest") {
	tref x = vr("x"), y = vr("y"), z = vr("z");
	ap::block P{ x, y };
	order_t o = order_of(P);
	tref body = wff("x & y & z = 0");
	tref prepared = ap::prepare_terms<node_t>(body, P, o);
	auto [l, r] = sides(prepared);
	CHECK(th::is_bdd_backed(l));
	CHECK(!th::is_bdd_backed(r)); // the constant side stays a constant
	tb::ref f = ref_of(l, o);
	// the inner variable y has the LOWER rank and sits on top, x below it;
	// z is a leaf
	REQUIRE(!tb::leaf(f));
	CHECK(tau::subtree_equals(tb::get_var(f), y));
	CHECK(tau::subtree_equals(tb::get_var(tb::get_high(f)), x));
	// z is not a decision variable anywhere: it sits in a leaf
	trefs lfv = ap::leaf_fv<node_t>(l);
	CHECK(std::binary_search(lfv.begin(), lfv.end(), z, tau::subtree_less));
	CHECK(!std::binary_search(lfv.begin(), lfv.end(), x, tau::subtree_less));
	// the same function as the input
	CHECK(same_function(l, bf("x & y & z"), { x, y, z }));
}

TEST_CASE("prepare_terms: an atom not touching P is the same tref") {
	tref x = vr("x");
	ap::block P{ x };
	order_t o = order_of(P);
	tref body = wff("y & z = 0");
	CHECK(ap::prepare_terms<node_t>(body, P, o) == body);
	tref both = wff("x = 0 && y & z = 0");
	tref prepared = ap::prepare_terms<node_t>(both, P, o);
	CHECK(prepared != both);
	// the untouched conjunct is the same node inside the result
	tref yz = wff("y & z = 0");
	CHECK(tau::get(prepared).find_top([yz](tref m) {
		return tree<node_t>::get(m) == tree<node_t>::get(yz); }) != nullptr);
	CHECK(!has_bdd_id(yz));
}

TEST_CASE("prepare_terms: an order atom stays plain, a binder is opaque") {
	tref x = vr("x");
	ap::block P{ x };
	order_t o = order_of(P);
	// a Boolean `<=` is decomposed into equations by the construction
	// hooks, so the only order atoms that survive are bitvector ones
	CHECK(!tau::get(wff("x & y <= x")).child_is(tau::bf_lteq));
	tref xb = vr("x:bv[8]");
	ap::block Pb{ xb };
	order_t ob = order_of(Pb);
	tref lt = wff("x:bv[8] & y:bv[8] <= x:bv[8]");
	REQUIRE(tau::get(lt).child_is(tau::bf_lteq));
	CHECK(ap::prepare_terms<node_t>(lt, Pb, ob) == lt);
	// while a bitvector EQUATION touching P is prepared
	tref eqb = wff("x:bv[8] & y:bv[8] = { 0 }:bv[8]");
	CHECK(ap::prepare_terms<node_t>(eqb, Pb, ob) != eqb);
	tref unit = wff("ex z (x & z = 0)");
	CHECK(ap::prepare_terms<node_t>(unit, P, o) == unit);
	// a negated equation is entered
	tref neg = wff("!(x & y = 0)");
	tref prepared = ap::prepare_terms<node_t>(neg, P, o);
	CHECK(prepared != neg);
	CHECK(tau::get(prepared).child_is(tau::wff_neg));
}

TEST_CASE("prepare_terms: equal sides fold through the hooks on BDD_ID operands (D4)") {
	tref x = vr("x"), y = vr("y");
	ap::block P{ x, y };
	order_t o = order_of(P);
	// (x & y) | (x & y') == x: the two sides are one BDD, one node, and
	// X = X ::= T fires on the interned operands.
	tref body = wff("(x & y) | (x & y') = x");
	tref prepared = ap::prepare_terms<node_t>(body, P, o);
	CHECK(tau::get(prepared).equals_T());
	// BDD_ID operands through the term hooks: X & X ::= X
	tref l = sides(ap::prepare_terms<node_t>(wff("x & y = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(build_bf_and<node_t>(l, l) == l);
	CHECK(tau::get(build_bf_eq<node_t>(l, l)).equals_T());
}

// 1b. finish_terms -------------------------------------------------------------

TEST_CASE("finish_terms: the round trip of prepare_terms is the identity by node") {
	tref x = vr("x");
	ap::block P{ x };
	order_t o = order_of(P);
	// The atoms are taken as PHASE 1 leaves them, in the plain regime's
	// normal form, because that is the form the close spells back to.
	const tref a = ap::simplify_atom<node_t>(wff("x & y = 0"), {});
	const tref b = ap::simplify_atom<node_t>(wff("!(x | z = 0)"), {});
	const tref phi = ap::canonical_and<node_t>(trefs{ a, b });
	const tref prepared = ap::prepare_terms<node_t>(phi, P, o);
	REQUIRE(has_bdd_id(prepared));
	CHECK(ap::finish_terms<node_t>(prepared) == phi);
	// the negated atom alone, so the negation rebuild is claimed on its own
	const tref one = ap::prepare_terms<node_t>(b, P, o);
	REQUIRE(has_bdd_id(one));
	CHECK(ap::finish_terms<node_t>(one) == b);
}

TEST_CASE("finish_terms: a kept chain comes back with a plain, canonical body") {
	tref x = vr("x");
	ap::block P{ x };
	order_t o = order_of(P);
	// §7 `DISCHARGE`'s keep-mode emission: a chain over the component's own
	// stored BDD. The close does NOT resolve it — it spells the body out,
	// in the plain regime's normal form, and the chain stands.
	tref l = sides(ap::prepare_terms<node_t>(wff("x & y = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	const tref atm = build_bf_eq_0<node_t>(
		tb::build_functional_quantifiers({{ x, tb::all }}, l));
	REQUIRE(has_bdd_id(atm));
	const tref got = ap::finish_terms<node_t>(atm);
	CHECK(!has_bdd_id(got));
	CHECK(ap::carries_functional_quantifier<node_t>(got));
	// SPELLED CANONICALLY: the plain atom simplification is the identity
	// on its own output, which is what makes `ANTI_PRENEX` a fixpoint on
	// its own output from the first run, a kept chain included.
	CHECK(ap::simplify_atom<node_t>(got, {}) == got);
}

TEST_CASE("finish_terms: a formula with no stored BDD is the same node") {
	const tref phi = ap::simplify_atom<node_t>(wff("x & y = 0"), {});
	REQUIRE(!has_bdd_id(phi));
	CHECK(ap::finish_terms<node_t>(phi) == phi);
	// A binder unit and a junction too: nothing is re-emitted when there
	// is nothing to spell.
	const tref psi = wff("ex z (x & z = 0) && !(y = 0)");
	REQUIRE(!has_bdd_id(psi));
	CHECK(ap::finish_terms<node_t>(psi) == psi);
}

TEST_CASE("finish_terms: an atom that decides folds through the junction") {
	tref x = vr("x");
	ap::block P{ x };
	order_t o = order_of(P);
	// `x = x′`: two DIFFERENT BDDs, so nothing folds while they are
	// stored — the hooks cannot read a complement pair off two interned
	// ids. Spelled out, the plain regime's joint `l + r` is `1`, which
	// decides the equation as `F`, and `F` decides the conjunction it
	// stood in.
	tref l = sides(ap::prepare_terms<node_t>(wff("x = 0"), P, o)).first;
	tref r = sides(ap::prepare_terms<node_t>(wff("x' = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	REQUIRE(th::is_bdd_backed(r));
	REQUIRE(l != r);
	const tref atm = build_bf_eq<node_t>(l, r);
	REQUIRE(has_bdd_id(atm));
	const tref phi = ap::canonical_and<node_t>(trefs{ atm, wff("z = 0") });
	REQUIRE(has_bdd_id(phi));
	CHECK(tau::get(ap::finish_terms<node_t>(phi)).equals_F());
}

// 2. cofactor -----------------------------------------------------------------

TEST_CASE("cofactor: the child when x is on top, a rebuild at any depth") {
	tref x = vr("x"), y = vr("y"), z = vr("z");
	ap::block P{ x, y };           // y inner → rank 1 → y on top
	order_t o = order_of(P);
	tref l = sides(ap::prepare_terms<node_t>(wff("x & y | x' & z = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	tb::ref f = ref_of(l, o);
	tref top = tb::get_var(f);
	CHECK(tau::subtree_equals(top, y));
	// top variable: child selection
	tref f1 = ap::cofactor<node_t>(l, y, true, o);
	tref f0 = ap::cofactor<node_t>(l, y, false, o);
	CHECK(ref_of(f1, o) == tb::get_high(f));
	CHECK(ref_of(f0, o) == tb::get_low(f));
	// a deeper variable: the nodes above it are rebuilt
	tref g1 = ap::cofactor<node_t>(l, x, true, o);
	tref g0 = ap::cofactor<node_t>(l, x, false, o);
	// x ← 1 leaves y; x ← 0 leaves the leaf z, which comes back PLAIN
	CHECK(same_function(g1, bf("y"), { y }));
	CHECK(!th::is_bdd_backed(g0));
	CHECK(tau::subtree_equals(g0, bf("z")));
	// identity on a variable that is not a decision variable
	CHECK(ap::cofactor<node_t>(l, z, true, o) == l);
	// identity on a plain term
	CHECK(ap::cofactor<node_t>(bf("z"), x, true, o) == bf("z"));
}

// 3. quantification -----------------------------------------------------------

TEST_CASE("quantify_over over all of P: meet / join of the leaves") {
	tref x = vr("x"), a = vr("a"), b = vr("b");
	ap::block P{ x };
	order_t o = order_of(P);
	tref l = sides(ap::prepare_terms<node_t>(wff("x & a | x' & b = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	tref all = ap::quantify_over<node_t>(tb::all, l, P, o);
	tref ex  = ap::quantify_over<node_t>(tb::ex,  l, P, o);
	CHECK(!th::is_bdd_backed(all));
	CHECK(!th::is_bdd_backed(ex));
	CHECK(same_function(all, bf("a & b"), { a, b }));
	CHECK(same_function(ex,  bf("a | b"), { a, b }));
	// over an empty block, or on a plain term: identity
	CHECK(ap::quantify_over<node_t>(tb::all, l, {}, o) == l);
	CHECK(ap::quantify_over<node_t>(tb::ex, bf("a"), P, o) == bf("a"));
	// over PART of the decision set: the library's quantification, and the
	// result stays BDD-backed on the rest
	tref y = vr("y");
	ap::block Q{ x, y };
	order_t oq = order_of(Q);
	tref g = sides(ap::prepare_terms<node_t>(wff("x & y & a | x' & b = 0"), Q, oq)).first;
	tref exx = ap::quantify_over<node_t>(tb::ex, g, { x }, oq);
	CHECK(th::is_bdd_backed(exx));
	trefs vx{ x };
	CHECK(ref_of(exx, oq) == tb::bdd_ex(ref_of(g, oq), vx, oq));
	CHECK(same_function(exx, bf("y & a | b"), { y, a, b }));
}

// 4. the library's slide, and RESOLVE_FUNCTIONAL -------------------------------

TEST_CASE("prepare_terms: the library slides a functional quantifier onto the leaves") {
	tref x = vr("x"), v = vr("v");
	ap::block P{ x };
	order_t o = order_of(P);
	// ∀_y (x·y ∪ x′·z) = x·∀_y y ∪ x′·∀_y z = x·0 ∪ x′·z: the high leaf is
	// a closed chain, which the constructor folds.
	tref l = sides(ap::prepare_terms<node_t>(wff("fall y (x & y | x' & z) = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(ref_of(l, o) == tb::build_bdd(bf("x' & z"), o));
	CHECK(!ap::carries_functional_quantifier<node_t>(l));
	// nested, with `v` free in the body so the chain survives on the leaf:
	// ∀_y ∃_w (x·y·w·v ∪ x′·z) → x·∀_y ∃_w (y·w·v) ∪ x′·z. The chain keeps
	// the parser's canonical ids and the leaf is rebuilt in content order of
	// those ids, which no parse string reproduces (the parser orders
	// operands under the source names and canonicalises afterwards), so the
	// chain is compared binder by binder and the leaf as a function.
	tref n = sides(ap::prepare_terms<node_t>(
		wff("fall y fex w (x & y & w & v | x' & z) = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(n));
	CHECK(ap::carries_functional_quantifier<node_t>(n));
	{
		tref got = ap::cofactor<node_t>(n, x, true, o);
		tref want = bf("fall y fex w (y & w & v)");
		auto var  = [](tref q) { return tau::trim_right_sibling(tau::get(q)[0].first()); };
		auto body = [](tref q) { return tau::trim_right_sibling(tau::get(q)[0].second()); };
		REQUIRE(tau::get(got).child_is(tau::bf_fall));
		CHECK(tau::subtree_equals(var(got), var(want)));
		tref got_in = body(got), want_in = body(want);
		REQUIRE(tau::get(got_in).child_is(tau::bf_fex));
		CHECK(tau::subtree_equals(var(got_in), var(want_in)));
		CHECK(same_function(body(got_in), body(want_in),
			{ var(got), var(got_in), v }));
	}
	CHECK(tau::subtree_equals(ap::cofactor<node_t>(n, x, false, o), bf("z")));
	// a quantifier whose body is P-free is a leaf as it stands
	tref m = sides(ap::prepare_terms<node_t>(wff("x & fall y (y | z) = 0"), P, o)).first;
	CHECK(tau::subtree_equals(ap::cofactor<node_t>(m, x, true, o), bf("fall y (y | z)")));
}

TEST_CASE("carries_functional_quantifier: plain, and in a BDD-backed term's leaves") {
	tref y = vr("y"), z = vr("z");
	tref f = bf("y & z");
	tref a = tb::build_functional_quantifiers({{y, tb::all}}, bf("y & z & w"));
	REQUIRE(tau::get(a).child_is(tau::bf_fall));
	CHECK(ap::carries_functional_quantifier<node_t>(a));
	CHECK(!ap::carries_functional_quantifier<node_t>(f));
	// BDD-backed: the chains sit in the leaves, not in the tree
	ap::block P{ z };
	order_t o = order_of(P);
	tref l = sides(ap::prepare_terms<node_t>(build_bf_eq_0<node_t>(a), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(ap::carries_functional_quantifier<node_t>(l));
}

TEST_CASE("resolve_functional_quantifiers_plain: plain, nested, hazard, keep") {
	tref y = vr("y"), z = vr("z"), w = vr("w"), u = vr("u");
	// ∀y (y ∪ z) = z
	tref c = tb::build_functional_quantifiers({{y, tb::all}}, bf("y | z"));
	REQUIRE(tau::get(c).child_is(tau::bf_fall));
	tref r = ap::resolve_functional_quantifiers_plain<node_t>(c);
	CHECK(!ap::carries_functional_quantifier<node_t>(r));
	CHECK(same_function(r, bf("z"), { z }));
	// nested, mixed kinds: ∀y ∃w (y·w·z ∪ y′·u) = z·u
	tref inner = tb::build_functional_quantifiers({{w, tb::ex}},
		bf("y & w & z | y' & u"));
	tref outer = tb::build_functional_quantifiers({{y, tb::all}}, inner);
	REQUIRE(tau::get(outer).child_is(tau::bf_fall));
	tref r2 = ap::resolve_functional_quantifiers_plain<node_t>(outer);
	CHECK(!ap::carries_functional_quantifier<node_t>(r2));
	CHECK(same_function(r2, bf("z & u"), { z, u }));
	// the LEAF HAZARD: `y` hides inside a reference argument, so the
	// quantification cannot reach it and the chain stays as it is
	tref hz = tb::build_functional_quantifiers({{y, tb::all}}, bf("y & r(y)"));
	REQUIRE(tau::get(hz).child_is(tau::bf_fall));
	CHECK(ap::resolve_functional_quantifiers_plain<node_t>(hz) == hz);
	// `keep` accepting everything leaves every chain standing
	auto keep_all = [](tref) { return true; };
	CHECK(ap::resolve_functional_quantifiers_plain<node_t>(c, keep_all) == c);
	CHECK(ap::resolve_functional_quantifiers_plain<node_t>(outer, keep_all) == outer);
	// the nest is ONE question, on the chain NODE, whose prefix is outermost
	// first (§3): accepting the ∀ it starts with keeps the ∃ under it too
	auto keep_all_kind = [](tref ch) {
		return prefix_of(ch)[0].second == tb::all; };
	CHECK(ap::resolve_functional_quantifiers_plain<node_t>(outer, keep_all_kind)
		== outer);
	// and declining that prefix resolves the nest whole, in one pass
	auto keep_ex = [](tref ch) { return prefix_of(ch)[0].second == tb::ex; };
	tref r3 = ap::resolve_functional_quantifiers_plain<node_t>(outer, keep_ex);
	CHECK(!ap::carries_functional_quantifier<node_t>(r3));
	CHECK(same_function(r3, bf("z & u"), { z, u }));
}

TEST_CASE("resolve_functional_quantifiers: a chain inside a BDD-backed term's leaf") {
	tref x = vr("x"), y = vr("y"), z = vr("z"), w = vr("w");
	ap::block P{ x };
	order_t o = order_of(P);
	// x·(∀y (y ∪ z)) ∪ x′·w. The ENTRY pass finds the chain wherever it
	// sits, and resolving it gives x·z ∪ x′·w.
	tref chain = tb::build_functional_quantifiers({{y, tb::all}}, bf("y | z"));
	tref t = tau::build_bf_or(tau::build_bf_and(bf("x"), chain),
		tau::build_bf_and(bf("x'"), bf("w")));
	tref plain = ap::resolve_functional_quantifiers_plain<node_t>(t);
	CHECK(!ap::carries_functional_quantifier<node_t>(plain));
	CHECK(same_function(plain, bf("x & z | x' & w"), { x, z, w }));
	// PREPARED over {x}, the chain is a LEAF, since its body does not touch
	// P — and the close never enters a leaf, so the term comes back as the
	// same node.
	tref l = sides(ap::prepare_terms<node_t>(build_bf_eq_0<node_t>(t), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	REQUIRE(ap::carries_functional_quantifier<node_t>(l));
	CHECK(ap::resolve_functional_quantifiers_bdd<node_t>(l, o) == l);
}

TEST_CASE("resolve_functional_quantifiers_bdd: a whole-block chain over a stored BDD") {
	tref x = vr("x"), y = vr("y"), a = vr("a"), b = vr("b");
	order_t live {{ x, 1 }, { y, 2 }};
	tref node = th::convert_to_tau_node(bf("x & y & a | x' & b"), live);
	REQUIRE(th::is_bdd_backed(node));
	tb::ref s = th::convert_to_handle(node).get();
	const size_t ty = find_ba_type<node_t>(node);
	// one kind over ALL the decision variables: one quantification on the
	// stored ref under the LIVE order, emitted like every other term
	for (auto kind : { tb::all, tb::ex }) {
		tref c = tb::build_functional_quantifiers(
			{{ x, kind }, { y, kind }}, node);
		REQUIRE(ap::carries_functional_quantifier<node_t>(c));
		trefs v{ x, y };
		tb::ref want = kind == tb::all ? tb::bdd_all(s, v, live)
			: tb::bdd_ex(s, v, live);
		CHECK(ap::resolve_functional_quantifiers_bdd<node_t>(c, live)
			== th::convert_to_tau_node_or_term(th(want), ty));
	}
	// MIXED kinds whose nesting follows the live ranks (the inner subscript
	// ranks lower): still one quantification, under the live order
	tb::quants qin {{ y, tb::all }, { x, tb::ex }};
	tref cin = tb::build_functional_quantifiers(qin, node);
	tref rin = ap::resolve_functional_quantifiers_bdd<node_t>(cin, live);
	CHECK(rin == th::convert_to_tau_node_or_term(
		th(tb::bdd_quant(s, qin, live)), ty));
	CHECK(same_function(rin, bf("b"), { a, b }));   // ∀y ∃x = b
	// The other nesting is not a shape the close meets: a mixed prefix
	// there is an entry chain's, which nests by construction (§3).
}

TEST_CASE("resolve_functional_quantifiers_bdd: a whole-block emission over P") {
	tref x = vr("x"), y = vr("y"), a = vr("a"), b = vr("b");
	ap::block P{ x, y };
	order_t o = order_of(P);
	// `a ∪ x·y·b` over P = {x, y}: the leaves are `a` and `a ∪ b`, so
	// ∀x∀y is their meet and ∃x∃y their join. The chain binds all of P,
	// which is keep mode's whole-block emission (§7 DISCHARGE).
	tref t = bf("a | x & y & b");
	tref l = sides(ap::prepare_terms<node_t>(build_bf_eq_0<node_t>(t), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	for (auto kind : { tb::all, tb::ex }) {
		tref c = tb::build_functional_quantifiers(
			{{ x, kind }, { y, kind }}, l);
		REQUIRE(ap::carries_functional_quantifier<node_t>(c));
		tref r = ap::resolve_functional_quantifiers_bdd<node_t>(c, o);
		CHECK(!ap::carries_functional_quantifier<node_t>(r));
		// nothing branches once all of P is quantified, so the result is
		// emitted PLAIN
		CHECK(!th::is_bdd_backed(r));
		CHECK(same_function(r, kind == tb::all ? bf("a") : bf("a | b"),
			{ a, b }));
	}
}

TEST_CASE("resolve_functional_quantifiers_bdd: a subscript hiding in a leaf keeps its binder") {
	tref x = vr("x"), y = vr("y");
	ap::block P{ x, y };
	order_t o = order_of(P);
	// `y ∪ r(x)`: `x` occurs inside a reference argument alone, which
	// prepare_terms puts in a LEAF, so no quantification reaches it. ∀y is
	// taken — ∀y (y ∪ r(x)) = r(x) — and ∀x stays over the result.
	tref l = sides(ap::prepare_terms<node_t>(
		build_bf_eq_0<node_t>(bf("y | r(x)")), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	tref c = tb::build_functional_quantifiers(
		{{ x, tb::all }, { y, tb::all }}, l);
	tref r = ap::resolve_functional_quantifiers_bdd<node_t>(c, o);
	CHECK(tau::subtree_equals(r, tb::build_functional_quantifiers(
		{{ x, tb::all }}, bf("r(x)"))));
}

TEST_CASE("resolve_functional_quantifiers_bdd: a chain over a plain body stays") {
	tref x = vr("x"), y = vr("y");
	ap::block P{ x };
	order_t o = order_of(P);
	// A chain whose body is PLAIN under the live order is one whose BDD did
	// not branch: every subscript of it sits in a leaf, and it stays.
	tref c = tb::build_functional_quantifiers({{ y, tb::all }}, bf("y & r(y)"));
	REQUIRE(ap::carries_functional_quantifier<node_t>(c));
	CHECK(ap::resolve_functional_quantifiers_bdd<node_t>(c, o) == c);
}

TEST_CASE("resolve_functional_quantifiers_plain: a mixed nest is ONE unit of work") {
	tref x = vr("x"), y = vr("y"), z = vr("z"), w = vr("w");
	tref f = bf("x & z | y & w");
	tb::quants q {{ x, tb::ex }, { y, tb::all }};
	tref c = tb::build_functional_quantifiers(q, f);
	REQUIRE(tau::get(c).child_is(tau::bf_fex));
	std::vector<tb::quants> seen;
	auto rec = [&seen](tref n) { seen.push_back(prefix_of(n)); return false; };
	tref r = ap::resolve_functional_quantifiers_plain<node_t>(c, rec);
	// asked ONCE, on the canonical node of the whole nest, prefix outermost
	// first
	REQUIRE(seen.size() == 1);
	REQUIRE(seen[0].size() == 2);
	CHECK(tau::subtree_equals(seen[0][0].first, x));
	CHECK(seen[0][0].second == tb::ex);
	CHECK(tau::subtree_equals(seen[0][1].first, y));
	CHECK(seen[0][1].second == tb::all);
	// and resolved in ONE pass over the chain's own order, the innermost
	// subscript ranking lowest
	order_t o {{ y, 1 }, { x, 2 }};
	CHECK(r == tb::to_tau_term(tb::bdd_quant(tb::build_bdd(f, o), q, o),
		find_ba_type<node_t>(f)));
	CHECK(same_function(r, bf("z"), { z, w }));   // ∃x ∀y (x·z ∪ y·w) = z
}

TEST_CASE("resolve_functional_quantifiers_plain: a same-kind nest is one merged prefix") {
	tref x = vr("x"), y = vr("y");
	tref f = bf("x & z | y & w");
	tref c = tb::build_functional_quantifiers(
		{{ x, tb::all }, { y, tb::all }}, f);
	std::vector<tb::quants> seen;
	auto rec = [&seen](tref n) { seen.push_back(prefix_of(n)); return true; };
	tref r = ap::resolve_functional_quantifiers_plain<node_t>(c, rec);
	// one run, one question, both subscripts in it (content order inside a
	// run, so the pair is not asserted positionally)
	REQUIRE(seen.size() == 1);
	REQUIRE(seen[0].size() == 2);
	CHECK(seen[0][0].second == tb::all);
	CHECK(seen[0][1].second == tb::all);
	CHECK((tau::subtree_equals(seen[0][0].first, x)
		|| tau::subtree_equals(seen[0][1].first, x)));
	CHECK((tau::subtree_equals(seen[0][0].first, y)
		|| tau::subtree_equals(seen[0][1].first, y)));
	// kept: the whole canonical chain comes back, its inner binder untouched
	CHECK(r == c);
	REQUIRE(tau::get(r).child_is(tau::bf_fall));
	tref inner = tau::trim_right_sibling(tau::get(tau::get(r).first()).second());
	CHECK(tau::get(inner).child_is(tau::bf_fall));
}

TEST_CASE("resolve_functional_quantifiers_plain: a chain inside the body goes first") {
	tref x = vr("x"), y = vr("y"), z = vr("z"), w = vr("w"), v = vr("v");
	// ∀x ∃v ( x·z·v ∪ ∃y (y·w) ) — the ∃y is NOT adjacent: it sits under ∪,
	// so it is a chain of its own, resolved before the nest above it
	tref in = tb::build_functional_quantifiers({{ y, tb::ex }}, bf("y & w"));
	tref body = tau::build_bf_or(bf("x & z & v"), in);
	tref c = tb::build_functional_quantifiers(
		{{ x, tb::all }, { v, tb::ex }}, body);
	std::vector<tb::quants> seen;
	auto rec = [&seen](tref n) { seen.push_back(prefix_of(n)); return false; };
	tref r = ap::resolve_functional_quantifiers_plain<node_t>(c, rec);
	REQUIRE(seen.size() == 2);
	REQUIRE(seen[0].size() == 1);
	CHECK(tau::subtree_equals(seen[0][0].first, y));   // the nested one first
	REQUIRE(seen[1].size() == 2);                      // then the nest, whole
	CHECK(tau::subtree_equals(seen[1][0].first, x));
	CHECK(tau::subtree_equals(seen[1][1].first, v));
	CHECK(!ap::carries_functional_quantifier<node_t>(r));
	// ∃y (y·w) = w, and ∀x ∃v (x·z·v ∪ w) = w
	CHECK(same_function(r, bf("w"), { z, w }));
}

TEST_CASE("resolve_functional_quantifiers_plain: the node keep is handed carries the body") {
	tref x = vr("x"), v = vr("v"), y = vr("y"), z = vr("z"), w = vr("w");
	// ∀x ∃v ( x·z·v ∪ ∃y (y·w) ): the nested chain is resolved BEFORE the
	// nest above it is offered to `keep`, so the body hanging off the node
	// `keep` is handed is the RESOLVED one (§3) — that body, and not only
	// the prefix, is what a policy gets to look at.
	tref in = tb::build_functional_quantifiers({{ y, tb::ex }}, bf("y & w"));
	tref body = tau::build_bf_or(bf("x & z & v"), in);
	tref c = tb::build_functional_quantifiers(
		{{ x, tb::all }, { v, tb::ex }}, body);
	trefs bodies;
	auto rec = [&bodies](tref n) {
		bodies.push_back(ap::strip_chain<node_t>(n).second);
		return false;
	};
	ap::resolve_functional_quantifiers_plain<node_t>(c, rec);
	REQUIRE(bodies.size() == 2);
	// the nested chain's own body first, then the nest's — with `∃y (y·w)`
	// resolved to `w` inside it
	CHECK(tau::subtree_equals(bodies[0], bf("y & w")));
	CHECK(!ap::carries_functional_quantifier<node_t>(bodies[1]));
	CHECK(same_function(bodies[1], bf("x & z & v | w"), { x, z, v, w }));
}

TEST_CASE("resolve_functional_quantifiers_plain: the stuck rule around a subscript in a leaf") {
	tref x = vr("x"), y = vr("y");
	tref f = bf("x & r(y)");                     // y hides in the argument
	std::vector<tb::quants> seen;
	auto rec = [&seen](tref n) { seen.push_back(prefix_of(n)); return false; };
	// ∃x ∀y (x·r(y)): innermost first, the hidden `∀y` opens a stuck run of
	// its kind, and `∃x` is of the OTHER kind, so it blocks and the whole
	// chain stays — asked about once all the same.
	tref blocked = tb::build_functional_quantifiers(
		{{ x, tb::ex }, { y, tb::all }}, f);
	REQUIRE(ap::carries_functional_quantifier<node_t>(blocked));
	CHECK(tau::subtree_equals(
		ap::resolve_functional_quantifiers_plain<node_t>(blocked, rec),
		blocked));
	REQUIRE(seen.size() == 1);
	CHECK(seen[0].size() == 2);
	// ∀y ∃x (x·r(y)) = ∀y r(y): `∃x` sits ahead of the hidden subscript and
	// is quantified, `∀y` stays where it is
	tref c = tb::build_functional_quantifiers(
		{{ y, tb::all }, { x, tb::ex }}, f);
	seen.clear();
	tref r = ap::resolve_functional_quantifiers_plain<node_t>(c, rec);
	REQUIRE(seen.size() == 1);
	CHECK(seen[0].size() == 2);
	CHECK(ap::carries_functional_quantifier<node_t>(r));
	CHECK(tau::subtree_equals(r, tb::build_functional_quantifiers(
		{{ y, tb::all }}, bf("r(y)"))));
	const trefs& fv = get_free_vars<node_t>(r);
	CHECK(!std::binary_search(fv.begin(), fv.end(), x, tau::subtree_less));
}

TEST_CASE("resolve_functional_quantifiers_plain: an outer subscript commutes into the stuck run") {
	tref x = vr("x"), y = vr("y");
	// ∃x ∃y (x·y·r(y)): the hidden `y` opens a stuck ∃ run, and `∃x` is of
	// that kind, so it is quantified into it — ∃y (y·r(y)) is what stays.
	tref c = tb::build_functional_quantifiers(
		{{ x, tb::ex }, { y, tb::ex }}, bf("x & y & r(y)"));
	tref r = ap::resolve_functional_quantifiers_plain<node_t>(c);
	auto [q, body] = ap::strip_chain<node_t>(r);
	REQUIRE(q.size() == 1);
	CHECK(tau::subtree_equals(q[0].first, y));
	CHECK(q[0].second == tb::ex);
	CHECK(same_function(body, bf("y & r(y)"), { y }));
}

TEST_CASE("resolve_functional_quantifiers_plain: keep sees the canonical chain node of a degenerate nest") {
	tref x = vr("x"), y = vr("y");
	// `fall x fall y (x & z)`, spelled without the constructor: `y` binds
	// nothing, so the canonical prefix holds `x` alone
	tref raw = tau::build_bf_fall(y, bf("x & z"), false);
	raw = tau::build_bf_fall(x, raw, false);
	std::vector<tb::quants> seen;
	auto rec = [&seen](tref n) { seen.push_back(prefix_of(n)); return true; };
	tref r = ap::resolve_functional_quantifiers_plain<node_t>(raw, rec);
	REQUIRE(seen.size() == 1);
	REQUIRE(seen[0].size() == 1);
	CHECK(tau::subtree_equals(seen[0][0].first, x));
	CHECK(seen[0][0].second == tb::all);
	// kept, and the canonical chain is what comes back
	CHECK(r == tb::build_functional_quantifiers({{ x, tb::all }}, bf("x & z")));
}

// 5. simplify_term ------------------------------------------------------------

TEST_CASE("simplify_term: the four laws on a plain term") {
	CHECK(tau::get(ap::simplify_term<node_t>(bf("0 & x"))).equals_0());
	CHECK(tau::get(ap::simplify_term<node_t>(bf("1 | x"))).equals_1());
	CHECK(tau::get(ap::simplify_term<node_t>(bf("x & x'"))).equals_0());
	CHECK(tau::get(ap::simplify_term<node_t>(bf("x | x'"))).equals_1());
	// per-path: x·f(x) = x·f(1) (the factor order is the path sweep's
	// canonical one, not pinned here)
	CHECK(same_function(ap::simplify_term<node_t>(bf("x & (x' | y)")), bf("x & y"),
		{ vr("x"), vr("y") }));
	// absorption through the dual pass
	CHECK(tau::subtree_equals(ap::simplify_term<node_t>(bf("x | x & y")), bf("x")));
}

TEST_CASE("simplify_term: leaves simplified and merged in the BDD regime") {
	tref x = vr("x"), y = vr("y"), z = vr("z");
	ap::block P{ x };
	order_t o = order_of(P);
	// x·(y·(y′ ∪ z)) ∪ x′·(y·z): the high leaf simplifies to y·z (per
	// path), merges with the low leaf, the decision node folds, and the
	// result is the plain y·z
	tref l = sides(ap::prepare_terms<node_t>(wff("x & (y & (y' | z)) | x' & (y & z) = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	tref s = ap::simplify_term<node_t>(l, o);
	CHECK(!th::is_bdd_backed(s));
	CHECK(same_function(s, bf("y & z"), { y, z }));
	// a plain combination of BDD-backed subterms is re-established over P
	tref f1 = ap::cofactor<node_t>(l, x, true, o);   // plain, P-free
	tref g  = sides(ap::prepare_terms<node_t>(wff("x & z = 0"), P, o)).first;
	tref prod = ap::simplify_term<node_t>(build_bf_and<node_t>(g, f1), o);
	CHECK(th::is_bdd_backed(prod));
	CHECK(same_function(prod, bf("x & z & y"), { x, y, z }));
	tref comp = ap::simplify_term<node_t>(build_bf_neg<node_t>(g), o);
	CHECK(same_function(comp, bf("(x & z)'"), { x, z }));
	// a P-free term under a live order: the plain regime
	CHECK(tau::subtree_equals(ap::simplify_term<node_t>(bf("y & (y' | z)"), o), bf("y & z")));
}

TEST_CASE("simplify_term: the cofactor check reduces over a variable the term does not decide") {
	tref x = vr("x"), y = vr("y");
	// both cofactors over x are `y`, so `y` is the term
	CHECK(tau::subtree_equals(ap::simplify_term<node_t>(bf("x & y | x' & y")),
		bf("y")));
	CHECK(tau::subtree_equals(
		ap::simplify_term<node_t>(bf("(x | y) & (x' | y)")), bf("y")));
	// the cofactors differ, so nothing is reduced
	tref t = bf("x & y | x' & y'");
	CHECK(same_function(ap::simplify_term<node_t>(t), t, { x, y }));
}

TEST_CASE("simplify_term: the cofactor check stays out of an arithmetic position") {
	// the Boolean product folds away; the sum is no place for a Shannon
	// expansion, so the occurrence in it is neither read nor replaced
	tref s = ap::simplify_term<node_t>(
		bf("x:bv[8] & x:bv[8]' | (x:bv[8] + { 1 }:bv[8])"));
	CHECK(tau::subtree_equals(s, bf("x:bv[8] + { 1 }:bv[8]")));
	CHECK(get_free_vars<node_t>(s).size() == 1);
}

TEST_CASE("simplify_term: the cofactor check takes a reference as an opaque element") {
	// x is free inside the reference argument too, where the expansion is
	// taken around it
	CHECK(tau::subtree_equals(
		ap::simplify_term<node_t>(bf("x & r(x) | x' & r(x)")), bf("r(x)")));
}

TEST_CASE("simplify_term: the cofactor check reduces a leaf in the BDD regime") {
	tref x = vr("x"), w = vr("w"), z = vr("z");
	ap::block P{ x };
	order_t o = order_of(P);
	// x·(y·z ∪ y′·z) ∪ x′·w: the high leaf does not depend on y, and y is
	// no decision variable, so the check reduces it where it lies
	tref l = sides(ap::prepare_terms<node_t>(
		wff("x & (y & z | y' & z) | x' & w = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	tref s = ap::simplify_term<node_t>(l, o);
	REQUIRE(th::is_bdd_backed(s));
	tb::ref b = ref_of(s, o);
	REQUIRE(!tb::leaf(b));
	CHECK(tau::subtree_equals(tb::get_var(b), x));
	CHECK(tau::subtree_equals(tb::get_var_term(tb::get_high(b)), bf("z")));
	CHECK(tau::subtree_equals(tb::get_var_term(tb::get_low(b)), bf("w")));
	CHECK(same_function(s, bf("x & z | x' & w"), { x, w, z }));
}

TEST_CASE("simplify_term: the cofactor check reaches the leaves of a kept emission") {
	tref x = vr("x");
	ap::block P{ x };
	order_t o = order_of(P);
	// §7 DISCHARGE's keep-mode emission over a stored BDD whose one leaf,
	// `w·u ∪ w′·u`, decides nothing over w
	tref stored = th::convert_to_tau_node(bf("x & (w & u | w' & u)"), o);
	REQUIRE(th::is_bdd_backed(stored));
	tref chain = tb::build_functional_quantifiers({{x, tb::all}}, stored);
	REQUIRE(tau::get(chain).child_is(tau::bf_fall));
	tref got = ap::simplify_term<node_t>(chain, o);
	CHECK(got != chain);
	REQUIRE(tau::get(got).child_is(tau::bf_fall));
	// the prefix as it stands, over a body still backed by the live order
	CHECK(prefix_of(got) == prefix_of(chain));
	tref gbody = ap::strip_chain<node_t>(got).second;
	REQUIRE(th::is_bdd_backed(gbody));
	tb::ref gb = ref_of(gbody, o);
	CHECK(tb::is_ordered(gb, o));
	REQUIRE(!tb::leaf(gb));
	CHECK(tau::subtree_equals(tb::get_var(gb), x));
	CHECK(tau::subtree_equals(tb::get_var_term(tb::get_high(gb)), bf("u")));
	CHECK(tb::get_low(gb) == tb::F);
}

TEST_CASE("simplify_term: the check's result is emitted as it stands, so a second call may go further") {
	// the cofactors over y are equal and both hold `x·(x′ ∪ w)`, a
	// per-path contradiction the sweep of THIS call is already past
	tref once = ap::simplify_term<node_t>(bf("(y | x) & (y' | x) & (x' | w)"));
	CHECK(tau::get(once).find_top(is<node_t, tau::bf_neg>) != nullptr);
	// the next call sweeps what this one emitted
	tref twice = ap::simplify_term<node_t>(once);
	CHECK(twice != once);
	CHECK(tau::get(twice).find_top(is<node_t, tau::bf_neg>) == nullptr);
	CHECK(same_function(twice, bf("x & w"), { vr("x"), vr("w") }));
}

// 6. simplify_atom ------------------------------------------------------------

TEST_CASE("simplify_atom: the sides simplified, the atom decided") {
	CHECK(tau::get(ap::simplify_atom<node_t>(wff("x & x' = 0"))).equals_T());
	CHECK(tau::get(ap::simplify_atom<node_t>(wff("x | x' = 0"))).equals_F());
	CHECK(tau::get(ap::simplify_atom<node_t>(wff("!(x & x' = 0)"))).equals_F());
	CHECK(tau::get(ap::simplify_atom<node_t>(wff("x & y = x & y"))).equals_T());
	// sides that differ as written and simplify to one term: x·y = x·(y ∪ x′)
	CHECK(tau::get(ap::simplify_atom<node_t>(wff("x & y = x & (y | x')"))).equals_T());
	// the same sides under `≠` decide dually
	CHECK(tau::get(ap::simplify_atom<node_t>(wff("x & y != x & (y | x')"))).equals_F());
}

TEST_CASE("simplify_atom: an atom nothing decides comes back as written") {
	// (a Boolean order atom is not in the list: the hooks decompose it into
	// a compound formula at parse, which is not an atom — Debug asserts)
	for (const char* s : { "x = y", "x = y'", "x != y", "x' = 0",
		"!(x = y)", "x:bv[8] <= y:bv[8]" })
		CHECK(ap::simplify_atom<node_t>(wff(s)) == wff(s));
}

TEST_CASE("simplify_atom: BDD regime — the sides, and the hooks around them") {
	tref x = vr("x");
	ap::block P{ x };
	order_t o = order_of(P);
	// sides differ as written, are equal once the leaf simplifies: T
	tref a = ap::prepare_terms<node_t>(wff("x & (y & (y' | z)) = x & (y & z)"), P, o);
	REQUIRE(th::is_bdd_backed(sides(a).first));
	CHECK(tau::get(ap::simplify_atom<node_t>(a, o)).equals_T());
	// through one ¬
	CHECK(tau::get(ap::simplify_atom<node_t>(build_wff_neg<node_t>(a), o)).equals_F());
	// an atom that does not fold keeps its shape and its sides BDD-backed
	tref b = ap::prepare_terms<node_t>(wff("x & y = x & z"), P, o);
	tref sb = ap::simplify_atom<node_t>(b, o);
	CHECK(sb == b);
	CHECK(th::is_bdd_backed(sides(sb).first));
	CHECK(th::is_bdd_backed(sides(sb).second));
}

TEST_CASE("simplify_atom: the joint decides what no hook can compare") {
	tref x = vr("x");
	ap::block P{ x };
	order_t o = order_of(P);
	// x and x′ as PREPARED sides: two different `BDD_ID` nodes, neither
	// the negation of the other to look at, so the ring sum — the constant
	// 1 — is what decides the atom
	tref l = th::convert_to_tau_node(bf("x"), o);
	tref r = th::convert_to_tau_node(bf("x'"), o);
	REQUIRE(th::is_bdd_backed(l));
	REQUIRE(th::is_bdd_backed(r));
	REQUIRE(l != r);
	tref eq = tau::get(tau::wff, tau::get(tau::bf_eq, l, r));
	REQUIRE(is_atomic_fm<node_t>(eq));
	CHECK(tau::get(ap::simplify_atom<node_t>(eq, o)).equals_F());
	// and `≠` dually
	tref neq = tau::get(tau::wff, tau::get(tau::bf_neq, l, r));
	REQUIRE(is_atomic_fm<node_t>(neq));
	CHECK(tau::get(ap::simplify_atom<node_t>(neq, o)).equals_T());
}

TEST_CASE("simplify_term / simplify_atom: a keep-mode emission over a stored BDD (D11)") {
	tref x = vr("x"), y = vr("y");
	ap::block P{ x, y };
	order_t o = order_of(P);
	// §7 DISCHARGE in keep mode emits the whole block symbolically over
	// the stored BDD: `∀x ∀y (bf(BDD_ID))`, P-free and not backed itself.
	tref stored = th::convert_to_tau_node(bf("x & y | x' & z"), o);
	REQUIRE(th::is_bdd_backed(stored));
	tref chain = tb::build_functional_quantifiers(
		{{x, tb::all}, {y, tb::all}}, stored);
	REQUIRE(tau::get(chain).child_is(tau::bf_fall));
	REQUIRE(ap::strip_chain<node_t>(chain).second == stored);
	// the emission comes back as it stands, holding the SAME stored BDD
	CHECK(ap::simplify_term<node_t>(chain, o) == chain);
	// and so does the atom it rides in
	tref atom = build_bf_eq_0<node_t>(chain);
	REQUIRE(sides(atom).first == chain);
	tref s = ap::simplify_atom<node_t>(atom, o);
	CHECK(s == atom);
	CHECK(sides(s).first == chain);
	CHECK(ap::strip_chain<node_t>(sides(s).first).second == stored);
}

TEST_CASE("simplify_term: the leaves INSIDE a kept emission are simplified") {
	tref x = vr("x");
	ap::block P{ x };
	order_t o = order_of(P);
	// the stored BDD's one leaf is `y·(y′ ∪ z)`, which the path sweep
	// reduces to `y·z`; the chain around it is untouched
	tref stored = th::convert_to_tau_node(bf("x & (y & (y' | z))"), o);
	REQUIRE(th::is_bdd_backed(stored));
	tref chain = tb::build_functional_quantifiers({{x, tb::all}}, stored);
	REQUIRE(tau::get(chain).child_is(tau::bf_fall));
	tref got = ap::simplify_term<node_t>(chain, o);
	CHECK(got != chain);
	REQUIRE(tau::get(got).child_is(tau::bf_fall));
	CHECK(prefix_of(got) == prefix_of(chain));
	tref gbody = ap::strip_chain<node_t>(got).second;
	REQUIRE(th::is_bdd_backed(gbody));
	tb::ref gb = ref_of(gbody, o);
	REQUIRE(!tb::leaf(gb));
	CHECK(tau::subtree_equals(tb::get_var(gb), x));
	CHECK(tau::subtree_equals(tb::get_var_term(tb::get_high(gb)), bf("y & z")));
	CHECK(tb::get_low(gb) == tb::F);
}

TEST_CASE("simplify_term: the sweep simplifies AROUND a kept emission") {
	tref x = vr("x"), y = vr("y");
	ap::block P{ x, y };
	order_t o = order_of(P);
	tref stored = th::convert_to_tau_node(bf("x & y | x' & z"), o);
	tref chain = tb::build_functional_quantifiers(
		{{x, tb::all}, {y, tb::all}}, stored);
	// the emission is an opaque leaf to the sweep: it is its own key, so
	// a repeat of it folds, `·0` folds, and a negation stays outside it
	CHECK(ap::simplify_term<node_t>(
		build_bf_or<node_t>(chain, chain), o) == chain);
	CHECK(tau::get(ap::simplify_term<node_t>(build_bf_and<node_t>(chain,
		tau::_0(find_ba_type<node_t>(chain))), o)).equals_0());
	tref neg = ap::simplify_term<node_t>(build_bf_neg<node_t>(chain), o);
	CHECK(has_bdd_id(neg));
	CHECK(neg == build_bf_neg<node_t>(chain));
}

// 7. term_of, norm_equation ---------------------------------------------------

TEST_CASE("term_of reads l + r without touching the atom; norm_equation descends through ¬") {
	tref x = vr("x"), y = vr("y");
	ap::block P{ x };
	order_t o = order_of(P);
	tref plain = wff("x = y");
	tref t = ap::term_of<node_t>(plain, o);
	CHECK(tau::subtree_equals(t, bf("x ^ y")));
	CHECK(plain == wff("x = y")); // hash-consed: the atom node is unchanged
	CHECK(ap::term_of<node_t>(wff("!(x = y)"), o) == t);
	// a `≠`, which phase 1 meets before NORMALIZE_OPERATORS, has the same
	// ring sum, read through one ¬ as well
	CHECK(ap::term_of<node_t>(wff("x != y"), o) == t);
	CHECK(ap::term_of<node_t>(wff("!(x != y)"), o) == t);
	// BDD-backed sides: the ring sum at BDD level, same function
	tref prepared = ap::prepare_terms<node_t>(wff("x & y = x"), P, o);
	tref tb = ap::term_of<node_t>(prepared, o);
	CHECK(same_function(tb, bf("(x & y) ^ x"), { x, y }));
	CHECK(th::is_bdd_backed(tb));
	// one BDD-backed side and a constant: the side itself
	tref eq0 = ap::prepare_terms<node_t>(wff("x & y = 0"), P, o);
	CHECK(ap::term_of<node_t>(eq0, o) == sides(eq0).first);
	// norm_equation: ours descends through ¬, the old one does not — pinned
	tref neg = wff("!(x = y)");
	tref ours = ap::norm_equation<node_t>(neg, o);
	tref old  = tau_lang::norm_equation<node_t>(neg);
	CHECK(old == neg);
	CHECK(ours != neg);
	CHECK(tau::get(ours).child_is(tau::wff_neg));
	CHECK(tau::subtree_equals(ours, build_wff_neg<node_t>(build_bf_eq_0<node_t>(bf("x ^ y")))));
	CHECK(tau::subtree_equals(ap::norm_equation<node_t>(plain, o), build_bf_eq_0<node_t>(bf("x ^ y"))));
	// a non-equation (a bitvector order atom — a Boolean one is decomposed
	// into an equation by the hooks) is returned as it is
	tref lt = wff("x:bv[8] <= y:bv[8]");
	REQUIRE(tau::get(lt).child_is(tau::bf_lteq));
	CHECK(ap::norm_equation<node_t>(lt, o) == lt);
#ifndef DEBUG
	// Debug asserts instead
	CHECK(ap::term_of<node_t>(lt, o) == nullptr);
#endif
}

// 8. mem_size ----------------------------------------------------------------

TEST_CASE("mem_size: shared BDD nodes counted once, terminals zero, leaves one") {
	tref x = vr("x"), y = vr("y");
	ap::block P{ x, y };
	order_t o = order_of(P);
	// x·y ∪ x′·y′: the two y-nodes are one node with an inverter — 2 nodes
	tref l = sides(ap::prepare_terms<node_t>(wff("x & y | x' & y' = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(ap::mem_size<node_t>(l) == 2);
	// x·y ∪ x′·z with x on top: x, y and the leaf z — 3
	ap::block Q{ y, x };
	order_t oq = order_of(Q);
	tref m = sides(ap::prepare_terms<node_t>(wff("x & y | x' & z = 0"), Q, oq)).first;
	CHECK(ap::mem_size<node_t>(m) == 3);
	// a plain term: operators and leaves once, no wrappers, a variable's
	// own nodes not counted, a reference opaque
	CHECK(ap::mem_size<node_t>(bf("x")) == 1);
	CHECK(ap::mem_size<node_t>(bf("x & y")) == 3);
	CHECK(ap::mem_size<node_t>(bf("x & (y & z)")) == 5);
	CHECK(ap::mem_size<node_t>(bf("x'")) == 2);
	CHECK(ap::mem_size<node_t>(bf("x & r(y & z)")) == 3);
}

// 9. D2 round trip ------------------------------------------------------------

TEST_CASE("convert_to_tau_terms: no BDD_ID remains, and prepare ∘ finish is the identity node (D2)") {
	tref x = vr("x"), y = vr("y"), z = vr("z");
	(void) z;
	ap::block P{ x, y };
	order_t o = order_of(P);
	tref body = wff("(x & y | x' & z = 0) && (y & z = x)");
	tref prepared = ap::prepare_terms<node_t>(body, P, o);
	REQUIRE(has_bdd_id(prepared));
	tref finished = th::convert_to_tau_terms(prepared);
	CHECK(!has_bdd_id(finished));
	// the round trip: the same BDD, hence the same interned node
	tref again = ap::prepare_terms<node_t>(finished, P, o);
	CHECK(again == prepared);
	// and a second round trip is stable too
	CHECK(th::convert_to_tau_terms(again) == finished);
	// a prepared atom REWRAPped under a binder is finished through it
	tref under = build_wff_ex<node_t>(z, prepared, false);
	CHECK(!has_bdd_id(th::convert_to_tau_terms(under)));
	// nothing to finish: the same tref
	CHECK(th::convert_to_tau_terms(body) == body);
	// interning: the same BDD built twice is one node
	tref l1 = sides(ap::prepare_terms<node_t>(wff("x & y = 0"), P, o)).first;
	tref l2 = sides(ap::prepare_terms<node_t>(wff("y & x = 0"), P, o)).first;
	CHECK(l1 == l2);
	CHECK(th::convert_to_tau_node(th::U.find(th::key_of(l1))->second, find_ba_type<node_t>(l1)) == l1);
	CHECK(th::convert_to_handle(l1) == th::U.find(th::key_of(l1))->second);
}

// 10. leaf hazard ------------------------------------------------------------

TEST_CASE("leaf_fv: the leaves' contribution alone") {
	tref x = vr("x"), y = vr("y"), z = vr("z");
	ap::block P{ x, y };
	order_t o = order_of(P);
	tref l = sides(ap::prepare_terms<node_t>(wff("x & y = 0"), P, o)).first;
	CHECK(ap::leaf_fv<node_t>(l).empty());
	tref m = sides(ap::prepare_terms<node_t>(wff("x & y & z = 0"), P, o)).first;
	trefs mfv = ap::leaf_fv<node_t>(m);
	CHECK(mfv.size() == 1);
	CHECK(tau::subtree_equals(mfv[0], z));
	// the §1 example: a block variable inside a reference argument
	tref h = sides(ap::prepare_terms<node_t>(wff("x & r(y) = 0"), P, o)).first;
	trefs hfv = ap::leaf_fv<node_t>(h);
	CHECK(std::binary_search(hfv.begin(), hfv.end(), y, tau::subtree_less));
	CHECK(!std::binary_search(hfv.begin(), hfv.end(), x, tau::subtree_less));
	CHECK(std::is_sorted(hfv.begin(), hfv.end(), tau::subtree_less));
	// fv of the whole term holds both
	const trefs& fv = get_free_vars<node_t>(h);
	CHECK(std::binary_search(fv.begin(), fv.end(), x, tau::subtree_less));
	// a plain term: its FV
	CHECK(ap::leaf_fv<node_t>(bf("y & z")).size() == 2);
}

} // TEST_SUITE
