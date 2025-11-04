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

TEST_SUITE("tree translation and simplification") {

	tref parse(const std::string& sample) {
		auto opts = tau::get_options{
			.parse = { .start = tau::wff },
			.reget_with_hooks = true
		};
		tref src = tree<node_t>::get(sample, opts);
		if (src == nullptr) {
			TAU_LOG_ERROR << "Parsing failed for: " << sample;
		}
		return src;
	}

	TEST_CASE("1") {
		const char* sample = "(ex x x = { 1 }:bv) && { 16 } : bv[7] - { 8 } = h && asdf' = j+k:bv[4] || !(j = i)";
		tref src = parse(sample);
		tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		bv bv_tree = bv_eval_node<node_t>(src, vars, free_vars).value();
		tref tau_tree = cvc5_tree_to_tau_tree<node_t>(bv_tree);
		CHECK(tau::get(src).to_str() == tau::get(tau_tree).to_str());
	}
	TEST_CASE("2") {
		const char* sample = "{ 8 } + { 16 } = { 24 } : bv[8]";
		tref src = parse(sample);
		tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		bv bv_tree = bv_eval_node<node_t>(src, vars, free_vars).value();
		tref tau_tree = cvc5_tree_to_tau_tree<node_t>(bv_tree);
		tau_tree = simplify_bv<node_t>(tau_tree);
		CHECK(tau::get(tau_tree).equals_T());
	}
	TEST_CASE("3") {
		const char* sample = "{ 8 } + { 16 } = { 25 } : bv[8]";
		tref src = parse(sample);
		tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		bv bv_tree = bv_eval_node<node_t>(src, vars, free_vars).value();
		tref tau_tree = cvc5_tree_to_tau_tree<node_t>(bv_tree);
		tau_tree = simplify_bv<node_t>(tau_tree);
		CHECK(tau::get(tau_tree).equals_F());
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

	TEST_CASE("X = { 1 }:bv") {
		const char* sample = "X = { 1 }:bv";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv != X") {
		const char* sample = "X:bv != X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv + { 0 } > { 0 }") {
		const char* sample = "X:bv + { 0 } > { 0 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv !> X") {
		const char* sample = "X:bv !> X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv + { 1 } !> X") {
		const char* sample = "X:bv + { 1 } !> X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv >= X") {
		const char* sample = "X:bv >= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv >= X + { 1 }") {
		const char* sample = "X:bv >= X + { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv !>= X") {
		const char* sample = "X:bv !>= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv + { 1 } !>= X") {
		const char* sample = "X:bv + { 1 } !>= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv <= X") {
		const char* sample = "X:bv <= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv + { 1 } <= X") {
		const char* sample = "X:bv + { 1 } <= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv !<= X") {
		const char* sample = "X:bv !<= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv + { 1 } !<= X") {
		const char* sample = "X:bv + { 1 } !<= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv < X") {
		const char* sample = "X:bv < X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv - { 1 } < X") {
		const char* sample = "X:bv - { 1 } < X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv !< X") {
		const char* sample = "X:bv !< X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv - { 1 } !< X") {
		const char* sample = "X:bv - { 1 } !< X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("variable") {
		const char* sample = "X:bv = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_neg") {
		const char* sample = "X:bv' = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_add") {
		const char* sample = "X:bv + { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_sub") {
		const char* sample = "X:bv - { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mul") {
		const char* sample = "X:bv * { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_div") {
		const char* sample = "X:bv / { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mod") {
		const char* sample = "X:bv % { 2 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_and") {
		const char* sample = "X:bv & { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	/*TEST_CASE("cvc5_nand") {
		const char* sample = "{ 2 } !& { 1 } = X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}*/

	TEST_CASE("cvc5_or") {
		const char* sample = "X:bv | { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	/*TEST_CASE("cvc5_nor") {
		const char* sample = "{ 2 } !| X = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}*/

	TEST_CASE("cvc5_xor") {
		const char* sample = "X:bv ^ { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	/*TEST_CASE("cvc5_xnor") {
		const char* sample = "X !^ { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}*/

	TEST_CASE("cvc5_left_shift") {
		const char* sample = "X:bv << { 1 } = { 2 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_right_shift") {
		const char* sample = "X:bv >> { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
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
