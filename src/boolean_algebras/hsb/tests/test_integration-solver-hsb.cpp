// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "solver.h"
#endif
#include "test_integration-solver_helper.h"


TEST_SUITE("test helpers") {

	TEST_CASE("no solution means code::unsat and no other error") {
		result<bool> unsat;
		(void)unsat.with_error(code::unsat, "no solution");
		CHECK( report_fails_only_with(unsat.report(), code::unsat) );
		(void)unsat.with_error(code::internal_error, "internal error");
		CHECK( !report_fails_only_with(unsat.report(), code::unsat) );
		result<bool> internal;
		(void)internal.with_error(code::internal_error, "internal error");
		CHECK( !report_fails_only_with(internal.report(), code::unsat) );
		result<bool> ok;
		(void)ok.with_value(true);
		CHECK( !report_fails_only_with(ok.report(), code::unsat) );
		CHECK( !test_no_solution("a : hsb != 0.", hsb_type<node_t>()) );
	}
}

TEST_SUITE("find_solution") {

	TEST_CASE("one var: {x[0] < 0}:hsb a = 0.") {
		CHECK( test_find_solution("{x[0] < 0}:hsb a = 0.") );
	}

	TEST_CASE("two vars: {x[0] < 0}:hsb a | {x[1] < 0}:hsb b = 0.") {
		CHECK( test_find_solution("{x[0] < 0}:hsb a | {x[1] < 0}:hsb b = 0.") );
	}

	TEST_CASE("complement: {x[0] < 0}:hsb a | {x[0] < 0}:hsb' b = 0.") {
		CHECK( test_find_solution("{x[0] < 0}:hsb a | {x[0] < 0}:hsb' b = 0.") );
	}
}

TEST_SUITE("solve_inequality_system") {

	bool test_solve_inequality_system(const std::vector<std::string>& inequalities) {
		return ::test_solve_inequality_system(inequalities, node_t::ba::splitter_one(hsb_type<node_t>()));
	}

	TEST_CASE("{x[0] < 0}:hsb a != 0 && {x[0] < 0}:hsb' a != 0.") {
		CHECK( test_solve_inequality_system({
			"{x[0] < 0}:hsb a != 0.",
			"{x[0] < 0}:hsb' a != 0."
		}) );
	}

	TEST_CASE("{x[0] < 0}:hsb a != 0 && {x[1] < 0}:hsb b != 0.") {
		CHECK( test_solve_inequality_system({
			"{x[0] < 0}:hsb a != 0.",
			"{x[1] < 0}:hsb b != 0."
		}) );
	}

	TEST_CASE("a : hsb != 0 && (a : hsb)' != 0.") {
		CHECK( test_solve_inequality_system({
			"a : hsb != 0.",
			"(a : hsb)' != 0."
		}) );
	}
}

TEST_SUITE("solve_system") {

	bool test_solve_system(const std::string& equality,
			const std::vector<std::string>& inequalities) {
		return ::test_solve_system(equality, inequalities, node_t::ba::splitter_one(hsb_type<node_t>()));
	}

	TEST_CASE("{x[0] < 0}:hsb a = 0 && {x[0] < 0}:hsb' a != 0.") {
		CHECK( test_solve_system(
			"{x[0] < 0}:hsb a = 0.",
			{"{x[0] < 0}:hsb' a != 0."}
		) );
	}

	// Both vars share the same hsb coefficient so the disjoint x[1] inequality is not contradicted.
	TEST_CASE("{x[0] < 0}:hsb a | {x[0] < 0}:hsb b = 0 && {x[1] < 0}:hsb b != 0.") {
		CHECK( test_solve_system(
			"{x[0] < 0}:hsb a | {x[0] < 0}:hsb b = 0.",
			{"{x[1] < 0}:hsb b != 0."}
		) );
	}

	TEST_CASE("a : hsb < b : hsb") {
		CHECK( test_solve_system(
			"a : hsb & (b : hsb)' = 0.",
			{"a : hsb & (b : hsb)' | (a : hsb)' & b : hsb != 0."}
		) );
	}
}

TEST_SUITE("solve") {

	bool test_solve(const std::string& system, const solver_options& options) {
		return ::test_solve(system, options);
	}
	bool test_solve(const std::string& system, const tref type = hsb_type<node_t>()) {
		return ::test_solve(system, type);
	}
	bool test_solve_min(const std::string& system, const tref type = hsb_type<node_t>()) {
		return ::test_solve_min(system, type);
	}
	bool test_solve_max(const std::string& system, const tref type = hsb_type<node_t>()) {
		return ::test_solve_max(system, type);
	}
	bool test_no_solution(const std::string& system, const tref type = hsb_type<node_t>()) {
		return ::test_no_solution(system, type);
	}
	bool test_no_solution_min(const std::string& system, const tref type = hsb_type<node_t>()) {
		return ::test_no_solution_min(system, type);
	}
	bool test_no_solution_max(const std::string& system, const tref type = hsb_type<node_t>()) {
		return ::test_no_solution_max(system, type);
	}

	TEST_CASE("a : hsb != 0") {
		const char* system = "a : hsb != 0.";
		CHECK( test_solve(system) );
		CHECK( test_no_solution_min(system) );
		CHECK( test_solve_max(system) );
	}

	TEST_CASE("a : hsb != 0 && a : hsb != 1") {
		const char* system = "a : hsb != 0 && a : hsb != 1.";
		CHECK( test_solve(system) );
		CHECK( test_no_solution_min(system) );
		CHECK( test_no_solution_max(system) );
	}

	TEST_CASE("{x[0] < 0}:hsb a != 0 && {x[1] < 0}:hsb b != 0") {
		const char* system = "{x[0] < 0}:hsb a != 0 && {x[1] < 0}:hsb b != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("unsatisfiable: a : hsb = 0 && a : hsb != 0") {
		const char* system = "a : hsb = 0 && a : hsb != 0.";
		CHECK( test_no_solution(system) );
	}
}
