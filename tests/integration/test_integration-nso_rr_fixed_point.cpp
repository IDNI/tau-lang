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

TEST_SUITE("fixed point") {

	TEST_CASE("wff_rec_relation simple") {
		const char* sample =
			"g[0]($Y) := T."
			"g[1]($Y) := T."
			"g(Y).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("wff_rec_relation loop returns F for now") {
		const char* sample =
			"g[$n]($x) := !g[$n-1]($x)."
			"g[0]($x)  := T."
			"g(x).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("wff_rec_relation referring itself") {
		const char* sample =
			"f[$n]($x) := f[$n-1]($x) && $x = 1."
			"f[0](x)   := T."
			"f(x).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_always;
		CHECK( check.has_value() );
	}

	TEST_CASE("wff_rec_relation multiple") {
		const char* sample =
			"g[$n]($x) := !g[$n-1]($x)."
			"g[0]($x)  := T."
			"f[$n]($x) := f[$n-1]($x) || g[$n]($x)."
			"f(x).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_always;
		CHECK( check.has_value() );
	}

}
