// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "doctest.h"
#include "nso_rr.h"
#include "bool_ba.h"
#include "bdd_handle.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;
using namespace std;

namespace testing = doctest;

TEST_SUITE("operator|") {

	TEST_CASE("match zero nodes") {
		const char* sample = "$X & $Y := $Z.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto lib = make_statement(src);
		auto args = lib
			| tau_parser::main;
		CHECK( !args );
	}

	TEST_CASE("match one node") {
		const char* sample = "$X & $Y := $Z.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto lib = make_statement(src);
		auto args = lib
			| tau_parser::rules;
		CHECK( args.has_value() );
	}
}

TEST_SUITE("operator||") {

	TEST_CASE("match zero nodes") {
		const char* sample = "$X & $Y := $Z.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto lib = make_statement(src);
		auto args = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule
			|| tau_parser::wff;
		CHECK( args.size() == 0 );
	}

	TEST_CASE("match one node") {
		const char* sample = "X & Y := Z.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto lib = make_statement(src);
		auto args = lib
			| tau_parser::rules
			| tau_parser::rule
			|| tau_parser::bf_rule;
		CHECK( args.size() == 1 );
	}

	TEST_CASE("match several nodes") {
		const char* sample = "X & Y := Z.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto lib = make_statement(src);
		auto args = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_and
			|| tau_parser::bf;
		CHECK( args.size() == 2 );
	}
}
