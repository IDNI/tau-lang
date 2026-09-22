// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// THE WHOLE PIPELINE, the phase-4 push included. Spec:
// anti_prenex.md §3 (ANTI_PRENEX's phase list), §4 (PROCESS_ALL_BLOCKS,
// COLLECT_RUN), §5 (PROCESS_BLOCK, PUSH_EX_BLOCK), §6 (PUSH_BLOCK's
// dispatcher), §1's `keep_functional` row, invariants 3 and 4.
//
// Every case is one whole spec through `anti_prenexing::anti_prenex`, parsed
// with `get_nso_rr` so that the types come from inference — §7's table has no
// row for an untyped block. What every case claims, through node identity
// and `are_nso_equivalent` and never through an output string:
//  1. the output is EQUIVALENT to the input, and running the pipeline again
//     returns the SAME node (idempotence by tref);
//  2. INVARIANT 4 — no `bf_neq`, no negated or mirrored order operator, and a
//     `¬` only directly over an atom;
//  3. binder ids are canonical (§3's phase 5 close);
//  4. no free variable ESCAPED, and nothing of §1's term representation left
//     the push: a component's close is total, so no `BDD_ID` survives.
// Then its own SHAPE claim: what the block driver does to that input, worked
// out from the spec and spelled in the case's comment.
//
// WHAT RESOLVES: a block over one literal, one negative tree (§1: an ∨-node
// all of whose leaves are negated equations) or one binder unit. Every other
// disjunction and every conjunction RE-WRAPS, after the strip of its X-free
// conjuncts — the push over a junction is not part of the module, and an
// undecided block comes back wrapped rather than answered (invariant 3).
//
// Some cases take their INPUT STRING, and nothing else, from
// tests/unit/test_antiprenexing.cpp; every expectation here is the spec's. A
// case naming `R`<n> shares its input with that case of the regression
// corpus, tests/unit/test_anti_prenex_regression.cpp.
//
// Parsing note: a parsed quantifier's body runs to the RIGHT END and
// juxtaposition is conjunction, so every input keeps its parentheses.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normalizer.h"
#include "anti_prenex/anti_prenex.h"

#include <algorithm>
#include <vector>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

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

bool holds_var(tref n, const char* name) {
	for (tref v : get_free_vars<node_t>(n))
		if (tau::get(v).to_str() == name) return true;
	return false;
}

/// NO FREE VARIABLE ESCAPED: `FV(out) ⊆ FV(in)`. A block variable that left
/// its scope would show up here as a new free variable.
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

/// The formula under the outermost @p k binders of `n`.
tref matrix_of(tref n, size_t k) {
	while (k-- > 0) {
		REQUIRE(is_child_quantifier<node_t>(n));
		n = tau::trim_right_sibling(ap::binder_body<node_t>(n));
	}
	return n;
}

/// THE PIPELINE WITHOUT ITS PUSH: §3's phases in order, phase 4 left out. A
/// run the push only RE-WRAPS gives its formula back as the node it arrived
/// as — a component's close spells its terms in the plain regime's normal
/// form (§3 `FINISH_TERMS`), the form the phases around it use — so a result
/// that equals this came out of phase 4 untouched.
tref without_the_push(tref phi) {
	tref n = ap::canonicalise_binder_ids<node_t>(phi);
	n = ap::resolve_functional_quantifiers_plain<node_t>(n,
		ap::keep_no_functional<node_t>);
	n = ap::to_canonically_factored_nnf<node_t>(n);
	n = ap::simplify<node_t>(n, {}, true);
	n = ap::eliminate_by_substitution<node_t>(n);
	n = ap::simplify<node_t>(n);
	n = ap::normalize_operators<node_t>(n);
	n = ap::simplify<node_t>(n);
	n = ap::fold_degenerate_binders<node_t>(n);
	return ap::canonicalise_binder_ids<node_t>(n);
}

/// A RE-WRAPPED RUN: one binder over the same junction with the same members,
/// and the block variable still bound under it.
void rewrapped_over(tref got, size_t junction, size_t member_count) {
	REQUIRE(binder_count(got) == 1);
	const tref matrix = matrix_of(got, 1);
	CHECK(is_child<node_t>(matrix, junction));
	CHECK(ap::members<node_t>(matrix).size() == member_count);
	CHECK(ap::fv_meets<node_t>(matrix,
		ap::block{ tau::trim_right_sibling(
			ap::binder_var<node_t>(got)) }));
}

/// THE CLAIMS EVERY CASE MAKES. @p equivalent is false where the checker is
/// out of reach — it does not decide a formula holding an unresolved
/// reference — and the case then rests on the structural claims alone.
tref anti_prenexed(tref phi, bool equivalent = true) {
	const tref got = ap::anti_prenex<node_t>(phi);
	if (equivalent) CHECK(are_nso_equivalent<node_t>(got, phi));
	CHECK(ap::anti_prenex<node_t>(got) == got);
	CHECK(invariant_4(got));
	CHECK(ap::canonicalise_binder_ids<node_t>(got) == got);
	CHECK(no_escape(got, phi));
	CHECK(!holds(got, tau::BDD_ID));
	return got;
}

} // namespace

TEST_SUITE("anti_prenex/layer4") {

// --- 1. the three regression claims that switch on with the driver ----------------

TEST_CASE("M1: a ∀ over an unsatisfiable body is F") {
	// test_antiprenexing.cpp:423, the input of the corpus's R5. The run is
	// dualised into `∃x. x·y = 0`, one literal, which the leaf discharges
	// to `T`; the outbound negation makes that `F`.
	const tref phi = parse("all x xy != 0.");
	const tref got = anti_prenexed(phi);
	CHECK(tau::get(got).equals_F());
	CHECK(!holds(got, tau::wff_all));
}

TEST_CASE("M2: an independent conjunct leaves the block's scope") {
	// test_antiprenexing.cpp:429, the input of the corpus's R6. `w·z = 0`
	// does not touch `x`, so the strip hoists it outside the block (§6),
	// and what is left is the single literal `x·y = 0`, which the leaf
	// discharges to `T` (`x := 0`).
	const tref phi = parse("ex x (xy = 0 && wz = 0).");
	const tref got = anti_prenexed(phi);
	CHECK(!holds(got, tau::wff_ex));
	CHECK(!holds_var(got, "x"));
	CHECK(holds_var(got, "w"));
	CHECK(holds_var(got, "z"));
}

TEST_CASE("M3: the strip leaves a one-literal block behind") {
	// The corpus's R23. The same shape as M2 with the independent conjunct
	// written over one variable.
	const tref phi = parse("ex x (x a = 0 && w = 0).");
	const tref got = anti_prenexed(phi);
	CHECK(!holds(got, tau::wff_ex));
	CHECK(!holds_var(got, "x"));
	CHECK(holds_var(got, "w"));
}

// --- 2. runs, nests and alternations ----------------------------------------------

TEST_CASE("M4: an alternating nest re-wraps around its conjunctive clause") {
	// The corpus's R9, whose stronger claim needs the push over a
	// conjunction. The inner matrix is a CONJUNCTION, which re-wraps
	// (invariant 3); the ∀ run above it is dualised onto a binder UNIT,
	// which the leaf transports rather than opens, so both binders stand
	// in the answer.
	const tref phi = parse("all a ex b (ab = 0 && bc != 0).");
	const tref got = anti_prenexed(phi);
	CHECK(binder_count(got) == 2);
}

TEST_CASE("M5: nested same-kind runs are shared between phases 2 and 4") {
	// `x = w` is a pin, so phase 2's deep witness takes `x` before any
	// block is pushed; what phase 4 meets is `∃y. w·y = 0`, one literal,
	// which the leaf discharges to `T` (`y := 0`).
	const tref phi = parse("ex x ex y (xy = 0 && x = w).");
	const tref got = anti_prenexed(phi);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("M6: an alternation is eliminated innermost first") {
	// `∀b. a·b = 0` is `a = 0` — the run is dualised into `∃b. ¬(a·b = 0)`,
	// one literal, discharged to `¬(a = 0)`, and negated back. The ∃ run
	// above it then meets that literal and discharges it to `T`.
	const tref phi = parse("ex a all b (a b = 0).");
	const tref got = anti_prenexed(phi);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_T());
}

// --- 3. what re-wraps -------------------------------------------------------------

TEST_CASE("M7: a ∀ matrix that is a negative tree comes back as it went") {
	// The matrix is a negative tree, but the ∀ run is DUALISED before the
	// push, so what the dispatcher sees is the conjunction `x·y = 0 ∧
	// x·w = 0` — which re-wraps. The outbound negation restores the run.
	const tref phi = parse("all x (xy != 0 || xw != 0).");
	const tref got = anti_prenexed(phi);
	CHECK(holds(got, tau::wff_all));
	rewrapped_over(got, tau::wff_or, 2);
	// The run came back as it went, BY NODE.
	CHECK(got == without_the_push(phi));
}

TEST_CASE("M8: a disjunction matrix re-wraps unchanged") {
	// Not a negative tree — its leaves are positive — so the dispatcher's
	// junction arm re-wraps the block whole (invariant 3).
	const tref phi = parse("ex x (xy = 0 || xw = 0).");
	const tref got = anti_prenexed(phi);
	CHECK(holds(got, tau::wff_ex));
	rewrapped_over(got, tau::wff_or, 2);
	CHECK(got == without_the_push(phi));
}

TEST_CASE("M9: the strip hoists the X-free conjunct and the rest re-wraps") {
	// `v = 0` does not touch `x`, so the strip hoists it outside the block
	// (§6); the disjunctive conjunct that is left re-wraps over `x` alone.
	// What comes back is a conjunction of exactly those two.
	//
	// The X-free conjunct is written over a variable OF ITS OWN: one the
	// disjunction also mentions would pin that variable at phase 1 and
	// dissolve the disjunct before any block is pushed.
	const tref phi = parse("ex x (v = 0 && (xy = 0 || xw = 0)).");
	const tref got = anti_prenexed(phi);
	CHECK(binder_count(got) == 1);
	REQUIRE(ap::members<node_t>(got).size() == 2);
	bool free_conjunct = false, wrapped = false;
	for (tref m : ap::members<node_t>(got)) {
		if (is_child_quantifier<node_t>(m)) wrapped = true;
		else free_conjunct = true;
	}
	CHECK(free_conjunct);
	CHECK(wrapped);
}

TEST_CASE("M10: a block over a reference is frozen and re-wrapped") {
	// A reference is opaque to every step (§4) and its arguments are
	// terms, so nothing here reads into it and the block comes back
	// wrapped. STRUCTURAL: the equivalence checker does not decide a
	// formula holding an unresolved reference, so that claim is not made
	// here.
	const tref phi = parse("ex x (q(x)).");
	const tref got = anti_prenexed(phi, false);
	CHECK(binder_count(got) == 1);
	CHECK(holds(got, tau::wff_ref));
}

// --- 4. one answer per meaning, not per spelling ----------------------------------

TEST_CASE("M11: both spellings of a disequation give the same node") {
	// The corpus's R22 claim, made here through the whole pipeline:
	// phase 3's `NORMALIZE_OPERATORS` runs before the push, so the two
	// inputs reach phase 4 as one formula and leave it as one node.
	const tref a = anti_prenexed(parse("ex x (x a = 0 && x b != 0)."));
	const tref b = anti_prenexed(parse("ex x (x a = 0 && !(x b = 0))."));
	CHECK(same(a, b));
}

// --- 5. keep mode end to end ------------------------------------------------------

TEST_CASE("M12: a kept block leaves a functional quantifier, resolved later") {
	// `DISCHARGE` under `ctx.keep_functional` spells the block out instead
	// of solving it, and the component's close keeps what the callback
	// keeps: `∃x. x·y = 0` comes back as the one atom `(∀_x x·y) = 0`,
	// with no formula binder left.
	//
	// The claims are spelled out rather than taken from `anti_prenexed`:
	// idempotence has to be claimed under the SAME callback.
	const tref phi = parse("ex x (xy = 0).");
	auto keep_all = [](tref) { return true; };
	const tref got = ap::anti_prenex<node_t>(phi, keep_all);
	CHECK(holds(got, tau::bf_fall));
	CHECK(binder_count(got) == 0);
	CHECK(invariant_4(got));
	CHECK(no_escape(got, phi));
	CHECK(!holds(got, tau::BDD_ID));
	// EQUIVALENT, which the checker does answer for this chain.
	CHECK(are_nso_equivalent<node_t>(got, phi));
	// IDEMPOTENT UNDER THE SAME CALLBACK, on the FIRST run: the close spells
	// the emission's term in the plain regime's normal form (§3
	// `FINISH_TERMS`), which is the form a second run — finding no formula
	// binder, hence no component and no order — would spell it in anyway.
	CHECK(ap::anti_prenex<node_t>(got, keep_all) == got);
	// Handed back to the default callback, phase 1's
	// `RESOLVE_FUNCTIONAL_PLAIN` resolves the chain, and what is left is
	// the answer the plain run gives directly.
	CHECK(ap::anti_prenex<node_t>(got) == ap::anti_prenex<node_t>(phi));
	// Which is `T`: `x := 0` satisfies the body.
	CHECK(tau::get(anti_prenexed(phi)).equals_T());
}

// --- 6. a run under a temporal operator -------------------------------------------

TEST_CASE("M13: a block inside a temporal operator is pushed too") {
	// The pass enters a temporal operator like any other formula node, so
	// the run below one is collected and discharged.
	const tref phi = parse("always ex x (xy = 0).");
	const tref got = anti_prenexed(phi);
	CHECK(binder_count(got) == 0);
}

// --- 7. the seed corpus: single-block inputs of the old module's suite -------------

TEST_CASE("M14: a negative tree under a block is resolved whole") {
	// test_antiprenexing.cpp:161. Phase 3 spells both disequations as
	// formula negations, so the matrix is an ∨-node all of whose leaves
	// are negated equations — a NEGATIVE TREE (§1), which the dispatcher
	// classifies ahead of the junctions and hands to the leaf in one
	// piece. RESOLVED.
	const tref phi = parse("ex x (xy != 0 || xw != 0).");
	const tref got = anti_prenexed(phi);
	CHECK(binder_count(got) == 0);
	CHECK(!holds_var(got, "x"));
}

TEST_CASE("M15: a conjunction of negated equations re-wraps") {
	// test_antiprenexing.cpp:178. A negative tree is an ∨-NODE (§1), so a
	// conjunction of the same literals is not one: it takes the
	// dispatcher's junction arm and re-wraps (invariant 3).
	const tref phi = parse("ex x (xy != 0 && xw != 0).");
	const tref got = anti_prenexed(phi);
	rewrapped_over(got, tau::wff_and, 2);
	CHECK(got == without_the_push(phi));
}

TEST_CASE("M16: a mixed-sign clause re-wraps") {
	// test_antiprenexing.cpp:189. Both conjuncts mention `x`, so the strip
	// hoists nothing and the conjunction re-wraps whole.
	const tref phi = parse("ex x (xy = 0 && xw != 0).");
	const tref got = anti_prenexed(phi);
	rewrapped_over(got, tau::wff_and, 2);
	CHECK(got == without_the_push(phi));
}

TEST_CASE("M17: a contradictory pin folds the whole formula to F") {
	// test_antiprenexing.cpp:276. `x = 0` pins `x` before phase 4 ever
	// sees a block, and the sibling becomes `1 = 0`; the binder is left
	// over a constant, which phase 5 drops.
	const tref phi = parse("ex x (x = 0 && x' = 0).");
	const tref got = anti_prenexed(phi);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_F());
}

TEST_CASE("M18: the strip leaves a disjunctive clause, which re-wraps") {
	// test_antiprenexing.cpp:340. `z = 0` rides outside the block; what is
	// left is an ∨-node with a POSITIVE leaf, so it is no negative tree
	// and re-wraps over `x`.
	const tref phi = parse("ex x (z = 0 && (xy != 0 || (xw = 0 && xk != 0))).");
	const tref got = anti_prenexed(phi);
	CHECK(binder_count(got) == 1);
	CHECK(holds_var(got, "z"));
	CHECK(ap::members<node_t>(got).size() == 2);
}

TEST_CASE("M19: a pin eliminates its binder before the push") {
	// test_antiprenexing.cpp:404. Phase 2's deep witness substitutes
	// `x := w`, so no block reaches phase 4 at all.
	const tref phi = parse("ex x (xy = 0 && x = w).");
	const tref got = anti_prenexed(phi);
	CHECK(binder_count(got) == 0);
	CHECK(!holds_var(got, "x"));
	CHECK(holds_var(got, "w"));
}

TEST_CASE("M20: a two-variable clause of one component re-wraps whole") {
	// test_antiprenexing.cpp:899. `x·y = 0` mentions both variables, so
	// the partition gives ONE component of two; the clause is a mixed-sign
	// conjunction, which re-wraps, and both binders come back.
	const tref phi = parse(
		"ex x ex y xy = 0 && yx = 0 && !(x|y = 0) && !(x = y).");
	const tref got = anti_prenexed(phi);
	CHECK(binder_count(got) == 2);
}

} // TEST_SUITE
