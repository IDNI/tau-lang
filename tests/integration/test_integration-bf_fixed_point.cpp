// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "nso_rr.h"
#include "bool_ba.h"
#include "bdd_handle.h"
#include "normalizer.h"
#include "bdd_binding.h"

#include "test_integration_helpers-bdd.h"
#include "../unit/test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

bool test_bf_rr_fp(const char* rec, const char* sample, tau_parser::nonterminal nt,
	bool expect_fail = false)
{
	tau_parser::parse_options options;
	options.start = tau_parser::rec_relations;
	auto rec_src = make_tau_source(rec, options);
	auto rec_formula = make_nso_rr_using_factory<bdd_binding>(rec_src);
	if (!rec_formula.has_value()) return expect_fail;
	rec_formula = infer_ref_types(rec_formula.value());
	if (!rec_formula.has_value()) return expect_fail;
	options.start = tau_parser::bf;
	auto sample_src = make_tau_source(sample, options);
	auto formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
	if (!formula.has_value()) return expect_fail;
	auto sample_formula = formula.value();
	sample_formula.rec_relations = rec_formula.value().rec_relations;
	auto result = bf_normalizer_with_rec_relation<bdd_binding>(
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
		const char* rec = "f[n](x) := f[n-1](x).";
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
