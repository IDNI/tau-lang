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

#include "../../src/doctest.h"
#include "../../src/nso_rr.h"
#include "../../src/bool.h"
#include "../../src/bdd_handle.h"
#include "../../src/normalizer2.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("parsing bf rules") {

	TEST_CASE("BF_DISTRIBUTE_0") {
		auto src_rule = make_tau_source(BF_DISTRIBUTE_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_DISTRIBUTE_1") {
		auto src_rule = make_tau_source(BF_DISTRIBUTE_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_ONE_0") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_ONE_1") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_ONE_2") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_ONE_3") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_ONE_4") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_4);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_ZERO_0") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_ZERO_1") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_ZERO_2") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_ZERO_3") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_ZERO_4") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_4);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_0") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_1") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_2") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_3") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_4") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_4);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_5") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_5);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_FUNCTIONAL_QUANTIFIERS_0") {
		auto src_rule = make_tau_source(BF_FUNCTIONAL_QUANTIFIERS_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_FUNCTIONAL_QUANTIFIERS_1") {
		auto src_rule = make_tau_source(BF_FUNCTIONAL_QUANTIFIERS_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SKIP_CONSTANTS_0") {
		auto src_rule = make_tau_source(BF_SKIP_CONSTANTS_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_DEF_XOR") {
		auto src_rule = make_tau_source(BF_DEF_XOR);
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

	TEST_CASE("BF_CALLBACK_HAS_SUBFORMULA_0") {
		auto src_rule = make_tau_source(BF_CALLBACK_HAS_SUBFORMULA_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
}
