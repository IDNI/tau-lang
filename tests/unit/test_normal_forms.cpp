// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "satisfiability.h"
#include "bool_ba.h"
#include "bdd_handle.h"
#include "normalizer.h"

// TODO (LOW) consider move this test to integration tests
#include "test_helpers.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

using namespace idni::rewriter;
using namespace idni::tau;
using namespace std;

namespace testing = doctest;

TEST_SUITE("normal forms: mnf for wffs") {

	TEST_CASE("simple case: T") {
		const char* sample = "T.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		auto result = mnf_wff(statement);
		auto check = result
			| tau_parser::nso_rr
			| tau_parser::nso_main
			| tau_parser::wff
			| tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("simple case: F") {
		const char* sample = "F.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		auto result = mnf_wff(statement);
		auto check = result
			| tau_parser::nso_rr
			| tau_parser::nso_main
			| tau_parser::wff
			| tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("simple case: X = 0") {
		const char* sample = "X = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::nso_rr
			| tau_parser::nso_main
			| tau_parser::wff).value();
		auto result = mnf_wff(statement);
		CHECK( statement == result );
	}

	TEST_CASE("simple case: X != 0") {
		const char* sample = "X != 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::nso_rr
			| tau_parser::nso_main
			| tau_parser::wff).value();
		auto result = mnf_wff(statement);
		auto check_eq = select_all(result, is_non_terminal<tau_parser::bf_eq, Bool>);
		auto check_neg = select_all(result, is_non_terminal<tau_parser::wff_neg, Bool>);
		CHECK( check_eq.size() == 1 );
		CHECK( check_neg.size() == 1 );
	}

	TEST_CASE("simple case: X = 0 && Y = 0") {
		const char* sample = "X = 0 && Y = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::nso_rr
			| tau_parser::nso_main
			| tau_parser::wff).value();
		auto result = mnf_wff(statement);
		auto check_and = select_all(result, is_non_terminal<tau_parser::wff_and, Bool>);
		auto check_eq = select_all(result, is_non_terminal<tau_parser::bf_eq, Bool>);
		CHECK( check_and.size() == 1 );
		CHECK( check_eq.size() == 2 );
	}

	TEST_CASE("simple case: X != 0 && Y != 0") {
		const char* sample = "X != 0 && Y != 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::nso_rr
			| tau_parser::nso_main
			| tau_parser::wff).value();
		auto result = mnf_wff(statement);
		auto check_eq = select_all(result, is_non_terminal<tau_parser::bf_eq, Bool>);
		auto check_neg = select_all(result, is_non_terminal<tau_parser::wff_neg, Bool>);
		auto check_and = select_all(result, is_non_terminal<tau_parser::wff_and, Bool>);
		CHECK( check_eq.size() == 2 );
		CHECK( check_neg.size() == 2 );
		CHECK( check_and.size() == 1 );
	}

	TEST_CASE("simple case: X = 0 || Y = 0") {
		const char* sample = "X = 0 || Y = 0.";
		auto src = make_tau_source(sample);
		auto statement = (make_statement(src)
			| tau_parser::nso_rr
			| tau_parser::nso_main
			| tau_parser::wff).value();
		auto result = mnf_wff(statement);
		auto check_eq = select_all(result, is_non_terminal<tau_parser::bf_eq, Bool>);
		auto check_or = select_all(result, is_non_terminal<tau_parser::wff_or, Bool>);
		CHECK( check_eq.size() == 2 );
		CHECK( check_or.size() == 1 );
	}


}

TEST_SUITE("normal forms: onf") {

	/* TEST_CASE("T") {
		const char* sample = "T.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto nts = std::get<tau_source_sym>(sample_formula.main->value).nts;
		auto var = make_node<tau_sym<bdd_test>>(tau_source_sym(tau_parser::variable, nts), {});
		auto result = onf(sample_formula.main, var);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
		CHECK( true );
	}*/
}

// TODO (MEDIUM) add tests for reduce_bf/wff
// TODO (MEDIUM) add tests for to_bdd_bf
// TODO (MEDIUM) add tests for minimize


// TODO (MEDIUM) add tests for dnf_bf
// TODO (MEDIUM) add tests for dnf_wff
// TODO (MEDIUM) add tests for cnf_bf
// TODO (MEDIUM) add tests for cnf_wff
// TODO (MEDIUM) add tests for nnf_bf
// TODO (MEDIUM) add tests for nnf_bf
// TODO (MEDIUM) add tests for sno_wwf
// TODO (MEDIUM) add tests for sno_wff
// TODO (MEDIUM) add tests for mnn_bf
// TODO (MEDIUM) add tests for mnn_wff
