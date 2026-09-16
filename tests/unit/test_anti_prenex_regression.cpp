// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The REGRESSION CORPUS of the anti-prenexing module: inputs taken from the
// suites of `src/antiprenexing/` that encode behaviour `anti_prenex.md` still
// wants. Spec: §3 (the pipeline), §4, invariant 4.
//
// Every case runs ONLY `anti_prenexing::anti_prenex` (anti_prenex.h) and
// names, in one line, the suite and case its input comes from; the spec is
// the reference, that suite only the source of the input. Cases that pin
// another module's own entry points, knobs or intermediate steps are not
// carried over — the spec's push steps are tested where they are defined —
// and an expectation that was an output STRING is replaced by the semantic
// claim it stands for: which variables are gone, which binder survives.
//
// TWO TIERS PER CASE.
//  ACTIVE, checked on every build: the output is EQUIVALENT to the input (by
//  `are_nso_equivalent` on pure inputs, by GROUND TRUTH — substitute a
//  constant for the one free variable and normalise the closed instance —
//  where the checker is undecidable, and not at all where neither applies,
//  which each such case says); no free variable ESCAPED (`FV(out) ⊆ FV(in)`);
//  INVARIANT 4 (no `bf_neq`, no negated or mirrored order operator, a `¬`
//  only directly over an atom); and IDEMPOTENCE by tref.
//  DORMANT, written and guarded: "the quantifier is actually eliminated" and
//  the shapes that depend on it. Each carries `resolved_from_layer(N)`
//  against the `built_layer` constant below, so an assertion switches on with
//  the machinery it needs: 4 the block driver, 5 the cheap push steps, 6
//  decomposition and `EXPAND`, 8 the bitvector method.
//
// Parsing note: a parsed quantifier's body runs to the RIGHT END and
// juxtaposition is conjunction, so every input keeps its parentheses.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normalizer.h"
#include "anti_prenex/anti_prenex.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

/// The layer this module is built to. The dormant tier is keyed on it.
/// Raising it switches on the assertions that layer makes true.
constexpr int built_layer = 2;

/// A DORMANT claim's guard: true once the layer that makes it true is built.
bool resolved_from_layer(int n) { return n <= built_layer; }

tref parse(const char* sample) {
	auto rr = get_nso_rr(sample);
	REQUIRE(rr.has_value());
	return rr.value().main->get();
}

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// Is there a node of this kind anywhere in `n`?
bool holds(tref n, size_t nt) {
	bool found = false;
	auto check = [&found, nt](tref m) {
		if (tau::get(m).is(nt)) return found = true, false;
		return true;
	};
	auto all_of_it = [](tref) { return true; };
	auto up = [](tref) {};
	pre_order<node_t>(n).search_unique(check, all_of_it, up);
	return found;
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

/// INVARIANT 4 over a whole formula.
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

/// How a case's meaning is checked.
enum class meaning {
	equivalence,   ///< `are_nso_equivalent`, on pure content
	ground_truth,  ///< the caller checks it by constant substitution
	structural     ///< neither applies (references, bitvector content)
};

/// THE ACTIVE TIER, for every kept case.
tref anti_prenexed(tref in, meaning how = meaning::equivalence) {
	tref got = ap::anti_prenex<node_t>(in);
	if (how == meaning::equivalence)
		CHECK(are_nso_equivalent<node_t>(got, in));
	CHECK(no_escape(got, in));
	CHECK(invariant_4(got));
	CHECK(ap::anti_prenex<node_t>(got) == got);
	return got;
}

/// The named free variable of `fm` as the PARSER built it — a node built by
/// hand would differ by type inference and `replace` would miss it.
tref var_of(tref fm, const char* name) {
	for (tref v : get_free_vars<node_t>(fm))
		if (tau::get(v).to_str() == name) return v;
	return nullptr;
}

/// GROUND TRUTH of a result with one free variable left: substitute the
/// constant and read the normalised closed instance. `T` and `F` are both
/// definite, which is what makes this a check where `are_nso_equivalent` has
/// nothing to say.
bool holds_at(tref res, tref var, bool one) {
	const size_t ty = tau::get(var).get_ba_type();
	tref g = rewriter::replace<node_t>(res, var,
		one ? tau::_1_trimmed(ty) : tau::_0_trimmed(ty));
	return tau::get(normalize_non_temp<node_t>(tau::reget(g))).equals_T();
}

} // namespace

TEST_SUITE("anti_prenex/regression") {

// --- AntiPrenexBlock0Arg: the one-argument pipeline on pure BA ---------------------

TEST_CASE("R1: a formula without a quantifier is returned unchanged") {
	// AntiPrenexBlock0Arg / "quantifier-free formula is returned
	// unchanged": §3's entry test.
	tref fm = parse("xy = 0 && wz = 0.");
	CHECK(ap::anti_prenex<node_t>(fm) == fm);
}

TEST_CASE("R2: a pin eliminates its binder and lands in the sibling") {
	// AntiPrenexBlock0Arg / "subs_elim: ex x (xy=0 && x=w) → wy=0": the
	// witness step, which is phase 2 (§3 TRY_WITNESS_DEEP). The claim: `x`
	// is gone and `w` has taken its place.
	tref got = anti_prenexed(parse("ex x (xy = 0 && x = w)."));
	CHECK(!holds(got, tau::wff_ex));
	CHECK(!holds_var(got, "x"));
	CHECK(holds_var(got, "w"));
}

TEST_CASE("R3: a lone pin leaves T") {
	// AntiPrenexBlock0Arg / "subs_elim: ex x (x=w) → T": the body IS the
	// pin, so what is left is the empty join (§3, the one-member spine).
	tref got = anti_prenexed(parse("ex x (x = w)."));
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("R4: excluded middle under a ∀ binder folds to T") {
	// AntiPrenexBlock0Arg / "all-block dualization: all x (x=0 || x!=0)":
	// phase 3 spells the disequation as a formula negation and the result
	// join's complement rule decides the disjunction (§3).
	tref got = anti_prenexed(parse("all x (x = 0 || x != 0)."));
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("R5: a ∀ over an unsatisfiable body is F") {
	// AntiPrenexBlock0Arg / "all-block dualization: all x (xy!=0) → F":
	// `x := 0` refutes it, which the block driver decides.
	tref got = anti_prenexed(parse("all x xy != 0."));
	if (resolved_from_layer(4)) {
		CHECK(tau::get(got).equals_F());
		CHECK(!holds(got, tau::wff_all));
	}
}

TEST_CASE("R6: an independent conjunct leaves the block's scope") {
	// AntiPrenexBlock0Arg / "ex block conjunction decomposition": `wz = 0`
	// does not touch `x`, so it rides outside, and `∃x.(xy = 0)` is T
	// (`x := 0`). Scope narrowing and the block's own elimination are
	// §6/§7.
	tref got = anti_prenexed(parse("ex x (xy = 0 && wz = 0)."));
	if (resolved_from_layer(4)) {
		CHECK(!holds(got, tau::wff_ex));
		CHECK(!holds_var(got, "x"));
		CHECK(holds_var(got, "w"));
		CHECK(holds_var(got, "z"));
	}
}

TEST_CASE("R7: a pin under a disjunction still witnesses") {
	// AntiPrenexBlock0Arg / "trivial_skolem wiring: ex x (x=w || z=0)":
	// `x := w` witnesses the left disjunct whatever `z` is, so the whole
	// formula is T. The descent reaches the atom through the other
	// connective and rewrites it as a one-member spine (§3).
	tref got = anti_prenexed(parse("ex x (x = w || z = 0)."));
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("R8: a mixed block — one variable pins, the other needs the push") {
	// AntiPrenexBlock0Arg / "trivial_skolem wiring: mixed block falls back
	// safely": `x` occurs once, in `x = c` under a disjunction; `y` occurs
	// in two disjuncts, which only the decomposition can take apart.
	tref got = anti_prenexed(
		parse("ex x ex y ((x = c || z = 0) && (yz = 0 || yw = 0))."));
	CHECK(!holds_var(got, "x"));
	if (resolved_from_layer(6)) {
		CHECK(!holds(got, tau::wff_ex));
		CHECK(!holds_var(got, "y"));
	}
}

TEST_CASE("R9: every block of an alternating nest is processed") {
	// AntiPrenexBlock0Arg / "nested alternation: every block is
	// processed": the inner ∃ block and the enclosing ∀ block both.
	tref got = anti_prenexed(parse("all a ex b (ab = 0 && bc != 0)."));
	if (resolved_from_layer(4)) CHECK(binder_count(got) == 0);
}

TEST_CASE("R10: a binder over a constant scope is dropped") {
	// AntiPrenexBlock0Arg / "no quantifier survives a constant scope":
	// `∃x.((x ∪ y)·x′·y′ ≠ 0)` is F for every `y`, so the `∀y` above it
	// ends up over a constant — which `FOLD_DEGENERATE_BINDERS` drops (§3,
	// phase 5). ACTIVE, not dormant: the term is identically `0`, so
	// SIMPLIFY folds the atom and both binders sit over a constant — no
	// block has to be pushed for this one.
	tref got = anti_prenexed(parse("all y ex x ((x|y)x'y' != 0)."));
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_F());
}

// --- Gamma4Guard: nothing escapes its scope ---------------------------------------

TEST_CASE("R11: no block variable escapes its scope") {
	// Gamma4Guard / "no block variable escapes its scope": an atom still
	// mentioning another block variable must not be lifted out of the
	// binders. That claim IS the active tier — `FV(out) ⊆ FV(in)` — so
	// these five inputs are kept for exactly it.
	for (const char* s : {
		"ex x ex y (((x|y')(x'|y') = 0 || w = 0) && x y != 0).",
		"ex x ex y ((y|y')x = 0 && x y != 0).",
		"ex x ex y (((y|y')x = 0 || w = 0) && x y != 0).",
		"ex x ex y ((y|y')(x|z) = 0 && x y != 0).",
		"ex x ex y ex z ((z|z')(x y) = 0 && x y != 0)." })
	{
		CAPTURE(s);
		anti_prenexed(parse(s));
	}
}

// --- PureBaBvEliminability: the two halves of the bitvector rule ------------------

TEST_CASE("R12: a pure-BA scope over bitvector variables") {
	// PureBaBvEliminability / "pure-BA bv scope is eliminated in Tau":
	// `∃x.(x ∪ y = 0)` is `y = 0` by Boole's expansion, in ANY BA, so the
	// bitvector type is no reason to keep the binder. Checked
	// STRUCTURALLY: `are_nso_equivalent` is undecidable on bitvector
	// content and the ground-truth substitution needs a one-variable
	// result, which this is not. The elimination itself is the bitvector
	// method's (§7).
	tref got = anti_prenexed(
		parse("ex x (x:bv[2] | y:bv[2] = { 0 }:bv[2])."),
		meaning::structural);
	if (resolved_from_layer(8)) {
		CHECK(!holds(got, tau::wff_ex));
		CHECK(!holds_var(got, "x"));
	}
}

TEST_CASE("R13: an atomic-BA counterexample is never answered T") {
	// PureBaBvEliminability / "atomic-BA counterexample: a bv[1] negated
	// pair is not distributed": `∃x.(x ≠ 0 ∧ x ≠ 1)` is UNSAT over a
	// two-element BA, so anything that distributes the negated pair would
	// wrongly answer T. A soundness claim, checked on every build.
	tref got = anti_prenexed(
		parse("ex x (x:bv[1] != { 0 }:bv[1] && x:bv[1] != { 1 }:bv[1])."),
		meaning::structural);
	CHECK_FALSE(tau::get(got).equals_T());
}

// --- references freeze only their own component ----------------------------------

TEST_CASE("R14: a block over only reference-entangled variables survives") {
	// FrozenBlockNormalization / "a block over only ref-entangled
	// variables survives verbatim": `q(y)` is opaque (§4), so nothing can
	// witness `y` and the binder stays. Structural: a reference puts the
	// equivalence checker out of reach.
	tref got = anti_prenexed(parse("ex y (q(y) && y != 0)."),
		meaning::structural);
	CHECK(binder_count(got) == 1);
	CHECK(holds(got, tau::wff_ref));
}

TEST_CASE("R15: an eliminable variable over a frozen scope still eliminates") {
	// FrozenBlockNormalization / "an eliminable variable over a frozen
	// scope still eliminates": `z = 0` pins `z`, while `q(y)` freezes only
	// `y`'s component — the reference is not a reason to keep `z`.
	tref got = anti_prenexed(parse("ex z ex y (q(y) && z = 0)."),
		meaning::structural);
	CHECK(binder_count(got) == 1);       // only `y`'s binder survives
	CHECK(!holds_var(got, "z"));
	CHECK(holds(got, tau::wff_ref));
}

TEST_CASE("R16: the same, with the eliminable variable under a disjunction") {
	// FrozenBlockNormalization / "…, disjunctive form": every branch of
	// `z = 0 ∨ z = 1` pins `z`, which is §3's CASE PIN, while `q(y)` again
	// freezes only `y`.
	tref got = anti_prenexed(parse("ex z ex y ((z = 0 || z = 1) && q(y))."),
		meaning::structural);
	CHECK(binder_count(got) == 1);
	CHECK(!holds_var(got, "z"));
	CHECK(holds(got, tau::wff_ref));
}

TEST_CASE("R17: a frozen binder survives alone, under canonical ids") {
	// CanonicalQuantifierIds / "a frozen binder survives alone, under
	// canonical ids": `x` pins, `y` is held by the reference `f(y)`.
	// Exactly one binder survives and the reference is intact.
	tref got = anti_prenexed(parse("ex x, y (x = 0 && y w = 0 && f(y))."),
		meaning::structural);
	CHECK(!holds_var(got, "x"));
	CHECK(binder_count(got) == 1);
	CHECK(holds(got, tau::wff_ref));
}

// --- ground truth by constant substitution ---------------------------------------

TEST_CASE("R18: the unique-zero shape keeps the pivot's negation") {
	// Gamma1NegatedBranch / "the unique-zero fold keeps the pivot's
	// negation": `∃q.(q ≠ 0 ∧ (a·q = 0 ∨ q = 0))` ≡ `a′ ≠ 0` — T at a = 0,
	// F at a = 1. A result that drops the `q ≠ 0` constraint along with
	// the atom it lives in answers T at a = 1 too, which the ground truth
	// catches whatever the shape of the result.
	const char* sample = "ex q (!(q = 0) && (a q = 0 || q = 0)).";
	tref in = parse(sample);
	tref got = anti_prenexed(in, meaning::ground_truth);
	tref a = var_of(in, "a");
	REQUIRE(a != nullptr);
	CHECK(holds_at(got, a, false));
	CHECK(!holds_at(got, a, true));
}

TEST_CASE("R19: the five-conjunct interpreter shape stays sound") {
	// Gamma1NegatedBranch / "the five-conjunct interpreter shape stays
	// sound": a shape from the issue #70 step system, reduced by delta
	// debugging. Same truth table as R18, for both values of `s`.
	const char* sample =
		"ex q (!(q = 0) && (a q = 0 || q = 0) && "
		"(!(q = a) || !(s' = 0) || a q = q || !(q = 0)) && "
		"(!(a' = 0) || !(s' = 0) || a q = a || a = q || !(q = 0)) && "
		"(!(a q = q) || !(s' = 0) || q = a || a q = 0 || q = 0)).";
	tref in = parse(sample);
	tref got = anti_prenexed(in, meaning::ground_truth);
	tref a = var_of(in, "a");
	tref s = var_of(in, "s");
	REQUIRE(a != nullptr);
	REQUIRE(s != nullptr);
	const size_t st = tau::get(s).get_ba_type();
	tref r0 = rewriter::replace<node_t>(got, s, tau::_0_trimmed(st));
	tref r1 = rewriter::replace<node_t>(got, s, tau::_1_trimmed(st));
	CHECK(holds_at(r0, a, false));
	CHECK(!holds_at(r0, a, true));
	CHECK(holds_at(r1, a, false));
	CHECK(!holds_at(r1, a, true));
}

// --- leaf_clause: a disequation is never silently dropped -------------------------

TEST_CASE("R20: a disequation is not dropped, single variable") {
	// leaf_clause / "a disequation is not silently dropped, single
	// variable": `∃x.(x·a = 0 ∧ ¬(x·b = 0))` is not a tautology; an answer
	// of T would mean the disequation was folded into the positive
	// squeeze. A soundness claim, checked on every build.
	tref got = anti_prenexed(parse("ex x (x a = 0 && !(x b = 0))."));
	CHECK_FALSE(tau::get(got).equals_T());
}

TEST_CASE("R21: a disequation is not dropped, two-variable block") {
	// leaf_clause / "a disequation is not silently dropped, block": the
	// same claim reached through a block.
	tref got = anti_prenexed(
		parse("ex x, y (x y a = 0 && !(x y b = 0))."));
	CHECK_FALSE(tau::get(got).equals_T());
}

TEST_CASE("R22: both spellings of a disequation give the same answer") {
	// leaf_clause / "the bf_neq spelling gives the same answer as
	// !(= 0)": phase 3's `NORMALIZE_OPERATORS` is what makes the two
	// spellings one node (§3, invariant 4).
	tref a = anti_prenexed(parse("ex x (x a = 0 && x b != 0)."));
	tref b = anti_prenexed(parse("ex x (x a = 0 && !(x b = 0))."));
	CHECK(same(a, b));
}

TEST_CASE("R23: an independent conjunct is lifted out of the binder") {
	// leaf_clause / "an independent conjunct is lifted out of the
	// binder": `w = 0` does not touch `x`, and the block that is left is
	// fully eliminable. Scope narrowing is §6.
	tref got = anti_prenexed(parse("ex x (x a = 0 && w = 0)."));
	CHECK(holds_var(got, "w"));
	if (resolved_from_layer(4)) {
		CHECK(!holds(got, tau::wff_ex));
		CHECK(!holds_var(got, "x"));
	}
}

} // TEST_SUITE
