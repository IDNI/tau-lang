// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#define DOCTEST_CONFIG_IMPLEMENT

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "doctest.h"
#include "defs.h"
#include "logging.h"
#include "benchmark_listener.h"
#include "utility/diagnostics.h"
#include "utility/options.h"
// Only the preprocessing/solver placement/cvc5-option parameters, not the
// machinery behind them: this header defines main() and is included before
// any tau header in every test TU, so it must not pull the tau tree in ahead
// of them. That is exactly why heuristics/preprocess_placement.h and
// backends/cvc5/cvc5_options.h are dependency-free.
#include "heuristics/preprocess_placement.h"
#include "backends/cvc5/cvc5_options.h"

using namespace std;

namespace idni {}
namespace idni::tau_lang {}

using namespace idni;
// These bring every tau name into the global namespace, where a few of them
// collide with the platform headers. On macOS doctest's implementation half
// (above) includes <sys/sysctl.h>, and <sys/ucred.h> declares a global
// `struct label` that makes an unqualified `label::` ambiguous; write
// `tau_lang::label::` in a test instead.
using namespace idni::tau_lang;

#if defined(_WIN32)
// Windows has no POSIX setenv/unsetenv; MinGW-w64 and MSVC both carry
// _putenv_s, and an empty value removes the variable, which is unsetenv.
inline int setenv(const char* name, const char* value, int overwrite) {
	if (!name || !value) return -1;
	if (!overwrite && std::getenv(name)) return 0;
	return _putenv_s(name, value) == 0 ? 0 : -1;
}
inline int unsetenv(const char* name) {
	if (!name) return -1;
	return _putenv_s(name, "") == 0 ? 0 : -1;
}
#endif

#if defined(_WIN32) && defined(_MSC_VER)
// _CRT_DECLARE_NONSTDC_NAMES=0 hides the CRT's popen alias, so map the
// names here for the tests that use them; MinGW-w64 declares them itself.
#define popen  _popen
#define pclose _pclose
#endif

// Experiment overrides for the preprocessing/solver placement parameters;
// every variable left unset keeps the shipped default, so an unset
// environment is exactly today's behaviour. Applied from main() rather than
// from a file-scope object with a constructor: main() lives here, it is the
// one-time setup this header already owns, and it runs before doctest
// registers or executes anything -- no inline-variable-per-TU question
// arises at all.
//
// Out-of-range values clamp to the default, matching the api setters
// (api::set_preprocess_placement and friends).
inline void apply_tau_experiment_env() {
	auto env_int = [](const char* name, int lo, int hi, int fallback) {
		const char* v = std::getenv(name);
		if (!v) return fallback;
		int i = std::atoi(v);
		return (i >= lo && i <= hi) ? i : fallback;
	};
	preprocess_placement = static_cast<preprocess_site>(
		env_int("TAU_PREPROCESS_PLACEMENT", 0, 2,
			static_cast<int>(preprocess_placement)));
	preprocess_method = static_cast<preprocess_mode>(
		env_int("TAU_PREPROCESS_METHOD", 0, 1,
			static_cast<int>(preprocess_method)));
	solver_placement = static_cast<solver_site>(
		env_int("TAU_SOLVER_PLACEMENT", 0, 2,
			static_cast<int>(solver_placement)));
	cvc5_options = static_cast<cvc5_option_set>(
		env_int("TAU_CVC5_OPTIONS", 0,
			static_cast<int>(cvc5_option_set::combined_best),
			static_cast<int>(cvc5_options)));
}

// Set by test_helpers.h once node_t is known; stays a bare pointer here
// so this header, which must not pull in the tau tree, never has to.
inline idni::diagnostics::result<void> (*test_tau_init_hook)() = nullptr;

// Set by a suite that must re-execute itself as a worker process: main()
// hands argv over before doctest runs, and the hook returns the worker's
// exit code, or -1 when it does not recognize the invocation.
inline int (*test_child_hook)(int argc, char** argv) = nullptr;

int main(int argc, char** argv) {
	apply_tau_experiment_env();
	DBG(std::cout << "Logging severity level: " << logging::level() << "\n";)
#ifdef TAU_LOG_TRACE_TESTS
	logging::trace();
	std::cout << "Logging severity level set: " << logging::level() << "\n";
#endif // TAU_LOG_TRACE_TESTS

	if (test_tau_init_hook) {
		auto init = test_tau_init_hook();
		init.print_pending();
		if (!init.has_value()) return 1;
		auto env = idni::options().load_env("TAU_");
		env.print_pending();
		if (!env.has_value()) return 1;
	}
	if (test_child_hook) {
		if (int rc = test_child_hook(argc, argv); rc >= 0) return rc;
	}
	return doctest::Context(argc, argv).run();
}
