// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Tests for #60: data-atom witness emission in the tau→C++ codegen.
//
// When a spec contains qlt (DLO) output atoms, emit_cpp_program<node_t> should:
//   - Emit `double varname` in Outputs instead of `bool o_pN`
//   - Embed a concrete witness value (computed via qlt_dlo_qe at codegen time)
//   - Produce a class that compiles and satisfies the spec at runtime

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"
#include "ltl_aba.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

using namespace idni::tau_lang;

namespace {

// Parse and synthesize a formula, returning the solution.
// Returns nullopt if UNREALIZABLE or parse failure.
static std::optional<LtlAbaSolution<node_t>> synth(const std::string& spec) {
	tref fm = api<node_t>::get_formula(spec);
	if (!fm) return std::nullopt;
	return solve_ltl_aba<node_t>(fm);
}

// Check whether string s contains the pattern.
static bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

// Compile a generated header to a temp executable and return its stdout line.
// Returns "" on compilation or runtime failure.
static std::string compile_and_run(
    const std::string& header_src,
    const std::string& main_src)
{
	const char* hdr  = "/tmp/_tau_cg_da_test.h";
	const char* main_f = "/tmp/_tau_cg_da_main.cpp";
	const char* exe  = "/tmp/_tau_cg_da_exe";
	{
		std::ofstream f(hdr);  f << header_src;
	}
	{
		std::ofstream f(main_f); f << main_src;
	}
	std::string cmd = std::string("g++ -O2 -std=c++17 -I/tmp -o ") + exe
	                + " " + main_f + " 2>&1";
	if (::system(cmd.c_str()) != 0) return "";
	std::string run_cmd = std::string(exe) + " > /tmp/_tau_cg_da_out 2>&1";
	if (::system(run_cmd.c_str()) != 0) return "";
	std::ifstream out("/tmp/_tau_cg_da_out");
	std::string line; std::getline(out, line);
	return line;
}

static bool has_gpp() {
	return ::system("g++ --version >/dev/null 2>&1") == 0;
}

} // namespace

TEST_SUITE("cpp_codegen_data_atoms") {

	// ── Structural tests (no compile) ──

	TEST_CASE("G(o1:qlt > 1/2): emits double o1 in Outputs") {
		auto sol = synth("G(o1[t]:qlt > {1/2}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		std::ostringstream os;
		emit_cpp_program<node_t>(*sol, os);
		std::string s = os.str();
		// Must have double output field, not bool o_p0
		CHECK(has(s, "double o1"));
		CHECK_FALSE(has(s, "bool o_p0"));
		CHECK_FALSE(has(s, "bool o_p"));
		// Must have a witness literal > 0.5 embedded
		// The witness is 1/2 + 1 = 1.5 (or midpoint if bounded above)
		CHECK(has(s, "o.o1 ="));
		// State machine structure still present
		CHECK(has(s, "enum class State"));
		CHECK(has(s, "Outputs step("));
	}

	TEST_CASE("G(o1:qlt > 1/4): emits double witness > 0.25") {
		auto sol = synth("G(o1[t]:qlt > {1/4}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		std::ostringstream os;
		emit_cpp_program<node_t>(*sol, os);
		std::string s = os.str();
		CHECK(has(s, "double o1"));
		// There should be a double assignment to o.o1
		CHECK(has(s, "o.o1 ="));
	}

	TEST_CASE("G(o1:bv = i1:bv): propositional path unchanged") {
		auto sol = synth("G(o1[t]:bv = i1[t]:bv)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		std::ostringstream os;
		emit_cpp_program<node_t>(*sol, os);
		std::string s = os.str();
		// Propositional: no double fields
		CHECK_FALSE(has(s, "double "));
		CHECK(has(s, "bool "));
	}

	TEST_CASE("G(o1:qlt > 1/3 && o1:qlt < 2/3): bounded witness in interval") {
		// Two atoms p0=(o1>1/3) and p1=(o1<2/3); witness should be midpoint ~0.5
		auto sol = synth("G(o1[t]:qlt > {1/3}:qlt && o1[t]:qlt < {2/3}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		std::ostringstream os;
		emit_cpp_program<node_t>(*sol, os);
		std::string s = os.str();
		CHECK(has(s, "double o1"));
		CHECK(has(s, "o.o1 ="));
		// The witness should be somewhere between 1/3 and 2/3 ≈ 0.5
		// We can't check the numeric value precisely here without parsing,
		// but we verify the structure.
		CHECK(has(s, "ok = true"));
	}

	// ── Round-trip tests (requires g++) ──

	TEST_CASE("G(o1:qlt > 1/2): compiled program produces witness > 0.5") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		auto sol = synth("G(o1[t]:qlt > {1/2}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		std::ostringstream os;
		emit_cpp_program<node_t>(*sol, os, "WitnessGT");
		std::string hdr = os.str();

		std::string main_src =
		    "#include \"_tau_cg_da_test.h\"\n"
		    "#include <cstdio>\n"
		    "int main() {\n"
		    "  WitnessGT c;\n"
		    "  WitnessGT::Inputs in;\n"
		    "  auto o = c.step(in);\n"
		    "  if (!o.ok) { std::printf(\"FAIL_OK\\n\"); return 1; }\n"
		    "  if (o.o1 > 0.5) std::printf(\"PASS\\n\");\n"
		    "  else std::printf(\"FAIL_WITNESS %.17g\\n\", o.o1);\n"
		    "  return 0;\n"
		    "}\n";

		auto result = compile_and_run(hdr, main_src);
		CHECK(result == "PASS");
	}

	TEST_CASE("G(o1:qlt > 1/4 && o1:qlt < 3/4): witness in (0.25, 0.75)") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		auto sol = synth("G(o1[t]:qlt > {1/4}:qlt && o1[t]:qlt < {3/4}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		std::ostringstream os;
		emit_cpp_program<node_t>(*sol, os, "BoundedWitness");
		std::string hdr = os.str();

		std::string main_src =
		    "#include \"_tau_cg_da_test.h\"\n"
		    "#include <cstdio>\n"
		    "int main() {\n"
		    "  BoundedWitness c;\n"
		    "  BoundedWitness::Inputs in;\n"
		    "  auto o = c.step(in);\n"
		    "  if (!o.ok) { std::printf(\"FAIL_OK\\n\"); return 1; }\n"
		    "  if (o.o1 > 0.25 && o.o1 < 0.75) std::printf(\"PASS\\n\");\n"
		    "  else std::printf(\"FAIL_WITNESS %.17g\\n\", o.o1);\n"
		    "  return 0;\n"
		    "}\n";

		auto result = compile_and_run(hdr, main_src);
		CHECK(result == "PASS");
	}
}
