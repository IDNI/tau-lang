// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_integration-satisfiability_helper.h"

TEST_SUITE("Alignments y2") {
	TEST_CASE("greater_lookback_two_st_2") {
		tref spec = create_spec("(always o1[t] = 1 && o2[t] = 1) && (sometimes o1[t-1] = 0) && (sometimes o2[t-2] = 1).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("smaller_lookback_two_st_1") {
		tref spec = create_spec("(always o1[t-2] = 0 && o2[t-2] = 0) && (sometimes o1[t] = 1) && (sometimes o1[t-1] = 0).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("smaller_lookback_two_st_2") {
		tref spec = create_spec("(always o1[t-2] = 0 && o2[t-2] = 0) && (sometimes o1[t] = 0) && (sometimes o1[t-1] = 1).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("mixed_lookback_two_st_1") {
		tref spec = create_spec("(always o1[t-2] = 1) && (sometimes o1[t-3] = 0) && (sometimes o1[t] = 1).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("mixed_lookback_two_st_2") {
		tref spec = create_spec("(always o1[t-2] = 1) && (sometimes o1[t-3] = 1) && (sometimes o1[t] = 0).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}
