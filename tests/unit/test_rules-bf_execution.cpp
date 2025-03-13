// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

std::tuple<tau<Bool>, tau<Bool>, tau<Bool>>
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
