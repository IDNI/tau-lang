// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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

	TEST_CASE("loop fallback T") {
		logging::trace();
		const char* sample =
			"g[n](x) := !g[n-1](x)."
			"g[0](x) := T."
			"g(x) fallback T.";
		CHECK( fp_test_T(sample) );
		logging::info();
	}

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

	TEST_CASE("cross-family type-blocked mutual recursion") {
		// Each family is internally consistent on its own (validate_rr_
		// case_types, 2a, passes both -- a single case per family has
		// nothing to disagree with): `a`'s one case requires its
		// argument :sbf, `b`'s requires :tau. A same-index mutual
		// reference here (`a[n](x):=b[n](x)`, `b[n](x):=a[n](x)`, no
		// "-1") would be rejected earlier by the PRE-EXISTING
		// is_well_founded cyclic check -- see "detect cycle indirect"
		// above, which is exactly that shape -- so this uses a
		// decreasing (well-founded) mutual reference instead: `a[n]`
		// hands off to `b[n-1]`, `b[n]` hands off to `a[n-1]`, well-
		// founded because a genuinely decreasing offset never adds an
		// edge to is_well_founded's same-index dependency graph (see
		// get_ref_info/is_well_founded above). Enumerating `a(x)`:
		// steps 0 and 1 dead-end on the shift itself (index 0 has
		// nowhere to decrement to, both typed and untyped -- the same
		// "no initial condition" shape above, legitimately
		// uninterpreted, not an error); step 2 reaches `a[2](x)` ->
		// `b[1](x)` where `b[n]`'s pattern DOES structurally cover the
		// position (its own decrement, 1-1=0, is valid) but x's type
		// (:sbf, from `a`) blocks `b[n]`'s (:tau) pattern -- exactly
		// the defect calculate_fixed_point's residual-ref guard (2b)
		// exists to catch, exercising its untyped-probe/attribution
		// path end to end (unlike the uncommitted Step-6 sanity check,
		// this is now committed coverage for that branch). The whole
		// binary is TIMEOUT-bounded at the ctest level
		// (tests/integration/CMakeLists.txt) so a regression that made
		// this (or the guard's own saturation cap, sized for a
		// genuinely diverging probe that this well-founded, naturally-
		// terminating shape does not exercise) hang would fail by
		// timeout rather than hanging ctest, mirroring how the REPL
		// tests bound their own pre-fix-hang reproducers.
		const char* sample =
			"a[n](x:sbf) := b[n-1](x)."
			"b[n](x:tau) := a[n-1](x)."
			"a(x).";
		CHECK( fp_test_fail(sample) );
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
