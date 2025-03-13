// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "doctest.h"

#include "test_integration_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

TEST_SUITE("cpp operators") {

	TEST_CASE("Ohad's example: ex X ( { ex X ( X = {ex Y (Y = 0).}). } = 0).") {
		const char* sample = "ex X ( { ex X ( X = { ( ex Y Y = 0).}). } = 0).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("allowing unresolved rr's in normalization") {

	TEST_CASE("f[0](x).") {
		const char* sample = "f[0](x).";
		auto normalized = normalize_test_tau(sample);
		std::stringstream ss; ss << normalized;
		CHECK( ss.str() == "f[0](x)" );
	}

	TEST_CASE("ex x f[0](x).") {
		const char* sample = "ex x f[0](x).";
		auto normalized = normalize_test_tau(sample);
		std::stringstream ss; ss << normalized;
		CHECK( ss.str() == "ex x f[0](x)" );
	}

	TEST_CASE("ex x f[0](x) && x = 0.") {
		const char* sample = "ex x f[0](x) && x = 0.";
		auto normalized = normalize_test_tau(sample);
		std::stringstream ss; ss << normalized;
		CHECK( ss.str() == "ex x f[0](x) && x = 0" );
	}

	TEST_CASE("ex x f[0](x) && x != 0.") {
		const char* sample = "ex x f[0](x) && x != 0.";
		auto normalized = normalize_test_tau(sample);
		std::stringstream ss; ss << normalized;
		CHECK( ss.str() == "ex x f[0](x) && x != 0" );
	}
}
