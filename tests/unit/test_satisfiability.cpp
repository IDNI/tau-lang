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
#include "../../src/satisfiability.h"
#include "../../src/bool.h"
#include "../../src/bdd_handle.h"
#include "../../src/normalizer2.h"

#ifdef DEBUG
#include "../../src/debug_helpers.h"
#endif // DEBUG

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
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test> fb(bf);
		auto sample_formula = make_tau_spec_using_factory<factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test>, bdd_test>(sample_src, fb);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_literals<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 1 );
	}

	TEST_CASE("one literal y2") {
		const char* sample = "{ T };";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test> fb(bf);
		auto sample_formula = make_tau_spec_using_factory<factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test>, bdd_test>(sample_src, fb);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_literals<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 1 );
	}

	TEST_CASE("two literals y1") {
		const char* sample = "({ T } &&& !!! { T });";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test> fb(bf);
		auto sample_formula = make_tau_spec_using_factory<factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test>, bdd_test>(sample_src, fb);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_literals<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 2 );
	}

	TEST_CASE("two literals y2") {
		const char* sample = "(!!! { T } &&& !!! { T });";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test> fb(bf);
		auto sample_formula = make_tau_spec_using_factory<factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test>, bdd_test>(sample_src, fb);
		std::vector<gssotc<bdd_test>> literals;
		get_gssotc_literals<bdd_test>(sample_formula.main, literals);
		CHECK( literals.size() == 2 );
	}

	TEST_CASE("one literal: true") {
		const char* sample = "{ T };";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test> fb(bf);
		auto sample_formula = make_tau_spec_using_factory<factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test>, bdd_test>(sample_src, fb);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<bdd_test>(sample_formula.main);
		CHECK( positive.has_value() );
	}

	TEST_CASE("one literal: false") {
		const char* sample = "!!! { T };";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test> fb(bf);
		auto sample_formula = make_tau_spec_using_factory<factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test>, bdd_test>(sample_src, fb);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<bdd_test>(sample_formula.main);
		CHECK( !positive.has_value() );
	}

	TEST_CASE("two literals: true") {
		const char* sample = "({ T } &&& !!! { T });";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test> fb(bf);
		auto sample_formula = make_tau_spec_using_factory<factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test>, bdd_test>(sample_src, fb);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<bdd_test>(sample_formula.main);
		CHECK( positive.has_value() );
	}

	TEST_CASE("two literals: false") {
		const char* sample = "(!!! { T } &&& !!! { T });";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test> fb(bf);
		auto sample_formula = make_tau_spec_using_factory<factory_binder<bdd_test_factory, tau_ba<bdd_test>, bdd_test>, bdd_test>(sample_src, fb);
		auto [positive, negatives] = get_gssotc_positive_negative_literals<bdd_test>(sample_formula.main);
		CHECK( !positive.has_value() );
	}
}

/*TEST_SUITE("get_gssotc_clauses") {
	TEST_CASE("get_gssotc_clauses") {
		const char* sample = "{ ( i_keyboard[t] = o_console[t] ) };";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto nso_rr = frml | tau_parser::nso_rr | tau_parser::nso_main ;
		CHECK( nso_rr.has_value() );
	}
}

TEST_SUITE("get_gssotc_io_vars") {
	TEST_CASE("get_gssotc_io_vars") {
		const char* sample = "{ ( i_keyboard[t] = o_console[t] ) };";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto nso_rr = frml | tau_parser::nso_rr | tau_parser::nso_main ;
		CHECK( nso_rr.has_value() );
	}
}*/