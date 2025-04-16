// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "depreciating/boolean_algebras/sbf_ba_depreciating.h"
#include "depreciating/satisfiability_depreciating.h"

// TODO (LOW) consider move this test to integration tests
#include "test_integration_helpers_depreciating.h"

#define base_bas tau_ba<sbf_ba>, sbf_ba

using namespace idni::tau_lang;
using namespace idni::tau_lang_depreciating;
using namespace idni::rewriter::depreciating;

namespace testing = doctest;


template<typename... BAs>
tau_<BAs...> create_spec(const char* spec) {
	auto sample_src = make_tau_source(spec);
	return make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(
		sample_src).value().main;
}

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		idni::tau_lang::bdd_init<Bool>();
	}
}

TEST_SUITE("Alignments") {
	TEST_CASE("equal_lookback_one_st") {
		auto spec = create_spec<base_bas>("(always o1[t-1] = 0) && (sometimes o1[t] = 1 && o1[t-1] = 0).");
		CHECK(!is_tau_formula_sat(spec));
	}
	TEST_CASE("smaller_lookback_one_st") {
		auto spec = create_spec<base_bas>("(always o1[t] = o1[t-1] && o1[t-1] = 1) && (sometimes o2[t] = 0).");
		CHECK(is_tau_formula_sat(spec));
	}
	TEST_CASE("greater_lookback_one_st") {
		auto spec = create_spec<base_bas>("(always o1[t] = o1[t-1]) && (sometimes o1[t] != o1[t-2]).");
		CHECK(!is_tau_formula_sat(spec));
	}
	TEST_CASE("equal_lookback_two_st") {
		auto spec = create_spec<base_bas>("(always o1[t] = 0) && (sometimes o1[t] = 0) && (sometimes o1[t] = 1).");
		CHECK(transform_to_execution(spec) == _F<base_bas>);
	}
	TEST_CASE("greater_lookback_two_st_1") {
		auto spec = create_spec<base_bas>("(always o1[t] = 1 && o2[t] = 1) && (sometimes o1[t-1] = 1) && (sometimes o2[t-2] = 0).");
		CHECK(!is_tau_formula_sat(spec));
	}
	TEST_CASE("greater_lookback_two_st_2") {
		auto spec = create_spec<base_bas>("(always o1[t] = 1 && o2[t] = 1) && (sometimes o1[t-1] = 0) && (sometimes o2[t-2] = 1).");
		CHECK(!is_tau_formula_sat(spec));
	}
	TEST_CASE("smaller_lookback_two_st_1") {
		auto spec = create_spec<base_bas>("(always o1[t-2] = 0 && o2[t-2] = 0) && (sometimes o1[t] = 1) && (sometimes o1[t-1] = 0).");
		CHECK(!is_tau_formula_sat(spec));
	}
	TEST_CASE("smaller_lookback_two_st_2") {
		auto spec = create_spec<base_bas>("(always o1[t-2] = 0 && o2[t-2] = 0) && (sometimes o1[t] = 0) && (sometimes o1[t-1] = 1).");
		CHECK(!is_tau_formula_sat(spec));
	}
#ifndef DEBUG
	TEST_CASE("mixed_lookback_two_st_1") {
		auto spec = create_spec<base_bas>("(always o1[t-2] = 1) && (sometimes o1[t-3] = 0) && (sometimes o1[t] = 1).");
		CHECK(!is_tau_formula_sat(spec));
	}

	TEST_CASE("mixed_lookback_two_st_2") {
		auto spec = create_spec<base_bas>("(always o1[t-2] = 1) && (sometimes o1[t-3] = 1) && (sometimes o1[t] = 0).");
		CHECK(!is_tau_formula_sat(spec));
	}
#endif
}
