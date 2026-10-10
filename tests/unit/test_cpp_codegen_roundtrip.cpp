// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Round-trip test: emit C++ program from a known hoa_automaton, write it to
// a temp file, compile it with the host C++ compiler, link + run a tiny
// driver, and check runtime behavior matches the synthesized strategy.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

// CG-N11: per-process scratch directory instead of fixed, predictable /tmp
// names -- concurrent checkouts running this suite used to clobber each
// other's headers/binaries. tau_test_tmp puts it under the platform temp dir
// with a random suffix, so the suite also builds and runs on MinGW and MSVC.
static const std::string& cg_tmp_dir() {
	static const std::string dir = tau_test_tmp("cg_roundtrip").string();
	return dir;
}
static std::string cg_tmp(const std::string& name) {
	return cg_tmp_dir() + "/" + name;
}

using namespace idni::tau_lang;

namespace {

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

bool has_cxx() { return tau_test_cxx_available(tau_test_cxx()); }

bool compile_and_run_echo(const std::string& header_src) {
	const std::string hdr_path = cg_tmp("_tau_codegen_test_ctrl.h");
	const std::string main_path = cg_tmp("_tau_codegen_test_main.cpp");
	const std::string exe_path = cg_tmp("_tau_codegen_test_exe")
		+ tau_test_exe_suffix();

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

	// The header is found through -I<scratch dir>, matching the #include.
	auto built = tau_test_compile(exe_path, { main_path }, cg_tmp_dir(), 17,
		/*ndebug=*/false, /*lto=*/true);
	if (!built.ok) { MESSAGE(built.out); return false; }
	auto run = tau_test_run({ exe_path });
	if (run.exit_code != 0) { MESSAGE(run.out << run.err); return false; }
	std::istringstream out(run.out);
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
	std::string hdr_path  = cg_tmp("_tau_cg_rt_" + tag + ".h");
	std::string hdr_name  = std::filesystem::path(hdr_path).filename().string();
	std::string main_path = cg_tmp("_tau_cg_rt_" + tag + "_main.cpp");
	std::string exe_path  = cg_tmp("_tau_cg_rt_" + tag + "_exe")
		+ tau_test_exe_suffix();

	{ std::ofstream f(hdr_path); f << header_src; }
	{
		std::ofstream f(main_path);
		f << "#include \"" << hdr_name << "\"\n"
		     "#include <cstdio>\n"
		     "#include <cstring>\n"
		  << preamble
		  << "int main() {\n" << main_body << "\n}\n";
	}

	if (!tau_test_compile(exe_path, { main_path }, cg_tmp_dir()).ok)
		return "";
	auto run = tau_test_run({ exe_path });
	if (run.exit_code != 0) return "";
	std::istringstream out(run.out);
	std::string line; std::getline(out, line);
	return line;
}

} // namespace

TEST_SUITE("cpp_codegen_roundtrip") {

	TEST_CASE("echo spec: emit → host C++ compiler → run passes") {
		if (!has_cxx()) { MESSAGE(tau_test_cxx() << " not available, skipping"); return; }
		auto d = build_program_desc_prop(
			echo_spec(), {"in_sig"}, {"out_sig"}, "echo_ctrl");
		REQUIRE_FALSE(d.needs_tau_link);
		std::ostringstream os;
		emit_program(d, os);
		CHECK(compile_and_run_echo(os.str()));
	}
	// ── Re-ports of the pre-rebase compiled regressions (CG-RT1/CG-RT2/CG-RT5) ──

	// CG-N2 (compiled): the paren'd disjunctive conjunct "(0|1)" must gate
	// the edge -- with a=F,b=F,c=T the edge must NOT fire.
	TEST_CASE("[CG-GUARD-02] compiled: parenthesised disjunctive conjunct must gate the edge") {
		if (!has_cxx()) { MESSAGE(tau_test_cxx() << " not available, skipping"); return; }
		hoa_automaton a;
		a.num_states = 1;
		a.initial_state = 0;
		a.aps = {"a", "b", "c", "o"};
		a.edges.resize(1);
		a.edges[0].push_back(hoa_edge{"(0|1)&2&3", 0, false});
		a.edges[0].push_back(hoa_edge{"!3", 0, false});
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {"a", "b", "c"}, {"o"}, "ParenGate");
		emit_program(d, os);
		std::string result = compile_and_run(os.str(),
			"  ParenGate p;\n"
			"  ParenGate::inputs in;\n"
			"  in.a = false; in.b = false; in.c = true;\n"
			"  auto o = p.step(in);\n"
			"  std::printf(\"%s\\n\", (o.ok && !o.o) ? \"OK\" : \"WRONGFIRE\");\n",
			"cgn2");
		CHECK(result == "OK");
	}

	// CG-N9 (compiled): a guard label "f" listed first must never fire.
	TEST_CASE("[CG-GUARD-03] compiled: guard 'f' must never fire") {
		if (!has_cxx()) { MESSAGE(tau_test_cxx() << " not available, skipping"); return; }
		hoa_automaton a;
		a.num_states = 2;
		a.initial_state = 0;
		a.aps = {"o"};
		a.edges.resize(2);
		a.edges[0].push_back(hoa_edge{"f", 0, false}); // must never fire
		a.edges[0].push_back(hoa_edge{"t", 1, false}); // must always fire
		a.edges[1].push_back(hoa_edge{"t", 1, false});
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {}, {"o"}, "DeadGuard");
		emit_program(d, os);
		std::string result = compile_and_run(os.str(),
			"  DeadGuard d;\n"
			"  DeadGuard::inputs in;\n"
			"  auto o = d.step(in);\n"
			"  std::printf(\"%s\\n\", (o.ok && d.state() == 1) "
			"? \"OK\" : \"STUCK_AT_Q0\");\n",
			"cgn9");
		CHECK(result == "OK");
	}

	// CG-RT2: multi-state compiled roundtrip -- full cycle plus the
	// no-matching-edge ok==false path.
	TEST_CASE("[CG-RT-02] compiled 3-state roundtrip: full cycle + ok=false on no match") {
		if (!has_cxx()) { MESSAGE(tau_test_cxx() << " not available, skipping"); return; }
		hoa_automaton a;
		a.num_states = 3;
		a.initial_state = 0;
		a.aps = {"i", "o"};
		a.edges.resize(3);
		a.edges[0].push_back(hoa_edge{"0&1",  1, false});  // q0 -[i&o]-> q1
		a.edges[1].push_back(hoa_edge{"!0&1", 2, false});  // q1 -[!i&o]-> q2
		a.edges[2].push_back(hoa_edge{"1",    0, false});  // q2 -[o]-> q0
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {"i"}, {"o"}, "Cycle3");
		emit_program(d, os);
		std::string result = compile_and_run(os.str(),
			"  Cycle3 c;\n"
			"  Cycle3::inputs in;\n"
			"  in.i = true;\n"
			"  auto s0 = c.step(in);  // q0 -> q1\n"
			"  in.i = false;\n"
			"  auto s1 = c.step(in);  // q1 -> q2\n"
			"  auto s2 = c.step(in);  // q2 -[o]-> q0 (guard '1' ignores input)\n"
			"  bool cycle_ok = s0.ok && s1.ok && s2.ok "
			"&& c.state() == 0;\n"
			"  in.i = false;\n"
			"  auto s3 = c.step(in);  // q0 requires i&o: fails\n"
			"  std::printf(\"%s\\n\", (cycle_ok && !s3.ok) ? \"OK\" : \"BROKEN\");\n",
			"cgrt02");
		CHECK(result == "OK");
	}

}
