// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_bool_only_helpers.h"

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

	// A bf-only operator such as postfix `'` (the tau-BA complement) types
	// the whole recurrence relation as bf, so it is exercised directly
	// through calculate_fixed_point (as its own doc comment does) rather
	// than through a top-level ".tau" spec: a bare "g(x):tau fallback ..."
	// call site is itself always parsed as wff, which made every bf-typed
	// relation using `'` silently mismatch the call site (see the next
	// test case) instead of ever reaching this resolution path.
	TEST_CASE("postfix negation (bf) resolves to its fallback on a loop") {
		auto nso_rr = get_bf_nso_rr(
			"h[n](X):tau := h[n - 1](X)'."
			"h[0](X):tau := X.", "h(Y)").value();
		auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
		tref main = rr_captures.main->get();
		tref fp = calculate_fixed_point<node_t>(rr_captures, main, tau::bf, 1,
			tau::_0(tau_type_id<node_t>()));
		REQUIRE( fp != nullptr );
		CHECK( tau::get(fp).to_str() == "0" );
	}

	// Regression: nt disagreeing with the relation's own (bf) type used to
	// go undetected, so `current` (built/compared at nt) could never
	// structurally match any rule -- calculate_fixed_point spun silently
	// until MAX_FP_STEPS instead of failing fast. Same relation as above,
	// just asked for at the wrong nt.
	TEST_CASE("postfix negation (bf) rejected when call site type disagrees") {
		auto nso_rr = get_bf_nso_rr(
			"h[n](X):tau := h[n - 1](X)'."
			"h[0](X):tau := X.", "h(Y)").value();
		auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
		tref main = rr_captures.main->get();
		tref fp = calculate_fixed_point<node_t>(rr_captures, main, tau::wff,
			1, tau::_F());
		CHECK( fp == nullptr );
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
