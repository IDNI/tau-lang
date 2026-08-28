// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Round-trip test: emit C++ program from a known hoa_automaton, write it to
// a temp file, compile it with g++ -O3 -flto, link + run a tiny driver,
// and check runtime behavior matches the synthesized strategy.

#include "test_init.h"
#include "cpp_codegen.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>

// CG-N11: per-process scratch directory (mkdtemp) instead of fixed,
// predictable /tmp names -- concurrent checkouts running this suite used
// to clobber each other's headers/binaries.
static const std::string& cg_tmp_dir() {
	static const std::string dir = [] {
		std::string t = "/tmp/tau_cg_XXXXXX";
		char* p = ::mkdtemp(t.data());
		return std::string(p ? p : "/tmp");
	}();
	return dir;
}
static std::string cg_tmp(const char* name) { return cg_tmp_dir() + "/" + name; }


using namespace idni::tau_lang;

namespace {

// PID-suffixed temp path prefix — avoids CG-N11-style collisions with the
// other codegen test files' fixed /tmp/_tau_* names when ctest runs them
// concurrently.
std::string unique_tmp(const std::string& suffix) {
	return cg_tmp("_tau_cg_rt_") + std::to_string(::getpid()) + "_" + suffix;
}

hoa_automaton echo_spec() {
	hoa_automaton a;
	a.num_states = 1;
	a.initial_state = 0;
	a.aps = {"in_sig", "out_sig"};
	a.edges.resize(1);
	a.edges[0].push_back(hoa_edge{"0&1",   0, false});
	a.edges[0].push_back(hoa_edge{"!0&!1", 0, false});
	a.state_accepting = {false};
	return a;
}

bool has_gpp() {
	return system("g++ --version >/dev/null 2>&1") == 0;
}

bool compile_and_run_echo(const std::string& header_src) {
	const std::string hdr_path = cg_tmp("_tau_codegen_test_ctrl.h");
	const std::string main_path = cg_tmp("_tau_codegen_test_main.cpp");
	const std::string exe_path = cg_tmp("_tau_codegen_test_exe");

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
		    "  echo_ctrl c;\n"
		    "  echo_ctrl::inputs in;\n"
		    "  in.in_sig = true;\n"
		    "  auto o1 = c.step(in);\n"
		    "  if (!o1.ok || !o1.out_sig) { std::printf(\"FAIL1\\n\"); return 1; }\n"
		    "  in.in_sig = false;\n"
		    "  auto o2 = c.step(in);\n"
		    "  if (!o2.ok || o2.out_sig)  { std::printf(\"FAIL2\\n\"); return 2; }\n"
		    "  std::printf(\"OK\\n\");\n"
		    "  return 0;\n"
		    "}\n";
	}

	std::string cmd = std::string("g++ -O3 -flto -std=c++17 -I" + cg_tmp_dir() + " -o ")
	                + exe_path + " " + main_path + " 2>&1";
	// Ensure the header is findable via -I<dir> and the #include matches.
	int rc = system(cmd.c_str());
	if (rc != 0) return false;

	rc = system((std::string(exe_path) + " >" + cg_tmp("_tau_codegen_test_out")).c_str());
	if (rc != 0) return false;

	std::ifstream out(cg_tmp("_tau_codegen_test_out"));
	std::string line; std::getline(out, line);
	return line == "OK";
}

// Generic compile+run: writes `header_src` to a uniquely-named header,
// wraps `main_body` (the code inside int main(){...}, referencing the
// header's class directly) with the #include + boilerplate, compiles,
// runs, and returns the first line of stdout (empty string on failure).
std::string compile_and_run(const std::string& header_src,
                             const std::string& main_body,
                             const std::string& tag,
                             const std::string& preamble = "") {
	std::string hdr_path  = unique_tmp(tag + ".h");
	std::string hdr_name  = hdr_path.substr(hdr_path.rfind('/') + 1);
	std::string main_path = unique_tmp(tag + "_main.cpp");
	std::string exe_path  = unique_tmp(tag + "_exe");
	std::string out_path  = unique_tmp(tag + "_out");

	{ std::ofstream f(hdr_path); f << header_src; }
	{
		std::ofstream f(main_path);
		f << "#include \"" << hdr_name << "\"\n"
		     "#include <cstdio>\n"
		     "#include <cstring>\n"
		  << preamble
		  << "int main() {\n" << main_body << "\n}\n";
	}

	std::string cmd = "g++ -O2 -std=c++17 -I" + cg_tmp_dir() + " -o " + exe_path
	                 + " " + main_path + " 2>&1";
	if (system(cmd.c_str()) != 0) return "";
	if (system((exe_path + " >" + out_path).c_str()) != 0) return "";
	std::ifstream out(out_path);
	std::string line; std::getline(out, line);
	return line;
}

} // namespace

TEST_SUITE("cpp_codegen_roundtrip") {

	TEST_CASE("echo spec: emit → g++ -O3 → run passes") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		auto d = build_program_desc_prop(
			echo_spec(), {"in_sig"}, {"out_sig"}, "echo_ctrl");
		REQUIRE_FALSE(d.needs_tau_link);
		std::ostringstream os;
		emit_program(d, os);
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

		const std::string hdr_path = cg_tmp("_tau_codegen_open_ctrl.h");
		const std::string main_path = cg_tmp("_tau_codegen_open_main.cpp");
		const std::string exe_path = cg_tmp("_tau_codegen_open_exe");
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
		std::string cmd = std::string("g++ -O2 -std=c++17 -I" + cg_tmp_dir() + " -o ")
		                + exe_path + " " + main_path + " 2>&1";
		REQUIRE(system(cmd.c_str()) == 0);
		REQUIRE(system((std::string(exe_path)
			+ " >" + cg_tmp("_tau_codegen_open_out")).c_str()) == 0);
		std::ifstream out(cg_tmp("_tau_codegen_open_out"));
		std::string line; std::getline(out, line);
		CHECK(line == "OK");
	}
	// ── Re-ports of the pre-rebase compiled regressions (CG-RT1/CG-RT2/CG-RT5) ──

	// CG-N2 (compiled): the paren'd disjunctive conjunct "(0|1)" must gate
	// the edge -- with a=F,b=F,c=T the edge must NOT fire.
	TEST_CASE("[CG-GUARD-02] compiled: parenthesised disjunctive conjunct must gate the edge") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		hoa_automaton a;
		a.num_states = 1;
		a.initial_state = 0;
		a.aps = {"a", "b", "c", "o"};
		a.edges.resize(1);
		a.edges[0].push_back(hoa_edge{"(0|1)&2&3", 0, false});
		a.edges[0].push_back(hoa_edge{"!3", 0, false});
		std::ostringstream os;
		emit_cpp_program_prop(a, {"a", "b", "c"}, {"o"}, os, "ParenGate");
		std::string result = compile_and_run(os.str(),
			"  ParenGate p;\n"
			"  ParenGate::Inputs in;\n"
			"  in.i_a = false; in.i_b = false; in.i_c = true;\n"
			"  auto o = p.step(in);\n"
			"  std::printf(\"%s\\n\", (o.ok && !o.o_o) ? \"OK\" : \"WRONGFIRE\");\n",
			"cgn2");
		CHECK(result == "OK");
	}

	// CG-N9 (compiled): a guard label "f" listed first must never fire.
	TEST_CASE("[CG-GUARD-03] compiled: guard 'f' must never fire") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		hoa_automaton a;
		a.num_states = 2;
		a.initial_state = 0;
		a.aps = {"o"};
		a.edges.resize(2);
		a.edges[0].push_back(hoa_edge{"f", 0, false}); // must never fire
		a.edges[0].push_back(hoa_edge{"t", 1, false}); // must always fire
		a.edges[1].push_back(hoa_edge{"t", 1, false});
		std::ostringstream os;
		emit_cpp_program_prop(a, {}, {"o"}, os, "DeadGuard");
		std::string result = compile_and_run(os.str(),
			"  DeadGuard d;\n"
			"  DeadGuard::Inputs in;\n"
			"  auto o = d.step(in);\n"
			"  std::printf(\"%s\\n\", (o.ok && d.state() == DeadGuard::State::q1) "
			"? \"OK\" : \"STUCK_AT_Q0\");\n",
			"cgn9");
		CHECK(result == "OK");
	}

	// CG-RT2: multi-state compiled roundtrip -- full cycle plus the
	// no-matching-edge ok==false path.
	TEST_CASE("[CG-RT-02] compiled 3-state roundtrip: full cycle + ok=false on no match") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		hoa_automaton a;
		a.num_states = 3;
		a.initial_state = 0;
		a.aps = {"i", "o"};
		a.edges.resize(3);
		a.edges[0].push_back(hoa_edge{"0&1",  1, false});  // q0 -[i&o]-> q1
		a.edges[1].push_back(hoa_edge{"!0&1", 2, false});  // q1 -[!i&o]-> q2
		a.edges[2].push_back(hoa_edge{"1",    0, false});  // q2 -[o]-> q0
		std::ostringstream os;
		emit_cpp_program_prop(a, {"i"}, {"o"}, os, "Cycle3");
		std::string result = compile_and_run(os.str(),
			"  Cycle3 c;\n"
			"  Cycle3::Inputs in;\n"
			"  in.i_i = true;\n"
			"  auto s0 = c.step(in);  // q0 -> q1\n"
			"  in.i_i = false;\n"
			"  auto s1 = c.step(in);  // q1 -> q2\n"
			"  auto s2 = c.step(in);  // q2 -[o]-> q0 (guard '1' ignores input)\n"
			"  bool cycle_ok = s0.ok && s1.ok && s2.ok "
			"&& c.state() == Cycle3::State::q0;\n"
			"  in.i_i = false;\n"
			"  auto s3 = c.step(in);  // q0 requires i&o: fails\n"
			"  std::printf(\"%s\\n\", (cycle_ok && !s3.ok) ? \"OK\" : \"BROKEN\");\n",
			"cgrt02");
		CHECK(result == "OK");
	}

	// CG-N3 / CG-RT5: step_with_oracle_dispatch only filled ap[] for
	// inputs and DECLARED-open outputs; an undeclared positive output
	// literal in the guard (here "o2", never registered open) stayed at
	// its default `false`, so an edge whose guard requires it true could
	// never fire even though the oracle satisfied every declared
	// constraint.  Each cube is now evaluated on its input+declared
	// projection and the undeclared outputs are assigned from it.
	TEST_CASE("[CG-OPEN-02] compiled: undeclared positive output literal must still be derivable") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		hoa_automaton a;
		a.num_states = 1;
		a.initial_state = 0;
		a.aps = {"i", "o1", "o2"};
		a.edges.resize(1);
		a.edges[0].push_back(hoa_edge{"0&1&2", 0, false});
		a.edges[0].push_back(hoa_edge{"!0&!1&!2", 0, false});
		std::ostringstream os;
		emit_cpp_program_open_prop(a, {"i"}, {"o1", "o2"}, {"o1"}, os, "OpenDispatch");
		std::string result = compile_and_run(os.str(),
			"  OpenDispatch d;\n"
			"  int rc = d.register_open_oracle(\"o1\", &oracle, nullptr);\n"
			"  if (rc != 0) { std::printf(\"REG_FAIL\\n\"); return 0; }\n"
			"  OpenDispatch::Inputs in;\n"
			"  in.i_i = true;\n"
			"  auto o = d.step_with_oracle_dispatch(in);\n"
			"  if (!o.ok) { std::printf(\"NO_EDGE_MATCHED\\n\"); return 0; }\n"
			"  if (!o.o_o2) { std::printf(\"O2_NOT_ASSIGNED\\n\"); return 0; }\n"
			"  // a cube whose DECLARED part disagrees with the oracle must not fire\n"
			"  in.i_i = false;\n"
			"  auto o2 = d.step_with_oracle_dispatch(in);\n"
			"  std::printf(\"%s\\n\", o2.ok ? \"WRONGFIRE\" : \"OK\");\n",
			"cgopen02",
			"static const char* oracle(const char*, void*) {\n"
			"  static const char* r = \"o1 := 1\";\n"
			"  return r;\n"
			"}\n");
		CHECK(result == "OK");
	}

}
