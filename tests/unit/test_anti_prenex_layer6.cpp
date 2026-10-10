// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// THE WHOLE PIPELINE with the conjunction's LICENSED DECOMPOSITION and its
// floor, the EXPANSION, in it. Spec: anti_prenex.md §6 (TRY_DECOMPOSE with
// FOLD_DECIDED's probe, DECOMPOSE_ARMS, EXPAND), §7
// (the leaves, DISCHARGE under `keep_functional`), §3 (the phases around the
// push, the time limit), invariant 5.
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
// value for an input the ladder RESOLVES. Each case's comment names the route
// its input takes.
//
// FOLD_DECIDED proves an atom `F` only in a BA with constants beyond 0 and 1
// (§6), so this suite is built over the `tau` pack of BAs, the one that
// carries `sbf`, whose constants parse as `{A}:sbf`.
//
// A case naming `L`<n> shares its input with that case of
// tests/unit/test_anti_prenex_layer5.cpp, one naming `C`<n> with
// tests/unit/test_anti_prenex_decompose.cpp, and one naming `X`<n> with
// tests/unit/test_anti_prenex_expand.cpp.
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

constexpr const char* l17 = "ex x ((xy = 0 || xw = 0) && xz != 0).";

/// A limit every check point is past, whenever it is reached: no case
/// depends on how fast it runs.
constexpr std::chrono::steady_clock::duration no_time =
	std::chrono::steady_clock::duration::zero();

} // namespace

TEST_SUITE("anti_prenex/layer6") {

TEST_CASE("M1: the inputs that reach the conjunction's floor resolve") {
	// L17–L23 and L26, whose comments give each route: no cheaper step
	// takes the clause and no atom sits at member tops in both signs, so
	// TRY_DECOMPOSE declines and EXPAND distributes; every case re-enters
	// the ladder and resolves.
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
			"&& (z c = 0 || x' d = 0) && (y' e = 0 || z' g = 0)).",
			false },
		{ "L26", "all x ((xy != 0 && xw != 0) || xz = 0).", false },
	};
	for (const row& r : rows) {
		CAPTURE(r.name);
		const tref got = anti_prenexed(parse(r.src));
		resolved(got);
		if (r.is_T) CHECK(tau::get(got).equals_T());
	}
}

TEST_CASE("M2: an expansion over an X-free member resolves") {
	// L19's clause. The X-free `v = 0` refuses 2b and the positive beside
	// it 2a, nothing pins `x`, and no atom is both-signed, so the floor
	// expands `x·y = 0 ∨ v = 0`: the case `v = 0` strips `v = 0` out of the
	// block and the case `x·y = 0` settles. The members tie in size and
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
	// C1's clause with `x` made binate. The X-free `a = 0` refuses 2b and
	// the negated one 2a, nothing pins `x`, and neither disjunct is a case
	// pin. `a = 0` sits at a member top of each disjunctive conjunct, once
	// per sign, and its cofactors weigh 4 against 7, so TRY_DECOMPOSE
	// takes it: the arms `a = 0 ∧ x′·z = 0` and `¬(a = 0) ∧ x·y = 0` push
	// to `a = 0` and `¬(a = 0)`, whose ∨-join is `T`.
	const tref got = anti_prenexed(
		parse("ex x ((a = 0 || xy = 0) && (a != 0 || x'z = 0))."));
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
	// X5b's clause. The negated `x·k = 0` refuses 2b and the positives 2a,
	// nothing pins `x`, and no atom is both-signed, so the floor expands;
	// the disjunctions tie on prio and member count, and content order
	// picks one. Either way the first case re-enters and reaches a clause
	// of positives, `x·y = 0 ∧ x·z = 0` or 2b's squeeze of `x·z = 0 ∧
	// (x·y = 0 ∨ x·w = 0)`, which is `T` at `x := 0` and decides the
	// ∨-joins.
	const tref got = anti_prenexed(
		parse("ex x ((xy = 0 || xw = 0) && (xz = 0 || xk != 0))."));
	resolved(got);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("M6: a constant-false census atom is folded, and no arm is "
	"built") {
	// C4's clause, `f = x′·A ∪ x·B` over `sbf`. Phase 1's SIMPLIFY keeps
	// `f = 0`: its sum `f` is no constant, and its cofactors on `x`, `A`
	// and `B`, differ. The census refuses both fast paths and `f = 0` pins
	// nothing (`A ∪ B ≠ 1`). TRY_DECOMPOSE's census selects `f = 0`, one
	// sign in each disjunctive conjunct, and its PROBE decides it: `A·B` is
	// a nonzero constant, so `f = 0` is `F`, and `ψ[f = 0 ↦ F]` is
	// `x·y = 0`, `T` at `x := 0`, with no arm built.
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
	// leaf's DISCHARGE asks the callback ONCE on `∀_x` of the case's
	// positive and on `∃_x` of each negative condition, as built. Each yes
	// keeps its chain SLID to x's levels, where every piece is closed and
	// folds (§1): nothing is kept, no formula binder is ever built, and the
	// answer is the plain one.
	//
	// The claims are spelled out rather than taken from `anti_prenexed`:
	// what this case pins is the keep-mode run, under its own callback.
	// A push the tables answer asks no callback.
	ap::flush_solver_dependent<node_t>();
	const tref phi = parse(l17);
	size_t chains = 0;
	auto keep_all = [&chains](tref n) {
		if (!ap::strip_chain<node_t>(n).first.empty()) ++chains;
		return true;
	};
	const tref got = ap::anti_prenex<node_t>(phi, keep_all);
	CHECK(chains > 0);
	CHECK(!holds(got, tau::bf_fall));
	CHECK(!holds(got, tau::bf_fex));
	CHECK(binder_count(got) == 0);
	CHECK(invariant_4(got));
	CHECK(no_escape(got, phi));
	CHECK(!holds(got, tau::BDD_ID));
	CHECK(are_nso_equivalent<node_t>(got, phi));
}

// --- the time limit (§3) -----------------------------------------------------------
// Each case flushes first: a push the tables answer reaches no check point.

TEST_CASE("T1: past a zero limit an input that reaches the floor comes back "
	"null") {
	// L17 reaches EXPAND (M1) and M3's clause DECOMPOSE_ARMS: the first case
	// or arm reads the clock past the limit, and the call aborts whole.
	for (const char* src : { l17,
		"ex x ((a = 0 || xy = 0) && (a != 0 || x'z = 0))." })
	{
		CAPTURE(src);
		ap::flush_solver_dependent<node_t>();
		CHECK(ap::anti_prenex<node_t>(parse(src),
			ap::keep_no_functional<node_t>, no_time) == nullptr);
	}
}

TEST_CASE("T2: an input that reaches no check point ignores a zero limit") {
	// `x` settles at once and the leaf eliminates it: no case, no arm and
	// no case-witness branch is built, so the clock is never read.
	const tref phi = parse("ex x (xy = 0 && xz != 0).");
	ap::flush_solver_dependent<node_t>();
	const tref got = ap::anti_prenex<node_t>(phi,
		ap::keep_no_functional<node_t>, no_time);
	REQUIRE(got != nullptr);
	CHECK(got == anti_prenexed(phi));
	resolved(got);
}

TEST_CASE("T3: an aborted call writes no entry for what it cut short") {
	// The inner block is eliminated, its entries written, before the outer
	// one reaches a check point and aborts. Run again without a limit, the
	// input gives what a run on flushed tables that never aborted gives.
	const tref phi = parse("ex x ((xy = 0 || xw = 0) && xz != 0 "
		"&& (ex u (ux = 0 && uv != 0))).");
	ap::flush_solver_dependent<node_t>();
	REQUIRE(ap::anti_prenex<node_t>(phi, ap::keep_no_functional<node_t>,
		no_time) == nullptr);
	const tref after = anti_prenexed(phi);
	ap::flush_solver_dependent<node_t>();
	CHECK(after == anti_prenexed(phi));
	resolved(after);
}

TEST_CASE("T4: a limit the call stays within changes nothing") {
	// An hour is past any ctest timeout, so every check point reads the
	// clock and lets the push through.
	const tref phi = parse(l17);
	ap::flush_solver_dependent<node_t>();
	const tref got = ap::anti_prenex<node_t>(phi,
		ap::keep_no_functional<node_t>, std::chrono::hours(1));
	CHECK(got == anti_prenexed(phi));
}

} // TEST_SUITE
