// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "parser_helper.h"

#include "boolean_algebras/bv/bv_ba.h"


TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

// GitHub #121: a conjunction of pure bitvector equalities. `var = constant`
// conjuncts are read off before the route is chosen, and the lgrs route is
// left above `lgrs_max_vars` distinct variables.
TEST_SUITE("solve: pure bitvector equalities") {

	static tref parse_form(const std::string& src) {
		return get_nso_rr<node_t>(tau::get(src).value_or(nullptr)).value().main->get();
	}

	static solver_options bv8_options() {
		tref type = ba_types<node_t>::type_tree(bv_type_id<node_t>(8)).value();
		return { .splitter_one = node_t::ba::splitter_one(type),
			.mode = solver_mode::general };
	}

	// The solution substituted into the formula normalizes to T.
	static bool solves(tref form, const solution<node_t>& sol) {
		tref inst = rewriter::replace<node_t>(form, sol);
		auto r = normalizer<node_t>(inst);
		return r.has_value() && tau::get(r.value()).equals_T();
	}

	TEST_CASE("twelve variables assigned constants") {
		std::string src;
		for (int i = 0; i < 12; ++i)
			src += (i ? " && " : "") + std::string("o") + std::to_string(i)
				+ ":bv[8] = { " + std::to_string(i + 1) + " }:bv[8]";
		tref form = parse_form(src + ".");
		auto sol = solve<node_t>(form, bv8_options());
		REQUIRE( sol.has_value() );
		CHECK( sol.value().size() == 12 );
		for (const auto& [var, value] : sol.value())
			CHECK( tau::get(value)[0].is_ba_constant() );
		CHECK( solves(form, sol.value()) );
	}

	TEST_CASE("a constant read off into the equations that read it") {
		tref form = parse_form("o0:bv[8] = { 1 }:bv[8] && o1:bv[8] = o0:bv[8] + { 1 }:bv[8] && o2:bv[8] = o1:bv[8] | { 4 }:bv[8].");
		auto sol = solve<node_t>(form, bv8_options());
		REQUIRE( sol.has_value() );
		CHECK( sol.value().size() == 3 );
		CHECK( solves(form, sol.value()) );
	}

	TEST_CASE("two constants for one variable refute the clause") {
		tref form = parse_form("o0:bv[8] = { 1 }:bv[8] && o1:bv[8] = { 2 }:bv[8] && o0:bv[8] = { 2 }:bv[8].");
		auto sol = solve<node_t>(form, bv8_options());
		CHECK( !sol.has_value() );
	}

	TEST_CASE("the same constant twice is one assignment") {
		tref form = parse_form("o0:bv[8] = { 1 }:bv[8] && o0:bv[8] = { 1 }:bv[8] && o1:bv[8] = o0:bv[8].");
		auto sol = solve<node_t>(form, bv8_options());
		REQUIRE( sol.has_value() );
		CHECK( solves(form, sol.value()) );
	}

	// GitHub #136: a pure-equality partition gets constants, not a
	// reproductive solution such as x := x|y that no caller can commit.
	static bool ground(const solution<node_t>& sol) {
		for (const auto& [var, value] : sol)
			if (tau::get(value).find_top(is_child<node_t, tau::variable>))
				return false;
		return true;
	}

	TEST_CASE("equal variables get ground values") {
		tref form = parse_form("x:bv[8] = y:bv[8].");
		auto sol = solve<node_t>(form, bv8_options());
		REQUIRE( sol.has_value() );
		CHECK( sol.value().size() == 2 );
		CHECK( ground(sol.value()) );
		CHECK( solves(form, sol.value()) );
	}

	TEST_CASE("equal variables get ground values in minimum mode") {
		tref form = parse_form("x:bv[8] = y:bv[8] && y:bv[8] = z:bv[8].");
		solver_options op = bv8_options();
		op.mode = solver_mode::minimum;
		auto sol = solve<node_t>(form, op);
		REQUIRE( sol.has_value() );
		CHECK( sol.value().size() == 3 );
		CHECK( ground(sol.value()) );
		CHECK( solves(form, sol.value()) );
	}

	TEST_CASE("above the lgrs cap the pack solver answers") {
		const size_t saved = lgrs_max_vars;
		lgrs_max_vars = 1;
		tref form = parse_form("o0:bv[8] = o1:bv[8] && o1:bv[8] = o2:bv[8] && o2:bv[8] != { 0 }:bv[8].");
		auto sol = solve<node_t>(form, bv8_options());
		lgrs_max_vars = saved;
		REQUIRE( sol.has_value() );
		CHECK( solves(form, sol.value()) );
	}
}

TEST_SUITE("cvc5_solve simple") {

	TEST_CASE("X = { 1 }:bv[16]") {
		const char* sample = "X = { 1 }:bv[16]";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] != X") {
		const char* sample = "X:bv[16] != X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] + { 0 } > { 0 }") {
		const char* sample = "X:bv[16] + { 0 } > { 0 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !> X") {
		const char* sample = "X:bv[16] !> X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] + { 1 } !> X") {
		const char* sample = "X:bv[16] + { 1 } !> X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] >= X") {
		const char* sample = "X:bv[16] >= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] >= X + { 1 }") {
		const char* sample = "X:bv[16] >= X + { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !>= X") {
		const char* sample = "X:bv[16] !>= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] + { 1 } !>= X") {
		const char* sample = "X:bv[16] + { 1 } !>= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] <= X") {
		const char* sample = "X:bv[16] <= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] + { 1 } <= X") {
		const char* sample = "X:bv[16] + { 1 } <= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !<= X") {
		const char* sample = "X:bv[16] !<= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] + { 1 } !<= X") {
		const char* sample = "X:bv[16] + { 1 } !<= X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] < X") {
		const char* sample = "X:bv[16] < X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( !solution.has_value() );
	}

	TEST_CASE("X:bv[16] - { 1 } < X") {
		const char* sample = "X:bv[16] - { 1 } < X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("X:bv[16] !< X") {
		const char* sample = "X:bv[16] !< X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// take into account modular arithmetic
	TEST_CASE("X:bv[16] - { 1 } !< X") {
		const char* sample = "X:bv[16] - { 1 } !< X";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	TEST_CASE("variable") {
		const char* sample = "X:bv[16] = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_neg") {
		const char* sample = "X:bv[16]' = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_add") {
		const char* sample = "X:bv[16] + { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_sub") {
		const char* sample = "X:bv[16] - { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mul") {
		const char* sample = "X:bv[16] * { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_div") {
		const char* sample = "X:bv[16] / { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_mod") {
		const char* sample = "X:bv[16] % { 2 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_and") {
		const char* sample = "X:bv[16] & { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_nand") {
		const char* sample = "{ 2 } !& { 1 } = X:bv[16]";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_or") {
		const char* sample = "X:bv[16] | { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_nor") {
		const char* sample = "{ 2 } !| X:bv[16] = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_xor") {
		const char* sample = "X:bv[16] ^ { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_xnor") {
		const char* sample = "X:bv[16] !^ { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_left_shift") {
		const char* sample = "X:bv[16] << { 1 } = { 2 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}

	TEST_CASE("cvc5_right_shift") {
		const char* sample = "X:bv[16] >> { 1 } = { 1 }";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
		CHECK( solution.value().size() == 1 );
	}
}

TEST_SUITE("regression") {

	TEST_CASE("Andrei's example (y1)") {
		const char* sample = "G (((i5[t]:bv[64] * i6[t]:bv[64]) !<= ((i5[t]:bv[64] * i6[t]:bv[64]) >> { 3 }:bv[64])) ? o1[t]:bv[64] = (i5[t]:bv[64] * i6[t]:bv[64]) : o1[t]:bv[64] = (0 - (i5[t]:bv[64] * i6[t]:bv[64])))";
		tref src = tau::get(sample, parse_opts_wff).value_or(nullptr);
		// tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		auto bv_tree = bv_eval_node<node_t>(src, vars, free_vars);
		CHECK(bv_tree.has_value());
	}

	TEST_CASE("Andrei's example (y2)") {
		// Removed unused quantified vars (b5, b7 from outer all; duplicate b6 from inner all)
		const char* sample = "all b6 b6*b6 != b6 || b6*b6>>{ 3 }:bv[64] < b6*b6 || (all b6 (ex o1[1]:bv[64] b6*b6 = o1[1]:bv[64] && b6*b6 != o1[1]:bv[64]) || b6*b6 = b6*b6)";
		tref src = tau::get(sample, parse_opts_wff).value_or(nullptr);
		// tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		auto bv_tree = bv_eval_node<node_t>(src, vars, free_vars);
		CHECK(bv_tree.has_value());
	}

	// Disable due to excesive runtime
	/*TEST_CASE("Andrei's example (y3)") {
		const char* sample = "all b8 (ex b6, b4, b8 b8*b8>>{ 3 }:bv[64] !< b8*b8 && b8*b8 != b8*b8 && (all o1[3]:bv[64] b8*b8 != o1[3]:bv[64] || b8*b8 = o1[3]:bv[64])) || (ex b4, b4, b8 (all o1[2]:bv[64] b8*b8 != o1[2]:bv[64] || b8*b8 = o1[2]:bv[64]) && b8*b8>>{ 3 }:bv[64] !< b8*b8 && b8*b8 != b8*b8) || (all b10 b10*b8>>{ 3 }:bv[64] < b10*b8 || (all b9 b8*b10 != b9) || (ex b8, b10 (all o1[1]:bv[64] b8*b10 != o1[1]:bv[64] || b8*b10 = o1[1]:bv[64]) && b8*b10 != b8*b10) || (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || (ex o1[4]:bv[64] b8*b8 = o1[4]:bv[64] && b8*b8 != o1[4]:bv[64]) || b8*b8 = b8*b8) && (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8 || (ex o1[3]:bv[64] b8*b8 != o1[3]:bv[64] && b8*b8 = o1[3]:bv[64])) && (all b8, b10 b8*b10 = b8*b10 || (ex o1[1]:bv[64] b8*b10 != o1[1]:bv[64] && b8*b10 = o1[1]:bv[64])) && (all b4, b4, b8 (ex o1[2]:bv[64] b8*b8 != o1[2]:bv[64] && b8*b8 = o1[2]:bv[64]) || b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8)) && (all b10 (all b9 b8*b10 != b9) || (ex b8, b10 F) || b10*b8>>{ 3 }:bv[64] !< b10*b8 || (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || (ex o1[4]:bv[64] b8*b8 = o1[4]:bv[64] && b8*b8 != o1[4]:bv[64]) || b8*b8 = b8*b8) && (all b6, b4, b8 b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8 || (ex o1[3]:bv[64] b8*b8 != o1[3]:bv[64] && b8*b8 = o1[3]:bv[64])) && (all b8, b10 T) && (all b4, b4, b8 (ex o1[2]:bv[64] b8*b8 != o1[2]:bv[64] && b8*b8 = o1[2]:bv[64]) || b8*b8>>{ 3 }:bv[64] < b8*b8 || b8*b8 = b8*b8))";
		tref src = tau::get(sample, parse_opts_wff).value_or(nullptr);
		tau::get(src).print_tree(std::cout << "parse tree: ") << "\n";
		subtree_map<node_t, bv> vars, free_vars;
		auto bv_tree = bv_eval_node<node_t>(src, vars, free_vars);
		CHECK(bv_tree.has_value());
	}*/
}

// Ported from main's test_bv_ba-solver2.cpp (deleted on the LTL branch as
// superseded; this case had no counterpart). BA-15: bv_eval_node's vars map
// is passed by reference, so the wff_all/wff_ex cases must save and restore
// any outer binding they shadow (nested quantifiers reusing the same name),
// or the outer "x" after the inner quantifier closes would fall through to
// the free-variable case and leak as an unbound variable in the solution.
TEST_SUITE("cvc5_solve quantifier shadowing") {
	TEST_CASE("outer x is still bound after an inner quantifier reuses the same name") {
		const char* sample =
			"ex x (x = { 1 }:bv[16] && (ex x (x = { 2 }:bv[16])) && x = { 1 }:bv[16])";
		auto src = tau::get(sample, parse_opts_wff_no_hooks).value_or(nullptr);
		auto solution = solve_bv<node_t>(src);
		REQUIRE( solution.has_value() );
		CHECK( solution->empty() );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
