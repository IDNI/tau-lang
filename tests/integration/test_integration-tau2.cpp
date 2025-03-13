// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "doctest.h"
#include "satisfiability.h"

#include "test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

TEST_SUITE("simple tau formulas: no negatives, no loopbacks, no vars") {

	TEST_CASE("{(0 = 0)}") {
		const char* sample = "{(0 = 0)};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{(1 = 0)}") {
		const char* sample = "{(1 = 0)};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}
}

TEST_SUITE("simple tau formulas: no negatives, no loopbacks") {

	TEST_CASE("{(i1[t] = o1[t])}") {
		const char* sample = "{ ( i1[t] = o1[t] ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{(i1[t] = i1[t]')}") {
		const char* sample = "{ ( i1[t] = i1[t]' ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}
}

TEST_SUITE("simple tau formulas: no negatives") {

	TEST_CASE("{(i1[t-1] = o1[t])}") {
		const char* sample = "{ ( i1[t-1] = o1[t] ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{(i1[t-1] = i1[t-1]')}") {
		const char* sample = "{ ( i1[t-1] = i1[t-1]' ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}
}

TEST_SUITE("simple tau formulas: negatives, no loopbacks") {

	TEST_CASE("{(i1[t] != o1[t])}") {
		const char* sample = "{ ( i1[t] != o1[t] ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{(i1[t] != i1[t])}") {
		const char* sample = "{ ( i1[t] != i1[t] ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}
}

TEST_SUITE("simple tau formulas: negatives") {

	TEST_CASE("{(i1[t-1] != o1[t])}") {
		const char* sample = "{ ( i1[t-1] != o1[t] ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{(i1[t-1] != i1[t-1])}") {
		const char* sample = "{ ( i1[t-1] != i1[t-1] ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}
}

TEST_SUITE("simple tau formulas: negatives, positive, no loopbacks") {

	//TODO (HIGH) implement the following tests properly

	TEST_CASE("{(i1[t-1] != o1[t])}") {
		const char* sample = "{ ( i1[t-1] != o1[t] ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{(i1[t-1] != i1[t-1])}") {
		const char* sample = "{ ( i1[t-1] != i1[t-1] ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}
}

TEST_SUITE("simple tau formulas: negatives, positive, loopbacks") {

	//TODO (HIGH) implement the following tests properly

	TEST_CASE("{(i1[t-1] != o1[t])}") {
		const char* sample = "{ ( i1[t-1] != o1[t] ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}

	TEST_CASE("{(i1[t-1] != i1[t-1])}") {
		const char* sample = "{ ( i1[t-1] != i1[t-1] ) };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<sbf_ba>(sample_formula) );
	}
}
