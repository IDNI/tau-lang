// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"


TEST_SUITE("hsb: always") {
	TEST_CASE("top always sat") {
		CHECK(sat_or_fail("(always o1[t]:hsb = {top}:hsb)."));
	}
	TEST_CASE("top and bot always unsat") {
		// top != bot — stream cannot equal both simultaneously
		CHECK_FALSE(sat_or_fail("(always o1[t]:hsb = {top}:hsb) && (always o1[t]:hsb = {bot}:hsb)."));
	}
}

TEST_SUITE("hsb: sometimes") {
	TEST_CASE("sometimes top sat") {
		CHECK(sat_or_fail("(sometimes o1[t]:hsb = {top}:hsb)."));
	}
}

TEST_SUITE("hsb: always and sometimes") {
	TEST_CASE("always top and sometimes top sat") {
		CHECK(sat_or_fail("(always o1[t]:hsb = {top}:hsb) && (sometimes o1[t]:hsb = {top}:hsb)."));
	}
	TEST_CASE("always top conflicts sometimes bot unsat") {
		CHECK_FALSE(sat_or_fail("(always o1[t]:hsb = {top}:hsb) && (sometimes o1[t]:hsb = {bot}:hsb)."));
	}
}

TEST_SUITE("hsb: loopback") {
	TEST_CASE("loopback sat") {
		CHECK(sat_or_fail("(always o1[t]:hsb = o1[t-1]:hsb)."));
	}
}
