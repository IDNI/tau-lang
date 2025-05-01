// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "test_helpers.h"
#include "normal_forms.h"
#include "normalizer.h"
#include "nso_rr.h"

using bnode = tau_lang::node<Bool>;
using tau = tree<bnode>;

std::tuple<tref, tref, tref> test_rule(const std::string& rule_str) {
        auto tau_rule = tau::get_rules(rule_str)[0];
	auto [matcher, body] = tau_rule;
	tref result = nso_rr_apply<bnode>(tau_rule, matcher->get());
	return { matcher->get(), body->get(), result };
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
