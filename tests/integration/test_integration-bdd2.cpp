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
#include "bdd_binding.h"

#include "test_integration_helpers-bdd.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

// TODO (LOW) simplify this test cases extracting common logic to the helpers file

TEST_SUITE("formulas: no variables, no bindings and no quantifiers") {

	TEST_CASE("i1[t] = o1[t]") {
		const char* sample = "( i1[t] = o1[t] ).";
		auto sample_src = make_tau_source(sample);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(
						sample_src);
		CHECK( sample_formula.has_value() );
		if (!sample_formula.has_value()) return;
		auto result = normalizer<bdd_binding>(sample_formula.value());
		auto check1 = result |  tau_parser::wff_f;
		auto check2 = result |  tau_parser::wff_t;
		CHECK( !check1.has_value() );
		CHECK( !check2.has_value() );
	}
}
