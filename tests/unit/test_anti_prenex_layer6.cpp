// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// THE WHOLE PIPELINE with the CONJUNCTION'S FLOOR in it. Spec:
// anti_prenex.md §6 (TRY_DECOMPOSE with FOLD_DECIDED's probe,
// DECOMPOSE_ARMS, EXPAND with its exclusive cases and its graceful
// exhaustion), §7 (the leaves, DISCHARGE under `keep_functional`), §3 (the
// phases around the push), §1 (`expand_max`, `taint_count`, cache scope),
// invariants 3 and 5.
//
// Every case is one whole spec through `anti_prenexing::anti_prenex`, parsed
// with `get_nso_rr` so that the types come from inference — §7's table has no
// row for an untyped block. What every case claims, through node identity and
// `are_nso_equivalent` and never through an output string:
//  1. the output is EQUIVALENT to the input (running the pipeline on its own
//     output is not claimed to give the same node back);
//  2. INVARIANT 4 — no `bf_neq`, no negated or mirrored order operator, and a
//     `¬` only directly over a formula leaf;
//  3. binder ids are canonical (§3's phase 5 close);
//  4. no free variable ESCAPED, and nothing of §1's term representation left
//     the push: a component's close is total, so no `BDD_ID` survives.
// Then ONE SHAPE CLAIM, the strict criterion: `binder_count == 0` and the
// value for an input the ladder RESOLVES, the binder count for one that WAITS
// on a spent expansion budget (invariant 3). The one other observable is the
// rise of `taint_count`, which is global and never reset, so a case reads its
// delta around one call. Each case's comment names the route its input takes.
//
// FOLD_DECIDED proves an atom `F` only in a BA with constants beyond 0 and 1
// (§6), so this suite is built over the `tau` pack of BAs, the one that
// carries `sbf`, whose constants parse as `{A}:sbf`.
//
// Parsing note: a parsed quantifier's body runs to the RIGHT END and
// juxtaposition is conjunction, so every input keeps its parentheses.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "normalizer.h"
#include "anti_prenex/anti_prenex.h"

#include <algorithm>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

tref parse(const char* sample) {
	// The engine behind `sbf`'s constants is set up before one is parsed;
	// the call is idempotent.
	bdd_init<Bool>();
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
/// order operator anywhere, and every `¬` sits directly over a formula LEAF —
/// an atom or a REFERENCE, which `is_atomic_fm` leaves out and NNF cannot
/// enter (§4).
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

/// THE CLAIMS EVERY CASE MAKES.
tref anti_prenexed(tref phi) {
	const tref got = ap::anti_prenex<node_t>(phi);
	CHECK(are_nso_equivalent<node_t>(got, phi));
	CHECK(invariant_4(got));
	CHECK(ap::canonicalise_binder_ids<node_t>(got) == got);
	CHECK(no_escape(got, phi));
	CHECK(!holds(got, tau::BDD_ID));
	return got;
}

/// THE BLOCK IS RESOLVED: the pipeline left no binder at all.
void resolved(tref got) { CHECK(binder_count(got) == 0); }

/// WHAT WAITS: a spent expansion budget re-wrapped what it could not build
/// (invariant 3), so @p n binders stand in the answer.
void waits(tref got, size_t n) { CHECK(binder_count(got) == n); }

/// `expand_max` for the scope of one case, restored on the way out. A
/// component's ctx copies the knob at its setup, so the guard precedes the
/// pipeline call.
struct expand_max_guard {
	const size_t saved = ap::expand_max;
	explicit expand_max_guard(size_t v) { ap::expand_max = v; }
	~expand_max_guard() { ap::expand_max = saved; }
};

constexpr const char* l17 = "ex x ((xy = 0 || xw = 0) && xz != 0).";

} // namespace

TEST_SUITE("anti_prenex/layer6") {

TEST_CASE("M1: the inputs that reach the conjunction's floor resolve") {
	// L17–L23 and L26 of tests/unit/test_anti_prenex_layer5.cpp, whose
	// comments give each route: no cheaper step takes the clause and no
	// atom sits at member tops in both signs, so TRY_DECOMPOSE declines and
	// EXPAND distributes; every case re-enters the ladder and resolves.
	// L21's all-positive first case is `T` at `x := 0` and decides every
	// ∨-join on the way up.
	struct row { const char* name; const char* src; bool is_T; };
	const row rows[] = {
		{ "L17", l17, false },
		{ "L18", "ex x ((xy = 0 || xw = 0) && !(xz = 0) "
			"&& (xz = 0 || xk = 0)).", false },
		{ "L19", "ex x ((xy = 0 || v = 0) && xk != 0).", false },
		{ "L20", "ex x ((xw = 0 || v = 0) && xy != 0).", false },
		{ "L21", "ex x ((xa = 0 || xb != 0) && (xc = 0 || xd != 0) && "
			"(xe = 0 || xf != 0) && (xg = 0 || xh != 0)).", true },
		{ "L22", "ex x ((x ^ y = 0 || z = 0) && xw != 0).", false },
		{ "L23", "ex x, y, z ((x a = 0 || y b = 0) "
			"&& (z c = 0 || x d = 0)).", false },
		{ "L26", "all x ((xy != 0 && xw != 0) || xz = 0).", false },
	};
	for (const row& r : rows) {
		CAPTURE(r.name);
		const tref got = anti_prenexed(parse(r.src));
		resolved(got);
		if (r.is_T) CHECK(tau::get(got).equals_T());
	}
}

TEST_CASE("M2: an exclusive expansion over an X-free member resolves") {
	// L19's clause. The X-free `v = 0` refuses 2b and the positive beside
	// it 2a, nothing pins `x`, and no atom is both-signed, so the floor
	// expands `x·y = 0 ∨ v = 0`: the case `v = 0` strips `v = 0` out of the
	// block, the case `x·y = 0` settles, and the later case carries the
	// earlier member's complement, a literal. The members tie in size and
	// content order picks the first; either way the answer is an ∨ of the
	// two cases.
	const tref got = anti_prenexed(
		parse("ex x ((xy = 0 || v = 0) && xk != 0)."));
	resolved(got);
	CHECK(are_nso_equivalent<node_t>(got,
		parse("(v = 0 && k != 0) || y'k != 0.")));
	CHECK(is_child<node_t>(got, tau::wff_or));
	CHECK(ap::members<node_t>(got).size() == 2);
}

TEST_CASE("M3: a spanning atom is decomposed, and its arms decide T") {
	// The clause of C1 in tests/unit/test_anti_prenex_decompose.cpp. The
	// X-free `a = 0` refuses 2b and the negated one 2a, nothing pins `x`,
	// and neither disjunct is a case pin. `a = 0` sits at a member top of
	// each disjunctive conjunct, once per sign, and its cofactors weigh 4
	// against 7, so TRY_DECOMPOSE takes it: the arms `a = 0 ∧ x·z = 0` and
	// `¬(a = 0) ∧ x·y = 0` push to `a = 0` and `¬(a = 0)`, whose ∨-join is
	// `T`.
	const tref got = anti_prenexed(
		parse("ex x ((a = 0 || xy = 0) && (a != 0 || xz = 0))."));
	resolved(got);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("M4: the pin arm deletes its binder") {
	// C6's clause. `x = a` is taken as M3's atom is, and its term `x + a`
	// pins `x` (cofactors `a`, `a′`), witness `a`: the T-arm is `z = 0`
	// with the binder deleted, and the F-arm `¬(x = a) ∧ y = 0` pushes to
	// `y = 0`, 2a answering `∃x(x + a ≠ 0)` with `T`.
	const tref got = anti_prenexed(
		parse("ex x ((x = a || y = 0) && (x != a || z = 0))."));
	resolved(got);
	CHECK(are_nso_equivalent<node_t>(got, parse("y = 0 || z = 0.")));
}

TEST_CASE("M5: two disjunctive conjuncts are expanded, and decide T") {
	// The clause of X5b in tests/unit/test_anti_prenex_expand.cpp. The
	// negated `x·k = 0` refuses 2b and the positives 2a, nothing pins `x`,
	// and no atom is both-signed, so the floor expands; the disjunctions
	// tie on prio and member count, and content order picks one. Either
	// way the first case re-enters and reaches a clause of positives,
	// `x·y = 0 ∧ x·z = 0` or 2b's squeeze of `x·z = 0 ∧ (x·y = 0 ∨
	// x·w = 0)`, which is `T` at `x := 0` and decides the ∨-joins.
	const tref got = anti_prenexed(
		parse("ex x ((xy = 0 || xw = 0) && (xz = 0 || xk != 0))."));
	resolved(got);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("M6: a constant-false atom is folded at the floor") {
	// C4's clause, `f = x′·A ∪ x·B` over `sbf`. Phase 1's SIMPLIFY keeps
	// `f = 0`: its sum `f` is no constant, and its cofactors on `x`, `A`
	// and `B`, differ. The census refuses both fast paths and `f = 0` pins
	// nothing (`A ∪ B ≠ 1`). At the floor TRY_DECOMPOSE's census selects
	// `f = 0`, one sign in each disjunctive conjunct, and its PROBE decides
	// it: `A·B` is a nonzero constant, so `f = 0` is `F`, and
	// `ψ[f = 0 ↦ F]` is `x·y = 0`, `T` at `x := 0`, with no arm built.
	const tref got = anti_prenexed(
		parse("ex x ((((x' & {A}:sbf) | (x & {B}:sbf)) = 0 "
			"|| x & y = 0) && (((x' & {A}:sbf) | (x & {B}:sbf)) != 0 "
			"|| x & z = 0))."));
	resolved(got);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("M7: keep mode on an expanding input stays binder-free") {
	// L17's clause under a callback that keeps every chain, on M1's route:
	// EXPAND's two cases each settle whole, and under `keep_functional` the
	// leaf's DISCHARGE emits `∀_x` of the case's positive and `∃_x` of each
	// negative condition as chains instead of quantifying — whole-block
	// emissions, free of `x`. The component's close asks the callback on
	// each chain, which keeps it, and the finish spells its body: no formula
	// binder is ever built, and both kinds of chain stand in the answer.
	//
	// The claims are spelled out rather than taken from `anti_prenexed`:
	// what this case pins is the KEPT shape, under its own callback.
	const tref phi = parse(l17);
	auto keep_all = [](tref) { return true; };
	const tref got = ap::anti_prenex<node_t>(phi, keep_all);
	CHECK(holds(got, tau::bf_fall));
	CHECK(holds(got, tau::bf_fex));
	CHECK(binder_count(got) == 0);
	CHECK(invariant_4(got));
	CHECK(no_escape(got, phi));
	CHECK(!holds(got, tau::BDD_ID));
	// The checker cannot decide kept chains; the default callback resolves
	// them in phase 1 (§3 RESOLVE_FUNCTIONAL_PLAIN).
	CHECK(are_nso_equivalent<node_t>(ap::anti_prenex<node_t>(got), phi));
}

TEST_CASE("M8: a spent budget leaves one pending block, and taints") {
	// L17's clause under a budget of one case. TRY_DECOMPOSE finds no
	// both-signed atom and charges nothing; EXPAND's first case resolves,
	// and at the second the budget is spent: one taint, and the remaining
	// member waits under `∃x` beside the first member's complement, ONE
	// pending block (invariant 3).
	expand_max_guard g(1);
	// An untainted entry under this key, cached by an earlier case under the
	// default budget, would answer it without the budget ever binding (§1
	// cache scope); the flush makes it a miss where the tables exist.
	ap::flush_solver_dependent<node_t>();
	const tref phi = parse(l17);
	const size_t before = ap::taint_count<node_t>();
	const tref got = anti_prenexed(phi);
	CHECK(ap::taint_count<node_t>() - before == 1);
	waits(got, 1);
}

} // TEST_SUITE
