// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

TEST_SUITE("simple cases") {

	TEST_CASE("true") {
		CHECK( normalize_and_check("T.", tau::wff_t) );
	}

	TEST_CASE("false") {
		CHECK( normalize_and_check("F.", tau::wff_f) );
	}
}

// TEST_SUITE("rec relations partial evaluation: simple cases") {

// 	TEST_CASE("wff_rec_relation") {
// 		const char* sample =
// 			"h[0](X, Y) := X + Y != 0."
// 			"g[n](Y) := h[n-1](Y, 0)."
// 			"g[1](1).";
// 		CHECK( normalize_and_check(sample, tau::wff_t) );
// 	}

// 	TEST_CASE("bf_rec_relation") {
// 		const char* sample =
// 			"h[0](X, Y) := X + Y."
// 			"g[n](Y) := h[n-1](Y, 0)."
// 			"g[1](0) = 0.";
// 		CHECK( normalize_and_check(sample, tau::wff_t) );
// 	}
// }

// TEST_SUITE("functions partial evaluation: simple cases") {

// 	TEST_CASE("wff_rec_relation") {
// 		const char* sample =
// 			"h(X, Y) := X = Y."
// 			"g(Y) := h(Y, 0)."
// 			"g(0).";
// 		CHECK( normalize_and_check(sample, tau::wff_t) );
// 	}

// 	TEST_CASE("bf_rec_relation") {
// 		const char* sample =
// 			"h(X, Y) := X + Y."
// 			"g(Y) := h(Y, 0)."
// 			"g(1) = 1.";
// 		CHECK( normalize_and_check(sample, tau::wff_t) );
// 	}
// }