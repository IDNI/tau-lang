// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Tests that tau→C++ code generation accepts specs produced by pointwise
// revision before synthesis.  The generated code is still a normal strategy
// header, but the source formula is the PWR output rather than a hand-written
// user spec.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"
#include "ltl_aba.h"
#include "pointwise_revision.h"

#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <optional>
#include <sstream>
#include <string>

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

static tref parse_spec(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

static tref revise(const char* spec_src, const char* update_src) {
	tref spec = parse_spec(spec_src);
	tref update = parse_spec(update_src);
	if (!spec || !update) return nullptr;
	return pointwise_revision_temporal<node_t>(spec, update, 0);
}

static std::optional<std::string> emit_revised_cpp(
	const char* spec_src,
	const char* update_src,
	const std::string& class_name = "PwrCtrl")
{
	tref revised = revise(spec_src, update_src);
	if (!revised || !is_tau_formula_sat<node_t>(revised)) return std::nullopt;
	auto sol = solve_ltl_aba<node_t>(revised);
	if (!sol) return std::nullopt;
	std::ostringstream out;
	emit_cpp_program<node_t>(*sol, out, class_name);
	return out.str();
}

// Emit a PWR-capable (table-driven) class from a spec string.
static std::optional<std::string> emit_pwr_class(
	const char* spec_src,
	const std::string& class_name = "PwrTbl")
{
	tref fm = parse_spec(spec_src);
	if (!fm) return std::nullopt;
	auto sol = solve_ltl_aba<node_t>(fm);
	if (!sol) return std::nullopt;
	std::ostringstream out;
	emit_cpp_program_pwr(sol->aut, sol->input_props, sol->output_props, out, class_name);
	return out.str();
}

// Emit a Strategy initializer from a spec string.
static std::optional<std::string> emit_strat_init(const char* spec_src) {
	tref fm = parse_spec(spec_src);
	if (!fm) return std::nullopt;
	auto sol = solve_ltl_aba<node_t>(fm);
	if (!sol) return std::nullopt;
	std::ostringstream out;
	emit_strategy_initializer(sol->aut, sol->input_props, sol->output_props, out);
	return out.str();
}

static bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

static bool has_gpp() {
	return ::system("g++ --version >/dev/null 2>&1") == 0;
}

static bool compile_and_run_ok_step(const std::string& header_src) {
	const std::string hdr = cg_tmp("_tau_codegen_pwr_test.h");
	const std::string main_f = cg_tmp("_tau_codegen_pwr_main.cpp");
	const std::string exe = cg_tmp("_tau_codegen_pwr_exe");
	{
		std::ofstream f(hdr);
		f << header_src;
	}
	{
		std::ofstream f(main_f);
		f <<
			"#include \"_tau_codegen_pwr_test.h\"\n"
			"#include <cstdio>\n"
			"int main() {\n"
			"  PwrRoundtrip c;\n"
			"  PwrRoundtrip::Inputs in;\n"
			"  for (int i = 0; i < 4; ++i) {\n"
			"    auto o = c.step(in);\n"
			"    if (!o.ok) { std::printf(\"FAIL\\n\"); return 1; }\n"
			"  }\n"
			"  std::printf(\"OK\\n\");\n"
			"  return 0;\n"
			"}\n";
	}
	std::string cmd = std::string("g++ -O2 -std=c++17 -I" + cg_tmp_dir() + " -o ")
		+ exe + " " + main_f + " >" + cg_tmp("_tau_codegen_pwr_build") + " 2>&1";
	if (::system(cmd.c_str()) != 0) return false;
	std::string run_cmd = std::string(exe) + " > " + cg_tmp("_tau_codegen_pwr_out") + " 2>&1";
	if (::system(run_cmd.c_str()) != 0) return false;
	std::ifstream out(cg_tmp("_tau_codegen_pwr_out"));
	std::string line;
	std::getline(out, line);
	return line == "OK";
}

} // namespace

TEST_SUITE("cpp_codegen_pwr") {

	TEST_CASE("safety PWR result can be synthesized and emitted") {
		auto generated = emit_revised_cpp(
			"G(o1[t] = 0).",
			"G(o1[t] = 1).");
		REQUIRE(generated.has_value());
		CHECK(has(*generated, "class PwrCtrl {"));
		CHECK(has(*generated, "struct Outputs"));
		CHECK(has(*generated, "Outputs step(const Inputs&"));
		CHECK(has(*generated, "ok = true"));
	}

	TEST_CASE("temporal PWR result can be synthesized and emitted") {
		auto generated = emit_revised_cpp(
			"sometimes(o1[t] = 1).",
			"G(o1[t] = 0).",
			"PwrTemporalCtrl");
		REQUIRE(generated.has_value());
		CHECK(has(*generated, "class PwrTemporalCtrl {"));
		CHECK(has(*generated, "enum class State"));
		CHECK(has(*generated, "Outputs step(const Inputs&"));
	}

	TEST_CASE("PWR-revised generated header compiles and steps") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		auto generated = emit_revised_cpp(
			"G(o1[t] = 0).",
			"G(o1[t] = 1).",
			"PwrRoundtrip");
		REQUIRE(generated.has_value());
		CHECK(compile_and_run_ok_step(*generated));
	}
}

// ── PWR-capable (table-driven) emitter tests ──────────────────────────────────

TEST_SUITE("cpp_codegen_pwr_table") {

	TEST_CASE("PWR emitter produces table-driven class") {
		auto generated = emit_pwr_class("G(o1[t] = 0).", "PwrSafety");
		REQUIRE(generated.has_value());
		CHECK(has(*generated, "class PwrSafety {"));
		CHECK(has(*generated, "struct Edge"));
		CHECK(has(*generated, "struct Strategy"));
		CHECK(has(*generated, "bool revise("));
		CHECK(has(*generated, "revision_count"));
		CHECK(has(*generated, "load_initial_strategy"));
	}

	TEST_CASE("PWR emitter includes step() and state()") {
		auto generated = emit_pwr_class("G(o1[t] = 0).", "PwrStep");
		REQUIRE(generated.has_value());
		CHECK(has(*generated, "Outputs step(const Inputs&"));
		CHECK(has(*generated, "int state() const"));
		CHECK(has(*generated, "const Strategy& strategy() const"));
	}

	TEST_CASE("strategy initializer produces valid C++ fragment") {
		auto init = emit_strat_init("G(o1[t] = 0).");
		REQUIRE(init.has_value());
		CHECK(has(*init, "num_states"));
		CHECK(has(*init, "initial_state"));
		CHECK(has(*init, "edges"));
		// CG-N5: the initializer embeds the program's AP order so the
		// generated revise() can verify it.
		CHECK(has(*init, "aps"));
	}

	TEST_CASE("PWR emitter handles input+output spec") {
		auto generated = emit_pwr_class(
			"G(i1[t] = 0 -> o1[t] = 0).", "PwrIO");
		REQUIRE(generated.has_value());
		CHECK(has(*generated, "struct Inputs"));
		CHECK(has(*generated, "struct Outputs"));
		CHECK(has(*generated, "i_"));
		CHECK(has(*generated, "o_"));
	}

	TEST_CASE("PWR emitter with different spec produces valid class") {
		// Synthesize two different specs and verify both emit valid PWR classes.
		auto gen1 = emit_pwr_class("G(o1[t] = 0).", "PwrSpec1");
		auto gen2 = emit_pwr_class("G(o1[t] = 1).", "PwrSpec2");
		REQUIRE(gen1.has_value());
		REQUIRE(gen2.has_value());
		CHECK(has(*gen1, "class PwrSpec1 {"));
		CHECK(has(*gen1, "bool revise("));
		CHECK(has(*gen2, "class PwrSpec2 {"));
		CHECK(has(*gen2, "bool revise("));
	}

	TEST_CASE("PWR table-driven class compiles and steps") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		auto generated = emit_pwr_class("G(o1[t] = 0).", "PwrTblRun");
		REQUIRE(generated.has_value());
		const std::string hdr = cg_tmp("_tau_pwr_tbl_test.h");
		const std::string main_f = cg_tmp("_tau_pwr_tbl_main.cpp");
		const std::string exe = cg_tmp("_tau_pwr_tbl_exe");
		{
			std::ofstream f(hdr);
			f << *generated;
		}
		{
			std::ofstream f(main_f);
			f <<
				"#include \"_tau_pwr_tbl_test.h\"\n"
				"#include <cstdio>\n"
				"int main() {\n"
				"  PwrTblRun c;\n"
				"  PwrTblRun::Inputs in;\n"
				"  for (int i = 0; i < 4; ++i) {\n"
				"    auto o = c.step(in);\n"
				"    if (!o.ok) { std::printf(\"FAIL\\n\"); return 1; }\n"
				"  }\n"
				"  std::printf(\"OK\\n\");\n"
				"  return 0;\n"
				"}\n";
		}
		std::string cmd = std::string("g++ -O2 -std=c++17 -I" + cg_tmp_dir() + " -o ")
			+ exe + " " + main_f + " >" + cg_tmp("_tau_pwr_tbl_build") + " 2>&1";
		CHECK(::system(cmd.c_str()) == 0);
		std::string run_cmd = std::string(exe)
			+ " > " + cg_tmp("_tau_pwr_tbl_out") + " 2>&1";
		CHECK(::system(run_cmd.c_str()) == 0);
		std::ifstream out(cg_tmp("_tau_pwr_tbl_out"));
		std::string line;
		std::getline(out, line);
		CHECK(line == "OK");
	}

	TEST_CASE("PWR revise() compiles and resets state") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		// Generate two strategies from different specs, then revise().
		auto gen1 = emit_pwr_class("G(o1[t] = 0).", "PwrRevT");
		REQUIRE(gen1.has_value());

		// Build a second strategy initializer for a different spec.
		tref fm2 = parse_spec("G(o1[t] = 1).");
		REQUIRE(fm2);
		auto sol2 = solve_ltl_aba<node_t>(fm2);
		REQUIRE(sol2.has_value());
		std::ostringstream strat2_ss;
		emit_strategy_initializer(sol2->aut, sol2->input_props,
			sol2->output_props, strat2_ss);
		auto strat2 = strat2_ss.str();

		const std::string hdr = cg_tmp("_tau_pwr_rev_test.h");
		const std::string main_f = cg_tmp("_tau_pwr_rev_main.cpp");
		const std::string exe = cg_tmp("_tau_pwr_rev_exe");
		{
			std::ofstream f(hdr);
			f << *gen1;
		}
		{
			std::ofstream f(main_f);
			f << "#include \"_tau_pwr_rev_test.h\"\n"
			     "#include <cstdio>\n"
			     "int main() {\n"
			     "  PwrRevT c;\n"
			     "  PwrRevT::Inputs in;\n"
			     "  auto o1 = c.step(in);\n"
			     "  if (!o1.ok) { std::printf(\"FAIL step1\\n\"); return 1; }\n"
			     "  if (c.revision_count() != 0) { std::printf(\"FAIL rev0\\n\"); return 1; }\n"
			     "  // Now revise with new strategy\n"
			     "  PwrRevT::Strategy s2 = " << strat2 << ";\n"
			     "  if (!c.revise(std::move(s2))) { std::printf(\"FAIL revise_refused\\n\"); return 1; }\n"
			     "  if (c.revision_count() != 1) { std::printf(\"FAIL rev1\\n\"); return 1; }\n"
			     "  if (c.state() != c.strategy().initial_state) { std::printf(\"FAIL state\\n\"); return 1; }\n"
			     "  auto o2 = c.step(in);\n"
			     "  if (!o2.ok) { std::printf(\"FAIL step2\\n\"); return 1; }\n"
			     "  std::printf(\"OK\\n\");\n"
			     "  return 0;\n"
			     "}\n";
		}
		std::string cmd = std::string("g++ -O2 -std=c++17 -I" + cg_tmp_dir() + " -o ")
			+ exe + " " + main_f + " >" + cg_tmp("_tau_pwr_rev_build") + " 2>&1";
		if (::system(cmd.c_str()) != 0) {
			std::ifstream blog(cg_tmp("_tau_pwr_rev_build"));
			std::string blog_str((std::istreambuf_iterator<char>(blog)),
				std::istreambuf_iterator<char>());
			MESSAGE("compile failed: ", blog_str);
		}
		CHECK(::system(cmd.c_str()) == 0);
		std::string run_cmd = std::string(exe)
			+ " > " + cg_tmp("_tau_pwr_rev_out") + " 2>&1";
		CHECK(::system(run_cmd.c_str()) == 0);
		std::ifstream out(cg_tmp("_tau_pwr_rev_out"));
		std::string line;
		std::getline(out, line);
		CHECK(line == "OK");
	}
}



TEST_SUITE("cpp_codegen_pwr_ndebug") {

	// CG-RT4 / CG-N5 (FIXED, Batch O6): revise()'s validation used to be
	// assert()-only; under -DNDEBUG (the flag customer release builds use,
	// and which g++ -O2 alone does NOT define) those asserts compiled out,
	// so an invalid Strategy (initial_state out of range for num_states)
	// caused OOB std::vector indexing (UB) on the next step() instead of
	// being rejected. revise() is now `bool` with real runtime refusal;
	// this test compiles WITH -DNDEBUG specifically to prove the checks
	// survive the customer-build configuration: the invalid strategies
	// (bad initial_state, out-of-range edge dst, mismatched aps) are all
	// refused with the running strategy and state untouched, and a valid
	// revision afterwards still succeeds.
	TEST_CASE("[CG-PWR-NDEBUG-01] revise() with an invalid Strategy under -DNDEBUG") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		auto gen1 = emit_pwr_class("G(o1[t] = 0).", "PwrNdebug");
		REQUIRE(gen1.has_value());

		const std::string hdr = cg_tmp("_tau_pwr_ndebug_test.h");
		const std::string main_f = cg_tmp("_tau_pwr_ndebug_main.cpp");
		const std::string exe = cg_tmp("_tau_pwr_ndebug_exe");
		{ std::ofstream f(hdr); f << *gen1; }
		{
			std::ofstream f(main_f);
			f << "#include \"_tau_pwr_ndebug_test.h\"\n"
			     "#include <cstdio>\n"
			     "#include <vector>\n"
			     "int main() {\n"
			     "  PwrNdebug c;\n"
			     "  PwrNdebug::Inputs in;\n"
			     "  auto o0 = c.step(in);\n"
			     "  if (!o0.ok) { std::printf(\"FAIL step0\\n\"); return 1; }\n"
			     "  int state_before = c.state();\n"
			     "  // initial_state=99 is out of range for a 1-state Strategy.\n"
			     "  PwrNdebug::Strategy bad;\n"
			     "  bad.num_states = 1;\n"
			     "  bad.initial_state = 99;\n"
			     "  bad.edges.resize(1);\n"
			     "  if (c.revise(std::move(bad))) { std::printf(\"ACCEPTED_INVALID\\n\"); return 1; }\n"
			     "  if (c.revision_count() != 0) { std::printf(\"FAIL revcount\\n\"); return 1; }\n"
			     "  if (c.state() != state_before) { std::printf(\"FAIL state_changed %d\\n\", c.state()); return 1; }\n"
			     "  auto o1 = c.step(in);\n"
			     "  if (!o1.ok) { std::printf(\"FAIL step_after_refusal\\n\"); return 1; }\n"
			     "  // An out-of-range edge dst is refused too (the old asserts\n"
			     "  // never even checked dst).\n"
			     "  PwrNdebug::Strategy bad2;\n"
			     "  bad2.num_states = 1;\n"
			     "  bad2.initial_state = 0;\n"
			     "  bad2.edges.resize(1);\n"
			     "  bad2.edges[0].push_back({std::vector<int8_t>(PwrNdebug::program_aps().size(), 0), 7});\n"
			     "  if (c.revise(std::move(bad2))) { std::printf(\"ACCEPTED_BAD_DST\\n\"); return 1; }\n"
			     "  // A non-empty aps list differing from the program's is refused.\n"
			     "  PwrNdebug::Strategy bad3 = c.strategy();\n"
			     "  bad3.aps = {\"__not_this_programs_ap__\"};\n"
			     "  if (c.revise(std::move(bad3))) { std::printf(\"ACCEPTED_BAD_APS\\n\"); return 1; }\n"
			     "  // A valid revision afterwards still succeeds.\n"
			     "  PwrNdebug::Strategy good = c.strategy();\n"
			     "  if (!c.revise(std::move(good))) { std::printf(\"FAIL good_refused\\n\"); return 1; }\n"
			     "  if (c.revision_count() != 1) { std::printf(\"FAIL revcount2\\n\"); return 1; }\n"
			     "  auto o2 = c.step(in);\n"
			     "  if (!o2.ok) { std::printf(\"FAIL step_after_good\\n\"); return 1; }\n"
			     "  std::printf(\"OK\\n\");\n"
			     "  return 0;\n"
			     "}\n";
		}
		std::string cmd = std::string("g++ -O2 -DNDEBUG -std=c++17 -I" + cg_tmp_dir() + " -o ")
			+ exe + " " + main_f + " >" + cg_tmp("_tau_pwr_ndebug_build") + " 2>&1";
		REQUIRE(::system(cmd.c_str()) == 0);
		std::string run_cmd = std::string(exe)
			+ " > " + cg_tmp("_tau_pwr_ndebug_out") + " 2>&1";
		int rc = ::system(run_cmd.c_str());
		std::ifstream out(cg_tmp("_tau_pwr_ndebug_out"));
		std::string line;
		std::getline(out, line);
		CHECK(rc == 0);
		CHECK(line == "OK");
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}