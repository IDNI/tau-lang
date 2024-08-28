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

TEST_SUITE("executing bf rules") {

	TEST_CASE("BF_CALLBACK_AND") {
		const char* sample = " { l } & { r } = 0 .";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) }; bs["r"] = { Bool(false) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_AND, {
						.start = tau_parser::library });
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = nso_rr_apply(tau_rule, binded);
		auto check = result
			| tau_parser::rr | tau_parser::main | tau_parser::wff | tau_parser::bf_eq
			| tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_OR") {
		const char* sample = " { l } | { r } = 0 .";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) }; bs["r"] = { Bool(false) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_OR, {
						.start = tau_parser::library });
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = nso_rr_apply(tau_rule, binded);
		auto check = result
			| tau_parser::rr | tau_parser::main | tau_parser::wff | tau_parser::bf_eq
			| tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_XOR") {
		const char* sample = " { l } + { r } = 0 .";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) }; bs["r"] = { Bool(false) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_XOR, {
						.start = tau_parser::library });
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = nso_rr_apply(tau_rule, binded);
		auto check = result
			| tau_parser::rr | tau_parser::main | tau_parser::wff | tau_parser::bf_eq
			| tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_NEG") {
		const char* sample = " { l }' = 0 .";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_NEG, {
						.start = tau_parser::library });
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = nso_rr_apply(tau_rule, binded);
		auto check = result
			| tau_parser::rr | tau_parser::main | tau_parser::wff | tau_parser::bf_eq
			| tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_EQ") {
		const char* sample = " { l } = 0 .";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_EQ, {
						.start = tau_parser::library });
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = nso_rr_apply(tau_rule, binded);
		auto check = result
			| tau_parser::rr | tau_parser::main | tau_parser::wff | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_NEQ") {
		const char* sample = " { l } != 0 .";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_NEQ, {
						.start = tau_parser::library });
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = nso_rr_apply(tau_rule, binded);
		auto check = result
			| tau_parser::rr | tau_parser::main | tau_parser::wff | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_IS_ONE") {
		const char* sample = " { l } = 0 .";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_IS_ONE, {
						.start = tau_parser::library });
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = nso_rr_apply(tau_rule, binded);
		auto check = result
			| tau_parser::rr | tau_parser::main | tau_parser::wff
			| tau_parser::bf_eq | tau_parser::bf | tau_parser::bf_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_IS_ZERO") {
		const char* sample = " { l } = 0 .";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(false) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_IS_ZERO, {
						.start = tau_parser::library });
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = nso_rr_apply(tau_rule, binded);
		auto check = result
			| tau_parser::rr | tau_parser::main | tau_parser::wff
			| tau_parser::bf_eq | tau_parser::bf | tau_parser::bf_f;
		CHECK( check.has_value() );
	}
}