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

#include "doctest.h"
#include "satisfiability.h"
#include "bool_ba.h"
#include "bdd_handle.h"
#include "normalizer.h"

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
		const char* sample = "T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_literals<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 1 );
	}

	TEST_CASE("one literal y2") {
		const char* sample = "T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_literals<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 1 );
	}

	TEST_CASE("two literals y1") {
		const char* sample = "T &&& ~T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_literals<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 2 );
	}

	TEST_CASE("two literals y2") {
		const char* sample = "T &&& ~T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_literals<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 2 );
	}
}

TEST_SUITE("get_gssotc_positive_negative_literals") {

	TEST_CASE("one literal: true") {
		const char* sample = "T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<bdd_test>(sample_formula.main);
		CHECK( positive.has_value() );
		CHECK( negatives.size() == 0 );
	}

	TEST_CASE("one literal: false") {
		const char* sample = "~T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<bdd_test>(sample_formula.main);
		CHECK( !positive.has_value() );
		CHECK( negatives.size() == 1 );
	}

	TEST_CASE("two literals: true") {
		const char* sample = "T &&& ~T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<bdd_test>(sample_formula.main);
		CHECK( positive.has_value() );
		CHECK( negatives.size() == 1 );
	}

	TEST_CASE("two literals: false") {
		const char* sample = "~T  &&& ~T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<bdd_test>(sample_formula.main);
		CHECK( !positive.has_value() );
		CHECK( negatives.size() == 2 );
	}
}

TEST_SUITE("get_gssotc_clauses") {

	TEST_CASE("one clause") {
		const char* sample = "T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_clauses<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 1 );
	}

	TEST_CASE("two clauses y1") {
		const char* sample = "T ||| ~T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_clauses<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 2 );
	}

	TEST_CASE("two clauses y2") {
		const char* sample = " T ||| ~T &&& T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_clauses<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 2 );
	}

	TEST_CASE("three clauses") {
		const char* sample = " T ||| ~T ||| T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_clauses<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 3 );
	}
}

TEST_SUITE("get_gssotc_io_vars") {

	TEST_CASE("none") {
		const char* sample = "T;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		auto [inputs, outputs] = get_gssotc_io_vars<bdd_test>(sample_formula.main);
		CHECK( (inputs.name.size() == 0 && outputs.name.size() == 0) );
	}

	TEST_CASE("one input") {
		const char* sample = "i_keyboard[t] = 0;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		auto [inputs, outputs] = get_gssotc_io_vars<bdd_test>(sample_formula.main);
		CHECK( (inputs.name.size() == 1 && outputs.name.size() == 0) );
	}

	TEST_CASE("one output") {
		const char* sample = "o_console[t] = 0;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		auto [inputs, outputs] = get_gssotc_io_vars<bdd_test>(sample_formula.main);
		CHECK( (inputs.name.size() == 0 && outputs.name.size() == 1) );
	}

	TEST_CASE("one input and one output") {
		const char* sample = "i_keyboard[t] = o_console[t];";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		std::vector<gssotc<bdd_test>> literals;
		auto [inputs, outputs] = get_gssotc_io_vars<bdd_test>(sample_formula.main);
		CHECK( (inputs.name.size() == 1 && outputs.name.size() == 1) );
	}
}

TEST_SUITE("tau_spec_vars") {

	TEST_CASE("num") {
		const char* sample = "i_keyboard[0] = 0;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		auto variable = find_top(sample_formula.main, is_non_terminal<tau_parser::io_var, tau_ba<bdd_test>, bdd_test>);
		tau_spec_vars<bdd_test> vars; vars.add(variable.value());
		CHECK( (vars.name.size() == 1 && vars.loopback == 0) );
	}

	TEST_CASE("capture") {
		const char* sample = "(i_keyboard[$t] = 0);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		auto variable = find_top(sample_formula.main, is_non_terminal<tau_parser::io_var, tau_ba<bdd_test>, bdd_test>);
		tau_spec_vars<bdd_test> vars; vars.add(variable.value());
		CHECK( (vars.name.size() == 1 && vars.loopback == 0) );
	}

	TEST_CASE("variable") {
		const char* sample = "i_keyboard[t] = 0;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		auto variable = find_top(sample_formula.main, is_non_terminal<tau_parser::io_var, tau_ba<bdd_test>, bdd_test>);
		tau_spec_vars<bdd_test> vars; vars.add(variable.value());
		CHECK( (vars.name.size() == 1 && vars.loopback == 0) );
	}

	TEST_CASE("shift capture") {
		const char* sample = "i_keyboard[$t - 1] = 0;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		auto variable = find_top(sample_formula.main, is_non_terminal<tau_parser::io_var, tau_ba<bdd_test>, bdd_test>);
		tau_spec_vars<bdd_test> vars; vars.add(variable.value());
		CHECK( (vars.name.size() == 1 && vars.loopback == 1) );
	}

	TEST_CASE("shift variable") {
		const char* sample = "i_keyboard[t - 1] = 0;";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		auto variable = find_top(sample_formula.main, is_non_terminal<tau_parser::io_var, tau_ba<bdd_test>, bdd_test>);
		tau_spec_vars<bdd_test> vars; vars.add(variable.value());
		CHECK( (vars.name.size() == 1 && vars.loopback == 1) );
	}
}
