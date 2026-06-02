// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
//
// Mixed-BA solver tests: formulas combine constraints from two distinct
// Boolean algebras via wff_and (&&), exercising the solver's ability to find
// solutions across heterogeneous BA-typed sub-formulas.  Single-equation
// APIs (find_solution, solve_system) cannot mix BA types in one equation;
// only the top-level solve() with &&-connected typed sub-formulas is tested.

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "solver.h"
#endif
#include "test_integration-solver_helper.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("solve") {

	bool test_solve(const std::string& system, const solver_options& options) {
		return ::test_solve(system, options);
	}
	bool test_solve(const std::string& system, const tref type = tau_type<node_t>()) {
		return ::test_solve(system, type);
	}

	TEST_CASE("qint + qlt: {[0, 1)}:qint x != 0 && {(0, 1)}:qlt y != 0") {
		const char* system = "{[0, 1)}:qint x != 0 && {(0, 1)}:qlt y != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("hsb + qint: {x[0] < 0}:hsb a != 0 && {[0, 1)}:qint b != 0") {
		const char* system = "{x[0] < 0}:hsb a != 0 && {[0, 1)}:qint b != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("qlt + hsb: {(0, 1)}:qlt x != 0 && {x[0] < 0}:hsb a != 0") {
		const char* system = "{(0, 1)}:qlt x != 0 && {x[0] < 0}:hsb a != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("qint + qlt + hsb: three independent typed constraints") {
		const char* system = "{[0, 1)}:qint x != 0 && {(0, 1)}:qlt y != 0 && {x[0] < 0}:hsb a != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("unsatisfiable: qint x = 0 && qint x != 0 (mixed system)") {
		const char* system = "{[0, 1)}:qint x = 0 && {(0, 1)}:qlt y != 0 && {[0, 1)}:qint x != 0.";
		CHECK( !test_solve(system) );
	}
}
