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

#include "doctest.h"
#include "nso_rr.h"
#include "bdd_handle.h"
#include "normalizer.h"
#include "satisfiability.h"

#include "test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("cpp operators") {

	TEST_CASE("Ohad's example: ex X ( { : ( X = { : (Y = 0).}). } = 0).") {
		const char* sample = "ex X ( { : ( X = { : (Y = 0).}). } = 0).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("allowing bool vars and unresolved rr's in normalization") {

	TEST_CASE("?y.") {
		const char* sample = "?y.";
		auto normalized = normalize_test_tau(sample);
		CHECK( normalized == "always ?y" );
	}

	TEST_CASE("f[0](x).") {
		const char* sample = "f[0](x).";
		auto normalized = normalize_test_tau(sample);
		CHECK( normalized == "always f[0](x)" );
	}

	TEST_CASE("ex x ?y.") {
		const char* sample = "ex x ?y.";
		auto normalized = normalize_test_tau(sample);
		CHECK( normalized == "always ?y" );
	}

	TEST_CASE("ex x f[0](x).") {
		const char* sample = "ex x f[0](x).";
		auto normalized = normalize_test_tau(sample);
		CHECK( normalized == "always f[0](x)" );
	}

	TEST_CASE("ex x f[0](x) && x = 0.") {
		const char* sample = "ex x f[0](x) && x = 0.";
		auto normalized = normalize_test_tau(sample);
		CHECK( normalized == "always f[0](x)" );
	}

	TEST_CASE("ex x f[0](x) && x != 0.") {
		const char* sample = "ex x f[0](x) && x != 0.";
		auto normalized = normalize_test_tau(sample);
		CHECK( normalized == "always f[0](x)" );
	}

	TEST_CASE("ex x ?y && x = 0.") {
		const char* sample = "ex x ?y && x = 0.";
		auto normalized = normalize_test_tau(sample);
		CHECK( normalized == "always ?y" );
	}

	TEST_CASE("ex x ?y && x != 0.") {
		const char* sample = "ex x ?y && x != 0.";
		auto normalized = normalize_test_tau(sample);
		CHECK( normalized == "always ?y" );
	}
}
