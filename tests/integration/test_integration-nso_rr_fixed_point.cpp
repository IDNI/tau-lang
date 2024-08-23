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

#include "test_integration_helpers-bdd.h"
#include "../unit/test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

bdd_test_factory bf;

bool fp_test(const char* sample, const size_t& nt) {
	auto sample_src = make_tau_source(sample);
	auto sample_formula = make_nso_rr_using_factory<
		bdd_test_factory_t, bdd_test>(sample_src, bf);
	return (normalizer<bdd_test>(sample_formula) | nt).has_value();
}

bool fp_test_wff_f(const char* sample) {
	return fp_test(sample, tau_parser::wff_f);
}

TEST_SUITE("rec relations fixed point") {

	TEST_CASE("loop fallbacks to F for now") {
		const char* sample =
			"g[n]($x) := !g[n-1]($x)."
			"g[0]($x) := T."
			"g(x).";
		CHECK( fp_test_wff_f(sample) );
	}

	TEST_CASE("referring itself") {
		const char* sample =
			"f[n]($x) := f[n-1]($x) && $x = 1."
			"f[0](x)  := T."
			"f(x).";
		CHECK( fp_test(sample, tau_parser::bf_eq) );
	}

	TEST_CASE("multiple") {
		const char* sample =
			"f[0]($x) := T."
			"g[0]($x) := F."
			"f[n]($x) := f[n-1]($x) || g[n]($x)."
			"g[n]($x) := !g[n-1]($x)."
			"f(x).";
		CHECK( fp_test(sample, tau_parser::wff_t) );
	}

	TEST_CASE("no initial condition") {
		const char* sample =
			"f[n]($x) := f[n-1]($x)."
			"f(x).";
		CHECK( fp_test(sample, tau_parser::wff_ref) );
	}

	TEST_CASE("with initial conditions") {
		const char* sample =
			"f[0]($x) := F."
			"f[2]($x) := F."
			"f[4]($x) := F."
			"f[8]($x) := F."
			"f[n]($x) := T."
			"f(x).";
		CHECK( fp_test(sample, tau_parser::wff_t) );
	}
}

TEST_SUITE("rec relations well foundedness") {

	TEST_CASE("shift in header") {
		const char* sample =
			"f[n-1](x) := f[n-2](x)."
			"f(x).";
		CHECK( fp_test_wff_f(sample) );
	}

	TEST_CASE("left fixed, right relative") {
		const char* sample =
			"f[0](x) := f[n](x)."
			"f(x).";
		CHECK( fp_test_wff_f(sample) );
	}

	TEST_CASE("left < right") {
		const char* sample =
			"f[1](x) := f[2](x)."
			"f(x).";
		CHECK( fp_test_wff_f(sample) );
	}

	TEST_CASE("detect cycle direct") {
		const char* sample =
			"f[n](x) := f[n](x)."
			"f(x).";
		CHECK( fp_test_wff_f(sample) );
	}

	TEST_CASE("detect cycle indirect") {
		const char* sample =
			"f[n](x) := g[n](x)."
			"g[n](x) := f[n](x)."
			"g(x).";
		CHECK( fp_test_wff_f(sample) );
	}

	TEST_CASE("no rule") {
		const char* sample =
			"g[0]($Y) := T."
			"g[1]($Y) := T."
			"g(Y).";
		CHECK( fp_test_wff_f(sample) );
	}

}
