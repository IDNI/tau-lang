// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "doctest.h"

#include "test_integration_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

// TODO (LOW) simplify this test cases extracting common logic to the helpers file

TEST_SUITE("formulas: no variables, no bindings and no quantifiers") {

	TEST_CASE("i1[t] = o1[t]") {
		const char* sample = "( i1[t] = o1[t] ).";
		auto sample_src = make_tau_source(sample);
		auto sample_formula = make_nso_rr_using_factory<sbf_ba>(
						sample_src);
		CHECK( sample_formula.has_value() );
		if (!sample_formula.has_value()) return;
		auto result = normalizer<sbf_ba>(sample_formula.value());
		auto check1 = result |  tau_parser::wff_f;
		auto check2 = result |  tau_parser::wff_t;
		CHECK( !check1.has_value() );
		CHECK( !check2.has_value() );
	}
}
