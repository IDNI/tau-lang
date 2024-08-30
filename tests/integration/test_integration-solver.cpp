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
#include "normal_forms.h"
#include "solver.h"

#include "test_integration_helpers-bdd.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

/*TEST_SUITE("minterm_iterator") {

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
}*/

TEST_SUITE("find_solution") {

	bool test_find_solution(const char* sample) {
		#ifdef DEBUG
		std::cout << "FIND_SOLUTION: " << std::string(sample) << "\n";
		#endif // DEBUG
		auto src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> equality = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(src, bf).main;
		auto solution = find_solution(equality);
		auto changes1 = solution;
		auto sample1 = replace(equality, changes1);
		auto check1 = snf_wff(sample1);
		auto changes2 = solution;
		auto sample2 = snf_wff(build_wff_neg(equality));
		auto check2 = replace(sample2, changes2);
		#ifdef DEBUG
		std::cout << "SOLUTION: " << solution << "\n";
		std::cout << "CHECK1: " << check1 << "\n";
		std::cout << "CHECK2: " << check2 << "\n";
		#endif // DEBUG
		return (check1 == _T<bdd_test>) && (check2 == _F<bdd_test>);
	}

	TEST_CASE("one var: x = 0.") {
		const char* sample = "x = 0.";
		CHECK ( test_find_solution(sample) );
	}

	TEST_CASE("two var: x | y = 0.") {
		const char* sample = "x | y = 0.";
		CHECK ( test_find_solution(sample) );
	}

	TEST_CASE("two var: x | y' = 0.") {
		const char* sample = "x | y' = 0.";
		CHECK ( test_find_solution(sample) );
	}

	TEST_CASE("two var: x & y = 0.") {
		const char* sample = "x & y = 0.";
		CHECK ( test_find_solution(sample) );
	}

	TEST_CASE("two var: x & y' = 0.") {
		const char* sample = "x & y' = 0.";
		CHECK ( test_find_solution(sample) );
	}
}

/*TEST_SUITE("lgrs") {

	TEST_CASE("two var: x | y = 0.") {
		const char* sample = "x | y = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		auto solution = lgrs(sample_formula);
		CHECK ( solution.size() == 2 );
	}

	TEST_CASE("two var: x & y = 0.") {
		const char* sample = "x & y = 0.";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src, bf).main;
		auto solution = lgrs(sample_formula);
		CHECK ( solution.size() == 2 );
	}
}

TEST_SUITE("solve_minterm_system") {

	TEST_CASE("one var: {bdd: a} x != 0 && {bdd: a} y != 0.") {
		bdd_init<Bool>();
		const char* sample1 = "{bdd: a} x != 0.";
		const char* sample2 = "{bdd: a}' x != 0.";
		auto sample_src1 = make_tau_source(sample1);
		auto sample_src2 = make_tau_source(sample2);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula1 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src1, bf).main;
		nso<bdd_test> sample_formula2 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src2, bf).main;
		minterm_system<bdd_test> sample_system {sample_formula1, sample_formula2};
		auto solution = solve_minterm_system<bdd_test, bdd_test>(sample_system);
		CHECK ( solution.size() == 1 );
	}
}

TEST_SUITE("solve_inequality_system") {

	TEST_CASE("one var: {bdd: a} x | {bdd: a} y != 0 && {bdd: a} y & {bdd: a} x != 0.") {
		bdd_init<Bool>();
		const char* sample1 = "{bdd: a} x | {bdd: b} y != 0.";
		const char* sample2 = "{bdd: b} y & {bdd: a} x != 0.";
		auto sample_src1 = make_tau_source(sample1);
		auto sample_src2 = make_tau_source(sample2);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula1 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src1, bf).main;
		nso<bdd_test> sample_formula2 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src2, bf).main;
		inequality_system<bdd_test> sample_system {sample_formula1, sample_formula2};
		auto solution = solve_inequality_system<bdd_test, bdd_test>(sample_system);
		CHECK ( solution.size() == 2 );
	}

}

TEST_SUITE("solve_inequality_system") {

	TEST_CASE("one var: {bdd: a} x | {bdd: a} y != 0 && {bdd: a} y & {bdd: a} x != 0.") {
		bdd_init<Bool>();
		const char* sample1 = "{bdd: a} x | {bdd: a} y != 0.";
		const char* sample2 = "{bdd: a} y & {bdd: a} x != 0.";
		auto sample_src1 = make_tau_source(sample1);
		auto sample_src2 = make_tau_source(sample2);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula1 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src1, bf).main;
		nso<bdd_test> sample_formula2 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src2, bf).main;
		inequality_system<bdd_test> sample_system {sample_formula1, sample_formula2};
		auto solution = solve_inequality_system<bdd_test, bdd_test>(sample_system);
		CHECK ( solution.size() == 2 );
	}

}

TEST_SUITE("solve_system") {

	TEST_CASE("one var: {bdd: a} x | {bdd: a} y != 0 && {bdd: a} y & {bdd: a} x != 0.") {
		bdd_init<Bool>();
		const char* sample1 = "{bdd: a} x & {bdd: a} y = 0.";
		const char* sample2 = "{bdd: a} y & {bdd: a} x != 0.";
		auto sample_src1 = make_tau_source(sample1);
		auto sample_src2 = make_tau_source(sample2);
		bdd_test_factory bf;
		nso<bdd_test> sample_formula1 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src1, bf).main;
		nso<bdd_test> sample_formula2 = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(sample_src2, bf).main;
		equation_system<bdd_test> sample_system { {sample_formula1}, {sample_formula2}};
		auto solution = solve_system<bdd_test, bdd_test>(sample_system);
		CHECK ( solution.size() == 2 );
	}
}*/
