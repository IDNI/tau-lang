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

TEST_SUITE("parsing bf rules") {

	TEST_CASE("BF_SIMPLIFY_ONE_0") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_ONE_1") { 
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_ONE_2") { 
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_ONE_3") { 
		auto src_rule = make_tau_source(BF_SIMPLIFY_ONE_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_ZERO_0") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_ZERO_1") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_ZERO_2") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_ZERO_3") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_ZERO_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_SELF_0") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_SELF_1") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_SELF_2") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_SELF_3") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SIMPLIFY_SELF_4") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_4);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
	
	TEST_CASE("BF_SIMPLIFY_SELF_5") {
		auto src_rule = make_tau_source(BF_SIMPLIFY_SELF_5);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_FUNCTIONAL_QUANTIFIERS_0") {
		auto src_rule = make_tau_source(BF_FUNCTIONAL_QUANTIFIERS_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_FUNCTIONAL_QUANTIFIERS_1") {
		auto src_rule = make_tau_source(BF_FUNCTIONAL_QUANTIFIERS_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_SKIP_CONSTANTS_0") { 
		auto src_rule = make_tau_source(BF_SKIP_CONSTANTS_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_ROTATE_LITERALS_0") {
		auto src_rule = make_tau_source(BF_ROTATE_LITERALS_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_ROTATE_LITERALS_1") {
		auto src_rule = make_tau_source(BF_ROTATE_LITERALS_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_AND") {
		auto src_rule = make_tau_source(BF_CALLBACK_AND);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_CALLBACK_OR") {
		auto src_rule = make_tau_source(BF_CALLBACK_OR);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_CALLBACK_XOR") {
		auto src_rule = make_tau_source(BF_CALLBACK_XOR);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_CALLBACK_NEG") {
		auto src_rule = make_tau_source(BF_CALLBACK_NEG);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_LESS") {
		auto src_rule = make_tau_source(BF_CALLBACK_LESS);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_CALLBACK_LESS_EQUAL") {
		auto src_rule = make_tau_source(BF_CALLBACK_LESS_EQUAL);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_CALLBACK_GREATER") {
		auto src_rule = make_tau_source(BF_CALLBACK_GREATER);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_CALLBACK_EQ") {
		auto src_rule = make_tau_source(BF_CALLBACK_EQ);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_CALLBACK_NEQ") {
		auto src_rule = make_tau_source(BF_CALLBACK_NEQ);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_IS_ZERO") {
		auto src_rule = make_tau_source(BF_CALLBACK_IS_ZERO);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_CALLBACK_IS_ONE") {
		auto src_rule = make_tau_source(BF_CALLBACK_IS_ONE);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("parsing cbf rules") {

	TEST_CASE("CBF_DISTRIBUTE_0") { 
		auto src_rule = make_tau_source(CBF_DISTRIBUTE_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("CBF_DISTRIBUTE_1") {
		auto src_rule = make_tau_source(CBF_DISTRIBUTE_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_PUSH_NEGATION_INWARDS_0") {
		auto src_rule = make_tau_source(CBF_PUSH_NEGATION_INWARDS_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_PUSH_NEGATION_INWARDS_1") {
		auto src_rule = make_tau_source(CBF_PUSH_NEGATION_INWARDS_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_ELIM_DOUBLE_NEGATION_0") {
		auto src_rule = make_tau_source(CBF_ELIM_DOUBLE_NEGATION_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ONE_0") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ONE_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ONE_1") { 
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ONE_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ONE_2") { 
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ONE_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ONE_3") { 
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ONE_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ZERO_0") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ZERO_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ZERO_1") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ZERO_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ZERO_2") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ZERO_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ZERO_3") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ZERO_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_SELF_0") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_SELF_1") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_SELF_2") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_SELF_3") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_SIMPLIFY_SELF_4") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_4);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
	
	TEST_CASE("CBF_SIMPLIFY_SELF_5") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_5);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("CBF_DEF_XOR") {
		auto src_rule = make_tau_source(CBF_DEF_XOR);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("CBF_DEF_IMPLY") {
		auto src_rule = make_tau_source(CBF_DEF_IMPLY);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("CBF_DEF_COIMPLY") {
		auto src_rule = make_tau_source(CBF_DEF_COIMPLY);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("CBF_DEF_EQUIV") {
		auto src_rule = make_tau_source(CBF_DEF_EQUIV);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::cbf_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("CBF_DEF_IF") {
		auto src_rule = make_tau_source(CBF_DEF_IF);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("parsing wff rules") {

	TEST_CASE("WFF_DISTRIBUTE_0") { 
		auto src_rule = make_tau_source(WFF_DISTRIBUTE_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_DISTRIBUTE_1") {
		auto src_rule = make_tau_source(WFF_DISTRIBUTE_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_0") {
		auto src_rule = make_tau_source(WFF_PUSH_NEGATION_INWARDS_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_1") {
		auto src_rule = make_tau_source(WFF_PUSH_NEGATION_INWARDS_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_ELIM_DOUBLE_NEGATION_0") {
		auto src_rule = make_tau_source(WFF_ELIM_DOUBLE_NEGATION_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_ELIM_FORALL") {
		auto src_rule = make_tau_source(WFF_ELIM_FORALL);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ONE_0") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ONE_1") { 
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ONE_2") { 
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ONE_3") { 
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ZERO_0") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ZERO_1") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ZERO_2") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ZERO_3") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_SELF_0") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_SELF_1") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_SELF_2") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_SELF_3") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_SIMPLIFY_SELF_4") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_4);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
	
	TEST_CASE("WFF_SIMPLIFY_SELF_5") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_5);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("WFF_DEF_XOR") {
		auto src_rule = make_tau_source(WFF_DEF_XOR);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_DEF_IMPLY") {
		auto src_rule = make_tau_source(WFF_DEF_IMPLY);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_DEF_COIMPLY") {
		auto src_rule = make_tau_source(WFF_DEF_COIMPLY);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("WFF_DEF_EQUIV") {
		auto src_rule = make_tau_source(WFF_DEF_EQUIV);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_TRIVIALITY_0") {
		auto src_rule = make_tau_source(BF_TRIVIALITY_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_TRIVIALITY_1") {
		auto src_rule = make_tau_source(BF_TRIVIALITY_1);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
	
	TEST_CASE("BF_TRIVIALITY_2") {
		auto src_rule = make_tau_source(BF_TRIVIALITY_2);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
	
	TEST_CASE("BF_TRIVIALITY_3") {
		auto src_rule = make_tau_source(BF_TRIVIALITY_3);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_SQUEEZE_POSITIVES_0") {
		auto src_rule = make_tau_source(BF_SQUEEZE_POSITIVES_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}

	TEST_CASE("BF_PROCESS_0") {
		auto src_rule = make_tau_source(BF_PROCESS_0);
		auto tau_rule = make_statement(src_rule);
		auto check = tau_rule 
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule;
		CHECK( check.has_value() );
	}
}

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
		
	TEST_CASE("BF_ROTATE_LITERALS_0") {
		auto src_rule = make_tau_source(BF_ROTATE_LITERALS_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("BF_ROTATE_LITERALS_1") {
		auto src_rule = make_tau_source(BF_ROTATE_LITERALS_1);
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
		static constexpr char* sample =	"( ({l} bf_and { r }) == F ).";
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
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::wff_eq
			| tau_parser::cbf | tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_CALLBACK_OR") {
		static constexpr char* sample =	"( ({l} bf_or { r }) == F ).";
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
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::wff_eq
			| tau_parser::cbf | tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_CALLBACK_XOR") {
		static constexpr char* sample =	"( ({l} bf_xor { r }) == F ).";
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
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::wff_eq
			| tau_parser::cbf | tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );	
	}
		
	TEST_CASE("BF_CALLBACK_NEG") {
		static constexpr char* sample =	"( bf_neg { l } == F ).";
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
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::wff_eq
			| tau_parser::cbf | tau_parser::bf | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}
		
	TEST_CASE("BF_CALLBACK_LESS") {
		static constexpr char* sample =	"( ( { l } bf_less { r } ) == F ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) }; bs["r"] = { Bool(false) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_LESS);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement 
			| tau_parser::library| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = tau_apply(tau_rule, binded);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::wff_eq
			| tau_parser::cbf | tau_parser::bf | tau_parser::bf_f;
		CHECK( check.has_value() );	
	}
		
	TEST_CASE("BF_CALLBACK_LESS_EQUAL") {
		static constexpr char* sample =	"( ( { l } bf_less_equal { r } ) == F ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) }; bs["r"] = { Bool(false) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_LESS_EQUAL);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement 
			| tau_parser::library| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = tau_apply(tau_rule, binded);
		print_sp_tau_node(std::cout, result);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::wff_eq
			| tau_parser::cbf | tau_parser::bf | tau_parser::bf_f;
		CHECK( check.has_value() );	
	}
		
	TEST_CASE("BF_CALLBACK_GREATER") {
		static constexpr char* sample =	"( ( { l } bf_greater { r } ) == F ).";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		bindings<Bool> bs; bs["l"] = { Bool(true) }; bs["r"] = { Bool(false) };
		auto binded = make_named_bindings(sample_statement, bs);
		auto rule_src = make_tau_source(BF_CALLBACK_GREATER);
		auto rule_statement = make_statement(rule_src);
		auto rule = rule_statement 
			| tau_parser::library| tau_parser::rules | tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto result = tau_apply(tau_rule, binded);
		print_sp_tau_node(std::cout, result);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::wff_eq
			| tau_parser::cbf | tau_parser::bf | tau_parser::bf_t;
		CHECK( check.has_value() );	
	}
		
	TEST_CASE("BF_CALLBACK_EQ") {
		static constexpr char* sample =	"( { l } == F ).";
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
		print_sp_tau_node(std::cout, result);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::wff_f;
		CHECK( check.has_value() );	
	}
		
	TEST_CASE("BF_CALLBACK_NEQ") {
		static constexpr char* sample =	"( { l } != F ).";
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
		print_sp_tau_node(std::cout, result);
		auto check = result
			| tau_parser::formula | tau_parser::main | tau_parser::wff | tau_parser::wff_t;
		CHECK( check.has_value() );	
	}

	TEST_CASE("BF_CALLBACK_IS_ONE") {
		static constexpr char* sample =	"( { l } == F ).";
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
		print_sp_tau_node(std::cout, result);
		auto check = result 
			| tau_parser::formula | tau_parser::main | tau_parser::wff 
			| tau_parser::wff_eq | tau_parser::cbf | tau_parser::bf | tau_parser::bf_t;
		CHECK( check.has_value() );	
	}

	TEST_CASE("BF_CALLBACK_IS_ZERO") {
		static constexpr char* sample =	"( { l } == F ).";
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
			| tau_parser::wff_eq | tau_parser::cbf | tau_parser::bf | tau_parser::bf_t;
		CHECK( check.has_value() );	
	}

	TEST_CASE("BF_CALLBACK_SUBS") {
		// TODO (HIGH) add test for subs callbacks
		CHECK( false );
	}
}

TEST_SUITE("executing cbf rules") {

	TEST_CASE("CBF_DISTRIBUTE_0") { 
		auto src_rule = make_tau_source(CBF_DISTRIBUTE_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("CBF_DISTRIBUTE_1") {
		auto src_rule = make_tau_source(CBF_DISTRIBUTE_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_PUSH_NEGATION_INWARDS_0") {
		auto src_rule = make_tau_source(CBF_PUSH_NEGATION_INWARDS_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_PUSH_NEGATION_INWARDS_1") {
		auto src_rule = make_tau_source(CBF_PUSH_NEGATION_INWARDS_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_ELIM_DOUBLE_NEGATION_0") {
		auto src_rule = make_tau_source(CBF_ELIM_DOUBLE_NEGATION_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ONE_0") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ONE_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ONE_1") { 
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ONE_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ONE_2") { 
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ONE_2);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ONE_3") { 
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ONE_3);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ZERO_0") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ZERO_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ZERO_1") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ZERO_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ZERO_2") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ZERO_2);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_ZERO_3") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_ZERO_3);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_SELF_0") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_SELF_1") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_SELF_2") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_2);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_SELF_3") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_3);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("CBF_SIMPLIFY_SELF_4") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_4);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
	
	TEST_CASE("CBF_SIMPLIFY_SELF_5") {
		auto src_rule = make_tau_source(CBF_SIMPLIFY_SELF_5);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("CBF_DEF_XOR") {
		auto src_rule = make_tau_source(CBF_DEF_XOR);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("CBF_DEF_IMPLY") {
		auto src_rule = make_tau_source(CBF_DEF_IMPLY);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("CBF_DEF_COIMPLY") {
		auto src_rule = make_tau_source(CBF_DEF_COIMPLY);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("CBF_DEF_EQUIV") {
		auto src_rule = make_tau_source(CBF_DEF_EQUIV);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("CBF_DEF_IF") {
		auto src_rule = make_tau_source(CBF_DEF_IF);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
}

TEST_SUITE("executing wff rules") {

	TEST_CASE("WFF_DISTRIBUTE_0") { 
		auto src_rule = make_tau_source(WFF_DISTRIBUTE_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_DISTRIBUTE_1") {
		auto src_rule = make_tau_source(WFF_DISTRIBUTE_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_0") {
		auto src_rule = make_tau_source(WFF_PUSH_NEGATION_INWARDS_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_PUSH_NEGATION_INWARDS_1") {
		auto src_rule = make_tau_source(WFF_PUSH_NEGATION_INWARDS_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_ELIM_DOUBLE_NEGATION_0") {
		auto src_rule = make_tau_source(WFF_ELIM_DOUBLE_NEGATION_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_ELIM_FORALL") {
		auto src_rule = make_tau_source(WFF_ELIM_FORALL);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ONE_0") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body ); 
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ONE_1") { 
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body ); 
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ONE_2") { 
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_2);
		auto statement = make_statement(src_rule); 
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body ); 
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ONE_3") { 
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ONE_3);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body ); 
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ZERO_0") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ZERO_1") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ZERO_2") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_2);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_ZERO_3") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_ZERO_3);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_SELF_0") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_SELF_1") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_SELF_2") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_2);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_SELF_3") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_3);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
		
	TEST_CASE("WFF_SIMPLIFY_SELF_4") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_4);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
	
	TEST_CASE("WFF_SIMPLIFY_SELF_5") {
		auto src_rule = make_tau_source(WFF_SIMPLIFY_SELF_5);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_DEF_XOR") {
		auto src_rule = make_tau_source(WFF_DEF_XOR);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_DEF_IMPLY") {
		auto src_rule = make_tau_source(WFF_DEF_IMPLY);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_DEF_COIMPLY") {
		auto src_rule = make_tau_source(WFF_DEF_COIMPLY);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("WFF_DEF_EQUIV") {
		auto src_rule = make_tau_source(WFF_DEF_EQUIV);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_TRIVIALITY_0") {
		auto src_rule = make_tau_source(BF_TRIVIALITY_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_TRIVIALITY_1") {
		auto src_rule = make_tau_source(BF_TRIVIALITY_1);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
	
	TEST_CASE("BF_TRIVIALITY_2") {
		auto src_rule = make_tau_source(BF_TRIVIALITY_2);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
	
	TEST_CASE("BF_TRIVIALITY_3") {
		auto src_rule = make_tau_source(BF_TRIVIALITY_3);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_SQUEEZE_POSITIVES_0") {
		auto src_rule = make_tau_source(BF_SQUEEZE_POSITIVES_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}

	TEST_CASE("BF_PROCESS_0") {
		auto src_rule = make_tau_source(BF_PROCESS_0);
		auto statement = make_statement(src_rule);
		auto rule = statement | tau_parser::library| tau_parser::rules	| tau_parser::rule;
		auto tau_rule = make_rule(rule.value());
		auto [matcher, body] = tau_rule;
		auto result = tau_apply(tau_rule, matcher);
		CHECK( matcher != body );
		CHECK( result == body );
	}
}

// TODO (MEDIUM) writes tests for simple recursive relations
// The tests should range from simple direct substitutions to more complex 
// substitutions (involving multiple variables and indexes).