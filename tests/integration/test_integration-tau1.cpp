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
#include "satisfiability.h"

#include "test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("simple tau formulas: no variables") {

	TEST_CASE("{T}") {
		const char* sample = "{T};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("{F}") {
		const char* sample = "{F};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("{T} &&& {T}") {
		const char* sample = "{T} &&& {T};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("{F} &&& {F}") {
		const char* sample = "{F} &&& {F};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("{T} &&& {F}") {
		const char* sample = "{T} &&& {F};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("{F} &&& {T}") {
		const char* sample = "{F} &&& {T};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("{T} ||| {T}") {
		const char* sample = "{T} ||| {T};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("{F} ||| {F}") {
		const char* sample = "{F} ||| {F};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("{T} ||| {F}") {
		const char* sample = "{T} ||| {F};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("{F} ||| {T}") {
		const char* sample = "{F} ||| {T};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("- {F}") {
		const char* sample = "- {F};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("- {T}") {
		const char* sample = "- {T};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory btf;
		tau_factory<bdd_test_factory,  bdd_test> tf(btf);
		auto sample_formula = make_tau_spec_using_factory<tau_factory<bdd_test_factory,  bdd_test>, bdd_test>(sample_src, tf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}
}
