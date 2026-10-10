// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/foundations/terms.h.
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
/// The trimmed `variable` node of a one-variable term — the shape
/// blocks and free-variable sets hold.
tref vr(const char* s) { return tau::trim(bf(s)); }

/// A `tau`-typed term variable. Purity is asked against a BA type, so a
/// fixture that carries one of its own spells it; a parsed fixture carries
/// none and is of whatever type it is read under.
tref tbf(const char* name) {
	return tau::build_bf_variable(name, tau_type_id<node_t>());
}

/// A real order atom `l ≤ r`. On a non-bitvector type the construction hooks
/// rewrite `≤` into the equation `l·r′ = 0`, so one does not survive
/// construction; the parser builds with the hooks disabled, and this helper
/// reproduces that shape the same way.
tref order_atom(tref l, tref r) {
	use_hooks_guard<node_t> g(false);
	return tau::build_bf_lteq(l, r);
}

/// The BDD ref of a term (a BDD-backed term's own ref).
tb::ref ref_of(tref t) {
	if (th::is_bdd_backed(t))
		return th::U.find(th::key_of(t))->second.get();
	return tb::build_bdd(t);
}

/// Semantic equality of two terms: equal ROBDDs (leaves opaque), both
/// finished first.
bool same_function(tref a, tref b) {
	return tb::build_bdd(th::convert_to_tau_terms(a))
		== tb::build_bdd(th::convert_to_tau_terms(b));
}

/// §7 DISCHARGE's keep-mode emission of @p chain: the chain BUILT, which
/// slides it to its subscripts' levels, emitted like every other term.
tref emitted(tref chain) {
	return th::convert_to_tau_node_or_term(th(tb::build_bdd(chain)),
		find_ba_type<node_t>(chain));
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

/// A formula parsed with inference, which types a binder's variable as it
/// types the variable's occurrences.
tref typed(const char* sample) {
	auto rr = get_nso_rr(sample);
	REQUIRE(rr.has_value());
	return rr.value().main->get();
}

} // namespace

TEST_SUITE("anti_prenex/foundations/terms") {

// 1. prepare_terms ------------------------------------------------------------

TEST_CASE("prepare_terms: every variable is a decision variable, in the global order") {
	tref x = vr("x"), y = vr("y"), z = vr("z");
	tref body = wff("x & y & z = 0");
	tref prepared = ap::prepare_terms<node_t>(body);
	auto [l, r] = sides(prepared);
	CHECK(th::is_bdd_backed(l));
	CHECK(!th::is_bdd_backed(r)); // the constant side stays a constant
	tb::ref f = ref_of(l);
	// free variables by name: x on top, then y, then z
	REQUIRE(!tb::leaf(f));
	CHECK(tau::subtree_equals(tb::get_var(f), x));
	REQUIRE(!tb::leaf(tb::get_high(f)));
	CHECK(tau::subtree_equals(tb::get_var(tb::get_high(f)), y));
	REQUIRE(!tb::leaf(tb::get_high(tb::get_high(f))));
	CHECK(tau::subtree_equals(tb::get_var(tb::get_high(tb::get_high(f))), z));
	// no leaf holds a variable
	CHECK(ap::leaf_fv<node_t>(l).empty());
	// the same function as the input
	CHECK(same_function(l, bf("x & y & z")));
	// a reference is a leaf, the variable in its argument hidden there
	tref m = sides(ap::prepare_terms<node_t>(wff("x & r(z) = 0"))).first;
	REQUIRE(th::is_bdd_backed(m));
	trefs lfv = ap::leaf_fv<node_t>(m);
	CHECK(std::binary_search(lfv.begin(), lfv.end(), z, tau::subtree_less));
	CHECK(!std::binary_search(lfv.begin(), lfv.end(), x, tau::subtree_less));
}

TEST_CASE("prepare_terms: every side holding a variable is backed, whatever block it meets") {
	tref both = wff("x = 0 && y & z = 0");
	tref prepared = ap::prepare_terms<node_t>(both);
	CHECK(prepared != both);
	const trefs ms = ap::members<node_t>(prepared);
	REQUIRE(ms.size() == 2);
	for (tref m : ms) CHECK(th::is_bdd_backed(sides(m).first));
	// a side whose BDD does not branch -- the variable sits in a reference
	// alone -- stays plain
	CHECK(!has_bdd_id(ap::prepare_terms<node_t>(wff("r(x) = 0"))));
}

TEST_CASE("prepare_terms: an order atom stays plain, a binder is entered") {
	// a Boolean `<=` is decomposed into equations by the construction
	// hooks, so the only order atoms that survive are bitvector ones
	CHECK(!tau::get(wff("x & y <= x")).child_is(tau::bf_lteq));
	tref lt = wff("x:bv[8] & y:bv[8] <= x:bv[8]");
	REQUIRE(tau::get(lt).child_is(tau::bf_lteq));
	CHECK(ap::prepare_terms<node_t>(lt) == lt);
	// while a bitvector EQUATION is prepared
	tref eqb = wff("x:bv[8] & y:bv[8] = { 0 }:bv[8]");
	CHECK(ap::prepare_terms<node_t>(eqb) != eqb);
	// the conversion runs once over the whole formula, so the body of a
	// binder is converted in place, the binder kept
	tref unit = wff("ex z (x & z = 0)");
	tref pu = ap::prepare_terms<node_t>(unit);
	REQUIRE(pu != unit);
	REQUIRE(is_child_quantifier<node_t>(pu));
	CHECK(th::is_bdd_backed(sides(ap::binder_body<node_t>(pu)).first));
	CHECK(same_function(sides(ap::binder_body<node_t>(pu)).first,
		sides(ap::binder_body<node_t>(unit)).first));
	// a negated equation is entered
	tref neg = wff("!(x & y = 0)");
	tref prepared = ap::prepare_terms<node_t>(neg);
	CHECK(prepared != neg);
	CHECK(tau::get(prepared).child_is(tau::wff_neg));
}

TEST_CASE("prepare_terms: equal sides fold through the hooks on BDD_ID operands (D4)") {
	// (x & y) | (x & y') == x: the two sides are one BDD, one node, and
	// X = X ::= T fires on the interned operands.
	tref body = wff("(x & y) | (x & y') = x");
	tref prepared = ap::prepare_terms<node_t>(body);
	CHECK(tau::get(prepared).equals_T());
	// BDD_ID operands through the term hooks: X & X ::= X
	tref l = sides(ap::prepare_terms<node_t>(wff("x & y = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(build_bf_and<node_t>(l, l) == l);
	CHECK(tau::get(build_bf_eq<node_t>(l, l)).equals_T());
}

// 1b. the spelling (FINISH_TERMS) ----------------------------------------------

TEST_CASE("spelling: the round trip of prepare_terms gives an equivalent plain formula") {
	const tref phi = ap::canonical_and<node_t>(trefs{ wff("x & y = 0"),
		wff("!(x | z = 0)") });
	const tref prepared = ap::prepare_terms<node_t>(phi);
	REQUIRE(has_bdd_id(prepared));
	const tref back = th::convert_to_tau_terms(prepared);
	CHECK(!has_bdd_id(back));
	CHECK(are_nso_equivalent<node_t>(back, phi));
	// the spelling builds back into the BDD it spelled, so a second round
	// trip is the identity node
	CHECK(ap::prepare_terms<node_t>(back) == prepared);
}

TEST_CASE("spelling: complement cofactors are spelled as the ring sum") {
	// `x + y`: the cofactors on `x` are `y′` and `y`, complements, so the
	// node is spelled `x + y`, not `x·y′ ∪ x′·y`.
	tref l = sides(ap::prepare_terms<node_t>(wff("x ^ y = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	tref s = th::convert_to_tau_terms(l);
	CHECK(tau::get(s).child_is(tau::bf_xor));
	CHECK(tb::build_bdd(s) == ref_of(l));
	// cofactors that are no complements keep the Shannon spelling
	tref m = sides(ap::prepare_terms<node_t>(wff("x & y | x' & z = 0"))).first;
	REQUIRE(th::is_bdd_backed(m));
	tref t = th::convert_to_tau_terms(m);
	CHECK(tau::get(t).find_top([](tref n) {
		return tree<node_t>::get(n).is(tau::bf_xor); }) == nullptr);
	CHECK(tb::build_bdd(t) == ref_of(m));
}

TEST_CASE("spelling: a cofactor 1 is spelled as a join with the literal") {
	// `x ∪ y`: the cofactor at `x` is 1, so the node is `x ∪ f₀` with
	// `f₀ = y`; in `x′ ∪ y` the cofactor at `x′` is 1, so it is `x′ ∪ f₁`.
	tref l = sides(ap::prepare_terms<node_t>(wff("x | y = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(tau::subtree_equals(th::convert_to_tau_terms(l), bf("x | y")));
	tref m = sides(ap::prepare_terms<node_t>(wff("x' | y = 0"))).first;
	REQUIRE(th::is_bdd_backed(m));
	CHECK(tau::subtree_equals(th::convert_to_tau_terms(m), bf("x' | y")));
}

TEST_CASE("spelling: a node shared under both polarities is spelled once") {
	// The parity of eight variables is eight nodes, each reached under both
	// polarities. Spelled as ring sums, every node is spelled once and the
	// term is one chain: eight variables and seven sums.
	tref l = sides(ap::prepare_terms<node_t>(wff(
		"a ^ b ^ c ^ d ^ e ^ f ^ g ^ h = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(tb::node_count(ref_of(l)) == 8);
	tref s = th::convert_to_tau_terms(l);
	CHECK(ap::mem_size<node_t>(s) == 15);
	CHECK(tb::build_bdd(s) == ref_of(l));
}

TEST_CASE("spelling: a kept chain's body is spelled inside its leaf") {
	tref x = vr("x");
	// A chain over a stored BDD: the spelling enters it and the chain
	// stands, over the spelling of the BDD it held.
	tref l = sides(ap::prepare_terms<node_t>(wff("x & y = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	const tref atm = build_bf_eq_0<node_t>(
		tb::build_functional_quantifiers({{ x, tb::all }}, l));
	REQUIRE(has_bdd_id(atm));
	const tref got = th::convert_to_tau_terms(atm);
	CHECK(!has_bdd_id(got));
	auto [prefix, body] = ap::strip_chain<node_t>(sides(got).first);
	REQUIRE(prefix.size() == 1);
	CHECK(tb::build_bdd(body) == ref_of(l));
}

TEST_CASE("spelling: a formula with no stored BDD is the same node") {
	const tref phi = wff("x & y = 0");
	REQUIRE(!has_bdd_id(phi));
	CHECK(th::convert_to_tau_terms(phi) == phi);
	const tref psi = wff("ex z (x & z = 0) && !(y = 0)");
	REQUIRE(!has_bdd_id(psi));
	CHECK(th::convert_to_tau_terms(psi) == psi);
}

// 2. cofactor -----------------------------------------------------------------

TEST_CASE("cofactor: the child when x is on top, a rebuild at any depth") {
	tref x = vr("x"), y = vr("y"), w = vr("w");
	tref l = sides(ap::prepare_terms<node_t>(wff("x & y | x' & z = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	tb::ref f = ref_of(l);
	// free variables by name: x on top
	CHECK(tau::subtree_equals(tb::get_var(f), x));
	// top variable: child selection
	tref f1 = ap::cofactor<node_t>(l, x, true);
	tref f0 = ap::cofactor<node_t>(l, x, false);
	CHECK(ref_of(f1) == tb::get_high(f));
	CHECK(ref_of(f0) == tb::get_low(f));
	// a deeper variable: the nodes above it are rebuilt
	tref g1 = ap::cofactor<node_t>(l, y, true);
	tref g0 = ap::cofactor<node_t>(l, y, false);
	CHECK(same_function(g1, bf("x | x' & z")));
	CHECK(same_function(g0, bf("x' & z")));
	// identity on a variable the term does not hold
	CHECK(ap::cofactor<node_t>(l, w, true) == l);
	// identity on a plain term
	CHECK(ap::cofactor<node_t>(bf("z"), x, true) == bf("z"));
}

// 3. quantification -----------------------------------------------------------

TEST_CASE("quantify_over over all the decision variables: meet / join of the leaves") {
	tref x = vr("x");
	ap::block P{ x };
	tref l = sides(ap::prepare_terms<node_t>(
		wff("x & r(a) | x' & r(b) = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	tref all = ap::quantify_over<node_t>(tb::all, l, P);
	tref ex  = ap::quantify_over<node_t>(tb::ex,  l, P);
	CHECK(!th::is_bdd_backed(all));
	CHECK(!th::is_bdd_backed(ex));
	CHECK(same_function(all, bf("r(a) & r(b)")));
	CHECK(same_function(ex,  bf("r(a) | r(b)")));
	// over an empty block, or on a plain term: identity
	CHECK(ap::quantify_over<node_t>(tb::all, l, {}) == l);
	CHECK(ap::quantify_over<node_t>(tb::ex, bf("a"), P) == bf("a"));
	// over PART of the decision set: the library's quantification, and the
	// result stays BDD-backed on the rest
	tref g = sides(ap::prepare_terms<node_t>(wff("x & y & a | x' & b = 0"))).first;
	tref exx = ap::quantify_over<node_t>(tb::ex, g, { x });
	CHECK(th::is_bdd_backed(exx));
	trefs vx{ x };
	CHECK(ref_of(exx) == tb::bdd_ex(ref_of(g), vx));
	CHECK(same_function(exx, bf("y & a | b")));
}

// 4. a chain in a BDD, and RESOLVE_FUNCTIONAL ----------------------------------

TEST_CASE("prepare_terms: the library slides a functional quantifier to its subscripts' levels") {
	tref x = vr("x");
	// ∀_y (x·y ∪ x′·z) = x·∀_y y ∪ x′·∀_y z = x′·z: the parser gives `y` a
	// canonical id, which sorts below the free `x` and `z`, and the closed
	// piece `y` folds
	tref l = sides(ap::prepare_terms<node_t>(
		wff("fall y (x & y | x' & z) = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(ref_of(l) == tb::build_bdd(bf("x' & z")));
	CHECK(!ap::carries_functional_quantifier<node_t>(l));
	// a piece holding a free variable in a leaf stays a chain:
	// ∀_y ∃_w (x·y·w·r(v) ∪ x′·z) → x·∀_y ∃_w (y·w·r(v)) ∪ x′·z. The chain
	// keeps the parser's canonical ids, which no parse string reproduces,
	// so it is compared binder by binder and its body as a function.
	tref n = sides(ap::prepare_terms<node_t>(
		wff("fall y fex w (x & y & w & r(v) | x' & z) = 0"))).first;
	REQUIRE(th::is_bdd_backed(n));
	CHECK(ap::carries_functional_quantifier<node_t>(n));
	{
		tref got = ap::cofactor<node_t>(n, x, true);
		tref want = bf("fall y fex w (y & w & r(v))");
		auto var  = [](tref q) { return tau::trim_right_sibling(tau::get(q)[0].first()); };
		auto body = [](tref q) { return tau::trim_right_sibling(tau::get(q)[0].second()); };
		REQUIRE(tau::get(got).child_is(tau::bf_fall));
		CHECK(tau::subtree_equals(var(got), var(want)));
		tref got_in = body(got), want_in = body(want);
		REQUIRE(tau::get(got_in).child_is(tau::bf_fex));
		CHECK(tau::subtree_equals(var(got_in), var(want_in)));
		CHECK(same_function(body(got_in), body(want_in)));
	}
	CHECK(same_function(ap::cofactor<node_t>(n, x, false), bf("z")));
	// beside a decision variable a chain slides too: x·∀_y (y ∪ z) = x·z
	tref m = sides(ap::prepare_terms<node_t>(
		wff("x & fall y (y | z) = 0"))).first;
	REQUIRE(th::is_bdd_backed(m));
	CHECK(ref_of(m) == tb::build_bdd(bf("x & z")));
}

TEST_CASE("carries_functional_quantifier: plain, and in a BDD-backed term's leaves") {
	tref y = vr("y");
	tref f = bf("y & z");
	tref a = tb::build_functional_quantifiers({{y, tb::all}}, bf("y & z & w"));
	REQUIRE(tau::get(a).child_is(tau::bf_fall));
	CHECK(ap::carries_functional_quantifier<node_t>(a));
	CHECK(!ap::carries_functional_quantifier<node_t>(f));
	// BDD-backed: the chain sits in a leaf, not in the tree
	tref l = sides(ap::prepare_terms<node_t>(
		build_bf_eq_0<node_t>(build_bf_and<node_t>(bf("x"), a)))).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(ap::carries_functional_quantifier<node_t>(l));
}

TEST_CASE("resolve_functional_quantifiers_plain: plain, nested, hazard, keep") {
	tref y = vr("y"), w = vr("w");
	// ∀y (y ∪ z) = z
	tref c = tb::build_functional_quantifiers({{y, tb::all}}, bf("y | z"));
	REQUIRE(tau::get(c).child_is(tau::bf_fall));
	tref r = ap::resolve_functional_quantifiers_plain<node_t>(c);
	CHECK(!ap::carries_functional_quantifier<node_t>(r));
	CHECK(same_function(r, bf("z")));
	// nested, mixed kinds: ∀y ∃w (y·w·z ∪ y′·u) = z·u
	tref inner = tb::build_functional_quantifiers({{w, tb::ex}},
		bf("y & w & z | y' & u"));
	tref outer = tb::build_functional_quantifiers({{y, tb::all}}, inner);
	REQUIRE(tau::get(outer).child_is(tau::bf_fall));
	tref r2 = ap::resolve_functional_quantifiers_plain<node_t>(outer);
	CHECK(!ap::carries_functional_quantifier<node_t>(r2));
	CHECK(same_function(r2, bf("z & u")));
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
	CHECK(same_function(r3, bf("z & u")));
}

TEST_CASE("resolve_functional_quantifiers_plain: a chain anywhere inside a term") {
	tref y = vr("y");
	// x·(∀y (y ∪ z)) ∪ x′·w. The ENTRY pass finds the chain wherever it
	// sits, and resolving it gives x·z ∪ x′·w.
	tref chain = tb::build_functional_quantifiers({{y, tb::all}}, bf("y | z"));
	tref t = tau::build_bf_or(tau::build_bf_and(bf("x"), chain),
		tau::build_bf_and(bf("x'"), bf("w")));
	tref plain = ap::resolve_functional_quantifiers_plain<node_t>(t);
	CHECK(!ap::carries_functional_quantifier<node_t>(plain));
	CHECK(same_function(plain, bf("x & z | x' & w")));
}

TEST_CASE("resolve_functional_quantifiers_plain: a mixed nest is ONE unit of work") {
	tref x = vr("x"), y = vr("y");
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
	// and resolved under the global order, one run at a time, innermost
	// first
	CHECK(r == tb::to_tau_term(tb::bdd_quant(tb::build_bdd(f), q),
		find_ba_type<node_t>(f)));
	CHECK(same_function(r, bf("z")));   // ∃x ∀y (x·z ∪ y·w) = z
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
	tref x = vr("x"), y = vr("y"), v = vr("v");
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
	CHECK(same_function(r, bf("w")));
}

TEST_CASE("resolve_functional_quantifiers_plain: the node keep is handed carries the body") {
	tref x = vr("x"), v = vr("v"), y = vr("y");
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
	CHECK(same_function(bodies[1], bf("x & z & v | w")));
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
	CHECK(same_function(body, bf("y & r(y)")));
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
	CHECK(same_function(ap::simplify_term<node_t>(bf("x & (x' | y)")), bf("x & y")));
	// absorption through the dual pass
	CHECK(tau::subtree_equals(ap::simplify_term<node_t>(bf("x | x & y")), bf("x")));
}

TEST_CASE("simplify_term: the backed regime is the identity") {
	tref x = vr("x");
	// x·ℓ₁ ∪ x′·ℓ₂ with ℓ₁ = r(y)·(r(y)′ ∪ r(z)) and ℓ₂ = r(y)·r(z): two
	// leaves the leaf product keeps apart, and SIMPLIFY_TERM leaves them so
	tref l = sides(ap::prepare_terms<node_t>(wff(
		"x & (r(y) & (r(y)' | r(z))) | x' & (r(y) & r(z)) = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(ap::simplify_term<node_t>(l, ap::regime::backed) == l);
	// a cofactor, a product and a complement are the library's operations,
	// and their terms come back as they are
	tref f1 = ap::cofactor<node_t>(l, x, true);   // plain: one leaf
	REQUIRE(!th::is_bdd_backed(f1));
	CHECK(ap::simplify_term<node_t>(f1, ap::regime::backed) == f1);
	tref g  = sides(ap::prepare_terms<node_t>(wff("x & z = 0"))).first;
	auto term = [&g](tb::ref r) {
		return th::convert_to_tau_node_or_term(th(r),
			find_ba_type<node_t>(g));
	};
	tref prod = term(tb::bdd_and(ref_of(g), ref_of(f1)));
	REQUIRE(th::is_bdd_backed(prod));
	CHECK(ap::simplify_term<node_t>(prod, ap::regime::backed) == prod);
	tref comp = term(tb::bdd_not(ref_of(g)));
	CHECK(ap::simplify_term<node_t>(comp, ap::regime::backed) == comp);
}

TEST_CASE("simplify_term: the cofactor check reduces over a variable the term does not decide") {
	// both cofactors over x are `y`, so `y` is the term
	CHECK(tau::subtree_equals(ap::simplify_term<node_t>(bf("x & y | x' & y")),
		bf("y")));
	CHECK(tau::subtree_equals(
		ap::simplify_term<node_t>(bf("(x | y) & (x' | y)")), bf("y")));
	// the cofactors differ, so nothing is reduced
	tref t = bf("x & y | x' & y'");
	CHECK(same_function(ap::simplify_term<node_t>(t), t));
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

TEST_CASE("simplify_term: in the backed regime the BDD decides every variable") {
	tref y = vr("y");
	// x·(y·z ∪ y′·z) ∪ x′·w: the term does not depend on y, which as a
	// decision variable the reduced BDD drops at its construction
	tref l = sides(ap::prepare_terms<node_t>(
		wff("x & (y & z | y' & z) | x' & w = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	tref s = ap::simplify_term<node_t>(l, ap::regime::backed);
	REQUIRE(th::is_bdd_backed(s));
	CHECK(s == l);
	const trefs& fv = get_free_vars<node_t>(s);
	CHECK(!std::binary_search(fv.begin(), fv.end(), y, tau::subtree_less));
	CHECK(same_function(s, bf("x & z | x' & w")));
}

TEST_CASE("simplify_term: a kept emission's BDD decides its variables already") {
	tref x = vr("x"), w = vr("w");
	// §7 DISCHARGE's keep-mode emission of ∀x over the stored BDD of
	// `a·(w·x·r(u) ∪ w′·x·r(u)) ∪ a′·b`, which decides nothing over w: the
	// BDD has no w node, and the emission, slid to x's levels — `a` and `b`
	// above, the piece `x·r(u)` one chain leaf — comes back as it stands
	tref stored = th::convert_to_tau_node(
		bf("a & (w & x & r(u) | w' & x & r(u)) | a' & b"));
	REQUIRE(th::is_bdd_backed(stored));
	tref e = emitted(tb::build_functional_quantifiers({{x, tb::all}}, stored));
	REQUIRE(th::is_bdd_backed(e));
	const trefs& fv = get_free_vars<node_t>(e);
	CHECK(!std::binary_search(fv.begin(), fv.end(), w, tau::subtree_less));
	CHECK(!std::binary_search(fv.begin(), fv.end(), x, tau::subtree_less));
	CHECK(ap::carries_functional_quantifier<node_t>(e));
	CHECK(ap::simplify_term<node_t>(e, ap::regime::backed) == e);
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
	CHECK(same_function(twice, bf("x & w")));
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

TEST_CASE("simplify_atom: backed regime — the sides, and the hooks around them") {
	// sides differ as written; their sum is x·(ℓ₁ + ℓ₂), one compound leaf
	// under x, which the open node test sweeps to 0: T
	tref a = ap::prepare_terms<node_t>(
		wff("x & (r(y) & (r(y)' | r(z))) = x & (r(y) & r(z))"));
	REQUIRE(th::is_bdd_backed(sides(a).first));
	CHECK(tau::get(ap::simplify_atom<node_t>(a, ap::regime::backed)).equals_T());
	// through one ¬
	CHECK(tau::get(ap::simplify_atom<node_t>(build_wff_neg<node_t>(a), ap::regime::backed)).equals_F());
	// an atom that does not fold keeps its shape and its sides BDD-backed
	tref b = ap::prepare_terms<node_t>(wff("x & y = x & z"));
	tref sb = ap::simplify_atom<node_t>(b, ap::regime::backed);
	CHECK(sb == b);
	CHECK(th::is_bdd_backed(sides(sb).first));
	CHECK(th::is_bdd_backed(sides(sb).second));
}

TEST_CASE("simplify_atom: the joint decides what no hook can compare") {
	// x and x′ as PREPARED sides: two different `BDD_ID` nodes, neither
	// the negation of the other to look at, so the ring sum — the constant
	// 1 — is what decides the atom
	tref l = th::convert_to_tau_node(bf("x"));
	tref r = th::convert_to_tau_node(bf("x'"));
	REQUIRE(th::is_bdd_backed(l));
	REQUIRE(th::is_bdd_backed(r));
	REQUIRE(l != r);
	tref eq = tau::get(tau::wff, tau::get(tau::bf_eq, l, r));
	REQUIRE(is_atomic_fm<node_t>(eq));
	CHECK(tau::get(ap::simplify_atom<node_t>(eq, ap::regime::backed)).equals_F());
	// and `≠` dually
	tref neq = tau::get(tau::wff, tau::get(tau::bf_neq, l, r));
	REQUIRE(is_atomic_fm<node_t>(neq));
	CHECK(tau::get(ap::simplify_atom<node_t>(neq, ap::regime::backed)).equals_T());
}

TEST_CASE("simplify_term / simplify_atom: a keep-mode emission is the slid chain (D11)") {
	tref y = vr("y"), b = vr("b");
	// §7 DISCHARGE in keep mode emits ∀y over the stored BDD SLID to y's
	// levels: `b` stays a decision variable, and the piece under it,
	// `y·r(a) ∪ y′·r(c)`, holding `a` and `c` free, is ONE chain leaf
	tref stored = th::convert_to_tau_node(bf("b & (y & r(a) | y' & r(c))"));
	REQUIRE(th::is_bdd_backed(stored));
	tref chain = tb::build_functional_quantifiers({{y, tb::all}}, stored);
	REQUIRE(tau::get(chain).child_is(tau::bf_fall));
	tref e = emitted(chain);
	REQUIRE(th::is_bdd_backed(e));
	tb::ref eb = ref_of(e);
	CHECK(tau::subtree_equals(tb::get_var(eb), b));
	REQUIRE(tb::leaf(tb::get_high(eb)));
	CHECK(tau::get(tb::get_var_term(tb::get_high(eb))).child_is(tau::bf_fall));
	CHECK(tb::get_low(eb) == tb::F);
	// the emission comes back as it stands
	CHECK(ap::simplify_term<node_t>(e, ap::regime::backed) == e);
	// and so does the atom it rides in
	tref atom = build_bf_eq_0<node_t>(e);
	REQUIRE(sides(atom).first == e);
	CHECK(ap::simplify_atom<node_t>(atom, ap::regime::backed) == atom);
}

TEST_CASE("simplify_term: around a kept emission the hooks fold and the rest stands") {
	tref y = vr("y");
	tref stored = th::convert_to_tau_node(bf("b & (y & r(a) | y' & r(c))"));
	tref e = emitted(tb::build_functional_quantifiers({{y, tb::all}}, stored));
	REQUIRE(th::is_bdd_backed(e));
	// a repeat of it and `·0` fold through the construction hooks, and its
	// complement, the library's, is the complemented BDD, the chain leaf
	// untouched
	CHECK(ap::simplify_term<node_t>(
		build_bf_or<node_t>(e, e), ap::regime::backed) == e);
	CHECK(tau::get(ap::simplify_term<node_t>(build_bf_and<node_t>(e,
		tau::_0(find_ba_type<node_t>(e))), ap::regime::backed)).equals_0());
	tref neg = ap::simplify_term<node_t>(th::convert_to_tau_node_or_term(
		th(tb::bdd_not(ref_of(e))), find_ba_type<node_t>(e)),
		ap::regime::backed);
	REQUIRE(th::is_bdd_backed(neg));
	CHECK(ref_of(neg) == tb::bdd_not(ref_of(e)));
}

// 6b. node_test --------------------------------------------------------------

TEST_CASE("node_test: an open test sweeps the compound leaves, and its rebuild is carried") {
	// x·ℓ₁ ∪ x′·ℓ₂ as above: both leaves are products, so "zero" is OPEN.
	// The sweep reduces ℓ₁ to r(y)·r(z) by its literal r(y), the leaves
	// merge, the decision node folds, and the rebuild is the plain ℓ₂: not
	// zero, and handed back
	tref l = sides(ap::prepare_terms<node_t>(wff(
		"x & (r(y) & (r(y)' | r(z))) | x' & (r(y) & r(z)) = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	const ap::node_answer a = ap::node_test<node_t>(l,
		ap::node_question::zero);
	CHECK(a.cls == 0);
	CHECK(!th::is_bdd_backed(a.term));
	CHECK(same_function(a.term, bf("r(y) & r(z)")));
	// SIMPLIFY_ATOM on `l = 0` runs that test and CARRIES its rebuild
	tref atom = build_bf_eq_0<node_t>(l);
	REQUIRE(sides(atom).first == l);
	tref s = ap::simplify_atom<node_t>(atom, ap::regime::backed);
	CHECK(tau::subtree_equals(sides(s).first, a.term));
	CHECK(tau::get(sides(s).second).equals_0());
	// a test whose leaves sweep to nothing new hands its term back
	tref plain = sides(ap::prepare_terms<node_t>(wff(
		"x & (r(y) & r(z)) | x' & r(w) & r(z) = 0"))).first;
	REQUIRE(th::is_bdd_backed(plain));
	const ap::node_answer b = ap::node_test<node_t>(plain,
		ap::node_question::zero);
	CHECK(b.term == plain);
	CHECK(b.cls == 0);
}

TEST_CASE("node_test: an atomic leaf answers no at once") {
	tref x = vr("x");
	// x·ℓ₁ ∪ x′·r(z): ℓ₁ would sweep, but r(z) is a single reference no
	// sweep changes, so every question is answered at once, nothing swept
	tref l = sides(ap::prepare_terms<node_t>(wff(
		"x & (r(y) & (r(y)' | r(z))) | x' & r(z) = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	for (unsigned char q : { ap::node_question::zero,
		ap::node_question::one, ap::node_question::nonzero })
	{
		const ap::node_answer a = ap::node_test<node_t>(l, q);
		CHECK(a.term == l);
		CHECK(a.cls == 0);
	}
	// a kept emission: ∀x over x·ℓ₁ slides into ONE chain leaf, atomic,
	// never entered — ℓ₁ inside it stays as written
	tref stored = th::convert_to_tau_node(
		bf("x & (r(y) & (r(y)' | r(z)))"));
	REQUIRE(th::is_bdd_backed(stored));
	tref e = emitted(tb::build_functional_quantifiers({{x, tb::all}}, stored));
	REQUIRE(tau::get(e).child_is(tau::bf_fall));
	CHECK(ap::simplify_term<node_t>(e, ap::regime::backed) == e);
	const ap::node_answer k = ap::node_test<node_t>(e,
		ap::node_question::zero);
	CHECK(k.term == e);
	CHECK(k.cls == 0);
	// so does a wrong terminal: x·ℓ₁ asked "one" meets 0 and sweeps
	// nothing, asked "zero" it opens and hands back x·r(y)·r(z)
	tref xl = sides(ap::prepare_terms<node_t>(wff(
		"x & (r(y) & (r(y)' | r(z))) = 0"))).first;
	REQUIRE(th::is_bdd_backed(xl));
	CHECK(ap::node_test<node_t>(xl, ap::node_question::one).term == xl);
	const ap::node_answer z = ap::node_test<node_t>(xl,
		ap::node_question::zero);
	CHECK(z.term != xl);
	CHECK(z.cls == 0);
	CHECK(same_function(z.term, bf("x & r(y) & r(z)")));
}

TEST_CASE("node_test: the classes of a terminal and a constant") {
	const size_t type = find_ba_type<node_t>(bf("x"));
	const ap::node_answer z = ap::node_test<node_t>(tau::_0(type),
		ap::node_question::one);
	CHECK(z.is(ap::node_question::zero));
	CHECK(!z.is(ap::node_question::nonzero));
	const ap::node_answer o = ap::node_test<node_t>(tau::_1(type),
		ap::node_question::zero);
	CHECK(o.is(ap::node_question::one));
	CHECK(o.is(ap::node_question::nonzero));
	CHECK(!o.is(ap::node_question::zero));
	// a BA constant other than 0 and 1 is a nonzero constant, not `1`
	const ap::node_answer c = ap::node_test<node_t>(bf("{ 5 }:bv[8]"),
		ap::node_question::zero);
	CHECK(c.is(ap::node_question::constant));
	CHECK(c.is(ap::node_question::nonzero));
	CHECK(!c.is(ap::node_question::one));
}

TEST_CASE("node_test: a product the leaf product cannot see is open, and its sweep decides T") {
	tref x = vr("x");
	const ap::block X{ x };
	// ∃x. x·(r ∪ s) ∪ x′·r′·s′ = 0 discharges to (r ∪ s)·r′·s′ = 0
	tref f = sides(ap::prepare_terms<node_t>(wff(
		"x & (r(a) | s(a)) | x' & r(a)' & s(a)' = 0"))).first;
	REQUIRE(th::is_bdd_backed(f));
	tref q = ap::quantify_over<node_t>(tb::all, f, X);
	CHECK(tau::get(ap::simplify_atom<node_t>(build_bf_eq_0<node_t>(q),
		ap::regime::backed)).equals_T());
	// here r ∪ s is built as the complement of the product r′·s′, so the
	// library meets the two as one node and its complement: 0 at once
	CHECK(tau::get(q).equals_0());
	// with a third member, (r ∪ s)·r′·s′·t(a) is a product the leaf
	// product cannot see: the test "zero" is open, and the sweep folds it
	tref g = sides(ap::prepare_terms<node_t>(wff(
		"x & (r(a) | s(a)) | x' & r(a)' & s(a)' & t(a) = 0"))).first;
	REQUIRE(th::is_bdd_backed(g));
	tref p = ap::quantify_over<node_t>(tb::all, g, X);
	REQUIRE(!th::is_bdd_backed(p));
	REQUIRE(!tau::get(p).equals_0());
	const ap::node_answer a = ap::node_test<node_t>(p,
		ap::node_question::zero);
	CHECK(a.is(ap::node_question::zero));
	CHECK(tau::get(a.term).equals_0());
	tref atom = build_bf_eq_0<node_t>(p);
	REQUIRE(!tau::get(atom).equals_T());
	CHECK(tau::get(ap::simplify_atom<node_t>(atom, ap::regime::backed))
		.equals_T());
	// the same product as a DERIVED sum decides the atom and leaves its
	// sides as they stand
	tref ne = tau::get(tau::wff, tau::get(tau::bf_eq, p,
		tau::_0(find_ba_type<node_t>(p))));
	CHECK(tau::get(ap::simplify_atom<node_t>(build_wff_neg<node_t>(ne),
		ap::regime::backed)).equals_F());
}

TEST_CASE("node_test: the canonical sum decides, and its sweep is dropped") {
	// x·ℓ₁ = x·r(w): the sum x·(ℓ₁ + r(w)) opens, its sweep decides
	// nothing, and the atom comes back as written — ℓ₁ not re-spelled
	tref b = ap::prepare_terms<node_t>(wff(
		"x & (r(y) & (r(y)' | r(z))) = x & r(w)"));
	REQUIRE(th::is_bdd_backed(sides(b).first));
	CHECK(ap::simplify_atom<node_t>(b, ap::regime::backed) == b);
}

// 7. term_of, norm_equation ---------------------------------------------------

TEST_CASE("term_of reads l + r without touching the atom; norm_equation descends through ¬") {
	tref plain = wff("x = y");
	tref t = ap::term_of<node_t>(plain);
	CHECK(tau::subtree_equals(t, bf("x ^ y")));
	CHECK(plain == wff("x = y")); // hash-consed: the atom node is unchanged
	CHECK(ap::term_of<node_t>(wff("!(x = y)")) == t);
	// a `≠`, which phase 1 meets before NORMALIZE_OPERATORS, has the same
	// ring sum, read through one ¬ as well
	CHECK(ap::term_of<node_t>(wff("x != y")) == t);
	CHECK(ap::term_of<node_t>(wff("!(x != y)")) == t);
	// BDD-backed sides: the ring sum at BDD level, same function
	tref prepared = ap::prepare_terms<node_t>(wff("x & y = x"));
	tref tb = ap::term_of<node_t>(prepared);
	CHECK(same_function(tb, bf("(x & y) ^ x")));
	CHECK(th::is_bdd_backed(tb));
	// one BDD-backed side and a constant: the side itself
	tref eq0 = ap::prepare_terms<node_t>(wff("x & y = 0"));
	CHECK(ap::term_of<node_t>(eq0) == sides(eq0).first);
	CHECK(ap::term_of<node_t>(prepared, ap::regime::backed) == tb);
	// sides that are leaves alone: the plain regime writes the `bf_xor`,
	// the backed regime takes the ring sum on their BDDs all the same
	tref leaves = wff("r(y) = r(z)");
	tref lp = ap::term_of<node_t>(leaves);
	CHECK(tau::get(lp).child_is(tau::bf_xor));
	tref lb = ap::term_of<node_t>(leaves, ap::regime::backed);
	CHECK(lb != lp);
	CHECK(ref_of(lb) == tau_term_bdd<node_t>::bdd_xor(ref_of(bf("r(y)")),
		ref_of(bf("r(z)"))));
	// norm_equation: ours descends through ¬, the old one does not — pinned
	tref neg = wff("!(x = y)");
	tref ours = ap::norm_equation<node_t>(neg);
	tref old  = tau_lang::norm_equation<node_t>(neg);
	CHECK(old == neg);
	CHECK(ours != neg);
	CHECK(tau::get(ours).child_is(tau::wff_neg));
	CHECK(tau::subtree_equals(ours, build_wff_neg<node_t>(build_bf_eq_0<node_t>(bf("x ^ y")))));
	CHECK(tau::subtree_equals(ap::norm_equation<node_t>(plain), build_bf_eq_0<node_t>(bf("x ^ y"))));
	// a non-equation (a bitvector order atom — a Boolean one is decomposed
	// into an equation by the hooks) is returned as it is
	tref lt = wff("x:bv[8] <= y:bv[8]");
	REQUIRE(tau::get(lt).child_is(tau::bf_lteq));
	CHECK(ap::norm_equation<node_t>(lt) == lt);
#ifndef DEBUG
	// Debug asserts instead
	CHECK(ap::term_of<node_t>(lt) == nullptr);
#endif
}

// 8. mem_size ----------------------------------------------------------------

TEST_CASE("mem_size: shared BDD nodes counted once, terminals zero, leaves one") {
	// x·y ∪ x′·y′: the two y-nodes are one node with an inverter — 2 nodes
	tref l = sides(ap::prepare_terms<node_t>(wff("x & y | x' & y' = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(ap::mem_size<node_t>(l) == 2);
	// x·y ∪ x′·z with x on top: x, y and z — 3
	tref m = sides(ap::prepare_terms<node_t>(wff("x & y | x' & z = 0"))).first;
	CHECK(ap::mem_size<node_t>(m) == 3);
	// x·r(y): x and the leaf — 2
	tref k = sides(ap::prepare_terms<node_t>(wff("x & r(y) = 0"))).first;
	CHECK(ap::mem_size<node_t>(k) == 2);
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
	tref z = vr("z");
	tref body = wff("(x & y | x' & z = 0) && (y & z = x)");
	tref prepared = ap::prepare_terms<node_t>(body);
	REQUIRE(has_bdd_id(prepared));
	tref finished = th::convert_to_tau_terms(prepared);
	CHECK(!has_bdd_id(finished));
	// the round trip: the same BDD, hence the same interned node
	tref again = ap::prepare_terms<node_t>(finished);
	CHECK(again == prepared);
	// and a second round trip is stable too
	CHECK(th::convert_to_tau_terms(again) == finished);
	// a prepared atom REWRAPped under a binder is finished through it
	tref under = build_wff_ex<node_t>(z, prepared, false);
	CHECK(!has_bdd_id(th::convert_to_tau_terms(under)));
	// nothing to finish: the same tref
	CHECK(th::convert_to_tau_terms(body) == body);
	// interning: the same BDD built twice is one node
	tref l1 = sides(ap::prepare_terms<node_t>(wff("x & y = 0"))).first;
	tref l2 = sides(ap::prepare_terms<node_t>(wff("y & x = 0"))).first;
	CHECK(l1 == l2);
	CHECK(th::convert_to_tau_node(th::U.find(th::key_of(l1))->second, find_ba_type<node_t>(l1)) == l1);
	CHECK(th::convert_to_handle(l1) == th::U.find(th::key_of(l1))->second);
}

// 10. leaf hazard ------------------------------------------------------------

TEST_CASE("leaf_fv: the leaves' contribution alone") {
	tref x = vr("x"), y = vr("y"), z = vr("z");
	tref l = sides(ap::prepare_terms<node_t>(wff("x & y = 0"))).first;
	CHECK(ap::leaf_fv<node_t>(l).empty());
	tref m = sides(ap::prepare_terms<node_t>(wff("x & y & r(z) = 0"))).first;
	trefs mfv = ap::leaf_fv<node_t>(m);
	CHECK(mfv.size() == 1);
	CHECK(tau::subtree_equals(mfv[0], z));
	// the §1 example: a block variable inside a reference argument
	tref h = sides(ap::prepare_terms<node_t>(wff("x & r(y) = 0"))).first;
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

// 11. the PURE shapes ---------------------------------------------------------

TEST_CASE("is_pure_term: variables, constants, the BA operations and a chain") {
	const ap::ba_type_id T = tau_type_id<node_t>();
	// ∪ · ′ and a functional-quantifier chain over them
	CHECK(ap::is_pure_term<node_t>(bf("x & y | z'"), T));
	CHECK(ap::is_pure_term<node_t>(tau::build_bf_and(tbf("x"), tbf("y")), T));
	CHECK(ap::is_pure_term<node_t>(bf("fall y fex w (x & y & w | z)"), T));
	// arithmetic is out, read under its own type
	tref sum = bf("x:bv[8] + { 1 }:bv[8]");
	const ap::ba_type_id BV = find_ba_type<node_t>(sum);
	REQUIRE(BV != 0);
	REQUIRE(BV != T);
	CHECK(!ap::is_pure_term<node_t>(sum, BV));
	CHECK(ap::is_pure_term<node_t>(bf("x:bv[8] & y:bv[8]"), BV));
	// the same term read under `tau`: every bitvector node is a
	// foreign-typed subterm, a constant as much as a variable
	CHECK(!ap::is_pure_term<node_t>(bf("x:bv[8] & y:bv[8]"), T));
	CHECK(!ap::is_pure_term<node_t>(bf("{ 1 }:bv[8]"), T));
	// and the `tau`-typed term read under the bitvector type is foreign the
	// same way, which is what says the fixture carries its type at all
	CHECK(!ap::is_pure_term<node_t>(
		tau::build_bf_and(tbf("x"), tbf("y")), BV));
	// a term-level reference
	CHECK(!ap::is_pure_term<node_t>(bf("x & r(y)"), T));
}

TEST_CASE("is_pure_term: a BDD-backed term is read leaf by leaf") {
	const ap::ba_type_id T = tau_type_id<node_t>();
	tref l = sides(ap::prepare_terms<node_t>(
		wff("x & y | x' & z = 0"))).first;
	REQUIRE(th::is_bdd_backed(l));
	CHECK(ap::is_pure_term<node_t>(l, T));
	// what a reference hides sits in a leaf (§1's leaf hazard), where the
	// scan of the leaves finds it
	tref m = sides(ap::prepare_terms<node_t>(wff("x & r(y) = 0"))).first;
	REQUIRE(th::is_bdd_backed(m));
	CHECK(!ap::is_pure_term<node_t>(m, T));
}

TEST_CASE("is_pure_equation: an equation through one ¬, never an order atom") {
	const ap::ba_type_id T = tau_type_id<node_t>();
	tref e = wff("x & y = 0");
	CHECK(ap::is_pure_equation<node_t>(e, T));
	tref ne = build_wff_neg<node_t>(e);
	REQUIRE(ap::is_negated_equation<node_t>(ne));
	CHECK(ap::is_pure_equation<node_t>(ne, T));
	// an order atom is no equation — it is never split and rides whole
	CHECK(!ap::is_pure_equation<node_t>(order_atom(tbf("x"), tbf("y")), T));
	// a reference on a side refuses the equation
	CHECK(!ap::is_pure_equation<node_t>(wff("x & r(y) = 0"), T));
	// a `bf_neq` atom is no equation here: after phase 3 a negative leaf is
	// `¬(l = r)` (invariant 4) and none of these is left
	tref neq = wff("x != y");
	REQUIRE(is_child<node_t>(neq, tau::bf_neq));
	CHECK(!ap::is_pure_equation<node_t>(neq, T));
	// and neither is anything that is no atom at all
	CHECK(!ap::is_pure_equation<node_t>(wff("f(x)"), T));
	CHECK(!ap::is_pure_equation<node_t>(wff("x = 0 && y = 0"), T));
}

TEST_CASE("is_pure_formula: pure literals, trees and units, through ∧, ∨, ¬ and binders") {
	const ap::ba_type_id BV = bv_type_id<node_t>(2);
	auto pure = [BV](tref n) { return ap::is_pure_formula<node_t>(n, BV); };
	CHECK(pure(typed("x:bv[2] & y:bv[2] = { 0 }:bv[2].")));
	CHECK(pure(typed("!(x:bv[2] & y:bv[2] = { 0 }:bv[2]).")));
	tref flat = typed("!(x:bv[2] = { 0 }:bv[2]) || !(y:bv[2] = { 0 }:bv[2]).");
	REQUIRE(ap::is_flat_tree<node_t>(flat));
	CHECK(pure(flat));
	tref nested = typed("!(x:bv[2] = { 0 }:bv[2]) "
		"|| (!(y:bv[2] = { 0 }:bv[2]) && !(z:bv[2] = { 0 }:bv[2])).");
	REQUIRE(ap::is_negative_tree<node_t>(nested));
	REQUIRE(!ap::is_flat_tree<node_t>(nested));
	CHECK(pure(nested));
	tref unit = typed("ex y (x:bv[2] & y:bv[2] = { 0 }:bv[2]).");
	REQUIRE(is_child_quantifier<node_t>(unit));
	CHECK(pure(unit));
	CHECK(pure(typed("x:bv[2] = { 0 }:bv[2] || !(y:bv[2] = { 0 }:bv[2]).")));
	CHECK(pure(tau::_T()));
}

TEST_CASE("is_pure_formula: an order atom, arithmetic, a reference, an impure or foreign unit, a bf_neq") {
	const ap::ba_type_id BV = bv_type_id<node_t>(2);
	auto pure = [BV](tref n) { return ap::is_pure_formula<node_t>(n, BV); };
	CHECK(!pure(typed("x:bv[2] <= y:bv[2].")));
	CHECK(!pure(typed("x:bv[2] + y:bv[2] = { 0 }:bv[2].")));
	CHECK(!pure(typed("x:bv[2] = { 0 }:bv[2] && q(y).")));
	CHECK(!pure(typed("ex y (x:bv[2] + y:bv[2] = { 0 }:bv[2]).")));
	// a binder over a variable of another type, around a pure body
	tref eq = typed("x:bv[2] = { 0 }:bv[2].");
	REQUIRE(pure(eq));
	CHECK(!pure(tau::build_wff_ex(
		tau::build_variable("w", bv_type_id<node_t>(4)), eq, false)));
	// a `bf_neq` atom, as `wff` parses `!=`
	tref neq = wff("x:bv[2] != { 0 }:bv[2]");
	REQUIRE(is_child<node_t>(neq, tau::bf_neq));
	CHECK(!pure(neq));
}

} // TEST_SUITE
