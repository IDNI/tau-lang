// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_integration-satisfiability_helper.h"

TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}

	TEST_CASE("bv size") {
		default_bv_size = 4;
	}
}

TEST_SUITE("Mixed Boolean algebras specs") {

	TEST_CASE("simple always: no loopback") {
		tref spec = create_spec("(always o1[t]:bv =_ { 1 } && o2[t] = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("simple always: loopback in tau") {
		tref spec = create_spec("(always o1[t]:bv =_ { 1 } && o2[t-2] = o2[t]).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("simple always: loopback in bv") {
		tref spec = create_spec("(always o1[t]:bv =_ o1[t-2] && o2[t] = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("simple always: loopback in bv and tau") {
		tref spec = create_spec("(always o1[t]:bv =_ o1[t-2] && o2[t] = o2[t]).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("simple always: constant position") {
		tref spec = create_spec("(always o1[0]:bv =_ { 1 } && o2[0] = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("simple sometimes: no loopback") {
		tref spec = create_spec("(sometimes o1[t]:bv =_ { 0 } || o2[t] = 0).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("simple sometimes: loopback in tau") {
		tref spec = create_spec("(sometimes o1[t]:bv =_ { 0 } || o2[t-2] = o2[t]).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("simple sometimes: loopback in bv") {
		tref spec = create_spec("(sometimes o1[t]:bv =_ o1[t-2] || o2[t] = 0).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("simple sometimes: loopback in bv and tau") {
		tref spec = create_spec("(sometimes o1[t]:bv =_ o1[t-2] || o2[t] = o2[t]).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("mixing always and sometimes: sat") {
		tref spec = create_spec("(always o1[t]:bv =_ o1[t-1] && o2[t] = o2[t-1]) || (sometimes o1[t]:bv =_ { 1 } && o2[t] = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}

	TEST_CASE("mixing always and sometimes: unsat") {
		tref spec = create_spec("(always o1[t]:bv =_ { 1 } && o2[t] = 1) && (sometimes o1[t-1]:bv =_ { 0 } || o2[t-1] = 0).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
