// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"

TEST_SUITE("Configuration") {
	TEST_CASE("logging") {
		//logging::trace();
	}

}

TEST_SUITE("Alignments y4") {
	// TODO (HIGH) fails in cli (returns T)
	TEST_CASE("greater_lookback_two_st_1") {  // <- takes too long
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

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
