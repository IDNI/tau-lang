// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "test_integration_helpers.h"
#include "../unit/test_helpers_depreciating.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

bool test_bf_rr_fp(const char* rec, const char* sample, tau_parser::nonterminal nt,
	bool expect_fail = false)
{
	tau_parser::parse_options options;
	options.start = tau_parser::rec_relations;
	auto rec_src = make_tau_source(rec, options);
	auto rec_formula = make_nso_rr_using_factory<sbf_ba>(rec_src);
	if (!rec_formula.has_value()) return expect_fail;
	rec_formula = infer_ref_types(rec_formula.value());
	if (!rec_formula.has_value()) return expect_fail;
	options.start = tau_parser::bf;
	auto sample_src = make_tau_source(sample, options);
	auto formula = make_nso_rr_using_factory<sbf_ba>(sample_src);
	if (!formula.has_value()) return expect_fail;
	auto sample_formula = formula.value();
	sample_formula.rec_relations = rec_formula.value().rec_relations;
	auto result = bf_normalizer_with_rec_relation<sbf_ba>(
								sample_formula);
	if (!result) return expect_fail;
	auto check = result | nt;
	if (!check) return expect_fail;
	return expect_fail ? !check.has_value() : check.has_value();
}

bool test_bf_rr_fp_1(const char* rec, const char* sample) {
	return test_bf_rr_fp(rec, sample, tau_parser::bf_t);
}

bool test_bf_rr_fp_0(const char* rec, const char* sample) {
	return test_bf_rr_fp(rec, sample, tau_parser::bf_f);
}

bool test_bf_rr_fp_expect_fail(const char* rec, const char* sample,
	tau_parser::nonterminal nt)
{
	return test_bf_rr_fp(rec, sample, nt, true);
}

TEST_SUITE("Boolean function recurrence relation fixed point calculation") {

	TEST_CASE("loop default fallback (0)") {
		const char* rec="g[n](x) := g[n-1](x)'."
				"g[0](x) := 1.";
		const char* sample = "g(0)";
		CHECK( test_bf_rr_fp_0(rec, sample) );
	}

	TEST_CASE("loop fallback 1") {
		const char* rec="g[n](x) := g[n-1](x)'."
				"g[0](x) := 1.";
		const char* sample = "g(x) fallback 1";
		CHECK( test_bf_rr_fp_1(rec, sample) );
	}

	TEST_CASE("loop fallback last") {
		const char* rec="g[n](x) := g[n-1](x)'."
				"g[0](x) := 1.";
		const char* sample = "g(x) fallback last";
		CHECK( test_bf_rr_fp_0(rec, sample) );
	}

	TEST_CASE("loop fallback first") {
		const char* rec="g[n](x) := g[n-1](x)'."
				"g[0](x) := 1.";
		const char* sample = "g(x) fallback first";
		CHECK( test_bf_rr_fp_1(rec, sample) );
	}

	TEST_CASE("referring itself") {
		const char* rec="f[n](x) := f[n-1](x) & x."
				"f[0](x) := 1.";
		const char* sample = "f(x)";
		CHECK( test_bf_rr_fp(rec, sample, tau_parser::variable) );
	}

	TEST_CASE("multiple") {
		const char* rec="f[0](x) := 1."
				"g[0](x) := 0."
				"f[n](x) := f[n-1](x) | g[n](x)."
				"g[n](x) := g[n-1](x)'.";
		const char* sample = "f(x)";
		CHECK( test_bf_rr_fp_1(rec, sample) );
	}

	TEST_CASE("no initial condition") {
		const char* rec = "f[n](x) := f[n-1](x) & 1.";
		const char* sample = "f(x)";
		CHECK( test_bf_rr_fp(rec, sample, tau_parser::bf_or) );
	}

	TEST_CASE("with initial conditions") {
		const char* rec="f[0](x) := 0."
				"f[2](x) := 0."
				"f[4](x) := 0."
				"f[8](x) := 0."
				"f[n](x) := 1.";
		const char* sample = "f(x)";
		CHECK( test_bf_rr_fp_1(rec, sample) );
	}
}

TEST_SUITE("Boolean function recurrence relation well foundedness") {

	TEST_CASE("detect cycle direct") {
		const char* rec="f[0](x) := 1."
				"f[n](x) := f[n](x).";
		const char* sample = "f(x)";
		CHECK( test_bf_rr_fp_expect_fail(rec, sample, tau_parser::bf_f) );
	}

	TEST_CASE("no rule") {
		const char* rec="g[0](Y) := 1."
				"g[1](Y) := 1.";
		const char* sample = "g(Y)";
		CHECK( test_bf_rr_fp_expect_fail(rec, sample, tau_parser::bf_f) );
	}

	TEST_CASE("fallback type mismatch") {
		const char* rec = "g[n](Y) := 1.";
		const char* sample = "g(Y) fallback T";
		CHECK( test_bf_rr_fp_expect_fail(rec, sample, tau_parser::bf_f) );
	}

}
