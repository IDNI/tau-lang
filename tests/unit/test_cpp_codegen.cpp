// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "cpp_codegen.h"

#include <sstream>
#include <string>

using namespace idni::tau_lang;

// Helpers.
static HoaAutomaton simple_mealy_echo() {
	// 1 state, one self-loop edge per input:
	//   state q0, guard 0 → q0 setting output 1
	//   state q0, guard !0 → q0 setting output !1
	// AP 0 = input "i", AP 1 = output "o".
	HoaAutomaton a;
	a.num_states = 1;
	a.initial_state = 0;
	a.aps = {"i", "o"};
	a.edges.resize(1);
	a.edges[0].push_back(HoaEdge{"0&1",  0, false});
	a.edges[0].push_back(HoaEdge{"!0&!1", 0, false});
	a.state_accepting = {false};
	return a;
}

TEST_SUITE("cpp_codegen") {

	TEST_CASE("emits valid C++17 for echo spec") {
		auto a = simple_mealy_echo();
		std::ostringstream os;
		emit_cpp_program_prop(a, {"i"}, {"o"}, os, "EchoCtrl");
		std::string s = os.str();
		// Basic structural assertions.
		CHECK(s.find("class EchoCtrl {") != std::string::npos);
		CHECK(s.find("enum class State") != std::string::npos);
		CHECK(s.find("State::q0") != std::string::npos);
		CHECK(s.find("struct Inputs") != std::string::npos);
		CHECK(s.find("struct Outputs") != std::string::npos);
		CHECK(s.find("Outputs step(const Inputs&") != std::string::npos);
		// Input and output identifiers.
		CHECK(s.find("i_i") != std::string::npos);
		CHECK(s.find("o_o") != std::string::npos);
		// Dispatch logic uses the input.
		CHECK(s.find("in.i_i") != std::string::npos);
		// Output assignment present.
		CHECK(s.find("o.o_o = true") != std::string::npos);
		CHECK(s.find("o.o_o = false") != std::string::npos);
	}

	TEST_CASE("emits ok=false fallback when no edge matches") {
		auto a = simple_mealy_echo();
		a.edges[0].pop_back();  // remove the !0&!1 edge → incomplete
		std::ostringstream os;
		emit_cpp_program_prop(a, {"i"}, {"o"}, os);
		std::string s = os.str();
		CHECK(s.find("o.ok = false") != std::string::npos);
	}

	TEST_CASE("multi-state emission mentions every state") {
		HoaAutomaton a;
		a.num_states = 3;
		a.initial_state = 0;
		a.aps = {"i", "o"};
		a.edges.resize(3);
		a.edges[0].push_back(HoaEdge{"0&1",  1, false});
		a.edges[1].push_back(HoaEdge{"!0&1", 2, false});
		a.edges[2].push_back(HoaEdge{"1",    0, false});
		a.state_accepting = {false, false, false};
		std::ostringstream os;
		emit_cpp_program_prop(a, {"i"}, {"o"}, os);
		std::string s = os.str();
		CHECK(s.find("State::q0") != std::string::npos);
		CHECK(s.find("State::q1") != std::string::npos);
		CHECK(s.find("State::q2") != std::string::npos);
		CHECK(s.find("case State::q0") != std::string::npos);
		CHECK(s.find("case State::q1") != std::string::npos);
		CHECK(s.find("case State::q2") != std::string::npos);
	}
}
