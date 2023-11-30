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

// TODO (TK) (HIGH) remove BLDR_WFF_T, BLDR_WFF_F, BLDR_BF_T and BLDR_BF_F

TEST_SUITE("builders parsing") {

	TEST_CASE("BLDR_WFF_EQ") {
		auto bldr = make_builder<Bool>(BLDR_WFF_EQ);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_NEQ") {
		auto bldr = make_builder<Bool>(BLDR_WFF_NEQ);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_AND") {
		auto bldr = make_builder<Bool>(BLDR_WFF_AND);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_OR") {
		auto bldr = make_builder<Bool>(BLDR_WFF_OR);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_XOR") {
		auto bldr = make_builder<Bool>(BLDR_WFF_XOR);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_NEG") {
		auto bldr = make_builder<Bool>(BLDR_WFF_NEG);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_IMPLY") {
		auto bldr = make_builder<Bool>(BLDR_WFF_IMPLY);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_EQUIV") {
		auto bldr = make_builder<Bool>(BLDR_WFF_EQUIV);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_COIMPLY") {
		auto bldr = make_builder<Bool>(BLDR_WFF_COIMPLY);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_ALL") {
		auto bldr = make_builder<Bool>(BLDR_WFF_ALL);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_EX") {
		auto bldr = make_builder<Bool>(BLDR_WFF_EX);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_AND") {
		auto bldr = make_builder<Bool>(BLDR_BF_AND);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_OR") {
		auto bldr = make_builder<Bool>(BLDR_BF_OR);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_XOR") {
		auto bldr = make_builder<Bool>(BLDR_BF_XOR);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_NEG") {
		auto bldr = make_builder<Bool>(BLDR_BF_NEG);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_LESS") {
		auto bldr = make_builder<Bool>(BLDR_BF_LESS);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_LESS_EQUAL") {
		auto bldr = make_builder<Bool>(BLDR_BF_LESS_EQUAL);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_GREATER") {
		auto bldr = make_builder<Bool>(BLDR_BF_GREATER);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_ALL") {
		auto bldr = make_builder<Bool>(BLDR_BF_ALL);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_EX") {
		auto bldr = make_builder<Bool>(BLDR_BF_EX);
		CHECK( is_non_terminal<tau_parser::captures, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}
}

TEST_SUITE("builders execution") {

	static constexpr char* sample =	" ( ?X = F ) .";
	auto src = make_tau_source(sample);
	auto frml = make_statement(src);
	auto X = frml
		| tau_parser::formula | tau_parser::main | tau_parser::wff
		| tau_parser::wff_eq | tau_parser::bf | tau_parser::variable
		| optional_value_extractor<sp_tau_node<Bool>>;
	auto F = frml
		| tau_parser::formula | tau_parser::main | tau_parser::wff
		| tau_parser::wff_eq | tau_parser::wff_f
		| optional_value_extractor<sp_tau_node<Bool>>;

	TEST_CASE("BLDR_WFF_EQ") {
		auto bldr = make_builder<Bool>(BLDR_WFF_EQ);
		std::vector<sp_tau_node<Bool>> args = {X};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_eq;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_NEQ") {
		auto bldr = make_builder<Bool>(BLDR_WFF_NEQ);
		std::vector<sp_tau_node<Bool>> args = {X};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_neq;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_AND") {
		auto bldr = make_builder<Bool>(BLDR_WFF_AND);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_and;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_OR") {
		auto bldr = make_builder<Bool>(BLDR_WFF_OR);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_or;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_XOR") {
		auto bldr = make_builder<Bool>(BLDR_WFF_XOR);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_xor;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_NEG") {
		auto bldr = make_builder<Bool>(BLDR_WFF_NEG);
		std::vector<sp_tau_node<Bool>> args = {F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_neg;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_IMPLY") {
		auto bldr = make_builder<Bool>(BLDR_WFF_IMPLY);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_imply;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_EQUIV") {
		auto bldr = make_builder<Bool>(BLDR_WFF_EQUIV);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_equiv;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_COIMPLY") {
		auto bldr = make_builder<Bool>(BLDR_WFF_COIMPLY);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_coimply;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_ALL") {
		auto bldr = make_builder<Bool>(BLDR_WFF_ALL);
		std::vector<sp_tau_node<Bool>> args = {X, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_all;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_EX") {
		auto bldr = make_builder<Bool>(BLDR_WFF_EX);
		std::vector<sp_tau_node<Bool>> args = {X, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_ex;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_AND") {
		auto bldr = make_builder<Bool>(BLDR_BF_AND);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_and;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_OR") {
		auto bldr = make_builder<Bool>(BLDR_BF_OR);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_or;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_XOR") {
		auto bldr = make_builder<Bool>(BLDR_BF_XOR);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_xor;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_NEG") {
		auto bldr = make_builder<Bool>(BLDR_BF_NEG);
		std::vector<sp_tau_node<Bool>> args = {F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_neg;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_LESS") {
		auto bldr = make_builder<Bool>(BLDR_BF_LESS);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_less;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_LESS_EQUAL") {
		auto bldr = make_builder<Bool>(BLDR_BF_LESS_EQUAL);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_less_equal;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_GREATER") {
		auto bldr = make_builder<Bool>(BLDR_BF_GREATER);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_greater;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_ALL") {
		auto bldr = make_builder<Bool>(BLDR_BF_ALL);
		std::vector<sp_tau_node<Bool>> args = {X, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_all;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_EX") {
		auto bldr = make_builder<Bool>(BLDR_BF_EX);
		std::vector<sp_tau_node<Bool>> args = {X, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_ex;
		CHECK( check.has_value() );
	}
}