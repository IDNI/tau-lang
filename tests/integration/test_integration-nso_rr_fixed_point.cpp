// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

bool fp_test(const char* sample, typename node_t::type nt, bool expect_fail = false) {
	return normalize_and_check(sample, nt, expect_fail);
}

bool fp_test_F(const char* sample) {
	return fp_test(sample, tau::wff_f);
}

bool fp_test_T(const char* sample) {
	return fp_test(sample, tau::wff_t);
}

bool fp_test_fail(const char* sample, typename node_t::type nt) {
	return fp_test(sample, nt, true);
}

bool fp_test_fail(const char* sample) {
	return fp_test_fail(sample, tau::wff_f);
}

TEST_SUITE("rec relations fixed point") {

	TEST_CASE("loop default fallback (F)") {
		const char* sample =
			"g[n](x) := !g[n-1](x)."
			"g[0](x) := T."
			"g(x).";
		CHECK( fp_test_F(sample) );
	}

	// TODO (HIGH) fix this test case
	/*TEST_CASE("loop fallback T") {
		const char* sample =
			"g[n](x) := !g[n-1](x)."
			"g[0](x) := T."
			"g(x) fallback T.";
		CHECK( fp_test_T(sample) );
	}*/

	TEST_CASE("loop fallback last") {
		const char* sample =
			"g[n](x) := !g[n-1](x)."
			"g[0](x) := T."
			"g(x) fallback last.";
		CHECK( fp_test_F(sample) );
	}

	TEST_CASE("loop fallback first") {
		const char* sample =
			"g[n](x) := !g[n-1](x)."
			"g[0](x) := T."
			"g(x) fallback first.";
		CHECK( fp_test_T(sample) );
	}

	TEST_CASE("referring itself") {
		const char* sample =
			"f[n](x) := f[n-1](x) && x = 1."
			"f[0](x) := T."
			"f(x).";
		CHECK( fp_test(sample, tau::bf_eq) );
	}

	TEST_CASE("multiple") {
		const char* sample =
			"f[0](x) := T."
			"g[0](x) := F."
			"f[n](x) := f[n-1](x) || g[n](x)."
			"g[n](x) := !g[n-1](x)."
			"f(x).";
		CHECK( fp_test_T(sample) );
	}

	TEST_CASE("no initial condition") {
		const char* sample =
			"f[n](x) := f[n-1](x) && T."
			"f(x).";
		CHECK( fp_test(sample, tau::wff_ref) );
	}

	TEST_CASE("with initial conditions") {
		const char* sample =
			"f[0](x) := F."
			"f[2](x) := F."
			"f[4](x) := F."
			"f[8](x) := F."
			"f[n](x) := T."
			"f(x).";
		CHECK( fp_test_T(sample) );
	}
}

TEST_SUITE("rec relations well foundedness") {

	TEST_CASE("shift in header") {
		const char* sample =
			"f[n-1](x) := f[n-2](x)."
			"f(x).";
		CHECK( fp_test_fail(sample) );
	}

	TEST_CASE("left fixed, right relative") {
		const char* sample =
			"f[0](x) := f[n](x)."
			"f(x).";
		CHECK( fp_test_fail(sample) );
	}

	TEST_CASE("left < right") {
		const char* sample =
			"f[1](x) := f[2](x)."
			"f(x).";
		CHECK( fp_test_fail(sample) );
	}

	TEST_CASE("detect cycle direct") {
		const char* sample =
			"f[0](x) := T."
			"f[n](x) := f[n](x)."
			"f(x).";
		CHECK( fp_test_fail(sample) );
	}

	TEST_CASE("detect cycle indirect") {
		const char* sample =
			"f[n](x) := g[n](x)."
			"g[n](x) := f[n](x)."
			"g(x).";
		CHECK( fp_test_fail(sample) );
	}

	TEST_CASE("no rule") {
		const char* sample =
			"g[0](Y) := T."
			"g[1](Y) := T."
			"g(Y).";
		CHECK( fp_test_fail(sample) );
	}

	// This test fails as type inference fails (see test case
	// "nso_rr_fixed_point/fallback type mismatch" in 
	// test_integration-ba_types_inference.cpp). however there is no simple
	// way to catch failed assertions in doctest...so we commented out until
	// that is possible.
	/*TEST_CASE("fallback type mismatch") {
		logging::trace();
		const char* sample =
			"g[n](Y) := T."
			"g(Y) fallback 1.";
		CHECK_THROWS( fp_test_fail(sample) );
		logging::trace();
	}*/
}
