// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"
#include "parser_helper.h"

TEST_SUITE("Configuration") {
	TEST_CASE("logging") {
		// logging::trace();
	}
	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("Alignments bv[16]") {
	TEST_CASE("equal_lookback_one_st") {
		tref spec = create_spec("(always o1[t-1]:bv[16] = { 0 }) && (sometimes o1[t]:bv[16] = { 1 } && o1[t-1]:bv[16] = { 0 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("smaller_lookback_one_st") {
		tref spec = create_spec("(always o1[t]:bv[16] = o1[t-1]:bv[16] && o1[t-1]:bv[16] = { 1 }) && (sometimes o2[t]:bv[16] = { 0 }).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("greater_lookback_one_st") {
		tref spec = create_spec("(always o1[t]:bv[16] = o1[t-1]:bv[16]) && (sometimes o1[t]:bv[16] != o1[t-2]:bv[16]).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("equal_lookback_two_st") {
		tref spec = create_spec("(always o1[t]:bv[16] = { 0 }) && (sometimes o1[t]:bv[16] = { 0 }) && (sometimes o1[t]:bv[16] = { 1 }).");
		CHECK(transform_to_execution<node_t>(spec) == tau::_F());
	}
	TEST_CASE("simple_conditional_case") {
		tref spec = create_spec("(always i1[t]:bv[16] = { 1 } ? o1[t]:bv[16] = { 0 } : o1[t]:bv[16] = { 1 }).");
		CHECK(transform_to_execution<node_t>(spec) != tau::_F());
	}
	TEST_CASE("simple_andreis_test_case") {
		tref spec = create_spec("always (((i1[t] + i2[t]) <= ((i1[t] + i2[t]) >> { 3 }:bv[16])) ? o1[t] = (i1[t] + i2[t]) : o1[t] = (i1[t] + i2[t])').");
		CHECK(transform_to_execution<node_t>(spec) != tau::_F());
	}
}

TEST_SUITE("Alignments bv[4]") {
	// TODO (HIGH) fails in cli (returns T)
	TEST_CASE("greater_lookback_two_st_1") {
		tref spec = create_spec("(always o1[t]:bv[4] = { 1 } && o2[t]:bv[4] = { 1 }) && (sometimes o1[t-1]:bv[4] = { 1 }) && (sometimes o2[t-2]:bv[4] = { 0 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	// TODO (HIGH) fails in cli (returns T)
	TEST_CASE("greater_lookback_two_st_2") {
		tref spec = create_spec("(always o1[t]:bv[4] = { 1 } && o2[t]:bv[4] = { 1 }) && (sometimes o1[t-1]:bv[4] = { 0 }) && (sometimes o2[t-2]:bv[4] = { 1 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	// TODO (HIGH) fails in cli (returns T)
	TEST_CASE("smaller_lookback_two_st_1") {
		tref spec = create_spec("(always o1[t-2]:bv[4] = { 0 } && o2[t-2]:bv[4] = { 0 }) && (sometimes o1[t]:bv[4] = { 1 }) && (sometimes o1[t-1]:bv[4] = { 0 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	// TODO (HIGH) fails in cli (returns T)
	TEST_CASE("smaller_lookback_two_st_2") {
		tref spec = create_spec("(always o1[t-2]:bv[4] = { 0 } && o2[t-2]:bv[4] = { 0 }) && (sometimes o1[t]:bv[4] = { 0 }) && (sometimes o1[t-1]:bv[4] = { 1 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	// TODO (HIGH) fails in cli (returns T)
	TEST_CASE("mixed_lookback_two_st_1") {
		tref spec = create_spec("(always o1[t-2]:bv[4] = { 1 }) && (sometimes o1[t-3]:bv[4] = { 0 }) && (sometimes o1[t]:bv[4] = { 1 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	// TODO (HIGH) fails in cli (returns T)
	TEST_CASE("mixed_lookback_two_st_2") {
		tref spec = create_spec("(always o1[t-2]:bv[4] = { 1 }) && (sometimes o1[t-3]:bv[4] = { 1 }) && (sometimes o1[t]:bv[4] = { 0 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("cvc5_satisfiability") {

	TEST_CASE("all x ex y x + y = { #b1 }:bv[4]") {
		const std::string sample = "all x ex y x + y = { #b1 }:bv[4]";
		auto formula = tau::get(sample, parse_opts_wff);
		CHECK( is_bv_formula_sat<node_t>(formula) );
	}

	TEST_CASE("all x x + y = { #b1 }:bv[4]") {
		const std::string sample = "all x x + y = { #b1 }:bv[4]";
		auto formula = tau::get(sample, parse_opts_wff);
		// y is implicitlly existentially quantified by cvc5
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );
	}

	TEST_CASE("all x x > { 0 }") {
		const std::string sample = "all x x > { 0 }:bv[4]";
		auto formula = tau::get(sample, parse_opts_wff);
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );
	}

	TEST_CASE("all x x + { 1 } = { 1 }") {
		const std::string sample = "all x x + { 1 } = { 1 }:bv[4]";
		auto formula = tau::get(sample, parse_opts_wff);
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );
	}

	TEST_CASE("all x x + { 1 } < { 1 }") {
		const std::string sample = "all x x + { 1 } < { 1 }:bv[4]";
		auto formula = tau::get(sample, parse_opts_wff);
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );
	}

	TEST_CASE("all x x + { 1 }:bv[4] < { 1 }:bv[4]") {
		const std::string sample = "all x x + { 1 }:bv[4] < { 1 }:bv[4]";
		auto formula = tau::get(sample, parse_opts_wff);
		// TODO (HIGH) change assertion when supporting overflows
		CHECK( !is_bv_formula_sat<node_t>(formula) );
		CHECK( is_bv_formula_unsat<node_t>(formula) );
		CHECK( is_bv_formula_valid<node_t>(build_wff_neg<node_t>(formula)) );
	}
}

// min/max are converted for cvc5 as ite(bvule(a,b), ...); these check the
// conversion by validity/satisfiability of the defining properties.
TEST_SUITE("cvc5_satisfiability: min/max") {

	TEST_CASE("all x all y min(x, y:bv[4]) <= x") {
		auto formula = tau::get("all x all y min(x, y:bv[4]) <= x",
			parse_opts_wff);
		CHECK( is_bv_formula_valid<node_t>(formula) );
	}

	TEST_CASE("all x all y ( min(x, y:bv[4]) = x || min(x, y) = y )") {
		auto formula = tau::get(
			"all x all y ( min(x, y:bv[4]) = x || min(x, y) = y )",
			parse_opts_wff);
		CHECK( is_bv_formula_valid<node_t>(formula) );
	}

	TEST_CASE("all x all y min(x, y:bv[4]) + max(x, y) = x + y") {
		auto formula = tau::get(
			"all x all y min(x, y:bv[4]) + max(x, y) = x + y",
			parse_opts_wff);
		CHECK( is_bv_formula_valid<node_t>(formula) );
	}

	TEST_CASE("ex x min(x, { 3 }:bv[4]) = { 2 }:bv[4]") {
		auto formula = tau::get(
			"ex x min(x, { 3 }:bv[4]) = { 2 }:bv[4]", parse_opts_wff);
		CHECK( is_bv_formula_sat<node_t>(formula) );
	}

	TEST_CASE("ex x max(x, { 3 }:bv[4]) < { 3 }:bv[4]") {
		auto formula = tau::get(
			"ex x max(x, { 3 }:bv[4]) < { 3 }:bv[4]", parse_opts_wff);
		CHECK( is_bv_formula_unsat<node_t>(formula) );
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
