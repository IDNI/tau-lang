// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "test_helpers.h"

#include "debug_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

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
}

// TODO (HIGH) check builded structures deeply in execution tests

TEST_SUITE("builders execution") {

	const char* sample = " X = 0 .";
	auto src = make_tau_source(sample);
	auto frml = make_statement(src);
	auto bfs = frml
		| tau_parser::rr | tau_parser::main | tau_parser::wff
		| tau_parser::bf_eq || tau_parser::bf;
	auto X = bfs[0] | tau_parser::variable
		| optional_value_extractor<tau<Bool>>;
	auto F = bfs[1] | tau_parser::bf_f
		| optional_value_extractor<tau<Bool>>;

	TEST_CASE("BLDR_WFF_EQ") {
		auto bldr = make_builder<Bool>(BLDR_WFF_EQ);
		std::vector<tau<Bool>> args = {X};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_eq;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_NEQ") {
		auto bldr = make_builder<Bool>(BLDR_WFF_NEQ);
		std::vector<tau<Bool>> args = {X};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_neq;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_XOR") {
		// TODO (LOW) write proper test
		CHECK( true );
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
		std::vector<tau<Bool>> args = {X, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_all;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_WFF_EX") {
		auto bldr = make_builder<Bool>(BLDR_WFF_EX);
		std::vector<tau<Bool>> args = {X, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::wff_ex;
		CHECK( check.has_value() );
	}

	TEST_CASE("BLDR_BF_SPLITTER") {
		auto bldr = make_builder<Bool>(BLDR_BF_SPLITTER);
		std::vector<tau<Bool>> args = {F};
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
		std::vector<tau<Bool>> args = {F, F};
		auto check = tau_apply_builder<Bool>(bldr, args) | tau_parser::bf_not_less_equal;
		CHECK( check.has_value() );
	}*/

	TEST_CASE("BLDR_BF_GREATER") {
		// TODO (LOW) write proper test
		CHECK( true );
	}
}