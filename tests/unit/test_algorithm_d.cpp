// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for Algorithm D: synthesis parity game × T_1 product game.
//
// Algorithm D avoids putting structural constraints (Φ_δ, Ψ_I) into the
// LTL formula.  Instead it builds the propositional synthesis game for
// φ*(D_i) and solves the data-enriched product game (game × T_1) with
// Zielonka's algorithm.
//
// Test naming: ALG-D-01..05 (Phase 1 DPA infrastructure, kept for regression),
//              ALG-D-10..XX (Phase 2 product game correctness).

#include "test_init.h"
#include "test_tau_helpers.h"
#include "ltl_aba.h"
#include "algorithm_d_game.h"

using namespace idni::tau_lang;

static tref spec(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

static bool alg_d_realizable(const char* s) {
	setenv("TAU_LTL_ALG", "D", 1);
	bdd_init<Bool>();
	tref fm = spec(s);
	bool result = (fm != nullptr) && is_tau_formula_sat<node_t>(fm);
	unsetenv("TAU_LTL_ALG");
	return result;
}

// ── Phase 1: guard evaluator and HOA game parser (regression) ────────────

TEST_SUITE("[Algorithm D: guard evaluator]") {

	TEST_CASE("[ALG-D-01] 't' guard is always true") {
		CHECK(alg_d::eval_guard("t", 0, 2));
		CHECK(alg_d::eval_guard("t", 3, 2));
	}

	TEST_CASE("[ALG-D-02] 'f' guard is always false") {
		CHECK_FALSE(alg_d::eval_guard("f", 0, 2));
		CHECK_FALSE(alg_d::eval_guard("f", 3, 2));
	}

	TEST_CASE("[ALG-D-03] single AP guard") {
		CHECK(     alg_d::eval_guard("0",  1, 2));  // AP0=true
		CHECK_FALSE(alg_d::eval_guard("0", 2, 2));  // AP0=false
	}

	TEST_CASE("[ALG-D-04] negation") {
		CHECK(     alg_d::eval_guard("!0", 0, 2));  // !AP0 when AP0=false
		CHECK_FALSE(alg_d::eval_guard("!0",1, 2));  // !AP0 when AP0=true
	}

	TEST_CASE("[ALG-D-05] conjunction") {
		CHECK(     alg_d::eval_guard("0 & 1",  3, 2));  // AP0=AP1=true
		CHECK_FALSE(alg_d::eval_guard("0 & 1", 1, 2));  // AP1=false
		CHECK_FALSE(alg_d::eval_guard("0 & 1", 2, 2));  // AP0=false
	}

	TEST_CASE("[ALG-D-06] disjunction") {
		CHECK(alg_d::eval_guard("0 | 1", 1, 2));
		CHECK(alg_d::eval_guard("0 | 1", 2, 2));
		CHECK_FALSE(alg_d::eval_guard("0 | 1", 0, 2));
	}

	TEST_CASE("[ALG-D-07] complex guard") {
		// "0 & !1" — AP0 true, AP1 false → only bitmask 01 = 1
		CHECK(     alg_d::eval_guard("0 & !1", 1, 2));
		CHECK_FALSE(alg_d::eval_guard("0 & !1", 3, 2));
		CHECK_FALSE(alg_d::eval_guard("0 & !1", 0, 2));
	}

	TEST_CASE("[ALG-D-08] D-pattern extraction uses AP names, not AP order") {
		alg_d::SynthGame g;
		g.aps = {"r_0", "d_1", "d_0"};
		g.controllable = {true, true, true};
		CHECK(alg_d::d_pattern_from_assignment(g, 0b110, 2) == 0b11);
		CHECK(alg_d::d_pattern_from_assignment(g, 0b010, 2) == 0b10);
		CHECK(alg_d::d_pattern_from_assignment(g, 0b100, 2) == 0b01);
	}

	TEST_CASE("[ALG-D-09] D-pattern extraction ignores non-D controllable APs") {
		alg_d::SynthGame g;
		g.aps = {"acc", "d_0", "grant"};
		g.controllable = {true, true, true};
		CHECK(alg_d::d_pattern_from_assignment(g, 0b111, 1) == 0b1);
		CHECK(alg_d::d_pattern_from_assignment(g, 0b101, 1) == 0b0);
	}

	TEST_CASE("[ALG-D-10] multi-digit D AP names are parsed by name") {
		CHECK(alg_d::d_index_from_ap_name("d_0") == 0);
		CHECK(alg_d::d_index_from_ap_name("d_10") == 10);
		CHECK(alg_d::d_index_from_ap_name("q_10") == -1);
		CHECK(alg_d::d_index_from_ap_name("d_x") == -1);
		alg_d::SynthGame g;
		g.aps = {"d_10", "d_2", "d_0"};
		g.controllable = {true, true, true};
		CHECK(alg_d::d_pattern_from_assignment(g, 0b111, 11)
			== ((1 << 10) | (1 << 2) | 1));
		CHECK(alg_d::d_pattern_from_assignment(g, 0b011, 11)
			== ((1 << 10) | (1 << 2)));
	}
}

// ── Phase 2: DPA extraction regression ───────────────────────────────────

TEST_SUITE("[Algorithm D Phase 1: DPA extraction]") {

	TEST_CASE("[ALG-D-11] G(F(p0)) produces a non-empty DPA") {
		std::string hoa = call_ltl2tgba_dpa("G(F(p0))");
		REQUIRE(!hoa.empty());
		DpaAutomaton dpa = parse_dpa_hoa(hoa);
		CHECK(dpa.num_states >= 1);
		CHECK(dpa.aps.size() == 1u);
	}

	TEST_CASE("[ALG-D-12] DPA for G(F(p0)) is deterministic") {
		std::string hoa = call_ltl2tgba_dpa("G(F(p0))");
		REQUIRE(!hoa.empty());
		DpaAutomaton dpa = parse_dpa_hoa(hoa);
		REQUIRE(dpa.num_states >= 1);
		int aps = (int)dpa.aps.size();
		int expected = 1 << aps;
		for (int s = 0; s < dpa.num_states; ++s)
			CHECK((int)dpa.edges[s].size() == expected);
	}
}

// ── Phase 2: product game correctness ────────────────────────────────────

TEST_SUITE("[Algorithm D: product game correctness]") {

	TEST_CASE("[ALG-D-20] Zielonka on trivial 2-state game: sys always wins") {
		// Simple game: state 0 (player 0=env), state 1 (player 1=sys)
		// Env unconditional → sys. Sys self-loops. All priority 1 (odd → sys wins).
		alg_d::ProductGame pg;
		pg.n_states  = 2;
		pg.init      = 0;
		pg.player    = {0, 1};
		pg.priority  = {1, 1};
		pg.succs     = {{1}, {1}};
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.count(0));  // sys wins from initial state
		CHECK(W1.count(1));
	}

	TEST_CASE("[ALG-D-21] Zielonka: env wins if stuck sys") {
		// State 0 (sys/player 1), priority 0 (even = env wins), no successors
		alg_d::ProductGame pg;
		pg.n_states  = 1;
		pg.init      = 0;
		pg.player    = {1};
		pg.priority  = {0};
		pg.succs     = {{}};  // sys stuck
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.empty());  // sys loses (stuck)
	}

	TEST_CASE("[ALG-D-22] G(o1[t]:qlt > {0}:qlt) REALIZABLE via Alg D") {
		// Output always > 0: always achievable (system sets y > 0 each step)
		CHECK(alg_d_realizable("G (o1[t]:qlt > {0}:qlt)."));
	}

	TEST_CASE("[ALG-D-23] F(o1[t]:qlt > {0}:qlt) REALIZABLE via Alg D") {
		// Eventually output > 0: trivially realizable
		CHECK(alg_d_realizable("F (o1[t]:qlt > {0}:qlt)."));
	}

	TEST_CASE("[ALG-D-24] G(o1[t]:qlt < {0}:qlt) REALIZABLE via Alg D") {
		// Output always < 0: realizable (set y = -1 always)
		CHECK(alg_d_realizable("G (o1[t]:qlt < {0}:qlt)."));
	}

	TEST_CASE("[ALG-D-25] GF(o1[t]:qlt > {0}:qlt) REALIZABLE via Alg D") {
		// Always eventually output > 0: realizable (system sets y > 0 always)
		CHECK(alg_d_realizable("G F (o1[t]:qlt > {0}:qlt)."));
	}

	TEST_CASE("[ALG-D-26] G(o1[t]:qlt > {1/2}:qlt) && G(o1[t]:qlt < {0}:qlt) UNREALIZABLE") {
		// Requires y > 1/2 and y < 0 simultaneously: UNREALIZABLE
		CHECK_FALSE(alg_d_realizable(
			"G (o1[t]:qlt > {1/2}:qlt) && G (o1[t]:qlt < {0}:qlt)."));
	}

	TEST_CASE("[ALG-D-27] G(o1[t]:qlt > o1[t-1]:qlt) REALIZABLE") {
		// Output always strictly increasing: realizable in DLO (always pick larger)
		CHECK(alg_d_realizable("G (o1[t]:qlt > o1[t-1]:qlt)."));
	}

	TEST_CASE("[ALG-D-28] G(o1[t]:qlt > {0}:qlt) U (o1[t]:qlt < {0}:qlt) REALIZABLE") {
		// G(y>0) U (y<0): the U releases when y<0; achievable since DLO has both
		// positive and negative elements
		CHECK(alg_d_realizable(
			"(o1[t]:qlt > {0}:qlt) U (o1[t]:qlt < {0}:qlt)."));
	}

	TEST_CASE("[ALG-D-29] ALG-D-gate falls through for non-qlt formula") {
		// Non-qlt formula: sbf type — Algorithm D says not applicable, falls through
		// to default path.  Use a simple formula that the default path handles.
		setenv("TAU_LTL_ALG", "D", 1);
		bdd_init<Bool>();
		tref fm = spec("G (o1[t]:sbf = 0).");
		REQUIRE(fm != nullptr);
		bool result = is_tau_formula_sat<node_t>(fm);
		unsetenv("TAU_LTL_ALG");
		// G(o1:sbf = 0) is satisfiable: always output 0
		CHECK(result);
	}

	TEST_CASE("[ALG-D-30] ALG-D-gate falls through when input variables are present") {
		// Algorithm D's current product game is output-only.  Input formulas must
		// fall through to Algorithm B instead of using the T1-only product.
		CHECK(alg_d_realizable("G (o1[t]:qlt > i1[t]:qlt)."));
	}

	TEST_CASE("[ALG-D-31] ALG-D-gate falls through for input lookback variables") {
		// Input lookback still requires Algorithm B's P-bit encoding.
		CHECK(alg_d_realizable("G (o1[t]:qlt > i1[t-1]:qlt)."));
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}