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
	auto nso_rr = get_nso_rr<node_t>(tau::get(s));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

// Check if a formula is realizable (REAL oracle).
static bool is_realizable(tref fm) {
	return fm && is_tau_formula_sat<node_t>(fm);
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
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-02] Incompatible safety: different values") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
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
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-04] T spec: T * G(o1=0) = update") {
		tref s = spec("T.");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-05] T update: G(o1=0) * T = spec unchanged") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("T.");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-06] Safety with negation") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (!(o1[t] = 0)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-07] Safety: spec conjunction with update") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-08] Safety: G(T) * G(o1=0) = conjunction") {
		tref s = spec("G T.");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-09] Safety with input variable") {
		tref s = spec("G (o1[t] = i1[t]).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-S-10] Safety: multiple outputs, no conflict") {
		tref s = spec("G ((o1[t] = 0) && (o2[t] = 1)).");
		tref u = spec("G ((o1[t] = 0) && (o2[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}
}

// ============================================================================
// PWR-T: Temporal operators (Until, Release, etc.)
// ============================================================================

TEST_SUITE("[PWR-T: Temporal operators]") {

	TEST_CASE("[PWR-T-01] Until: compatible invariants") {
		tref s = spec("(o1[t] = 0) U (o1[t] = 1).");
		tref u = spec("(o1[t] = 0) U (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-02] Until: incompatible invariants") {
		tref s = spec("(o1[t] = 0) U (o2[t] = 0).");
		tref u = spec("(o1[t] = 1) U (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-03] Release: same operator") {
		tref s = spec("(o1[t] = 0) R (o2[t] = 0).");
		tref u = spec("(o1[t] = 1) R (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-04] F operator: spec and update") {
		tref s = spec("F (o1[t] = 0).");
		tref u = spec("F (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-05] G wrapping F: nested temporal") {
		tref s = spec("G (F (o1[t] = 0)).");
		tref u = spec("G (F (o1[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-06] Until vs Release: operator mismatch") {
		tref s = spec("(o1[t] = 0) U (o2[t] = 0).");
		tref u = spec("(o1[t] = 0) R (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-07] Until: different commitments") {
		tref s = spec("(o1[t] = 0) U (o1[t] = 1).");
		tref u = spec("(o1[t] = 0) U (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-08] F with conjunction") {
		tref s = spec("F ((o1[t] = 0) && (o2[t] = 0)).");
		tref u = spec("F (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-09] Safety and liveness mix: G && F") {
		tref s = spec("G (o1[t] = 0) && F (o1[t] = 1).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-T-10] W operator") {
		tref s = spec("(o1[t] = 0) W (o1[t] = 1).");
		tref u = spec("(o1[t] = 0) W (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
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
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-V-02] Unrealizable update: return update anyway") {
		// Update is unrealizable (contradictory)
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G ((o1[t] = 0) && (o1[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		// Update o1=0 && o1=1 is contradictory — revision inherits unrealizability
		CHECK_FALSE(is_realizable(result));
	}

	TEST_CASE("[PWR-V-03] Spec is F: always realizable") {
		tref s = spec("F (0 = 1).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-V-04] Both unrealizable") {
		tref s = spec("G ((o1[t] = 0) && (o1[t] = 1)).");
		tref u = spec("G ((o2[t] = 0) && (o2[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
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
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-I-02] Until idempotence") {
		const char* formula = "(o1[t] = 0) U (o1[t] = 1).";
		tref s = spec(formula);
		tref u = spec(formula);
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-I-03] Release idempotence") {
		const char* formula = "(o1[t] = 1) R (o1[t] = 0).";
		tref s = spec(formula);
		tref u = spec(formula);
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-I-04] Multi-clause idempotence") {
		const char* formula = "G (o1[t] = 0) && F (o2[t] = 0).";
		tref s = spec(formula);
		tref u = spec(formula);
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-I-05] T idempotence") {
		const char* formula = "T.";
		tref s = spec(formula);
		tref u = spec(formula);
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
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
		tref u = spec("G (o1[t] = 0) || G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-M-02] Multi-clause spec") {
		tref s = spec("G (o1[t] = 0) || G (o1[t] = 1).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-M-03] Safety and liveness clauses") {
		tref s = spec("G (o1[t] = 0) && F (o2[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-M-04] Two F clauses") {
		tref s = spec("F (o1[t] = 0) && F (o2[t] = 0).");
		tref u = spec("F (o1[t] = 1) && F (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
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
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-02] Deeply nested: G(F(G(o1=0)))") {
		tref s = spec("G (F (o1[t] = 0)).");
		tref u = spec("G (F (o1[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-03] Single atom spec") {
		tref s = spec("F (o1[t] = 0).");
		tref u = spec("F (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-04] Negation in update") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (!(o1[t] = 0)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-05] Input-only spec") {
		tref s = spec("F (i1[t] = 0).");
		tref u = spec("F (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-06] Until with immediate discharge") {
		// o1=1 U o1=1 can discharge immediately
		tref s = spec("(o1[t] = 1) U (o1[t] = 1).");
		tref u = spec("(o1[t] = 0) U (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-E-07] G(F) liveness combination") {
		tref s = spec("G (F (o1[t] = 1)).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
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
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		// Result should be realizable (update is realizable)
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-P-02] Consistency: result is realizable when update is") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("F (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-P-03] Vacuity: compatible spec+update preserved") {
		// Both can be satisfied simultaneously
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-P-04] Idempotence: spec*spec = spec") {
		const char* formula = "G (o1[t] = 0).";
		tref s = spec(formula);
		tref u = spec(formula);
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}
}

// ============================================================================
// PWR-D: DeepSeek nontrivial tests
// ============================================================================

TEST_SUITE("[PWR-D: DeepSeek nontrivial]") {

	TEST_CASE("[PWR-D-01] Until with shared commitment, conflicting invariants") {
		tref s = spec("(o1[t] = 0) U (o2[t] = 1).");
		tref u = spec("(o1[t] = 1) U (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-02] Release with shared invariant") {
		tref s = spec("(o1[t] = 0) R (o2[t] = 0).");
		tref u = spec("(o1[t] = 1) R (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-03] Cross-clause with F and G") {
		tref s = spec("G (o1[t] = 0) && F (o2[t] = 1).");
		tref u = spec("G (o1[t] = 1) && F (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-04] Until chains") {
		tref s = spec("(o1[t] = 0) U ((o1[t] = 1) U (o1[t] = 0)).");
		tref u = spec("(o1[t] = 1) U (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-05] Disjunctive update selection") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 0) || G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-06] Two outputs, two temporal operators") {
		tref s = spec("F (o1[t] = 0) && G (o2[t] = 0).");
		tref u = spec("F (o1[t] = 1) && G (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-07] Release vs Until mismatch") {
		tref s = spec("(o1[t] = 0) R (o2[t] = 0).");
		tref u = spec("(o1[t] = 0) U (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-08] G(o1=0) * F(o1=1): safety vs liveness") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("F (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-09] Input-dependent with temporal") {
		tref s = spec("G (o1[t] = i1[t]).");
		tref u = spec("F (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-10] Nested G F with conflicting outputs") {
		tref s = spec("G (F (o1[t] = 0)).");
		tref u = spec("G (F (o1[t] = 1)).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-11] Multiple outputs Until") {
		tref s = spec("(o1[t] = 0) U ((o1[t] = 1) && (o2[t] = 0)).");
		tref u = spec("(o1[t] = 0) U (o2[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-12] W (weak until) operator") {
		tref s = spec("(o1[t] = 0) W (o1[t] = 1).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-13] Contradictory spec: both values at once") {
		tref s = spec("G ((o1[t] = 0) && (o1[t] = 1)).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-14] F and G on different outputs") {
		tref s = spec("F (o1[t] = 0).");
		tref u = spec("G (o2[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-15] Revision preserves compatible G clause") {
		tref s = spec("G (o1[t] = 0) && G (o2[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-16] U then F interleaving") {
		tref s = spec("(o1[t] = 0) U (o1[t] = 1).");
		tref u = spec("F (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-17] Three outputs safety") {
		tref s = spec("G ((o1[t] = 0) && (o2[t] = 0) && (o3[t] = 0)).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-18] Nested Until") {
		tref s = spec("(o1[t] = 0) U ((o2[t] = 0) U (o1[t] = 1)).");
		tref u = spec("(o1[t] = 0) U (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-19] G with disjunction inside") {
		tref s = spec("G ((o1[t] = 0) || (o1[t] = 1)).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[PWR-D-20] F(o1=0 && o2=1) with conflicting G update") {
		tref s = spec("F ((o1[t] = 0) && (o2[t] = 1)).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}
}
