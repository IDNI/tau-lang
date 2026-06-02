// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"

TEST_SUITE("Configuration") {
	TEST_CASE("logging") {
		// logging::trace();
	}
}

TEST_SUITE("tau_ba: always") {
	TEST_CASE("constant T always sat") {
		tref spec = create_spec("(always o1[t] = {T}).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("constant F always sat") {
		tref spec = create_spec("(always o1[t] = {F}).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("loopback always sat") {
		tref spec = create_spec("(always o1[t] = o1[t-1]).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("T and F conflict always unsat") {
		tref spec = create_spec("(always o1[t] = {T}) && (always o1[t] = {F}).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("tau_ba: sometimes") {
	TEST_CASE("constant T sometimes sat") {
		tref spec = create_spec("(sometimes o1[t] = {T}).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("constant F sometimes sat") {
		tref spec = create_spec("(sometimes o1[t] = {F}).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("tau_ba: always and sometimes") {
	TEST_CASE("always T and sometimes T sat") {
		tref spec = create_spec("(always o1[t] = {T}) && (sometimes o1[t] = {T}).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("always T conflicts with sometimes F unsat") {
		tref spec = create_spec("(always o1[t] = {T}) && (sometimes o1[t] = {F}).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("multi-output sat") {
		tref spec = create_spec("(always o1[t] = {<:z> = 0}) && (always o2[t] = {<:x> = 0}) && (always o3[t] = {<:y> = 0}).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
