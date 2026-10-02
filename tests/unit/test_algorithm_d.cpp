// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for the synthesis game of ltlsynt: its HOA guard evaluator, its
// parser and the call that produces it.

#include "test_init.h"
#include <set>
#include "test_tau_helpers.h"
#include "ltl_aba.h"
#include "algorithm_d_game.h"

using namespace idni::tau_lang;


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
		auto g_r = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g_r.has_value());
		alg_d::synth_game g = g_r.value();
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

	// The header comes from an external process; a garbled or absurd header
	// is a parse error, never a game built on a garbage count.
	TEST_CASE("[ALG-D-32b] garbled header integers are a parse error") {
		const std::string tail = R"(
AP: 1 "p0"
acc-name: all
--BODY--
State: 0
[t] 0
--END--
)";
		CHECK(alg_d::parse_synth_game_hoa(
			"HOA: v1\nStates: abc\nStart: 0" + tail).has_error());
		CHECK(alg_d::parse_synth_game_hoa(
			"HOA: v1\nStates: 0\nStart: 0" + tail).has_error());
		CHECK(alg_d::parse_synth_game_hoa(
			"HOA: v1\nStates: 99999999999999999999\nStart: 0" + tail)
				.has_error());
		CHECK(alg_d::parse_synth_game_hoa(
			"HOA: v1\nStates: 1\nStart: -1" + tail).has_error());
		CHECK(alg_d::parse_synth_game_hoa(
			"HOA: v1\nStates: 1\nStart: x" + tail).has_error());
		// The state cap is the runtime parameter ltl_hoa_max_states.
		const long saved = ltl_hoa_max_states_param;
		ltl_hoa_max_states_param = 3;
		CHECK(alg_d::parse_synth_game_hoa(
			"HOA: v1\nStates: 4\nStart: 0" + tail).has_error());
		auto ok3_r = alg_d::parse_synth_game_hoa(
			"HOA: v1\nStates: 3\nStart: 0" + tail);
		REQUIRE(ok3_r.has_value());
		CHECK(ok3_r.value().num_states == 3);
		ltl_hoa_max_states_param = saved;
	}

	TEST_CASE("[ALG-D-32c] more atomic propositions than the game can "
	          "enumerate are refused") {
		std::string aps;
		for (int i = 0; i <= ltl_max_game_aps; ++i)
			aps += " \"p" + std::to_string(i) + "\"";
		std::string hoa = "HOA: v1\nStates: 1\nStart: 0\nAP: "
			+ std::to_string(ltl_max_game_aps + 1) + aps
			+ "\nacc-name: all\n--BODY--\nState: 0\n[t] 0\n--END--\n";
		CHECK(alg_d::parse_synth_game_hoa(hoa).has_error());
		// ltl_max_game_aps itself is accepted.
		std::string ok = "HOA: v1\nStates: 1\nStart: 0\nAP: "
			+ std::to_string(ltl_max_game_aps)
			+ aps.substr(0, aps.rfind(" \""))
			+ "\nacc-name: all\n--BODY--\nState: 0\n[t] 0\n--END--\n";
		auto ok_r = alg_d::parse_synth_game_hoa(ok);
		REQUIRE(ok_r.has_value());
		CHECK(ok_r.value().num_states == 1);
	}

	TEST_CASE("[ALG-D-32d] a decomposed multi-game text is refused") {
		std::string one = R"(HOA: v1
States: 1
Start: 0
AP: 1 "p0"
acc-name: all
--BODY--
State: 0
[t] 0
--END--
)";
		auto one_r = alg_d::parse_synth_game_hoa(one);
		REQUIRE(one_r.has_value());
		CHECK(one_r.value().num_states == 1);
		CHECK(alg_d::parse_synth_game_hoa(one + one).has_error());
	}

	TEST_CASE("[ALG-D-32e] a guard with an absurd AP index reads as false, "
	          "the DNF parser fails it") {
		CHECK(!alg_d::eval_guard("99999999999", 1, 2));
		CHECK(!alg_d::hoa_guard::to_dnf("99999999999").has_value());
		CHECK(alg_d::hoa_guard::to_dnf("0 & 1").has_value());
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
		auto g_r = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g_r.has_value());
		alg_d::synth_game g = g_r.value();
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
		auto g_r = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g_r.has_value());
		alg_d::synth_game g = g_r.value();
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
		auto g_r = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g_r.has_value());
		alg_d::synth_game g = g_r.value();
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
		auto g_r = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g_r.has_value());
		alg_d::synth_game g = g_r.value();
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
		auto g_r = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g_r.has_value());
		alg_d::synth_game g = g_r.value();
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
		auto g_r = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g_r.has_value());
		alg_d::synth_game g = g_r.value();
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
		auto g1_r = alg_d::parse_synth_game_hoa(hoa_trans);
		REQUIRE(g1_r.has_value());
		alg_d::synth_game g1 = g1_r.value();
		CHECK(g1.trans_acc);

		std::string hoa_no_trans = R"(HOA: v1
States: 1
Start: 0
AP: 1 "p0"
--BODY--
State: 0
--END--
)";
		auto g2_r = alg_d::parse_synth_game_hoa(hoa_no_trans);
		REQUIRE(g2_r.has_value());
		alg_d::synth_game g2 = g2_r.value();
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
		auto g_r = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g_r.has_value());
		alg_d::synth_game g = g_r.value();
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
		auto g_r = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g_r.has_value());
		alg_d::synth_game g = g_r.value();
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
		auto g_r = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g_r.has_value());
		alg_d::synth_game g = g_r.value();
		REQUIRE(g.player.size() == 3u);
		CHECK(g.player[0] == 0);
		CHECK(g.player[1] == 1);
		CHECK(g.player[2] == 1);
	}

	// Spot wraps the players of a large game onto indented lines; every
	// state past the first line would otherwise stay env-owned.
	TEST_CASE("[ALG-D-41b] a state-player header wrapped over lines assigns "
	          "every state") {
		std::string hoa = R"(HOA: v1
States: 4
Start: 0
AP: 2 "p0"
      "p1"
controllable-AP: 1
spot-state-player: 0 0
                   1 1
acc-name: all
Acceptance: 0 t
--BODY--
State: 0
[0] 2
[!0] 3
State: 1
[t] 2
State: 2
[t] 0
State: 3
[1] 1
[!1] 0
--END--
)";
		auto g_r = alg_d::parse_synth_game_hoa(hoa);
		REQUIRE(g_r.has_value());
		alg_d::synth_game g = g_r.value();
		REQUIRE(g.player.size() == 4u);
		CHECK(g.player[0] == 0);
		CHECK(g.player[1] == 0);
		CHECK(g.player[2] == 1);
		CHECK(g.player[3] == 1);
		REQUIRE(g.aps.size() == 2u);
		CHECK(g.aps[1] == "p1");
		CHECK(g.controllable[1]);
	}

	TEST_CASE("[ALG-D-61] the acceptance of a run that sees no colour") {
		CHECK(alg_d::acceptance_without_colors("Inf(0)") == false);
		CHECK(alg_d::acceptance_without_colors("Fin(0)") == true);
		CHECK(alg_d::acceptance_without_colors(" t") == true);
		CHECK(alg_d::acceptance_without_colors("f") == false);
		CHECK(alg_d::acceptance_without_colors(
			"Fin(2) & (Inf(1) | Fin(0))") == true);
		CHECK(alg_d::acceptance_without_colors(
			"Inf(0) | (Fin(1) & Inf(2))") == false);
		CHECK(alg_d::acceptance_without_colors("!Inf(3)") == true);
		CHECK_FALSE(alg_d::acceptance_without_colors("Inf(0) &").has_value());
		CHECK_FALSE(alg_d::acceptance_without_colors("Rabin").has_value());
	}

	TEST_CASE("[ALG-D-62] a parity acceptance is known, a Streett one is not") {
		auto game = [](const std::string& acc) {
			auto r = alg_d::parse_synth_game_hoa("HOA: v1\nStates: 1\n"
				"Start: 0\nAP: 1 \"p0\"\n" + acc + "--BODY--\n"
				"State: 0\n[t] 0\n--END--\n");
			REQUIRE(r.has_value());
			return r.value();
		};
		auto buchi = game("acc-name: Buchi\nAcceptance: 1 Inf(0)\n");
		CHECK(buchi.acc_known);
		CHECK_FALSE(buchi.acc_accepts_uncolored);
		auto cobuchi = game("acc-name: co-Buchi\nAcceptance: 1 Fin(0)\n");
		CHECK(cobuchi.acc_known);
		CHECK(cobuchi.acc_accepts_uncolored);
		auto parity = game("acc-name: parity max odd 3\n"
			"Acceptance: 3 Fin(2) & (Inf(1) | Fin(0))\n");
		CHECK(parity.acc_known);
		CHECK(parity.acc_accepts_uncolored);
		auto streett = game("acc-name: Streett 1\n"
			"Acceptance: 2 Fin(0) | Inf(1)\n");
		CHECK_FALSE(streett.acc_known);
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
		auto g_r = alg_d::parse_synth_game_hoa(fixture);
		REQUIRE(g_r.has_value());
		auto g = g_r.value();
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
		auto g_r = alg_d::parse_synth_game_hoa(fixture);
		REQUIRE(g_r.has_value());
		auto g = g_r.value();
		REQUIRE(g.state_priority.size() == 1);
		CHECK(g.state_priority[0] == 0);
	}
}

// ── CG-RT6: hoa_guard::to_dnf, the guard parser behind six consumers ─────

TEST_SUITE("[Algorithm D: hoa_guard::to_dnf]") {

	using alg_d::hoa_guard::to_dnf;

	static std::set<std::vector<std::pair<int,bool>>> as_set(
		const std::vector<alg_d::hoa_guard::cube>& d)
	{
		std::set<std::vector<std::pair<int,bool>>> out;
		for (const auto& c : d) {
			std::vector<std::pair<int,bool>> v;
			for (const auto& l : c) v.emplace_back(l.ap, l.pos);
			out.insert(v);
		}
		return out;
	}

	TEST_CASE("[DNF-01] constants and the empty label") {
		auto t = to_dnf("t");  REQUIRE(t);  CHECK(t->size() == 1); CHECK((*t)[0].empty());
		auto e = to_dnf("");   REQUIRE(e);  CHECK(e->size() == 1); CHECK((*e)[0].empty());
		auto f = to_dnf("f");  REQUIRE(f);  CHECK(f->empty());
	}

	TEST_CASE("[DNF-02] a disjunction is one cube per disjunct") {
		auto d = to_dnf("0|1");
		REQUIRE(d);
		CHECK(as_set(*d) == as_set({{{0,true}}, {{1,true}}}));
	}

	TEST_CASE("[DNF-03] conjunction distributes over a parenthesised disjunction") {
		auto d = to_dnf("(0|1)&2");
		REQUIRE(d);
		CHECK(as_set(*d) == as_set({{{0,true},{2,true}}, {{1,true},{2,true}}}));
	}

	TEST_CASE("[DNF-04] negation of a disjunction is the conjunction of negations") {
		auto d = to_dnf("!(0|1)");
		REQUIRE(d);
		CHECK(as_set(*d) == as_set({{{0,false},{1,false}}}));
		auto c = to_dnf("!(0&1)");
		REQUIRE(c);
		CHECK(as_set(*c) == as_set({{{0,false}}, {{1,false}}}));
	}

	TEST_CASE("[DNF-05] a contradictory cube is dropped, duplicates merged") {
		auto d = to_dnf("0&!0");
		REQUIRE(d);
		CHECK(d->empty());
		auto e = to_dnf("0&!0|1&1");
		REQUIRE(e);
		CHECK(as_set(*e) == as_set({{{1,true}}}));
	}

	TEST_CASE("[DNF-06] the cube cap refuses instead of truncating") {
		// (0|1)&(2|3)&(4|5) = 8 cubes; a cap of 4 must refuse.
		CHECK_FALSE(to_dnf("(0|1)&(2|3)&(4|5)", 4).has_value());
		auto ok = to_dnf("(0|1)&(2|3)&(4|5)", 16);
		REQUIRE(ok);
		CHECK(ok->size() == 8);
	}

	TEST_CASE("[DNF-07] malformed labels are refused, not partially read") {
		CHECK_FALSE(to_dnf("(0|1").has_value());
		CHECK_FALSE(to_dnf("0&").has_value());
		CHECK_FALSE(to_dnf("0 1").has_value());
		CHECK_FALSE(to_dnf("&0").has_value());
		CHECK_FALSE(to_dnf("x").has_value());
	}

	TEST_CASE("[DNF-08] whitespace and nested groups") {
		auto d = to_dnf(" ( 0 & ( 1 | !2 ) ) | 3 ");
		REQUIRE(d);
		CHECK(as_set(*d) == as_set({{{0,true},{1,true}}, {{0,true},{2,false}}, {{3,true}}}));
	}

} // TEST_SUITE("[Algorithm D: hoa_guard::to_dnf]")

// ── Phase 2: product game correctness ────────────────────────────────────

TEST_SUITE("[Algorithm D: ltlsynt game]") {

	// ── LS-10: call_ltlsynt_game's input mechanism ───────────────────────
	//
	// A formula past the Linux MAX_ARG_STRLEN cap of 131072 on a single
	// argument must still reach ltlsynt, or the game comes back empty and
	// every caller reads it as "unrealizable".
	TEST_CASE("[ALG-D-48] a formula past MAX_ARG_STRLEN still produces a game") {
		// Skip when ltlsynt is not on PATH: nothing to compare against.
		auto small = alg_d::call_ltlsynt_game("d_0", {}, {"d_0"});
		if (!small.has_value() || small->num_states == 0) return;

		// 140000 characters — just past the 131072-byte single-argument cap.
		// The conjunction is trivially reducible, so ltlsynt itself is cheap;
		// only the argument length is under test.
		std::string big = "d_0";
		big.reserve(150000);
		while (big.size() < 140000) big += " & d_0";
		auto g = alg_d::call_ltlsynt_game(big, {}, {"d_0"});
		REQUIRE(g.has_value());
		CHECK(g->num_states > 0);
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}