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
#include "normal_forms.h"

#include "test_integration_helpers-bdd.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("normal forms: onf") {

	TEST_CASE("T") {
		const char* sample = "T.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto nts = std::get<tau_source_sym>(sample_formula.main->value).nts;
		auto var = make_node<tau_sym<bdd_test>>(tau_source_sym(tau_parser::var, nts), {});
		auto result = onf(sample_formula.main, var);
		auto check = result | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("F") {
		const char* sample = "F.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf);
		auto nts = std::get<tau_source_sym>(sample_formula.main->value).nts;
		auto var = make_node<tau_sym<bdd_test>>(tau_source_sym(tau_parser::var, nts), {});
		auto result = onf(sample_formula.main, var);
		auto check = result | tau_parser::wff_f;
		CHECK( check.has_value() );
	}
}

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
