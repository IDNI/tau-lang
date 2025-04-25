// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "../unit/test_helpers.h"
#include "z3.h"


using namespace idni::rewriter;
using namespace idni::tau_lang;
using namespace boost::log;

namespace testing = doctest;

TEST_SUITE("configuration") {

	TEST_CASE("logging") {
		core::get()->set_filter(trivial::severity >= trivial::trace);
		add_console_log(std::cout, keywords::format =
			expressions::stream << expressions::smessage);
	}

	TEST_CASE("z3_config") {
		z3_config();
	}
}

TEST_SUITE("z3_solve simple") {

	TEST_CASE("X =_ 1") {
		const char* sample = "X =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
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

	TEST_CASE("variable") {
		const char* sample = "X =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_checked") {
		const char* sample = "[X] =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_neg") {
		const char* sample = "~X =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_add") {
		const char* sample = "X + 1 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_sub") {
		const char* sample = "X - 1 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_mul") {
		const char* sample = "X * 1 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_div") {
		const char* sample = "X / 1 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_mod") {
		const char* sample = "X % 2 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_and") {
		const char* sample = "X & 1 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_nand") {
		const char* sample = "2 !& 1 =_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_or") {
		const char* sample = "X | 1 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_nor") {
		const char* sample = "2 !| 1 =_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_xor") {
		const char* sample = "X (+) 1 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_xnor") {
		const char* sample = "X !(+) 1 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_rotate_shift") {
		const char* sample = "X << 1 =_ 2";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_rotate_right") {
		const char* sample = "X >> 1 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_min") {
		const char* sample = "min 2 1 =_ X";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("z3_max") {
		const char* sample = "max X 0 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}
}

TEST_SUITE("z3 overflow/underflow") {

	TEST_CASE("multiplication overflow") {
		const char* sample = "X =_ [4294967296 * 8589934592]"; // 2^32 + 2^33
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		//CHECK_THROWS( solve_z3(equation) );
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );

	}

	TEST_CASE("no multiplication overflow") {
		const char* sample = "X =_ 4294967296 * 8589934592"; // 2^32 + 2^33
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("substraction underflow") {
		const char* sample = "X =_ [1 - 2]";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		//CHECK_THROWS( solve_z3(equation) );
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );

	}

	TEST_CASE("no substraction underflow") {
		const char* sample = "X =_ 1 - 2";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("addition overflow") {
		const char* sample = "X =_ [18446744073709551615 + 1]"; // (2^64 - 1) + 1
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		//CHECK_THROWS( solve_z3(equation) );
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );

	}

	TEST_CASE("no addition overflow") {
		const char* sample = "X =_ 18446744073709551615 + 1"; // (2^64 - 1) + 1
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("intermediate addition overflow") {
		const char* sample = "X =_ 18446744073709551615 && Y =_ 1 && Z =_ [X + Y]"; // (2^64 - 1) + 1
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		//CHECK_THROWS( solve_z3(equation) );
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );

	}

	TEST_CASE("intermediate no addition overflow") {
		const char* sample = "X =_ 18446744073709551615 && Y =_ 1 && Z =_ X + Y"; // (2^64 - 1) + 1
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("x + 3 =_ 2 with overflow off") {
		const char* sample = "x + 3 =_ 2";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("x + 3 =_ 2 with overflow on") {
		const char* sample = "[x + 3] =_ 2";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto equation = make_statement(src);
		//CHECK_THROWS( solve_z3(equation) );
		auto solution = solve_z3(equation);
		CHECK( solution.has_value() );
	}
}

TEST_SUITE("complex solve_z3") {

}
