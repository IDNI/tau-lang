// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "test_integration_helpers.h"

TEST_SUITE("simple cases") {

	TEST_CASE("true") {
		CHECK( check_normalized("T.", tau::wff_t) );
	}

	TEST_CASE("false") {
		CHECK( check_normalized("F.", tau::wff_f) );
	}
}

// TEST_SUITE("rec relations partial evaluation: simple cases") {

// 	TEST_CASE("wff_rec_relation") {
// 		const char* sample =
// 			"h[0](X, Y) := X + Y != 0."
// 			"g[n](Y) := h[n-1](Y, 0)."
// 			"g[1](1).";
// 		CHECK( check_normalized(sample, tau::wff_t) );
// 	}

// 	TEST_CASE("bf_rec_relation") {
// 		const char* sample =
// 			"h[0](X, Y) := X + Y."
// 			"g[n](Y) := h[n-1](Y, 0)."
// 			"g[1](0) = 0.";
// 		CHECK( check_normalized(sample, tau::wff_t) );
// 	}
// }

// TEST_SUITE("functions partial evaluation: simple cases") {

// 	TEST_CASE("wff_rec_relation") {
// 		const char* sample =
// 			"h(X, Y) := X = Y."
// 			"g(Y) := h(Y, 0)."
// 			"g(0).";
// 		CHECK( check_normalized(sample, tau::wff_t) );
// 	}

// 	TEST_CASE("bf_rec_relation") {
// 		const char* sample =
// 			"h(X, Y) := X + Y."
// 			"g(Y) := h(Y, 0)."
// 			"g(1) = 1.";
// 		CHECK( check_normalized(sample, tau::wff_t) );
// 	}
// }