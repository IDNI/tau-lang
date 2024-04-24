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
#include "bool.h"
#include "bdd_handle.h"
#include "normalizer2.h"
#include "test_helpers.h"

#include "debug_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

// TODO (MEDIUM) split into test_builder-parsing and test_builder-execution

TEST_SUITE("builders parsing") {

	TEST_CASE("BLDR_WFF_EQ") {
		auto bldr = make_builder<Bool>(BLDR_WFF_EQ);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_NEQ") {
		auto bldr = make_builder<Bool>(BLDR_WFF_NEQ);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_AND") {
		auto bldr = make_builder<Bool>(BLDR_WFF_AND);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_OR") {
		auto bldr = make_builder<Bool>(BLDR_WFF_OR);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_NEG") {
		auto bldr = make_builder<Bool>(BLDR_WFF_NEG);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_ALL") {
		auto bldr = make_builder<Bool>(BLDR_WFF_ALL);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_WFF_EX") {
		auto bldr = make_builder<Bool>(BLDR_WFF_EX);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_AND") {
		auto bldr = make_builder<Bool>(BLDR_BF_AND);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_OR") {
		auto bldr = make_builder<Bool>(BLDR_BF_OR);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_NEG") {
		auto bldr = make_builder<Bool>(BLDR_BF_NEG);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_SPLITTER") {
		auto bldr = make_builder<Bool>(BLDR_BF_SPLITTER);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_NOT_LESS_EQUAL") {
		auto bldr = make_builder<Bool>(BLDR_BF_NOT_LESS_EQUAL);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_ALL") {
		auto bldr = make_builder<Bool>(BLDR_BF_ALL);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_EX") {
		auto bldr = make_builder<Bool>(BLDR_BF_EX);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}

	TEST_CASE("BLDR_BF_CONSTANT") {
		auto bldr = make_builder<Bool>(BLDR_BF_CONSTANT);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(bldr.first) );
		CHECK( is_non_terminal<tau_parser::bf, Bool>(bldr.second) );
	}
}

// TODO (HIGH) check builded structures deeply in execution tests

TEST_SUITE("builders execution") {

	const char* sample = " X = 0 .";
	auto src = make_tau_source(sample);
	auto frml = make_statement(src);
	auto bfs = frml
		| tau_parser::nso_rr | tau_parser::nso_main | tau_parser::wff
		| tau_parser::bf_eq || tau_parser::bf;
	auto X = bfs[0] | tau_parser::variable
		| optional_value_extractor<sp_tau_node<Bool>>;
	auto F = bfs[1] | tau_parser::bf_f
		| optional_value_extractor<sp_tau_node<Bool>>;

	TEST_CASE("BLDR_WFF_EQ") {
		auto bldr = make_builder<Bool>(BLDR_WFF_EQ);
		std::vector<sp_tau_node<Bool>> args = {X};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_eq;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_NEQ") {
		auto bldr = make_builder<Bool>(BLDR_WFF_NEQ);
		std::vector<sp_tau_node<Bool>> args = {X};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_neq;
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
		// TODO (LOW) write proper test
		CHECK( true );
	}

	TEST_CASE("BLDR_WFF_NEG") {
		auto bldr = make_builder<Bool>(BLDR_WFF_NEG);
		std::vector<sp_tau_node<Bool>> args = {F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_neg;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_IMPLY") {
		// TODO (LOW) write proper test
		CHECK( true );
	}

	TEST_CASE("BLDR_WFF_EQUIV") {
		// TODO (LOW) write proper test
		CHECK( true );
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
		// TODO (LOW) write proper test
		CHECK( true );
	}

	TEST_CASE("BLDR_BF_NEG") {
		auto bldr = make_builder<Bool>(BLDR_BF_NEG);
		std::vector<sp_tau_node<Bool>> args = {F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_neg;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_SPLITTER") {
		auto bldr = make_builder<Bool>(BLDR_BF_SPLITTER);
		std::vector<sp_tau_node<Bool>> args = {F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_splitter;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_LESS") {
		// TODO (LOW) write proper test
		CHECK( true );
	}

	TEST_CASE("BLDR_BF_LESS_EQUAL") {
		// TODO (LOW) write proper test
		CHECK( true );
	}

	// TODO (HIGH) implement update this tests to cover upper and lower bounds
	/*TEST_CASE("BLDR_BF_NOT_LESS_EQUAL") {
		auto bldr = make_builder<Bool>(BLDR_BF_NOT_LESS_EQUAL);
		std::vector<sp_tau_node<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_not_less_equal;
		CHECK( check.has_value() );
	}*/

	TEST_CASE("BLDR_BF_GREATER") {
		// TODO (LOW) write proper test
		CHECK( true );
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

	TEST_CASE("BLDR_BF_CONSTANT") {
		auto bldr = make_builder<Bool>(BLDR_BF_CONSTANT);
		std::vector<sp_tau_node<Bool>> args = { F };
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_constant | tau_parser::constant;
		CHECK( check.has_value() );
	}
}