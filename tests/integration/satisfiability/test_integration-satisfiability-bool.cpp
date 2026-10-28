// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"


TEST_SUITE("Alignments: one stream") {
	TEST_CASE("equal_lookback_one_st") {
		CHECK_FALSE(sat_or_fail("(always o1[t-1] = 0) && (sometimes o1[t] = 1 && o1[t-1] = 0)."));
	}
	TEST_CASE("smaller_lookback_one_st") {
		CHECK(sat_or_fail("(always o1[t] = o1[t-1] && o1[t-1] = 1) && (sometimes o2[t] = 0)."));
	}
	TEST_CASE("greater_lookback_one_st") {
		CHECK_FALSE(sat_or_fail("(always o1[t] = o1[t-1]) && (sometimes o1[t] != o1[t-2])."));
	}
	TEST_CASE("equal_lookback_two_st") {
		tref spec = spec_or_fail("(always o1[t] = 0) && (sometimes o1[t] = 0) && (sometimes o1[t] = 1).");
		auto exec = transform_to_execution<node_t>(spec);
		REQUIRE(exec.has_value());
		CHECK(exec.value() == tau::_F());
	}
	TEST_CASE("greater_lookback_two_st_1") {
		CHECK_FALSE(sat_or_fail("(always o1[t] = 1 && o2[t] = 1) && (sometimes o1[t-1] = 1) && (sometimes o2[t-2] = 0)."));
	}
}

TEST_SUITE("Alignments: two streams") {
	TEST_CASE("greater_lookback_two_st_2") {
		CHECK_FALSE(sat_or_fail("(always o1[t] = 1 && o2[t] = 1) && (sometimes o1[t-1] = 0) && (sometimes o2[t-2] = 1)."));
	}
	TEST_CASE("smaller_lookback_two_st_1") {
		CHECK_FALSE(sat_or_fail("(always o1[t-2] = 0 && o2[t-2] = 0) && (sometimes o1[t] = 1) && (sometimes o1[t-1] = 0)."));
	}
	TEST_CASE("smaller_lookback_two_st_2") {
		CHECK_FALSE(sat_or_fail("(always o1[t-2] = 0 && o2[t-2] = 0) && (sometimes o1[t] = 0) && (sometimes o1[t-1] = 1)."));
	}
	TEST_CASE("mixed_lookback_two_st_1") {
		CHECK_FALSE(sat_or_fail("(always o1[t-2] = 1) && (sometimes o1[t-3] = 0) && (sometimes o1[t] = 1)."));
	}
	TEST_CASE("mixed_lookback_two_st_2") {
		CHECK_FALSE(sat_or_fail("(always o1[t-2] = 1) && (sometimes o1[t-3] = 1) && (sometimes o1[t] = 0)."));
	}
}

TEST_SUITE("Bool: input stream constraints") {
	TEST_CASE("this_stream_is_input_stream") {
		auto spec = spec_or_fail("this[t] < 1.");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(!sat.value());
	}
}
