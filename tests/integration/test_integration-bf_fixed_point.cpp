// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

bool test_bf_rr_fp(const char* rec, const char* sample,
	typename node_t::type nt, bool expect_fail = false)
{
	auto nso_rr = get_bf_nso_rr(rec, sample);
	if (!nso_rr.has_value()) return expect_fail;

	tref result = bf_normalizer_with_rec_relation<node_t>(nso_rr.value());
	if (!result) return expect_fail;

	return tau::get(result).child_is(nt) != expect_fail;
}

bool test_bf_rr_fp_1(const char* rec, const char* sample) {
	return test_bf_rr_fp(rec, sample, tau::bf_t);
}

bool test_bf_rr_fp_0(const char* rec, const char* sample) {
	return test_bf_rr_fp(rec, sample, tau::bf_f);
}

bool test_bf_rr_fp_expect_fail(const char* rec, const char* sample,
	typename node_t::type nt)
{
	return test_bf_rr_fp(rec, sample, nt, true);
}

TEST_SUITE("Boolean function recurrence relation fixed point calculation") {

	TEST_CASE("loop fallback 1") {
		logging::trace();
		const char* rec=
			"g[n](x):tau := g[n-1](x)'."
			"g[0](x):tau := 1.";
		const char* sample = "g(x):tau fallback 1";
		CHECK( test_bf_rr_fp_1(rec, sample) );
		logging::info();
	}

	TEST_CASE("loop default fallback (0)") {
		const char* rec=
			"g[n](x):tau := g[n-1](x)'."
			"g[0](x):tau := 1.";
		const char* sample = "g(0):tau";
		CHECK( test_bf_rr_fp_0(rec, sample) );
	}

	TEST_CASE("loop fallback last") {
		const char* rec=
			"g[n](x):tau := g[n-1](x)'."
			"g[0](x):tau := 1.";
		const char* sample = "g(x):tau fallback last";
		CHECK( test_bf_rr_fp_0(rec, sample) );
	}

	TEST_CASE("loop fallback first") {
		const char* rec=
			"g[n](x):tau := g[n-1](x)'."
			"g[0](x):tau := 1.";
		const char* sample = "g(x):tau fallback first";
		CHECK( test_bf_rr_fp_1(rec, sample) );
	}

	TEST_CASE("referring itself") {
		const char* rec=
			"f[n](x):tau := f[n-1](x) & x."
			"f[0](x):tau := 1.";
		const char* sample = "f(x):tau";
		CHECK( test_bf_rr_fp(rec, sample, tau::variable) );
	}

	TEST_CASE("multiple") {
		const char* rec="f[0](x):tau := 1."
			"g[0](x):tau := 0."
			"f[n](x):tau := f[n-1](x) | g[n](x)."
			"g[n](x):tau := g[n-1](x)'.";
		const char* sample = "f(x):tau";
		CHECK( test_bf_rr_fp_1(rec, sample) );
	}

	TEST_CASE("no initial condition") {
		const char* rec =
			"f[n](x):tau := f[n-1](x) & 1.";
		const char* sample = "f(x):tau";
		CHECK( test_bf_rr_fp(rec, sample, tau::bf_or) );
	}

	TEST_CASE("with initial conditions") {
		const char* rec=
			"f[0](x):tau := 0."
			"f[2](x):tau := 0."
			"f[4](x):tau := 0."
			"f[8](x):tau := 0."
			"f[n](x):tau := 1.";
		const char* sample = "f(x):tau";
		CHECK( test_bf_rr_fp_1(rec, sample) );
	}
}

TEST_SUITE("Boolean function recurrence relation well foundedness") {

	TEST_CASE("detect cycle direct") {
		const char* rec=
			"f[0](x):tau := 1."
			"f[n](x):tau := f[n](x).";
		const char* sample = "f(x):tau";
		CHECK( test_bf_rr_fp_expect_fail(rec, sample, tau::bf_f) );
	}

	TEST_CASE("no rule") {
		const char* rec=
			"g[0](Y):tau := 1."
			"g[1](Y):tau := 1.";
		const char* sample = "g(Y):tau";
		CHECK( test_bf_rr_fp_expect_fail(rec, sample, tau::bf_f) );
	}

	TEST_CASE("fallback type mismatch") {
		const char* rec =
			"g[n](Y):tau := 1.";
		const char* sample = "g(Y):tau fallback T";
		CHECK( test_bf_rr_fp_expect_fail(rec, sample, tau::bf_f) );
	}
}
