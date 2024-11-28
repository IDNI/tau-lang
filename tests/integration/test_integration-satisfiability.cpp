// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

// TODO (LOW) consider move this test to integration tests
#include "../integration/test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

using namespace idni::rewriter;
using namespace idni::tau;
using namespace std;

namespace testing = doctest;

TEST_SUITE("get_gssotc_literals") {

	TEST_CASE("one literal y1") {
		const char* sample = "{ T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		get_gssotc_literals<sbf_ba>(sample_formula.main, literals);
		CHECK( literals.size() == 1 );
	}

	TEST_CASE("one literal y2") {
		const char* sample = "{ T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		get_gssotc_literals<sbf_ba>(sample_formula.main, literals);
		CHECK( literals.size() == 1 );
	}

	TEST_CASE("two literals y1") {
		const char* sample = "{ T } &&& - { T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		get_gssotc_literals<sbf_ba>(sample_formula.main, literals);
		CHECK( literals.size() == 2 );
	}

	TEST_CASE("two literals y2") {
		const char* sample = "- { T } &&& - { T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		get_gssotc_literals<sbf_ba>(sample_formula.main, literals);
		CHECK( literals.size() == 2 );
	}
}

TEST_SUITE("get_gssotc_positive_negative_literals") {

	TEST_CASE("one literal: true") {
		const char* sample = "{ T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<sbf_ba>(sample_formula.main);
		CHECK( positive.has_value() );
		CHECK( negatives.size() == 0 );
	}

	TEST_CASE("one literal: false") {
		const char* sample = "- { T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<sbf_ba>(sample_formula.main);
		CHECK( !positive.has_value() );
		CHECK( negatives.size() == 1 );
	}

	TEST_CASE("two literals: true") {
		const char* sample = "{ T } &&& - { T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<sbf_ba>(sample_formula.main);
		CHECK( positive.has_value() );
		CHECK( negatives.size() == 1 );
	}

	TEST_CASE("two literals: false") {
		const char* sample = "- { T } &&& - { T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<sbf_ba>(sample_formula.main);
		CHECK( !positive.has_value() );
		CHECK( negatives.size() == 2 );
	}
}

TEST_SUITE("get_gssotc_clauses") {

	TEST_CASE("one clause") {
		const char* sample = "{ T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		get_gssotc_clauses<sbf_ba>(sample_formula.main, literals);
		CHECK( literals.size() == 1 );
	}

	TEST_CASE("two clauses y1") {
		const char* sample = "{ T } ||| - { T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		get_gssotc_clauses<sbf_ba>(sample_formula.main, literals);
		CHECK( literals.size() == 2 );
	}

	TEST_CASE("two clauses y2") {
		const char* sample = "{ T } ||| - { T } &&& { T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		get_gssotc_clauses<sbf_ba>(sample_formula.main, literals);
		CHECK( literals.size() == 2 );
	}

	TEST_CASE("three clauses") {
		const char* sample = "{ T } ||| - { T } ||| { T};";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		get_gssotc_clauses<sbf_ba>(sample_formula.main, literals);
		CHECK( literals.size() == 3 );
	}
}

TEST_SUITE("get_gssotc_io_vars") {

	TEST_CASE("none") {
		const char* sample = "{ T };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		auto [inputs, outputs] = get_gssotc_io_vars<sbf_ba>(sample_formula.main);
		CHECK( (inputs.name.size() == 0 && outputs.name.size() == 0) );
	}

	TEST_CASE("one input") {
		const char* sample = "{ i1[t] = 0 };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		auto [inputs, outputs] = get_gssotc_io_vars<sbf_ba>(sample_formula.main);
		CHECK( (inputs.name.size() == 1 && outputs.name.size() == 0) );
	}

	TEST_CASE("one output") {
		const char* sample = "{ o1[t] = 0 };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		auto [inputs, outputs] = get_gssotc_io_vars<sbf_ba>(sample_formula.main);
		CHECK( (inputs.name.size() == 0 && outputs.name.size() == 1) );
	}

	TEST_CASE("one input and one output") {
		const char* sample = "{ i1[t] = o1[t] };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		std::vector<gssotc<sbf_ba>> literals;
		auto [inputs, outputs] = get_gssotc_io_vars<sbf_ba>(sample_formula.main);
		CHECK( (inputs.name.size() == 1 && outputs.name.size() == 1) );
	}
}

TEST_SUITE("tau_spec_vars") {

	TEST_CASE("num") {
		const char* sample = "{ i1[0] = 0 };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		auto variable = find_top(sample_formula.main, is_non_terminal<tau_parser::io_var, tau_ba<sbf_ba>, sbf_ba>);
		tau_spec_vars<sbf_ba> vars; vars.add(variable.value());
		CHECK( (vars.name.size() == 1 && vars.loopback == 0) );
	}

	TEST_CASE("capture") {
		const char* sample = "{ (i1[$t] = 0)  };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		auto variable = find_top(sample_formula.main, is_non_terminal<tau_parser::io_var, tau_ba<sbf_ba>, sbf_ba>);
		tau_spec_vars<sbf_ba> vars; vars.add(variable.value());
		CHECK( (vars.name.size() == 1 && vars.loopback == 0) );
	}

	TEST_CASE("variable") {
		const char* sample = "{ i1[t] = 0 };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		auto variable = find_top(sample_formula.main, is_non_terminal<tau_parser::io_var, tau_ba<sbf_ba>, sbf_ba>);
		tau_spec_vars<sbf_ba> vars; vars.add(variable.value());
		CHECK( (vars.name.size() == 1 && vars.loopback == 0) );
	}

	TEST_CASE("shift capture") {
		const char* sample = "{ i1[$t - 1] = 0 };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		auto variable = find_top(sample_formula.main, is_non_terminal<tau_parser::io_var, tau_ba<sbf_ba>, sbf_ba>);
		tau_spec_vars<sbf_ba> vars; vars.add(variable.value());
		CHECK( (vars.name.size() == 1 && vars.loopback == 1) );
	}

	TEST_CASE("shift variable") {
		const char* sample = "{ i1[t - 1] = 0 };";
		auto sample_src = make_tau_source(sample);
		sbf_ba_factory bf;
		auto sample_formula = make_tau_spec_using_factory<sbf_ba_factory, sbf_ba>(sample_src, bf);
		auto variable = find_top(sample_formula.main, is_non_terminal<tau_parser::io_var, tau_ba<sbf_ba>, sbf_ba>);
		tau_spec_vars<sbf_ba> vars; vars.add(variable.value());
		CHECK( (vars.name.size() == 1 && vars.loopback == 1) );
	}
}
