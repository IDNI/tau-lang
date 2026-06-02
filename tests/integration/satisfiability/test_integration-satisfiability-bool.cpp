// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"

TEST_SUITE("Configuration") {
	TEST_CASE("logging") {
		// logging::trace();
	}
}

TEST_SUITE("Alignments: one stream") {
	TEST_CASE("equal_lookback_one_st") {
		tref spec = create_spec("(always o1[t-1] = 0) && (sometimes o1[t] = 1 && o1[t-1] = 0).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("smaller_lookback_one_st") {
		tref spec = create_spec("(always o1[t] = o1[t-1] && o1[t-1] = 1) && (sometimes o2[t] = 0).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("greater_lookback_one_st") {
		tref spec = create_spec("(always o1[t] = o1[t-1]) && (sometimes o1[t] != o1[t-2]).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("equal_lookback_two_st") {
		tref spec = create_spec("(always o1[t] = 0) && (sometimes o1[t] = 0) && (sometimes o1[t] = 1).");
		CHECK(transform_to_execution<node_t>(spec) == tau::_F());
	}
	TEST_CASE("greater_lookback_two_st_1") {
		tref spec = create_spec("(always o1[t] = 1 && o2[t] = 1) && (sometimes o1[t-1] = 1) && (sometimes o2[t-2] = 0).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Alignments: two streams") {
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

TEST_SUITE("Bool: input stream constraints") {
	TEST_CASE("this_stream_is_input_stream") {
		bdd_init<Bool>();
		auto spec = create_spec("this[t] < 1.");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
