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

TEST_SUITE("qlt: always") {
	TEST_CASE("positive always sat") {
		tref spec = create_spec("(always o1[t]:qlt > {0}:qlt).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("constant equality sat") {
		tref spec = create_spec("(always o1[t]:qlt = {1/2}:qlt).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("contradictory bounds unsat") {
		tref spec = create_spec("(always o1[t]:qlt > {3/4}:qlt) && (always o1[t]:qlt < {1/4}:qlt).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("contradictory equalities unsat") {
		tref spec = create_spec("(always o1[t]:qlt = {1/3}:qlt) && (always o1[t]:qlt = {2/3}:qlt).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("qlt: interval constraints") {
	TEST_CASE("open interval sat") {
		tref spec = create_spec("(always o1[t]:qlt > {1/4}:qlt && o1[t]:qlt < {3/4}:qlt).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("negative open interval sat") {
		tref spec = create_spec("(always o1[t]:qlt > {-1}:qlt && o1[t]:qlt < {0}:qlt).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("degenerate open interval unsat") {
		// (x > 1/2) && (x < 1/2) is empty in DLO
		tref spec = create_spec("(always o1[t]:qlt > {1/2}:qlt && o1[t]:qlt < {1/2}:qlt).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("qlt: sometimes") {
	TEST_CASE("sometimes positive sat") {
		tref spec = create_spec("(sometimes o1[t]:qlt > {0}:qlt).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("qlt: always and sometimes") {
	TEST_CASE("always positive sometimes below one sat") {
		// can always be 1/2, which is > 0 and < 1
		tref spec = create_spec("(always o1[t]:qlt > {0}:qlt) && (sometimes o1[t]:qlt < {1}:qlt).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("always half conflicts sometimes above three-quarter unsat") {
		// always = 1/2, but 1/2 is not > 3/4
		tref spec = create_spec("(always o1[t]:qlt = {1/2}:qlt) && (sometimes o1[t]:qlt > {3/4}:qlt).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("qlt: loopback") {
	TEST_CASE("strictly increasing sat") {
		// In DLO, an infinite strictly increasing sequence exists
		tref spec = create_spec("(always o1[t]:qlt > o1[t-1]:qlt).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
