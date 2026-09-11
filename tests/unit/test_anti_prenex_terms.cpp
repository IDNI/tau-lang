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
	tref all = ap::quantify_over<node_t>(ap::binder::all, l, P, o);
	tref ex  = ap::quantify_over<node_t>(ap::binder::ex,  l, P, o);
	CHECK(!th::is_bdd_backed(all));
	CHECK(!th::is_bdd_backed(ex));
	CHECK(same_function(all, bf("a & b"), { a, b }));
	CHECK(same_function(ex,  bf("a | b"), { a, b }));
	// over an empty block, or on a plain term: identity
	CHECK(ap::quantify_over<node_t>(ap::binder::all, l, {}, o) == l);
	CHECK(ap::quantify_over<node_t>(ap::binder::ex, bf("a"), P, o) == bf("a"));
	// over PART of the decision set: the library's quantification, and the
	// result stays BDD-backed on the rest
	tref y = vr("y");
	ap::block Q{ x, y };
	order_t oq = order_of(Q);
	tref g = sides(ap::prepare_terms<node_t>(wff("x & y & a | x' & b = 0"), Q, oq)).first;
	tref exx = ap::quantify_over<node_t>(ap::binder::ex, g, { x }, oq);
	CHECK(th::is_bdd_backed(exx));
	trefs vx{ x };
	CHECK(ref_of(exx, oq) == tb::bdd_ex(ref_of(g, oq), vx, oq));
	CHECK(same_function(exx, bf("y & a | b"), { y, a, b }));
}

// 4. the slide ----------------------------------------------------------------

TEST_CASE("prepare_terms slides a functional quantifier onto the leaves, innermost first") {
	tref x = vr("x");
	ap::block P{ x };
	order_t o = order_of(P);
	// ∀_y (x·y ∪ x′·z) = x·∀_y y ∪ x′·∀_y z = x·(fall y y) ∪ x′·z
	tref l = sides(ap::prepare_terms<node_t>(wff("fall y (x & y | x' & z) = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	tref hi = ap::cofactor<node_t>(l, x, true, o);
	tref lo = ap::cofactor<node_t>(l, x, false, o);
	CHECK(tau::subtree_equals(hi, bf("fall y y")));
	CHECK(tau::subtree_equals(lo, bf("z")));
	CHECK(ap::carries_functional_quantifier<node_t>(l));
	// nested: ∀_y ∃_w (x·y·w ∪ x′·z) → x·∀_y ∃_w (y·w) ∪ x′·z. The chain
	// keeps the parser's canonical ids and the leaf is rebuilt in content
	// order of those ids, which no parse string reproduces (the parser
	// orders operands under the source names and canonicalises afterwards),
	// so the chain is compared binder by binder and the leaf as a function.
	tref n = sides(ap::prepare_terms<node_t>(wff("fall y fex w (x & y & w | x' & z) = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(n));
	{
		tref got = ap::cofactor<node_t>(n, x, true, o);
		tref want = bf("fall y fex w (y & w)");
		auto var  = [](tref q) { return tau::trim_right_sibling(tau::get(q)[0].first()); };
		auto body = [](tref q) { return tau::trim_right_sibling(tau::get(q)[0].second()); };
		REQUIRE(tau::get(got).child_is(tau::bf_fall));
		CHECK(tau::subtree_equals(var(got), var(want)));
		tref got_in = body(got), want_in = body(want);
		REQUIRE(tau::get(got_in).child_is(tau::bf_fex));
		CHECK(tau::subtree_equals(var(got_in), var(want_in)));
		CHECK(same_function(body(got_in), body(want_in), { var(got), var(got_in) }));
	}
	CHECK(tau::subtree_equals(ap::cofactor<node_t>(n, x, false, o), bf("z")));
	// a quantifier whose body is P-free is a leaf as it stands
	tref m = sides(ap::prepare_terms<node_t>(wff("x & fall y (y | z) = 0"), P, o)).first;
	CHECK(tau::subtree_equals(ap::cofactor<node_t>(m, x, true, o), bf("fall y (y | z)")));
}

TEST_CASE("functional_quantifier: one node per permutation, absent variables dropped") {
	tref y = vr("y"), z = vr("z"), w = vr("w");
	tref f = bf("y & z");
	tref a = ap::functional_quantifier<node_t>(ap::binder::all, { y, z }, f);
	tref b = ap::functional_quantifier<node_t>(ap::binder::all, { z, y }, f);
	CHECK(a == b);
	CHECK(tau::get(a).child_is(tau::bf_fall));
	// a nested single-variable chain, as the parser builds it. The module
	// keeps the source names (it never renames, ground rule 4) while the
	// parser canonicalises functional subscripts, so the built chain is
	// canonicalised before the comparison.
	tref ca = canonize_quantifier_ids<node_t>(a);
	CHECK((tau::subtree_equals(ca, bf("fall y fall z (y & z)"))
		|| tau::subtree_equals(ca, bf("fall z fall y (y & z)"))));
	CHECK((tau::subtree_equals(ca, bf("fall y, z (y & z)"))
		|| tau::subtree_equals(ca, bf("fall z, y (y & z)"))));
	// w ∉ FV(f): dropped
	CHECK(ap::functional_quantifier<node_t>(ap::binder::ex, { w }, f) == f);
	CHECK(ap::functional_quantifier<node_t>(ap::binder::ex, { w, y }, f)
		== ap::functional_quantifier<node_t>(ap::binder::ex, { y }, f));
	// a constant is returned as it is
	CHECK(ap::functional_quantifier<node_t>(ap::binder::all, { y }, bf("1")) == bf("1"));
	// Y excluded from FV
	const trefs& fv = get_free_vars<node_t>(a);
	CHECK(fv.empty());
	CHECK(ap::carries_functional_quantifier<node_t>(a));
	CHECK(!ap::carries_functional_quantifier<node_t>(f));
}

// 5. subst_term ---------------------------------------------------------------

TEST_CASE("subst_term: reaches a reference argument and re-simplifies it") {
	tref x = vr("x"), z = vr("z");
	ap::block P{ x };
	order_t o = order_of(P);
	tref t = bf("x & r(x & z)");
	tref l = sides(ap::prepare_terms<node_t>(build_bf_eq_0<node_t>(t), P, o)).first;
	REQUIRE(th::is_bdd_backed(l));
	// the leaf hazard: x is hidden in the reference argument
	trefs lfv = ap::leaf_fv<node_t>(l);
	CHECK(std::binary_search(lfv.begin(), lfv.end(), x, tau::subtree_less));
	// x ← z′: the decision node composes, the argument becomes r(z′·z),
	// which the re-simplification folds to r(0)
	tref s = ap::subst_term<node_t>(l, x, bf("z'"), o);
	const trefs& fv = get_free_vars<node_t>(s);
	CHECK(!std::binary_search(fv.begin(), fv.end(), x, tau::subtree_less));
	CHECK(std::binary_search(fv.begin(), fv.end(), z, tau::subtree_less));
	CHECK(!th::is_bdd_backed(s)); // no decision variable is left
	CHECK(tau::get(s).find_top([](tref m) {
		return tree<node_t>::get(m).is(tau::bf_ref); }) != nullptr);
	// the argument folded to 0
	tref arg = tau::get(s).find_top([](tref m) {
		return tree<node_t>::get(m).is(tau::ref_arg); });
	REQUIRE(arg != nullptr);
	CHECK(tau::get(arg)[0].equals_0());
	// x ∉ FV(f): the same tref
	CHECK(ap::subst_term<node_t>(bf("y & z"), x, bf("z"), o) == bf("y & z"));
	// a plain term: ordinary replace
	CHECK(tau::subtree_equals(ap::subst_term<node_t>(bf("x & y"), x, bf("z"), {}),
		bf("z & y")));
}

TEST_CASE("subst_term: a leaf that gains a block variable is re-canonicalised") {
	tref p = vr("p"), a = vr("a"), b = vr("b"), c = vr("c");
	ap::block P{ p };
	order_t o = order_of(P);
	// p·a ∪ p′·b with the OUTER variable a in a leaf; a ← p·c puts the
	// decision variable into that leaf, which the rebuild must lift:
	// p·(p·c) ∪ p′·b = p·c ∪ p′·b
	tref f = sides(ap::prepare_terms<node_t>(wff("p & a | p' & b = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(f));
	tref s = ap::subst_term<node_t>(f, a, bf("p & c"), o);
	CHECK(th::is_bdd_backed(s));
	CHECK(same_function(s, bf("p & c | p' & b"), { p, b, c }));
	CHECK(ap::leaf_fv<node_t>(s).size() == 2); // b and c, p is a decision node again
}

TEST_CASE("subst_term: a BDD-backed t composes on the decision variable and enters no leaf as BDD_ID") {
	tref x = vr("x"), y = vr("y"), z = vr("z");
	ap::block P{ x, y };
	order_t o = order_of(P);
	tref f = sides(ap::prepare_terms<node_t>(wff("x & r(x) = 0"), P, o)).first;
	tref t = sides(ap::prepare_terms<node_t>(wff("y & z = 0"), P, o)).first;
	REQUIRE(th::is_bdd_backed(f));
	REQUIRE(th::is_bdd_backed(t));
	tref s = ap::subst_term<node_t>(f, x, t, o);
	CHECK(same_function(s, bf("y & z & r(y & z)"), { x, y, z }));
	// finish: nothing BDD-backed remains anywhere
	CHECK(!has_bdd_id(th::convert_to_tau_terms(s)));
}

// 6. simplify_term ------------------------------------------------------------

TEST_CASE("simplify_term: the four laws on a plain term") {
	CHECK(tau::get(ap::simplify_term<node_t>(bf("0 & x"))).equals_0());
	CHECK(tau::get(ap::simplify_term<node_t>(bf("1 | x"))).equals_1());
	CHECK(tau::get(ap::simplify_term<node_t>(bf("x & x'"))).equals_0());
	CHECK(tau::get(ap::simplify_term<node_t>(bf("x | x'"))).equals_1());
	// per-path: x·f(x) = x·f(1)
	CHECK(tau::subtree_equals(ap::simplify_term<node_t>(bf("x & (x' | y)")), bf("x & y")));
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

// 7. simplify_atom ------------------------------------------------------------

TEST_CASE("simplify_atom: plain regime folds and is idempotent") {
	CHECK(tau::get(ap::simplify_atom<node_t>(wff("x & x' = 0"))).equals_T());
	CHECK(tau::get(ap::simplify_atom<node_t>(wff("x | x' = 0"))).equals_F());
	CHECK(tau::get(ap::simplify_atom<node_t>(wff("!(x & x' = 0)"))).equals_F());
	CHECK(tau::get(ap::simplify_atom<node_t>(wff("x & y = x & y"))).equals_T());
	// the joint l + r catches what side-wise cannot: x·y = x·(y ∪ x′)
	CHECK(tau::get(ap::simplify_atom<node_t>(wff("x & y = x & (y | x')"))).equals_T());
	// (a Boolean order atom is not in the list: the hooks decompose it into
	// a compound formula at parse, which is not an atom — Debug asserts)
	for (const char* s : { "x = 1", "x = y", "x & y = x", "x' = 0", "x = y'",
		"x | y = 1", "x & y = 0", "!(x = y)",
		"x:bv[8] <= y:bv[8]", "x:bv[8] & y:bv[8] < x:bv[8]",
		"{ 1 }:bv[8] <= { 2 }:bv[8]" })
	{
		tref once  = ap::simplify_atom<node_t>(wff(s));
		tref twice = ap::simplify_atom<node_t>(once);
		CHECK(once == twice);
	}
}

TEST_CASE("simplify_atom: BDD regime is side-wise and folds through the hooks") {
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
}

// 8. term_of, norm_equation ---------------------------------------------------

TEST_CASE("term_of reads l + r without touching the atom; norm_equation descends through ¬") {
	tref x = vr("x"), y = vr("y");
	ap::block P{ x };
	order_t o = order_of(P);
	tref plain = wff("x = y");
	tref t = ap::term_of<node_t>(plain, o);
	CHECK(tau::subtree_equals(t, bf("x ^ y")));
	CHECK(plain == wff("x = y")); // hash-consed: the atom node is unchanged
	CHECK(ap::term_of<node_t>(wff("!(x = y)"), o) == t);
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

// 9. mem_size ----------------------------------------------------------------

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

// 10. D2 round trip ------------------------------------------------------------

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

// 11. leaf hazard ------------------------------------------------------------

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
