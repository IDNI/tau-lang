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

TEST_SUITE("qint: always") {
	TEST_CASE("half-open interval sat") {
		tref spec = create_spec("(always o1[t]:qint = {[0, 1)}:qint).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("fractional interval sat") {
		tref spec = create_spec("(always o1[t]:qint = {[1/4, 3/4)}:qint).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("union of intervals sat") {
		tref spec = create_spec("(always o1[t]:qint = {[-1, 0) | [1, 2)}:qint).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("disjoint intervals always unsat") {
		// [0, 1/2) and [1/2, 1) are disjoint in qint — no element equals both
		tref spec = create_spec("(always o1[t]:qint = {[0, 1/2)}:qint) && (always o1[t]:qint = {[1/2, 1)}:qint).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("qint: sometimes") {
	TEST_CASE("interval sometimes sat") {
		tref spec = create_spec("(sometimes o1[t]:qint = {[0, 1)}:qint).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("qint: always and sometimes") {
	TEST_CASE("consistent always and sometimes sat") {
		tref spec = create_spec("(always o1[t]:qint = {[0, 1)}:qint) && (sometimes o1[t]:qint = {[0, 1)}:qint).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("always half-unit conflicts sometimes different interval unsat") {
		// always [0,1/2) but sometimes [1/4,3/4): different elements — unsat
		tref spec = create_spec("(always o1[t]:qint = {[0, 1/2)}:qint) && (sometimes o1[t]:qint = {[1/4, 3/4)}:qint).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("qint: loopback") {
	TEST_CASE("loopback constant sat") {
		tref spec = create_spec("(always o1[t]:qint = o1[t-1]:qint).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
