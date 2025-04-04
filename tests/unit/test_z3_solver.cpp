// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "../unit/test_helpers.h"
#include "z3_solver.h"


using namespace idni::rewriter;
using namespace idni::tau_lang;
using namespace boost::log;

namespace testing = doctest;

TEST_SUITE("z3_solve") {

	TEST_CASE("sample") {
		context c;
		expr x = c.bv_const("x", 32);
		expr y = c.bv_const("y", 32);
		solver s(c);
		// In C++, the operator == has higher precedence than ^.
		s.add((x + y) == 1);
		std::cout << s << "\n";
		std::cout << s.check() << "\n";
		std::cout << s.get_model() << "\n";
		std::cout << s.get_model().eval(x) << "\n";
		std::cout << s.get_model().eval(y) << "\n";
	}

	TEST_CASE("X =_ X") {
		const char* sample = "X =_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		for (const auto& v: solution.value().second) {
			std::cout << solution.value().first.eval(v) << "\n";
		}
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !=_ X") {
		const char* sample = "X !=_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X >_ X") {
		const char* sample = "X >_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X !>_ X") {
		const char* sample = "X !>_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X >=_ X") {
		const char* sample = "X >=_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !>=_ X") {
		const char* sample = "X !>=_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X <=_ X") {
		const char* sample = "X <=_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !<=_ X") {
		const char* sample = "X !<=_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X <_ X") {
		const char* sample = "X <_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X !<_ X") {
		const char* sample = "X !<_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
	}


	/*TEST_CASE("variable") {
		const char* sample = "X + Y =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}*/
}
