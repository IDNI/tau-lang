// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"


TEST_SUITE("tau_ba: always") {
	TEST_CASE("constant T always sat") {
		CHECK(sat_or_fail("(always o1[t] = {T})."));
	}
	TEST_CASE("constant F always sat") {
		CHECK(sat_or_fail("(always o1[t] = {F})."));
	}
	TEST_CASE("loopback always sat") {
		CHECK(sat_or_fail("(always o1[t] = o1[t-1])."));
	}
	TEST_CASE("T and F conflict always unsat") {
		CHECK_FALSE(sat_or_fail("(always o1[t] = {T}) && (always o1[t] = {F})."));
	}
}

TEST_SUITE("tau_ba: sometimes") {
	TEST_CASE("constant T sometimes sat") {
		CHECK(sat_or_fail("(sometimes o1[t] = {T})."));
	}
	TEST_CASE("constant F sometimes sat") {
		CHECK(sat_or_fail("(sometimes o1[t] = {F})."));
	}
}

TEST_SUITE("tau_ba: always and sometimes") {
	TEST_CASE("always T and sometimes T sat") {
		CHECK(sat_or_fail("(always o1[t] = {T}) && (sometimes o1[t] = {T})."));
	}
	TEST_CASE("always T conflicts with sometimes F unsat") {
		CHECK_FALSE(sat_or_fail("(always o1[t] = {T}) && (sometimes o1[t] = {F})."));
	}
	TEST_CASE("multi-output sat") {
		CHECK(sat_or_fail("(always o1[t] = {<:z> = 0}) && (always o2[t] = {<:x> = 0}) && (always o3[t] = {<:y> = 0})."));
	}
}
