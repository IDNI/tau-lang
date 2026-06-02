// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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

TEST_SUITE("find_solution") {

	bool test_find_solution(const char* src) {
		tref equation = get_nso_rr<node_t>(tau::get(src)).value().main->get();
		equation = norm_all_equations<node_t>(equation);
		equation = apply_all_xor_def<node_t>(equation);
		auto solution = find_solution<node_t>(equation);
		return check_solution<node_t>(equation, solution.value());
	}

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
		inequality_system<node_t> system;
		for (const auto& ineq : inequalities)
			system.insert(get_nso_rr<node_t>(tau::get(ineq)).value().main->get());
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(hsb_type<node_t>()),
			.mode = solver_mode::general
		};
		auto solution = solve_inequality_system<node_t>(system, options);
		bool check = true;
		for (tref equation : system)
			check = check && check_solution<node_t>(equation, solution.value());
		return check;
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
		equation_system<node_t> system;
		if (!equality.empty())
			system.first = get_nso_rr<node_t>(
				tau::get(equality)).value().main->get();
		for (const auto& ineq : inequalities)
			system.second.insert(get_nso_rr<node_t>(
				tau::get(ineq)).value().main->get());
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(hsb_type<node_t>()),
			.mode = solver_mode::general
		};
		auto solution = solve_system<node_t>(system, options);
		auto copy = solution.value();
		bool check = system.first
			? check_solution<node_t>(system.first.value(), copy) : false;
		for (const auto& equation : system.second) {
			auto copy2 = solution.value();
			check = check && check_solution<node_t>(equation, copy2);
		}
		return check;
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
		tref form = get_nso_rr<node_t>(tau::get(system)).value().main->get();
		bool solve_error = false;
		auto solution = solve<node_t>(form, options, solve_error);
		return solution ? check_solution<node_t>(form, solution.value()) : false;
	}

	bool test_solve(const std::string& system,
			const tref type = hsb_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::general
		};
		return test_solve(system, options);
	}

	bool test_solve_min(const std::string& system,
			const tref type = hsb_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::minimum
		};
		return test_solve(system, options);
	}

	bool test_solve_max(const std::string& system,
			const tref type = hsb_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::maximum
		};
		return test_solve(system, options);
	}

	TEST_CASE("a : hsb != 0") {
		const char* system = "a : hsb != 0.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( test_solve_max(system) );
	}

	TEST_CASE("a : hsb != 0 && a : hsb != 1") {
		const char* system = "a : hsb != 0 && a : hsb != 1.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( !test_solve_max(system) );
	}

	TEST_CASE("{x[0] < 0}:hsb a != 0 && {x[1] < 0}:hsb b != 0") {
		const char* system = "{x[0] < 0}:hsb a != 0 && {x[1] < 0}:hsb b != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("unsatisfiable: a : hsb = 0 && a : hsb != 0") {
		const char* system = "a : hsb = 0 && a : hsb != 0.";
		CHECK( !test_solve(system) );
	}
}
