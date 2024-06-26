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
#include "bool_ba.h"
#include "bdd_handle.h"
#include "normalizer.h"

// TODO (LOW) consider move this test to integration tests
#include "../integration/test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

using namespace idni::rewriter;
using namespace idni::tau;
using namespace std;

namespace testing = doctest;

TEST_SUITE("strong normal forms for wff") {

	TEST_CASE("case1") {
		/*const char* sample = "{ T };";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_literals<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 1 );*/
		CHECK( true );
	}
}

TEST_SUITE("order normal form") {

	TEST_CASE("case1") {
		/*const char* sample = "{ T };";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_literals<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 1 );*/
		CHECK( true );
	}
}
