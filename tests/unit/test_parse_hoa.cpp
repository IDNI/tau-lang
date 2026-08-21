// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// parse_hoa: the HOA strategy parser behind every REALIZABLE verdict.
//
// LA-8 / SY-1 / SY-R3 / SY-4: a malformed or truncated strategy (timed-out
// ltlsynt, a stub on PATH, a garbled header) used to parse to the EMPTY
// automaton, which is_ltl_aba_realizable reads as trivially REALIZABLE with
// the ABA oracle skipped. It is now refused with ltl_synthesis_error.

#include "test_init.h"
#include "test_tau_helpers.h"

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
		HoaAutomaton aut = parse_hoa(good_hoa);
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
		HoaAutomaton aut = parse_hoa(two);
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
		HoaAutomaton aut = parse_hoa(
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
		HoaAutomaton aut = parse_hoa(
			"HOA: v1\nStates: 1\nStart: 0\nAP: 1 \"p0\"\n--BODY--\nState: 0\n--END--\n");
		CHECK(aut.num_states == 1);
		CHECK(aut.edges[0].empty());
	}
}
