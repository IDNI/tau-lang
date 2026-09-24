// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/push/expand.h: §6's FOLD_DECIDED, the
// CONJOINABLE test and EXPAND, the floor of the conjunction's ladder. Spec:
// anti_prenex.md §6 (FOLD_DECIDED, EXPAND), §1 (`prio`), §3 (the result
// joins), invariant 7.
//
// The harness is the junctions suite's: parse `∃P.ψ`, normalise `ψ` the way
// phases 1 and 3 leave it — no SIMPLIFY, so a shape phase 1 would fold still
// reaches the step — put it into §1's term representation and call the step
// DIRECTLY on the prepared clause. What is claimed of a result: EQUIVALENT to
// `∃P.ψ` (`are_nso_equivalent` on the finished formula), no free variable
// ESCAPED, INVARIANT 4, and per case the block RESOLVED or the binders left.
//
// FOLD_DECIDED fires only in a BA with constants beyond 0 and 1: over those
// two alone a constant residual forces a constant term, which SIMPLIFY_ATOM
// folds already. Its positive cases run over `sbf`, whose constants parse as
// `{A}:sbf` — which is why this suite is built over the `tau` pack of BAs,
// the one that carries `sbf`.
//
// Parsing note: a parsed quantifier's body runs to the RIGHT END and
// juxtaposition is conjunction, so every input keeps its parentheses.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "normalizer.h"
#include "anti_prenex/push/push_block.h"
#include "anti_prenex/push/expand.h"
#include "anti_prenex/normalisers/nnf.h"

#include <algorithm>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using th = term_handle<node_t>;

tref parse(const char* sample) {
	auto rr = get_nso_rr(sample);
	REQUIRE(rr.has_value());
	return rr.value().main->get();
}

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// Is there a node of this kind anywhere in `n`?
bool holds(tref n, size_t nt) {
	return tau::get(n).find_top([nt](tref m) {
		return tree<node_t>::get(m).is(nt); }) != nullptr;
}

/// Every formula binder of `n`, nested ones included.
size_t binder_count(tref n) {
	return tau::get(n).select_all(is_child_quantifier<node_t>).size();
}

/// Does `n` hold a BA constant once its terms are spelled out?
bool has_constant(tref n) {
	return holds(th::convert_to_tau_terms(n), tau::ba_constant);
}

/// A member of `n` content-equal to `m`?
bool has_member(tref n, tref m) {
	for (tref c : ap::members<node_t>(n)) if (same(c, m)) return true;
	return false;
}

/// The formula with every stored BDD spelled out, which is what the oracle
/// reads.
tref finished(tref n) { return th::convert_to_tau_terms(n); }

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

fixture make(const char* src) {
	// The engine behind `sbf`'s constants is set up before one is parsed;
	// the call is idempotent.
	bdd_init<Bool>();
	fixture f;
	f.quantified = parse(src);
	tref n = f.quantified;
	while (is_child_quantifier<node_t>(n)) {
		f.P.push_back(tau::trim_right_sibling(
			ap::binder_var<node_t>(n)));
		n = tau::trim_right_sibling(ap::binder_body<node_t>(n));
	}
	REQUIRE(!f.P.empty());
	f.c = ap::ctx<node_t>::for_component(f.P,
		find_ba_type<node_t>(f.P[0]), false);
	f.clause = ap::prepare_terms<node_t>(normalised(n), f.P, f.c.order);
	return f;
}

/// EXPAND on the prepared clause.
tref expanded(fixture& f) {
	return ap::expand<node_t>(f.clause, f.P, f.c);
}

/// FOLD_DECIDED on the prepared clause's conjunct list.
std::optional<trefs> folded(fixture& f) {
	return ap::fold_decided<node_t>(ap::members<node_t>(f.clause), f.P,
		f.c);
}

/// What every pushed block claims: equivalent to `∃P.ψ`, no free variable
/// escaped, invariant 4.
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

/// The clause's disjunctive conjuncts, in member order.
trefs disjunctive(const fixture& f) {
	trefs out;
	for (tref m : ap::members<node_t>(f.clause))
		if (is_child<node_t>(m, tau::wff_or)) out.push_back(m);
	return out;
}

// Untyped atoms for the structural CONJOINABLE cases.
tref atom(const char* name) {
	return tau::build_bf_eq_0(tau::build_bf_variable(name, 0));
}
tref neg(tref n)          { return tau::build_wff_neg(n); }
tref conj(tref l, tref r) { return tau::build_wff_and(l, r); }
tref disj(tref l, tref r) { return tau::build_wff_or(l, r); }

} // namespace


TEST_SUITE("anti_prenex/expand") {

// --- FOLD_DECIDED -------------------------------------------------------------------

TEST_CASE("FD1: an equation without a block variable, a unit and a reference "
	"are kept") {
	// `a = 0` holds no block variable, so there is nothing to probe; `x·y
	// = 0` is probed, and its cofactors' product `0·y` is no nonzero
	// constant; the unit and the reference are opaque. The list comes back
	// as it went in.
	fixture f = make("ex x (xy = 0 && a = 0 && q(x) "
		"&& (ex z (xz = 0 && zw != 0))).");
	const trefs C = ap::members<node_t>(f.clause);
	REQUIRE(C.size() == 4);
	REQUIRE(std::ranges::count_if(C, is_child_quantifier<node_t>) == 1);
	REQUIRE(std::ranges::count_if(C, [](tref m) {
		return is_child<node_t>(m, tau::wff_ref); }) == 1);
	REQUIRE(std::ranges::count_if(C, [&f](tref m) {
		return is_child<node_t>(m, tau::bf_eq)
			&& !ap::fv_meets<node_t>(m, f.P); }) == 1);
	const auto got = ap::fold_decided<node_t>(C, f.P, f.c);
	REQUIRE(got.has_value());
	REQUIRE(got->size() == C.size());
	for (size_t i = 0; i < C.size(); ++i) CHECK(same((*got)[i], C[i]));
}

TEST_CASE("FD2: a positive equation whose residual is a nonzero constant makes "
	"the list F") {
	// `f = x′·A ∪ x·B` has the cofactors `A` and `B` on `x`, and `f = 0`
	// forces `A·B = 0` whatever `x` is. `A·B` is a nonzero constant of
	// `sbf`, so the atom is `F`, and so is the list.
	fixture f = make("ex x (((x' & {A}:sbf) | (x & {B}:sbf)) = 0 "
		"&& x & y != 0).");
	REQUIRE(f.c.type == sbf_type_id<node_t>());
	REQUIRE(ap::members<node_t>(f.clause).size() == 2);
	CHECK(!folded(f).has_value());
}

TEST_CASE("FD3: a negated equation proven F rides out as T") {
	fixture f = make("ex x (((x' & {A}:sbf) | (x & {B}:sbf)) != 0 "
		"&& x & y != 0).");
	REQUIRE(f.c.type == sbf_type_id<node_t>());
	REQUIRE(ap::members<node_t>(f.clause).size() == 2);
	const auto got = folded(f);
	REQUIRE(got.has_value());
	REQUIRE(got->size() == 1);
	CHECK(!has_constant((*got)[0]));
	CHECK(has_member(f.clause, (*got)[0]));
}

TEST_CASE("FD4: a negative tree folds literal-wise") {
	// A two-literal tree whose decided literal is `T` is `T`, and dropped.
	fixture f = make("ex x ((((x' & {A}:sbf) | (x & {B}:sbf)) != 0 "
		"|| x & y != 0) && x & z != 0).");
	REQUIRE(f.c.type == sbf_type_id<node_t>());
	REQUIRE(disjunctive(f).size() == 1);
	REQUIRE(ap::is_negative_tree<node_t>(disjunctive(f)[0]));
	const auto got = folded(f);
	REQUIRE(got.has_value());
	REQUIRE(got->size() == 1);
	CHECK(ap::is_negated_equation<node_t>((*got)[0]));
	CHECK(has_member(f.clause, (*got)[0]));
	// A decided literal under an inner ∧ leaves the tree's other material:
	// `(¬(f = 0) ∧ ¬(x·z = 0)) ∨ ¬(x·y = 0)` keeps the two other atoms.
	fixture g = make("ex x (((((x' & {A}:sbf) | (x & {B}:sbf)) != 0 "
		"&& x & z != 0) || x & y != 0) && x & w != 0).");
	const trefs ts = disjunctive(g);
	REQUIRE(ts.size() == 1);
	REQUIRE(ap::is_negative_tree<node_t>(ts[0]));
	REQUIRE(!ap::is_flat_tree<node_t>(ts[0]));
	REQUIRE(ap::atoms<node_t>(ts[0]).size() == 3);
	const auto kept = ap::fold_decided<node_t>(ap::members<node_t>(g.clause),
		g.P, g.c);
	REQUIRE(kept.has_value());
	REQUIRE(kept->size() == 2);
	tref t = nullptr;
	for (tref m : *kept) if (is_child<node_t>(m, tau::wff_or)) t = m;
	REQUIRE(t != nullptr);
	CHECK(ap::is_flat_tree<node_t>(t));
	const trefs after = ap::atoms<node_t>(t);
	CHECK(after.size() == 2);
	for (tref a : after) {
		CHECK(ap::has_atom<node_t>(ts[0], a));
		CHECK(!has_constant(a));
	}
}

TEST_CASE("FD5: the probe variable is the innermost block variable") {
	// `f = y′·(A ∪ x·B′) ∪ y·B` under `X = (x, y)`. On `y` the cofactors
	// are `A ∪ x·B′` and `B`, whose product is the constant `A·B`; on `x`
	// they are `y′·A ∪ y·B` and `y′·(A ∪ B′) ∪ y·B`, whose product
	// `y′·A ∪ y·B` is no constant. `y`, the innermost, has the higher prio
	// and is the one probed, so the atom is `F`.
	fixture f = make("ex x ex y (((y' & ({A}:sbf | (x & ({B}:sbf)'))) "
		"| (y & {B}:sbf)) = 0).");
	REQUIRE(f.P.size() == 2);
	REQUIRE(f.c.type == sbf_type_id<node_t>());
	CHECK(!folded(f).has_value());
	// Under the block `(x)` alone the probe is on `x`, and the atom stays.
	const auto got = ap::fold_decided<node_t>(ap::members<node_t>(f.clause),
		ap::block{ f.P[0] }, f.c);
	REQUIRE(got.has_value());
	CHECK(got->size() == 1);
}

// --- CONJOINABLE --------------------------------------------------------------------

TEST_CASE("CJ1: a literal is conjoinable") {
	CHECK(ap::is_conjoinable<node_t>(atom("a")));
	CHECK(ap::is_conjoinable<node_t>(neg(atom("a"))));
}

TEST_CASE("CJ2: a flat negative tree is conjoinable") {
	const tref t = disj(neg(atom("a")), neg(atom("b")));
	REQUIRE(ap::is_flat_tree<node_t>(t));
	CHECK(ap::is_conjoinable<node_t>(t));
}

TEST_CASE("CJ3: a nested negative tree is conjoinable") {
	const tref t = disj(conj(neg(atom("a")), neg(atom("b"))),
		neg(atom("c")));
	REQUIRE(ap::is_negative_tree<node_t>(t));
	REQUIRE(!ap::is_flat_tree<node_t>(t));
	CHECK(ap::is_conjoinable<node_t>(t));
}

TEST_CASE("CJ4: a conjunction of positives is conjoinable, and so is one "
	"with a negative tree") {
	const tref ab = conj(atom("a"), atom("b"));
	REQUIRE(is_child<node_t>(ab, tau::wff_and));
	CHECK(ap::is_conjoinable<node_t>(ab));
	CHECK(ap::is_conjoinable<node_t>(conj(atom("a"),
		disj(neg(atom("b")), neg(atom("c"))))));
}

TEST_CASE("CJ5: a disjunction with a positive member is not conjoinable") {
	CHECK(!ap::is_conjoinable<node_t>(disj(atom("a"), neg(atom("b")))));
	CHECK(!ap::is_conjoinable<node_t>(disj(atom("a"), atom("b"))));
}

TEST_CASE("CJ6: a conjunction holding such a disjunction is not "
	"conjoinable") {
	const tref e = conj(atom("c"), disj(atom("a"), neg(atom("b"))));
	REQUIRE(is_child<node_t>(e, tau::wff_and));
	CHECK(!ap::is_conjoinable<node_t>(e));
}

// --- EXPAND -------------------------------------------------------------------------

TEST_CASE("X1: L17's clause resolves through two cases") {
	// `E = ¬(x·z = 0)` and `D = x·y = 0 ∨ x·w = 0`. Each case is a
	// conjunction of literals, so `x` settles and the leaf takes it; the
	// second case carries the first member's complement.
	fixture f = make("ex x ((xy = 0 || xw = 0) && xz != 0).");
	const tref got = expanded(f);
	check_against_source(got, f);
	check_resolved(got, f);
}

TEST_CASE("X2: the cases are exclusive — a later case carries the complement "
	"of an earlier member") {
	// L19's clause. `v = 0` and `x·y = 0` have one size, and the chain puts
	// the X-free `v = 0` first: its case is `v = 0 ∧ ∃x(x·k ≠ 0)`, and the
	// second case is built under `¬(v = 0)`.
	fixture f = make("ex x ((xy = 0 || v = 0) && xk != 0).");
	REQUIRE(disjunctive(f).size() == 1);
	const trefs ds = ap::members<node_t>(disjunctive(f)[0]);
	REQUIRE(ds.size() == 2);
	REQUIRE(!ap::fv_meets<node_t>(ds[0], f.P));
	const tref got = expanded(f);
	check_against_source(got, f);
	check_resolved(got, f);
	REQUIRE(is_child<node_t>(got, tau::wff_or));
	const tref excluded = ap::complement_of<node_t>(ds[0]);
	CHECK(std::ranges::any_of(ap::members<node_t>(got),
		[excluded](tref m) { return has_member(m, excluded); }));
}

TEST_CASE("X3: a dead case builds no copy and no exclusion") {
	// Unsimplified by the fixture, `x·z = 0` sits in `D` beside the
	// conjunct `¬(x·z = 0)`: its case meets its complement in the ∧-join
	// and dies unbuilt. The result is the second case's alone,
	// `∃x(x·y = 0 ∧ x·z ≠ 0)`.
	fixture f = make("ex x ((xz = 0 || xy = 0) && xz != 0).");
	REQUIRE(disjunctive(f).size() == 1);
	const trefs ds = ap::members<node_t>(disjunctive(f)[0]);
	REQUIRE(ds.size() == 2);
	REQUIRE(has_member(f.clause, ap::complement_of<node_t>(ds[0])));
	const tref got = expanded(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(ap::is_negated_equation<node_t>(got));
}

TEST_CASE("X4: a member proven T makes D T") {
	// `D = ¬(f = 0) ∨ x·y = 0` with FD2's `f`. `x·y = 0` is the smaller
	// member and is expanded first; then `¬(f = 0)` folds to `T`, so `ψ`
	// is `E` alone: one push of `¬(x·z = 0)`, `z ≠ 0`, which subsumes the
	// case built before it.
	fixture f = make("ex x ((((x' & {A}:sbf) | (x & {B}:sbf)) != 0 "
		"|| x & y = 0) && x & z != 0).");
	REQUIRE(f.c.type == sbf_type_id<node_t>());
	REQUIRE(disjunctive(f).size() == 1);
	const tref got = expanded(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(ap::is_negated_equation<node_t>(got));
}

TEST_CASE("X5: a case that decides T leaves the later cases unbuilt") {
	// `x = 0` is the smaller member, and `∃x(x = 0 ∧ x·z = 0)` is `T`: the
	// ∨-join is decided and `¬(x·w = 0)` is never built.
	fixture f = make("ex x ((x = 0 || xw != 0) && xz = 0).");
	REQUIRE(disjunctive(f).size() == 1);
	REQUIRE(ap::members<node_t>(disjunctive(f)[0]).size() == 2);
	const tref got = expanded(f);
	check_against_source(got, f);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("X5b: the inner expansion through the floor decides T at its first "
	"case") {
	// The two disjunctions tie on prio and member count, so content order
	// picks `D = x·y = 0 ∨ x·w = 0`. Its first case, `x·y = 0` (`x·w = 0`
	// alike), pushes `x·y = 0 ∧ (x·z = 0 ∨ ¬(x·k = 0))`, which no cheaper
	// step takes, back to the floor; there the smaller member `x·z = 0`
	// gives `∃x(x·y = 0 ∧ x·z = 0)`, `T` at `x := 0`, which decides the
	// inner ∨-join and then the outer one: two of the four members built.
	fixture f = make("ex x ((xy = 0 || xw = 0) && (xz = 0 || xk != 0)).");
	const trefs S = disjunctive(f);
	REQUIRE(S.size() == 2);
	auto all_positive = [](tref d) {
		return std::ranges::all_of(ap::members<node_t>(d), [](tref m) {
			return is_child<node_t>(m, tau::bf_eq); });
	};
	const tref D = all_positive(S[0]) ? S[0] : S[1];
	const tref other = D == S[0] ? S[1] : S[0];
	REQUIRE(all_positive(D));
	REQUIRE(!all_positive(other));
	REQUIRE(tau::subtree_less(D, other));
	const tref got = expanded(f);
	check_against_source(got, f);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("X6: every member of the disjunct is expanded, and the block "
	"resolves") {
	// L18's clause: the floor builds one case per member of the disjunct
	// it takes, each re-entering the ladder, and every case resolves.
	fixture f = make("ex x ((xy = 0 || xw = 0) && !(xz = 0) "
		"&& (xz = 0 || xk = 0)).");
	const tref got = expanded(f);
	check_against_source(got, f);
	check_resolved(got, f);
}

TEST_CASE("X7: a complement that is not conjoinable is no exclusion") {
	// The smaller member `¬(x·a = 0) ∧ x·b = 0` has the complement
	// `x·a = 0 ∨ ¬(x·b = 0)`, a disjunction with a positive member. The
	// second case is built without it, so its variable settles and the
	// case resolves.
	fixture f = make("ex x (((xa != 0 && xb = 0) "
		"|| (xc != 0 && xd = 0 && xe = 0)) && xf != 0).");
	REQUIRE(disjunctive(f).size() == 1);
	trefs ds = ap::members<node_t>(disjunctive(f)[0]);
	REQUIRE(ds.size() == 2);
	std::ranges::sort(ds, {}, ap::formula_size<node_t>);
	const tref e = ap::normalize_operators<node_t>(
		ap::canonically_factored_neg<node_t>(tau::trim_right_sibling(ds[0])));
	REQUIRE(is_child<node_t>(e, tau::wff_or));
	CHECK(!ap::is_negative_tree<node_t>(e));
	CHECK(!ap::is_conjoinable<node_t>(e));
	const tref got = expanded(f);
	check_against_source(got, f);
	check_resolved(got, f);
}

} // TEST_SUITE
