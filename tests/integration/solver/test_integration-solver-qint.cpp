// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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

TEST_SUITE("find_solution") {

	TEST_CASE("one var: {[0, 1)}:qint x = 0.") {
		CHECK( test_find_solution("{[0, 1)}:qint x = 0.") );
	}

	TEST_CASE("two vars: {[0, 1)}:qint x | {[1, 2)}:qint y = 0.") {
		CHECK( test_find_solution("{[0, 1)}:qint x | {[1, 2)}:qint y = 0.") );
	}

	TEST_CASE("complement: {[0, 1)}:qint x | {[0, 1)}:qint' y = 0.") {
		CHECK( test_find_solution("{[0, 1)}:qint x | {[0, 1)}:qint' y = 0.") );
	}
}

TEST_SUITE("solve_inequality_system") {

	bool test_solve_inequality_system(const std::vector<std::string>& inequalities) {
		return ::test_solve_inequality_system(inequalities, node_t::ba::splitter_one(qint_type<node_t>()));
	}

	TEST_CASE("{[0, 1)}:qint x != 0 && {[0, 1)}:qint' x != 0.") {
		CHECK( test_solve_inequality_system({
			"{[0, 1)}:qint x != 0.",
			"{[0, 1)}:qint' x != 0."
		}) );
	}

	TEST_CASE("{[0, 1)}:qint x != 0 && {[1, 2)}:qint y != 0.") {
		CHECK( test_solve_inequality_system({
			"{[0, 1)}:qint x != 0.",
			"{[1, 2)}:qint y != 0."
		}) );
	}

	TEST_CASE("x : qint != 0 && (x : qint)' != 0.") {
		CHECK( test_solve_inequality_system({
			"x : qint != 0.",
			"(x : qint)' != 0."
		}) );
	}
}

TEST_SUITE("solve_system") {

	bool test_solve_system(const std::string& equality,
			const std::vector<std::string>& inequalities) {
		return ::test_solve_system(equality, inequalities, node_t::ba::splitter_one(qint_type<node_t>()));
	}

	TEST_CASE("{[0, 1)}:qint x = 0 && {[0, 1)}:qint' x != 0.") {
		CHECK( test_solve_system(
			"{[0, 1)}:qint x = 0.",
			{"{[0, 1)}:qint' x != 0."}
		) );
	}

	TEST_CASE("{[0, 1)}:qint x | {[0, 1)}:qint y = 0 && {[1, 2)}:qint y != 0.") {
		CHECK( test_solve_system(
			"{[0, 1)}:qint x | {[0, 1)}:qint y = 0.",
			{"{[1, 2)}:qint y != 0."}
		) );
	}

	TEST_CASE("x : qint < y : qint") {
		CHECK( test_solve_system(
			"x : qint & (y : qint)' = 0.",
			{"x : qint & (y : qint)' | (x : qint)' & y : qint != 0."}
		) );
	}
}

TEST_SUITE("solve") {

	bool test_solve(const std::string& system, const solver_options& options) {
		return ::test_solve(system, options);
	}
	bool test_solve(const std::string& system, const tref type = qint_type<node_t>()) {
		return ::test_solve(system, type);
	}
	bool test_solve_min(const std::string& system, const tref type = qint_type<node_t>()) {
		return ::test_solve_min(system, type);
	}
	bool test_solve_max(const std::string& system, const tref type = qint_type<node_t>()) {
		return ::test_solve_max(system, type);
	}

	TEST_CASE("x : qint != 0") {
		const char* system = "x : qint != 0.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( test_solve_max(system) );
	}

	TEST_CASE("x : qint != 0 && x : qint != 1") {
		const char* system = "x : qint != 0 && x : qint != 1.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( !test_solve_max(system) );
	}

	TEST_CASE("{[0, 1)}:qint x != 0 && {[1, 2)}:qint y != 0") {
		const char* system = "{[0, 1)}:qint x != 0 && {[1, 2)}:qint y != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("unsatisfiable: x : qint = 0 && x : qint != 0") {
		const char* system = "x : qint = 0 && x : qint != 0.";
		CHECK( !test_solve(system) );
	}
}
