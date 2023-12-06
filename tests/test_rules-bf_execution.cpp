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

#include "../src/doctest.h"
#include "../src/formula.h"
#include "../src/bool.h"
#include "../src/bdd_handle.h"
#include "../src/normalizer2.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("executing bf rules") {

	TEST_CASE("BF_SIMPLIFY_ONE_0") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_ONE_1") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_ONE_2") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_2);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_ONE_3") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_3);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_ONE_4") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_4);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_ZERO_0") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_ZERO_1") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_ZERO_2") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_2);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_ZERO_3") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_3);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_ZERO_4") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_4);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_0") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_1") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_2") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_2);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_3") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_3);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_4") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_4);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SIMPLIFY_SELF_5") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_5);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_FUNCTIONAL_QUANTIFIERS_0") {
		auto src_rule = make_tau_source(BF_FUNCTIONAL_QUANTIFIERS_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		auto args = result  | tau_parser::bf_and || tau_parser::bf;
		CHECK( is_non_terminal<tau_parser::bf, Bool>(result) );
		CHECK( args.size() == 2 );
		CHECK( args[0] == args[1] );
	}

	TEST_CASE("BF_FUNCTIONAL_QUANTIFIERS_1") {
		auto src_rule = make_tau_source(BF_FUNCTIONAL_QUANTIFIERS_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		auto args = result  | tau_parser::bf_or ||tau_parser::bf;
		CHECK( is_non_terminal<tau_parser::bf, Bool>(result) );
		CHECK( args.size() == 2 );
		CHECK( args[0] == args[1] );
	}

	TEST_CASE("BF_SKIP_CONSTANTS_0") {
		auto src_rule = make_tau_source(BF_SKIP_CONSTANTS_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_DEF_XOR") {
		auto src_rule = make_tau_source(BF_DEF_XOR);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	// REVIEW (MEDIUM) remaining tests of the test suite are flaky

	// They properly run when executed independenly but fails otherwise. Maybe
	// there is something wrong with the way the tests are executed, the constants
	// used within or something else.

	TEST_CASE("BF_CALLBACK_AND") {
		const char* sample = "( ({l} & { r } ) = 0 ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) }; bs["r"] = { Bool(false) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_AND);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::library| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = tau_apply(tau_rule, binded);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::bf_eq
			| tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_OR") {
		const char* sample = "( ({l} | { r } ) = 0 ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) }; bs["r"] = { Bool(false) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_OR);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::library| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = tau_apply(tau_rule, binded);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::bf_eq
			| tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_XOR") {
		const char* sample = "( ({l} + { r } ) = 0 ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) }; bs["r"] = { Bool(false) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_XOR);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::library| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = tau_apply(tau_rule, binded);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::bf_eq
			| tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_NEG") {
		const char* sample = "( ~ { l } = 0 ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_NEG);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::library| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = tau_apply(tau_rule, binded);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::bf_eq
			| tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_EQ") {
		const char* sample = "( { l } = 0 ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_EQ);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::library| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = tau_apply(tau_rule, binded);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_NEQ") {
		const char* sample = "( { l } != 0 ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_NEQ);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::library| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = tau_apply(tau_rule, binded);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_IS_ONE") {
		const char* sample = "( { l } = 0 ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_IS_ONE);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::library| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = tau_apply(tau_rule, binded);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff
			| tau_parser::bf_eq | tau_parser::bf | tau_parser::bf_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_IS_ZERO") {
		const char* sample = "( { l } = 0 ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(false) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_IS_ZERO);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::library| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = tau_apply(tau_rule, binded);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff
			| tau_parser::bf_eq | tau_parser::bf | tau_parser::bf_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_CLASHING_SUBFORMULAS_0") {
		const char* sample = "( (0 & ~ 0) = 0 ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		auto rule_src = make_tau_source(BF_CALLBACK_CLASHING_SUBFORMULAS_0);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::library| tau_parser::rules | tau_parser::rule | optional_value_extractor<sp_tau_node<Bool>>;
		auto tau_rule = make_rule(rule);
		auto result = tau_apply(tau_rule, sample_statement);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff
			| tau_parser::bf_eq | tau_parser::bf | tau_parser::bf_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_HAS_SUBFORMULA_0") {
		const char* sample = "( (1 & 0) = 0 ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		auto rule_src = make_tau_source(BF_CALLBACK_HAS_SUBFORMULA_0);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement
			| tau_parser::library| tau_parser::rules | tau_parser::rule | optional_value_extractor<sp_tau_node<Bool>>;
		auto tau_rule = make_rule(rule);
		auto result = tau_apply(tau_rule, sample_statement);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff
			| tau_parser::bf_eq | tau_parser::bf | tau_parser::bf_f;
		CHECK( check.has_value() );
	}

}