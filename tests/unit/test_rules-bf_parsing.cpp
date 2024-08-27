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

TEST_SUITE("parsing bf rules") {

	TEST_CASE("BF_TO_DNF_0") {
		auto src_rule = make_tau_source(BF_TO_DNF_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_TO_DNF_1") {
		auto src_rule = make_tau_source(BF_TO_DNF_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_AND") {
		auto src_rule = make_tau_source(BF_CALLBACK_AND);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_OR") {
		auto src_rule = make_tau_source(BF_CALLBACK_OR);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_XOR") {
		auto src_rule = make_tau_source(BF_CALLBACK_XOR);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_NEG") {
		auto src_rule = make_tau_source(BF_CALLBACK_NEG);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_EQ") {
		auto src_rule = make_tau_source(BF_CALLBACK_EQ);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_NEQ") {
		auto src_rule = make_tau_source(BF_CALLBACK_NEQ);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_IS_ZERO") {
		auto src_rule = make_tau_source(BF_CALLBACK_IS_ZERO);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_IS_ONE") {
		auto src_rule = make_tau_source(BF_CALLBACK_IS_ONE);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
}
