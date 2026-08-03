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

TEST_SUITE("sbf: always") {
	TEST_CASE("constant zero always sat") {
		tref spec = create_spec("(always o1[t]:sbf = 0).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("constant one always sat") {
		tref spec = create_spec("(always o1[t]:sbf = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("loopback always sat") {
		tref spec = create_spec("(always o1[t]:sbf = o1[t-1]:sbf).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("contradictory constants always unsat") {
		tref spec = create_spec("(always o1[t]:sbf = 0) && (always o1[t]:sbf = 1).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("sbf: sometimes") {
	TEST_CASE("constant zero sometimes sat") {
		tref spec = create_spec("(sometimes o1[t]:sbf = 0).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("constant one sometimes sat") {
		tref spec = create_spec("(sometimes o1[t]:sbf = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("sbf: always and sometimes") {
	TEST_CASE("always zero and sometimes zero sat") {
		tref spec = create_spec("(always o1[t]:sbf = 0) && (sometimes o1[t]:sbf = 0).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("always one conflicts with sometimes zero unsat") {
		tref spec = create_spec("(always o1[t]:sbf = 1) && (sometimes o1[t]:sbf = 0).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("loopback and constant always unsat") {
		tref spec = create_spec("(always o1[t]:sbf = o1[t-1]:sbf) && (always o1[t-1]:sbf = 1) && (sometimes o1[t]:sbf = 0).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
