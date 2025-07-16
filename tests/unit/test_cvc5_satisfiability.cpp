// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "../unit/test_helpers.h"


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
}

TEST_SUITE("cvc5_satisfiability") {

	TEST_CASE("all x ex y x + y =_ 1") {
		const char* sample = "all x ex y x + y =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		CHECK( is_bv_formula_sat(formula) );
	}

	TEST_CASE("all x x + y =_ 1") {
		const char* sample = "all x x + y =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		// y is implicitlly existentially quantified by cvc5
		CHECK( !is_bv_formula_sat(formula) );
		CHECK( is_bv_formula_unsat(formula) );
		CHECK( is_bv_formula_valid(build_wff_neg(formula)) );
	}

	TEST_CASE("all x x >_ 0") {
		const char* sample = "all x x >_ 0";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		CHECK( !is_bv_formula_sat(formula) );
		CHECK( is_bv_formula_unsat(formula) );
		CHECK( is_bv_formula_valid(build_wff_neg(formula)) );
	}

	TEST_CASE("all x x + 1 =_ 1") {
		const char* sample = "all x x + 1 =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		CHECK( !is_bv_formula_sat(formula) );
		CHECK( is_bv_formula_unsat(formula) );
		CHECK( is_bv_formula_valid(build_wff_neg(formula)) );
	}

	TEST_CASE("all x all y (x + y) * (x + y)  =_ x * x + y* y + 2 * x * y") {
		const char* sample = "all x all y (x + y) * (x + y)  =_ x * x + y* y + 2 * x * y";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		CHECK( is_bv_formula_sat(formula) );
	}

	TEST_CASE("all x x + 1 <_ 1") {
		const char* sample = "all x x + 1 <_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		CHECK( !is_bv_formula_sat(formula) );
		CHECK( is_bv_formula_unsat(formula) );
		CHECK( is_bv_formula_valid(build_wff_neg(formula)) );
	}

	TEST_CASE("all x [x + 1] <_ 0") {
		const char* sample = "all x [x + 1] <_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		// TODO (HIGH) change assertion when supporting overflows
		//CHECK_THROWS( is_bv_formula_sat(formula) );
		CHECK( !is_bv_formula_sat(formula) );
		CHECK( is_bv_formula_unsat(formula) );
		CHECK( is_bv_formula_valid(build_wff_neg(formula)) );
	}

	TEST_CASE("all x ex y [x + y] =_ 1") {
		const char* sample = "all x ex y [x + y] =_ 1";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		// TODO (HIGH) change assertion when supporting overflows
		//CHECK_THROWS( is_bv_formula_sat(formula) );
		CHECK( is_bv_formula_sat(formula) );

	}

	TEST_CASE("all x [x - (x/2)] =_ x") {
		const char* sample = "all x [x - (x/2)] =_ x";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		// TODO (HIGH) change assertion when supporting overflows
		//CHECK_THROWS( is_bv_formula_sat(formula) );
		CHECK( !is_bv_formula_sat(formula) );
		CHECK( is_bv_formula_unsat(formula) );
		CHECK( is_bv_formula_valid(build_wff_neg(formula)) );

	}

	TEST_CASE("all x [x - (x/2)] <=_ x") {
		const char* sample = "all x [x - (x/2)] <=_ x";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		// TODO (HIGH) change assertion when supporting overflows
		CHECK( is_bv_formula_sat(formula) );
		//CHECK_THROWS( is_bv_formula_sat(formula) );
	}

	TEST_CASE("all x ((x <_ 18446744073709551614) -> ([x + 1] >_ 0))") {
		const char* sample = "all x ((x <_ 18446744073709551614) -> ([x + 1] >_ 0))";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		CHECK( is_bv_formula_sat(formula) );
	}

	/*TEST_CASE("all x ((x <_ 18446744073709551615) -> ([x + 1] = 0))") {
		const char* sample = "all x ((x <_ 18446744073709551615) -> ([x + 1] = 0))";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		CHECK_THROWS( is_bv_formula_sat(formula) );
	}*/

	TEST_CASE("all x ((x <_ 5) -> ([x + 1] =_ 0))") {
		const char* sample = "all x ((x <_ 5) -> ([x + 1] =_ 0))";
		auto src = make_tau_source(sample, {
						.start = tau_parser::wff });
		auto formula = make_statement(src);
		//CHECK_THROWS( is_bv_formula_sat(formula) );
		CHECK( !is_bv_formula_sat(formula) );
		CHECK( is_bv_formula_unsat(formula) );
		CHECK( is_bv_formula_valid(build_wff_neg(formula)) );
	}
}
