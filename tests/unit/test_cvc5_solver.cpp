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


	TEST_CASE("X = { 1 }") {
		const char* sample = "X = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	TEST_CASE("X != X") {
		const char* sample = "X != X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X + { 0 } > { 0 }") {
		const char* sample = "X + { 0 } > { 0 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !> X") {
		const char* sample = "X !> X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X + { 1 } !> X") {
		const char* sample = "X + { 1 } !> X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	TEST_CASE("X >= X") {
		const char* sample = "X >= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X >= X + { 1 }") {
		const char* sample = "X >= X + { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !>= X") {
		const char* sample = "X !>= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X + { 1 } !>= X") {
		const char* sample = "X + { 1 }!>= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	TEST_CASE("X <= X") {
		const char* sample = "X <= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X + { 1 } <= X") {
		const char* sample = "X + { 1 } <= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !<= X") {
		const char* sample = "X !<= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X + { 1 } !<= X") {
		const char* sample = "X + { 1 } !<= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	TEST_CASE("X < X") {
		const char* sample = "X < X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X - { 1 } < X") {
		const char* sample = "X - { 1 } < X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	TEST_CASE("X !< X") {
		const char* sample = "X !< X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X - { 1 } !< X") {
		const char* sample = "X - { 1 } !< X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
	}

	TEST_CASE("variable") {
		const char* sample = "X = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_neg") {
		const char* sample = "X' = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_add") {
		const char* sample = "X + { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_sub") {
		const char* sample = "X - { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mul") {
		const char* sample = "X * { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_div") {
		const char* sample = "X / { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mod") {
		const char* sample = "X % { 2 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_and") {
		const char* sample = "X & { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	/*TEST_CASE("cvc5_nand") {
		const char* sample = "{ 2 } !& { 1 } = X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}*/

	TEST_CASE("cvc5_or") {
		const char* sample = "X | { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	/*TEST_CASE("cvc5_nor") {
		const char* sample = "{ 2 } !| X = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}*/

	TEST_CASE("cvc5_xor") {
		const char* sample = "X ^ { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	/*TEST_CASE("cvc5_xnor") {
		const char* sample = "X !^ { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}*/

	TEST_CASE("cvc5_rotate_shift") {
		const char* sample = "X << { 1 } = { 2 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_rotate_right") {
		const char* sample = "X >> { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src, find_ba_type_tree<node_t>(src));
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	/*TEST_CASE("cvc5_min") {
		const char* sample = "min 2 1 = X";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_max") {
		const char* sample = "max X 0 = 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto equation = make_statement(src);
		auto solution = solve_bv<node_t>(equation);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}*/
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
