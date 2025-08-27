// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_integration-satisfiability_helper.h"

TEST_SUITE("Alignments y3") {
	TEST_CASE("equal_lookback_one_st") {
		tref spec = create_spec("(always o1[t-1]:bv =_ 0) && (sometimes o1[t]:bv =_ 1 && o1[t-1]:bv =_ 0).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("smaller_lookback_one_st") {
		tref spec = create_spec("(always o1[t]:bv =_ o1[t-1]:bv&& o1[t-1]:bv =_ 1) && (sometimes o2[t]:bv =_ 0).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("greater_lookback_one_st") {
		tref spec = create_spec("(always o1[t]:bv =_ o1[t-1]:bv) && (sometimes o1[t]:bv !=_ o1[t-2]:bv).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("equal_lookback_two_st") {
		tref spec = create_spec("(always o1[t]:bv =_ 0) && (sometimes o1[t]:bv =_ 0) && (sometimes o1[t]:bv =_ 1).");
		CHECK(transform_to_execution<node_t>(spec) == tau::_F());
	}
}
