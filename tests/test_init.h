// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#define DOCTEST_CONFIG_IMPLEMENT

#include <cassert>
#include <cstdlib>
#include "doctest.h"
#include "defs.h"
#include "logging.h"
// Only the blasting/solver placement/cvc5-option parameters, not the
// machinery behind them: this header defines main() and is included before
// any tau header in every test TU, so it must not pull the tau tree in ahead
// of them. That is exactly why heuristics/blast_placement.h and
// boolean_algebras/cvc5/cvc5_options.h are dependency-free.
#include "heuristics/blast_placement.h"
#include "boolean_algebras/cvc5/cvc5_options.h"

using namespace std;

namespace idni {}
namespace idni::tau_lang {}

using namespace idni;
using namespace idni::tau_lang;

// Experiment overrides for the blasting/solver placement parameters; every
// variable left unset keeps the shipped default, so an unset environment is
// exactly today's behaviour. Applied from main() rather than from a file-scope
// object with a constructor: main() lives here, it is the one-time setup this
// header already owns, and it runs before doctest registers or executes
// anything -- no inline-variable-per-TU question arises at all.
//
// Out-of-range values clamp to the default, matching the api setters
// (api::set_blast_placement and friends).
//
// See private/sdd-eliminability/task-8-brief.md; Task 9 drives the matrix.
inline void apply_tau_experiment_env() {
	auto env_int = [](const char* name, int lo, int hi, int fallback) {
		const char* v = std::getenv(name);
		if (!v) return fallback;
		int i = std::atoi(v);
		return (i >= lo && i <= hi) ? i : fallback;
	};
	if (const char* v = std::getenv("TAU_BV_BLASTING"))
		bv_blasting = std::atoi(v) != 0;
	blast_placement = static_cast<blast_site>(
		env_int("TAU_BLAST_PLACEMENT", 0, 2,
			static_cast<int>(blast_placement)));
	blast_method = static_cast<blast_mode>(
		env_int("TAU_BLAST_METHOD", 0, 1,
			static_cast<int>(blast_method)));
	solver_placement = static_cast<solver_site>(
		env_int("TAU_SOLVER_PLACEMENT", 0, 2,
			static_cast<int>(solver_placement)));
	cvc5_options = static_cast<cvc5_option_set>(
		env_int("TAU_CVC5_OPTIONS", 0,
			static_cast<int>(cvc5_option_set::combined_best),
			static_cast<int>(cvc5_options)));
}

int main(int argc, char** argv) {
	apply_tau_experiment_env();
	DBG(std::cout << "Logging severity level: " << logging::level() << "\n";)
#ifdef TAU_LOG_TRACE_TESTS
	logging::trace();
	std::cout << "Logging severity level set: " << logging::level() << "\n";
#endif // TAU_LOG_TRACE_TESTS

	return doctest::Context(argc, argv).run();
}
