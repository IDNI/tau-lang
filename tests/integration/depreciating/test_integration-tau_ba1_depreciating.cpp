// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentiTd cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "test_integration_helpers_depreciating.h"

TEST_SUITE("operators: negation") {

	TEST_CASE("{ F. }' = 0") {
		const char* sample = " { F. }' = 0 .";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ T. }' = 0") {
		const char* sample = " { T. }' = 0 .";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

// TODO (HIGH) add tests for is_zero
// TODO (HIGH) add tests for is_one

TEST_SUITE("operators: conjunction") {

	TEST_CASE("{ T. } & { T. } = 0") {
		const char* sample = "{ T. } & { T. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ F. } & { T. } = 0") {
		const char* sample = "{ F. } & { T. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ T. } & { F. } = 0") {
		const char* sample = "{ T. } & { F. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ F. } & { F. } = 0") {
		const char* sample = "{ F. } & { F. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("operators: disjunction") {

	TEST_CASE("{ T. } | { T. } = 0") {
		const char* sample = "{ T. } | { T. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ F. } | { T. } = 0") {
		const char* sample = "{ F. } | { T. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ T. } | { F. } = 0") {
		const char* sample = "{ T. } | { F. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ F. } | { F. } = 0") {
		const char* sample = "{ F. } | { F. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("operators: exclusive or") {

	TEST_CASE("{ T. } + { T. } = 0") {
		const char* sample = "{ T. } + { T. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ F. } + { T. } = 0") {
		const char* sample = "{ F. } + { T. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ T. } + { F. } = 0") {
		const char* sample = "{ T. } + { F. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ F. } + { F. } = 0") {
		const char* sample = "{ F. } + { F. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("formulas: one level rec, no variables, no bindings and no quantifiers") {

	TEST_CASE("{ F. } = 0") {
		const char* sample = "{ F. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ T. } = 0") {
		const char* sample = "{ T. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ F. } != 0") {
		const char* sample = "{ F. } != 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ T. } != 0") {
		const char* sample = "{ T. } != 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("formulas: two level rec, no variables, no bindings and no quantifiers") {

	TEST_CASE("{ { F. } = 0. } = 0") {
		const char* sample = "{ { F. } = 0. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ { T. } = 0. } = 0") {
		const char* sample = "{ { T. } = 0. } = 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ { F.} = 0. } != 0") {
		const char* sample = "{ { F. } = 0. } != 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ { T. } = 0. } != 0") {
		const char* sample = "{ { T. } = 0. } != 0.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("regression/bf_and not trimmed") {

	// The problem was related to not trimming the bf_and_sym in the grammar.
	// We could write the same formula in different ways and it would be
	// process differently

	TEST_CASE("ab = ba") {
		const char* sample = "ab = ba.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("ab = (b & a)") {
		const char* sample = "ab = (b & a).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("(a & b) = ba") {
		const char* sample = "(a & b) = ba.";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("(a & b) = (b & a)") {
		const char* sample = "(a & b) = (b & a).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("((a & b) = (b & a))") {
		const char* sample = "((a & b) = (b & a)).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}
