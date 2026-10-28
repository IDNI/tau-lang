// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"


TEST_SUITE("qint: always") {
	TEST_CASE("half-open interval sat") {
		CHECK(sat_or_fail("(always o1[t]:qint = {[0, 1)}:qint)."));
	}
	TEST_CASE("fractional interval sat") {
		CHECK(sat_or_fail("(always o1[t]:qint = {[1/4, 3/4)}:qint)."));
	}
	TEST_CASE("union of intervals sat") {
		CHECK(sat_or_fail("(always o1[t]:qint = {[-1, 0) | [1, 2)}:qint)."));
	}
	TEST_CASE("disjoint intervals always unsat") {
		// [0, 1/2) and [1/2, 1) are disjoint in qint — no element equals both
		CHECK_FALSE(sat_or_fail("(always o1[t]:qint = {[0, 1/2)}:qint) && (always o1[t]:qint = {[1/2, 1)}:qint)."));
	}
}

TEST_SUITE("qint: sometimes") {
	TEST_CASE("interval sometimes sat") {
		CHECK(sat_or_fail("(sometimes o1[t]:qint = {[0, 1)}:qint)."));
	}
}

TEST_SUITE("qint: always and sometimes") {
	TEST_CASE("consistent always and sometimes sat") {
		CHECK(sat_or_fail("(always o1[t]:qint = {[0, 1)}:qint) && (sometimes o1[t]:qint = {[0, 1)}:qint)."));
	}
	TEST_CASE("always half-unit conflicts sometimes different interval unsat") {
		// always [0,1/2) but sometimes [1/4,3/4): different elements — unsat
		CHECK_FALSE(sat_or_fail("(always o1[t]:qint = {[0, 1/2)}:qint) && (sometimes o1[t]:qint = {[1/4, 3/4)}:qint)."));
	}
}

TEST_SUITE("qint: loopback") {
	TEST_CASE("loopback constant sat") {
		CHECK(sat_or_fail("(always o1[t]:qint = o1[t-1]:qint)."));
	}
}
