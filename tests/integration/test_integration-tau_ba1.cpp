// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("operators: negation") {

	TEST_CASE("{ F. }' = 0") {
		const char* sample = " { F. }' = 0 .";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ T. }' = 0") {
		const char* sample = " { T. }' = 0 .";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}

TEST_SUITE("tau_ba predicates") {

	TEST_CASE("is_zero detects contradiction") {
		tau_ba<bv, sbf_ba> fm(get_nso_rr("x = 0 && x != 0.")->main->get());
		CHECK(fm.is_zero());
		CHECK(!(fm == true));
	}

	TEST_CASE("is_one detects tautology") {
		tau_ba<bv, sbf_ba> fm(get_nso_rr("x = 0 || x != 0.")->main->get());
		CHECK(fm.is_one());
		CHECK(fm == true);
	}
}

TEST_SUITE("operators: conjunction") {

	TEST_CASE("{ T. } & { T. } = 0") {
		const char* sample = "{ T. } & { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ F. } & { T. } = 0") {
		const char* sample = "{ F. } & { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ T. } & { F. } = 0") {
		const char* sample = "{ T. } & { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ F. } & { F. } = 0") {
		const char* sample = "{ F. } & { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}

TEST_SUITE("operators: disjunction") {

	TEST_CASE("{ T. } | { T. } = 0") {
		const char* sample = "{ T. } | { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ F. } | { T. } = 0") {
		const char* sample = "{ F. } | { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ T. } | { F. } = 0") {
		const char* sample = "{ T. } | { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ F. } | { F. } = 0") {
		const char* sample = "{ F. } | { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}

TEST_SUITE("operators: exclusive or") {

	TEST_CASE("{ T. } ^ { T. } = 0") {
		const char* sample = "{ T. } ^ { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ F. } ^ { T. } = 0") {
		const char* sample = "{ F. } ^ { T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ T. } ^ { F. } = 0") {
		const char* sample = "{ T. } ^ { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ F. } ^ { F. } = 0") {
		const char* sample = "{ F. } ^ { F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}

TEST_SUITE("formulas: one level rec, no variables, no bindings and no quantifiers") {

	TEST_CASE("{ F. } = 0") {
		const char* sample = "{ F. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ T. } = 0") {
		const char* sample = "{ T. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ F. } != 0") {
		const char* sample = "{ F. } != 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ T. } != 0") {
		const char* sample = "{ T. } != 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}

TEST_SUITE("formulas: two level rec, no variables, no bindings and no quantifiers") {

	TEST_CASE("{ { F. } = 0. } = 0") {
		const char* sample = "{ { F. } = 0. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}

	TEST_CASE("{ { T. } = 0. } = 0") {
		const char* sample = "{ { T. } = 0. } = 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ { F.} = 0. } != 0") {
		const char* sample = "{ { F. } = 0. } != 0.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("{ { T. } = 0. } != 0") {
		const char* sample = "{ { T. } = 0. } != 0.";
		CHECK (normalize_and_check(sample, tau::wff_f));
	}
}

TEST_SUITE("regression/bf_and not trimmed") {

	// The problem was related to not trimming the bf_and_sym in the grammar.
	// We could write the same formula in different ways and it would be
	// process differently

	TEST_CASE("ab = ba") {
		const char* sample = "ab = ba.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("ab = (b & a)") {
		const char* sample = "ab = (b & a).";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("(a & b) = ba") {
		const char* sample = "(a & b) = ba.";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("(a & b) = (b & a)") {
		const char* sample = "(a & b) = (b & a).";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}

	TEST_CASE("((a & b) = (b & a))") {
		const char* sample = "((a & b) = (b & a)).";
		CHECK (normalize_and_check(sample, tau::wff_t));
	}
}
