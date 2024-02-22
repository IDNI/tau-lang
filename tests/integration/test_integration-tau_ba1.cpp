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

#include <cassert>

#include "../../src/doctest.h"
#include "../../src/nso_rr.h"
#include "../../src/bdd_handle.h"
#include "../../src/normalizer2.h"
#include "../../src/tau.h"

#include "test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("formulas: one level rec, no variables, no bindings and no quantifiers") {

	TEST_CASE("{ : F. } = 0") {
		const char* sample = "( { : F. } = 0 ).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized.main | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ : T. } = 0") {
		const char* sample = "( { : T. } = 0 ).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized.main | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ : F. } != 0") {
		const char* sample = "( { : F. } != 0 ).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized.main | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ : T. } != 0") {
		const char* sample = "( { : T. } != 0 ).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized.main | tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("formulas: two level rec, no variables, no bindings and no quantifiers") {

	TEST_CASE("( { : ({ : F.} = 0). } = 0 ).") {
		const char* sample = "( { : ({ : F.} = 0). } = 0 ).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized.main | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("({ : ({ : T.} = 0). } = 0).") {
		const char* sample = "({ : ({ : T.} = 0). } = 0).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized.main | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("({ : ({ : F.} = 0). } != 0).") {
		const char* sample = "({ : ({ : F.} = 0). } != 0).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized.main | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("( { : ({ : T.} = 0). } != 0 ).") {
		const char* sample = "( { : ({ : T.} = 0). } != 0 ).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized.main | tau_parser::wff_f;
		CHECK( check.has_value() );
	}
}
