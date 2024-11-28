// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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

bool test_rule(const std::string& sample) {
	auto src_rule = make_tau_source(sample, {
						.start = tau_parser::library });
	auto tau_rule = make_statement(src_rule);
	auto check = tau_rule
		| tau_parser::rules
		| tau_parser::rule
		| tau_parser::wff_rule;
	return check.has_value();
}

TEST_SUITE("parsing wff rules") {

	TEST_CASE("WFF_TO_DNF_0") {
		CHECK( test_rule(WFF_TO_DNF_0) );
	}

	TEST_CASE("WFF_TO_DNF_1") {
		CHECK( test_rule(WFF_TO_DNF_1) );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_0") {
		CHECK( test_rule(WFF_PUSH_NEGATION_INWARDS_0) );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_1") {
		CHECK( test_rule(WFF_PUSH_NEGATION_INWARDS_1) );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_2") {
		CHECK( test_rule(WFF_PUSH_NEGATION_INWARDS_2) );
	}

	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_3") {
		CHECK( test_rule(WFF_PUSH_NEGATION_INWARDS_3) );
	}

	TEST_CASE("WFF_ELIM_FORALL") {
		CHECK( test_rule(WFF_ELIM_FORALL) );
	}

	TEST_CASE("WFF_REMOVE_EX_0") {
		CHECK( test_rule(WFF_REMOVE_EX_0) );
	}
}