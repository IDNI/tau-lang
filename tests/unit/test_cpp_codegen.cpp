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
