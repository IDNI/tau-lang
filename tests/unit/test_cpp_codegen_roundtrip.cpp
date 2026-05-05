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
