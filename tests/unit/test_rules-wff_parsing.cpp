// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

bool test_rule(const std::string& sample) {
	auto lib = tt(tau::get(sample, parse_library()));
	auto check = lib | tau::rules | tau::rule | tau::wff_rule;
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
}