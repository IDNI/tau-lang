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
#include "bool.h"
#include "bdd_handle.h"
#include "normalizer2.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("parsing wff rules") {

	TEST_CASE("WFF_TO_DNF_0") {
		auto src_rule = make_tau_source(WFF_TO_DNF_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_TO_DNF_1") {
		auto src_rule = make_tau_source(WFF_TO_DNF_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_0") {
		auto src_rule = make_tau_source(WFF_PUSH_NEGATION_INWARDS_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_1") {
		auto src_rule = make_tau_source(WFF_PUSH_NEGATION_INWARDS_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_2") {
		auto src_rule = make_tau_source(WFF_PUSH_NEGATION_INWARDS_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_3") {
		auto src_rule = make_tau_source(WFF_PUSH_NEGATION_INWARDS_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_ELIM_DOUBLE_NEGATION_0") {
		auto src_rule = make_tau_source(WFF_ELIM_DOUBLE_NEGATION_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_ELIM_FORALL") {
		auto src_rule = make_tau_source(WFF_ELIM_FORALL);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_ONE_0") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_ONE_1") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_ONE_2") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_ONE_3") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_ONE_4") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_4);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_ZERO_0") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_ZERO_1") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_ZERO_2") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_ZERO_3") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_ZERO_4") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_4);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_SELF_0") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_SELF_1") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_SELF_2") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_SELF_3") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_SELF_4") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_4);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_SIMPLIFY_SELF_5") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_5);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_DEF_XOR") {
		auto src_rule = make_tau_source(WFF_DEF_XOR);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_DEF_IMPLY") {
		auto src_rule = make_tau_source(WFF_DEF_IMPLY);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_DEF_EQUIV") {
		auto src_rule = make_tau_source(WFF_DEF_EQUIV);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_DEF_CONDITIONAL") {
		auto src_rule = make_tau_source(WFF_DEF_CONDITIONAL);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_DEF_LESS_EQUAL") {
		auto src_rule = make_tau_source(BF_DEF_LESS_EQUAL);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
	TEST_CASE("BF_DEF_LESS") {
		auto src_rule = make_tau_source(BF_DEF_LESS);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
	TEST_CASE("BF_DEF_GREATER") {
		auto src_rule = make_tau_source(BF_DEF_GREATER);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
	TEST_CASE("BF_DEF_EQ") {
		auto src_rule = make_tau_source(BF_DEF_EQ);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
	TEST_CASE("BF_DEF_NEQ") {
		auto src_rule = make_tau_source(BF_DEF_NEQ);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_EQ_SIMPLIFY_0") {
		auto src_rule = make_tau_source(BF_EQ_SIMPLIFY_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_EQ_SIMPLIFY_1") {
		auto src_rule = make_tau_source(BF_EQ_SIMPLIFY_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_NEQ_SIMPLIFY_0") {
		auto src_rule = make_tau_source(BF_NEQ_SIMPLIFY_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_NEQ_SIMPLIFY_1") {
		auto src_rule = make_tau_source(BF_NEQ_SIMPLIFY_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SQUEEZE_POSITIVES_0") {
		auto src_rule = make_tau_source(BF_SQUEEZE_POSITIVES_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_POSITIVE_LITERAL_UPWARDS_0") {
		auto src_rule = make_tau_source(BF_POSITIVE_LITERAL_UPWARDS_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_POSITIVE_LITERAL_UPWARDS_1") {
		auto src_rule = make_tau_source(BF_POSITIVE_LITERAL_UPWARDS_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_POSITIVE_LITERAL_UPWARDS_2") {
		auto src_rule = make_tau_source(BF_POSITIVE_LITERAL_UPWARDS_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_POSITIVE_LITERAL_UPWARDS_3") {
		auto src_rule = make_tau_source(BF_POSITIVE_LITERAL_UPWARDS_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_POSITIVE_LITERAL_UPWARDS_4") {
		auto src_rule = make_tau_source(BF_POSITIVE_LITERAL_UPWARDS_4);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_REMOVE_EX_0") {
		auto src_rule = make_tau_source(WFF_REMOVE_EX_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_DEF_BEX_0") {
		auto src_rule = make_tau_source(WFF_DEF_BEX_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_DEF_BALL_0") {
		auto src_rule = make_tau_source(WFF_DEF_BALL_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
}