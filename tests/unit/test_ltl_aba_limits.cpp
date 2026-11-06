// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "ltl_aba_limits.h"

#include <string>

using namespace idni::tau_lang;

TEST_SUITE("ltl_verdict_unknown") {

	TEST_CASE("no flag leaves the unrealizable verdict decided") {
		ltl_verdict_scope scope;
		CHECK_FALSE( ltl_verdict_unknown_cause().has_value() );
	}

	TEST_CASE("a cap alone names the cap and its options") {
		ltl_verdict_scope scope;
		ltl_verdict_incomplete = true;
		auto cause = ltl_verdict_unknown_cause();
		REQUIRE( cause.has_value() );
		CHECK( ltl_verdict_unknown_message(*cause)
			== "UNKNOWN: unrealizable only because a consistency cap "
			"skipped checks (--max-consistency-subsets / "
			"--max-cover-products); realizability could not be decided" );
	}

	TEST_CASE("an undecided check alone names no option") {
		ltl_verdict_scope scope;
		ltl_verdict_undecided = true;
		auto cause = ltl_verdict_unknown_cause();
		REQUIRE( cause.has_value() );
		CHECK( ltl_verdict_unknown_message(*cause)
			== "UNKNOWN: unrealizable only because a feasibility check "
			"gave no answer; realizability could not be decided" );
	}

	TEST_CASE("both name both, with the options of the cap") {
		ltl_verdict_scope scope;
		ltl_verdict_incomplete = ltl_verdict_undecided = true;
		auto cause = ltl_verdict_unknown_cause();
		REQUIRE( cause.has_value() );
		CHECK( ltl_verdict_unknown_message(*cause)
			== "UNKNOWN: unrealizable only because a feasibility check "
			"gave no answer and a consistency cap skipped checks "
			"(--max-consistency-subsets / --max-cover-products); "
			"realizability could not be decided" );
	}

	TEST_CASE("the outermost scope clears the flags") {
		{
			ltl_verdict_scope scope;
			ltl_verdict_incomplete = ltl_verdict_undecided = true;
		}
		CHECK_FALSE( ltl_verdict_unknown_cause().has_value() );
	}
}
