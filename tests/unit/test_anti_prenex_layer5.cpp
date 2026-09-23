// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// THE WHOLE PIPELINE with §6's CHEAP PUSH STEPS in it. Spec: anti_prenex.md
// §6 (PUSH_BLOCK's dispatcher, the two fast paths 2a and 2b,
// PUSH_OVER_DISJUNCTION, PUSH_OVER_CONJUNCTION with its scope narrowing,
// consistency check, witness step and case witness), §7 (the leaves), §3 (the
// phases around the push), invariants 3, 6, 7 and 8.
//
// Every case is one whole spec through `anti_prenexing::anti_prenex`, parsed
// with `get_nso_rr` so that the types come from inference — §7's table has no
// row for an untyped block. What every case claims, through node identity and
// `are_nso_equivalent` and never through an output string:
//  1. the output is EQUIVALENT to the input (running the pipeline on its own
//     output is not claimed to give the same node back: what a run hands back
//     re-wrapped, phase 5's `SIMPLIFY` may fold further);
//  2. INVARIANT 4 — no `bf_neq`, no negated or mirrored order operator, and a
//     `¬` only directly over a formula leaf;
//  3. binder ids are canonical (§3's phase 5 close);
//  4. no free variable ESCAPED, and nothing of §1's term representation left
//     the push: a component's close is total, so no `BDD_ID` survives.
// Then ONE SHAPE CLAIM, the strict criterion: `binder_count == 0` and the
// value for an input the ladder RESOLVES, the binder count for one that WAITS
// at the conjunction's floor, in place of §6's decomposition and expansion,
// which this module does not carry. Each case's comment names the route its
// input takes.
//
// The inputs come, and nothing else does, from the suite of the old module,
// tests/unit/test_antiprenexing.cpp, at the line each case names; every
// expectation here is worked out from the spec. Where an input is also a
// layer-4 case (tests/unit/test_anti_prenex_layer4.cpp) that case pins what
// the block DRIVER does with it and this one pins the ladder's route.
//
// Parsing note: a parsed quantifier's body runs to the RIGHT END and
// juxtaposition is conjunction, so every input keeps its parentheses.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normalizer.h"
#include "anti_prenex/anti_prenex.h"

#include <algorithm>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

tref parse(const char* sample) {
	auto rr = get_nso_rr(sample);
	REQUIRE(rr.has_value());
	return rr.value().main->get();
}

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
/// order operator anywhere, and every `¬` sits directly over a formula LEAF.
/// A leaf is an atom or a REFERENCE: `is_atomic_fm` leaves a reference out on
/// purpose (src/tau_tree_queries.tmpl.h), and NNF cannot enter one either
/// (§4, a reference is opaque), so `¬ q(y)` is as far as phase 1 pushes a
/// negation over one — which the bitvector case below is the first input to
/// produce.
bool invariant_4(tref n) {
	bool clean = true;
	auto check = [&clean](tref m) {
		const auto& t = tau::get(m);
		if (t.is(tau::bf_neq) || t.is(tau::bf_nlt) || t.is(tau::bf_nlteq)
			|| t.is(tau::bf_gt) || t.is(tau::bf_gteq)
			|| t.is(tau::bf_ngt) || t.is(tau::bf_ngteq))
				return clean = false;
		if (t.child_is(tau::wff_neg)) {
			const tref under = tau::trim_right_sibling(
				t[0].first());
			if (!is_atomic_fm<node_t>(under)
				&& !is_child<node_t>(under, tau::wff_ref))
					return clean = false;
		}
		return true;
	};
	auto all_of_it = [](tref) { return true; };
	auto up = [](tref) {};
	pre_order<node_t>(n).search_unique(check, all_of_it, up);
	return clean;
}

/// THE CLAIMS EVERY CASE MAKES. @p equivalent is false where the checker is
/// out of reach — it decides neither a formula holding an unresolved
/// reference nor bitvector content — and the case then rests on the
/// structural claims alone.
tref anti_prenexed(tref phi, bool equivalent = true) {
	const tref got = ap::anti_prenex<node_t>(phi);
	if (equivalent) CHECK(are_nso_equivalent<node_t>(got, phi));
	CHECK(invariant_4(got));
	CHECK(ap::canonicalise_binder_ids<node_t>(got) == got);
	CHECK(no_escape(got, phi));
	CHECK(!holds(got, tau::BDD_ID));
	return got;
}

/// THE BLOCK IS RESOLVED: the pipeline left no binder at all.
void resolved(tref got) { CHECK(binder_count(got) == 0); }

/// WHAT WAITS: the push reached the conjunction's floor and re-wrapped
/// (invariant 3), so @p n binders stand in the answer.
void waits(tref got, size_t n) { CHECK(binder_count(got) == n); }

} // namespace

TEST_SUITE("anti_prenex/layer5") {

// --- 1. 2a: the block distributed to every negated atom -----------------------------

TEST_CASE("L1: an all-negated formula is distributed in one pass") {
	// test_antiprenexing.cpp:169. The fast paths are tried before the
	// negative-tree arm and before 2d (§6): `P` is empty, the census
	// counts two negated leaves and nothing else, so 2a distributes `∃x`
	// to each of them — `∃_x x·y ≠ 0` is `y ≠ 0`.
	const tref got = anti_prenexed(parse("ex x (xy != 0 || xw != 0)."));
	resolved(got);
	CHECK(are_nso_equivalent<node_t>(got, parse("!(y = 0) || !(w = 0).")));
}

TEST_CASE("L2: an X-free conjunct rides outside, and 2a takes the rest") {
	// test_antiprenexing.cpp:113. `z = 0` does not touch `x`, so the
	// wrapper's strip hoists it out of the block (§6) and what is pushed
	// is L1's formula.
	const tref got = anti_prenexed(
		parse("ex x (z = 0 && (xy != 0 || xw != 0))."));
	resolved(got);
	CHECK(are_nso_equivalent<node_t>(got,
		parse("z = 0 && (!(y = 0) || !(w = 0)).")));
}

// --- 2. 2b: the positive skeleton squeezed on terms ---------------------------------

TEST_CASE("L3: a disjunction of positives is squeezed, not distributed") {
	// test_antiprenexing.cpp:64. Every leaf is a positive equation and
	// none is X-free, so the census lets 2b through before the `∨` arm is
	// ever reached: the skeleton squeezes on TERMS to `{x·y, x·w}`, and
	// each one-atom clause is discharged at `x := 0`.
	const tref got = anti_prenexed(parse("ex x (xy = 0 || xw = 0)."));
	resolved(got);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("L4: a positive clause with a disjunctive conjunct squeezes") {
	// test_antiprenexing.cpp:206. `x` touches a disjunctive conjunct, so
	// neither move of the scope narrowing fires and the fast paths are
	// asked on the whole clause: `{x(y ∪ z), x(w ∪ z)}`, both `T` at
	// `x := 0`.
	const tref got = anti_prenexed(
		parse("ex x ((xy = 0 || xw = 0) && xz = 0)."));
	resolved(got);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("L5: 2b's cross product is the DNF clause count") {
	// test_antiprenexing.cpp:733. Two binary disjunctions give four
	// terms — the honest cost of 2b (invariant 8) — and each is
	// discharged at `x := 0`.
	const tref got = anti_prenexed(
		parse("ex x ((xa = 0 || xb = 0) && (xc = 0 || xd = 0))."));
	resolved(got);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("L6: a wider cross product is taken all the same") {
	// test_antiprenexing.cpp:804. Three ternary disjunctions: 27 terms,
	// and no cap refuses them — the spec sets none (§6, 2b).
	const tref got = anti_prenexed(
		parse("ex x ((xa = 0 || xb = 0 || xc = 0) "
			"&& (xd = 0 || xe = 0 || xf = 0) "
			"&& (xg = 0 || xh = 0 || xi = 0))."));
	resolved(got);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("L7: the strip leaves a positive skeleton for 2b") {
	// test_antiprenexing.cpp:97. `z = 0` rides outside the block, L3's
	// disjunction is squeezed to `T`, and a `T` leaves the ∧-join
	// (invariant 6): what is left is `z = 0` alone.
	const tref got = anti_prenexed(
		parse("ex x (z = 0 && (xy = 0 || xw = 0))."));
	resolved(got);
	CHECK(holds_var(got, "z"));
	CHECK(are_nso_equivalent<node_t>(got, parse("z = 0.")));
}

TEST_CASE("L8: a top-level positive rides into the squeeze") {
	// test_antiprenexing.cpp:128. After the strip the clause is
	// `(x·y = 0 ∨ x·w = 0) ∧ x·k = 0`: 2a is refused — the census counts
	// three positive leaves against one top-level literal — and 2b takes
	// it, `{x(y ∪ k), x(w ∪ k)}`, both `T` at `x := 0`.
	const tref got = anti_prenexed(
		parse("ex x (z = 0 && (xy = 0 || xw = 0) && xk = 0)."));
	resolved(got);
	CHECK(are_nso_equivalent<node_t>(got, parse("z = 0.")));
}

// --- 3. 2d: the block pushed per disjunct -------------------------------------------

TEST_CASE("L9: an X-free disjunct refuses both fast paths, and 2d answers") {
	// test_antiprenexing.cpp:215. `z = 0` is stripped; in what is left,
	// `w = 0` is a maximal X-FREE subtree, which the census counts in
	// `free` — that refuses 2b, and the positive leaf refuses 2a — so the
	// `∨` arm pushes per disjunct, where `∃x(x·y = 0)` is `T` and decides
	// the join.
	const tref got = anti_prenexed(
		parse("ex x ((xy = 0 || w = 0) && z = 0)."));
	resolved(got);
	CHECK(are_nso_equivalent<node_t>(got, parse("z = 0.")));
}

TEST_CASE("L10: a mixed disjunction is pushed disjunct by disjunct") {
	// test_antiprenexing.cpp:346. `z = 0` rides outside; the rest is an
	// ∨-node with one positive leaf, so it is no negative tree and the
	// census — a positive together with negatives — refuses both fast
	// paths. 2d then takes it: 2a answers the literal disjunct with
	// `y ≠ 0`, and the conjunctive one settles `x` and reaches the leaf as
	// `w′·k ≠ 0`.
	const tref got = anti_prenexed(
		parse("ex x (z = 0 && (xy != 0 || (xw = 0 && xk != 0)))."));
	resolved(got);
	CHECK(ap::members<node_t>(got).size() == 2);
	CHECK(are_nso_equivalent<node_t>(got,
		parse("z = 0 && (!(y = 0) || !(w'k = 0)).")));
}

// --- 4. the conjunction's ladder: scope narrowing -----------------------------------

TEST_CASE("L11: a mixed-sign clause settles and is pushed home") {
	// test_antiprenexing.cpp:192. Neither conjunct is disjunctive, so the
	// settle move's condition holds for `x` and `Xs = X`: the whole clause
	// goes to the leaf in one piece. `∃x(x·y = 0 ∧ x·w ≠ 0)` is
	// `y′·w ≠ 0`, witnessed by `x := y′·w`.
	const tref got = anti_prenexed(parse("ex x (xy = 0 && xw != 0)."));
	resolved(got);
	CHECK(are_nso_equivalent<node_t>(got, parse("!(y'w = 0).")));
}

TEST_CASE("L12: a conjunction of negated equations settles too") {
	// test_antiprenexing.cpp:184. A negative tree is an ∨-NODE (§1), so
	// this is not one; it is a clause with no disjunctive conjunct, which
	// the narrowing settles whole. `x := 1` witnesses it.
	const tref got = anti_prenexed(parse("ex x (xy != 0 && xw != 0)."));
	resolved(got);
	CHECK(are_nso_equivalent<node_t>(got, parse("!(y = 0) && !(w = 0).")));
}

TEST_CASE("L13: a two-variable component settles both variables at once") {
	// test_antiprenexing.cpp:899. `x·y = 0` mentions both variables, so
	// §5's partition gives ONE component of two; no member of the clause
	// is disjunctive, so the settle move takes `Xs = X` and the leaf
	// answers the whole clause. `x := 1, y := 0` satisfies it in any
	// atomless BA.
	const tref got = anti_prenexed(parse(
		"ex x ex y xy = 0 && yx = 0 && !(x|y = 0) && !(x = y)."));
	resolved(got);
	CHECK(tau::get(got).equals_T());
}

// --- 5. inputs the phases before the push already settle ----------------------------

TEST_CASE("L14: a pivot that is identically 0 folds before any block") {
	// test_antiprenexing.cpp:271. `(x ∪ y)·x′·y′` is `0` in any BA, so
	// phase 1's `SIMPLIFY` decides that atom `T` and with it the whole
	// disjunct; the binder is left over an x-free body, which phase 5
	// drops. No block is ever pushed.
	const tref got = anti_prenexed(
		parse("ex x (((x|y)x'y' = 0 || xk = 0) && z = 0)."));
	resolved(got);
	CHECK(are_nso_equivalent<node_t>(got, parse("z = 0.")));
}

TEST_CASE("L15: a pivot that is identically 1 kills its disjunct") {
	// test_antiprenexing.cpp:372. `(x ∪ y′) ∪ (x′ ∪ y)` is `1`, so its
	// equation folds to `F` at phase 1 and the disjunct dies; what is left
	// is `x·k = 0 ∧ z = 0`, where the strip hoists `z = 0` and the
	// one-atom block is discharged at `x := 0`.
	const tref got = anti_prenexed(
		parse("ex x (((x|y')|(x'|y) = 0 || xk = 0) && z = 0)."));
	resolved(got);
	CHECK(are_nso_equivalent<node_t>(got, parse("z = 0.")));
}

TEST_CASE("L16: a pin under a disjunct is taken by phase 2, not by the push") {
	// test_antiprenexing.cpp:302. `x ⊕ y = 0` pins `y`, and phase 2's DEEP
	// witness reaches it through the disjunction (§3) and rewrites that
	// disjunct to `T` — so the disjunction is gone before phase 4, and the
	// block that is left, `∃x(x·w ≠ 0)`, is a one-literal clause for the
	// leaf.
	const tref got = anti_prenexed(
		parse("ex x ex y ((x ^ y = 0 || z = 0) && xw != 0)."));
	resolved(got);
	CHECK(are_nso_equivalent<node_t>(got, parse("!(w = 0).")));
}

// --- 6. what waits at the conjunction's floor ---------------------------------------

TEST_CASE("L17: a clause no cheap step takes waits, one binder") {
	// test_antiprenexing.cpp:222 — the floor's shape. `x` sits in a
	// disjunctive conjunct, so nothing settles and there is one part; the
	// census refuses 2a (a positive inside the disjunction) and 2b (the
	// negated conjunct); the positives are consistent; neither disjunct
	// pins `x`, so there is no pin and no case pin. The block comes back
	// re-wrapped, undecided (invariant 3): §6's licensed decomposition and
	// expansion, which would take it, are not part of this module.
	const tref got = anti_prenexed(
		parse("ex x ((xy = 0 || xw = 0) && xz != 0)."));
	waits(got, 1);
	CHECK(holds(got, tau::wff_ex));
}

TEST_CASE("L18: two disjunctive conjuncts over one variable wait") {
	// test_antiprenexing.cpp:154. The same refusals as L17, with the
	// negated conjunct and a second disjunction: `¬(x·z = 0)` bars 2b and
	// the positives bar 2a, no variable settles, and no disjunct of either
	// disjunction pins `x`.
	const tref got = anti_prenexed(parse(
		"ex x ((xy = 0 || xw = 0) && !(xz = 0) && (xz = 0 || xk = 0))."));
	waits(got, 1);
}

TEST_CASE("L19: an X-free disjunct does not help a waiting clause") {
	// test_antiprenexing.cpp:231. `v = 0` is X-free, so the census counts
	// it in `free` and 2b is refused for that alone, 2a by the positive
	// beside it; `v = 0` is no pin for `x` either, so the case witness
	// declines and the clause waits.
	const tref got = anti_prenexed(
		parse("ex x ((xy = 0 || v = 0) && xk != 0)."));
	waits(got, 1);
}

TEST_CASE("L20: the same with the negated atom in the other disjunct") {
	// test_antiprenexing.cpp:383. `x·y ≠ 0` is the spine's negated
	// conjunct and `v = 0` the X-free disjunct: neither fast path, no
	// settle, no pin.
	const tref got = anti_prenexed(
		parse("ex x ((xw = 0 || v = 0) && xy != 0)."));
	waits(got, 1);
}

TEST_CASE("L21: four mixed-sign pairs wait as one clause") {
	// test_antiprenexing.cpp:334, the old module's budget case. Every
	// conjunct is disjunctive and mixed-sign, so no variable settles and
	// the census refuses both fast paths; nothing multiplies here, so the
	// whole clause comes back under one binder at LINEAR cost — where the
	// old module paid `2^4` branches.
	const tref got = anti_prenexed(
		parse("ex x ((xa = 0 || xb != 0) && (xc = 0 || xd != 0) && "
			"(xe = 0 || xf != 0) && (xg = 0 || xh != 0))."));
	waits(got, 1);
}

TEST_CASE("L22: a free variable in the pin's atom keeps the clause waiting") {
	// test_antiprenexing.cpp:362 — L16's formula with `y` FREE. `x ⊕ y = 0`
	// is then no pin phase 2 may take (it pins `x` to a free term only
	// under the disjunction, which is a CASE pin, and the other disjunct
	// `z = 0` pins nothing), so the case witness declines and the clause
	// waits.
	const tref got = anti_prenexed(
		parse("ex x ((x ^ y = 0 || z = 0) && xw != 0)."));
	waits(got, 1);
}

TEST_CASE("L23: a block of three one-variable components waits three times") {
	// test_antiprenexing.cpp:1143. No atom mentions two block variables,
	// so §5's partition gives THREE components, each pushed on its own.
	// For each of them the other components' atoms are maximal X-FREE
	// subtrees, which refuse 2b, and their positive leaves refuse 2a; no
	// variable settles, and a disjunct that mentions another component's
	// variable pins nothing. So each component re-wraps at the floor, and
	// the three binders stand.
	const tref got = anti_prenexed(parse(
		"ex x, y, z ((x a = 0 || y b = 0) && (z c = 0 || x d = 0))."));
	waits(got, 3);
}

TEST_CASE("L24: a bitvector run under a reference waits, structurally") {
	// test_antiprenexing.cpp:1556. `q(y)` is opaque (§4) and the bitvector
	// conjunct reaches no method this module carries (§7), so what the
	// census sees is `other > 0` and the clause waits. STRUCTURAL: the
	// equivalence checker decides neither an unresolved reference nor
	// bitvector content, so this case claims no escape, invariant 4 and
	// the shape.
	const tref got = anti_prenexed(
		parse("ex a all y ex z "
			"(((a:bv[2] + { 1 }:bv[2] = { 0 }:bv[2]) <-> q(y)) "
			"&& z = 0)."), false);
	waits(got, 3);
	CHECK(holds(got, tau::wff_ref));
	CHECK(!holds_var(got, "z"));
}

// --- 7. a ∀ run reaches the ladder through its dual ---------------------------------

TEST_CASE("L25: a ∀ run whose dual is a negative tree resolves") {
	// `∀x(x·y = 0 ∧ x·w = 0)` is dualised at the block boundary (§5) into
	// `∃x(x·y ≠ 0 ∨ x·w ≠ 0)`, a negative tree that 2a distributes to its
	// two leaves; the outbound negation turns `y ≠ 0 ∨ w ≠ 0` back into
	// `y = 0 ∧ w = 0` — the answer `x := 1` gives.
	const tref got = anti_prenexed(parse("all x (xy = 0 && xw = 0)."));
	resolved(got);
	CHECK(!holds(got, tau::wff_all));
	CHECK(are_nso_equivalent<node_t>(got, parse("y = 0 && w = 0.")));
}

TEST_CASE("L26: a ∀ run whose dual waits comes back as a ∀ head") {
	// The dual of this run is L17's clause — `(x·y = 0 ∨ x·w = 0) ∧
	// x·z ≠ 0` — which no cheap step takes, so the `∃` block re-wraps and
	// the outbound negation puts the `∀` head back over the matrix.
	const tref got = anti_prenexed(
		parse("all x ((xy != 0 && xw != 0) || xz = 0)."));
	waits(got, 1);
	CHECK(holds(got, tau::wff_all));
}

// --- 8. keep mode end to end --------------------------------------------------------

TEST_CASE("L27: a kept emission rides into the next step's terms") {
	// The settle move hands `∃x(x·y = 0)` to §7 as a SUB-block, and under
	// `keep_functional` `DISCHARGE` spells it instead of solving it: the
	// atom `(∀_x x·y) = 0` rides on as a conjunct of the smaller block,
	// where 2b squeezes it INTO the terms it builds. The chain then sits
	// in a BDD LEAF, which no quantification reaches (§1's leaf hazard),
	// so the elimination of those one-atom clauses under `∃y` freezes and
	// re-wraps instead of discharging (invariant 3) — two blocks, each
	// carrying the chain, where the plain run answers `T`.
	//
	// The claims are spelled out rather than taken from `anti_prenexed`:
	// what this case pins is the KEPT shape, under its own callback.
	const tref phi = parse("ex x ex y (xy = 0 && (ya = 0 || yb = 0)).");
	auto keep_all = [](tref) { return true; };
	const tref got = ap::anti_prenex<node_t>(phi, keep_all);
	CHECK(holds(got, tau::bf_fall));
	CHECK(binder_count(got) == 2);
	CHECK(invariant_4(got));
	CHECK(no_escape(got, phi));
	CHECK(!holds(got, tau::BDD_ID));
	CHECK(are_nso_equivalent<node_t>(got, phi));
	// Handed back to the DEFAULT callback, the kept answer gives what the
	// plain run gives directly: phase 1's `RESOLVE_FUNCTIONAL_PLAIN` takes
	// the chains and the blocks are pushed with nothing kept.
	CHECK(ap::anti_prenex<node_t>(got) == ap::anti_prenex<node_t>(phi));
	// Which is `T`: `x := 0, y := 0` satisfies the body.
	CHECK(tau::get(anti_prenexed(phi)).equals_T());
}

} // TEST_SUITE
