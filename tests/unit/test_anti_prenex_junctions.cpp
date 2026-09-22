// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/push/junctions.h: §6's push over a
// disjunction (2d) and the conjunction's ladder — scope narrowing, the fast
// paths, the consistency check, the witness step, the case witness and the
// floor. Spec: anti_prenex.md §6 (PUSH_OVER_DISJUNCTION,
// PUSH_OVER_CONJUNCTION, INCIDENCE), §3 (TRY_WITNESS, TRY_CASE_WITNESS, the
// result joins), §7 (ELIMINATE_BLOCK), invariants 1, 3, 6, 7 and 8.
//
// The harness is the push suite's: parse `∃P.ψ`, normalise `ψ` the way phases
// 1 and 3 leave it, put it into §1's term representation and hand the block
// to the step under test, which is called DIRECTLY on the prepared clause.
// What is claimed of a result: EQUIVALENT to `∃P.ψ` (`are_nso_equivalent` on
// the finished formula), no free variable ESCAPED, INVARIANT 4, and per case
// the block RESOLVED or exactly the RE-WRAP.
//
// EVERY RE-ENTRY of these steps goes through `push_block`, whose `∧` and `∨`
// arms re-wrap; `push/push_block.h` is the hub that wires the steps into that
// dispatcher. So a junction a step hands back comes back around the narrowed
// block, which is what the shapes below expect.
//
// TYPES: §7's table has no row for an untyped block, so every fixture carries
// typed variables, read off the parsed block through inference.
//
// Parsing note: a parsed quantifier's body runs to the RIGHT END and
// juxtaposition is conjunction, so every input keeps its parentheses.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normalizer.h"
#include "anti_prenex/push/push_block.h"
#include "anti_prenex/push/junctions.h"
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

/// `case_max` for the scope of one case, restored on the way out. A `ctx`
/// copies the knob at setup, so a fixture made under the guard carries it.
struct case_max_guard {
	const size_t saved = ap::case_max;
	explicit case_max_guard(size_t v) { ap::case_max = v; }
	~case_max_guard() { ap::case_max = saved; }
};

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

fixture make(const char* src, bool keep = false) {
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
		find_ba_type<node_t>(f.P[0]), keep);
	f.clause = ap::prepare_terms<node_t>(normalised(n), f.P, f.c.order);
	return f;
}

/// In KEEP MODE the component's close (§5) run over what the step gave back:
/// one quantification per chain over the stored BDD, then every remaining
/// backed term spelled out. @p emitted takes the result BEFORE the close,
/// where the chains are still there.
tref closed(fixture& f, tref r, tref* emitted) {
	if (emitted) *emitted = r;
	if (!f.c.keep_functional) return r;
	return ap::finish_terms<node_t>(
		ap::resolve_functional_quantifiers_bdd<node_t>(r, f.c.order,
			ap::keep_no_functional<node_t>));
}

/// 2d on the prepared clause.
tref pushed_or(fixture& f, tref* emitted = nullptr) {
	return closed(f, ap::push_over_disjunction<node_t>(f.clause, f.P, f.c),
		emitted);
}

/// The conjunction's ladder on the prepared clause.
tref pushed_and(fixture& f, tref* emitted = nullptr) {
	return closed(f, ap::push_over_conjunction<node_t>(f.clause, f.P, f.c),
		emitted);
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

/// `∃P.ψ` re-wrapped whole — what the floor gives back for a conjunction no
/// step reaches (invariant 3). The formula is re-wrapped AS IT IS: no step
/// re-assembles it.
tref rewrapped(const fixture& f) {
	return ap::rewrap<node_t>(f.clause, f.P);
}

/// The conjuncts of `f.clause` that hold the block variable @p v.
trefs conjuncts_of(const fixture& f, tref v) {
	trefs out;
	for (tref m : ap::members<node_t>(f.clause))
		if (ap::fv_meets<node_t>(m, ap::block{ v })) out.push_back(m);
	return out;
}

} // namespace

TEST_SUITE("anti_prenex/junctions") {

// --- 2d -------------------------------------------------------------------------

TEST_CASE("J1: a disjunct that resolves to T decides the join") {
	// `∃x.(x·y = 0)` is `∀_x x·y = 0`, which is `0 = 0`: the first
	// disjunct pushed decides the ∨-join, and the rest are not pushed at
	// all (invariant 7). That laziness is not observable from the outside;
	// the result is.
	fixture f = make("ex x (x y = 0 || x w = 0).");
	REQUIRE(is_child<node_t>(f.clause, tau::wff_or));
	const tref got = pushed_or(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("J2: a disjunct no step resolves comes back re-wrapped beside the "
	"resolved one") {
	// `∃x(x·y ≠ 0)` is `y ≠ 0`; the conjunction beside it re-enters
	// `push_block`, whose `∧` arm re-wraps it over `x` alone. `∃X` is
	// distributed either way, which is 2d.
	fixture f = make("ex x (x y != 0 || (x a = 0 && x b != 0)).");
	REQUIRE(is_child<node_t>(f.clause, tau::wff_or));
	REQUIRE(ap::members<node_t>(f.clause).size() == 2);
	const tref got = pushed_or(f);
	check_against_source(got, f);
	REQUIRE(is_child<node_t>(got, tau::wff_or));
	const trefs ms = ap::members<node_t>(got);
	REQUIRE(ms.size() == 2);
	// One member is the resolved literal, the other the re-wrapped block.
	CHECK(binder_count(got) == 1);
	size_t wrapped = 0, literals = 0;
	for (tref m : ms) {
		if (binder_count(m) == 1) ++wrapped;
		else if (ap::is_negated_equation<node_t>(m)) ++literals;
	}
	CHECK(wrapped == 1);
	CHECK(literals == 1);
}

// --- scope narrowing: the split ---------------------------------------------------

TEST_CASE("N1: the split pushes each part under its own variables") {
	// `∃x∃y(x·a = 0 ∧ y·b = 0)` is `∃x(x·a = 0) ∧ ∃y(y·b = 0)`: no block
	// variable holds the two conjuncts together, and each part is one
	// atom that discharges to `T`.
	fixture f = make("ex x ex y (x a = 0 && y b = 0).");
	REQUIRE(ap::incidence<node_t>(ap::members<node_t>(f.clause),
		f.P).parts.size() == 2);
	const tref got = pushed_and(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("N2: a part takes only the variables of its own component") {
	// The four `x` conjuncts are one part and `y·b = 0` another. The `y`
	// part discharges to `T` and drops out of the ∧-join; the `x` part is
	// a conjunction, which re-enters `push_block` and comes back wrapped
	// over `x` ALONE — `y` is not in its block.
	fixture f = make("ex x ex y (x a = 0 && x' a = 0 && x a' = 0 "
		"&& x' a' = 0 && y b = 0).");
	REQUIRE(f.P.size() == 2);
	REQUIRE(ap::incidence<node_t>(ap::members<node_t>(f.clause),
		f.P).parts.size() == 2);
	// The positives of the `x` part squeeze to `1`, so §7 answers `F` for
	// it: this is the part an eager `F` would decide the whole ∧-join on.
	const ap::block x{ f.P[0] };
	const tref part = ap::simplified_and_join<node_t>(
		conjuncts_of(f, f.P[0]));
	REQUIRE(tau::get(ap::eliminate_block<node_t>(part, x, f.c)).equals_F());
	const tref got = pushed_and(f);
	check_against_source(got, f);
	REQUIRE(binder_count(got) == 1);
	CHECK(same(ap::binder_var<node_t>(got), f.P[0]));
	CHECK(!ap::fv_meets<node_t>(got, ap::block{ f.P[1] }));
}

// --- scope narrowing: the settle move ---------------------------------------------

TEST_CASE("N3: a settled variable leaves through its own sub-block") {
	// `x` touches no disjunctive conjunct, so its scope is final:
	// `∃x∃y(x·y = 0 ∧ D) = ∃y(∃x(x·y = 0) ∧ D)`. The sub-block discharges
	// to `T`, and what is left is pushed on under `y` alone — a
	// disjunction, which re-wraps.
	fixture f = make("ex x ex y (x y = 0 && (y a = 0 || y b = 0)).");
	REQUIRE(f.P.size() == 2);
	const ap::incidence_result<node_t> inc =
		ap::incidence<node_t>(ap::members<node_t>(f.clause), f.P);
	REQUIRE(inc.parts.size() == 1);
	REQUIRE(inc.settled.size() == 1);
	REQUIRE(same(inc.settled[0], f.P[0]));
	const tref got = pushed_and(f);
	check_against_source(got, f);
	REQUIRE(binder_count(got) == 1);
	CHECK(same(ap::binder_var<node_t>(got), f.P[1]));
	CHECK(!ap::fv_meets<node_t>(got, ap::block{ f.P[0] }));
}

TEST_CASE("N4: a clause whose ∨-nodes settle nothing is pushed home") {
	// `Xs = X`, the degenerate case: the whole conjunction is one clause
	// for §7, which resolves it. The same formula the dispatcher's `∧` arm
	// re-wraps on its own.
	fixture f = make("ex x (x y = 0 && x w != 0).");
	const ap::incidence_result<node_t> inc =
		ap::incidence<node_t>(ap::members<node_t>(f.clause), f.P);
	REQUIRE(inc.settled.size() == f.P.size());
	const tref got = pushed_and(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(!same(got, rewrapped(f)));
}

TEST_CASE("N5: in keep mode the settled sub-block emits its chain") {
	// `DISCHARGE` under `ctx.keep_functional` spells the sub-block instead
	// of solving it: `∃x(x·y = 0)` comes back as the atom `(∀_x x·y) = 0`,
	// a chain over the stored BDD, which rides on as a conjunct of the
	// smaller block. The component's close resolves it, and what is left
	// is the answer the plain mode gives.
	fixture plain = make("ex x ex y (x y = 0 && (y a = 0 || y b = 0)).");
	fixture kept = make("ex x ex y (x y = 0 && (y a = 0 || y b = 0)).",
		true);
	const tref want = pushed_and(plain);
	tref emitted = nullptr;
	const tref got = pushed_and(kept, &emitted);
	CHECK(holds(emitted, tau::bf_fall));
	CHECK(!same(emitted, want));
	check_against_source(got, kept);
	CHECK(!holds(got, tau::bf_fall));
	CHECK(!holds(got, tau::bf_fex));
	CHECK(are_nso_equivalent<node_t>(finished(got), finished(want)));
}

// --- the fast paths through the conjunction ---------------------------------------

TEST_CASE("F1: a narrowed clause that settles nothing goes to the fast paths") {
	// Every variable of the block touches a disjunctive conjunct, so
	// neither move of the narrowing fires and 2b takes the clause: the
	// positive skeleton is squeezed on TERMS into four one-atom clauses.
	fixture f = make("ex x ((x a = 0 || x b = 0) && (x c = 0 || x d = 0)).");
	const ap::incidence_result<node_t> inc =
		ap::incidence<node_t>(ap::members<node_t>(f.clause), f.P);
	REQUIRE(inc.parts.size() == 1);
	REQUIRE(inc.settled.empty());
	REQUIRE(ap::try_fast_paths<node_t>(f.clause, f.P, f.c).has_value());
	const tref got = pushed_and(f);
	check_against_source(got, f);
	check_resolved(got, f);
}

// --- the consistency check --------------------------------------------------------

TEST_CASE("C1: positives with no common zero in the block decide F") {
	// The four positives squeeze to `1`, so `∃x.⋀P` is `F` — joint over
	// every positive and the whole block, which no leaf sees on its own.
	// Neither fast path takes the clause: 2a is refuted by the positives
	// inside the disjunction (`pos > |P|`) and 2b by the negative.
	fixture f = make("ex x (x a = 0 && x' a = 0 && x a' = 0 && x' a' = 0 "
		"&& x e != 0 && (x c = 0 || x d = 0)).");
	const ap::incidence_result<node_t> inc =
		ap::incidence<node_t>(ap::members<node_t>(f.clause), f.P);
	REQUIRE(inc.parts.size() == 1);
	REQUIRE(inc.settled.empty());
	REQUIRE(!ap::try_fast_paths<node_t>(f.clause, f.P, f.c).has_value());
	const tref got = pushed_and(f);
	CHECK(tau::get(got).equals_F());
	check_against_source(got, f);
}

// --- the witness step -------------------------------------------------------------

TEST_CASE("W1: a pin at spine level substitutes its variable away") {
	// `x = a` pins `x` strictly, so `∃x∃y.ψ` is `∃y.ψ[x ← a]`: the
	// remaining conjunction re-enters `push_block` under `y` alone and
	// re-wraps there. The step is called on a prepared body, so phase 2's
	// deep pass never ran over this pin.
	fixture f = make("ex x ex y (x = a && (x y = 0 || y w = 0) "
		"&& y b != 0).");
	REQUIRE(f.P.size() == 2);
	// Nothing above the witness step takes the clause: both variables sit
	// in the disjunction, 2a is refuted by the positives inside it and 2b
	// by the negative, and the one positive conjunct is consistent.
	REQUIRE(ap::incidence<node_t>(ap::members<node_t>(f.clause),
		f.P).settled.empty());
	REQUIRE(!ap::try_fast_paths<node_t>(f.clause, f.P, f.c).has_value());
	REQUIRE(ap::try_witness<node_t>(f.P[0], f.clause, f.c).has_value());
	const tref got = pushed_and(f);
	check_against_source(got, f);
	REQUIRE(binder_count(got) == 1);
	CHECK(same(ap::binder_var<node_t>(got), f.P[1]));
	CHECK(!ap::fv_meets<node_t>(got, ap::block{ f.P[0] }));
}

// --- the case witness -------------------------------------------------------------

TEST_CASE("K1: a guarded assignment is witnessed branch by branch") {
	// `∃x.(D ∧ R) = ⋁ᵢ (dᵢ ∧ R)[x ← tᵢ]` (§3): each branch pins `x`, so
	// each copy is binder-free and the block is gone. The pin becomes its
	// branch's residual, which is `T` for a strict one.
	fixture f = make("ex x (((c = 0 && x = a) || (c' = 0 && x = b)) "
		"&& x y = 0).");
	// The pins sit under the disjunction, where no step above the case
	// witness sees them: `x·y = 0` pins nothing on its own.
	REQUIRE(ap::incidence<node_t>(ap::members<node_t>(f.clause),
		f.P).settled.empty());
	REQUIRE(!ap::try_fast_paths<node_t>(f.clause, f.P, f.c).has_value());
	REQUIRE(!ap::try_witness<node_t>(f.P[0], f.clause, f.c).has_value());
	auto cw = ap::try_case_witness<node_t>(f.P[0], f.clause, f.c);
	REQUIRE(cw.has_value());
	REQUIRE(cw->branches.size() == 2);
	const tref got = pushed_and(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got),
		parse("(c = 0 && a y = 0) || (c' = 0 && b y = 0).")));
}

TEST_CASE("K2: a branch that decides T leaves the later copies unbuilt") {
	// `x = 0` is the smallest branch, and `(x = 0 ∧ x·y = 0)[x ← 0]` is
	// `T`: the ∨-join is decided before the second branch is substituted
	// at all. What that laziness saves is not observable; the result is.
	fixture f = make("ex x ((x = 0 || (c' = 0 && x = b)) && x y = 0).");
	REQUIRE(ap::try_case_witness<node_t>(f.P[0], f.clause,
		f.c).has_value());
	const tref got = pushed_and(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("K3: `ctx.case_max` refuses a wider case pin, and the floor "
	"re-wraps") {
	// The knob is read off the component's ctx, which copies it at setup,
	// so the fixture is made under the guard. With no case pin left, no
	// step of the ladder takes the clause.
	case_max_guard cap(1);
	fixture f = make("ex x (((c = 0 && x = a) || (c' = 0 && x = b)) "
		"&& x y = 0).");
	REQUIRE(f.c.case_max == 1);
	CHECK(!ap::try_case_witness<node_t>(f.P[0], f.clause,
		f.c).has_value());
	const tref got = pushed_and(f);
	check_against_source(got, f);
	CHECK(same(got, rewrapped(f)));
	CHECK(binder_count(got) == 1);
}

// --- the floor --------------------------------------------------------------------

TEST_CASE("R1: a conjunction no step takes re-wraps exactly") {
	// `x` sits in a disjunction, so nothing settles; 2a is refuted by the
	// positives in it and 2b by the negative; the positives are
	// consistent; neither disjunct pins `x`, so there is no pin and no
	// case pin. What is left is the re-wrap (invariant 3).
	fixture f = make("ex x ((x y = 0 || x w = 0) && x z != 0).");
	const ap::incidence_result<node_t> inc =
		ap::incidence<node_t>(ap::members<node_t>(f.clause), f.P);
	REQUIRE(inc.settled.empty());
	REQUIRE(!ap::try_fast_paths<node_t>(f.clause, f.P, f.c).has_value());
	REQUIRE(!ap::try_witness<node_t>(f.P[0], f.clause, f.c).has_value());
	REQUIRE(!ap::try_case_witness<node_t>(f.P[0], f.clause,
		f.c).has_value());
	const tref got = pushed_and(f);
	check_against_source(got, f);
	CHECK(same(got, rewrapped(f)));
	CHECK(binder_count(got) == 1);
}

} // TEST_SUITE
