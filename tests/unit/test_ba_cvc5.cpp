// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"

tref parse_bv_formula(const std::string spec) {
	typename tau::get_options opts{
		.parse = { .start = tau::wff } };
	return tau::get(spec, opts);
}

TEST_SUITE("cvc5_parsing") {

}

TEST_SUITE("cvc5_satisfiability") {

	TEST_CASE("all x ex y x + y =_ 1") {
		const std::string sample = "all x ex y x + y =_ 1";
		auto formula = parse_bv_formula(sample);
		CHECK( is_bv_formula_sat<node_t>(formula) );
	}

	TEST_CASE("all x x + y =_ 1") {
		const std::string sample = "all x x + y =_ 1";
		auto formula = parse_bv_formula(sample);
		// y is implicitlly existentially quantified by cvc5
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );
	}

	TEST_CASE("all x x >_ 0") {
		const std::string sample = "all x x >_ 0";
		auto formula = parse_bv_formula(sample);
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );
	}

	TEST_CASE("all x x + 1 =_ 1") {
		const std::string sample = "all x x + 1 =_ 1";
		auto formula = parse_bv_formula(sample);
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );
	}

	/*TEST_CASE("all x all y (x + y) * (x + y)  =_ x * x + y* y + 2 * x * y") {
		const std::string sample = "all x all y (x + y) * (x + y)  =_ x * x + y* y + 2 * x * y";
		auto formula = parse_bv_formula(sample);
		CHECK( is_bv_formula_sat<node_t>(formula) );
	}*/

	TEST_CASE("all x x + 1 <_ 1") {
		const std::string sample = "all x x + 1 <_ 1";
		auto formula = parse_bv_formula(sample);
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );
	}

	TEST_CASE("all x [x + 1] <_ 0") {
		const std::string sample = "all x [x + 1] <_ 1";
		auto formula = parse_bv_formula(sample);
		// TODO (HIGH) change assertion when supporting overflows
		//CHECK_THROWS( is_bv_formula_sat<node_t>(formula) );
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );
	}

	TEST_CASE("all x ex y [x + y] =_ 1") {
		const std::string sample = "all x ex y [x + y] =_ 1";
		auto formula = parse_bv_formula(sample);
		// TODO (HIGH) change assertion when supporting overflows
		//CHECK_THROWS( is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_sat<node_t>(formula) );

	}

	TEST_CASE("all x [x - (x/2)] =_ x") {
		const std::string sample = "all x [x - (x/2)] =_ x";
		auto formula = parse_bv_formula(sample);
		// TODO (HIGH) change assertion when supporting overflows
		//CHECK_THROWS( is_bv_formula_sat<node_t>(formula) );
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );

	}

	TEST_CASE("all x [x - (x/2)] <=_ x") {
		const std::string sample = "all x [x - (x/2)] <=_ x";
		auto formula = parse_bv_formula(sample);
		// TODO (HIGH) change assertion when supporting overflows
		CHECK( is_bv_formula_sat<node_t>(formula) );
		//CHECK_THROWS( is_bv_formula_sat<node_t>(formula) );
	}

	TEST_CASE("all x ((x <_ 18446744073709551614) -> ([x + 1] >_ 0))") {
		const std::string sample = "all x ((x <_ 18446744073709551614) -> ([x + 1] >_ 0))";
		auto formula = parse_bv_formula(sample);
		CHECK( is_bv_formula_sat<node_t>(formula) );
	}

	/*TEST_CASE("all x ((x <_ 18446744073709551615) -> ([x + 1] = 0))") {
		const std::string sample = "all x ((x <_ 18446744073709551615) -> ([x + 1] = 0))";
		auto formula = tau::get(sample, parse_wff());
		CHECK_THROWS( is_bv_formula_sat<node_t>(formula) );
	}*/

	TEST_CASE("all x ((x <_ 5) -> ([x + 1] =_ 0))") {
		const std::string sample = "all x ((x <_ 5) -> ([x + 1] =_ 0))";
		auto formula = parse_bv_formula(sample);
		//CHECK_THROWS( is_bv_formula_sat<node_t>(formula) );
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );
	}
}

TEST_SUITE("cvc5_solve") {

	TEST_CASE("...") {
		// ...
	}

}