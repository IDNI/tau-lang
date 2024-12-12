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

#include "test_integration_helpers-sbf.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

bool normalize_and_test_for_value(const char* sample, tau_parser::nonterminal nt) {
	auto sample_src = make_tau_source(sample);
	auto sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src);
	if (!sample_formula.has_value()) return false;
	auto result = normalizer<sbf_ba>(sample_formula.value());
	auto check = result | nt;
	return check.has_value();
}

// TODO (LOW) simplify this test cases extracting common logic to the helpers file

TEST_SUITE("formulas: no variables, no bindings and no quantifiers") {

	TEST_CASE("T") {
		CHECK( normalize_and_test_for_value("T.", tau_parser::wff_t) );
	}

	TEST_CASE("F") {
		CHECK( normalize_and_test_for_value("F.", tau_parser::wff_f) );
	}

	TEST_CASE("T && F") {
		CHECK( normalize_and_test_for_value("T && F.", tau_parser::wff_f) );
	}

	TEST_CASE("T ^ F") {
		CHECK( normalize_and_test_for_value("T ^ F.", tau_parser::wff_t) );
	}

	TEST_CASE("T ? T : F") {
		CHECK( normalize_and_test_for_value("T ? T : F.", tau_parser::wff_t) );
	}

	TEST_CASE("F ? T : F") {
		CHECK( normalize_and_test_for_value("F ? T : F.", tau_parser::wff_f) );
	}

	TEST_CASE("F || F") {
		CHECK( normalize_and_test_for_value("F || F.", tau_parser::wff_f) );
	}

	TEST_CASE("! F") {
		CHECK( normalize_and_test_for_value("! F.", tau_parser::wff_t) );
	}

	TEST_CASE("F -> F") {
		CHECK( normalize_and_test_for_value("F -> F.", tau_parser::wff_t) );
	}

	TEST_CASE("F <-> F") {
		CHECK( normalize_and_test_for_value("F <-> F.", tau_parser::wff_t) );
	}

	TEST_CASE("1 & 0 = 0.") {
		CHECK( normalize_and_test_for_value("1 & 0 = 0.", tau_parser::wff_t) );
	}

	TEST_CASE("1 | 0 = 0.") {
		CHECK( normalize_and_test_for_value("1 | 0 = 0.", tau_parser::wff_f) );
	}

	TEST_CASE("1 + 0 != 0.") {
		CHECK( normalize_and_test_for_value("1 + 0 != 0.", tau_parser::wff_t) );
	}

	TEST_CASE("0' = 0.") {
		CHECK( normalize_and_test_for_value("(0' = 0).", tau_parser::wff_f) );
	}

	TEST_CASE("1' = 0.") {
		CHECK( normalize_and_test_for_value("1' = 0.", tau_parser::wff_t) );
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
		bdd_init<Bool>();
		CHECK( normalize_and_test_for_value("{ X } : sbf != 0.", tau_parser::wff_t) );
	}

	TEST_CASE("{ X } : sbf = 0") {
		bdd_init<Bool>();
		CHECK( normalize_and_test_for_value("{ X } : sbf = 0.", tau_parser::wff_f) );
	}

	TEST_CASE("{ Y } : sbf | { Z } : sbf != 0") {
		bdd_init<Bool>();
		CHECK( normalize_and_test_for_value("{ Y } : sbf | { Z } : sbf != 0.", tau_parser::wff_t) );
	}

	TEST_CASE("{ Y } : sbf & { Z } : sbf != 0") {
		bdd_init<Bool>();
		CHECK( normalize_and_test_for_value("{ Y } : sbf | { Z } : sbf != 0.", tau_parser::wff_t) );
	}

	TEST_CASE("{ Y } : sbf' != 0") {
		bdd_init<Bool>();
		// TODO (HING) check this one too
		// CHECK( normalize_and_test_for_value("{ Y } : sbf' != 0.", tau_parser::wff_t) );
		CHECK( normalize_and_test_for_value("({ Y } : sbf)' != 0.", tau_parser::wff_t) );
	}

	TEST_CASE("{ Y } : sbf != 0") {
		bdd_init<Bool>();
		CHECK( normalize_and_test_for_value("{ Y } : sbf != 0.", tau_parser::wff_t) );
	}
}

TEST_SUITE("formulas: variables, no bindings and quantifiers") {

	TEST_CASE("ex P P != 0") {
		CHECK( normalize_and_test_for_value("ex P P != 0.", tau_parser::wff_t) );
	}

	TEST_CASE("ex P P = 0") {
		CHECK( normalize_and_test_for_value("ex P P = 0.", tau_parser::wff_t) );
	}

	TEST_CASE("all P P != 0") {
		CHECK( normalize_and_test_for_value("all P P != 0.", tau_parser::wff_f) );
	}

	TEST_CASE("all P P = 0") {
		CHECK( normalize_and_test_for_value("all P P = 0.", tau_parser::wff_f) );
	}

	TEST_CASE("all X ex Y X = Y") {
		CHECK( normalize_and_test_for_value("all X ex Y X = Y.", tau_parser::wff_t) );
	}

	TEST_CASE("ex X all Y X = Y") {
		CHECK( normalize_and_test_for_value("ex X all Y X = Y.", tau_parser::wff_f) );
	}
}
