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

// ── Phase 1: HOA game parser ──────────────────────────────────────────────

TEST_SUITE("[Algorithm D: HOA game parser]") {

	TEST_CASE("[ALG-D-32] full basic parse: states, aps, controllable, player, "
	          "trans, all-acceptance priorities") {
		std::string hoa = R"(HOA: v1
States: 2
Start: 0
AP: 2 "p0" "d_0"
controllable-AP: 1
spot-state-player: 0 1
acc-name: all
tool: ltlsynt
--BODY--
State: 0
[0] 1
[!0] 0
State: 1
[t] 1
--END--
)";
		alg_d::SynthGame g = alg_d::parse_synth_game_hoa(hoa);
		CHECK(g.num_states == 2);
		CHECK(g.init == 0);
		REQUIRE(g.aps.size() == 2u);
		CHECK(g.aps[0] == "p0");
		CHECK(g.aps[1] == "d_0");
		REQUIRE(g.controllable.size() == 2u);
		CHECK_FALSE(g.controllable[0]);
		CHECK(g.controllable[1]);
		REQUIRE(g.player.size() == 2u);
		CHECK(g.player[0] == 0);
		CHECK(g.player[1] == 1);
		REQUIRE(g.trans.size() == 2u);
		REQUIRE(g.trans[0].size() == 2u);
		CHECK(std::get<0>(g.trans[0][0]) == "0");
		CHECK(std::get<1>(g.trans[0][0]) == 1);
		CHECK(std::get<0>(g.trans[0][1]) == "!0");
		CHECK(std::get<1>(g.trans[0][1]) == 0);
		REQUIRE(g.trans[1].size() == 1u);
		CHECK(std::get<0>(g.trans[1][0]) == "t");
		CHECK(std::get<1>(g.trans[1][0]) == 1);
		REQUIRE(g.state_priority.size() == 2u);
		CHECK(g.state_priority[0] == 1);
		CHECK(g.state_priority[1] == 1);
		REQUIRE(g.edge_priority.size() == 2u);
		REQUIRE(g.edge_priority[0].size() == 2u);
		CHECK(g.edge_priority[0][0] == -1);
		CHECK(g.edge_priority[0][1] == -1);
	}

	TEST_CASE("[ALG-D-33] Buchi acceptance: colored state and edge get priority 1") {
		std::string hoa = R"(HOA: v1
States: 1
Start: 0
AP: 1 "p0"
acc-name: Buchi 1 Inf(0)
Acceptance: 1 Inf(0)
--BODY--
State: 0 {0}
[t] 0 {0}
--END--
)";
		alg_d::SynthGame g = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g.state_priority.size() == 1u);
		CHECK(g.state_priority[0] == 1);
		REQUIRE(g.edge_priority.size() == 1u);
		REQUIRE(g.edge_priority[0].size() == 1u);
		CHECK(g.edge_priority[0][0] == 1);
	}

	TEST_CASE("[ALG-D-34] co-Buchi acceptance: colored state and edge get the "
	          "dominant even priority") {
		// Fin(0): colour 0 marks the *rejecting* states.  Visiting them
		// infinitely often must lose for sys, so colour 0 has to map to an
		// even priority that dominates the uncoloured (odd) priority 1.
		std::string hoa = R"(HOA: v1
States: 1
Start: 0
AP: 1 "p0"
acc-name: co-Buchi 1 Fin(0)
Acceptance: 1 Fin(0)
--BODY--
State: 0 {0}
[t] 0 {0}
--END--
)";
		alg_d::SynthGame g = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g.state_priority.size() == 1u);
		CHECK(g.state_priority[0] == 2);
		REQUIRE(g.edge_priority.size() == 1u);
		REQUIRE(g.edge_priority[0].size() == 1u);
		CHECK(g.edge_priority[0][0] == 2);
	}

	// LG-3: the solver is hardwired to max-odd, so every other parity
	// flavor must be normalized on parse. For "min even 3": reflect
	// (c' = 2 - c) then shift (+1, because reflecting around the even
	// k-1 = 2 keeps the winning parity even) -- so p = 3 - c.
	TEST_CASE("[ALG-D-35] parity min even is normalized to max odd") {
		std::string hoa = R"(HOA: v1
States: 2
Start: 0
AP: 1 "p0"
acc-name: parity min even 3
Acceptance: 3 Inf(0)
--BODY--
State: 0 {2}
State: 1
--END--
)";
		alg_d::SynthGame g = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g.state_priority.size() == 2u);
		// color 2 (even, accepting under min-even since a run stuck on
		// state 0 has min color 2) must land on an ODD max-odd priority
		CHECK(g.state_priority[0] == 1);
		// uncolored stays at the neutral lowest priority
		CHECK(g.state_priority[1] == 0);
	}

	TEST_CASE("[ALG-D-35b] parity max even is shifted by one") {
		std::string hoa = R"(HOA: v1
States: 2
Start: 0
AP: 1 "p0"
acc-name: parity max even 3
Acceptance: 3 Inf(0)
--BODY--
State: 0 {2}
State: 1 {1}
--END--
)";
		alg_d::SynthGame g = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g.state_priority.size() == 2u);
		CHECK(g.state_priority[0] == 3);  // even winner 2 -> odd 3
		CHECK(g.state_priority[1] == 2);  // odd loser 1 -> even 2
	}

	TEST_CASE("[ALG-D-35c] parity max odd is the identity") {
		std::string hoa = R"(HOA: v1
States: 2
Start: 0
AP: 1 "p0"
acc-name: parity max odd 3
Acceptance: 3 Inf(0)
--BODY--
State: 0 {2}
State: 1 {1}
--END--
)";
		alg_d::SynthGame g = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g.state_priority.size() == 2u);
		CHECK(g.state_priority[0] == 2);
		CHECK(g.state_priority[1] == 1);
	}

	TEST_CASE("[ALG-D-36] controllable-AP with multiple indices") {
		std::string hoa = R"(HOA: v1
States: 1
Start: 0
AP: 3 "a" "b" "c"
controllable-AP: 0 2
--BODY--
State: 0
--END--
)";
		alg_d::SynthGame g = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g.controllable.size() == 3u);
		CHECK(g.controllable[0]);
		CHECK_FALSE(g.controllable[1]);
		CHECK(g.controllable[2]);
	}

	TEST_CASE("[ALG-D-37] trans-acc detection") {
		std::string hoa_trans = R"(HOA: v1
States: 1
Start: 0
AP: 1 "p0"
properties: trans-acc
--BODY--
State: 0
--END--
)";
		alg_d::SynthGame g1 = alg_d::parse_synth_game_hoa(hoa_trans);
		CHECK(g1.trans_acc);

		std::string hoa_no_trans = R"(HOA: v1
States: 1
Start: 0
AP: 1 "p0"
--BODY--
State: 0
--END--
)";
		alg_d::SynthGame g2 = alg_d::parse_synth_game_hoa(hoa_no_trans);
		CHECK_FALSE(g2.trans_acc);
	}

	TEST_CASE("[ALG-D-38] quoted AP names have surrounding quotes stripped") {
		std::string hoa = R"(HOA: v1
States: 1
Start: 0
AP: 1 "my_ap"
--BODY--
State: 0
--END--
)";
		alg_d::SynthGame g = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g.aps.size() == 1u);
		CHECK(g.aps[0] == "my_ap");
	}

	TEST_CASE("[ALG-D-39] stray non-transition body line is skipped without "
	          "corrupting transition order") {
		std::string hoa = R"(HOA: v1
States: 2
Start: 0
AP: 1 "p0"
--BODY--
State: 0
[0] 1
not a transition line
[1] 0
--END--
)";
		alg_d::SynthGame g = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g.trans.size() == 2u);
		REQUIRE(g.trans[0].size() == 2u);
		CHECK(std::get<0>(g.trans[0][0]) == "0");
		CHECK(std::get<1>(g.trans[0][0]) == 1);
		CHECK(std::get<0>(g.trans[0][1]) == "1");
		CHECK(std::get<1>(g.trans[0][1]) == 0);
	}

	TEST_CASE("[ALG-D-41] Spot's dotted 'spot.state-player' header assigns "
	          "ownership") {
		// Spot (--print-game-hoa) writes the header name with a dot.  If the
		// parser only matches the dashed spelling every state stays env-owned
		// and the sys branch of build_product_game is never taken.
		std::string hoa = R"(HOA: v1
States: 3
Start: 0
AP: 2 "p0" "d_0"
controllable-AP: 1
spot.state-player: 0 1 1
acc-name: all
tool: ltlsynt
--BODY--
State: 0
[0] 1
State: 1
[t] 2
State: 2
[t] 1
--END--
)";
		alg_d::SynthGame g = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g.player.size() == 3u);
		CHECK(g.player[0] == 0);
		CHECK(g.player[1] == 1);
		CHECK(g.player[2] == 1);
	}

	// AL-11 / AL-RT2 (re-port of the pre-rebase [ALG-D-45]): an HOA whose
	// acceptance is `Acceptance: 0 t` and that carries NO acc-name: line is
	// trivially-all.  Without this every state got priority 0 (env-good),
	// the all-even game was won by env everywhere, and the formula was
	// reported UNREALIZABLE regardless of structure.
	TEST_CASE("[ALG-D-60] Acceptance: 0 t with no acc-name: is treated as trivially-all") {
		const char* fixture =
			"HOA: v1\n"
			"States: 1\n"
			"Start: 0\n"
			"AP: 1 \"p0\"\n"
			"Acceptance: 0 t\n"
			"--BODY--\n"
			"State: 0\n"
			"[t] 0\n"
			"--END--\n";
		auto g = alg_d::parse_synth_game_hoa(fixture);
		REQUIRE(g.state_priority.size() == 1);
		CHECK(g.state_priority[0] == 1);
	}

	// ... and `Acceptance: 0 f` (never accepting) stays env-good.
	TEST_CASE("[ALG-D-60b] Acceptance: 0 f with no acc-name: stays priority 0") {
		const char* fixture =
			"HOA: v1\n"
			"States: 1\n"
			"Start: 0\n"
			"AP: 1 \"p0\"\n"
			"Acceptance: 0 f\n"
			"--BODY--\n"
			"State: 0\n"
			"[t] 0\n"
			"--END--\n";
		auto g = alg_d::parse_synth_game_hoa(fixture);
		REQUIRE(g.state_priority.size() == 1);
		CHECK(g.state_priority[0] == 0);
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

	TEST_CASE("[ALG-D-42] co-Buchi Fin(0): a run revisiting a rejecting state "
	          "infinitely often is env-win") {
		// Two-state cycle 0 → 1 → 0, colour 0 (= Fin(0) rejecting) on state 0
		// only.  The single run of this game visits the rejecting state
		// infinitely often, so under Fin(0) it must be scored env-win.
		// Both states are env-owned, so the verdict is decided purely by the
		// priorities the parser assigns — no ownership or attractor choice is
		// involved.
		std::string hoa = R"(HOA: v1
States: 2
Start: 0
AP: 1 "p0"
acc-name: co-Buchi 1 Fin(0)
Acceptance: 1 Fin(0)
--BODY--
State: 0 {0}
[t] 1
State: 1
[t] 0
--END--
)";
		alg_d::SynthGame g = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g.state_priority.size() == 2u);
		// The rejecting colour must dominate the uncoloured priority and be
		// even (even = env wins under the max-odd convention).
		CHECK(g.state_priority[0] > g.state_priority[1]);
		CHECK(g.state_priority[0] % 2 == 0);
		CHECK(g.state_priority[1] % 2 == 1);

		alg_d::ProductGame pg;
		pg.n_states = 2;
		pg.init     = 0;
		pg.player   = {0, 0};
		pg.priority = {g.state_priority[0], g.state_priority[1]};
		pg.succs    = {{1}, {0}};
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.empty());  // sys wins nowhere
	}

	TEST_CASE("[ALG-D-40] Zielonka recursive branch: the opponent attractor "
	          "goes to the opponent's winning set") {
		// Hand-built game reaching the recursive branch of zielonka_impl::solve
		// (the branch taken when the sub-game's loser set is non-empty).
		// Two env-owned states, priorities {1, 0}, edges 0 → 1 and 1 → 1.
		// Every play ends in the self-loop on state 1, whose priority 0 is
		// even, so env wins from both states and sys wins nowhere.
		//
		// Hand trace (max-parity, even = env):
		//   V = {0,1}, c_max = 1 (state 0), beneficiary = 1 (sys).
		//   X = attr_1({0}) = {0}   (state 0 has no predecessors).
		//   solve({1}) → c_max = 0, beneficiary = 0, attractor swallows {1},
		//                sub-game loser set empty → returns W0' = {1}, W1' = {}.
		//   Back at the top: Wl = W0' = {1} is NON-empty, so the early return
		//   at the top of the branch is skipped and the recursion runs:
		//   Y = attr_0({1}) = {0,1}, V3 = {} → W0'' = W1'' = {}.
		//   Correct Zielonka: Y ∪ Wl belongs to the OPPONENT (player 0 here),
		//   so W0 = {0,1}, W1 = {}.
		alg_d::ProductGame pg;
		pg.n_states = 2;
		pg.init     = 0;
		pg.player   = {0, 0};
		pg.priority = {1, 0};
		pg.succs    = {{1}, {1}};
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.empty());  // env wins everywhere
	}

	TEST_CASE("[ALG-D-43] Zielonka: flipping ownership of the choice state "
	          "flips the winner") {
		// Three states: 0 is a choice state with successors 1 and 2;
		// state 1 self-loops at priority 1 (odd → sys), state 2 self-loops at
		// priority 0 (even → env).  Whoever owns state 0 steers to its own
		// sink, so ownership alone decides the verdict at state 0 — while
		// states 1 and 2 keep their verdicts in both variants.
		//
		// Both variants take the recursive branch: c_max = 1 selects A = {1},
		// the beneficiary-1 attractor never covers the even sink 2, so the
		// sub-game hands back a non-empty player-0 set.  Under the swapped
		// merge the opponent attractor Y (which contains sink 2) is credited
		// to sys in both variants, so sys is reported to win everywhere and
		// the flip disappears.
		alg_d::ProductGame env_owns;
		env_owns.n_states = 3;
		env_owns.init     = 0;
		env_owns.player   = {0, 1, 0};
		env_owns.priority = {0, 1, 0};
		env_owns.succs    = {{1, 2}, {1}, {2}};
		auto W1_env = alg_d::zielonka_win_player1(env_owns);
		CHECK(!W1_env.count(0));  // env steers to the even sink
		CHECK(W1_env.count(1));
		CHECK(!W1_env.count(2));

		alg_d::ProductGame sys_owns = env_owns;
		sys_owns.player = {1, 1, 0};  // only the choice state changes hands
		auto W1_sys = alg_d::zielonka_win_player1(sys_owns);
		CHECK(W1_sys.count(0));   // sys steers to the odd sink
		CHECK(W1_sys.count(1));
		CHECK(!W1_sys.count(2));
	}

	// ── LG-32: dead ends lose for their owner ────────────────────────────
	//
	// Parity-game semantics: a player who cannot move loses the finite play.
	// `zielonka_impl::solve` had no dead-end handling at all — a successor-less
	// state was simply scored by its own priority's parity, so a stuck sys
	// state with an odd priority was counted a sys WIN.  ALG-D-21 uses an even
	// priority, where parity and the correct answer coincide, so it never
	// discriminated.  `build_product_game` can produce successor-less sys
	// states whenever T3 feasibility prunes every D-pattern of a product
	// state, which became reachable once LG-1 gave states real ownership.

	TEST_CASE("[ALG-D-44] a stuck sys state loses even at an odd priority") {
		// Single sys-owned state, priority 1 (odd = sys under max-parity),
		// no successors.  Sys cannot move → sys loses.
		alg_d::ProductGame pg;
		pg.n_states = 1;
		pg.init     = 0;
		pg.player   = {1};
		pg.priority = {1};
		pg.succs    = {{}};
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.empty());
	}

	TEST_CASE("[ALG-D-45] a stuck env state loses even at an even priority") {
		// Mirror image: env-owned, priority 0 (even = env), no successors.
		// Env cannot move → sys wins the state.
		alg_d::ProductGame pg;
		pg.n_states = 1;
		pg.init     = 0;
		pg.player   = {0};
		pg.priority = {0};
		pg.succs    = {{}};
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.count(0));
	}

	TEST_CASE("[ALG-D-46] a dead end propagates through the attractor") {
		// 0 (sys) → 1 (sys, stuck).  Sys is forced into the dead end from
		// state 0, so sys loses both — despite both priorities being odd.
		alg_d::ProductGame pg;
		pg.n_states = 2;
		pg.init     = 0;
		pg.player   = {1, 1};
		pg.priority = {1, 1};
		pg.succs    = {{1}, {}};
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.empty());
	}

	TEST_CASE("[ALG-D-47] a dead end does not poison a live alternative") {
		// 0 (sys) chooses between the stuck state 1 and the good odd sink 2.
		// Sys avoids the dead end and wins from 0 and 2; state 1 stays lost.
		alg_d::ProductGame pg;
		pg.n_states = 3;
		pg.init     = 0;
		pg.player   = {1, 1, 1};
		pg.priority = {1, 1, 1};
		pg.succs    = {{1, 2}, {}, {2}};
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.count(0));
		CHECK(!W1.count(1));
		CHECK(W1.count(2));
	}

	// AL-R1 / AL-RT4: the discriminating case [ALG-D-47] misses.  State 0
	// (sys) chooses between the stuck state 1 and state 2, an env-owned
	// EVEN self-loop (env wins there).  Both of sys's real options lose, so
	// W1 must be empty.  Before the fix, `solve` scored the dead end 1 as a
	// sys win by parity, handed {0,1} to sys, and the one-shot override
	// only erased 1 — leaving 0 in W1 (false REALIZABLE).
	TEST_CASE("[ALG-D-49] AL-R1: a dead end next to an env-won successor does not keep the predecessor") {
		alg_d::ProductGame pg;
		pg.n_states = 3;
		pg.init     = 0;
		pg.player   = {1, 1, 0};
		pg.priority = {1, 1, 0};
		pg.succs    = {{1, 2}, {}, {2}};
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.empty());
	}

	// The symmetric shape keeps its documented asymmetry: an ENV state that
	// could move into an env dead end is still NOT awarded to sys (the env
	// over-approximation refusal), but the env dead end itself is.
	TEST_CASE("[ALG-D-50] AL-R1: env edges into env dead ends are pruned, attractor still refused") {
		alg_d::ProductGame pg;
		pg.n_states = 3;
		pg.init     = 0;
		pg.player   = {0, 0, 0};
		pg.priority = {0, 0, 0};
		pg.succs    = {{1, 2}, {}, {2}};
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.count(1));      // env cannot move: lost for env
		CHECK(!W1.count(2));     // even self-loop: env wins
		CHECK(!W1.count(0));     // env avoids its dead end; attractor not awarded
	}

	// Pruning must not remove the live alternative: [ALG-D-47] with an
	// extra sys edge from the dead end's sibling stays REALIZABLE.
	TEST_CASE("[ALG-D-51] AL-R1: pruning keeps live alternatives") {
		alg_d::ProductGame pg;
		pg.n_states = 4;
		pg.init     = 0;
		pg.player   = {1, 1, 0, 1};
		pg.priority = {1, 1, 0, 1};
		pg.succs    = {{1, 2, 3}, {}, {2}, {3}};
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.count(0));
		CHECK(!W1.count(1));
		CHECK(!W1.count(2));
		CHECK(W1.count(3));
	}

	// ── LS-10: call_ltlsynt_game's input mechanism ───────────────────────
	//
	// `call_ltlsynt` retired inline `--formula="…"` for tempfile +
	// posix_spawn precisely because a grown formula (Algorithm B with many
	// constants) blows past the Linux MAX_ARG_STRLEN cap of 131072 on a
	// single argument.  `call_ltlsynt_game` — the entry point for Algorithm D
	// and for semantic_pwr_optimal — kept the old popen path, so the same
	// formula comes back as an empty game, which every caller reads as
	// "unrealizable".
	TEST_CASE("[ALG-D-48] a formula past MAX_ARG_STRLEN still produces a game") {
		// Skip when ltlsynt is not on PATH: nothing to compare against.
		auto small = alg_d::call_ltlsynt_game("d_0", {}, {"d_0"});
		if (small.num_states == 0) return;

		// 140000 characters — just past the 131072-byte single-argument cap.
		// The conjunction is trivially reducible, so ltlsynt itself is cheap;
		// only the argument length is under test.
		std::string big = "d_0";
		big.reserve(150000);
		while (big.size() < 140000) big += " & d_0";
		auto g = alg_d::call_ltlsynt_game(big, {}, {"d_0"});
		CHECK(g.num_states > 0);
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


// ── AL-RT1: build_product_game direct tests (re-ported from the pre-rebase
// suite [ALG-D-55]/[ALG-D-56a]/[ALG-D-56b]) ───────────────────────────────

TEST_SUITE("[Algorithm D: product game construction]") {

	TEST_CASE("[ALG-D-55] edge-color layer creates intermediate stub states") {
		alg_d::SynthGame g;
		g.num_states = 2;
		g.init = 0;
		g.player = {1, 0};       // state 0 = sys, state 1 = env
		g.aps = {"d_0"};
		g.controllable = {true};
		g.state_color = {-1, -1};
		g.state_priority = {1, 1};
		g.trans.resize(2);
		g.trans[0].emplace_back("t", 1, 0);  // marked edge (color 0)
		g.trans[1].emplace_back("t", 0, -1); // unmarked edge
		g.edge_priority = {{1}, {-1}};

		int T1_size = 1;
		std::vector<omcat::QltType3> T3(1);
		T3[0].pos_m = 0; T3[0].pos_y = 0;
		std::vector<int> type_A = {1}; // pattern 1 (d_0 = true) feasible

		auto pg = alg_d::build_product_game(g, T1_size, T3, type_A, /*K=*/1);
		int base_n = g.num_states * T1_size;
		CHECK(pg.n_states > base_n);
		// The sys base state (0,rho=0) = index 0 must reach a stub with the
		// edge's priority (1), and that stub must have exactly one successor.
		REQUIRE(!pg.succs[0].empty());
		int stub = pg.succs[0][0];
		REQUIRE(stub >= base_n);
		CHECK(pg.priority[stub] == 1);
		CHECK(pg.succs[stub].size() == 1);
	}

	TEST_CASE("[ALG-D-56a] pattern-feasible D-pattern: sys wins") {
		alg_d::SynthGame g;
		g.num_states = 1;
		g.init = 0;
		g.player = {1};
		g.aps = {"d_0"};
		g.controllable = {true};
		g.state_color = {-1};
		g.state_priority = {1};   // "all" acceptance: priority 1 everywhere
		g.trans.resize(1);
		g.trans[0].emplace_back("0", 0, -1); // self-loop, requires d_0 = true
		g.edge_priority = {{-1}};

		int T1_size = 1;
		std::vector<omcat::QltType3> T3(1);
		T3[0].pos_m = 0; T3[0].pos_y = 0;
		std::vector<int> type_A = {1}; // D-pattern 1 feasible at (rho=0,rho'=0)

		auto pg = alg_d::build_product_game(g, T1_size, T3, type_A, /*K=*/1);
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.count(0));
	}

	TEST_CASE("[ALG-D-56b] pattern-infeasible D-pattern: dead end must NOT win for sys") {
		alg_d::SynthGame g;
		g.num_states = 1;
		g.init = 0;
		g.player = {1};
		g.aps = {"d_0"};
		g.controllable = {true};
		g.state_color = {-1};
		g.state_priority = {1};
		g.trans.resize(1);
		g.trans[0].emplace_back("0", 0, -1); // requires d_0 = true (pattern 1)
		g.edge_priority = {{-1}};

		int T1_size = 1;
		std::vector<omcat::QltType3> T3(1);
		T3[0].pos_m = 0; T3[0].pos_y = 0;
		std::vector<int> type_A = {0}; // only pattern 0 feasible — guard needs 1

		auto pg = alg_d::build_product_game(g, T1_size, T3, type_A, /*K=*/1);
		// Base sys state must have no successors (dead end).
		CHECK(pg.succs[0].empty());
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(W1.empty());
	}

	// AL-R1 through the product game: sys state 0 has two edges under the
	// same feasible D-pattern — one into sys state 1, whose only edge needs
	// an infeasible pattern (a dead end after pruning), and one into env
	// state 2 whose even self-loop env wins.  Both options lose.
	TEST_CASE("[ALG-D-56c] AL-R1: dead-end edge plus env-won edge is UNREALIZABLE") {
		alg_d::SynthGame g;
		g.num_states = 3;
		g.init = 0;
		g.player = {1, 1, 0};
		g.aps = {"d_0", "d_1"};
		g.controllable = {true, true};
		g.state_color = {-1, -1, -1};
		g.state_priority = {1, 1, 0};
		g.trans.resize(3);
		g.trans[0].emplace_back("0", 1, -1);   // pattern 1 (d_0 only): feasible
		g.trans[0].emplace_back("0", 2, -1);   // same pattern, into the env sink
		g.trans[1].emplace_back("1", 1, -1);   // needs d_1: no feasible pattern
		g.trans[2].emplace_back("t", 2, -1);   // env even sink
		g.edge_priority = {{-1, -1}, {-1}, {-1}};

		int T1_size = 1;
		std::vector<omcat::QltType3> T3(1);
		T3[0].pos_m = 0; T3[0].pos_y = 0;
		std::vector<int> type_A = {1};         // only pattern 1 feasible

		auto pg = alg_d::build_product_game(g, T1_size, T3, type_A, /*K=*/2);
		REQUIRE(pg.n_states >= 3);
		CHECK(pg.succs[1].empty());            // sys dead end
		CHECK(pg.succs[0].size() == 2);
		auto W1 = alg_d::zielonka_win_player1(pg);
		CHECK(!W1.count(0));
		CHECK(!W1.count(1));
		CHECK(!W1.count(2));
	}

} // TEST_SUITE("[Algorithm D: product game construction]")


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}