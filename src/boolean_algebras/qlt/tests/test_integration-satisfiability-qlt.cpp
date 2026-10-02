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
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(sat.value());
	}
	TEST_CASE("constant equality sat") {
		tref spec = create_spec("(always o1[t]:qlt = {1/2}:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(sat.value());
	}
	TEST_CASE("contradictory bounds unsat") {
		tref spec = create_spec("(always o1[t]:qlt > {3/4}:qlt) && (always o1[t]:qlt < {1/4}:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(!sat.value());
	}
	TEST_CASE("contradictory equalities unsat") {
		tref spec = create_spec("(always o1[t]:qlt = {1/3}:qlt) && (always o1[t]:qlt = {2/3}:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(!sat.value());
	}
}

// < is strict inclusion: a set lies strictly between {1/4} and [1/4,3/4], none
// between two points.
TEST_SUITE("qlt: interval constraints") {
	TEST_CASE("between a point and an interval sat") {
		tref spec = create_spec("(always o1[t]:qlt > {1/4}:qlt && o1[t]:qlt < {[1/4,3/4]}:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(sat.value());
	}
	TEST_CASE("between two points unsat") {
		tref spec = create_spec("(always o1[t]:qlt > {1/4}:qlt && o1[t]:qlt < {3/4}:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(!sat.value());
	}
	TEST_CASE("between a point and a negative interval sat") {
		tref spec = create_spec("(always o1[t]:qlt > {-1}:qlt && o1[t]:qlt < {[-1,0)}:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(sat.value());
	}
	TEST_CASE("degenerate open interval unsat") {
		tref spec = create_spec("(always o1[t]:qlt > {1/2}:qlt && o1[t]:qlt < {1/2}:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(!sat.value());
	}
}

TEST_SUITE("qlt: sometimes") {
	TEST_CASE("sometimes positive sat") {
		tref spec = create_spec("(sometimes o1[t]:qlt > {0}:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(sat.value());
	}
}

TEST_SUITE("qlt: always and sometimes") {
	TEST_CASE("always above a point sometimes below an interval sat") {
		tref spec = create_spec("(always o1[t]:qlt > {0}:qlt) && (sometimes o1[t]:qlt < {[0,1]}:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(sat.value());
	}
	TEST_CASE("always above a point sometimes below another unsat") {
		tref spec = create_spec("(always o1[t]:qlt > {0}:qlt) && (sometimes o1[t]:qlt < {1}:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(!sat.value());
	}
	TEST_CASE("always half conflicts sometimes above three-quarter unsat") {
		tref spec = create_spec("(always o1[t]:qlt = {1/2}:qlt) && (sometimes o1[t]:qlt > {3/4}:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(!sat.value());
	}
}

TEST_SUITE("qlt: loopback") {
	TEST_CASE("strictly increasing sat") {
		// {0}, {0, 1}, {0, 1, 2}, ... increases strictly forever
		tref spec = create_spec("(always o1[t]:qlt > o1[t-1]:qlt).");
		auto sat = is_tau_formula_sat<node_t>(spec);
		REQUIRE(sat.has_value());
		CHECK(sat.value());
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
