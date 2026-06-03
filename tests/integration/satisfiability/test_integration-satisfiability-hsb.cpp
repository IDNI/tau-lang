// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"

TEST_SUITE("Configuration") {
	TEST_CASE("logging") {
		// logging::trace();
	}
}

TEST_SUITE("hsb: always") {
	TEST_CASE("top always sat") {
		tref spec = create_spec("(always o1[t]:hsb = {top}:hsb).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("top and bot always unsat") {
		// top != bot — stream cannot equal both simultaneously
		tref spec = create_spec("(always o1[t]:hsb = {top}:hsb) && (always o1[t]:hsb = {bot}:hsb).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("hsb: sometimes") {
	TEST_CASE("sometimes top sat") {
		tref spec = create_spec("(sometimes o1[t]:hsb = {top}:hsb).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("hsb: always and sometimes") {
	TEST_CASE("always top and sometimes top sat") {
		tref spec = create_spec("(always o1[t]:hsb = {top}:hsb) && (sometimes o1[t]:hsb = {top}:hsb).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("always top conflicts sometimes bot unsat") {
		tref spec = create_spec("(always o1[t]:hsb = {top}:hsb) && (sometimes o1[t]:hsb = {bot}:hsb).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("hsb: loopback") {
	TEST_CASE("loopback sat") {
		tref spec = create_spec("(always o1[t]:hsb = o1[t-1]:hsb).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
