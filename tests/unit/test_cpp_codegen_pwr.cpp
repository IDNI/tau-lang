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
#include <optional>
#include <sstream>
#include <string>

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
	const std::string& class_name = "pwr_ctrl")
{
	tref revised = revise(spec_src, update_src);
	if (!revised || !is_tau_formula_sat<node_t>(revised)) return std::nullopt;
	auto sol = solve_ltl_aba<node_t>(revised);
	if (!sol) return std::nullopt;
	// revisable=true: this desc's formula came out of pointwise revision,
	// the PWR pipeline's own positive marker (see build_program_desc's own
	// doc comment) -- outside one, an unannotated output atom defaults to
	// the reserved tau type and is ordinary data (routes to
	// witness_template, which this suite's bare emit_program cannot run;
	// see test_cpp_codegen_program_desc.cpp's self-lookback tests).
	auto d = build_program_desc<node_t>(*sol, class_name, /*revisable=*/true);
	if (!d) return std::nullopt;
	std::ostringstream out;
	emit_program(*d, out);
	return out.str();
}

// Emit a PWR-capable (revisable) class from a spec string.
static std::optional<std::string> emit_pwr_class(
	const char* spec_src,
	const std::string& class_name = "pwr_tbl")
{
	tref fm = parse_spec(spec_src);
	if (!fm) return std::nullopt;
	auto sol = solve_ltl_aba<node_t>(fm);
	if (!sol) return std::nullopt;
	auto d = build_program_desc_prop(sol->aut, sol->input_props,
		sol->output_props, class_name, /*revisable=*/true);
	std::ostringstream out;
	emit_program(d, out);
	return out.str();
}

static bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

static bool has_gpp() {
	return ::system("g++ --version >/dev/null 2>&1") == 0;
}

static bool compile_and_run_ok_step(const std::string& header_src) {
	const char* hdr = "/tmp/_tau_codegen_pwr_test.h";
	const char* main_f = "/tmp/_tau_codegen_pwr_main.cpp";
	const char* exe = "/tmp/_tau_codegen_pwr_exe";
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
			"  pwr_roundtrip c;\n"
			"  pwr_roundtrip::inputs in;\n"
			"  for (int i = 0; i < 4; ++i) {\n"
			"    auto o = c.step(in);\n"
			"    if (!o.ok) { std::printf(\"FAIL\\n\"); return 1; }\n"
			"  }\n"
			"  std::printf(\"OK\\n\");\n"
			"  return 0;\n"
			"}\n";
	}
	std::string cmd = std::string("g++ -O2 -std=c++23 -I/tmp -o ")
		+ exe + " " + main_f + " >/tmp/_tau_codegen_pwr_build 2>&1";
	if (::system(cmd.c_str()) != 0) return false;
	std::string run_cmd = std::string(exe) + " > /tmp/_tau_codegen_pwr_out 2>&1";
	if (::system(run_cmd.c_str()) != 0) return false;
	std::ifstream out("/tmp/_tau_codegen_pwr_out");
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
		CHECK(has(*generated, "class pwr_ctrl {"));
		CHECK(has(*generated, "struct outputs"));
		CHECK(has(*generated, "outputs step(const inputs&"));
		CHECK(has(*generated, "ok = true"));
	}

	TEST_CASE("temporal PWR result can be synthesized and emitted") {
		auto generated = emit_revised_cpp(
			"sometimes(o1[t] = 1).",
			"G(o1[t] = 0).",
			"pwr_temporal_ctrl");
		REQUIRE(generated.has_value());
		CHECK(has(*generated, "class pwr_temporal_ctrl {"));
		CHECK(has(*generated, "strategy_step"));
		CHECK(has(*generated, "outputs step(const inputs&"));
	}

	TEST_CASE("PWR-revised generated header compiles and steps") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		auto generated = emit_revised_cpp(
			"G(o1[t] = 0).",
			"G(o1[t] = 1).",
			"pwr_roundtrip");
		REQUIRE(generated.has_value());
		CHECK(compile_and_run_ok_step(*generated));
	}
}

// ── PWR-capable (revisable) emitter tests ──────────────────────────────────

TEST_SUITE("cpp_codegen_pwr_table") {

	TEST_CASE("PWR emitter produces revisable class") {
		auto generated = emit_pwr_class("G(o1[t] = 0).", "pwr_safety");
		REQUIRE(generated.has_value());
		CHECK(has(*generated, "class pwr_safety {"));
		CHECK(has(*generated, "struct edge {"));
		CHECK(has(*generated, "struct strategy {"));
		CHECK(has(*generated, "void revise("));
		CHECK(has(*generated, "revision_count"));
		CHECK(has(*generated, "load_initial_strategy"));
	}

	TEST_CASE("PWR emitter includes step() and state()") {
		auto generated = emit_pwr_class("G(o1[t] = 0).", "pwr_step");
		REQUIRE(generated.has_value());
		CHECK(has(*generated, "outputs step(const inputs&"));
		CHECK(has(*generated, "int state() const"));
		CHECK(has(*generated, "const tau_codegen_detail::strategy& strategy() const"));
	}

	TEST_CASE("emitted initial strategy embeds num_states/initial_state/edges") {
		auto generated = emit_pwr_class("G(o1[t] = 0).", "pwr_init");
		REQUIRE(generated.has_value());
		CHECK(has(*generated, "strat_.num_states"));
		CHECK(has(*generated, "strat_.initial_state"));
		CHECK(has(*generated, "strat_.edges"));
	}

	TEST_CASE("PWR emitter handles input+output spec") {
		tref fm = parse_spec("G(i1[t] = 0 -> o1[t] = 0).");
		REQUIRE(fm);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		auto d = build_program_desc_prop(sol->aut, sol->input_props,
			sol->output_props, "pwr_io", /*revisable=*/true);
		REQUIRE_FALSE(d.inputs.empty());
		REQUIRE_FALSE(d.outputs.empty());
		std::ostringstream os;
		emit_program(d, os);
		std::string s = os.str();
		CHECK(has(s, "struct inputs"));
		CHECK(has(s, "struct outputs"));
		CHECK(has(s, "bool " + d.inputs[0].cpp_name));
		CHECK(has(s, "bool " + d.outputs[0].cpp_name));
	}

	TEST_CASE("PWR emitter with different spec produces valid class") {
		// Synthesize two different specs and verify both emit valid PWR classes.
		auto gen1 = emit_pwr_class("G(o1[t] = 0).", "pwr_spec1");
		auto gen2 = emit_pwr_class("G(o1[t] = 1).", "pwr_spec2");
		REQUIRE(gen1.has_value());
		REQUIRE(gen2.has_value());
		CHECK(has(*gen1, "class pwr_spec1 {"));
		CHECK(has(*gen1, "void revise("));
		CHECK(has(*gen2, "class pwr_spec2 {"));
		CHECK(has(*gen2, "void revise("));
	}

	TEST_CASE("PWR revisable class compiles and steps") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		auto generated = emit_pwr_class("G(o1[t] = 0).", "pwr_tbl_run");
		REQUIRE(generated.has_value());
		const char* hdr = "/tmp/_tau_pwr_tbl_test.h";
		const char* main_f = "/tmp/_tau_pwr_tbl_main.cpp";
		const char* exe = "/tmp/_tau_pwr_tbl_exe";
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
				"  pwr_tbl_run c;\n"
				"  pwr_tbl_run::inputs in;\n"
				"  for (int i = 0; i < 4; ++i) {\n"
				"    auto o = c.step(in);\n"
				"    if (!o.ok) { std::printf(\"FAIL\\n\"); return 1; }\n"
				"  }\n"
				"  std::printf(\"OK\\n\");\n"
				"  return 0;\n"
				"}\n";
		}
		std::string cmd = std::string("g++ -O2 -std=c++23 -I/tmp -o ")
			+ exe + " " + main_f + " >/tmp/_tau_pwr_tbl_build 2>&1";
		CHECK(::system(cmd.c_str()) == 0);
		std::string run_cmd = std::string(exe)
			+ " > /tmp/_tau_pwr_tbl_out 2>&1";
		CHECK(::system(run_cmd.c_str()) == 0);
		std::ifstream out("/tmp/_tau_pwr_tbl_out");
		std::string line;
		std::getline(out, line);
		CHECK(line == "OK");
	}

	TEST_CASE("PWR revise() compiles and resets state") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		// Generate two strategies from different specs, then revise().
		auto gen1 = emit_pwr_class("G(o1[t] = 0).", "pwr_rev_t");
		REQUIRE(gen1.has_value());

		// Build a second strategy's braced-init text directly from its
		// program_desc's edges (no standalone initializer-emitter exists).
		tref fm2 = parse_spec("G(o1[t] = 1).");
		REQUIRE(fm2);
		auto sol2 = solve_ltl_aba<node_t>(fm2);
		REQUIRE(sol2.has_value());
		auto d2 = build_program_desc_prop(sol2->aut, sol2->input_props,
			sol2->output_props);

		std::ostringstream strat2;
		strat2 << "{ " << d2.num_states << ", " << d2.initial_state << ", "
		       << d2.inputs.size() << ", {\n";
		for (auto& edges : d2.edges) {
			strat2 << "{ ";
			for (auto& e : edges) {
				strat2 << "{ {";
				for (size_t k = 0; k < e.guard.size(); ++k) {
					if (k) strat2 << ",";
					strat2 << (int)e.guard[k];
				}
				strat2 << "}, " << e.dst << " }, ";
			}
			strat2 << " },\n";
		}
		strat2 << "} }";

		const char* hdr = "/tmp/_tau_pwr_rev_test.h";
		const char* main_f = "/tmp/_tau_pwr_rev_main.cpp";
		const char* exe = "/tmp/_tau_pwr_rev_exe";
		{
			std::ofstream f(hdr);
			f << *gen1;
		}
		{
			std::ofstream f(main_f);
			f << "#include \"_tau_pwr_rev_test.h\"\n"
			     "#include <cstdio>\n"
			     "#include <utility>\n"
			     "int main() {\n"
			     "  pwr_rev_t c;\n"
			     "  pwr_rev_t::inputs in;\n"
			     "  auto o1 = c.step(in);\n"
			     "  if (!o1.ok) { std::printf(\"FAIL step1\\n\"); return 1; }\n"
			     "  if (c.revision_count() != 0) { std::printf(\"FAIL rev0\\n\"); return 1; }\n"
			     "  // Now revise with new strategy\n"
			     "  tau_codegen_detail::strategy s2 = " << strat2.str() << ";\n"
			     "  c.revise(std::move(s2));\n"
			     "  if (c.revision_count() != 1) { std::printf(\"FAIL rev1\\n\"); return 1; }\n"
			     "  if (c.state() != c.strategy().initial_state) { std::printf(\"FAIL state\\n\"); return 1; }\n"
			     "  auto o2 = c.step(in);\n"
			     "  if (!o2.ok) { std::printf(\"FAIL step2\\n\"); return 1; }\n"
			     "  std::printf(\"OK\\n\");\n"
			     "  return 0;\n"
			     "}\n";
		}
		std::string cmd = std::string("g++ -O2 -std=c++23 -I/tmp -o ")
			+ exe + " " + main_f + " >/tmp/_tau_pwr_rev_build 2>&1";
		if (::system(cmd.c_str()) != 0) {
			std::ifstream blog("/tmp/_tau_pwr_rev_build");
			std::string blog_str((std::istreambuf_iterator<char>(blog)),
				std::istreambuf_iterator<char>());
			MESSAGE("compile failed: ", blog_str);
		}
		CHECK(::system(cmd.c_str()) == 0);
		std::string run_cmd = std::string(exe)
			+ " > /tmp/_tau_pwr_rev_out 2>&1";
		CHECK(::system(run_cmd.c_str()) == 0);
		std::ifstream out("/tmp/_tau_pwr_rev_out");
		std::string line;
		std::getline(out, line);
		CHECK(line == "OK");
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
