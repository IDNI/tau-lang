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
#include "normalizer2.h"
#include "tau.h"

#include "../integration/test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;
TEST_SUITE("applying defs once rules") {

	// The problem was an extra rule in apply_defs_once that was not needed and
	// caused several problems during matchin. The rule was BF_ELIM_PARENTHESIS.
	// The test should execxute the normalization without issues

	TEST_CASE("X & (Y | Z) = 0") {
		const char* sample = "X & (Y | Z) = 0.";
		auto normalized = normalize_test_tau(sample);
		CHECK( true );
	}

	// We add several extra test cases to check the remaining operators that
	// could happen while applying once definitions.

	TEST_CASE("X & (Y | Z) != 0") {
		const char* sample = "X & (Y | Z) != 0.";
		auto normalized = normalize_test_tau(sample);
		CHECK( true );
	}

	TEST_CASE("X & (Y | Z) > 0") {
		const char* sample = "X & (Y | Z) > 0.";
		auto normalized = normalize_test_tau(sample);
		CHECK( true );
	}

	// TODO (MEDIUM) check result, could we normalize this to F? use a rule?
	TEST_CASE("X & (Y | Z) < 0") {
		const char* sample = "X & (Y | Z) < 0.";
		auto normalized = normalize_test_tau(sample);
		CHECK( true );
	}

	TEST_CASE("X & (Y | Z) <= 0") {
		const char* sample = "X & (Y | Z) <= 0.";
		auto normalized = normalize_test_tau(sample);
		CHECK( true );
	}

	// And also other possible constants.

	TEST_CASE("X & (Y | Z) = 1") {
		const char* sample = "X & (Y | Z) = 1.";
		auto normalized = normalize_test_tau(sample);
		CHECK( true );
	}

	TEST_CASE("X & (Y | Z) != 1") {
		const char* sample = "X & (Y | Z) != 1.";
		auto normalized = normalize_test_tau(sample);
		CHECK( true );
	}

	// TODO (MEDIUM) check result, could we normalize this to F? use a rule?
	TEST_CASE("X & (Y | Z) > 1") {
		const char* sample = "X & (Y | Z) > 1.";
		auto normalized = normalize_test_tau(sample);
		CHECK( true );
	}

	TEST_CASE("X & (Y | Z) < 1") {
		const char* sample = "X & (Y | Z) < 1.";
		auto normalized = normalize_test_tau(sample);
		CHECK( true );
	}

	TEST_CASE("X & (Y | Z) <= 1") {
		const char* sample = "X & (Y | Z) <= 1.";
		auto normalized = normalize_test_tau(sample);
		CHECK( true );
	}
}
