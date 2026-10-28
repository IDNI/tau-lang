// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__TEST_CODEGEN_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_CODEGEN_HELPERS_H__

// Helpers of the code generation suites, over the full pack.
#include "test_tau_helpers.h"
#include "cpp_codegen.h"
#include "ltl_aba.h"

#include <optional>
#include <string>

namespace idni::tau_lang {

// The solution synthesized for @p spec; nullopt when it does not parse or is
// unrealizable. An undecided verdict fails the test.
inline std::optional<ltl_aba_solution<node_t>> synth(const std::string& spec) {
	auto fm = api<node_t>::get_formula(spec);
	if (!fm.has_value()) return std::nullopt;
	auto r = solve_ltl_aba<node_t>(fm.value());
	REQUIRE(r.has_value());
	return r.value();
}

// Whether @p s contains @p pat.
inline bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

// Whether a host C++ compiler is there to build an emitted program.
inline bool has_cxx() { return tau_test_cxx_available(tau_test_cxx()); }

// Opt-in: an SDK-link case drives a real, minutes-long cmake configure+build.
inline bool run_sdk_link_test() {
	auto v = test_env("TAU_CODEGEN_RUN_SDK_LINK_TEST");
	v.print_pending();
	REQUIRE(v.has_value());
	return v.value() && !v.value()->empty() && *v.value() != "0";
}

// The (p, q) integer components of the LAST `qlt_rational(p, q)` factory
// expression of @p s. Earlier occurrences belong to program_desc::atoms'
// ground-constant rendering (emitted ahead of step()); the witness's own is
// emitted inside step().
inline bool extract_qlt_rational(const std::string& s, long long& p, long long& q) {
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

// The combined stdout+stderr of a run and its exit code. spawn_capture
// decides the exit code itself, so only the zero/non-zero distinction is
// meaningful (see tau_test_run).
struct captured_run { std::string out; int exit_code = -1; };

// Runs the artifact @p exe_path with @p stdin_data on its input.
inline captured_run run_capture(const std::string& exe_path,
	const std::string& stdin_data = "")
{
	auto run = tau_test_run({ exe_path }, stdin_data);
	return { run.out + run.err, run.exit_code };
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__TEST_CODEGEN_HELPERS_H__
