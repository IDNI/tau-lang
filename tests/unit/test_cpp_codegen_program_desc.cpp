// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Tests for the data-driven emit path: program_desc construction,
// emit_program's output, and the codegen::strategy runtime semantics.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"
#include "codegen_strategy.h"
#include "ltl_aba.h"
#include "tau_compile.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/wait.h>

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

std::optional<ltl_aba_solution<node_t>> synth(const std::string& spec) {
	tref fm = api<node_t>::get_formula(spec);
	if (!fm) return std::nullopt;
	return solve_ltl_aba<node_t>(fm);
}

// Parse a raw wff string.
tref wff(const char* s) {
	tau::get_options opts;
	opts.parse.start = tau::wff;
	return tau::get(s, opts);
}

bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

bool has_gpp() {
	return ::system("g++ --version >/dev/null 2>&1") == 0;
}

// Write `header_src` + `main_src` to /tmp, compile with g++, run, return the
// first stdout line ("" on any compile/run failure).
std::string compile_and_run(
    const std::string& header_src,
    const std::string& main_src,
    const std::string& tag)
{
	std::string hdr  = "/tmp/_tau_cg_pd_" + tag + ".h";
	std::string mainf = "/tmp/_tau_cg_pd_" + tag + "_main.cpp";
	std::string exe  = "/tmp/_tau_cg_pd_" + tag + "_exe";
	{ std::ofstream f(hdr); f << header_src; }
	{ std::ofstream f(mainf); f << main_src; }
	std::string cmd = "g++ -O2 -std=c++23 -I/tmp -o " + exe + " " + mainf + " 2>&1";
	if (::system(cmd.c_str()) != 0) return "";
	std::string run_cmd = exe + " > " + exe + ".out 2>&1";
	if (::system(run_cmd.c_str()) != 0) return "";
	std::ifstream out(exe + ".out");
	std::string line;
	std::getline(out, line);
	return line;
}

// Read a tests/codegen_specs/<name> fixture, trying both the ctest working
// directory unit tests run under (tests/) and the repo root (a manually
// invoked binary's likely CWD). Returns "" when neither is found.
std::string read_codegen_spec(const std::string& name) {
	for (const char* prefix : {"codegen_specs/", "tests/codegen_specs/"}) {
		std::ifstream f(std::string(prefix) + name);
		if (f) {
			std::ostringstream ss;
			ss << f.rdbuf();
			return ss.str();
		}
	}
	return "";
}

// Parse via the same spec grammar path compile_spec<Node> uses
// (tau_compile.tmpl.h step 1): get_spec -> get_nso_rr -> nso_rr_apply ->
// normalizer, falling back to the bare-formula grammar.
tref parse_like_compile_spec(const std::string& src) {
	if (tref spec_tree = api<node_t>::get_spec(src); spec_tree)
		if (auto nso_rr = get_nso_rr<node_t>(spec_tree); nso_rr)
			if (tref applied = nso_rr_apply<node_t>(*nso_rr); applied)
				if (tref fm = normalizer<node_t>(applied); fm)
					return fm;
	return api<node_t>::get_formula(src);
}

// Opt-in: mirrors TAU_CODEGEN_RUN_SDK_LINK_TEST in test_cpp_codegen_data_atoms.cpp
// and test_bv_codegen.cpp -- the compile+build+run coverage below drives a real,
// minutes-long cmake configure+build.
bool run_sdk_link_test() {
	const char* v = std::getenv("TAU_CODEGEN_RUN_SDK_LINK_TEST");
	return v && *v && std::string(v) != "0";
}

// Run `cmd`, returning its combined stdout+stderr and its exit code.
struct captured_run { std::string out; int exit_code = -1; };
captured_run run_capture_ec(const std::string& cmd) {
	captured_run r;
	FILE* p = popen((cmd + " 2>&1").c_str(), "r");
	if (!p) return r;
	char buf[256];
	while (std::fgets(buf, sizeof(buf), p)) r.out += buf;
	int status = pclose(p);
	r.exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
	return r;
}

} // namespace

TEST_SUITE("cpp_codegen_program_desc") {

	// ── (a) build_program_desc_prop + emit_program ──────────────────────────

	TEST_CASE("build_program_desc_prop: structural shape") {
		auto d = build_program_desc_prop(
			echo_spec(), {"in_sig"}, {"out_sig"}, "echo_dd");
		CHECK(d.class_name == "echo_dd");
		CHECK(d.num_states == 1);
		CHECK(d.initial_state == 0);
		CHECK(d.inputs.size() == 1);
		CHECK(d.outputs.size() == 1);
		CHECK(d.inputs[0].prop == "in_sig");
		CHECK(d.inputs[0].kind == field_kind::flag);
		CHECK(d.outputs[0].prop == "out_sig");
		CHECK(d.outputs[0].kind == field_kind::flag);
		CHECK_FALSE(d.revisable);
		CHECK_FALSE(d.needs_tau_link);
		REQUIRE(d.edges.size() == 1);
		CHECK(d.edges[0].size() == 2);
		// guard layout: [input][flag outputs] -- 2 entries per edge.
		for (auto& e : d.edges[0]) CHECK(e.guard.size() == 2);
	}

	TEST_CASE("emit_program: echo spec structural checks") {
		auto d = build_program_desc_prop(
			echo_spec(), {"in_sig"}, {"out_sig"}, "echo_dd2");
		std::ostringstream os;
		emit_program(d, os);
		std::string s = os.str();
		CHECK(has(s, "class echo_dd2 {"));
		CHECK(has(s, "struct inputs"));
		CHECK(has(s, "struct outputs"));
		CHECK(has(s, "bool in_sig"));
		CHECK(has(s, "bool out_sig"));
		CHECK(has(s, "outputs step(const inputs&"));
		// Flag-only path is table-driven internally.
		CHECK(has(s, "strategy_step"));
		CHECK(has(s, "struct strategy"));
		// No PWR surface requested.
		CHECK_FALSE(has(s, "void revise("));
	}

	TEST_CASE("emit_program: echo spec compiles and runs (self-contained)") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		auto d = build_program_desc_prop(
			echo_spec(), {"in_sig"}, {"out_sig"}, "echo_dd3");
		REQUIRE_FALSE(d.needs_tau_link);
		std::ostringstream os;
		emit_program(d, os);

		std::string main_src =
		    "#include \"_tau_cg_pd_echo.h\"\n"
		    "#include <cstdio>\n"
		    "int main() {\n"
		    "  echo_dd3 c;\n"
		    "  echo_dd3::inputs in;\n"
		    "  in.in_sig = true;\n"
		    "  auto o1 = c.step(in);\n"
		    "  if (!o1.ok || !o1.out_sig) { std::printf(\"FAIL1\\n\"); return 1; }\n"
		    "  in.in_sig = false;\n"
		    "  auto o2 = c.step(in);\n"
		    "  if (!o2.ok || o2.out_sig)  { std::printf(\"FAIL2\\n\"); return 2; }\n"
		    "  std::printf(\"OK\\n\");\n"
		    "  return 0;\n"
		    "}\n";
		auto result = compile_and_run(os.str(), main_src, "echo");
		CHECK(result == "OK");
	}

	// ── (b) PWR x witness refusal ─────────────────────────────────────────

	TEST_CASE("build_program_desc: revisable refuses witness outputs") {
		auto sol = synth("G(o1[t]:qlt > {1/2}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		CHECK_THROWS_WITH_AS(
			build_program_desc<node_t>(*sol, "refused", /*revisable=*/true),
			"PWR revision with data-atom outputs is not supported",
			std::runtime_error);
	}

	TEST_CASE("build_program_desc: witness output still builds when not revisable") {
		auto sol = synth("G(o1[t]:qlt > {1/2}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		auto d = build_program_desc<node_t>(*sol, "witness_ok", /*revisable=*/false);
		REQUIRE(d.has_value());
		bool found_witness = false;
		for (auto& f : d->outputs) if (f.kind == field_kind::witness) found_witness = true;
		CHECK(found_witness);
		std::ostringstream os;
		emit_program(*d, os);
		std::string s = os.str();
		CHECK(has(s, "tref o1"));
		CHECK(has(s, "o.o1 ="));
		CHECK_FALSE(has(s, "void revise("));
	}

	// ── (b') untyped io var reaching codegen is a hard error ─────────────

	TEST_CASE("build_program_desc: untyped io var is a hard emission error") {
		// A var whose ba_type is 0 (never went through type inference,
		// distinct from the real "untyped" BA type) reaching build_program_desc
		// at all is the invariant violation under test, so it is hand-built
		// here rather than reached through synth(). The entry-point scan
		// catches this before classify_output_field's own deep check ever
		// runs, so the message is that scan's, not "reached codegen".
		tref untyped_var = build_out_var<node_t>(build_var_name<node_t>("o1"), 0);

		ltl_aba_solution<node_t> sol;
		sol.atoms = {{untyped_var, "o1"}};
		sol.output_props = {"o1"};
		sol.aut.num_states = 1;
		sol.aut.initial_state = 0;
		sol.aut.aps = {"o1"};
		sol.aut.edges.resize(1);
		sol.aut.edges[0].push_back(hoa_edge{"0", 0, false});
		sol.aut.state_accepting = {false};

		CHECK_THROWS_WITH_AS(
			build_program_desc<node_t>(sol, "untyped"),
			"build_program_desc: variable 'o1' carries no BA type -- the "
			"formula did not go through type inference before reaching "
			"build_program_desc",
			std::runtime_error);
	}

	// ── (c) strategy_step unit cases ─────────────────────────────────────

	TEST_CASE("strategy_step: guard matching, -1/0/+1 semantics") {
		codegen::strategy s;
		s.num_states = 1;
		s.initial_state = 0;
		s.num_inputs = 2;
		s.edges.resize(1);
		// state 0: "ap0 true, ap1 false" -> dst 1; "ap0 false, don't-care ap1" -> dst 2
		s.edges[0].push_back(codegen::edge{{1, -1}, 1});
		s.edges[0].push_back(codegen::edge{{-1, 0}, 2});

		bool ap_match_first[2] = {true, false};
		auto* e1 = codegen::strategy_step(s, 0, ap_match_first);
		REQUIRE(e1 != nullptr);
		CHECK(e1->dst == 1);

		bool ap_match_second_a[2] = {false, true};
		auto* e2 = codegen::strategy_step(s, 0, ap_match_second_a);
		REQUIRE(e2 != nullptr);
		CHECK(e2->dst == 2);  // don't-care on ap1

		bool ap_match_second_b[2] = {false, false};
		auto* e3 = codegen::strategy_step(s, 0, ap_match_second_b);
		REQUIRE(e3 != nullptr);
		CHECK(e3->dst == 2);
	}

	TEST_CASE("strategy_step: no match returns nullptr") {
		codegen::strategy s;
		s.num_states = 1;
		s.initial_state = 0;
		s.num_inputs = 1;
		s.edges.resize(1);
		s.edges[0].push_back(codegen::edge{{1}, 0});  // only matches ap0=true
		bool ap_false[1] = {false};
		CHECK(codegen::strategy_step(s, 0, ap_false) == nullptr);
	}

	TEST_CASE("strategy_step: out-of-range state returns nullptr") {
		codegen::strategy s;
		s.num_states = 1;
		s.initial_state = 0;
		s.num_inputs = 0;
		s.edges.resize(1);
		CHECK(codegen::strategy_step(s, 5, nullptr) == nullptr);
		CHECK(codegen::strategy_step(s, -1, nullptr) == nullptr);
	}

	// ── (d) revisable prop emission exposes revise() ─────────────────────

	TEST_CASE("emit_program: revisable=true exposes revise()/state()/revision_count()") {
		auto d = build_program_desc_prop(
			echo_spec(), {"in_sig"}, {"out_sig"}, "echo_tbl", /*revisable=*/true);
		CHECK(d.revisable);
		std::ostringstream os;
		emit_program(d, os);
		std::string s = os.str();
		CHECK(has(s, "void revise("));
		CHECK(has(s, "revision_count"));
		CHECK(has(s, "int state() const"));
		CHECK(has(s, "struct strategy"));
	}

	TEST_CASE("emit_program: revisable=true compiles, steps, and revises") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		auto d1 = build_program_desc_prop(
			echo_spec(), {"in_sig"}, {"out_sig"}, "tbl_rev_a", /*revisable=*/true);
		std::ostringstream os1;
		emit_program(d1, os1);

		auto d2 = build_program_desc_prop(
			echo_spec(), {"in_sig"}, {"out_sig"}, "tbl_rev_a", /*revisable=*/true);
		// Flip the strategy: identity -> constant-false output, to prove
		// revise() actually swaps behaviour, not just resets state.
		for (auto& edges : d2.edges) for (auto& e : edges) e.guard[1] = -1;

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

		std::string main_src =
		    "#include \"_tau_cg_pd_tblrev.h\"\n"
		    "#include <cstdio>\n"
		    "#include <utility>\n"
		    "int main() {\n"
		    "  tbl_rev_a c;\n"
		    "  tbl_rev_a::inputs in; in.in_sig = true;\n"
		    "  auto o1 = c.step(in);\n"
		    "  if (!o1.ok || !o1.out_sig) { std::printf(\"FAIL1\\n\"); return 1; }\n"
		    "  if (c.revision_count() != 0) { std::printf(\"FAIL_REV0\\n\"); return 1; }\n"
		    "  tau_codegen_detail::strategy s2 = " + strat2.str() + ";\n" +
		    "  c.revise(std::move(s2));\n"
		    "  if (c.revision_count() != 1) { std::printf(\"FAIL_REV1\\n\"); return 1; }\n"
		    "  auto o2 = c.step(in);\n"
		    "  if (!o2.ok || o2.out_sig) { std::printf(\"FAIL2 %d\\n\", (int)o2.out_sig); return 1; }\n"
		    "  std::printf(\"OK\\n\");\n"
		    "  return 0;\n"
		    "}\n";

		auto result = compile_and_run(os1.str(), main_src, "tblrev");
		CHECK(result == "OK");
	}

	// ── (e) baked auto_continue bounds (lookback / highest_initial_pos) ──

	TEST_CASE("build_program_desc_prop: lookback and highest_initial_pos default to 0") {
		auto d = build_program_desc_prop(
			echo_spec(), {"in_sig"}, {"out_sig"}, "echo_zero");
		CHECK(d.lookback == 0);
		CHECK(d.highest_initial_pos == 0);
	}

	TEST_CASE("build_program_desc: positional-only spec bakes highest_initial_pos, no lookback") {
		tref fm = wff("(o[0]:bv[1] = { 1 }:bv[1]) && (o[1]:bv[1] = { 0 }:bv[1]) "
		              "&& (o[2]:bv[1] = { 1 }:bv[1])");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		auto d = build_program_desc<node_t>(*sol, "positional");
		REQUIRE(d.has_value());
		// bv[1] is the pack's resolved bool carrier, so this stays flag-only.
		CHECK_FALSE(d->needs_tau_link);
		CHECK(d->highest_initial_pos == 2);
		CHECK(d->lookback == 0);
	}

	// o1[0]=1 relativizes to the same atom as F(o1[t]=1)'s own atom;
	// apply_step_counter_encoding merges the duplicate away (see
	// ltl_aba_normalization.tmpl.h), so exactly one output field reaches
	// build_program_desc for "o1", not two dangling props.
	TEST_CASE("build_program_desc: positional atom colliding with a same-instant "
	          "natural atom over the same var yields one output, not two") {
		tref p0_atom = wff("o1[0]:bv[2] = {1}");
		tref f_atom  = wff("F (o1[t]:bv[2] = {1})");
		REQUIRE(p0_atom != nullptr);
		REQUIRE(f_atom != nullptr);
		tref fm = tau::build_wff_and(p0_atom, f_atom);
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		REQUIRE(sol->atoms.size() == 1);
		auto d = build_program_desc<node_t>(*sol, "positional_dup");
		REQUIRE(d.has_value());
		CHECK(d->outputs.size() == 1);
	}

	TEST_CASE("build_program_desc: relative-only (lookback) spec bakes lookback, no highest_initial_pos") {
		auto sol = synth("F (o1[t]:bv[8] = i1[t-2]:bv[8])");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		auto d = build_program_desc<node_t>(*sol, "lookback");
		REQUIRE(d.has_value());
		CHECK(d->lookback == 2);
		CHECK(d->highest_initial_pos == 0);
	}

	TEST_CASE("emit_program: emits lookback/highest_initial_pos as baked static constexpr constants") {
		tref fm = wff("(o[0]:bv[1] = { 1 }:bv[1]) && (o[1]:bv[1] = { 0 }:bv[1])");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		auto d = build_program_desc<node_t>(*sol, "positional_emit");
		REQUIRE(d.has_value());
		std::ostringstream os;
		emit_program(*d, os);
		std::string s = os.str();
		CHECK(has(s, "static constexpr int lookback = "
			+ std::to_string(d->lookback) + ";"));
		CHECK(has(s, "static constexpr int highest_initial_pos = "
			+ std::to_string(d->highest_initial_pos) + ";"));
	}

	TEST_CASE("emit_program: prop path emits lookback=0/highest_initial_pos=0") {
		auto d = build_program_desc_prop(
			echo_spec(), {"in_sig"}, {"out_sig"}, "echo_consts");
		std::ostringstream os;
		emit_program(d, os);
		std::string s = os.str();
		CHECK(has(s, "static constexpr int lookback = 0;"));
		CHECK(has(s, "static constexpr int highest_initial_pos = 0;"));
	}

	// A no-input positional artifact runs exactly highest_initial_pos+1
	// steps and exits cleanly.
	TEST_CASE("emitted driver loop: no-input positional spec runs highest_initial_pos+1 steps") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		tref fm = wff("(o[0]:bv[1] = { 1 }:bv[1]) && (o[1]:bv[1] = { 0 }:bv[1]) "
		              "&& (o[2]:bv[1] = { 1 }:bv[1])");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		auto d = build_program_desc<node_t>(*sol, "positional_run");
		REQUIRE(d.has_value());
		REQUIRE_FALSE(d->needs_tau_link);
		REQUIRE(d->highest_initial_pos == 2);
		std::ostringstream os;
		emit_program(*d, os);

		std::string main_src =
		    "#include \"_tau_cg_pd_posrun.h\"\n"
		    "#include <cstdio>\n"
		    "int main() {\n"
		    "  positional_run prog;\n"
		    "  int t = 0;\n"
		    "  bool auto_continue = true;\n"
		    "  bool ok = true;\n"
		    "  while (auto_continue) {\n"
		    "    positional_run::inputs in;\n"
		    "    auto out = prog.step(in);\n"
		    "    if (!out.ok) { ok = false; break; }\n"
		    "    bool has_input = false;\n"
		    "    auto_continue = has_input || (t < positional_run::highest_initial_pos);\n"
		    "    ++t;\n"
		    "  }\n"
		    "  if (!ok || t != positional_run::highest_initial_pos + 1) {\n"
		    "    std::printf(\"FAIL t=%d\\n\", t); return 1;\n"
		    "  }\n"
		    "  std::printf(\"OK steps=%d\\n\", t);\n"
		    "  return 0;\n"
		    "}\n";
		auto result = compile_and_run(os.str(), main_src, "posrun");
		CHECK(result == "OK steps=3");
	}

	// ── (f) positional chain via the automatic step-counter encoding ──────

	TEST_CASE("build_program_desc: 3-position bv witness spec chains positions+1 states, "
	          "one witness per prefix edge and none on the final self-loop") {
		tref fm = wff("(o[0]:bv[8] = {5}) && (o[1]:bv[8] = {6}) && (o[2]:bv[8] = {7})");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		CHECK(sol->aut.num_states == 4);
		auto d = build_program_desc<node_t>(*sol, "pos_witness_chain");
		REQUIRE(d.has_value());
		CHECK(d->needs_tau_link);
		REQUIRE(d->edges.size() == 4);
		for (int s = 0; s < 3; ++s) {
			REQUIRE(d->edges[s].size() == 1);
			CHECK(d->edges[s][0].witness_ctors.size() == 1);
			CHECK(d->edges[s][0].dst == s + 1);
		}
		REQUIRE(d->edges[3].size() == 1);
		CHECK(d->edges[3][0].witness_ctors.empty());
		CHECK(d->edges[3][0].dst == 3);
	}

	TEST_CASE("emitted driver loop: 3-position flag spec outputs a distinct "
	          "combination at each step through ordinary edge outputs") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		tref fm = wff("(o[0]:bv[1] = { 1 }:bv[1]) && (o[1]:bv[1] = { 0 }:bv[1]) "
		              "&& (o[2]:bv[1] = { 1 }:bv[1])");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		auto d = build_program_desc<node_t>(*sol, "pos_flag_run");
		REQUIRE(d.has_value());
		REQUIRE_FALSE(d->needs_tau_link);
		REQUIRE(d->outputs.size() == 3);
		std::ostringstream os;
		emit_program(*d, os);

		std::string main_src =
		    "#include \"_tau_cg_pd_posflag.h\"\n"
		    "#include <cstdio>\n"
		    "int main() {\n"
		    "  pos_flag_run prog;\n"
		    "  int t = 0;\n"
		    "  bool auto_continue = true;\n"
		    "  bool ok = true;\n"
		    "  while (auto_continue) {\n"
		    "    pos_flag_run::inputs in;\n"
		    "    auto out = prog.step(in);\n"
		    "    if (!out.ok) { ok = false; break; }\n"
		    "    std::printf(\"%d%d%d \", (int)out." + d->outputs[0].cpp_name
		            + ", (int)out." + d->outputs[1].cpp_name
		            + ", (int)out." + d->outputs[2].cpp_name + ");\n"
		    "    bool has_input = false;\n"
		    "    auto_continue = has_input || (t < pos_flag_run::highest_initial_pos);\n"
		    "    ++t;\n"
		    "  }\n"
		    "  if (!ok || t != pos_flag_run::highest_initial_pos + 1) {\n"
		    "    std::printf(\"FAIL t=%d\\n\", t); return 1;\n"
		    "  }\n"
		    "  std::printf(\"\\n\");\n"
		    "  return 0;\n"
		    "}\n";
		auto result = compile_and_run(os.str(), main_src, "posflag");
		CHECK(result == "100 010 001 ");
	}

	// Both positional atoms sit inside an implication -- an implication of
	// purely-positional atoms is a hoistable Boolean shape (max-position
	// hoisting, .local/positional_general_case_DESIGN.md section 4): the
	// whole conjunct's k_max is max(1,3)=3, the input reference relativizes
	// to a lookback (i[t-2]), and the two conjuncts (this branch and its
	// complement) guard the same k_max step, so the strategy branches there
	// on the input AP -- the design's own validation example (section 5).
	TEST_CASE("branching example: an implication of positional atoms hoists "
	          "and branches at k_max on the input AP") {
		tref fm = wff("((i[1]:bv[2] = {1}) -> (o[3]:bv[2] = {1})) "
		              "&& (!(i[1]:bv[2] = {1}) -> (o[3]:bv[2] = {2}))");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		MESSAGE("branching example automaton state count: ", sol->aut.num_states);
		// No X-placement, ever; the counter's own bits carry the schedule.
		CHECK(sol->skeleton.find("X(p") == std::string::npos);
		CHECK(sol->skeleton.find("o__ltl_ctr") != std::string::npos);

		auto d = build_program_desc<node_t>(*sol, "branch");
		REQUIRE(d.has_value());
		CHECK(d->needs_tau_link);
		CHECK(d->highest_initial_pos == 3);

		// Exactly one state branches (more than one outgoing edge): the
		// k_max step, decided by the input AP -- everywhere else the
		// counter's own deterministic recurrence is the only transition.
		int branch_state = -1, branch_count = 0;
		for (int s = 0; s < d->num_states; ++s)
			if (d->edges[s].size() > 1) { ++branch_count; branch_state = s; }
		REQUIRE(branch_count == 1);
		REQUIRE(d->edges[branch_state].size() == 2);

		// Both branch edges write "o", with distinct witnesses -- the
		// per-branch values {1} and {2} the spec asked for.
		std::vector<std::string> exprs;
		for (auto& e : d->edges[branch_state]) {
			REQUIRE(e.witness_ctors.size() == 1);
			CHECK(e.witness_ctors[0].first == "o");
			exprs.push_back(e.witness_ctors[0].second);
		}
		CHECK(exprs[0] != exprs[1]);
		CHECK((has(exprs[0], "\"1\"") || has(exprs[1], "\"1\"")));
		CHECK((has(exprs[0], "\"2\"") || has(exprs[1], "\"2\"")));
	}

	// Executed interpreter-match: run the SAME branching spec through a
	// real interpreter with vector input streams for both input values,
	// then walk the artifact-side program_desc's strategy deterministically
	// (codegen::strategy_step, the SAME matching logic the emitted step()
	// uses) for both values and confirm the edge reached at k_max carries a
	// witness for "o" with the SAME constant the interpreter produced.
	TEST_CASE("branching example: artifact program_desc matches the "
	          "interpreter's per-branch output") {
		bdd_init<Bool>();
		const char* spec_text =
		    "((i[1]:bv[2] = {1}:bv[2]) -> (o[3]:bv[2] = {1}:bv[2])) && "
		    "((i[1]:bv[2] != {1}:bv[2]) -> (o[3]:bv[2] = {2}:bv[2])).";

		auto interpret_branch = [&](const std::string& tape_val) -> std::string {
			io_context<node_t> ctx;
			auto o = std::make_shared<vector_output_stream>();
			ctx.add_output("o", bv_type_id<node_t>(2), o);
			ctx.add_input("i", bv_type_id<node_t>(2),
			              std::make_shared<vector_input_stream>(strings{tape_val}));
			auto nso = get_nso_rr<node_t>(ctx, tau::get(spec_text));
			REQUIRE(nso.has_value());
			tref fm = nso.value().main->get();
			auto ran = run<node_t>(fm, ctx, 4);
			REQUIRE(ran.has_value());
			auto vals = o->get_values();
			REQUIRE(vals.size() >= 4);
			return vals[3];
		};

		std::string interp_1 = interpret_branch("1");  // i[1] == 1
		std::string interp_2 = interpret_branch("2");  // i[1] != 1
		MESSAGE("interpreter o[3] when i[1]=1: ", interp_1);
		MESSAGE("interpreter o[3] when i[1]!=1: ", interp_2);
		CHECK(interp_1 != interp_2);

		tref fm = wff("((i[1]:bv[2] = {1}) -> (o[3]:bv[2] = {1})) "
		              "&& (!(i[1]:bv[2] = {1}) -> (o[3]:bv[2] = {2}))");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		auto d = build_program_desc<node_t>(*sol, "branch_match");
		REQUIRE(d.has_value());
		REQUIRE(d->inputs.size() == 1);

		codegen::strategy strat;
		strat.num_states = d->num_states;
		strat.initial_state = d->initial_state;
		strat.num_inputs = (int)d->inputs.size();
		strat.edges.resize(d->num_states);
		for (int s = 0; s < d->num_states; ++s)
			for (auto& e : d->edges[s])
				strat.edges[s].push_back({e.guard, e.dst});

		// The counter's own recurrence is input-independent everywhere
		// except the branch state, so walking with a fixed input value the
		// whole way reaches k_max deterministically either way.
		auto witness_at_k3 = [&](bool i_is_one) -> const edge_desc* {
			bool ap[1] = { i_is_one };
			int state = strat.initial_state;
			const edge_desc* last = nullptr;
			for (int t = 0; t <= 3; ++t) {
				auto* e = codegen::strategy_step(strat, state, ap);
				REQUIRE(e != nullptr);
				for (auto& de : d->edges[state])
					if (de.dst == e->dst && de.guard == e->guard) { last = &de; break; }
				state = e->dst;
			}
			return last;
		};

		const edge_desc* edge_1 = witness_at_k3(true);
		const edge_desc* edge_2 = witness_at_k3(false);
		REQUIRE(edge_1 != nullptr);
		REQUIRE(edge_2 != nullptr);
		REQUIRE(edge_1->witness_ctors.size() == 1);
		REQUIRE(edge_2->witness_ctors.size() == 1);
		CHECK(edge_1->witness_ctors[0].first == "o");
		CHECK(edge_2->witness_ctors[0].first == "o");
		CHECK(has(edge_1->witness_ctors[0].second, "\"" + interp_1 + "\""));
		CHECK(has(edge_2->witness_ctors[0].second, "\"" + interp_2 + "\""));
	}

	// Multi-position atom o1[1] = o2[3]: the max-position hoisting rewrite
	// must give each io_var its own offset (k_max - j), not flatten every
	// io_var to [t] -- the defect the general rewrite fixes (design section
	// 4.5). k_max = max(1,3) = 3; o1 (position 1) becomes a lookback two
	// steps behind the guard step, o2 (position 3) stays current-time.
	TEST_CASE("multi-position atom o1[1]=o2[3] hoists with per-variable "
	          "offsets, not a flattened [t]") {
		tref fm = wff("o1[1]:bv[2] = o2[3]:bv[2]");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		REQUIRE(sol->atoms.size() == 1);
		std::string atom_str = tau::get(sol->atoms[0].first).to_str();
		MESSAGE("relativized atom: ", atom_str);
		CHECK(has(atom_str, "o1[t-2]"));
		CHECK(has(atom_str, "o2[t]"));
		CHECK_FALSE(has(atom_str, "o1[t]")); // not flattened to current-time
		CHECK(sol->skeleton.find("o__ltl_ctr") != std::string::npos);

		// Interplay: the hoist-introduced offset (o1[t-2]) is a real
		// lookback shift now, so compute_auto_continue_bounds/get_max_shift
		// must bake it into d->lookback exactly like any other relative
		// atom -- counter_highest_initial_pos still comes from the
		// pre-rewrite positions (k_max=3), not this shift.
		auto d = build_program_desc<node_t>(*sol, "multi_pos");
		REQUIRE(d.has_value());
		CHECK(d->lookback == 2);
		CHECK(d->highest_initial_pos == 3);
	}

	// ── (g) hello_world.tau: the 12-position bv canary ────────────────────
	//
	// The compile+build+run coverage below (compile_spec) stays opt-in
	// (TAU_CODEGEN_RUN_SDK_LINK_TEST) -- it drives a real, minutes-long cmake
	// configure+build regardless of solve_ltl_aba's own cost. This structural
	// test only calls solve_ltl_aba/build_program_desc directly, which the
	// ground-equality consistency fast path keeps well within the default
	// ctest budget.

	TEST_CASE("hello_world.tau: positional bv chain, one witness per prefix "
	          "edge in order, silent final self-loop, baked bounds") {
		std::string src = read_codegen_spec("hello_world.tau");
		REQUIRE_FALSE(src.empty());
		tref fm = parse_like_compile_spec(src);
		REQUIRE(fm != nullptr);

		auto t0 = std::chrono::steady_clock::now();
		auto sol = solve_ltl_aba<node_t>(fm);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - t0).count();
		MESSAGE("hello_world solve_ltl_aba: ", ms, " ms");
		REQUIRE(sol.has_value());

		// No X-placement, ever; the counter's own bits carry the schedule.
		CHECK(sol->skeleton.find("X(p") == std::string::npos);
		CHECK(sol->skeleton.find("o__ltl_ctr0__") != std::string::npos);

		auto d = build_program_desc<node_t>(*sol, "hello_world");
		REQUIRE(d.has_value());
		CHECK(d->needs_tau_link);
		CHECK(d->highest_initial_pos == 11);
		CHECK(d->lookback == 0);
		MESSAGE("hello_world automaton state count: ", d->num_states);
		// 12 positions -> 13 states (positions+2), matching the N+1-states
		// pattern verified at N=3 in commit 0d6bc1f5: one prefix-transition
		// edge per position plus the trailing silent self-loop state.
		CHECK(d->num_states == 13);
		REQUIRE(d->edges.size() == (size_t)d->num_states);

		static const char* const constants[] = {
			"72", "101", "108", "108", "111", "32",
			"87", "111", "114", "108", "100", "33"
		};
		for (int s = 0; s < 12; ++s) {
			REQUIRE(d->edges[s].size() == 1);
			REQUIRE(d->edges[s][0].witness_ctors.size() == 1);
			CHECK(d->edges[s][0].witness_ctors[0].first == "o");
			CHECK(d->edges[s][0].dst == s + 1);
			CHECK(has(d->edges[s][0].witness_ctors[0].second,
				std::string("\"") + constants[s] + "\""));
		}
		int final_state = d->num_states - 1;
		REQUIRE(d->edges[final_state].size() == 1);
		CHECK(d->edges[final_state][0].witness_ctors.empty());
		CHECK(d->edges[final_state][0].dst == final_state);
	}

	TEST_CASE("hello_world.tau: compile_spec builds and runs, twelve steps "
	          "print the constants in order, exit code 0") {
		if (!run_sdk_link_test()) {
			MESSAGE("TAU_CODEGEN_RUN_SDK_LINK_TEST not set; skipping the "
				"compile+build+run coverage (a real, minutes-long cmake "
				"configure+build)");
			return;
		}
		namespace fs = std::filesystem;
		fs::path bdir = fs::temp_directory_path()
			/ "test_cpp_codegen_hello_world_sdk_link.build";
		std::error_code ec;
		fs::remove_all(bdir, ec);

		std::string src = read_codegen_spec("hello_world.tau");
		REQUIRE_FALSE(src.empty());

		auto t0 = std::chrono::steady_clock::now();
		auto res = compile_spec<node_t>(src, "", bdir.string());
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - t0).count();
		MESSAGE("hello_world compile_spec (synth+build): ", ms, " ms");
		REQUIRE_MESSAGE(res.ok(), res.error);

		auto run = run_capture_ec(res.exe_path);
		MESSAGE("hello_world artifact output: ", run.out);
		CHECK(run.exit_code == 0);

		// Exactly twelve witness values were printed (one per step).
		int step_count = 0;
		for (size_t pos = 0; (pos = run.out.find("o=", pos)) != std::string::npos; pos += 2)
			++step_count;
		CHECK(step_count == 12);

		// The twelve constants appear, in order (loose substring/ordering
		// check -- the driver's exact rendering is not contract).
		static const char* const constants[] = {
			"72", "101", "108", "108", "111", "32",
			"87", "111", "114", "108", "100", "33"
		};
		size_t pos = 0;
		for (auto* c : constants) {
			auto found = run.out.find(c, pos);
			REQUIRE_MESSAGE(found != std::string::npos,
				"constant '" << c << "' not found in order");
			pos = found + std::string(c).size();
		}

		fs::remove_all(bdir, ec);
	}

	// End-to-end: echo has no bakeable witness (its value is each step's own
	// input), so its edge carries a witness_template solved at runtime by
	// table_step_provider; the artifact replays the baked ba-type table,
	// reads i1 from stdin and echoes it to o1 until input ends.
	TEST_CASE("echo.tau: compile_spec emits the artifact, "
	          "piped inputs come back in order, exit code 0") {
		if (!has_gpp()) { MESSAGE("g++ not available, skipping"); return; }
		namespace fs = std::filesystem;
		fs::path bdir = fs::temp_directory_path() / "_tau_cg_pd_echo";
		std::error_code ec;
		fs::remove_all(bdir, ec);

		std::string src = read_codegen_spec("echo_dot.tau");
		REQUIRE_FALSE(src.empty());

		auto t0 = std::chrono::steady_clock::now();
		auto res = compile_spec<node_t>(src, "", bdir.string());
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - t0).count();
		MESSAGE("echo compile_spec (synth+build): ", ms, " ms");
		REQUIRE_MESSAGE(res.ok(), res.error);

		auto run = run_capture_ec(
			"printf '2\\n7\\n4\\n' | " + res.exe_path);
		MESSAGE("echo artifact output: ", run.out);
		CHECK(run.exit_code == 0);
		// Each piped input value comes back on the o1 stream, in order
		// (loose substring check -- piped stdin is not echoed by a tty,
		// so the values appear in the capture only as written outputs).
		CHECK(run.out.find("o1") != std::string::npos);
		size_t epos = 0;
		for (const std::string v : {"2", "7", "4"}) {
			auto found = run.out.find(v, epos);
			REQUIRE_MESSAGE(found != std::string::npos,
				"echoed value '" << v << "' not found in order");
			epos = found + 1;
		}

		fs::remove_all(bdir, ec);
	}

	// ── (h) program_desc::atoms (atom_desc) emission ──────────────────────

	// A single-variable equality atom over a real (non-carrier) BA: ground_expr
	// reconstructs it via build_bf_eq over a plain variable reference and the
	// literal's own codegen_constant_expr rendering (qlt_rational(1, 2), not a
	// re-parsed "1/2" string).
	TEST_CASE("build_program_desc: atom_desc captures a ground-equality atom "
	          "over a real BA type") {
		auto sol = synth("G(o1[t]:qlt = {1/2}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		auto d = build_program_desc<node_t>(*sol, "atom_ground");
		REQUIRE(d.has_value());
		REQUIRE(d->atoms.size() == 1);
		CHECK(d->atoms[0].prop == sol->atoms[0].second);
		CHECK(has(d->atoms[0].ground_expr, "build_bf_eq<"));
		CHECK(has(d->atoms[0].ground_expr, "build_out_var_at_t<"));
		CHECK(has(d->atoms[0].ground_expr, "qlt_rational(1, 2)"));

		std::ostringstream os;
		emit_program(*d, os);
		std::string s = os.str();
		CHECK(has(s, "struct atom_entry"));
		CHECK(has(s, "static const atom_entry* atoms() noexcept"));
		CHECK(has(s, "atoms_count() noexcept"));
	}

	// A purely-propositional atom (bool-carrier typed) needs no reconstruction
	// -- its value is already fully carried by the edge guard bit.
	TEST_CASE("build_program_desc: atom_desc excludes bool-carrier atoms") {
		auto d = build_program_desc_prop(
			echo_spec(), {"in_sig"}, {"out_sig"}, "echo_atoms");
		CHECK(d.atoms.empty());
	}

	// Two-io_var atom, no ground constant at all: both operands rebuild via
	// the plain-variable branch, one with a baked lookback shift, the other
	// at current time -- and both output variables become witness-template
	// fields, their values solved at runtime rather than baked per edge.
	TEST_CASE("build_program_desc: atom_desc rebuilds a two-variable atom "
	          "with per-variable shifts as a witness template") {
		tref fm = wff("o1[1]:bv[2] = o2[3]:bv[2]");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		REQUIRE(sol->atoms.size() == 1);
		auto d = build_program_desc<node_t>(*sol, "atom_shift");
		REQUIRE(d.has_value());
		REQUIRE(d->atoms.size() == 1);
		const std::string& e = d->atoms[0].ground_expr;
		CHECK(has(e, "build_bf_eq<"));
		CHECK(has(e, "build_out_var_at_t_minus<"));
		CHECK(has(e, "\"o1\""));
		CHECK(has(e, "build_out_var_at_t<"));
		CHECK(has(e, "\"o2\""));
		size_t tmpl_fields = 0;
		for (auto& f : d->outputs)
			if (f.kind == field_kind::witness_template) ++tmpl_fields;
		CHECK(tmpl_fields == 2);
	}

	// The tau BA's own complement operator (`x'`) wrapping a plain variable is
	// neither a ground constant nor a bare variable reference itself, so
	// build_atom_term_expr must recurse into its operand and re-wrap with the
	// runtime build_bf_neg counterpart, rather than refusing the shape.
	TEST_CASE("build_program_desc: atom_desc rebuilds a tau-BA complement "
	          "operand via build_bf_neg") {
		tref fm = wff("o1[t]:tau' = o2[t]:tau");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		REQUIRE(sol->atoms.size() == 1);
		auto d = build_program_desc<node_t>(*sol, "atom_complement");
		REQUIRE(d.has_value());
		REQUIRE(d->atoms.size() == 1);
		const std::string& e = d->atoms[0].ground_expr;
		CHECK(has(e, "build_bf_neg<"));
		CHECK(has(e, "build_out_var_at_t<"));
		CHECK(has(e, "\"o1\""));
		CHECK(has(e, "\"o2\""));
	}

	// apply_step_counter_encoding relativizes every positional atom to a
	// current-time equivalent before it reaches sol.atoms (the same rewrite
	// counter_highest_initial_pos's own doc comment describes), so a bare
	// position-0 atom rebuilds via the no-shift build_out_var_at_t branch,
	// not build_out_var_at_n -- the constant side is the codegen_constant_expr
	// under test here (build_out_var_at_n is exercised structurally by the
	// build_atom_term_expr code path itself, not reachable through sol.atoms).
	TEST_CASE("build_program_desc: atom_desc rebuilds a relativized "
	          "positional atom's constant side") {
		tref fm = wff("o[0]:bv[8] = {5}:bv[8]");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		REQUIRE(sol->atoms.size() == 1);
		auto d = build_program_desc<node_t>(*sol, "atom_pos");
		REQUIRE(d.has_value());
		REQUIRE(d->atoms.size() == 1);
		const std::string& e = d->atoms[0].ground_expr;
		CHECK(has(e, "build_out_var_at_t<"));
		CHECK(has(e, "\"o\""));
		CHECK(has(e, "make_bitvector_value(8, \"5\", 10)"));
	}

	// Owner-without-capability refusal: qint's output classifies as a
	// witness template (no codegen_witness to bake with), but its atom
	// carries a ground constant qint cannot spell into the atoms table
	// (no codegen_constant_expr) -- a hard build-time error, never a
	// silently-dropped atom or a re-parsed fallback. qint is not the pack's
	// bool carrier, so this goes through build_atom_term_expr's non-carrier
	// refusal, naming qint, rather than the carrier's is_one/is_zero marker.
	TEST_CASE("build_program_desc: a data atom over a BA without "
	          "codegen_constant_expr is a hard emission error") {
		tref fm = wff("o1[t]:qint = {[0, 1)}:qint");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		CHECK_THROWS_WITH_AS(
			build_program_desc<node_t>(*sol, "qint_refused"),
			"atom constant's owning BA type ':qint' declined "
			"codegen_constant_expr; atom emission does not support this shape",
			std::runtime_error);
	}

	// Built with get_raw rather than the parser/solve_ltl_aba: "x:tau =
	// {T.}:tau" written as spec text never reaches build_program_desc as a
	// genuine ba_constant at all -- the ordinary hooked tree constructor
	// folds any BA constant that is_one()/is_zero() straight to a bf_t/bf_f
	// marker on construction (of any BA, not just the carrier's), the same
	// simplification that turns a real {1/2}:qlt or {5}:bv[8] constant into
	// nothing at all when it happens to equal 1 or 0 for its own algebra.
	// get_raw bypasses that fold, so the tree handed to build_program_desc
	// here is exactly the shape the interpreter keeps for real: a standing
	// ba_constant, not yet collapsed to a marker.
	// tau is not the pack's bool carrier (bv[1] is) and declares no
	// codegen_constant_expr, so this ground constant is refused outright --
	// it must never fall back to the bf_t/bf_f marker build_bf_t_type builds
	// for the carrier, since that marker is not a real tau constant and
	// downstream consumers (e.g. the splitter) require one. The atom is
	// registered as an input, so it is collected unconditionally regardless
	// of its flag/witness classification.
	TEST_CASE("build_program_desc: a tau-typed ba_constant declining "
	          "codegen_constant_expr is refused, not emitted as a marker") {
		size_t tau_tid = ba_types<node_t>::id(tau_type<node_t>());
		tref io = build_in_var_at_t<node_t>(
			build_var_name<node_t>("i1"), tau_tid, "t");
		tref leaf = tau::get_ba_constant(node_t::ba::pack_tau_ba(io), tau_tid);
		REQUIRE(tau::get(leaf).is_ba_constant());
		tref cst = tau::get_raw(node_t(tau::bf), &leaf, 1);
		REQUIRE(tree<node_t>::get(tree<node_t>::trim(cst)).is_ba_constant());
		tref atom_ref = build_bf_eq<node_t>(io, cst);

		ltl_aba_solution<node_t> sol;
		sol.atoms = {{atom_ref, "p0"}};
		sol.input_props = {"p0"};
		sol.aut.num_states = 1;
		sol.aut.initial_state = 0;
		sol.aut.aps = {"p0"};
		sol.aut.edges.resize(1);
		sol.aut.edges[0].push_back(hoa_edge{"0", 0, false});
		sol.aut.state_accepting = {false};

		CHECK_THROWS_WITH_AS(
			build_program_desc<node_t>(sol, "tau_const_refused"),
			"atom constant's owning BA type ':tau' declined "
			"codegen_constant_expr; atom emission does not support this shape",
			std::runtime_error);
	}

	// Same hand-built shape, but over the pack's actual bool carrier
	// (bv[1]): the non-carrier gate above must not catch this one too.
	// bv answers codegen_constant_expr for every value it owns, including
	// its bool-carrier width, so this reaches that success path rather than
	// the is_one/is_zero marker fallback -- a pack whose bool carrier is a
	// BA without codegen_constant_expr (e.g. sbf, in a pack built without
	// bv) is what actually drives that fallback; not reproducible here
	// without reconfiguring the pack.
	TEST_CASE("build_program_desc: a bool-carrier ba_constant still emits, "
	          "not refused by the non-carrier gate") {
		size_t carrier_tid = ba_types<node_t>::id(pack_bool_carrier_type<node_t>());
		tref io = build_in_var_at_t<node_t>(
			build_var_name<node_t>("i1"), carrier_tid, "t");
		size_t width = get_bv_size<node_t>(get_ba_type_tree<node_t>(carrier_tid));
		tref leaf = tau::get_ba_constant(
			make_bitvector_value(width, size_t(1)), carrier_tid);
		REQUIRE(tau::get(leaf).is_ba_constant());
		tref cst = tau::get_raw(node_t(tau::bf), &leaf, 1);
		REQUIRE(tree<node_t>::get(tree<node_t>::trim(cst)).is_ba_constant());
		tref atom_ref = build_bf_eq<node_t>(io, cst);

		ltl_aba_solution<node_t> sol;
		sol.atoms = {{atom_ref, "p0"}};
		sol.input_props = {"p0"};
		sol.aut.num_states = 1;
		sol.aut.initial_state = 0;
		sol.aut.aps = {"p0"};
		sol.aut.edges.resize(1);
		sol.aut.edges[0].push_back(hoa_edge{"0", 0, false});
		sol.aut.state_accepting = {false};

		auto d = build_program_desc<node_t>(sol, "carrier_const_ok");
		REQUIRE(d.has_value());
		REQUIRE(d->atoms.size() == 1);
		CHECK(has(d->atoms[0].ground_expr, "make_bitvector_value(1, \"1\", 10)"));
	}

	// A bare literal ("o1[t] = 1", not "o1[t] = {1}") never parses as a
	// ba_constant -- the grammar reserves that shape for the braced form --
	// so it reaches codegen as a bf_t/bf_f node. It only arises typed once
	// type inference has actually assigned it a BA (parse_like_compile_spec's
	// defaults path, mirroring compile_spec's own get_spec parse); an
	// untyped bf_t/bf_f (api::get_formula's no-defaults path) never reaches
	// here because validate_atom_io_types already rejects the atom's
	// still-untyped io variable first.
	TEST_CASE("build_program_desc: atom_desc emits a bare literal operand "
	          "as its owner's typed bf_t/bf_f constant") {
		tref fm = wff("G(i1[t]:bv[8] = 1 -> o1[t] = 1)");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		REQUIRE(!sol->atoms.empty());
		auto d = build_program_desc<node_t>(*sol, "bare_literal");
		REQUIRE(d.has_value());
		auto it = std::find_if(d->atoms.begin(), d->atoms.end(),
			[](auto& a) { return has(a.ground_expr, "\"i1\""); });
		REQUIRE(it != d->atoms.end());
		const std::string& e = it->ground_expr;
		CHECK((has(e, "build_bf_eq<") || has(e, "build_bf_neq<")));
		CHECK(has(e, "build_in_var_at_t<"));
		CHECK((has(e, "build_bf_t_type<") || has(e, "build_bf_f_type<")));
	}

	// build_program_desc's own precondition, not classify_output_field's
	// deep one: a hand-built solution whose atom carries a variable that
	// never went through type inference (api::get_formula's no-defaults,
	// no-inference-at-all path -- verified below to actually leave the
	// variable at ba_type 0) is refused immediately, before any per-atom
	// classification runs, with a message naming the variable and pointing
	// at type inference rather than "reached codegen".
	TEST_CASE("build_program_desc: an untyped variable is refused at the "
	          "entry-point check, not classify_output_field's deep one") {
		tref untyped = api<node_t>::get_formula("o1[t] = 1", false);
		REQUIRE(untyped != nullptr);
		auto fv = get_free_vars<node_t>(untyped);
		REQUIRE(!fv.empty());
		// Sanity: simplified=false really does skip inference entirely, so
		// the annotation-less "o1" is left untyped -- otherwise this test
		// would not be exercising the entry check at all.
		REQUIRE(tree<node_t>::get(fv[0]).get_ba_type() == 0);

		ltl_aba_solution<node_t> sol;
		sol.atoms.push_back({untyped, "o1"});
		CHECK_THROWS_WITH_AS(
			build_program_desc<node_t>(sol, "untyped_refused"),
			"build_program_desc: variable 'o1' carries no BA type -- the "
			"formula did not go through type inference before reaching "
			"build_program_desc",
			std::runtime_error);
	}

	// A self-lookback atom (o2[t] vs o2[t-1]) is two free-var nodes naming
	// the same variable. Outside a PWR (revisable) build, tau is an
	// ordinary data type -- it owns no codegen_witness, so classify_atom_field
	// lands this on witness_template (atom_single_var_name's single-name
	// merge still applies, keying the one template var on "o2" rather than
	// refusing it as multi-variable) -- and the standalone emitter refuses a
	// witness_template output outright, naming the table step provider as the
	// supported path.
	TEST_CASE("build_program_desc: self-lookback tau-typed atom is "
	          "witness_template outside a PWR build, refused by the "
	          "standalone emitter") {
		tref fm = wff("!(o2[t]:tau = o2[t-1]:tau)");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		REQUIRE(sol->atoms.size() == 1);
		REQUIRE(get_free_vars<node_t>(sol->atoms[0].first).size() == 2);

		auto d = build_program_desc<node_t>(*sol, "self_lookback_template");
		REQUIRE(d.has_value());
		REQUIRE(d->outputs.size() == 1);
		CHECK(d->outputs[0].kind == field_kind::witness_template);
		CHECK(d->outputs[0].prop == "o2");
		CHECK(d->flag_output_vars.empty());

		std::ostringstream os;
		CHECK_THROWS_WITH_AS(emit_program(*d, os),
			"output 'o2' needs runtime witness solving, which the "
			"standalone emitted step() does not support; drive the "
			"program through the interpreter's table step provider",
			std::runtime_error);
	}

	// Same self-lookback shape, but inside a PWR (revisable) build: the
	// tau-typed update stream's guard bit is itself the value there, so it
	// stays flag -- build_program_desc's flag-stream extraction must key
	// this flag slot on the shared name "o2" instead of refusing it as
	// multi-variable, and the resulting program_desc must emit_program
	// cleanly, matching compile_spec's own refusal check
	// (d->flag_output_vars[k].empty()) never tripping for it.
	TEST_CASE("build_program_desc: self-lookback tau-typed atom keys its "
	          "flag slot on the shared variable name in a PWR build, not "
	          "refused") {
		tref fm = wff("!(o2[t]:tau = o2[t-1]:tau)");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());
		REQUIRE(sol->atoms.size() == 1);
		REQUIRE(get_free_vars<node_t>(sol->atoms[0].first).size() == 2);

		auto d = build_program_desc<node_t>(*sol, "self_lookback_flag",
			/*revisable=*/true);
		REQUIRE(d.has_value());
		REQUIRE(d->outputs.size() == d->flag_output_vars.size());

		bool found = false;
		for (size_t k = 0; k < d->outputs.size(); ++k) {
			if (d->outputs[k].prop != sol->atoms[0].second) continue;
			found = true;
			CHECK(d->outputs[k].kind == field_kind::flag);
			CHECK(d->flag_output_vars[k] == "o2");
		}
		REQUIRE(found);
		// Mirrors compile_spec's own hard-refusal gate: no flag slot may
		// carry an empty stream key.
		for (auto& v : d->flag_output_vars) CHECK_FALSE(v.empty());

		std::ostringstream os;
		emit_program(*d, os);
		CHECK(has(os.str(), "bool " + d->outputs[0].cpp_name));
	}

	// The ppLTLTT past-operator DFA testers (__past_s*/__past_t*,
	// ltl_aba_helpers.tmpl.h's skeleton_wff_with_testers) are automaton
	// bookkeeping registered straight into sol.output_props with no backing
	// entry in sol.atoms at all -- unlike an ordinary atom, there is no io
	// variable to key a flag slot on. build_program_desc must exclude them
	// from the emitted Outputs surface the same way it already excludes the
	// step-counter's o__ltl_ctr bits, instead of defaulting them to an
	// unkeyable flag field (compile_spec's own hard-refusal gate would then
	// reject the whole spec -- this is the ltl_past_since_trigger.tau parity
	// mismatch).
	TEST_CASE("build_program_desc: ppLTLTT past-operator DFA tester props "
	          "are excluded from Outputs, not emitted as unkeyable flags") {
		tref fm = wff(
			"(o1[t]:bv[1] = {1}:bv[1]) S (o2[t]:bv[1] = {1}:bv[1])");
		REQUIRE(fm != nullptr);
		auto sol = solve_ltl_aba<node_t>(fm);
		REQUIRE(sol.has_value());

		bool has_tester_prop = false;
		for (auto& p : sol->output_props)
			if (p.rfind("__past_", 0) == 0) has_tester_prop = true;
		REQUIRE(has_tester_prop);

		auto d = build_program_desc<node_t>(*sol, "past_tester_excluded");
		REQUIRE(d.has_value());
		REQUIRE(d->outputs.size() == d->flag_output_vars.size());

		for (auto& f : d->outputs) CHECK(f.prop.rfind("__past_", 0) != 0);
		// Mirrors compile_spec's own hard-refusal gate: no flag slot may
		// carry an empty stream key.
		for (auto& v : d->flag_output_vars) CHECK_FALSE(v.empty());

		std::ostringstream os;
		emit_program(*d, os);
		CHECK_FALSE(has(os.str(), "__past_"));
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
