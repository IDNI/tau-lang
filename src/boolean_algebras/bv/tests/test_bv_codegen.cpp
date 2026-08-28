// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// codegen_witness contract: bv's witness is a self-contained tref expression
// built from the real bv constant, never a bool flag (the value would be lost).

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"
#include "ltl_aba.h"
#include "tau_compile.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <sstream>
#include <string>
#include <sys/wait.h>

using namespace idni::tau_lang;

namespace {

// Parse and synthesize a formula, returning the solution.
std::optional<ltl_aba_solution<node_t>> synth(const std::string& spec) {
	tref fm = api<node_t>::get_formula(spec);
	if (!fm) return std::nullopt;
	return solve_ltl_aba<node_t>(fm);
}

bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

// Opt-in: the SDK-link test below drives a real, minutes-long cmake configure+build.
bool run_sdk_link_test() {
	const char* v = std::getenv("TAU_CODEGEN_RUN_SDK_LINK_TEST");
	return v && *v && std::string(v) != "0";
}

// Run `cmd`, returning its combined stdout+stderr and its exit code.
struct captured_run { std::string out; int exit_code = -1; };
captured_run run_capture(const std::string& cmd) {
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

TEST_SUITE("bv_codegen") {

	TEST_CASE("G(o1:bv[8] = 1): emits tref o1 with a bv factory expression, not a bool flag") {
		auto sol = synth("G(o1[t]:bv = { 1 }:bv[8])");
		REQUIRE(sol.has_value());
		auto d = build_program_desc<node_t>(*sol);
		REQUIRE(d.has_value());
		CHECK(d->needs_tau_link);
		std::ostringstream os;
		emit_program(*d, os);
		std::string s = os.str();
		CHECK(has(s, "tref o1"));
		CHECK_FALSE(has(s, "bool p0"));
		CHECK_FALSE(has(s, "bool o1"));
		CHECK(has(s, "make_bitvector_value"));
		CHECK(has(s, "ba_constants<"));
		CHECK(has(s, "ba_descriptor<"));
		CHECK(has(s, "o.o1 ="));
	}

	TEST_CASE("G(o1:bv = 1): the default width (no [8] anywhere) still emits a bv witness") {
		auto sol = synth("G(o1[t]:bv = { 1 }:bv)");
		REQUIRE(sol.has_value());
		auto d = build_program_desc<node_t>(*sol);
		REQUIRE(d.has_value());
		CHECK(d->needs_tau_link);
		std::ostringstream os;
		emit_program(*d, os);
		std::string s = os.str();
		CHECK(has(s, "tref o1"));
		CHECK(has(s, "make_bitvector_value(" + std::to_string(default_bv_size) + ", \"1\""));
	}

	TEST_CASE("G(o1:bv[8] = 1): compile_spec builds and runs, printed witness matches the interpreter") {
		if (!run_sdk_link_test()) {
			MESSAGE("TAU_CODEGEN_RUN_SDK_LINK_TEST not set; skipping the "
				"compile+build+run parity coverage (structural tests above "
				"cover the emitted text on every run)");
			return;
		}
		namespace fs = std::filesystem;
		fs::path bdir = fs::temp_directory_path() / "test_bv_codegen_sdk_link.build";
		std::error_code ec;
		fs::remove_all(bdir, ec);

		const std::string spec = "G(o1[t]:bv = { 1 }:bv[8])";
		auto res = compile_spec<node_t>(spec, "", bdir.string());
		REQUIRE_MESSAGE(res.ok(), res.error);

		auto captured = run_capture(res.exe_path);
		CHECK(captured.exit_code == 0);
		std::string out = captured.out;
		// The artifact is stream-based like the interpreter: "o1[t] := value".
		auto o1_pos = out.find("o1[");
		REQUIRE(o1_pos != std::string::npos);
		auto assign_pos = out.find(":=", o1_pos);
		REQUIRE(assign_pos != std::string::npos);
		std::string artifact_witness = out.substr(assign_pos + 2);
		auto nl_pos = artifact_witness.find('\n');
		if (nl_pos != std::string::npos) artifact_witness.resize(nl_pos);
		while (!artifact_witness.empty() && std::isspace((unsigned char)artifact_witness.back()))
			artifact_witness.pop_back();
		while (!artifact_witness.empty() && std::isspace((unsigned char)artifact_witness.front()))
			artifact_witness.erase(artifact_witness.begin());
		MESSAGE("compiled artifact printed: ", artifact_witness);

		// Parity: the interpreter, run on the same spec, prints the same value.
		auto interp = api<node_t>::get_interpreter(spec);
		REQUIRE(interp.has_value());
		// interactive=false: this spec has no input streams, so auto_continue
		// never triggers -- force the outputs to be returned regardless.
		auto values = api<node_t>::step(*interp, {}, false);
		REQUIRE(values.has_value());
		std::string interp_witness;
		for (auto& [at, v] : *values) if (at.name == "o1") interp_witness = v;
		MESSAGE("interpreter printed: ", interp_witness);

		// api<node_t>::step() returns the bare stream-serialized value ("1")
		// while the artifact's console stream may render more around it, so
		// parity is the decimal value they encode, not the surrounding syntax.
		auto extract_decimal = [](const std::string& s) -> std::string {
			auto pos = s.find_first_of("0123456789");
			if (pos == std::string::npos) return "";
			auto end = s.find_first_not_of("0123456789", pos);
			return s.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
		};
		std::string artifact_value = extract_decimal(artifact_witness);
		std::string interp_value = extract_decimal(interp_witness);
		REQUIRE_FALSE(artifact_value.empty());
		REQUIRE_FALSE(interp_value.empty());
		CHECK(artifact_value == interp_value);
		CHECK(artifact_value == "1");

		fs::remove_all(bdir, ec);
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
