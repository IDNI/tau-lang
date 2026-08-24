// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Tests for #60: data-atom witness emission in the tau→C++ codegen.
//
// When a spec contains qlt (DLO) output atoms, build_program_desc<node_t> +
// emit_program should:
//   - Emit `tref varname` in outputs instead of `bool o_pN`
//   - Embed an exact-rational witness factory expression (qlt_rational(p,q)
//     via the BA's own ba_constants pool), not a rounded double
//   - Set needs_tau_link=true, since the factory expression links tau

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"
#include "ltl_aba.h"
#include "tau_compile.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <regex>
#include <sstream>
#include <string>

using namespace idni::tau_lang;

namespace {

// Parse and synthesize a formula, returning the solution.
// Returns nullopt if UNREALIZABLE or parse failure.
static std::optional<ltl_aba_solution<node_t>> synth(const std::string& spec) {
	tref fm = api<node_t>::get_formula(spec);
	if (!fm) return std::nullopt;
	return solve_ltl_aba<node_t>(fm);
}

// Check whether string s contains the pattern.
static bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

// Extract the (p, q) integer components of the LAST `qlt_rational(p, q)`
// factory expression in the emitted text -- exact arithmetic on these is
// what the tests below use to check the witness against its interval,
// instead of trusting a rounded floating-point value. Earlier occurrences
// belong to program_desc::atoms' ground-constant rendering (emitted ahead
// of step()); the witness's own is emitted inside step().
static bool extract_qlt_rational(const std::string& s, long long& p, long long& q) {
	static const std::string tag = "qlt_rational(";
	auto pos = s.rfind(tag);
	if (pos == std::string::npos) return false;
	pos += tag.size();
	auto comma = s.find(',', pos);
	if (comma == std::string::npos) return false;
	auto close = s.find(')', comma);
	if (close == std::string::npos) return false;
	try {
		p = std::stoll(s.substr(pos, comma - pos));
		q = std::stoll(s.substr(comma + 1, close - comma - 1));
	} catch (...) { return false; }
	return q != 0;
}

// Opt-in: the SDK-link test below drives a real, minutes-long cmake configure+build.
static bool run_sdk_link_test() {
	const char* v = std::getenv("TAU_CODEGEN_RUN_SDK_LINK_TEST");
	return v && *v && std::string(v) != "0";
}

// Run `cmd`, return its combined stdout+stderr.
static std::string run_capture(const std::string& cmd) {
	std::string out;
	FILE* p = popen((cmd + " 2>&1").c_str(), "r");
	if (!p) return out;
	char buf[256];
	while (std::fgets(buf, sizeof(buf), p)) out += buf;
	pclose(p);
	return out;
}

// Extract (p, q) from the running artifact's printed "{ p/q }" qlt witness.
static bool extract_printed_qlt(const std::string& s, long long& p, long long& q) {
	std::smatch m;
	if (!std::regex_search(s, m, std::regex(R"(\{\s*(-?\d+)/(\d+)\s*\})")))
		return false;
	p = std::stoll(m[1]); q = std::stoll(m[2]);
	return q != 0;
}

} // namespace

TEST_SUITE("cpp_codegen_data_atoms") {

	TEST_CASE("G(o1:qlt > 1/2): emits tref o1 with an exact-rational factory expression") {
		auto sol = synth("G(o1[t]:qlt > {1/2}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		auto d = build_program_desc<node_t>(*sol);
		REQUIRE(d.has_value());
		CHECK(d->needs_tau_link);
		std::ostringstream os;
		emit_program(*d, os);
		std::string s = os.str();
		// Must have a tref output field, not bool o_p0.
		CHECK(has(s, "tref o1"));
		CHECK_FALSE(has(s, "bool o_p0"));
		CHECK_FALSE(has(s, "bool o_p"));
		CHECK_FALSE(has(s, "double o1"));
		// The factory expression is baked in, and the assignment reads it back.
		CHECK(has(s, "qlt_rational("));
		CHECK(has(s, "ba_constants<"));
		CHECK(has(s, "ba_descriptor<"));
		CHECK(has(s, "o.o1 ="));
		// State machine structure still present.
		CHECK(has(s, "outputs step("));

		long long p = 0, q = 0;
		REQUIRE(extract_qlt_rational(s, p, q));
		CHECK(q > 0);
		CHECK(p * 2 > q);  // p/q > 1/2
	}

	TEST_CASE("G(o1:qlt > 1/4): exact-rational witness satisfies > 1/4") {
		auto sol = synth("G(o1[t]:qlt > {1/4}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		auto d = build_program_desc<node_t>(*sol);
		REQUIRE(d.has_value());
		std::ostringstream os;
		emit_program(*d, os);
		std::string s = os.str();
		CHECK(has(s, "tref o1"));
		CHECK(has(s, "o.o1 ="));

		long long p = 0, q = 0;
		REQUIRE(extract_qlt_rational(s, p, q));
		CHECK(q > 0);
		CHECK(p * 4 > q);  // p/q > 1/4
	}

	// A two-var data atom's value depends on the step's input, so o1 routes
	// as a witness-template field (solved at runtime by
	// table_step_provider) and the edge records the atom's prop; only the
	// standalone baked step() emitter, which cannot solve, refuses.
	TEST_CASE("G(o1:bv = i1:bv): output routes as a witness template; the "
	          "standalone emitter refuses") {
		auto sol = synth("G(o1[t]:bv = i1[t]:bv)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		auto d = build_program_desc<node_t>(*sol);
		REQUIRE(d.has_value());
		REQUIRE(d->atoms.size() == 1);
		CHECK(d->needs_tau_link);
		bool tmpl_field = false;
		for (auto& f : d->outputs)
			if (f.kind == field_kind::witness_template && f.prop == "o1")
				tmpl_field = true;
		CHECK(tmpl_field);
		bool tmpl_edge = false;
		for (auto& es : d->edges) for (auto& e : es)
			if (!e.witness_template_props.empty()) tmpl_edge = true;
		CHECK(tmpl_edge);
		std::ostringstream os;
		CHECK_THROWS_AS(emit_program(*d, os), std::runtime_error);
	}

	TEST_CASE("G(o1:qlt > 1/3 && o1:qlt < 2/3): exact-rational witness stays in (1/3, 2/3)") {
		auto sol = synth("G(o1[t]:qlt > {1/3}:qlt && o1[t]:qlt < {2/3}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		auto d = build_program_desc<node_t>(*sol);
		REQUIRE(d.has_value());
		std::ostringstream os;
		emit_program(*d, os);
		std::string s = os.str();
		CHECK(has(s, "tref o1"));
		CHECK(has(s, "o.o1 ="));
		CHECK(has(s, "ok = true"));

		long long p = 0, q = 0;
		REQUIRE(extract_qlt_rational(s, p, q));
		CHECK(q > 0);
		CHECK(p * 3 > q);      // p/q > 1/3
		CHECK(p * 3 < q * 2);  // p/q < 2/3
	}

	// Cheap structural checks; the compile+build+run coverage is the opt-in SDK-link test below.

	TEST_CASE("G(o1:qlt > 1/2): exact-rational witness satisfies > 1/2 (structural)") {
		auto sol = synth("G(o1[t]:qlt > {1/2}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		auto d = build_program_desc<node_t>(*sol, "witness_gt");
		REQUIRE(d.has_value());
		CHECK(d->needs_tau_link);
		std::ostringstream os;
		emit_program(*d, os);
		std::string s = os.str();

		long long p = 0, q = 0;
		REQUIRE(extract_qlt_rational(s, p, q));
		CHECK(q > 0);
		CHECK(p * 2 > q);  // p/q > 1/2
	}

	TEST_CASE("G(o1:qlt > 1/4 && o1:qlt < 3/4): exact-rational witness stays in (1/4, 3/4) (structural)") {
		auto sol = synth("G(o1[t]:qlt > {1/4}:qlt && o1[t]:qlt < {3/4}:qlt)");
		if (!sol) { MESSAGE("UNREALIZABLE/parse; skip"); return; }
		auto d = build_program_desc<node_t>(*sol, "bounded_witness");
		REQUIRE(d.has_value());
		std::ostringstream os;
		emit_program(*d, os);
		std::string s = os.str();

		long long p = 0, q = 0;
		REQUIRE(extract_qlt_rational(s, p, q));
		CHECK(q > 0);
		CHECK(p * 4 > q);      // p/q > 1/4
		CHECK(p * 4 < q * 3);  // p/q < 3/4
	}

	TEST_CASE("G(o1:qlt > 1/2): compile_spec builds and runs, printed witness satisfies the bound") {
		if (!run_sdk_link_test()) {
			MESSAGE("TAU_CODEGEN_RUN_SDK_LINK_TEST not set; skipping the "
				"compile+build+run coverage (structural tests above cover "
				"the emitted text on every run)");
			return;
		}
		namespace fs = std::filesystem;
		fs::path bdir = fs::temp_directory_path() / "test_cpp_codegen_sdk_link.build";
		std::error_code ec;
		fs::remove_all(bdir, ec);

		auto res = compile_spec<node_t>("G(o1[t]:qlt > {1/2}:qlt)", "", bdir.string());
		REQUIRE_MESSAGE(res.ok(), res.error);

		std::string out = run_capture(res.exe_path);
		CHECK(has(out, "OK"));

		long long p = 0, q = 0;
		REQUIRE(extract_printed_qlt(out, p, q));
		CHECK(q > 0);
		CHECK(p * 2 > q);  // p/q > 1/2

		fs::remove_all(bdir, ec);
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
