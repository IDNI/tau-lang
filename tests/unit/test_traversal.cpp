// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "tau.h"
#include "test_Bool_helpers.h"

TEST_SUITE("operator|") {

	TEST_CASE("match zero nodes") {
		const char* sample = "$X & $Y := $Z.";
		auto lib = tt(tau::get(sample, parse_library()));
		CHECK( !(lib | tau::main) );
	}

	TEST_CASE("match one node") {
		const char* sample = "$X & $Y := $Z.";
		auto lib = tt(tau::get(sample, parse_library()));
		CHECK( (lib | tau::rules).size() == 1 );
	}
}

TEST_SUITE("operator||") {

	TEST_CASE("match zero nodes") {
		const char* sample = "$X & $Y := $Z.";
		auto lib = tt(tau::get(sample, parse_library()));
		CHECK( (lib | tau::rules | tau::rule | tau::wff_rule					
						|| tau::wff).empty() );
	}

	TEST_CASE("match one node") {
		const char* sample = "X & Y := Z.";
		auto lib = tt(tau::get(sample, parse_library()));
		CHECK( (lib | tau::rules | tau::rule
						|| tau::bf_rule).size() == 1 );
	}

	TEST_CASE("match several nodes") {
		const char* sample = "X & Y := Z.";
		auto lib = tt(tau::get(sample, parse_library()));
		CHECK( (lib | tau::rules | tau::rule | tau::bf_rule
			| tau::bf_matcher | tau::bf | tau::bf_and
						|| tau::bf).size() == 2 );
	}
}
