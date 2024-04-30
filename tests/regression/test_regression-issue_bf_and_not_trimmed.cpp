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

#include "doctest.h"
#include "nso_rr.h"
#include "bdd_handle.h"
#include "normalizer.h"
#include "tau.h"

#include "../integration/test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;
TEST_SUITE("bf_and not trimmed") {

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