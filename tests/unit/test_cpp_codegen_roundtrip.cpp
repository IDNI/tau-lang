// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Round-trip test: emit C++ program from a known HoaAutomaton, write it to
// a temp file, compile it with g++ -O3 -flto, link + run a tiny driver,
// and check runtime behavior matches the synthesized strategy.

#include "test_init.h"
#include "cpp_codegen.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

using namespace idni::tau_lang;

namespace {

HoaAutomaton echo_spec() {
	HoaAutomaton a;
	a.num_states = 1;
	a.initial_state = 0;
	a.aps = {"in_sig", "out_sig"};
	a.edges.resize(1);
	a.edges[0].push_back(HoaEdge{"0&1",   0, false});
	a.edges[0].push_back(HoaEdge{"!0&!1", 0, false});
	a.state_accepting = {false};
	return a;
}

bool has_gpp() {
	return system("g++ --version >/dev/null 2>&1") == 0;
}

bool compile_and_run_echo(const std::string& header_src) {
	const char* hdr_path = "/tmp/_tau_codegen_test_ctrl.h";
	const char* main_path = "/tmp/_tau_codegen_test_main.cpp";
	const char* exe_path  = "/tmp/_tau_codegen_test_exe";

	{
		std::ofstream f(hdr_path);
		f << header_src;
	}
	{
		std::ofstream f(main_path);
		f <<
		    "#include \"_tau_codegen_test_ctrl.h\"\n"
		    "#include <cstdio>\n"
		    "int main() {\n"
		    "  EchoCtrl c;\n"
		    "  EchoCtrl::Inputs in;\n"
		    "  in.i_in_sig = true;\n"
		    "  auto o1 = c.step(in);\n"
		    "  if (!o1.ok || !o1.o_out_sig) { std::printf(\"FAIL1\\n\"); return 1; }\n"
		    "  in.i_in_sig = false;\n"
		    "  auto o2 = c.step(in);\n"
		    "  if (!o2.ok || o2.o_out_sig)  { std::printf(\"FAIL2\\n\"); return 2; }\n"
		    "  std::printf(\"OK\\n\");\n"
		    "  return 0;\n"
		    "}\n";
	}

	std::string cmd = std::string("g++ -O3 -flto -std=c++17 -I/tmp -o ")
	                + exe_path + " " + main_path + " 2>&1";
	// Ensure the header is findable via -I/tmp and the #include matches.
	int rc = system(cmd.c_str());
	if (rc != 0) return false;

	rc = system((std::string(exe_path) + " >/tmp/_tau_codegen_test_out").c_str());
	if (rc != 0) return false;

	std::ifstream out("/tmp/_tau_codegen_test_out");
	std::string line; std::getline(out, line);
	return line == "OK";
}

} // namespace

TEST_SUITE("cpp_codegen_roundtrip") {

	TEST_CASE("echo spec: emit → g++ -O3 → run passes") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		std::ostringstream os;
		emit_cpp_program_prop(echo_spec(), {"in_sig"}, {"out_sig"}, os, "EchoCtrl");
		CHECK(compile_and_run_echo(os.str()));
	}
}

// LG-23: the declare_open emitters were entirely untested (~400 lines,
// including the "};"-splice injection and the generated oracle protocol).
TEST_SUITE("cpp_codegen_open") {

	TEST_CASE("open emit: scaffolding is spliced into the class") {
		std::ostringstream os;
		emit_cpp_program_open_prop(echo_spec(), {"in_sig"}, {"out_sig"},
			{"o_out_sig"}, os, "EchoCtrl");
		std::string s = os.str();
		CHECK(s.find("open_streams()") != std::string::npos);
		CHECK(s.find("register_open_oracle") != std::string::npos);
		CHECK(s.find("unregister_open_oracle") != std::string::npos);
		CHECK(s.find("admissible_values_mask") != std::string::npos);
		CHECK(s.find("step_with_oracle_dispatch") != std::string::npos);
		CHECK(s.find("\"o_out_sig\"") != std::string::npos);
		// The splice must not have destroyed the base program.
		CHECK(s.find("Outputs step(") != std::string::npos);
		CHECK(s.find("enum class State") != std::string::npos);
	}

	TEST_CASE("open emit: g++ compile, oracle dispatch, mask, edge walk") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		std::ostringstream os;
		emit_cpp_program_open_prop(echo_spec(), {"in_sig"}, {"out_sig"},
			{"o_out_sig"}, os, "EchoCtrl");

		const char* hdr_path  = "/tmp/_tau_codegen_open_ctrl.h";
		const char* main_path = "/tmp/_tau_codegen_open_main.cpp";
		const char* exe_path  = "/tmp/_tau_codegen_open_exe";
		{ std::ofstream f(hdr_path); f << os.str(); }
		{
			std::ofstream f(main_path);
			f <<
			    "#include \"_tau_codegen_open_ctrl.h\"\n"
			    "#include <cstdio>\n"
			    "static const char* pick_true(const char*, void*) {\n"
			    "  return \"o_out_sig := 1\";\n"
			    "}\n"
			    "int main() {\n"
			    "  EchoCtrl c;\n"
			    "  // echo spec: both output values admissible from q0\n"
			    "  if (EchoCtrl::admissible_values_mask(\n"
			    "      EchoCtrl::State::q0, \"o_out_sig\") != 0x3)\n"
			    "    { std::printf(\"FAILMASK\\n\"); return 1; }\n"
			    "  // unknown stream is rejected on registration\n"
			    "  if (c.register_open_oracle(\"o_nope\", &pick_true, nullptr) != -1)\n"
			    "    { std::printf(\"FAILREG1\\n\"); return 2; }\n"
			    "  // no handler yet: dispatch must fail closed\n"
			    "  EchoCtrl::Inputs in;\n"
			    "  in.i_in_sig = true;\n"
			    "  if (c.step_with_oracle_dispatch(in).ok)\n"
			    "    { std::printf(\"FAILNOH\\n\"); return 3; }\n"
			    "  if (c.register_open_oracle(\"o_out_sig\", &pick_true, nullptr) != 0)\n"
			    "    { std::printf(\"FAILREG2\\n\"); return 4; }\n"
			    "  // oracle picks true; with in=true the 0&1 edge matches\n"
			    "  auto o1 = c.step_with_oracle_dispatch(in);\n"
			    "  if (!o1.ok || !o1.o_out_sig)\n"
			    "    { std::printf(\"FAIL1\\n\"); return 5; }\n"
			    "  // oracle still picks true; with in=false no edge matches\n"
			    "  in.i_in_sig = false;\n"
			    "  if (c.step_with_oracle_dispatch(in).ok)\n"
			    "    { std::printf(\"FAIL2\\n\"); return 6; }\n"
			    "  if (c.unregister_open_oracle(\"o_out_sig\") != 0)\n"
			    "    { std::printf(\"FAILUNREG\\n\"); return 7; }\n"
			    "  std::printf(\"OK\\n\");\n"
			    "  return 0;\n"
			    "}\n";
		}
		std::string cmd = std::string("g++ -O2 -std=c++17 -I/tmp -o ")
		                + exe_path + " " + main_path + " 2>&1";
		REQUIRE(system(cmd.c_str()) == 0);
		REQUIRE(system((std::string(exe_path)
			+ " >/tmp/_tau_codegen_open_out").c_str()) == 0);
		std::ifstream out("/tmp/_tau_codegen_open_out");
		std::string line; std::getline(out, line);
		CHECK(line == "OK");
	}
}
