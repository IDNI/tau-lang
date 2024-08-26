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

std::tuple<sp_tau_node<Bool>, sp_tau_node<Bool>, sp_tau_node<Bool>>
	test_rule(const std::string& rule_str)
{
	auto src_rule = make_tau_source(rule_str);
	auto statement = make_statement(src_rule);
	auto rule = statement
		| tau_parser::library
		| tau_parser::rules
		| tau_parser::rule;
	auto tau_rule = make_rule(rule.value());
	auto [matcher, body] = tau_rule;
	auto result = nso_rr_apply(tau_rule, matcher);
	return { matcher, body, result };
}

TEST_SUITE("executing wff rules") {

	TEST_CASE("WFF_TO_DNF_0") {
		auto [ matcher, body, result ] = test_rule(WFF_TO_DNF_0);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_TO_DNF_1") {
		auto [ matcher, body, result ] = test_rule(WFF_TO_DNF_1);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_0") {
		auto [ matcher, body, result ] =
			test_rule(WFF_PUSH_NEGATION_INWARDS_0);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_1") {
		auto [ matcher, body, result ] =
			test_rule(WFF_PUSH_NEGATION_INWARDS_1);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_2") {
		auto [ matcher, body, result ] =
			test_rule(WFF_PUSH_NEGATION_INWARDS_2);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_3") {
		auto [ matcher, body, result ] =
			test_rule(WFF_PUSH_NEGATION_INWARDS_3);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_ELIM_FORALL") {
		auto [ matcher, body, result ] = test_rule(WFF_ELIM_FORALL);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_DEF_XOR") {
		auto [ matcher, body, result ] = test_rule(WFF_DEF_XOR);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_DEF_CONDITIONAL") {
		auto [ matcher, body, result ] = test_rule(WFF_DEF_CONDITIONAL);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_DEF_IMPLY") {
		auto [ matcher, body, result ] = test_rule(WFF_DEF_IMPLY);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_DEF_EQUIV") {
		auto [ matcher, body, result ] = test_rule(WFF_DEF_EQUIV);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_DEF_LESS_EQUAL") {
		auto [ matcher, body, result ] = test_rule(BF_DEF_LESS_EQUAL);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_DEF_LESS") {
		auto [ matcher, body, result ] = test_rule(BF_DEF_LESS);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_DEF_GREATER") {
		auto [ matcher, body, result ] = test_rule(BF_DEF_GREATER);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_DEF_EQ") {
		auto [ matcher, body, result ] = test_rule(BF_DEF_EQ);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_DEF_NEQ") {
		auto [ matcher, body, result ] = test_rule(BF_DEF_NEQ);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_SQUEEZE_POSITIVES_0") {
		auto [ matcher, body, result ] =
			test_rule(WFF_SQUEEZE_POSITIVES_0);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_POSITIVE_LITERAL_UPWARDS_0") {
		auto [ matcher, body, result ] =
			test_rule(BF_POSITIVE_LITERAL_UPWARDS_0);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_POSITIVE_LITERAL_UPWARDS_1") {
		auto [ matcher, body, result ] =
			test_rule(BF_POSITIVE_LITERAL_UPWARDS_1);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_POSITIVE_LITERAL_UPWARDS_2") {
		auto [ matcher, body, result ] =
			test_rule(BF_POSITIVE_LITERAL_UPWARDS_2);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_POSITIVE_LITERAL_UPWARDS_3") {
		auto [ matcher, body, result ] =
			test_rule(BF_POSITIVE_LITERAL_UPWARDS_3);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_POSITIVE_LITERAL_UPWARDS_4") {
		auto [ matcher, body, result ] =
			test_rule(BF_POSITIVE_LITERAL_UPWARDS_4);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_REMOVE_EX_0") {
		auto [ matcher, body, result ] = test_rule(WFF_REMOVE_EX_0);
		CHECK( matcher != body );
		CHECK( matcher != result );
	}
}
