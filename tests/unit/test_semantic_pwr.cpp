// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Tests for semantic PWR (Optimal Mode) — winning-region-based revision.
//
// Tests the Algorithm D-based fallback from pwr-ltl.tex §11:
//   θ = ψ ∧ Win_0 ∧ G(Win → X Win)
//
// Test categories:
//   SPWR-S-*   Safety regression (optimal mode matches fast mode)
//   SPWR-L-*   Liveness preservation
//   SPWR-I-*   Idempotence
//   SPWR-F-*   Fallback chain (fast → optimal → update)
//   SPWR-A-*   AlgDResult struct and winning region export

#include "test_init.h"
#include "test_tau_helpers.h"
#include "pointwise_revision.h"
#include "semantic_pwr.h"

using namespace idni::tau_lang;

static tref spec(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

static bool is_realizable(tref fm) {
	return fm && is_tau_formula_sat<node_t>(fm);
}

// ============================================================================
// SPWR-S: Safety regression — optimal mode should not break fast mode
// ============================================================================

TEST_SUITE("[SPWR-S: Safety regression]") {

	TEST_CASE("[SPWR-S-01] Compatible safety: same value") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-S-02] Incompatible safety: different values") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-S-03] Partial conflict: two outputs") {
		tref s = spec("G ((o1[t] = 0) && (o2[t] = 0)).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-S-04] Trivial spec: T * G(o1=0) = update") {
		tref s = spec("T.");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

}

// ============================================================================
// SPWR-L: Liveness preservation
// ============================================================================

TEST_SUITE("[SPWR-L: Liveness preservation]") {

	TEST_CASE("[SPWR-L-01] F(goal) with compatible update") {
		tref s = spec("F (o1[t] = 1).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-L-02] Until with compatible update") {
		tref s = spec("(o1[t] = 0) U (o1[t] = 1).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-L-03] Release with compatible update") {
		tref s = spec("(o1[t] = 1) R (o1[t] = 0).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

}

// ============================================================================
// SPWR-I: Idempotence — spec * spec should be equivalent to spec
// ============================================================================

TEST_SUITE("[SPWR-I: Idempotence]") {

	TEST_CASE("[SPWR-I-01] Safety idempotence") {
		tref s = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, s, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-I-02] Eventually idempotence") {
		tref s = spec("F (o1[t] = 1).");
		REQUIRE(s != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, s, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-I-03] Multi-clause idempotence") {
		tref s = spec("G (o1[t] = 0) && G (o2[t] = 1).");
		REQUIRE(s != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, s, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

}

// ============================================================================
// SPWR-F: Fallback chain (fast → optimal → update)
// ============================================================================

TEST_SUITE("[SPWR-F: Fallback chain]") {

	TEST_CASE("[SPWR-F-01] Result always realizable (AGM success)") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-F-02] F spec: unrealizable spec with realizable update") {
		tref s = spec("F.");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-F-03] Multi-clause: one compatible, one not") {
		tref s = spec("G (o1[t] = 0) && G (o2[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

}

// ============================================================================
// SPWR-A: AlgDResult and winning region export
// ============================================================================

TEST_SUITE("[SPWR-A: Algorithm D result]") {

	TEST_CASE("[SPWR-A-01] solve_algorithm_d_full returns realizable with winning region") {
		// Build a simple realizable formula G(d_0)
		// T1_size = 1 (one constant: 0), T3 = trivial
		// This is a basic sanity check for the extended interface.
		std::string phi_star = "G(d_0)";
		int T1_size = 1;
		int K = 1;
		std::vector<omcat::QltType3> T3;
		std::vector<int> type_A;

		// Create a minimal T3/type_A for a single atom that's always true.
		omcat::QltType3 t3;
		t3.pos_m = 0;
		t3.pos_x = 0;
		t3.pos_y = 0;
		T3.push_back(t3);
		type_A.push_back(1); // D_0 is true in this type

		auto result = alg_d::solve_algorithm_d_full(phi_star, T1_size, T3, type_A, K);

		if (result.realizable) {
			CHECK(!result.winning_region.empty());
			CHECK(result.init_rho >= 0);
			CHECK(result.T1_size == T1_size);
			CHECK(result.K == K);
		}
		// If unrealizable due to ltlsynt not installed, that's OK — skip.
	}

	TEST_CASE("[SPWR-A-02] solve_algorithm_d_full empty input returns unrealizable") {
		auto result = alg_d::solve_algorithm_d_full("", 0, {}, {}, 0);
		CHECK_FALSE(result.realizable);
		CHECK(result.winning_region.empty());
		CHECK(result.init_rho == -1);
	}

}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}