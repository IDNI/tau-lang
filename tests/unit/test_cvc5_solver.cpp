// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"


TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("cvc5_solve simple") {

	tref parse(const std::string& sample) {
		auto opts = tau::get_options{
			.parse = { .start = tau::wff },
			.reget_with_hooks = false
		};
		auto src = tree<node_t>::get(sample, opts);
		if (src == nullptr) {
			TAU_LOG_ERROR << "Parsing failed for: " << sample;
		}
		return src;
	}


	TEST_CASE("X =_ 1") {
		const char* sample = "X =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !=_ X") {
		const char* sample = "X !=_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X + 0 >_ 0") {
		const char* sample = "X + 0 >_ 0";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !>_ X") {
		const char* sample = "X !>_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X + 1 !>_ X") {
		const char* sample = "X + 1 !>_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X >=_ X") {
		const char* sample = "X >=_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X >=_ X + 1") {
		const char* sample = "X >=_ X + 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !>=_ X") {
		const char* sample = "X !>=_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X + 1 !>=_ X") {
		const char* sample = "X + 1!>=_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X <=_ X") {
		const char* sample = "X <=_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X + 1 <=_ X") {
		const char* sample = "X + 1 <=_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !<=_ X") {
		const char* sample = "X !<=_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X + 1 !<=_ X") {
		const char* sample = "X + 1 !<=_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X <_ X") {
		const char* sample = "X <_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X - 1 <_ X") {
		const char* sample = "X - 1 <_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !<_ X") {
		const char* sample = "X !<_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X - 1 !<_ X") {
		const char* sample = "X - 1 !<_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("variable") {
		const char* sample = "X =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_checked") {
		const char* sample = "[X] =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_neg") {
		const char* sample = "~X =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_add") {
		const char* sample = "X + 1 =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_sub") {
		const char* sample = "X - 1 =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mul") {
		const char* sample = "X * 1 =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_div") {
		const char* sample = "X / 1 =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mod") {
		const char* sample = "X % 2 =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_and") {
		const char* sample = "X & 1 =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_nand") {
		const char* sample = "2 !& 1 =_ X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_or") {
		const char* sample = "X | 1 =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_nor") {
		const char* sample = "2 !| X =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_xor") {
		const char* sample = "X (+) 1 =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_xnor") {
		const char* sample = "X !(+) 1 =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_rotate_shift") {
		const char* sample = "X << 1 =_ 2";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_rotate_right") {
		const char* sample = "X >> 1 =_ 1";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	/*TEST_CASE("cvc5_min") {
		const char* sample = "min 2 1 =_ X";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_max") {
		const char* sample = "max X 0 =_ 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}*/
}

/*TEST_SUITE("bv overflow/underflow") {

	TEST_CASE("multiplication overflow") {
		const char* sample = "X =_ [4294967296 * 8589934592]"; // 2^32 + 2^33
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		//CHECK_THROWS( solve_bv<node_t>(equation) );
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );

	}

	TEST_CASE("no multiplication overflow") {
		const char* sample = "X =_ 4294967296 * 8589934592"; // 2^32 + 2^33
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("substraction underflow") {
		const char* sample = "X =_ [1 - 2]";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		//CHECK_THROWS( solve_bv<node_t>(equation) );
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );

	}

	TEST_CASE("no substraction underflow") {
		const char* sample = "X =_ 1 - 2";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("addition overflow") {
		const char* sample = "X =_ [18446744073709551615 + 1]"; // (2^64 - 1) + 1
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		//CHECK_THROWS( solve_bv<node_t>(equation) );
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );

	}

	TEST_CASE("no addition overflow") {
		const char* sample = "X =_ 18446744073709551615 + 1"; // (2^64 - 1) + 1
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("intermediate addition overflow") {
		const char* sample = "X =_ 18446744073709551615 && Y =_ 1 && Z =_ [X + Y]"; // (2^64 - 1) + 1
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		//CHECK_THROWS( solve_bv<node_t>(equation) );
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );

	}

	TEST_CASE("intermediate no addition overflow") {
		const char* sample = "X =_ 18446744073709551615 && Y =_ 1 && Z =_ X + Y"; // (2^64 - 1) + 1
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("x + 3 =_ 2 with overflow off") {
		const char* sample = "x + 3 =_ 2";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );
	}

	TEST_CASE("x + 3 =_ 2 with overflow on") {
		const char* sample = "[x + 3] =_ 2";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		//CHECK_THROWS( solve_bv<node_t>(equation) );
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );
	}
}*/

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
