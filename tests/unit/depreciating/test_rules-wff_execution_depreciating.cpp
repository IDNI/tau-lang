// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "depreciating/nso_rr_depreciating.h"
#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/bdds/bdd_handle.h"
#include "depreciating/normalizer_depreciating.h"
#include "test_helpers_depreciating.h"

using namespace idni::rewriter::depreciating;
using namespace idni::tau_lang;
using namespace idni::tau_lang_depreciating;

namespace testing = doctest;

std::tuple<tau_<Bool>, tau_<Bool>, tau_<Bool>>
	test_rule(const std::string& rule_str)
{
	auto src_rule = make_tau_source(rule_str, {
						.start = tau_parser::library });
	auto statement = make_statement(src_rule);
	auto rule = statement
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
}
