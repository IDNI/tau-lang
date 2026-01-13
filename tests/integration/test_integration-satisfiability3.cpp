// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"

TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		// logging::trace();
	}

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("Alignments y3") {

	TEST_CASE("equal_lookback_one_st") {
		tref spec = create_spec("(always o1[t-1]:bv = { 0 }) && (sometimes o1[t]:bv = { 1 } && o1[t-1]:bv = { 0 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("smaller_lookback_one_st") {
		tref spec = create_spec("(always o1[t]:bv = o1[t-1]:bv && o1[t-1]:bv = { 1 }) && (sometimes o2[t]:bv = { 0 }).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("greater_lookback_one_st") {
		tref spec = create_spec("(always o1[t]:bv = o1[t-1]:bv) && (sometimes o1[t]:bv != o1[t-2]:bv).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("equal_lookback_two_st") {
		tref spec = create_spec("(always o1[t]:bv = { 0 }) && (sometimes o1[t]:bv = { 0 }) && (sometimes o1[t]:bv = { 1 }).");
		CHECK(transform_to_execution<node_t>(spec) == tau::_F());
	}

	TEST_CASE("simple_conditional_case") {
		tref spec = create_spec("(always i1[t]:bv = { 1 } ? o1[t]:bv = { 0 } : o1[t]:bv = { 1 }).");
		CHECK(transform_to_execution<node_t>(spec) != tau::_F());
	}

	TEST_CASE("simple_andreis_test_case") {
		tref spec = create_spec("always (((i1[t] + i2[t]) <= ((i1[t] + i2[t]) >> { 3 }:bv)) ? o1[t] = (i1[t] + i2[t]) : o1[t] = (i1[t] + i2[t])').");
		CHECK(transform_to_execution<node_t>(spec) != tau::_F());
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
