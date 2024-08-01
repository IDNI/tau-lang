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
#include "nso_rr.h"
#include "bdd_handle.h"
#include "normalizer.h"
#include "solver.h"

#include "test_integration_helpers-bdd.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("minterm_iterator") {

	TEST_CASE("with no vars") {
		const char* sample = "0 = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_test>>;
		minterm_iterator<bdd_test> it(sample_formula);
		CHECK ( it == minterm_iterator<bdd_test>::end );
	}

	TEST_CASE("with one var") {
		const char* sample = "x = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_test>>;
		minterm_iterator<bdd_test> it(sample_formula);
		CHECK ( ++it == minterm_iterator<bdd_test>::end );
	}

	TEST_CASE("with two vars") {
		const char* sample = "x | y = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_test>>;
		minterm_iterator<bdd_test> it(sample_formula);
		CHECK ( ++it != minterm_iterator<bdd_test>::end );
		CHECK ( ++it != minterm_iterator<bdd_test>::end );
		CHECK ( ++it == minterm_iterator<bdd_test>::end );
	}
	TEST_CASE("with three vars") {
		const char* sample = "x | y | z = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_test>>;
		minterm_iterator<bdd_test> it(sample_formula);
		CHECK ( ++it != minterm_iterator<bdd_test>::end );
		CHECK ( ++it != minterm_iterator<bdd_test>::end );
		CHECK ( ++it != minterm_iterator<bdd_test>::end );
		CHECK ( ++it != minterm_iterator<bdd_test>::end );
		CHECK ( ++it != minterm_iterator<bdd_test>::end );
		CHECK ( ++it != minterm_iterator<bdd_test>::end );
		CHECK ( ++it == minterm_iterator<bdd_test>::end );
	}
}

TEST_SUITE("minterm_range") {

	TEST_CASE("no var") {
		const char* sample = "0 = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_test>>;
		minterm_range<bdd_test> range(sample_formula);
		CHECK ( range.begin() == range.end() );
	}

	TEST_CASE("one var") {
		const char* sample = "x = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_test>>;
		minterm_range<bdd_test> range(sample_formula);
		CHECK ( range.begin() != range.end() );
		CHECK ( ++range.begin() == range.end() );
	}

	TEST_CASE("two var") {
		const char* sample = "x | y = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_test>>;
		minterm_range<bdd_test> range(sample_formula);
		auto it = range.begin();
		CHECK ( ++it != range.end() );
		CHECK ( ++it != range.end() );
		CHECK ( ++it == range.end() );
	}

	TEST_CASE("three var") {
		const char* sample = "x | y | z = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_test>>;
		minterm_range<bdd_test> range(sample_formula);
		auto it = range.begin();
		CHECK ( ++it != range.end() );
		CHECK ( ++it != range.end() );
		CHECK ( ++it != range.end() );
		CHECK ( ++it != range.end() );
		CHECK ( ++it != range.end() );
		CHECK ( ++it != range.end() );
		CHECK ( ++it == range.end() );
	}
}

TEST_SUITE("minterm_inequality_system_iterator") {

	TEST_CASE("one inequality with one minterm") {

	}

	TEST_CASE("one inequality with two minterms") {

	}

	TEST_CASE("two inequalities with one minterm") {

	}

	TEST_CASE("two inequalities with two minterms") {

	}

	TEST_CASE("") {

	}
}

TEST_SUITE("minterm_inequality_system_range") {

	TEST_CASE("one inequality with one minterm") {

	}

	TEST_CASE("one inequality with two minterms") {

	}

	TEST_CASE("two inequalities with one minterm") {

	}

	TEST_CASE("two inequalities with two minterms") {

	}

	TEST_CASE("") {

	}
}
