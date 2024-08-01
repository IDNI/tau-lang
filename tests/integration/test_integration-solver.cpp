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
		size_t n = 1 ; while (it++ != minterm_iterator<bdd_test>::end) n++;
		CHECK ( n == 3 );
	}
	TEST_CASE("with three vars") {
		const char* sample = "x | y | z = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_test>>;
		minterm_iterator<bdd_test> it(sample_formula);
		size_t n = 1 ; while (it++ != minterm_iterator<bdd_test>::end) n++;
		CHECK ( n == 7 );
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
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 3 );
	}

	TEST_CASE("three var") {
		const char* sample = "x | y | z = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_test>>;
		minterm_range<bdd_test> range(sample_formula);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 7 );
	}
}

TEST_SUITE("minterm_inequality_system_iterator") {

	TEST_CASE("no inequality") {
		inequality_system<bdd_test> sys;
		minterm_inequality_system_iterator<bdd_test> it(sys);
		CHECK ( (it == minterm_inequality_system_iterator<bdd_test>::end) );
	}

	TEST_CASE("one inequality with no vars") {
		const char* sample = "0 != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula);
		minterm_inequality_system_iterator<bdd_test> it(sys);
		CHECK ( it == minterm_inequality_system_iterator<bdd_test>::end );
	}

	TEST_CASE("one inequality with one var") {
		const char* sample = "x != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula);
		minterm_inequality_system_iterator<bdd_test> it(sys);
		CHECK ( ++it == minterm_inequality_system_iterator<bdd_test>::end );
	}

	TEST_CASE("one inequality with two vars") {
		const char* sample = "x | y != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula);
		minterm_inequality_system_iterator<bdd_test> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<bdd_test>::end) n++;
		CHECK ( n == 3 );	}

	TEST_CASE("one inequality with three vars") {
		const char* sample = "x | y | z != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula);
		minterm_inequality_system_iterator<bdd_test> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<bdd_test>::end) n++;
		CHECK ( n == 7 );
	}

	TEST_CASE("two inequalities with no vars") {
		const char* sample = "0 != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula); sys.insert(sample_formula);
		minterm_inequality_system_iterator<bdd_test> it(sys);
		CHECK ( it == minterm_inequality_system_iterator<bdd_test>::end );

	}

	TEST_CASE("two inequalities with one var") {
		const char* sample1 = "a != 0.";
		auto sample_src1 = make_tau_source(sample1);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula1 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src1, bf).main;
		const char* sample2 = "x != 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_test> sample_formula2 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src2, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_iterator<bdd_test> it(sys);
		CHECK ( ++it == minterm_inequality_system_iterator<bdd_test>::end );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a | b != 0.";
		auto sample_src1 = make_tau_source(sample1);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula1 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src1, bf).main;
		const char* sample2 = "x | y != 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_test> sample_formula2 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src2, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_iterator<bdd_test> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<bdd_test>::end) n++;
		CHECK ( n == 9 );
	}

	TEST_CASE("two inequalities with three vars") {
		const char* sample1 = "a | b |c != 0.";
		auto sample_src1 = make_tau_source(sample1);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula1 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src1, bf).main;
		const char* sample2 = "x | y | z!= 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_test> sample_formula2 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src2, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_iterator<bdd_test> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<bdd_test>::end) n++;
		CHECK ( n == 49 );
	}
}

TEST_SUITE("minterm_inequality_system_range") {

	TEST_CASE("no inequality") {
		inequality_system<bdd_test> sys;
		minterm_inequality_system_range<bdd_test> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 0 );
	}

	TEST_CASE("one inequality with no vars") {
		const char* sample = "0 != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula);
		minterm_inequality_system_range<bdd_test> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 0 );
	}

	TEST_CASE("one inequality with one var") {
		const char* sample = "x != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula);
		minterm_inequality_system_range<bdd_test> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 1 );
	}

	TEST_CASE("one inequality with two vars") {
		const char* sample = "x | y != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula);
		minterm_inequality_system_range<bdd_test> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 3 );
	}

	TEST_CASE("one inequality with three vars") {
		const char* sample = "x | y | z != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula);
		minterm_inequality_system_range<bdd_test> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 7 );
	}

	TEST_CASE("two inequalities with no vars") {
		const char* sample = "0 != 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula); sys.insert(sample_formula);
		minterm_inequality_system_range<bdd_test> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 0 );
	}

	TEST_CASE("two inequalities with one var") {
		const char* sample1 = "a != 0.";
		auto sample_src1 = make_tau_source(sample1);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula1 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src1, bf).main;
		const char* sample2 = "x != 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_test> sample_formula2 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src2, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_range<bdd_test> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 1 );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a | b != 0.";
		auto sample_src1 = make_tau_source(sample1);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula1 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src1, bf).main;
		const char* sample2 = "x | y != 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_test> sample_formula2 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src2, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_range<bdd_test> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 9 );
	}

	TEST_CASE("three inequalities with two vars") {
		const char* sample1 = "a | b | c != 0.";
		auto sample_src1 = make_tau_source(sample1);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula1 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src1, bf).main;
		const char* sample2 = "x | y | z!= 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_test> sample_formula2 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src2, bf).main;
		inequality_system<bdd_test> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_range<bdd_test> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 49 );
	}
}
