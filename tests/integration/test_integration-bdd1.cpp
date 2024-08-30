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

#include "test_integration_helpers-bdd.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

// TODO (LOW) simplify this test cases extracting common logic to the helpers file

TEST_SUITE("formulas: no variables, no bindings and no quantifiers") {

	TEST_CASE("T") {
		const char* sample = "T.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("F") {
		const char* sample = "F.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("T && F") {
		const char* sample = "T && F.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("T ^ F") {
		const char* sample = "T ^ F.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("T ? T : F") {
		const char* sample = "T ? T : F.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("F ? T : F") {
		const char* sample = "F ? T : F.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("F || F") {
		const char* sample = "F || F.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("! F") {
		const char* sample = "! F.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("F -> F") {
		const char* sample = "F -> F.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("F <-> F") {
		const char* sample = "F <-> F.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("1 & 0 = 0.") {
		const char* sample = "1 & 0 = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("1 | 0 = 0.") {
		const char* sample = "1 | 0 = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("1 + 0 != 0.") {
		const char* sample = "1 + 0 != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("0' = 0.") {
		const char* sample = "(0' = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("1' = 0.") {
		const char* sample = "1' = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
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

	TEST_CASE("{ bdd : X } != 0") {
		bdd_init<Bool>();
		const char* sample = "{ bdd : X } != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ bdd : X } = 0") {
		bdd_init<Bool>();
		const char* sample = "{ bdd : X } = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ bdd : Y } | { bdd : Z } != 0") {
		bdd_init<Bool>();
		const char* sample = "{ bdd : Y } | { bdd : Z } != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ bdd : Y } & { bdd : Z } != 0") {
		bdd_init<Bool>();
		const char* sample = "{ bdd : Y } & { bdd : Z } != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ bdd : Y }' != 0") {
		bdd_init<Bool>();
		const char* sample = "{ bdd : Y }' != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("{ bdd : Y } != 0") {
		bdd_init<Bool>();
		const char* sample = "{ bdd : Y } != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("formulas: variables, no bindings and quantifiers") {

	TEST_CASE("ex P P != 0") {
		const char* sample = "ex P P != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("ex P P = 0") {
		const char* sample = "ex P P = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("all P P != 0") {
		const char* sample = "all P P != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("all P P = 0") {
		const char* sample = "all P P = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("all X ex Y X = Y") {
		const char* sample = "all X ex Y X = Y.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("ex X all Y X = Y") {
		const char* sample = "ex X all Y X = Y.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}
}
