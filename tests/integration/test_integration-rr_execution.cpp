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

#include "../../src/doctest.h"
#include "../../src/nso_rr.h"
#include "../../src/bool.h"
#include "../../src/bdd_handle.h"
#include "../../src/normalizer2.h"

#include "test_integration_helpers-bdd.h"
#include "../unit/test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("rec relations execution") {
	// TODO (HIGH) writes tests for recursive relations execution
	// The tests should range from simple direct substitutions to more complex
	// substitutions (involving multiple variables and indexes).

	TEST_CASE("T") {
		const char* sample = "T.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula).main;
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}


	TEST_CASE("nso_rr wff_rec_relation") {
		const char* sample =
			"g[0](Y) ::= T."
			" T .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::nso_rr | tau_parser::rec_relations | tau_parser::rec_relation | tau_parser::wff_rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}
}
