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
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

bool test_rule(const std::string& sample, tau_parser::nonterminal rule_type) {
	auto src_rule = make_tau_source(sample, {
						.start = tau_parser::library });
	auto tau_rule = make_statement(src_rule);
	auto check = tau_rule
		| tau_parser::rules
		| tau_parser::rule
		| rule_type;
	return check.has_value();
}

TEST_SUITE("parsing bf rules") {

	TEST_CASE("BF_TO_DNF_0") {
		CHECK( test_rule(BF_TO_DNF_0, tau_parser::bf_rule) );
	}

	TEST_CASE("BF_TO_DNF_1") {
		CHECK( test_rule(BF_TO_DNF_1, tau_parser::bf_rule) );
	}
}
