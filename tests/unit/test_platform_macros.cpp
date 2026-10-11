// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Names that platform headers define as macros cannot be identifiers here:
// the macOS SDK defines TRUE and FALSE (mach/boolean.h). This file compiles
// the library's headers with those macros set, as a macOS build does.
#define TRUE 1
#define FALSE 0

#include "test_init.h"
#include "test_tau_helpers.h"
#include "satisfiability.h"

TEST_SUITE("platform macros") {

	TEST_CASE("the headers compile with TRUE and FALSE defined as macros") {
		CHECK(TRUE == 1);
		CHECK(FALSE == 0);
	}
}
