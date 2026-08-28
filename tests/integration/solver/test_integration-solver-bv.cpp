// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "parser_helper.h"

#include "boolean_algebras/bv_ba.h"
#include "test_integration-solver_helper.h"


TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("cvc5_solve simple") {

	TEST_CASE("X = { 1 }:bv[16]") {
		const char* sample = "X = { 1 }:bv[16]";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] != X") {
		const char* sample = "X:bv[16] != X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] + { 0 } > { 0 }") {
		const char* sample = "X:bv[16] + { 0 } > { 0 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !> X") {
		const char* sample = "X:bv[16] !> X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] + { 1 } !> X") {
		const char* sample = "X:bv[16] + { 1 } !> X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] >= X") {
		const char* sample = "X:bv[16] >= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] >= X + { 1 }") {
		const char* sample = "X:bv[16] >= X + { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !>= X") {
		const char* sample = "X:bv[16] !>= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] + { 1 } !>= X") {
		const char* sample = "X:bv[16] + { 1 } !>= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] <= X") {
		const char* sample = "X:bv[16] <= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] + { 1 } <= X") {
		const char* sample = "X:bv[16] + { 1 } <= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !<= X") {
		const char* sample = "X:bv[16] !<= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] + { 1 } !<= X") {
		const char* sample = "X:bv[16] + { 1 } !<= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] < X") {
		const char* sample = "X:bv[16] < X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] - { 1 } < X") {
		const char* sample = "X:bv[16] - { 1 } < X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !< X") {
		const char* sample = "X:bv[16] !< X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] - { 1 } !< X") {
		const char* sample = "X:bv[16] - { 1 } !< X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("variable") {
		const char* sample = "X:bv[16] = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_neg") {
		const char* sample = "X:bv[16]' = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_add") {
		const char* sample = "X:bv[16] + { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_sub") {
		const char* sample = "X:bv[16] - { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mul") {
		const char* sample = "X:bv[16] * { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_div") {
		const char* sample = "X:bv[16] / { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mod") {
		const char* sample = "X:bv[16] % { 2 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_and") {
		const char* sample = "X:bv[16] & { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_nand") {
		const char* sample = "{ 2 } !& { 1 } = X:bv[16]";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_or") {
		const char* sample = "X:bv[16] | { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_nor") {
		const char* sample = "{ 2 } !| X:bv[16] = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_xor") {
		const char* sample = "X:bv[16] ^ { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_xnor") {
		const char* sample = "X:bv[16] !^ { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_left_shift") {
		const char* sample = "X:bv[16] << { 1 } = { 2 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_right_shift") {
		const char* sample = "X:bv[16] >> { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}
}

TEST_SUITE("regression") {

	TEST_CASE("Andrei's example (y1)") {
		const char* sample = "G (((i5[t]:bv[64] * i6[t]:bv[64]) !<= ((i5[t]:bv[64] * i6[t]:bv[64]) >> { 3 }:bv[64])) ? o1[t]:bv[64] = (i5[t]:bv[64] * i6[t]:bv[64]) : o1[t]:bv[64] = (0 - (i5[t]:bv[64] * i6[t]:bv[64])))";
		tref src = tau::get(sample, parse_opts_wff);
		// tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		auto bv_tree = bv_eval_node<node_t>(src, vars, free_vars);
		CHECK(bv_tree.has_value());
	}

	TEST_CASE("Andrei's example (y2)") {
		// Removed unused quantified vars (b5, b7 from outer all; duplicate b6 from inner all)
		const char* sample = "all b6 b6*b6 != b6 || b6*b6>>{ 3 }:bv[64] < b6*b6 || (all b6 (ex o1[1]:bv[64] b6*b6 = o1[1]:bv[64] && b6*b6 != o1[1]:bv[64]) || b6*b6 = b6*b6)";
		tref src = tau::get(sample, parse_opts_wff);
		// tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		auto bv_tree = bv_eval_node<node_t>(src, vars, free_vars);
		CHECK(bv_tree.has_value());
	}

	// Disable due to excesive runtime
	/*TEST_CASE("Andrei's example (y3)") {
		const char* sample = "all b8 (ex b6, b4, b8 b8*b8>>{ 3 }:bv[64] !< b8*b8 && b8*b8 != b8*b8 && (all o1[3]:bv[64] b8*b8 != o1[3]:bv[64] || b8*b8 = o1[3]:bv[64])) || (ex b4, b4, b8 (all o1[2]:bv[64] b8*b8 != o1[2]:bv[64] || b8*b8 = o1[2]:bv[64]) && b8*b8>>{ 3 }:bv[64] !< b8*b8 && b8*b8 != b8*b8) || (all b10 b10*b8>>{ 3 }:bv[64] < b10*b8 || (all b9 b8*b10 != b9) || (ex b8, b10 (all o1[1]:bv[64] b8*b10 != o1[1]:bv[64] || b8*b10 = o1[1]:bv[64]) && b8*b10 != b8*b10) || (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || (ex o1[4]:bv[64] b8*b8 = o1[4]:bv[64] && b8*b8 != o1[4]:bv[64]) || b8*b8 = b8*b8) && (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8 || (ex o1[3]:bv[64] b8*b8 != o1[3]:bv[64] && b8*b8 = o1[3]:bv[64])) && (all b8, b10 b8*b10 = b8*b10 || (ex o1[1]:bv[64] b8*b10 != o1[1]:bv[64] && b8*b10 = o1[1]:bv[64])) && (all b4, b4, b8 (ex o1[2]:bv[64] b8*b8 != o1[2]:bv[64] && b8*b8 = o1[2]:bv[64]) || b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8)) && (all b10 (all b9 b8*b10 != b9) || (ex b8, b10 F) || b10*b8>>{ 3 }:bv[64] !< b10*b8 || (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || (ex o1[4]:bv[64] b8*b8 = o1[4]:bv[64] && b8*b8 != o1[4]:bv[64]) || b8*b8 = b8*b8) && (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8 || (ex o1[3]:bv[64] b8*b8 != o1[3]:bv[64] && b8*b8 = o1[3]:bv[64])) && (all b8, b10 T) && (all b4, b4, b8 (ex o1[2]:bv[64] b8*b8 != o1[2]:bv[64] && b8*b8 = o1[2]:bv[64]) || b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8))";
		tref src = tau::get(sample, parse_opts_wff);
		tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		auto bv_tree = bv_eval_node<node_t>(src, vars, free_vars);
		CHECK(bv_tree.has_value());
	}*/
}

// Ported from main's tests/integration/test_integration-solver.cpp (bv cast
// cases added for #86/#87/#88); that file is superseded here by this split.
TEST_SUITE("solve bv cast") {

	// A cast is arithmetic to the solver: the lgrs fast path cannot see
	// across widths, so a cast equation must go to the bitvector solver.
	// Routing it through lgrs used to answer "no solution" to a widening
	// cast that is trivially satisfiable.
	TEST_CASE("bv: widening cast equality goes to the bv solver") {
		CHECK ( test_solve("(bv[16]) x:bv[8] = { 5 }:bv[16].", tau_type<node_t>()) );
		CHECK ( !test_solve("(bv[16]) x:bv[8] = { 256 }:bv[16].", tau_type<node_t>()) );
	}

	TEST_CASE("bv: narrowing cast equality goes to the bv solver") {
		CHECK ( test_solve("(bv[8]) x:bv[16] = { 7 }:bv[8].", tau_type<node_t>()) );
	}

	// A cast lets one variable occur in atoms of two widths. The solver
	// used to partition atoms by type and solve each partition on its
	// own, so `d` in the bv[16] quotient and `d` in the bv[8] guard were
	// assigned independently and the second overwrote the first: the
	// first system below reported a "solution" with d = 0.
	TEST_CASE("bv: one variable across widths via a cast is solved jointly") {
		// a non-zero divisor can never push the widened quotient past 255
		CHECK ( !test_solve("(((bv[16]) x:bv[8]) / ((bv[16]) d:bv[8])) > { 255 }:bv[16]"
			" && d:bv[8] != { 0 }:bv[8].", tau_type<node_t>()) );
		CHECK ( test_solve("(((bv[16]) x:bv[8]) / ((bv[16]) d:bv[8])) > { 255 }:bv[16]"
			" && d:bv[8] = { 0 }:bv[8].", tau_type<node_t>()) );
		CHECK ( test_solve("(((bv[16]) x:bv[8]) / ((bv[16]) d:bv[8])) = { 5 }:bv[16]"
			" && d:bv[8] = { 3 }:bv[8].", tau_type<node_t>()) );
	}

	// Pure equalities of different widths keep their per-width lgrs path.
	TEST_CASE("bv: pure equalities of different widths") {
		CHECK ( test_solve("x:bv[8] = { 1 }:bv[8] && y:bv[16] = { 2 }:bv[16].", tau_type<node_t>()) );
		CHECK ( !test_solve("x:bv[8] = { 1 }:bv[8] && x:bv[8] = { 2 }:bv[8] && y:bv[16] = { 2 }:bv[16].", tau_type<node_t>()) );
	}

	TEST_CASE("bv: cast under an operator") {
		// 255 * 5 = 1275 does not fit in 8 bits but does in 16: the widened
		// product is exact, so the overflow test below is decidable.
		CHECK ( test_solve("((bv[16]) x:bv[8]) * { 5 }:bv[16] = { 1275 }:bv[16].", tau_type<node_t>()) );
		CHECK ( test_solve("((bv[16]) x:bv[8]) * { 5 }:bv[16] > { 255 }:bv[16].", tau_type<node_t>()) );
		CHECK ( !test_solve("((bv[16]) x:bv[8]) * { 5 }:bv[16] > { 1275 }:bv[16].", tau_type<node_t>()) );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
