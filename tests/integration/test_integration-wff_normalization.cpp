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
//#include "../unit/test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("wff_sometimes") {
	TEST_CASE("push_in_1") {
		const char* sample = "sometimes (?x && o1[t] = 0 && sometimes(?x && o1[t] = 0 && sometimes(?x && o1[t] = 0))).";
		auto src = make_tau_source(sample);
		bdd_test_factory bf;
		auto fm = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(src, bf).main;
		auto result = push_sometimes_always_in(fm);
		auto simp_res = result
			| repeat_all<step<bdd_test>, bdd_test>(simplify_wff<bdd_test>)
			| reduce_wff<bdd_test>;
		std::stringstream ss; ss << simp_res;
		CHECK((ss.str() == "?x && (sometimes o1[t] = 0)" || ss.str() == "(sometimes o1[t] = 0) && ?x"));
	}

	TEST_CASE("push_in_2") {
		const char* sample = "sometimes T.";
		auto src = make_tau_source(sample);
		bdd_test_factory bf;
		auto fm = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(src, bf).main;
		auto result = push_sometimes_always_in(fm);
		auto simp_res = result
			| repeat_all<step<bdd_test>, bdd_test>(simplify_wff<bdd_test>)
			| reduce_wff<bdd_test>;
		CHECK( (simp_res | tau_parser::wff_t).has_value() );
	}

	TEST_CASE("push_in_3") {
		const char* sample = "sometimes (always o1[t] = 0 || (sometimes o1[t] = 0)) && (always o1[t] = 0).";
		auto src = make_tau_source(sample);
		bdd_test_factory bf;
		auto fm = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(src, bf).main;
		auto result = push_sometimes_always_in(fm);
		auto simp_res = result
			| repeat_all<step<bdd_test>, bdd_test>(simplify_wff<bdd_test>)
			| reduce_wff<bdd_test>;
		std::stringstream ss; ss << simp_res ;
		CHECK( ss.str() == "always o1[t] = 0" );
	}

	TEST_CASE("pull_out_1") {
		const char* sample = "(sometimes T && ?x) || (sometimes T && ?x) || (sometimes ?x).";
		auto src = make_tau_source(sample);
		bdd_test_factory bf;
		auto fm = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(src, bf).main;
		auto result = pull_sometimes_always_out(fm);
		auto simp_res = result
			| repeat_all<step<bdd_test>, bdd_test>(simplify_wff<bdd_test>)
			| reduce_wff<bdd_test>;
		std::stringstream ss; ss << simp_res;
		CHECK(ss.str() == "sometimes ?x");
	}

	TEST_CASE("pull_out_2") {
		const char* sample = "(always ?x) && ?x && (sometimes ?x && ?x).";
		auto src = make_tau_source(sample);
		bdd_test_factory bf;
		auto fm = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(src, bf).main;
		auto result = pull_sometimes_always_out(fm);
		auto simp_res = result
			| repeat_all<step<bdd_test>, bdd_test>(simplify_wff<bdd_test>)
			| reduce_wff<bdd_test>;
		std::stringstream ss; ss << simp_res;
		CHECK(ss.str() == "always ?x");
	}
}