// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Tests for the semantic pointwise revision algorithm.
//
// Tests the generalized algorithm from pwr-ltl.tex that handles
// nested-temporal LTL(ABA), not just the safety fragment.
//
// The algorithm's core property: at atomic leaves, it uses the semantic
// per-step formula ((exists o. alpha /\ beta) -> alpha) /\ beta.
//
// Test categories:
//   PWR-S-*   Safety fragment (backward compatibility)
//   PWR-T-*   Temporal operators (Until, Release, etc.)
//   PWR-V-*   Vacuity
//   PWR-I-*   Idempotence
//   PWR-M-*   Multi-clause
//   PWR-E-*   Edge cases
//   PWR-P-*   AGM properties
//   PWR-D-*   DeepSeek nontrivial

#include "test_init.h"
#include "test_tau_helpers.h"
#include "pointwise_revision.h"

using namespace idni::tau_lang;

// Parse a tau spec string and return the main wff tref.
static tref spec(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s).value_or(nullptr));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

// Every case needs the revision's value; a failed revision is a failure of
// the case, so the helper unwraps the result here.
static tref revision_value(tref s, tref u) {
	auto rr = pointwise_revision_temporal<node_t>(s, u, 0);
	REQUIRE(rr.has_value());
	return rr.value();
}

// Check if a formula is realizable (REAL oracle).
static bool is_realizable(tref fm) {
	if (!fm) return false;
	auto r = is_tau_formula_sat<node_t>(fm);
	REQUIRE(r.has_value());
	return r.value();
}

// PW-RT2: entailment oracle under synthesis semantics -- `a` entails `b`
// iff no strategy realizing `a` can violate `b`, i.e. a ∧ ¬b is
// unrealizable (the environment can only pick inputs, so a violation of
// `b` must be forced on every input sequence).
static bool entails(tref a, tref b) {
	if (!a || !b) return false;
	auto r = is_tau_formula_sat<node_t>(
		tau::build_wff_and(a, tau::build_wff_neg(b)));
	REQUIRE(r.has_value());
	return !r.value();
}

// ============================================================================
// PWR-S: Safety fragment (backward compatibility)
// ============================================================================

TEST_SUITE("[PWR-S: Safety fragment]") {

	TEST_CASE("[PWR-S-01] Compatible safety: same value") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-02] Incompatible safety: different values") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		// Update must be satisfiable in result (AGM success)
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-03] Partial: two outputs, one conflicts") {
		// o1 conflicts, but o2 should be preserved
		tref s = spec("G ((o1[t] = 0) && (o2[t] = 0)).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-04] T spec: T * G(o1=0) = update") {
		tref s = spec("T.");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-05] T update: G(o1=0) * T = spec unchanged") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("T.");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-06] Safety with negation") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (!(o1[t] = 0)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-07] Safety: spec conjunction with update") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-08] Safety: G(T) * G(o1=0) = conjunction") {
		tref s = spec("G T.");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-09] Safety with input variable") {
		tref s = spec("G (o1[t] = i1[t]).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-10] Safety: multiple outputs, no conflict") {
		tref s = spec("G ((o1[t] = 0) && (o2[t] = 1)).");
		tref u = spec("G ((o1[t] = 0) && (o2[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}
}

// ============================================================================
// PWR-T: Temporal operators (Until, Release, etc.)
// ============================================================================

TEST_SUITE("[PWR-T: Temporal operators]") {

	TEST_CASE("[PWR-T-01] Until: compatible invariants" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) until (o1[t] = 1).");
		tref u = spec("(o1[t] = 0) until (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-02] Until: incompatible invariants" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) until (o2[t] = 0).");
		tref u = spec("(o1[t] = 1) until (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-03] Release: same operator" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) release (o2[t] = 0).");
		tref u = spec("(o1[t] = 1) release (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-04] F operator: spec and update" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("F (o1[t] = 0).");
		tref u = spec("F (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-05] G wrapping F: nested temporal" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("G (F (o1[t] = 0)).");
		tref u = spec("G (F (o1[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-06] Until vs Release: operator mismatch" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) until (o2[t] = 0).");
		tref u = spec("(o1[t] = 0) release (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-07] Until: different commitments" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) until (o1[t] = 1).");
		tref u = spec("(o1[t] = 0) until (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-08] F with conjunction" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("F ((o1[t] = 0) && (o2[t] = 0)).");
		tref u = spec("F (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-09] Safety and liveness mix: G && F" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("G (o1[t] = 0) && F (o1[t] = 1).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-10] W operator" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) weak_until (o1[t] = 1).");
		tref u = spec("(o1[t] = 0) weak_until (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}
}

// ============================================================================
// PWR-V: Vacuity
// ============================================================================

TEST_SUITE("[PWR-V: Vacuity]") {

	TEST_CASE("[PWR-V-01] Unrealizable spec: return update") {
		// Spec requires contradictory values on same output
		tref s = spec("G ((o1[t] = 0) && (o1[t] = 1)).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-V-02] Unrealizable update: return update anyway") {
		// Update is unrealizable (contradictory)
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G ((o1[t] = 0) && (o1[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		// Update o1=0 && o1=1 is contradictory — revision inherits unrealizability
		CHECK_FALSE(is_realizable(result));
	}

	TEST_CASE("[PWR-V-03] Spec is F: always realizable" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("F (0 = 1).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-V-04] Both unrealizable") {
		tref s = spec("G ((o1[t] = 0) && (o1[t] = 1)).");
		tref u = spec("G ((o2[t] = 0) && (o2[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		// Both spec and update are contradictory — result is unrealizable
		CHECK_FALSE(is_realizable(result));
	}
}

// ============================================================================
// PWR-I: Idempotence
// ============================================================================

TEST_SUITE("[PWR-I: Idempotence]") {

	TEST_CASE("[PWR-I-01] Safety idempotence: spec = update") {
		const char* formula = "G (o1[t] = 0).";
		tref s = spec(formula);
		tref u = spec(formula);
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-I-02] Until idempotence" * doctest::skip(!ltlsynt_available())) {
		const char* formula = "(o1[t] = 0) until (o1[t] = 1).";
		tref s = spec(formula);
		tref u = spec(formula);
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-I-03] Release idempotence" * doctest::skip(!ltlsynt_available())) {
		const char* formula = "(o1[t] = 1) release (o1[t] = 0).";
		tref s = spec(formula);
		tref u = spec(formula);
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-I-04] Multi-clause idempotence" * doctest::skip(!ltlsynt_available())) {
		const char* formula = "G (o1[t] = 0) && F (o2[t] = 0).";
		tref s = spec(formula);
		tref u = spec(formula);
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-I-05] T idempotence") {
		const char* formula = "T.";
		tref s = spec(formula);
		tref u = spec(formula);
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}
}

// ============================================================================
// PWR-M: Multi-clause
// ============================================================================

TEST_SUITE("[PWR-M: Multi-clause]") {

	TEST_CASE("[PWR-M-01] Multi-clause update: pick best clause") {
		tref s = spec("G (o1[t] = 0).");
		// Use explicit parentheses: in Tau, G extends through || so
		// "G A || G B" parses as G(A || G B), not (G A) || (G B).
		tref u = spec("(G (o1[t] = 0)) || (G (o1[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-M-02] Multi-clause spec") {
		// Use explicit parentheses: in Tau, G extends through || so
		// "G A || G B" parses as G(A || G B), not (G A) || (G B).
		tref s = spec("(G (o1[t] = 0)) || (G (o1[t] = 1)).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-M-03] Safety and liveness clauses" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("G (o1[t] = 0) && F (o2[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-M-04] Two F clauses" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("F (o1[t] = 0) && F (o2[t] = 0).");
		tref u = spec("F (o1[t] = 1) && F (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}
}

// ============================================================================
// PWR-E: Edge cases
// ============================================================================

TEST_SUITE("[PWR-E: Edge cases]") {

	TEST_CASE("[PWR-E-01] Both T") {
		tref s = spec("T.");
		tref u = spec("T.");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-02] Deeply nested: G(F(G(o1=0)))" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("G (F (o1[t] = 0)).");
		tref u = spec("G (F (o1[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-03] Single atom spec" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("F (o1[t] = 0).");
		tref u = spec("F (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-04] Negation in update") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (!(o1[t] = 0)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-05] Input-only spec" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("F (i1[t] = 0).");
		tref u = spec("F (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-06] Until with immediate discharge" * doctest::skip(!ltlsynt_available())) {
		// o1=1 U o1=1 can discharge immediately
		tref s = spec("(o1[t] = 1) until (o1[t] = 1).");
		tref u = spec("(o1[t] = 0) until (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-07] G(F) liveness combination" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("G (F (o1[t] = 1)).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}
}

// ============================================================================
// PWR-P: AGM properties
// ============================================================================

TEST_SUITE("[PWR-P: AGM properties]") {

	TEST_CASE("[PWR-P-01] Success: result implies update") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		// Result should be realizable (update is realizable)
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-P-02] Consistency: result is realizable when update is" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("F (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-P-03] Vacuity: compatible spec+update preserved") {
		// Both can be satisfied simultaneously
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-P-04] Idempotence: spec*spec = spec") {
		const char* formula = "G (o1[t] = 0).";
		tref s = spec(formula);
		tref u = spec(formula);
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}
}

// ============================================================================
// PWR-D: DeepSeek nontrivial tests
// ============================================================================

TEST_SUITE("[PWR-D: DeepSeek nontrivial]") {

	TEST_CASE("[PWR-D-01] Until with shared commitment, conflicting invariants" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) until (o2[t] = 1).");
		tref u = spec("(o1[t] = 1) until (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-02] Release with shared invariant" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) release (o2[t] = 0).");
		tref u = spec("(o1[t] = 1) release (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-03] Cross-clause with F and G" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("G (o1[t] = 0) && F (o2[t] = 1).");
		tref u = spec("G (o1[t] = 1) && F (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-04] Until chains" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) until ((o1[t] = 1) until (o1[t] = 0)).");
		tref u = spec("(o1[t] = 1) until (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-05] Disjunctive update selection") {
		tref s = spec("G (o1[t] = 0).");
		// Use explicit parentheses: in Tau, G extends through ||.
		tref u = spec("(G (o1[t] = 0)) || (G (o1[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-06] Two outputs, two temporal operators" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("F (o1[t] = 0) && G (o2[t] = 0).");
		tref u = spec("F (o1[t] = 1) && G (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-07] Release vs Until mismatch" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) release (o2[t] = 0).");
		tref u = spec("(o1[t] = 0) until (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-08] G(o1=0) * F(o1=1): safety vs liveness" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("F (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-09] Input-dependent with temporal" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("G (o1[t] = i1[t]).");
		tref u = spec("F (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-10] Nested G F with conflicting outputs" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("G (F (o1[t] = 0)).");
		tref u = spec("G (F (o1[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-11] Multiple outputs Until" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) until ((o1[t] = 1) && (o2[t] = 0)).");
		tref u = spec("(o1[t] = 0) until (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-12] W (weak until) operator" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) weak_until (o1[t] = 1).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-13] Contradictory spec: both values at once") {
		tref s = spec("G ((o1[t] = 0) && (o1[t] = 1)).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-14] F and G on different outputs" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("F (o1[t] = 0).");
		tref u = spec("G (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-15] Revision preserves compatible G clause") {
		tref s = spec("G (o1[t] = 0) && G (o2[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-16] U then F interleaving" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) until (o1[t] = 1).");
		tref u = spec("F (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-17] Three outputs safety") {
		tref s = spec("G ((o1[t] = 0) && (o2[t] = 0) && (o3[t] = 0)).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-18] Nested Until" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 0) until ((o2[t] = 0) until (o1[t] = 1)).");
		tref u = spec("(o1[t] = 0) until (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-19] G with disjunction inside") {
		tref s = spec("G ((o1[t] = 0) || (o1[t] = 1)).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-20] F(o1=0 && o2=1) with conflicting G update" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("F ((o1[t] = 0) && (o2[t] = 1)).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}
}


// PW-N2: revise() had no case for a NON-TEMPORAL formula that is not an
// atom leaf (a Boolean combination of atoms): such a spec side against an
// atom update fell through to the operator-mismatch case and was dropped
// wholesale, so an obligation the update did not touch was forgotten.
TEST_SUITE("[PWR-C: boolean-structured clauses]") {

	// spec: for i1 = 1 emit o1 = 1, for i1 = 0 emit o2 = 1.
	// update: o1 = 0 -- conflicts only at i1 = 1.
	// Pointwise revision must keep the i1 = 0 branch's obligation o2 = 1.
	TEST_CASE("[PWR-C-01] disjunctive spec keeps its untouched obligation") {
		tref s = spec("G((i1[t] = 1 && o1[t] = 1) || (i1[t] = 0 && o2[t] = 1)).");
		tref u = spec("G(o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref r = revision_value(s, u);
		REQUIRE(r != nullptr);
		CHECK(is_realizable(r));
		// AGM success: the update holds
		CHECK(entails(r, u));
		// the obligation at the cooperating input survives
		tref obligation = spec("G(i1[t] = 0 -> o2[t] = 1).");
		REQUIRE(obligation != nullptr);
		CHECK(entails(r, obligation));
	}

	// Two conflicting updates in sequence: the first revision's retained
	// obligation must survive the second revision too (no amnesia).
	TEST_CASE("[PWR-C-02] sequential revisions keep earlier obligations") {
		tref s = spec("G((i1[t] = 1 && o1[t] = 1) || (i1[t] = 0 && o2[t] = 1)).");
		tref u1 = spec("G(o1[t] = 0).");
		tref u2 = spec("G(i1[t] = 1 -> o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u1 != nullptr);
		REQUIRE(u2 != nullptr);
		tref r1 = revision_value(s, u1);
		REQUIRE(r1 != nullptr);
		tref r2 = revision_value(r1, u2);
		REQUIRE(r2 != nullptr);
		CHECK(is_realizable(r2));
		CHECK(entails(r2, u2));
		tref obligation = spec("G(i1[t] = 0 -> o2[t] = 1).");
		REQUIRE(obligation != nullptr);
		CHECK(entails(r2, obligation));
		// and the first update is still honoured where u2 allows it
		tref kept = spec("G(i1[t] = 0 -> o1[t] = 0).");
		REQUIRE(kept != nullptr);
		CHECK(entails(r2, kept));
	}

	// Case 3 generalised: a non-temporal Boolean spec side against a
	// binary temporal update is lifted like an atom, not dropped.
	TEST_CASE("[PWR-C-03] boolean spec vs until update is revised, not dropped"
		* doctest::skip(!ltlsynt_available())) {
		tref s = spec("G((i1[t] = 1 && o1[t] = 1) || (i1[t] = 0 && o2[t] = 1)).");
		tref u = spec("(o1[t] = 0) weak_until (i1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref r = revision_value(s, u);
		REQUIRE(r != nullptr);
		CHECK(is_realizable(r));
		CHECK(entails(r, u));
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
// LS-17: revise() Cases 4/5, S/T past-operator specs, and binary-lhs
// and_distribute shapes had zero coverage.
TEST_SUITE("[PWR-LS17: uncovered cases]") {

	TEST_CASE("[PWR-LS17-01] Case 4: G spec vs R update"
		* doctest::skip(!ltlsynt_available())) {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("(o1[t] = 1) release (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-LS17-02] Case 5: F spec vs U update"
		* doctest::skip(!ltlsynt_available())) {
		tref s = spec("F (o1[t] = 0).");
		tref u = spec("(o1[t] = 1) until (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-LS17-03] F spec vs sometimes update revises "
			"(LS-3: same eventually operator)" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("F (o1[t] = 0).");
		tref u = spec("sometimes (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-LS17-04] S past-operator spec survives revision" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("(o1[t] = 1) since (o1[t] = 0).");
		tref u = spec("G (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-LS17-05] binary-lhs conjunction distributes: "
			"(a && b) U c" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("((o1[t] = 1) && (o2[t] = 1)) until (o3[t] = 1).");
		tref u = spec("G (o4[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = revision_value(s, u);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}
}

// ============================================================================
// PWR-R6: per-revision satisfiability memo (+ the cross-revision TAU_CACHE
// result cache inside is_tau_formula_sat).
//
// Every is_tau_formula_sat query with U/R/W/S/T content is one ltlsynt
// subprocess (call_ltlsynt has no cache), and one revision repeats identical
// hash-consed (formula, start_time) queries: Step 1's spec ∧ update, Step 2's
// clause ∧ update on a single-clause spec and revise()'s early-exit
// conjunction are the same tref. The memo answers repeats once per revision
// in every build; under TAU_CACHE (Release) the global result cache also
// answers repeats across revisions.
//
// Counted from the library: call_ltlsynt records one info_count per process
// it starts, and the report-returning revision entry hands that report back.
// ============================================================================

#include <cstdio>
#include <cstdlib>

namespace {

// Total of the "ltlsynt_calls" counters a revision's report carries: one
// info_count node per non-memoised is_tau_formula_sat query.
int count_ltlsynt_calls(const report& rep) {
	int n = 0;
	for (const auto& node : rep.nodes())
		if (node.tag == code::info_count
			&& rep.str(node.key) == "ltlsynt_calls")
			n += (int)node.value;
	return n;
}

} // namespace

TEST_SUITE("[PWR-R6: satisfiability memoisation]") {

	TEST_CASE("[PWR-R6-01] one U-revision spawns each distinct query "
			"once; a repeated revision spawns none under "
			"TAU_CACHE" * doctest::skip(!ltlsynt_available())) {
		// s ∧ u is UNSAT at every discharge combination (invariants
		// o1=0 vs o1=1 conflict, commitments force o1 both ways), so
		// the revision runs the full recursion instead of returning
		// at the Step 1 vacuity check.
		tref s = spec("(o1[t] = 0) until ((o1[t] = 0) && (o2[t] = 1)).");
		tref u = spec("(o1[t] = 1) until ((o1[t] = 1) && (o2[t] = 0)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);

		auto rr1 = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(rr1.has_value());
		tref r1 = rr1.value();
		const int n1 = count_ltlsynt_calls(rr1.report());

		auto rr2 = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(rr2.has_value());
		tref r2 = rr2.value();
		const int n2 = count_ltlsynt_calls(rr2.report());

		REQUIRE(r1 != nullptr);
		CHECK(r2 == r1);

		// The revision makes 6 temporal queries of which 2 are
		// hash-consed repeats (Step 2's clause ∧ update and the
		// outer revise() early exit); the memo answers those, so at
		// most 4 queries run, each with one synthesis and at most one
		// data game.
		CHECK(n1 >= 1);
		CHECK(n1 <= 8);
#ifdef TAU_CACHE
		// Cross-revision result cache: the identical revision answers
		// every query from the cache — zero subprocesses.
		CHECK(n2 == 0);
#else
		// No global cache in this build: the repeat costs at most the
		// same, less when call_ltlsynt_game's string-keyed cache, which
		// TAU_CACHE does not govern, answers the data games' queries.
		CHECK(n2 >= 1);
		CHECK(n2 <= n1);
#endif // TAU_CACHE
	}
}

// revise() and its role helpers called directly: through
// pointwise_revision_temporal a recursion step is reached only when every
// check above it fails, so the cases below pin each step on its own.
TEST_SUITE("[PWR-RC: revise cases and role helpers]") {

	static tref revise_value(tref phi, tref psi, tref psi_f) {
		auto rr = revise<node_t>(phi, psi, psi_f, 0);
		REQUIRE(rr.has_value());
		REQUIRE(rr.value() != nullptr);
		return rr.value();
	}

	TEST_CASE("[PWR-RC-01] roles of every binary operator round-trip") {
		struct { const char* src; temporal_op op; bool inv_is_lhs; } cases[] = {
			{ "(o1[t] = 0) until (o2[t] = 0).",      temporal_op::U, true },
			{ "(o1[t] = 0) weak_until (o2[t] = 0).", temporal_op::W, true },
			{ "(o1[t] = 0) since (o2[t] = 0).",      temporal_op::S, true },
			{ "(o1[t] = 0) release (o2[t] = 0).",    temporal_op::R, false },
			{ "(o1[t] = 0) trigger (o2[t] = 0).",    temporal_op::T, false },
		};
		for (const auto& c : cases) {
			CAPTURE(c.src);
			tref fm = spec(c.src);
			REQUIRE(fm != nullptr);
			REQUIRE(get_temporal_op<node_t>(fm) == c.op);
			tref lhs = tau::get(fm)[0].first();
			tref rhs = tau::get(fm)[0].child(1);
			auto [inv, commit] = decompose_roles<node_t>(fm);
			CHECK(inv == (c.inv_is_lhs ? lhs : rhs));
			CHECK(commit == (c.inv_is_lhs ? rhs : lhs));
			CHECK(tau::subtree_equals(
				rebuild_from_roles<node_t>(c.op, inv, commit), fm));
		}
	}

	TEST_CASE("[PWR-RC-02] unary operators have no roles") {
		tref g = spec("G (o1[t] = 0).");
		tref a = tau::get(g)[0].first();
		auto [inv, commit] = decompose_roles<node_t>(g);
		CHECK(inv == nullptr);
		CHECK(commit == nullptr);
		CHECK(rebuild_from_roles<node_t>(temporal_op::ALWAYS, a, a)
			== nullptr);
	}

	TEST_CASE("[PWR-RC-03] a conjunctive invariant of R and T distributes") {
		for (const char* src : {
			"(o3[t] = 1) release ((o1[t] = 1) && (o2[t] = 1)).",
			"(o3[t] = 1) trigger ((o1[t] = 1) && (o2[t] = 1))." })
		{
			CAPTURE(src);
			tref fm = spec(src);
			REQUIRE(fm != nullptr);
			temporal_op op = get_temporal_op<node_t>(fm);
			std::vector<tref> conjs;
			gather_top_conjuncts<node_t>(and_distribute<node_t>(fm), conjs);
			REQUIRE(conjs.size() == 2);
			for (tref c : conjs) {
				CHECK(get_temporal_op<node_t>(c) == op);
				// The commitment o3 = 1 stays on the left of each copy.
				CHECK(tau::subtree_equals(tau::get(c)[0].first(),
					tau::get(fm)[0].first()));
			}
		}
	}

	TEST_CASE("[PWR-RC-04] G vs G revises under the operator") {
		tref phi = spec("G (o1[t] = 0).");
		tref psi = spec("G (o1[t] = 1).");
		tref res = revise_value(phi, psi, psi);
		CHECK(get_temporal_op<node_t>(res) == temporal_op::ALWAYS);
		CHECK(is_realizable(res));
		CHECK(entails(res, psi));
	}

	TEST_CASE("[PWR-RC-05] F vs F revises under the operator"
		* doctest::skip(!ltlsynt_available()))
	{
		tref phi = spec("F (o1[t] = 0 && o2[t] = 0).");
		tref psi = spec("F (o1[t] = 1).");
		tref psi_f = spec("(F (o1[t] = 1)) && (G (o2[t] = 1)).");
		tref res = revise_value(phi, psi, psi_f);
		CHECK(get_temporal_op<node_t>(res) == temporal_op::SOMETIMES);
		CHECK(is_realizable(res));
	}

	TEST_CASE("[PWR-RC-06] a step formula is lifted against an until"
		* doctest::skip(!ltlsynt_available()))
	{
		tref phi = spec("o1[t] = 0 && o2[t] = 0.");
		tref psi = spec("(o1[t] = 1) until (o2[t] = 1).");
		tref res = revise_value(phi, psi, psi);
		CHECK(get_temporal_op<node_t>(res) == temporal_op::U);
		CHECK(is_realizable(res));
	}

	TEST_CASE("[PWR-RC-07] an until spec meets a step update"
		* doctest::skip(!ltlsynt_available()))
	{
		tref phi = spec("(o1[t] = 1) until (o2[t] = 1).");
		tref psi = spec("o1[t] = 0 && o2[t] = 0.");
		tref res = revise_value(phi, psi, psi);
		CHECK(get_temporal_op<node_t>(res) == temporal_op::U);
		CHECK(is_realizable(res));
	}

	TEST_CASE("[PWR-RC-08] R and T specs keep their operator against G"
		* doctest::skip(!ltlsynt_available()))
	{
		struct { const char* src; temporal_op op; } cases[] = {
			{ "(o1[t] = 0) release (o2[t] = 0).", temporal_op::R },
			{ "(o1[t] = 0) trigger (o2[t] = 0).", temporal_op::T },
		};
		tref psi = spec("G (o2[t] = 1).");
		for (const auto& c : cases) {
			CAPTURE(c.src);
			tref res = revise_value(spec(c.src), psi, psi);
			CHECK(get_temporal_op<node_t>(res) == c.op);
			// The kept candidate is realizable together with the update.
			CHECK(is_realizable(build_wff_and<node_t>(res, psi)));
		}
	}

	TEST_CASE("[PWR-RC-09] conflicting W, T and S pairs revise in place"
		* doctest::skip(!ltlsynt_available()))
	{
		struct { const char* phi; const char* psi; temporal_op op; } cases[] = {
			{ "(o1[t] = 0) weak_until (o1[t] = 0 && o2[t] = 1).",
			  "(o1[t] = 1) weak_until (o1[t] = 1 && o2[t] = 0).",
			  temporal_op::W },
			{ "(o1[t] = 0) trigger (o2[t] = 0).",
			  "(o1[t] = 1) trigger (o2[t] = 1).", temporal_op::T },
			{ "(o1[t] = 0) since (o2[t] = 0).",
			  "(o1[t] = 1) since (o2[t] = 1).", temporal_op::S },
		};
		for (const auto& c : cases) {
			CAPTURE(c.phi);
			tref psi = spec(c.psi);
			tref res = revise_value(spec(c.phi), psi, psi);
			CHECK(get_temporal_op<node_t>(res) == c.op);
			CHECK(is_realizable(res));
		}
	}

	TEST_CASE("[PWR-RC-10] the semantic fallback leaves a sound revision"
		* doctest::skip(!ltlsynt_available()))
	{
		struct fallback_guard {
			bool saved = pwr_semantic_fallback;
			~fallback_guard() { pwr_semantic_fallback = saved; }
		} guard;
		pwr_semantic_fallback = true;
		tref s = spec("(o1[t] = 0) until (o2[t] = 0).");
		tref u = spec("(o1[t] = 1) release (o2[t] = 1).");
		tref res = revision_value(s, u);
		REQUIRE(res != nullptr);
		CHECK(is_realizable(res));
		CHECK(entails(res, u));
	}
}
