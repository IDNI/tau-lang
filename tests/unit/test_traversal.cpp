// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

TEST_SUITE("operator|") {

	TEST_CASE("match zero nodes") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf));
		CHECK( !(rule | tau::main) );
	}

	TEST_CASE("match one node") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf));
		CHECK( (rule | tau::bf_and).size() == 1 );
	}
}

TEST_SUITE("operator||") {

	TEST_CASE("match zero nodes") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf));
		CHECK( (rule || tau::wff).empty() );
	}

	TEST_CASE("match one node") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf));
		CHECK( (rule || tau::bf_and).size() == 1 );
	}

	TEST_CASE("match several nodes") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf));
		CHECK( (rule | tau::bf_and || tau::bf).size() == 2 );
	}
}
