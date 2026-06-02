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

template <NodeType node>
bool check_solution(tref eq, const solution<node>& sol) {
	using tau = tree<node>;
	DBG(TAU_LOG_TRACE << "check_solution/sol:\n" << dump_to_str(sol);)
	tref substitution = rewriter::replace<node>(eq, sol);
	tref check = normalizer<node>(substitution);
#ifdef DEBUG
	std::cout << "check_solution/equation: " << TAU_DUMP_TO_STR(eq) << "\n";
	std::cout << "check_solution/substitution: " << TAU_DUMP_TO_STR(substitution) << "\n";
	std::cout << "check_solution/check: " << TAU_DUMP_TO_STR(check) << "\n";
#endif
	return tau::get(check).equals_T();
}

TEST_SUITE("configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("solve") {

	bool test_solve(const std::string& system, const solver_options& options) {
		tref form = get_nso_rr<node_t>(tau::get(system)).value().main->get();
		bool solve_error = false;
		auto solution = solve<node_t>(form, options, solve_error);
		return solution ? check_solution<node_t>(form, solution.value()) : false;
	}

	bool test_solve(const std::string& system,
			const tref type = tau_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::general
		};
		return test_solve(system, options);
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
