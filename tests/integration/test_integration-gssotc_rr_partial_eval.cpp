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
#include "satisfiability.h"
#include "bool.h"
#include "bdd_handle.h"
#include "normalizer2.h"

#include "test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("rec relations execution: simple cases") {

	TEST_CASE("tau_rec_relation") {
		const char* sample =
			"h[0]($X $Y) :::= {($X = $Y)}."
			"g[$n]($Y) :::= h[$n - 1]($Y 0)."
			"g[1](0);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory, bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory, bdd_test>, bdd_test>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}
}

TEST_SUITE("functions partial evaluation: simple cases") {

	TEST_CASE("tau_rec_relation") {
		const char* sample =
			"h($X $Y) :::= {($X = $Y)}."
			"g($Y) :::= h($Y 0)."
			"g(0);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory, bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory, bdd_test>, bdd_test>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}
}