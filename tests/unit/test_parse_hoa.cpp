// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// parse_hoa: the HOA strategy parser behind every REALIZABLE verdict.
//
// LA-8 / SY-1 / SY-R3 / SY-4: a malformed or truncated strategy (timed-out
// ltlsynt, a stub on PATH, a garbled header) used to parse to the EMPTY
// automaton, which is_ltl_aba_realizable reads as trivially REALIZABLE with
// the ABA oracle skipped. It is now refused with ltl_synthesis_error.

#include "test_init.h"
#include "test_tau_helpers.h"

#include <chrono>
#include <csignal>
#include <cstdlib>

using namespace idni::tau_lang;

static const char* good_hoa =
	"HOA: v1\n"
	"States: 2\n"
	"Start: 0\n"
	"AP: 2 \"p0\" \"p1\"\n"
	"acc-name: all\n"
	"Acceptance: 0 t\n"
	"--BODY--\n"
	"State: 0\n"
	"[0&1] 1\n"
	"[!0] 0\n"
	"State: 1 {0}\n"
	"[t] 1\n"
	"--END--\n";

TEST_SUITE("parse_hoa") {

	TEST_CASE("[HOA-01] a well-formed strategy parses") {
		hoa_automaton aut = parse_hoa(good_hoa);
		CHECK(aut.num_states == 2);
		CHECK(aut.initial_state == 0);
		REQUIRE(aut.aps.size() == 2);
		CHECK(aut.aps[0] == "p0");
		CHECK(aut.aps[1] == "p1");
		REQUIRE(aut.edges.size() == 2);
		CHECK(aut.edges[0].size() == 2);
		CHECK(aut.edges[0][0].guard_label == "0&1");
		CHECK(aut.edges[0][0].dst == 1);
		CHECK(aut.edges[1].size() == 1);
		// state 1 carries the acceptance mark: its incoming edges inherit it
		CHECK(aut.edges[0][0].accepting);
		CHECK(aut.state_accepting[1]);
	}

	TEST_CASE("[HOA-02] text after --END-- is not spliced in (SY-4)") {
		std::string two = std::string(good_hoa)
			+ "HOA: v1\nStates: 7\n--BODY--\nState: 0\n[t] 0\n--END--\n";
		hoa_automaton aut = parse_hoa(two);
		CHECK(aut.num_states == 2);
		CHECK(aut.edges[0].size() == 2);
	}

	TEST_CASE("[HOA-03] garbage state count is refused") {
		CHECK_THROWS_AS(parse_hoa("HOA: v1\nStates: garbage\n--BODY--\n--END--\n"),
			ltl_synthesis_error);
	}

	TEST_CASE("[HOA-04] negative and zero state counts are refused") {
		CHECK_THROWS_AS(parse_hoa("HOA: v1\nStates: -2\n--BODY--\n--END--\n"),
			ltl_synthesis_error);
		CHECK_THROWS_AS(parse_hoa("HOA: v1\nStates: 0\n--BODY--\n--END--\n"),
			ltl_synthesis_error);
	}

	TEST_CASE("[HOA-05] an absurd state count is refused instead of resized (SY-R3)") {
		CHECK_THROWS_AS(parse_hoa("HOA: v1\nStates: 99999999999\n--BODY--\n--END--\n"),
			ltl_synthesis_error);
		CHECK_THROWS_AS(parse_hoa("HOA: v1\nStates: 2000000000\n--BODY--\n--END--\n"),
			ltl_synthesis_error);
	}

	TEST_CASE("[HOA-06] no States: header is refused") {
		CHECK_THROWS_AS(parse_hoa("HOA: v1\nStart: 0\n--BODY--\nState: 0\n[t] 0\n--END--\n"),
			ltl_synthesis_error);
		CHECK_THROWS_AS(parse_hoa(""), ltl_synthesis_error);
		CHECK_THROWS_AS(parse_hoa("this is not a verdict\n"), ltl_synthesis_error);
	}

	TEST_CASE("[HOA-07] header without --BODY-- (truncated) is refused") {
		CHECK_THROWS_AS(parse_hoa("HOA: v1\nStates: 1\nStart: 0\n"),
			ltl_synthesis_error);
	}

	TEST_CASE("[HOA-08] out-of-range states and edges are dropped, not indexed (LT-10)") {
		hoa_automaton aut = parse_hoa(
			"HOA: v1\nStates: 1\nStart: 0\nAP: 1 \"p0\"\n--BODY--\n"
			"State: 0\n[0] 5\n[!0] 0\nState: 3\n[t] 0\n--END--\n");
		CHECK(aut.num_states == 1);
		REQUIRE(aut.edges.size() == 1);
		CHECK(aut.edges[0].size() == 1);
		CHECK(aut.edges[0][0].dst == 0);
	}

	TEST_CASE("[HOA-09] a 1-state strategy with no edge is not executable (LA-R6)") {
		// parse_hoa accepts it (it is a well-formed header + body); the
		// encoder must refuse to execute it as `always T`.
		hoa_automaton aut = parse_hoa(
			"HOA: v1\nStates: 1\nStart: 0\nAP: 1 \"p0\"\n--BODY--\nState: 0\n--END--\n");
		CHECK(aut.num_states == 1);
		CHECK(aut.edges[0].empty());
	}
}

// ── SY-RT2: spawn_capture contract (re-port of the pre-rebase SPAWN suite) ──
//
// classify_spot_exit depends on the 127 / 128+signo encodings, and the
// pipe drain must outrun a child that writes more than the pipe buffer.

TEST_SUITE("spawn_capture") {

	struct EnvGuard {
		std::string key;
		std::string old_val;
		bool had;
		EnvGuard(const char* k, const char* v) : key(k) {
			const char* c = std::getenv(k);
			had = c != nullptr;
			if (had) old_val = c;
			setenv(k, v, 1);
		}
		~EnvGuard() {
			if (had) setenv(key.c_str(), old_val.c_str(), 1);
			else unsetenv(key.c_str());
		}
	};

	// NOTE (SY-3): the missing SIGKILL escalation for TERM-ignoring children
	// is deliberately not tested — a faithful test would hang the suite.
	TEST_CASE("[SPAWN-01] timeout kills a slow child promptly with exit >= 128") {
		auto t0 = std::chrono::steady_clock::now();
		auto [out, code] = spawn_capture({"sleep", "10"}, 1);
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::steady_clock::now() - t0).count();
		CHECK(elapsed < 5);
		CHECK(code >= 128);
		CHECK(out.empty());
	}

	TEST_CASE("[SPAWN-02] nonexistent binary returns exit code 127") {
		auto [out, code] = spawn_capture({"definitely_not_a_binary_xyz_12345"});
		CHECK(code == 127);
		CHECK(out.empty());
	}

	// IN-N1 / Batch 3: a missing backend is no verdict -- call_ltlsynt
	// throws instead of answering {false, ""} (= UNREALIZABLE).
	TEST_CASE("[SPAWN-03] call_ltlsynt without Spot throws ltl_synthesis_error") {
		EnvGuard g("PATH", "/nonexistent");
		CHECK_THROWS_AS(call_ltlsynt("F(p0)", {}, {"p0"}), ltl_synthesis_error);
	}

	TEST_CASE("[SPAWN-04] 70KB of child output round-trips through the pipe") {
		auto [out, code] = spawn_capture(
			{"dd", "if=/dev/zero", "bs=70000", "count=1"});
		CHECK(code == 0);
		CHECK(out.size() == 70000);
	}

	TEST_CASE("[SPAWN-05] empty argv returns {empty, -1}") {
		auto [out, code] = spawn_capture({});
		CHECK(code == -1);
		CHECK(out.empty());
	}

	TEST_CASE("[SPAWN-06] signal exit is encoded as 128 + signo") {
		auto [out, code] = spawn_capture({"sh", "-c", "kill -TERM $$"});
		CHECK(code == 128 + SIGTERM); // 143
	}

	// SY-RT4 / SY-R5: the TAU_LTL_TIMEOUT_SEC parser.
	TEST_CASE("[TIMEOUT-01] unset keeps the 60s default") {
		unsetenv("TAU_LTL_TIMEOUT_SEC");
		CHECK(ltl_timeout_sec() == 60);
	}
	TEST_CASE("[TIMEOUT-02] a number is taken verbatim; 0 disables") {
		{ EnvGuard g("TAU_LTL_TIMEOUT_SEC", "5"); CHECK(ltl_timeout_sec() == 5); }
		{ EnvGuard g("TAU_LTL_TIMEOUT_SEC", "0"); CHECK(ltl_timeout_sec() == 0); }
	}
	TEST_CASE("[TIMEOUT-03] text garbage and negatives keep the default") {
		{ EnvGuard g("TAU_LTL_TIMEOUT_SEC", "abc"); CHECK(ltl_timeout_sec() == 60); }
		{ EnvGuard g("TAU_LTL_TIMEOUT_SEC", "12x"); CHECK(ltl_timeout_sec() == 60); }
		{ EnvGuard g("TAU_LTL_TIMEOUT_SEC", "-3");  CHECK(ltl_timeout_sec() == 60); }
		{ EnvGuard g("TAU_LTL_TIMEOUT_SEC", "");    CHECK(ltl_timeout_sec() == 60); }
	}
	// SY-R5: 2^32 used to truncate to 0 (watchdog silently off) and 2^31
	// to a negative; both are clamped to the one-day maximum now.
	TEST_CASE("[TIMEOUT-04] range garbage is clamped, never truncated to 0") {
		{ EnvGuard g("TAU_LTL_TIMEOUT_SEC", "4294967296");
		  CHECK(ltl_timeout_sec() == (int)ltl_timeout_sec_max); }
		{ EnvGuard g("TAU_LTL_TIMEOUT_SEC", "2147483648");
		  CHECK(ltl_timeout_sec() == (int)ltl_timeout_sec_max); }
		{ EnvGuard g("TAU_LTL_TIMEOUT_SEC", "99999999999999999999");
		  CHECK(ltl_timeout_sec() == 60); }   // strtol ERANGE: not a number
		{ EnvGuard g("TAU_LTL_TIMEOUT_SEC", "86400");
		  CHECK(ltl_timeout_sec() == 86400); }
	}

} // TEST_SUITE("spawn_capture")


