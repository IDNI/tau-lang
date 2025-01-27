// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../integration/test_integration_helpers.h"
#include "satisfiability.h"

namespace testing = doctest;

using namespace idni::rewriter;
using namespace idni::tau_lang;
using namespace std;

TEST_SUITE("SAT") {
	TEST_CASE("!always_contradiction") {
		const char* sample = "!(always o1[t] = i1[t]) && (always o1[t] = i2[t]).";
		auto sample_src = make_tau_source(sample);
		auto sample_formula = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(sample_src);
		auto result = normalizer<tau_ba<sbf_ba>, sbf_ba>(sample_formula.value());
		CHECK(!is_tau_formula_sat(result));
	}
        TEST_CASE("!sometimes_contradiction") {
		const char* sample = "!(sometimes o1[t] = i1[t]) && (sometimes o1[t] = i2[t]).";
		auto sample_src = make_tau_source(sample);
		auto sample_formula = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(sample_src);
		auto result = normalizer<tau_ba<sbf_ba>, sbf_ba>(sample_formula.value());
		CHECK(!is_tau_formula_sat(result));
	}
        TEST_CASE("!always_sometimes_sat") {
		const char* sample = "!(always o1[t] = i1[t]) && (sometimes o1[t] = i1[t]).";
		auto sample_src = make_tau_source(sample);
		auto sample_formula = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(sample_src);
		auto result = normalizer<tau_ba<sbf_ba>, sbf_ba>(sample_formula.value());
		CHECK(is_tau_formula_sat(result));
	}
        TEST_CASE("!sometimes_always_contradiction") {
		const char* sample = "!(sometimes o1[t] = i1[t]) && (always o1[t] = i1[t]).";
		auto sample_src = make_tau_source(sample);
		auto sample_formula = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(sample_src);
		auto result = normalizer<tau_ba<sbf_ba>, sbf_ba>(sample_formula.value());
		CHECK(!is_tau_formula_sat(result));
	}
}
