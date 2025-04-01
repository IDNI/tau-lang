// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "test_helpers.h"

bool test_rule(const std::string& sample, tau_parser::nonterminal rule_type) {
	auto lib = make_tt<Bool>(sample, { .start = tau::library });
	auto check = lib | tau::rules | tau::rule | rule_type;
	return check.has_value();
}

TEST_SUITE("parsing bf rules") {

	TEST_CASE("BF_TO_DNF_0") {
		CHECK( test_rule(BF_TO_DNF_0, tau::bf_rule) );
	}

	TEST_CASE("BF_TO_DNF_1") {
		CHECK( test_rule(BF_TO_DNF_1, tau::bf_rule) );
	}
}
