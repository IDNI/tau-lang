// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

bool test_rule(const std::string& sample, typename bnode::type rule_type) {
	auto lib = tt(tau::get(sample, parse_library()));
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
