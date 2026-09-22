// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/push/fast_paths.h: §6's sign census, the two
// whole-formula fast paths 2a and 2b, the distribution to the atoms, and §3's
// SQUEEZE_POSITIVES behind 2b. Spec: anti_prenex.md §6 (SIGN_CENSUS,
// TRY_FAST_PATHS, DISTRIBUTE_TO_ATOMS), §3 (SQUEEZE_POSITIVES, the result
// joins), §1 (the PURE (¬)equation), §7 (EX_DISTRIBUTES_OVER_NEGATIVES),
// invariants 1, 6, 7 and 8.
//
// The harness is the push suite's: parse `∃P.ψ`, normalise `ψ` the way phases
// 1 and 3 leave it, put it into §1's term representation and hand the block to
// the step under test. What is claimed of a result: EQUIVALENT to `∃P.ψ`
// (`are_nso_equivalent` on the finished formula), no free variable ESCAPED,
// INVARIANT 4, and — where a fast path fires — the block RESOLVED.
//
// A CENSUS case states its COUNTS, which is how the walk's pruning and its
// early abort are read: a count that did NOT happen is the claim. A shape
// whose leaf order must be known is built by hand, since the joins emit a
// parsed formula's members in content order.
//
// TYPES: §7's table has no row for an untyped block, so every built fixture
// spells the type of its variables.
//
// Parsing note: a parsed quantifier's body runs to the RIGHT END and
// juxtaposition is conjunction, so every input keeps its parentheses.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normalizer.h"
#include "anti_prenex/push/fast_paths.h"
#include "anti_prenex/normalisers/nnf.h"

#include <algorithm>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using tb = tau_term_bdd<node_t>;
using th = term_handle<node_t>;
using order_t = ap::var_order<node_t>;

tref parse(const char* sample) {
	auto rr = get_nso_rr(sample);
	REQUIRE(rr.has_value());
	return rr.value().main->get();
}

/// A `tau`-typed `variable` node — a block member and an order key — and the
/// `bf` around it, which is what a term holds.
tref tvar(const char* name) {
	return tau::build_variable(name, tau_type_id<node_t>());
}
tref tbf(const char* name) {
	return tau::build_bf_variable(name, tau_type_id<node_t>());
}

/// The `bf` term around a `variable` node — how a block member is written
/// into a term. A variable a binder bound carries that binder's id, so it is
/// not the node a name alone builds.
tref as_bf(tref var) { return tau::get(tau::bf, var); }

/// `a·b = 0` and `¬(a·b = 0)` over two named `tau` variables: the two leaf
/// shapes the census classifies, built where the ORDER of the leaves matters.
tref eq2(const char* a, const char* b) {
	return build_bf_eq_0<node_t>(tau::build_bf_and(tbf(a), tbf(b)));
}
tref neq2(const char* a, const char* b) {
	return build_wff_neg<node_t>(eq2(a, b));
}

/// A real order atom `l ≤ r`. On a non-bitvector type the construction hooks
/// rewrite `≤` into the equation `l·r′ = 0`, so one does not survive
/// construction; the parser builds with the hooks disabled, and this helper
/// reproduces that shape the same way.
tref order_atom(tref l, tref r) {
	use_hooks_guard<node_t> g(false);
	return tau::build_bf_lteq(l, r);
}

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// Every formula binder of `n`, nested ones included.
size_t binder_count(tref n) {
	return tau::get(n).select_all(is_child_quantifier<node_t>).size();
}

/// The formula with every stored BDD spelled out, which is what the oracle
/// reads.
tref finished(tref n) { return th::convert_to_tau_terms(n); }

/// Semantic equality of two terms over the variables `vs`: equal ROBDDs under
/// one order over all of them (leaves opaque). Both are finished first, since
/// a BDD-backed term's own ref belongs to its own order.
bool same_function(tref a, tref b, const ap::block& vs) {
	order_t o;
	for (size_t i = 0; i < vs.size(); ++i)
		o.emplace(vs[i], int_t(vs.size() - i));
	return tb::build_bdd(th::convert_to_tau_terms(a), o)
		== tb::build_bdd(th::convert_to_tau_terms(b), o);
}

/// NO FREE VARIABLE ESCAPED: `FV(out) ⊆ FV(in)`.
bool no_escape(tref out, tref in) {
	const trefs& before = get_free_vars<node_t>(in);
	for (tref v : get_free_vars<node_t>(out))
		if (!std::binary_search(before.begin(), before.end(), v,
			tau::subtree_less)) return false;
	return true;
}

/// INVARIANT 4 over a whole formula: no `bf_neq` and no negated or mirrored
/// order operator anywhere, and every `¬` sits directly over an ATOM.
bool invariant_4(tref n) {
	bool clean = true;
	auto check = [&clean](tref m) {
		const auto& t = tau::get(m);
		if (t.is(tau::bf_neq) || t.is(tau::bf_nlt) || t.is(tau::bf_nlteq)
			|| t.is(tau::bf_gt) || t.is(tau::bf_gteq)
			|| t.is(tau::bf_ngt) || t.is(tau::bf_ngteq))
				return clean = false;
		if (t.child_is(tau::wff_neg)
			&& !is_atomic_fm<node_t>(
				tau::trim_right_sibling(t[0].first())))
				return clean = false;
		return true;
	};
	auto all_of_it = [](tref) { return true; };
	auto up = [](tref) {};
	pre_order<node_t>(n).search_unique(check, all_of_it, up);
	return clean;
}

/// A parsed `∃P.ψ` split the way the push's callers hand it over: the block
/// OUTERMOST FIRST, the component's ctx over it — `type` read off the block,
/// as invariant 2 allows — and the body as phase 4 meets it, put into §1's
/// term representation.
struct fixture {
	tref quantified = nullptr;   ///< `∃P.ψ` as parsed, the oracle's reference
	ap::block P;
	ap::ctx<node_t> c;
	tref clause = nullptr;       ///< `ψ`, normalised and prepared over `P`
};

/// `ψ` as PHASE 4 meets it: in NNF, with `f ≠ 0` spelled `¬(f = 0)` (phases 1
/// and 3, §3).
tref normalised(tref body) {
	return ap::normalize_operators<node_t>(
		ap::to_canonically_factored_nnf<node_t>(body));
}

/// @p vars caps how many binders of the chain become the BLOCK, the rest
/// staying part of the body: a nested binder is a UNIT to the census, and the
/// only way to write one is to leave it in the body.
fixture make(const char* src, bool keep = false, size_t vars = 0) {
	fixture f;
	f.quantified = parse(src);
	tref n = f.quantified;
	while (is_child_quantifier<node_t>(n)
		&& (vars == 0 || f.P.size() < vars))
	{
		f.P.push_back(tau::trim_right_sibling(
			ap::binder_var<node_t>(n)));
		n = tau::trim_right_sibling(ap::binder_body<node_t>(n));
	}
	REQUIRE(!f.P.empty());
	f.c = ap::ctx<node_t>::for_component(f.P,
		find_ba_type<node_t>(f.P[0]), keep);
	f.clause = ap::prepare_terms<node_t>(normalised(n), f.P, f.c.order);
	return f;
}

/// What every result of a fast path claims: equivalent to `∃P.ψ`, no free
/// variable escaped, invariant 4.
void check_against_source(tref got, const fixture& f) {
	CHECK(are_nso_equivalent<node_t>(finished(got), f.quantified));
	CHECK(no_escape(finished(got), f.quantified));
	CHECK(invariant_4(got));
}

/// THE BLOCK IS RESOLVED: no binder came back and no block variable is free
/// in the result.
void check_resolved(tref got, const fixture& f) {
	CHECK(binder_count(got) == 0);
	CHECK(!ap::fv_meets<node_t>(got, f.P));
}

} // namespace

TEST_SUITE("anti_prenex/fast_paths") {

// --- SIGN_CENSUS ------------------------------------------------------------------

TEST_CASE("S1: one count per X-touching leaf") {
	// `p = 1` is what 2a tolerates here — one top-level positive — so the
	// walk runs to the end and the counts are the whole formula's.
	fixture f = make("ex x (x a = 0 && (x y != 0 || (x w != 0 && x z != 0))).");
	const ap::census k = ap::sign_census<node_t>(f.clause, f.P, 1);
	CHECK(k.pos == 1);
	CHECK(k.neg == 3);
	CHECK(k.other == 0);
	CHECK(k.free == 0);
}

TEST_CASE("S2: a maximal X-free subtree is counted once and not descended") {
	// `w = 0 ∧ v = 0` is a constant under `∃x`: one count, and the two
	// equations inside it are never classified.
	fixture f = make("ex x (x a = 0 && (x y != 0 || (w = 0 && v = 0))).");
	const ap::census k = ap::sign_census<node_t>(f.clause, f.P, 1);
	CHECK(k.free == 1);
	CHECK(k.pos == 1);
	CHECK(k.neg == 1);
	CHECK(k.other == 0);
}

TEST_CASE("S3: a leaf reached twice is classified once") {
	const ap::block P{ tvar("x") };
	const tref l = neq2("x", "y");
	// The same literal stands in two places: hash-consing makes the two
	// occurrences ONE node, and the walk classifies it once (§10).
	const tref phi = build_wff_or<node_t>(l,
		build_wff_and<node_t>(l, neq2("x", "w")));
	REQUIRE(ap::members<node_t>(phi).size() == 2);
	const ap::census k = ap::sign_census<node_t>(phi, P, 0);
	CHECK(k.neg == 2);
	CHECK(k.pos == 0);
	CHECK(k.other == 0);
	CHECK(k.free == 0);
}

TEST_CASE("S4: an order atom is `other`, and the walk stops on it") {
	const ap::block P{ tvar("x") };
	// Built by hand, the order atom FIRST: the positive behind it is what
	// the abort leaves unclassified.
	const tref phi = build_wff_or<node_t>(
		order_atom(tbf("x"), tbf("b")), eq2("x", "c"));
	const ap::census k = ap::sign_census<node_t>(phi, P, 0);
	CHECK(k.other == 1);
	CHECK(k.pos == 0);
}

TEST_CASE("S5: a binder unit is `other`, and its body is never entered") {
	// The unit holds two positives; neither is counted, and neither is the
	// positive standing behind the unit.
	fixture f = make("ex x ex y (x y = 0 && x c = 0).", false, 1);
	REQUIRE(is_child_quantifier<node_t>(f.clause));
	const tref positive = build_bf_eq_0<node_t>(
		tau::build_bf_and(as_bf(f.P[0]), tbf("d")));
	const tref phi = build_wff_or<node_t>(f.clause, positive);
	const ap::census k = ap::sign_census<node_t>(phi, f.P, 0);
	CHECK(k.other == 1);
	CHECK(k.pos == 0);
}

TEST_CASE("S6: a reference is `other`, and the walk stops on it") {
	fixture f = make("ex x (q(x)).");
	REQUIRE(is_child<node_t>(f.clause, tau::wff_ref));
	const tref positive = build_bf_eq_0<node_t>(
		tau::build_bf_and(as_bf(f.P[0]), tbf("d")));
	const tref phi = build_wff_or<node_t>(f.clause, positive);
	const ap::census k = ap::sign_census<node_t>(phi, f.P, 0);
	CHECK(k.other == 1);
	CHECK(k.pos == 0);
}

TEST_CASE("S7: a positive past the tolerance, beside a negative, stops the walk") {
	const ap::block P{ tvar("x") };
	// The negative first, then two positives.
	const tref phi = build_wff_and<node_t>(neq2("x", "y"),
		build_wff_and<node_t>(eq2("x", "a"), eq2("x", "b")));
	// `p = 0`: the first positive refutes 2a (it is no top-level literal
	// of a caller's `P`) and 2b (the negative), and the second positive is
	// never classified.
	const ap::census k0 = ap::sign_census<node_t>(phi, P, 0);
	CHECK(k0.neg == 1);
	CHECK(k0.pos == 1);
	// `p = 1` tolerates one, so the walk goes on to the second.
	const ap::census k1 = ap::sign_census<node_t>(phi, P, 1);
	CHECK(k1.neg == 1);
	CHECK(k1.pos == 2);
}

// --- 2a ---------------------------------------------------------------------------

TEST_CASE("A1: 2a distributes the block to every negated leaf") {
	// `∃x(x·a = 0 ∧ (x·y ≠ 0 ∨ (x·w ≠ 0 ∧ x·z ≠ 0)))`: the top-level
	// positive rides into every leaf's clause, the `∀_x x·a = 0` half is
	// emitted once at the top — here it is `0 = 0`, which the and-join
	// drops — and the ∧/∨ skeleton comes back with one condition per
	// negated leaf.
	fixture f = make("ex x (x a = 0 && (x y != 0 || (x w != 0 && x z != 0))).");
	auto got = ap::try_fast_paths<node_t>(f.clause, f.P, f.c);
	REQUIRE(got.has_value());
	check_against_source(*got, f);
	check_resolved(*got, f);
	REQUIRE(is_child<node_t>(*got, tau::wff_or));
	const trefs ms = ap::members<node_t>(*got);
	REQUIRE(ms.size() == 2);
	size_t literals = 0, pairs = 0;
	for (tref m : ms) {
		if (ap::is_negated_equation<node_t>(m)) ++literals;
		else if (is_child<node_t>(m, tau::wff_and)
			&& ap::members<node_t>(m).size() == 2) ++pairs;
	}
	CHECK(literals == 1);
	CHECK(pairs == 1);
}

TEST_CASE("A2: 2a on a bare negative tree takes the empty positive set") {
	// No conjunction, so `P` is empty and `⋀P` is `T`: every leaf's clause
	// is the leaf alone.
	fixture f = make("ex x (x y != 0 || x w != 0).");
	auto got = ap::try_fast_paths<node_t>(f.clause, f.P, f.c);
	REQUIRE(got.has_value());
	check_against_source(*got, f);
	check_resolved(*got, f);
	CHECK(are_nso_equivalent<node_t>(finished(*got),
		parse("y != 0 || w != 0.")));
}

TEST_CASE("A3: an inner positive refutes 2a") {
	// `x·y = 0` is one positive more than the top-level literals, so it is
	// no conjunct of every DNF clause and the leaf-wise transform does not
	// hold; 2b is refuted by the negative.
	fixture f = make("ex x (x a = 0 && (x y = 0 || x w != 0)).");
	const ap::census k = ap::sign_census<node_t>(f.clause, f.P, 1);
	CHECK(k.pos == 2);
	CHECK(k.neg == 1);
	CHECK(!ap::try_fast_paths<node_t>(f.clause, f.P, f.c).has_value());
}

TEST_CASE("A4: 2a asks the type table, and a bitvector block refuses it") {
	// `∃` does not distribute over negatives outside the atomless types
	// (invariant 1), and the question is asked before anything is
	// eliminated.
	fixture f = make("ex x (x:bv[8] & a:bv[8] != { 0 }:bv[8] "
		"|| x:bv[8] & w:bv[8] != { 0 }:bv[8]).");
	REQUIRE(!ap::ex_distributes_over_negatives<node_t>(f.c.type));
	const ap::census k = ap::sign_census<node_t>(f.clause, f.P, 0);
	REQUIRE(k.neg == 2);
	REQUIRE(k.pos == 0);
	CHECK(!ap::try_fast_paths<node_t>(f.clause, f.P, f.c).has_value());
}

TEST_CASE("A5: an impure positive is an ordinary positive leaf") {
	// `x + 1 = 0` is no PURE equation (§1), so it is not one of 2a's
	// top-level literals: the census counts one positive where the path
	// tolerates none.
	fixture f = make("ex x (x:bv[8] + { 1 }:bv[8] = { 0 }:bv[8] "
		"&& x:bv[8] & y:bv[8] != { 0 }:bv[8]).");
	const trefs ms = ap::members<node_t>(f.clause);
	REQUIRE(ms.size() == 2);
	for (tref m : ms) CHECK(!(is_child<node_t>(m, tau::bf_eq)
		&& ap::is_pure_equation<node_t>(m, f.c.type)));
	const ap::census k = ap::sign_census<node_t>(f.clause, f.P, 0);
	CHECK(k.pos == 1);
	CHECK(k.neg == 1);
	CHECK(!ap::try_fast_paths<node_t>(f.clause, f.P, f.c).has_value());
}

// --- 2b ---------------------------------------------------------------------------

TEST_CASE("B1: 2b squeezes a positive skeleton on terms") {
	// Four DNF clauses, so four terms and four one-atom clauses.
	fixture f = make("ex x ((x a = 0 || x b = 0) && (x c = 0 || x d = 0)).");
	const ap::census k = ap::sign_census<node_t>(f.clause, f.P, 0);
	REQUIRE(k.pos == 4);
	REQUIRE(k.neg == 0);
	REQUIRE(k.free == 0);
	REQUIRE(k.other == 0);
	CHECK(ap::squeeze_positives<node_t>(f.clause, f.c.order).size() == 4);
	auto got = ap::try_fast_paths<node_t>(f.clause, f.P, f.c);
	REQUIRE(got.has_value());
	check_against_source(*got, f);
	check_resolved(*got, f);
}

TEST_CASE("B2: a negative or an X-free leaf refutes 2b") {
	fixture f = make("ex x ((x a = 0 || x b = 0) && x c != 0).");
	CHECK(ap::sign_census<node_t>(f.clause, f.P, 0).neg == 1);
	CHECK(!ap::try_fast_paths<node_t>(f.clause, f.P, f.c).has_value());
	// The term-level squeeze has no way to carry a formula-level X-free
	// part through the cross product, so one refutes the path.
	fixture g = make("ex x ((x a = 0 || b = 0) && (x c = 0 || x d = 0)).");
	CHECK(ap::sign_census<node_t>(g.clause, g.P, 0).free == 1);
	CHECK(!ap::try_fast_paths<node_t>(g.clause, g.P, g.c).has_value());
}

TEST_CASE("B3: an atom that discharges to T decides the join") {
	// `∃x.(x·a = 0)` is `∀x x·a = 0`, which is `0 = 0`: the first atom of
	// the list decides the ∨-join (invariant 7). That the rest of the list
	// is not eliminated is not observable from the outside; the result is.
	fixture f = make("ex x (x a = 0 || x b = 0).");
	auto got = ap::try_fast_paths<node_t>(f.clause, f.P, f.c);
	REQUIRE(got.has_value());
	CHECK(tau::get(*got).equals_T());
	check_against_source(*got, f);
}

// --- DISTRIBUTE_TO_ATOMS ------------------------------------------------------------

TEST_CASE("D1: an X-free subtree comes back verbatim") {
	fixture f = make("ex x (x a != 0 || (b = 0 && c = 0)).");
	const trefs ms = ap::members<node_t>(f.clause);
	REQUIRE(ms.size() == 2);
	tref neutral = nullptr;
	for (tref m : ms) if (!ap::fv_meets<node_t>(m, f.P)) neutral = m;
	REQUIRE(neutral != nullptr);
	const tref got = ap::distribute_to_atoms<node_t>(f.clause, f.P, f.c,
		trefs{});
	bool kept = false;
	for (tref m : ap::members<node_t>(got)) kept = kept || same(m, neutral);
	CHECK(kept);
	CHECK(binder_count(got) == 0);
	check_against_source(got, f);
}

TEST_CASE("D2: a literal that decided folds through the skeleton") {
	// The leaf `x·a = 0` eliminates to `T`, and the ∨-join above it
	// decides the whole skeleton.
	fixture f = make("ex x (x a = 0 || (x y != 0 && x z != 0)).");
	const tref got = ap::distribute_to_atoms<node_t>(f.clause, f.P, f.c,
		trefs{});
	CHECK(tau::get(got).equals_T());
}

// --- SQUEEZE_POSITIVES --------------------------------------------------------------

TEST_CASE("Q1: a conjunction squeezes to the union, a disjunction to the list") {
	// `(a = 0 ∨ b = 0) ∧ c = 0` is `a ∪ c = 0 ∨ b ∪ c = 0`.
	fixture f = make("ex a ex b ex c ((a = 0 || b = 0) && c = 0).");
	const trefs ts = ap::squeeze_positives<node_t>(f.clause, f.c.order);
	REQUIRE(ts.size() == 2);
	REQUIRE(f.P.size() == 3);
	const tref ac = build_bf_or<node_t>(as_bf(f.P[0]), as_bf(f.P[2]));
	const tref bc = build_bf_or<node_t>(as_bf(f.P[1]), as_bf(f.P[2]));
	CHECK((same_function(ts[0], ac, f.P) || same_function(ts[0], bc, f.P)));
	CHECK((same_function(ts[1], ac, f.P) || same_function(ts[1], bc, f.P)));
	CHECK(!same_function(ts[0], ts[1], f.P));
}

TEST_CASE("Q2: a single equation squeezes to its own term") {
	fixture f = make("ex a (a c = 0).");
	const trefs ts = ap::squeeze_positives<node_t>(f.clause, f.c.order);
	REQUIRE(ts.size() == 1);
	CHECK(same_function(ts[0],
		ap::term_of<node_t>(f.clause, f.c.order), f.P));
}

TEST_CASE("Q3: the list is as long as the skeleton has DNF clauses") {
	// Three binary disjunctions: 2³ clauses, the honest cost of 2b
	// (invariant 8).
	fixture f = make("ex a ex b ex c ex d ex e ex g "
		"((a = 0 || b = 0) && (c = 0 || d = 0) && (e = 0 || g = 0)).");
	CHECK(ap::squeeze_positives<node_t>(f.clause, f.c.order).size() == 8);
}

} // TEST_SUITE
