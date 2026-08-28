// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "cpp_codegen.h"

#include <sstream>
#include <string>

using namespace idni::tau_lang;

// Helpers.
static hoa_automaton simple_mealy_echo() {
	// 1 state, one self-loop edge per input:
	//   state q0, guard 0 → q0 setting output 1
	//   state q0, guard !0 → q0 setting output !1
	// AP 0 = input "i", AP 1 = output "o".
	hoa_automaton a;
	a.num_states = 1;
	a.initial_state = 0;
	a.aps = {"i", "o"};
	a.edges.resize(1);
	a.edges[0].push_back(hoa_edge{"0&1",  0, false});
	a.edges[0].push_back(hoa_edge{"!0&!1", 0, false});
	a.state_accepting = {false};
	return a;
}

TEST_SUITE("cpp_codegen") {

	TEST_CASE("emits valid C++ for echo spec") {
		auto a = simple_mealy_echo();
		auto d = build_program_desc_prop(a, {"i"}, {"o"}, "echo_ctrl");
		std::ostringstream os;
		emit_program(d, os);
		std::string s = os.str();
		// Basic structural assertions.
		CHECK(s.find("class echo_ctrl {") != std::string::npos);
		CHECK(s.find("struct inputs") != std::string::npos);
		CHECK(s.find("struct outputs") != std::string::npos);
		CHECK(s.find("outputs step(const inputs&") != std::string::npos);
		// Input and output identifiers.
		CHECK(s.find("bool i") != std::string::npos);
		CHECK(s.find("bool o") != std::string::npos);
		// Dispatch logic uses the input.
		CHECK(s.find("in.i") != std::string::npos);
		// The strategy table carries the edges' guard/assignment literals.
		CHECK(s.find("strat_.edges[0].push_back({{1,1}, 0});") != std::string::npos);
		CHECK(s.find("strat_.edges[0].push_back({{-1,-1}, 0});") != std::string::npos);
	}

	TEST_CASE("emits ok=false fallback when no edge matches") {
		auto a = simple_mealy_echo();
		a.edges[0].pop_back();  // remove the !0&!1 edge → incomplete
		auto d = build_program_desc_prop(a, {"i"}, {"o"});
		std::ostringstream os;
		emit_program(d, os);
		std::string s = os.str();
		CHECK(s.find("o.ok = false") != std::string::npos);
	}

	TEST_CASE("multi-state program_desc mentions every state") {
		hoa_automaton a;
		a.num_states = 3;
		a.initial_state = 0;
		a.aps = {"i", "o"};
		a.edges.resize(3);
		a.edges[0].push_back(hoa_edge{"0&1",  1, false});
		a.edges[1].push_back(hoa_edge{"!0&1", 2, false});
		a.edges[2].push_back(hoa_edge{"1",    0, false});
		a.state_accepting = {false, false, false};
		auto d = build_program_desc_prop(a, {"i"}, {"o"});
		CHECK(d.num_states == 3);
		REQUIRE(d.edges.size() == 3);
		CHECK(d.edges[0].size() == 1);
		CHECK(d.edges[0][0].dst == 1);
		CHECK(d.edges[1][0].dst == 2);
		CHECK(d.edges[2][0].dst == 0);
		std::ostringstream os;
		emit_program(d, os);
		std::string s = os.str();
		CHECK(s.find("strat_.num_states = 3") != std::string::npos);
	}
}

// ── LG-4: disjunctive / parenthesised HOA guards ─────────────────────────────
//
// Every emitter tokenised the guard label by top-level '&' and then read each
// conjunct with `for (char c : idx_str) if (isdigit(c)) idx = idx*10+(c-'0')`.
// That loop SKIPS '|', '(' and ')' instead of rejecting them, so:
//   "0|1"   → the digits are concatenated across the '|' → the single literal
//             (1, true): wrong AP, and the other disjunct vanishes;
//   "(0|1)" → the leading '(' fails the isdigit test and the whole conjunct is
//             dropped, silently widening the guard to `true`.
// Spot prints strategy edge labels as sums of products, so both shapes are
// real.  The assertions below are on the EMITTED TEXT, so they survive any
// choice of internal representation.

TEST_SUITE("cpp_codegen guard parsing (LG-4)") {

	// aps: 0 = input i0, 1 = input i1, 2 = output o.
	static hoa_automaton two_input_one_output(const char* guard) {
		hoa_automaton a;
		a.num_states = 1;
		a.initial_state = 0;
		a.aps = {"i0", "i1", "o"};
		a.edges.resize(1);
		a.edges[0].push_back(hoa_edge{guard, 0, false});
		a.state_accepting = {false};
		return a;
	}

	// "0|1" fires on EITHER input.  The digit-concatenating scan reads it as
	// AP 1 alone, so the emitted dispatch never mentions i0.
	TEST_CASE("[LG4-01] disjunctive guard keeps both disjuncts") {
		auto a = two_input_one_output("0|1");
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {"i0", "i1"}, {"o"}, "Disj");
		emit_program(d, os);
		std::string s = os.str();
		CHECK(s.find("in.i0") != std::string::npos);
		CHECK(s.find("in.i1") != std::string::npos);
	}

	// "(0|1)&2" assigns the output and gates on either input.  The '(' makes
	// the first conjunct disappear, so the emitted guard becomes `true` — the
	// generated program reacts to inputs it was never supposed to react to.
	TEST_CASE("[LG4-02] parenthesised conjunct is not dropped") {
		auto a = two_input_one_output("(0|1)&2");
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {"i0", "i1"}, {"o"}, "ParenDisj");
		emit_program(d, os);
		std::string s = os.str();
		CHECK(s.find("o.o = true") != std::string::npos);
		CHECK(s.find("in.i0") != std::string::npos);
		CHECK(s.find("in.i1") != std::string::npos);
	}

	// Regression guard: flat conjunctions must keep emitting exactly what they
	// did before the consolidation.
	TEST_CASE("[LG4-03] flat conjunctive guards unchanged") {
		auto a = two_input_one_output("0&!1&2");
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {"i0", "i1"}, {"o"}, "Flat");
		emit_program(d, os);
		std::string s = os.str();
		// Flag-only prop emission bakes guard polarity into the strategy
		// table's ints, not a textual "!in.iN" conditional.
		CHECK(s.find("strat_.edges[0].push_back({{1,-1,1}, 0});") != std::string::npos);
		CHECK(s.find("o.o = true") != std::string::npos);
	}

} // TEST_SUITE("cpp_codegen guard parsing (LG-4)")


// ── LG-5: trivially-realizable solutions (num_states == 0) ───────────────────
//
// `solve_ltl_aba` returns `num_states = 0` on the constant-output fast path
// and `is_ltl_aba_realizable` calls that REALIZABLE, so tau_codegen hands the
// empty automaton straight to the emitters.  The State enum body then comes
// out empty while `State state_ = State::q0;` still names q0 — the generated
// header does not compile, and the CLI exits 0 without a word.

TEST_SUITE("cpp_codegen trivial solution (LG-5)") {

	static hoa_automaton empty_automaton() {
		hoa_automaton a;
		a.num_states = 0;
		a.initial_state = 0;
		a.aps = {"i", "o"};
		a.state_accepting = {};
		return a;
	}

	TEST_CASE("[LG5-01] prop emitter does not reference q0 with an empty enum") {
		auto a = empty_automaton();
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {"i"}, {"o"}, "Trivial");
		emit_program(d, os);
		std::string s = os.str();
		CHECK(s.find("State::q0") == std::string::npos);
		CHECK(s.find("class Trivial {") != std::string::npos);
		CHECK(s.find("outputs step(const inputs&") != std::string::npos);
	}

	TEST_CASE("[LG5-02] open-prop emitter does not reference q0 either") {
		auto a = empty_automaton();
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {"i"}, {"o"}, "TrivialOpen", /*revisable=*/false, {"o"});
		emit_program(d, os);
		std::string s = os.str();
		CHECK(s.find("State::q0") == std::string::npos);
	}

} // TEST_SUITE("cpp_codegen trivial solution (LG-5)")
