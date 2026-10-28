// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"


TEST_SUITE("sbf: always") {
	TEST_CASE("constant zero always sat") {
		CHECK(sat_or_fail("(always o1[t]:sbf = 0)."));
	}
	TEST_CASE("constant one always sat") {
		CHECK(sat_or_fail("(always o1[t]:sbf = 1)."));
	}
	TEST_CASE("loopback always sat") {
		CHECK(sat_or_fail("(always o1[t]:sbf = o1[t-1]:sbf)."));
	}
	TEST_CASE("contradictory constants always unsat") {
		CHECK_FALSE(sat_or_fail("(always o1[t]:sbf = 0) && (always o1[t]:sbf = 1)."));
	}
}

TEST_SUITE("sbf: sometimes") {
	TEST_CASE("constant zero sometimes sat") {
		CHECK(sat_or_fail("(sometimes o1[t]:sbf = 0)."));
	}
	TEST_CASE("constant one sometimes sat") {
		CHECK(sat_or_fail("(sometimes o1[t]:sbf = 1)."));
	}
}

TEST_SUITE("sbf: always and sometimes") {
	TEST_CASE("always zero and sometimes zero sat") {
		CHECK(sat_or_fail("(always o1[t]:sbf = 0) && (sometimes o1[t]:sbf = 0)."));
	}
	TEST_CASE("always one conflicts with sometimes zero unsat") {
		CHECK_FALSE(sat_or_fail("(always o1[t]:sbf = 1) && (sometimes o1[t]:sbf = 0)."));
	}
	TEST_CASE("loopback and constant always unsat") {
		CHECK_FALSE(sat_or_fail("(always o1[t]:sbf = o1[t-1]:sbf) && (always o1[t-1]:sbf = 1) && (sometimes o1[t]:sbf = 0)."));
	}
}
