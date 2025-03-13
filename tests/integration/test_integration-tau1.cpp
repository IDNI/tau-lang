// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "doctest.h"
#include "satisfiability.h"

#include "test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

TEST_SUITE("simple tau formulas: no variables") {

	TEST_CASE("{T}") {
		const char* sample = "{T};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{F}") {
		const char* sample = "{F};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{T} &&& {T}") {
		const char* sample = "{T} &&& {T};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{F} &&& {F}") {
		const char* sample = "{F} &&& {F};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{T} &&& {F}") {
		const char* sample = "{T} &&& {F};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{F} &&& {T}") {
		const char* sample = "{F} &&& {T};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{T} ||| {T}") {
		const char* sample = "{T} ||| {T};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{F} ||| {F}") {
		const char* sample = "{F} ||| {F};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{T} ||| {F}") {
		const char* sample = "{T} ||| {F};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{F} ||| {T}") {
		const char* sample = "{F} ||| {T};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("- {F}") {
		const char* sample = "- {F};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("- {T}") {
		const char* sample = "- {T};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory btf;
		tau_factory<sbf_ba_factory,  sbf_ba> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<sbf_ba_factory,  sbf_ba>, sbf_ba>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}
}
