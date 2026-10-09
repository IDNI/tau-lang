// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// THE WHOLE PIPELINE with §7's BITVECTOR ROUTER in it. Spec: anti_prenex.md
// §7 (ELIMINATE_BITVECTOR_CLAUSE with SPLIT_ARITHMETIC and SOLVE_ARITHMETIC;
// ELIMINATE_FINITE_CLAUSE through its tiers; DECIDE_FINITE with its node
// budget; ASK), §6 (the push steps that reach the leaf), §5
// (the components and the ∀ run's dualisation), §3 (the phases around the
// push), §1 (`qbf_node_max`, `taint_count`), invariant 3.
//
// Every case is one whole spec through `anti_prenexing::anti_prenex`, parsed
// with `get_nso_rr`, every variable and constant typed where it occurs. The
// ring sum is `^`; `+` is arithmetic, which the solver's cases use on
// purpose. What every case claims, never through an output string:
//  1. the output is EQUIVALENT to the input by THE BV ORACLE, the solver
//     asked whether their exclusive-or is unsatisfiable (running the
//     pipeline on its own output is not claimed to give the same node back);
//  2. INVARIANT 4 — no `bf_neq`, no negated or mirrored order operator, and a
//     `¬` only directly over a formula leaf;
//  3. binder ids are canonical (§3's phase 5 close);
//  4. no free variable ESCAPED, and no `BDD_ID` survives.
// Then ONE SHAPE CLAIM, THE STRICT CRITERION: every bitvector input without a
// reference, a chain-carrying unit or an open query neither engine decides
// is binder-free, with its value where the route fixes one; an input holding
// such a query WAITS, and its binder count is claimed (invariant 3). The one
// other observable is the delta of `taint_count`, global and never reset.
// Each case's comment names the steps its input takes, per the spec.
//
// A case naming F<n> shares its input with that case of
// tests/unit/test_anti_prenex_finite.cpp, V<n> with
// tests/unit/test_anti_prenex_bitvector.cpp, and R<n> with
// tests/unit/test_anti_prenex_regression.cpp.
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

using th = term_handle<node_t>;

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

/// The formula with every stored BDD spelled out, which is what the oracle
/// reads.
tref finished(tref n) { return th::convert_to_tau_terms(n); }

/// THE ORACLE: `a` and `b` agree on every assignment of their free
/// variables, asked as the unsatisfiability of their exclusive-or.
bool bv_equivalent(tref a, tref b) {
	const tref A = finished(a), B = finished(b);
	const tref differ = tau::build_wff_or(
		tau::build_wff_and(A, tau::build_wff_neg(B)),
		tau::build_wff_and(tau::build_wff_neg(A), B));
	return bv_formula_sat_status<node_t>(differ) == bv_sat_status::unsat;
}

/// THE CLAIMS EVERY CASE MAKES.
tref bv_anti_prenexed(tref phi) {
	const tref got = ap::anti_prenex<node_t>(phi);
	CHECK(bv_equivalent(got, phi));
	CHECK(invariant_4(got));
	CHECK(ap::canonicalise_binder_ids<node_t>(got) == got);
	CHECK(no_escape(got, phi));
	CHECK(!holds(got, tau::BDD_ID));
	return got;
}

/// THE BLOCK IS RESOLVED: the pipeline left no binder at all.
void resolved(tref got) { CHECK(binder_count(got) == 0); }

/// WHAT WAITS: an open query neither engine decides is re-wrapped
/// (invariant 3), so @p n binders stand in the answer.
void waits(tref got, size_t n) { CHECK(binder_count(got) == n); }

/// `qbf_node_max` for the scope of one case, restored on the way out. A
/// component's ctx copies the knob at its setup, so the guard precedes the
/// pipeline call.
struct qbf_node_max_guard {
	const size_t saved = ap::qbf_node_max;
	explicit qbf_node_max_guard(size_t v) { ap::qbf_node_max = v; }
	~qbf_node_max_guard() { ap::qbf_node_max = saved; }
};

/// F7's bv[2] shape: `x ≠ 0 ∧ x ≠ 3` leaves the elements 1 and 2.
const char* const f7_bv2 = "ex x (!(x:bv[2] = { 0 }:bv[2]) "
	"&& !(x:bv[2] = { 3 }:bv[2])).";

/// F3's clause, `x·a = 0 ∧ x·b ≠ 0`.
const char* const f3 = "ex x (x:bv[2] & a:bv[2] = { 0 }:bv[2] "
	"&& !(x:bv[2] & b:bv[2] = { 0 }:bv[2])).";

} // namespace

TEST_SUITE("anti_prenex/layer7") {

// --- the budget (first: the sweep's universe is append-only) --------------------

TEST_CASE("N1: a spent sweep budget hands the query to the solver, untainted") {
	// F7's bv[2] shape. First in the file: the sweep's universe only grows,
	// so a zero budget binds only on a sweep that allocates. No atom is
	// disjunctive, so the settle move takes the block home (§6); the router
	// hands both negatives to the finite side, whose tier-2 query is closed.
	// The sweep binds at once and the solver answers `sat`: `T`, and no
	// taint (§7 DECIDE_FINITE).
	qbf_node_max_guard g(0);
	const size_t before = ap::taint_count<node_t>();
	const tref got = bv_anti_prenexed(parse(f7_bv2));
	resolved(got);
	CHECK(tau::get(got).equals_T());
	CHECK(ap::taint_count<node_t>() == before);
	// N4 asks the same query and has to reach the sweep.
	ap::flush_solver_dependent<node_t>();
}

// --- the pure side ----------------------------------------------------------------

TEST_CASE("N2: a weak pin like R12's resolves before the method dispatch") {
	// Phase 2 takes a strict spelled pin alone, so the atom reaches the
	// push — `x` is binate, so it does not leave at block entry — where 2b
	// hands the one-atom clause to ELIMINATE_BLOCK. Its witness pass reads
	// the cofactors of `x·a ∪ x′·(a′ ∪ y)` on `x`, `a′ ∪ y` and `a`, whose
	// union is `1`: a pin, witness `a′`, residual `a·y = 0` (§3
	// TRY_WITNESS).
	const tref got = bv_anti_prenexed(
		parse("ex x ((x:bv[2] & a:bv[2]) "
			"| (x:bv[2]' & (a:bv[2]' | y:bv[2])) = { 0 }:bv[2])."));
	resolved(got);
	CHECK(!holds_var(got, "x"));
	CHECK(bv_equivalent(got, parse("a:bv[2] & y:bv[2] = { 0 }:bv[2].")));
}

TEST_CASE("N3: the finite method's tiers 0 and 1 resolve end to end") {
	// F2 is R12's shape, whose `x` only lowers its truth, so it leaves at
	// block entry (§5). F3 and F4 pin nothing: no
	// conjunct is disjunctive but a negative tree, so the settle move takes
	// the block home (§6), the router hands the pure clause to the finite
	// side, and tier 1 emits `pos` — here `∀_x x·a = 0`, i.e. `T` — and one
	// condition per literal of the one negative.
	struct row { const char* name; const char* src; const char* value; };
	const row rows[] = {
		{ "F2", "ex x (x:bv[2] | a:bv[2] = { 0 }:bv[2]).",
			"a:bv[2] = { 0 }:bv[2]." },
		{ "F3", f3, "!(a:bv[2]' & b:bv[2] = { 0 }:bv[2])." },
		{ "F4", "ex x (x:bv[2] & a:bv[2] = { 0 }:bv[2] "
			"&& (!(x:bv[2] & b:bv[2] = { 0 }:bv[2]) "
			"|| !(x:bv[2] & c:bv[2] = { 0 }:bv[2]))).",
			"!(a:bv[2]' & b:bv[2] = { 0 }:bv[2]) "
			"|| !(a:bv[2]' & c:bv[2] = { 0 }:bv[2])." },
	};
	for (const row& r : rows) {
		CAPTURE(r.name);
		const tref got = bv_anti_prenexed(parse(r.src));
		resolved(got);
		CHECK(bv_equivalent(got, parse(r.value)));
	}
}

TEST_CASE("N4: F7's closed tier-2 queries resolve to their values") {
	// Two negatives and no positive: the settle move takes the block home,
	// the router hands it to the finite side, and tier 2 asks one closed
	// query, which the sweep decides. Over bv[1] (R13's input) no third
	// element is left; over bv[2] the elements 1 and 2 are.
	const size_t before = ap::taint_count<node_t>();
	const tref r13 = bv_anti_prenexed(parse(
		"ex x (x:bv[1] != { 0 }:bv[1] && x:bv[1] != { 1 }:bv[1])."));
	resolved(r13);
	CHECK(tau::get(r13).equals_F());
	const tref bv2 = bv_anti_prenexed(parse(f7_bv2));
	resolved(bv2);
	CHECK(tau::get(bv2).equals_T());
	CHECK(ap::taint_count<node_t>() == before);
}

TEST_CASE("N5: a ∀ run whose dual is tier 1 resolves") {
	// `∀x (x·a ≠ 0 ∨ x·b = 0)` is dualised at the block boundary (§5) into
	// F3's clause, which tier 1 answers with `a′·b ≠ 0`; the outbound
	// negation turns it into `a′·b = 0` — the answer `x := a′` gives.
	const tref got = bv_anti_prenexed(parse(
		"all x (!(x:bv[2] & a:bv[2] = { 0 }:bv[2]) "
		"|| x:bv[2] & b:bv[2] = { 0 }:bv[2])."));
	resolved(got);
	CHECK(!holds(got, tau::wff_all));
	CHECK(bv_equivalent(got, parse("a:bv[2]' & b:bv[2] = { 0 }:bv[2].")));
}

TEST_CASE("N6: F8's open tier-2 queries, closed both ways") {
	SUBCASE("valid: T") {
		// Four elements, at most two excluded: closed by `∀a` the query
		// holds.
		const tref got = bv_anti_prenexed(parse(
			"ex x (!(x:bv[2] = { 0 }:bv[2]) "
			"&& !(x:bv[2] = a:bv[2]))."));
		resolved(got);
		CHECK(tau::get(got).equals_T());
	}
	SUBCASE("neither: the block waits") {
		// F8's clause with `x` complemented in its second literal, so
		// that `x` is binate and reaches the query. Not valid (`a = 0`),
		// satisfiable (`a = x = 1`, `b = 2`): the query re-wraps.
		const tref got = bv_anti_prenexed(parse(
			"ex x (!(x:bv[2] & a:bv[2] = { 0 }:bv[2]) "
			"&& !(x:bv[2]' & b:bv[2] = { 0 }:bv[2]))."));
		waits(got, 1);
	}
}

TEST_CASE("N7: F10's unit is swallowed into the query") {
	// F10's input with `y′` in its last atom, so that `y` is binate. The
	// `∀y` block goes first (§4, post-order): its dual `∃y (y·x ≠ 0 ∧
	// y′ ≠ 0)` is a tier-2 query open in `x`, neither valid nor
	// unsatisfiable, so its unit stands. The `∃x` block's clause is pure:
	// the router hands it to the finite method, which swallows the unit
	// into its tier-2 query, unsatisfiable since the unit forces `x = 0`.
	const tref got = bv_anti_prenexed(parse(
		"ex x (!(x:bv[2] & a:bv[2] = { 0 }:bv[2]) "
		"&& all y (y:bv[2] & x:bv[2] = { 0 }:bv[2] "
		"|| y:bv[2]' = { 0 }:bv[2]))."));
	resolved(got);
	CHECK(tau::get(got).equals_F());
}

// --- the solver side --------------------------------------------------------------

TEST_CASE("N8: V3's closed arithmetic resolves through the solver") {
	// `x` sits under an arithmetic operator, so nothing pins it; the router
	// hands the atom to the solver, and its closed query decides.
	SUBCASE("satisfiable: T") {
		// `x = 3` is `−1` modulo 4.
		const tref got = bv_anti_prenexed(parse(
			"ex x (x:bv[2] + { 1 }:bv[2] = { 0 }:bv[2])."));
		resolved(got);
		CHECK(tau::get(got).equals_T());
	}
	SUBCASE("unsatisfiable: F") {
		// Adding `1` changes every value.
		const tref got = bv_anti_prenexed(parse(
			"ex x (x:bv[2] + { 1 }:bv[2] = x:bv[2])."));
		resolved(got);
		CHECK(tau::get(got).equals_F());
	}
}

TEST_CASE("N9: V6's mixed block resolves, one component per engine") {
	// No atom mentions both `x` and `y`: two components (§5). The `x` one is
	// the solver's, open and valid (`x = −a`); the `y` one is the finite
	// method's closed tier-2 query, `T` over bv[2].
	const tref got = bv_anti_prenexed(parse(
		"ex x, y (x:bv[2] + a:bv[2] = { 0 }:bv[2] "
		"&& !(y:bv[2] = { 0 }:bv[2]) && !(y:bv[2] = { 3 }:bv[2]))."));
	resolved(got);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("N10: V4's undecided arithmetic block waits") {
	// `x·1 ∈ {0, 1}`, so `a ∈ {0, 1}` alone satisfies the atom: the solver's
	// open query is neither valid nor unsatisfiable, and re-wraps.
	const tref got = bv_anti_prenexed(parse(
		"ex x ((x:bv[2] & { 1 }:bv[2]) + a:bv[2] = { 1 }:bv[2])."));
	waits(got, 1);
}

// --- keep mode --------------------------------------------------------------------

TEST_CASE("N11: keep mode on F3's input keeps its chains") {
	// N3's route under a callback that keeps every chain: tier 1's
	// DISCHARGE emits `∀_x` of the positive and `∃_x` of the condition as
	// chains instead of quantifying, whole-block emissions free of `x`; the
	// component's close keeps them and the finish spells their bodies.
	const tref phi = parse(f3);
	auto keep_all = [](tref) { return true; };
	const tref got = ap::anti_prenex<node_t>(phi, keep_all);
	CHECK((holds(got, tau::bf_fall) || holds(got, tau::bf_fex)));
	CHECK(binder_count(got) == 0);
	CHECK(invariant_4(got));
	CHECK(no_escape(got, phi));
	CHECK(!holds(got, tau::BDD_ID));
	// The solver reads no chain: the plain re-run resolves them first.
	const tref plain = ap::resolve_functional_quantifiers_plain<node_t>(got);
	CHECK(!holds(plain, tau::bf_fall));
	CHECK(!holds(plain, tau::bf_fex));
	CHECK(bv_equivalent(plain, phi));
}

TEST_CASE("N12: a bv variable that only lowers the truth leaves at block entry") {
	// x·a = 0 falls as x grows and so does ¬(x′·b = 0): §5 deletes x at
	// x := 0 before any component, which leaves ¬(b = 0).
	const tref phi = parse("ex x (x:bv[8] & a:bv[8] = { 0 }:bv[8] "
		"&& !(x:bv[8]' & b:bv[8] = { 0 }:bv[8])).");
	const tref matrix = tau::trim_right_sibling(ap::binder_body<node_t>(phi));
	const ap::block X{ tau::trim_right_sibling(ap::binder_var<node_t>(phi)) };
	const tref got = ap::delete_unate_binders<node_t>(matrix, X);
	CHECK(!ap::fv_meets<node_t>(got, X));
	CHECK(bv_equivalent(got, phi));
	CHECK(bv_equivalent(got, parse("!(b:bv[8] = { 0 }:bv[8]).")));
	resolved(bv_anti_prenexed(phi));
}

TEST_CASE("N13: a compound guard is split on as one unit") {
	// No atom is both-signed at the member tops of the two disjunctive
	// conjuncts, but the guard `a = 0 ∧ b = 0` is, its neg the group
	// `¬(a = 0) ∨ ¬(b = 0)` of the second: TRY_DECOMPOSE splits on the unit,
	// the arms push to the guard and its neg, and their ∨-join is `T`.
	const tref got = bv_anti_prenexed(parse(
		"ex x (((a:bv[2] = { 0 }:bv[2] && b:bv[2] = { 0 }:bv[2]) "
		"|| x:bv[2] & y:bv[2] = { 0 }:bv[2]) "
		"&& (!(a:bv[2] = { 0 }:bv[2]) || !(b:bv[2] = { 0 }:bv[2]) "
		"|| x:bv[2]' & z:bv[2] = { 0 }:bv[2]))."));
	resolved(got);
	CHECK(tau::get(got).equals_T());
}

} // TEST_SUITE
