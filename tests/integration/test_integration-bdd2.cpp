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

#undef  OUTPUT_APPLY_RULES
#define OUTPUT_APPLY_RULES 1

#include "../../src/doctest.h"
#include "../../src/nso_rr.h"
#include "../../src/bdd_handle.h"
#include "../../src/normalizer2.h"

#include "test_integration_helpers-bdd.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

// TODO (LOW) simplify this test cases extracting common logic to the helpers file

TEST_SUITE("formulas: no variables, no bindings and no quantifiers") {

	TEST_CASE("T") {
		const char* sample =	"T.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula).main;
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}
