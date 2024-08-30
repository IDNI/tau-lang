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

TEST_SUITE("rec relations partial evaluation: simple cases") {

	TEST_CASE("wff_rec_relation") {
		const char* sample =
			"h[0](X, Y) := X + Y != 0."
			"g[n](Y) := h[n-1](Y, 0)."
			"g[1](1).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation") {
		const char* sample =
			"h[0](X, Y) := X + Y."
			"g[n](Y) := h[n-1](Y, 0)."
			"g[1](0) = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("functions partial evaluation: simple cases") {

	TEST_CASE("wff_rec_relation") {
		const char* sample =
			"h(X, Y) := X = Y."
			"g(Y) := h(Y, 0)."
			"g(0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation") {
		const char* sample =
			"h(X, Y) := X + Y."
			"g(Y) := h(Y, 0)."
			"g(1) = 1.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}