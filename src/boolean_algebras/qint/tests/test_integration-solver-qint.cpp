// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "solver.h"
#endif
#include "test_integration-solver_ba_suite.h"

struct qint_solver_ba {
	static tref type() { return qint_type<node_t>(); }
	static constexpr const char* name = "qint";
	static constexpr const char* a = "{[0, 1)}:qint";
	static constexpr const char* b = "{[1, 2)}:qint";
	static constexpr const char* x = "x";
	static constexpr const char* y = "y";
};

TAU_SOLVER_BA_SUITE(qint_solver_ba);
