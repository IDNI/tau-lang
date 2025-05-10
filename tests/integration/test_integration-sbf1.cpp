// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_sbf_ba_helpers.h"

TEST_SUITE("configuration") {
	TEST_CASE("init trace logging") {
		logging::trace();
	}
	TEST_CASE("init bdd") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("formulas: no variables, no bindings and no quantifiers") {

	TEST_CASE("T") {
		CHECK( normalize_and_check("T.", tau::wff_t) );
	}

	TEST_CASE("F") {
		CHECK( normalize_and_check("F.", tau::wff_f) );
	}

	TEST_CASE("T && F") {
		CHECK( normalize_and_check("T && F.", tau::wff_f) );
	}

	TEST_CASE("T ^ F") {
		CHECK( normalize_and_check("T ^ F.", tau::wff_t) );
	}

	TEST_CASE("T ? T : F") {
		CHECK( normalize_and_check("T ? T : F.", tau::wff_t) );
	}

	TEST_CASE("F ? T : F") {
		CHECK( normalize_and_check("F ? T : F.", tau::wff_f) );
	}

	TEST_CASE("F || F") {
		CHECK( normalize_and_check("F || F.", tau::wff_f) );
	}

	TEST_CASE("! F") {
		CHECK( normalize_and_check("! F.", tau::wff_t) );
	}

	TEST_CASE("F -> F") {
		CHECK( normalize_and_check("F -> F.", tau::wff_t) );
	}

	TEST_CASE("F <-> F") {
		CHECK( normalize_and_check("F <-> F.", tau::wff_t) );
	}

	TEST_CASE("1 & 0 = 0.") {
		CHECK( normalize_and_check("1 & 0 = 0.", tau::wff_t) );
	}

	TEST_CASE("1 | 0 = 0.") {
		CHECK( normalize_and_check("1 | 0 = 0.", tau::wff_f) );
	}

	TEST_CASE("1 + 0 != 0.") {
		CHECK( normalize_and_check("1 + 0 != 0.", tau::wff_t) );
	}

	TEST_CASE("0' = 0.") {
		CHECK( normalize_and_check("(0' = 0).", tau::wff_f) );
	}

	TEST_CASE("1' = 0.") {
		CHECK( normalize_and_check("1' = 0.", tau::wff_t) );
	}
}

TEST_SUITE("formulas: variables, no bindings and no quantifiers") {
	// TODO (LOW) write tests for variables
	//
	// This task is tagged as LOW because it is extensively tested inderectly in
	// other unit/integration tests. However, it is better to have explicit tests
	// for it.
}

TEST_SUITE("formulas: no variables, bindings and no quantifiers") {

	// TODO (MEDIUM) check if we need to add more tests for bindings

	TEST_CASE("{ X } : sbf != 0") {
		CHECK( normalize_and_check("{ X } : sbf != 0.", tau::wff_t) );
	}

	TEST_CASE("{ X } : sbf = 0") {
		CHECK( normalize_and_check("{ X } : sbf = 0.", tau::wff_f) );
	}

	TEST_CASE("{ Y } : sbf | { Z } : sbf != 0") {
		CHECK( normalize_and_check("{ Y } : sbf | { Z } : sbf != 0.", tau::wff_t) );
	}

	TEST_CASE("{ Y } : sbf & { Z } : sbf != 0") {
		CHECK( normalize_and_check("{ Y } : sbf | { Z } : sbf != 0.", tau::wff_t) );
	}

	TEST_CASE("{ Y } : sbf' != 0") {
		// TODO (HING) check this one too
		// CHECK( normalize_and_check("{ Y } : sbf' != 0.", tau::wff_t) );
		CHECK( normalize_and_check("({ Y } : sbf)' != 0.", tau::wff_t) );
	}

	TEST_CASE("{ Y } : sbf != 0") {
		CHECK( normalize_and_check("{ Y } : sbf != 0.", tau::wff_t) );
	}
}

TEST_SUITE("formulas: variables, no bindings and quantifiers") {

	TEST_CASE("ex P P != 0") {
		CHECK( normalize_and_check("ex P P != 0.", tau::wff_t) );
	}

	TEST_CASE("ex P P = 0") {
		CHECK( normalize_and_check("ex P P = 0.", tau::wff_t) );
	}

	TEST_CASE("all P P != 0") {
		CHECK( normalize_and_check("all P P != 0.", tau::wff_f) );
	}

	TEST_CASE("all P P = 0") {
		CHECK( normalize_and_check("all P P = 0.", tau::wff_f) );
	}

	TEST_CASE("all X ex Y X = Y") {
		CHECK( normalize_and_check("all X ex Y X = Y.", tau::wff_t) );
	}

	TEST_CASE("ex X all Y X = Y") {
		CHECK( normalize_and_check("ex X all Y X = Y.", tau::wff_f) );
	}
}
