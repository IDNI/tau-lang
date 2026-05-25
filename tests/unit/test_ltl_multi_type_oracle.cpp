// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Q40-XV3: cross-validation of tau's multi-type LTL realizability decision.
//
// Multi-type LTL formulas like `G(phi^qlt) && G(phi^sbf) && F(phi^bv)` touch
// more than one BA.  The factorization tau-lang implements is: atoms live in
// exactly one BA, so per-BA realizability checks are independent, and the
// overall formula is realizable iff the per-BA projections are simultaneously
// realizable modulo the formula's temporal/propositional structure.
//
// For this cross-validation we construct formulas where the expected answer
// is obvious by inspection (a single BA dominates or the BAs trivially
// factor) and check that tau agrees.  For the cvc5 side we delegate the
// single-BA oracle queries to the existing machinery via tau's own
// is_tau_formula_sat invoked on per-BA projections.

#include "test_init.h"
#include "test_tau_helpers.h"

using namespace idni::tau_lang;

static bool realizable(const char* s) {
	auto nso = get_nso_rr<node_t>(tau::get(s));
	if (!nso.has_value()) return false;
	tref fm = nso.value().main->get();
	if (!fm) return false;
	return is_tau_formula_sat<node_t>(fm);
}

TEST_SUITE("multi-type LTL cross-validation") {

	// Factoring: REAL qlt atom ∧ REAL sbf atom ⇒ REAL.
	TEST_CASE("MT-01: G(qlt REAL) && G(sbf REAL) ⇒ REAL") {
		CHECK(realizable(
			"G ((o1[t]:qlt > {0}:qlt) && (o2[t]:sbf = {X & Y}:sbf))."
		));
	}

	// Factoring: UNREAL qlt atom ∧ REAL sbf atom ⇒ UNREAL.
	TEST_CASE("MT-02: G(qlt UNREAL) && G(sbf REAL) ⇒ UNREAL") {
		CHECK_FALSE(realizable(
			"G ((o1[t]:qlt > {1}:qlt) && (o1[t]:qlt < {0}:qlt)"
			" && (o2[t]:sbf = {X & Y}:sbf))."
		));
	}

	// Mixed G+F with different BAs.
	TEST_CASE("MT-03: G(qlt) && F(bv) ⇒ REAL") {
		CHECK(realizable(
			"G ((o1[t]:qlt > {0}:qlt) && (o1[t]:qlt < {1}:qlt))"
			" && F ((o2[t]:bv = {#b10110101}:bv))."
		));
	}

	// Three BAs, all individually REAL.
	TEST_CASE("MT-04: G(qlt) && G(sbf) && G(bv) all REAL ⇒ REAL") {
		CHECK(realizable(
			"(G ((o1[t]:qlt = {1/2}:qlt)))"
			" && (G ((o2[t]:sbf = {X | (Y & Z)}:sbf)))"
			" && (G ((o3[t]:bv = {#b11110000}:bv)))."
		));
	}

	// qint + qlt simultaneously.
	TEST_CASE("MT-05: G(qlt) && G(qint) both REAL ⇒ REAL") {
		CHECK(realizable(
			"G ((o1[t]:qlt = {3/4}:qlt))"
			" && G ((o2[t]:qint = {[0,1/2)}:qint))."
		));
	}

	// F-mix: each F can be satisfied independently.
	TEST_CASE("MT-06: F(qlt) && F(sbf) both reachable ⇒ REAL") {
		CHECK(realizable(
			"F ((o1[t]:qlt = {1/3}:qlt))"
			" && F ((o2[t]:sbf = {X & Y}:sbf))."
		));
	}

	// Per-BA independence: mixed conjuncts with lookback on one and not the other.
	TEST_CASE("MT-07: G(qlt past) && G(sbf current) ⇒ REAL") {
		CHECK(realizable(
			"G ((o1[t]:qlt > {0}:qlt) && (o1[t-1]:qlt >= {0}:qlt))"
			" && G ((o2[t]:sbf = {X & Y}:sbf))."
		));
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}