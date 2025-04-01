// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "nso_rr.h"
#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/bdds/bdd_handle.h"
#include "normalizer.h"
#include "test_helpers_depreciating.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

bool test_rule(const std::string& sample, tau_parser::nonterminal rule_type) {
	auto src_rule = make_tau_source(sample, {
						.start = tau_parser::library });
	auto tau_rule = make_statement(src_rule);
	auto check = tau_rule
		| tau_parser::rules
		| tau_parser::rule
		| rule_type;
	return check.has_value();
}

TEST_SUITE("parsing bf rules") {

	TEST_CASE("BF_TO_DNF_0") {
		CHECK( test_rule(BF_TO_DNF_0, tau_parser::bf_rule) );
	}

	TEST_CASE("BF_TO_DNF_1") {
		CHECK( test_rule(BF_TO_DNF_1, tau_parser::bf_rule) );
	}
}
