// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("cpp operators") {

	TEST_CASE("Ohad's example: ex X ( { ex X ( X = {ex Y (Y = 0).}). } = 0).") {
		const char* sample = "ex X ( { ex X ( X = { ( ex Y Y = 0).}). } = 0).";
		CHECK( normalize_and_check(sample, tau::wff_f) );
	}
}

TEST_SUITE("allowing unresolved rr's in normalization") {

	TEST_CASE("f[0](x).") {
		const char* sample = "f[0](x).";
		CHECK( normalize_and_check(sample, "f[0](x)") );
	}

	TEST_CASE("ex x f[0](x).") {
		const char* sample = "ex x f[0](x).";
		CHECK( normalize_and_check(sample, "ex b1 f[0](b1)") );
	}

	TEST_CASE("ex x f[0](x) && x = 0.") {
		const char* sample = "ex x f[0](x) && x = 0.";
		CHECK( normalize_and_check(sample, "ex b1 b1 = 0 && f[0](b1)") );
	}

	TEST_CASE("ex x f[0](x) && x != 0.") {
		const char* sample = "ex x f[0](x) && x != 0.";
		CHECK( normalize_and_check(sample, "ex b1 f[0](b1) && b1 != 0") );
	}
}
