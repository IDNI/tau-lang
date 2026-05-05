// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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

	TEST_CASE("X = { 1 }:bv[16]") {
		const char* sample = "X = { 1 }:bv[16]";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] != X") {
		const char* sample = "X:bv[16] != X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] + { 0 } > { 0 }") {
		const char* sample = "X:bv[16] + { 0 } > { 0 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !> X") {
		const char* sample = "X:bv[16] !> X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] + { 1 } !> X") {
		const char* sample = "X:bv[16] + { 1 } !> X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] >= X") {
		const char* sample = "X:bv[16] >= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] >= X + { 1 }") {
		const char* sample = "X:bv[16] >= X + { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !>= X") {
		const char* sample = "X:bv[16] !>= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] + { 1 } !>= X") {
		const char* sample = "X:bv[16] + { 1 } !>= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] <= X") {
		const char* sample = "X:bv[16] <= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] + { 1 } <= X") {
		const char* sample = "X:bv[16] + { 1 } <= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !<= X") {
		const char* sample = "X:bv[16] !<= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] + { 1 } !<= X") {
		const char* sample = "X:bv[16] + { 1 } !<= X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] < X") {
		const char* sample = "X:bv[16] < X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] - { 1 } < X") {
		const char* sample = "X:bv[16] - { 1 } < X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !< X") {
		const char* sample = "X:bv[16] !< X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] - { 1 } !< X") {
		const char* sample = "X:bv[16] - { 1 } !< X";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("variable") {
		const char* sample = "X:bv[16] = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_neg") {
		const char* sample = "X:bv[16]' = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_add") {
		const char* sample = "X:bv[16] + { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_sub") {
		const char* sample = "X:bv[16] - { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mul") {
		const char* sample = "X:bv[16] * { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_div") {
		const char* sample = "X:bv[16] / { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mod") {
		const char* sample = "X:bv[16] % { 2 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_and") {
		const char* sample = "X:bv[16] & { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_nand") {
		const char* sample = "{ 2 } !& { 1 } = X:bv[16]";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_or") {
		const char* sample = "X:bv[16] | { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_nor") {
		const char* sample = "{ 2 } !| X:bv[16] = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_xor") {
		const char* sample = "X:bv[16] ^ { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_xnor") {
		const char* sample = "X:bv[16] !^ { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_left_shift") {
		const char* sample = "X:bv[16] << { 1 } = { 2 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_right_shift") {
		const char* sample = "X:bv[16] >> { 1 } = { 1 }";
		auto src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	// BA-15: bv_eval_node's vars map is now passed by reference for
	// performance instead of by value per recursive call; the wff_all/
	// wff_ex cases must save and restore any outer binding they shadow
	// (nested quantifiers reusing the same tref for the same variable
	// name), or the outer "x" reference after the inner quantifier closes
	// would incorrectly fall through to the free-variable case -- leaking
	// as an unbound free variable in the solution instead of staying
	// bound by the outer "ex x".
	TEST_CASE("outer x is still bound after an inner quantifier reuses the same name") {
		const char* sample =
			"ex x (x = { 1 }:bv[16] && (ex x (x = { 2 }:bv[16])) && x = { 1 }:bv[16])";
		tref src = parse(sample);
		auto solution = solve_bv<node_t>(src);
		REQUIRE(solution.has_value());
		CHECK(solution->empty());
	}
}

TEST_SUITE("regression") {

	TEST_CASE("Andrei's example (y1)") {
		const char* sample = "G (((i5[t]:bv[64] * i6[t]:bv[64]) !<= ((i5[t]:bv[64] * i6[t]:bv[64]) >> { 3 }:bv[64])) ? o1[t]:bv[64] = (i5[t]:bv[64] * i6[t]:bv[64]) : o1[t]:bv[64] = (0 - (i5[t]:bv[64] * i6[t]:bv[64])))";
		tref src = parse(sample);
		// tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		auto bv_tree = bv_eval_node<node_t>(src, vars, free_vars);
		CHECK(bv_tree.has_value());
	}

	TEST_CASE("Andrei's example (y2)") {
		// Removed unused quantified vars (b5, b7 from outer all; duplicate b6 from inner all)
		const char* sample = "all b6 b6*b6 != b6 || b6*b6>>{ 3 }:bv[64] < b6*b6 || (all b6 (ex o1[1]:bv[64] b6*b6 = o1[1]:bv[64] && b6*b6 != o1[1]:bv[64]) || b6*b6 = b6*b6)";
		tref src = parse(sample);
		// tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		auto bv_tree = bv_eval_node<node_t>(src, vars, free_vars);
		CHECK(bv_tree.has_value());
	}

	// Disable due to excesive runtime
	/*TEST_CASE("Andrei's example (y3)") {
		const char* sample = "all b8 (ex b6, b4, b8 b8*b8>>{ 3 }:bv[64] !< b8*b8 && b8*b8 != b8*b8 && (all o1[3]:bv[64] b8*b8 != o1[3]:bv[64] || b8*b8 = o1[3]:bv[64])) || (ex b4, b4, b8 (all o1[2]:bv[64] b8*b8 != o1[2]:bv[64] || b8*b8 = o1[2]:bv[64]) && b8*b8>>{ 3 }:bv[64] !< b8*b8 && b8*b8 != b8*b8) || (all b10 b10*b8>>{ 3 }:bv[64] < b10*b8 || (all b9 b8*b10 != b9) || (ex b8, b10 (all o1[1]:bv[64] b8*b10 != o1[1]:bv[64] || b8*b10 = o1[1]:bv[64]) && b8*b10 != b8*b10) || (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || (ex o1[4]:bv[64] b8*b8 = o1[4]:bv[64] && b8*b8 != o1[4]:bv[64]) || b8*b8 = b8*b8) && (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8 || (ex o1[3]:bv[64] b8*b8 != o1[3]:bv[64] && b8*b8 = o1[3]:bv[64])) && (all b8, b10 b8*b10 = b8*b10 || (ex o1[1]:bv[64] b8*b10 != o1[1]:bv[64] && b8*b10 = o1[1]:bv[64])) && (all b4, b4, b8 (ex o1[2]:bv[64] b8*b8 != o1[2]:bv[64] && b8*b8 = o1[2]:bv[64]) || b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8)) && (all b10 (all b9 b8*b10 != b9) || (ex b8, b10 F) || b10*b8>>{ 3 }:bv[64] !< b10*b8 || (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || (ex o1[4]:bv[64] b8*b8 = o1[4]:bv[64] && b8*b8 != o1[4]:bv[64]) || b8*b8 = b8*b8) && (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8 || (ex o1[3]:bv[64] b8*b8 != o1[3]:bv[64] && b8*b8 = o1[3]:bv[64])) && (all b8, b10 T) && (all b4, b4, b8 (ex o1[2]:bv[64] b8*b8 != o1[2]:bv[64] && b8*b8 = o1[2]:bv[64]) || b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8))";
		tref src = parse(sample);
		tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		auto bv_tree = bv_eval_node<node_t>(src, vars, free_vars);
		CHECK(bv_tree.has_value());
	}*/
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
