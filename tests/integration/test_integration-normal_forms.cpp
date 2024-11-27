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

#include "test_integration_helpers-sbf.h"
#include "../unit/test_helpers.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("normal forms: snf for wff") {

	TEST_CASE("simple case: T") {
		const char* sample = "T.";
		auto sample_src = make_tau_source(sample);
		auto nso__rr = make_nso_rr_using_factory<sbf_ba>(sample_src);
		CHECK( nso__rr.has_value() );
		if (!nso__rr.has_value()) return;
		#ifdef DEBUG
		print_sp_tau_node_tree(std::cout, nso__rr.value().main);
		#endif // DEBUG
		auto check = nso__rr.value().main
			| tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("simple case: F") {
		const char* sample = "F.";
		auto sample_src = make_tau_source(sample);
		auto nso__rr = make_nso_rr_using_factory<sbf_ba>(sample_src);
		CHECK( nso__rr.has_value() );
		if (!nso__rr.has_value()) return;
		auto check = nso__rr.value().main
			| tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("simple case: X = 0") {
		const char* sample = "X = 0.";
		auto sample_src = make_tau_source(sample);
		auto nso__rr = make_nso_rr_using_factory<sbf_ba>(sample_src);
		CHECK( nso__rr.has_value() );
		if (!nso__rr.has_value()) return;
		auto result = snf_wff(nso__rr.value().main);
		CHECK( nso__rr.value().main == result );
	}

	TEST_CASE("quantifiers: always X = 0") {
		const char* sample = "always X = 0.";
		auto sample_src = make_tau_source(sample);
		auto nso__rr = make_nso_rr_using_factory<sbf_ba>(sample_src);
		CHECK( nso__rr.has_value() );
		if (!nso__rr.has_value()) return;
		auto result = snf_wff(nso__rr.value().main);
		CHECK( nso__rr.value().main == result );
	}

	TEST_CASE("quantifiers: sometimes o1[t] = 0") {
		const char* sample = "sometimes o1[t] = 0.";
		auto sample_src = make_tau_source(sample);
		auto nso__rr = make_nso_rr_using_factory<sbf_ba>(sample_src);
		CHECK( nso__rr.has_value() );
		if (!nso__rr.has_value()) return;
		auto result = snf_wff(nso__rr.value().main);
		CHECK( nso__rr.value().main == result );
	}

	TEST_CASE("quantifiers: all X X = 0") {
		const char* sample = "all X X = 0.";
		auto sample_src = make_tau_source(sample);
		auto nso__rr = make_nso_rr_using_factory<sbf_ba>(sample_src);
		CHECK( nso__rr.has_value() );
		if (!nso__rr.has_value()) return;
		auto result = snf_wff(nso__rr.value().main);
		CHECK( nso__rr.value().main == result );
	}

	TEST_CASE("quantifiers: ex X X = 0") {
		const char* sample = "ex X X = 0.";
		auto sample_src = make_tau_source(sample);
		auto nso__rr = make_nso_rr_using_factory<sbf_ba>(sample_src);
		CHECK( nso__rr.has_value() );
		if (!nso__rr.has_value()) return;
		auto result = snf_wff(nso__rr.value().main);
		CHECK( nso__rr.value().main == result );
	}

	TEST_CASE("rec. relations: f[0](X)") {
		const char* sample = "f[0](X).";
		auto sample_src = make_tau_source(sample);
		auto nso__rr = make_nso_rr_using_factory<sbf_ba>(sample_src);
		CHECK( nso__rr.has_value() );
		if (!nso__rr.has_value()) return;
		auto result = snf_wff(nso__rr.value().main);
		CHECK( nso__rr.value().main == result );
	}

	// TODO (MEDIUM) fix this test
	/*TEST_CASE("simple case: {sbf: a} x = 0 && {sbf:a}' x = 0") {
		const char* sample = "{sbf: a} x = 0 && {sbf:a}' x = 0.";
		auto sample_src = make_tau_source(sample);
		auto nso__rr = make_nso_rr_using_factory<sbf_ba>(sample_src);
		CHECK( nso__rr.has_value() );
		if (!nso__rr.has_value()) return;
		auto result = snf_wff(nso__rr.value().main);
		std::stringstream ss; ss << result;
		CHECK( ss.str() == "x = 0" );
	}*/

	TEST_CASE("simple case: xy = 0 && x = 0") {
		const char* sample = "xy = 0 && x = 0.";
		auto sample_src = make_tau_source(sample);
		auto nso__rr = make_nso_rr_using_factory<sbf_ba>(sample_src);
		CHECK( nso__rr.has_value() );
		if (!nso__rr.has_value()) return;
		auto result = snf_wff(nso__rr.value().main);
		std::stringstream ss; ss << result;
		CHECK( ss.str() == "x = 0" );
	}

	TEST_CASE("simple case: xy != 0 && x != 0") {
		const char* sample = "xy != 0 && x != 0.";
		auto sample_src = make_tau_source(sample);
		auto nso__rr = make_nso_rr_using_factory<sbf_ba>(sample_src);
		CHECK( nso__rr.has_value() );
		if (!nso__rr.has_value()) return;
		auto result = snf_wff(nso__rr.value().main);
		std::stringstream ss; ss << result;
		CHECK( ss.str() == "xy != 0" );
	}
}
