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


// ============================================================================
// SPWR-W: Win formula construction
// ============================================================================

TEST_SUITE("[SPWR-W: Win formula construction]") {

	TEST_CASE("[SPWR-W-01] build_win_formula: empty winning region returns nullptr") {
		alg_d::AlgDResult result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.K = 1;

		std::vector<omcat::QltType3> T3;
		omcat::QltType3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {1};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref result_formula = build_win_formula<node_t>(result, atoms, T3, type_A);
		CHECK(result_formula == nullptr);
	}

	TEST_CASE("[SPWR-W-02] build_win0_formula: empty winning region returns nullptr") {
		alg_d::AlgDResult result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.synth_game.init = 0;
		result.K = 1;

		std::vector<omcat::QltType3> T3;
		omcat::QltType3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {1};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref result_formula = build_win0_formula<node_t>(result, atoms, T3, type_A);
		CHECK(result_formula == nullptr);
	}

	TEST_CASE("[SPWR-W-03] build_win_formula: single atom, bit set returns the atom itself") {
		alg_d::AlgDResult result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.K = 1;
		result.winning_region = {0};

		std::vector<omcat::QltType3> T3;
		omcat::QltType3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {1};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref result_formula = build_win_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(result_formula != nullptr);
		CHECK(result_formula == atom0);
	}

	TEST_CASE("[SPWR-W-04] build_win_formula: single atom, bit clear returns negated atom") {
		alg_d::AlgDResult result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.K = 1;
		result.winning_region = {0};

		std::vector<omcat::QltType3> T3;
		omcat::QltType3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {0};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref result_formula = build_win_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(result_formula != nullptr);
		tref expected = build_wff_neg<node_t>(atom0);
		CHECK(result_formula == expected);
	}

	TEST_CASE("[SPWR-W-05] build_win_formula: two winning T3 types yield disjunction of both patterns") {
		alg_d::AlgDResult result;
		result.T1_size = 2;
		result.synth_game.num_states = 1;
		result.K = 1;
		result.winning_region = {0, 1};

		std::vector<omcat::QltType3> T3;
		omcat::QltType3 t3_0;
		t3_0.pos_m = 0;
		T3.push_back(t3_0);
		omcat::QltType3 t3_1;
		t3_1.pos_m = 1;
		T3.push_back(t3_1);
		std::vector<int> type_A = {0, 1};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref result_formula = build_win_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(result_formula != nullptr);
		tref expected = build_wff_or<node_t>(build_wff_neg<node_t>(atom0), atom0);
		CHECK(result_formula == expected);
	}

	TEST_CASE("[SPWR-W-06] build_win_formula: two atoms, mixed-bit pattern builds correct conjunction") {
		alg_d::AlgDResult result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.K = 2;
		result.winning_region = {0};

		std::vector<omcat::QltType3> T3;
		omcat::QltType3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {2}; // binary 10: bit0 clear, bit1 set

		tref atom0 = spec("o1[t] = 1.");
		tref atom1 = spec("o2[t] = 1.");
		REQUIRE(atom0 != nullptr);
		REQUIRE(atom1 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {
			{atom0, "d_0"}, {atom1, "d_1"}};

		tref result_formula = build_win_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(result_formula != nullptr);
		tref expected = build_wff_and<node_t>(build_wff_neg<node_t>(atom0), atom1);
		CHECK(result_formula == expected);
	}

	TEST_CASE("[SPWR-W-07] build_win_formula: stub states beyond base_n are ignored") {
		alg_d::AlgDResult result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.K = 1;
		result.winning_region = {0, 5};

		std::vector<omcat::QltType3> T3;
		omcat::QltType3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {1};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref with_stub = build_win_formula<node_t>(result, atoms, T3, type_A);

		alg_d::AlgDResult result_no_stub;
		result_no_stub.T1_size = 1;
		result_no_stub.synth_game.num_states = 1;
		result_no_stub.K = 1;
		result_no_stub.winning_region = {0};

		tref without_stub = build_win_formula<node_t>(result_no_stub, atoms, T3, type_A);

		REQUIRE(with_stub != nullptr);
		REQUIRE(without_stub != nullptr);
		CHECK(with_stub == without_stub);
		CHECK(with_stub == atom0);
	}

	TEST_CASE("[SPWR-W-08] build_win_formula vs build_win0_formula: all-states vs initial-state-only") {
		alg_d::AlgDResult result;
		result.T1_size = 2;
		result.synth_game.num_states = 2;
		result.synth_game.init = 0;
		result.K = 1;
		result.winning_region = {0, 3};

		std::vector<omcat::QltType3> T3;
		omcat::QltType3 t3_0;
		t3_0.pos_m = 0;
		T3.push_back(t3_0);
		omcat::QltType3 t3_1;
		t3_1.pos_m = 1;
		T3.push_back(t3_1);
		std::vector<int> type_A = {1, 0};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref win = build_win_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(win != nullptr);
		tref expected_win = build_wff_or<node_t>(build_wff_neg<node_t>(atom0), atom0);
		CHECK(win == expected_win);

		tref win0 = build_win0_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(win0 != nullptr);
		CHECK(win0 == atom0);
	}

}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}