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
#include "bdd_binding.h"
#include "normal_forms.h"
#include "solver.h"

#include "test_integration_helpers-bdd.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

auto splitter_one_bdd() {
	auto var_name = "splitter_one";
	auto v = dict(var_name);
	auto ref = bdd<Bool>::bit(v);
	auto splitter_one = bdd_handle<Bool>::get(ref);
	return build_bf_constant(variant<bdd_binding>(splitter_one));
}

bool check_solution(const nso<bdd_binding>& equation, std::map<nso<bdd_binding>, nso<bdd_binding>> solution) {
	auto copy = solution;
	auto substitution = replace(equation, copy);
	auto check = snf_wff(substitution);
	#ifdef DEBUG
	std::cout << "checking solution: " << solution;
	std::cout << "equation: " << equation << "\n";
	std::cout << "substitution: " << substitution << "\n";
	std::cout << "snf: " << check << "\n";
	#endif // DEBUG
	return check == _T<bdd_binding>;
}

TEST_SUITE("minterm_iterator") {

	TEST_CASE("with one var") {
		const char* sample = "x = 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_binding>>;
		minterm_iterator<bdd_binding> it(sample_formula);
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << sample_formula << "\n";
		std::cout << "minterm: " << *it << "\n";
		#endif // DEBUG
		CHECK ( ++it == minterm_iterator<bdd_binding>::end );
	}

	TEST_CASE("with two vars") {
		const char* sample = "x | y = 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_binding>>;
		minterm_iterator<bdd_binding> it(sample_formula);
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << sample_formula << "\n";
		std::cout << "minterm: " << *it << "\n";
		#endif // DEBUG
		size_t n = 1 ; while (it++ != minterm_iterator<bdd_binding>::end) {
			#ifdef DEBUG
			std::cout << "minterm: " << *it << "\n";
			#endif // DEBUG
			n++;
		}
		CHECK ( n == 3 );
	}
	TEST_CASE("with three vars") {
		std::cout << "------------------------------------------------------\n";
		const char* sample = "x | y | z = 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_binding>>;
		minterm_iterator<bdd_binding> it(sample_formula);
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << sample_formula << "\n";
		std::cout << "minterm: " << *it << "\n";
		#endif // DEBUG
		size_t n = 1 ; while (it++ != minterm_iterator<bdd_binding>::end) {
			#ifdef DEBUG
			std::cout << "minterm: " << *it << "\n";
			#endif // DEBUG
			n++;
		}
		CHECK ( n == 7 );
	}
}

TEST_SUITE("minterm_range") {

	TEST_CASE("one var") {
		const char* sample = "x = 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_binding>>;
		minterm_range<bdd_binding> range(sample_formula);
		CHECK ( range.begin() != range.end() );
		CHECK ( ++range.begin() == range.end() );
	}

	TEST_CASE("two var") {
		const char* sample = "x | y = 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_binding>>;
		minterm_range<bdd_binding> range(sample_formula);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 3 );
	}

	TEST_CASE("three var") {
		const char* sample = "x | y | z = 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<nso<bdd_binding>>;
		minterm_range<bdd_binding> range(sample_formula);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 7 );
	}
}

TEST_SUITE("minterm_inequality_system_iterator") {

	TEST_CASE("no inequality") {
		inequality_system<bdd_binding> sys;
		minterm_inequality_system_iterator<bdd_binding> it(sys);
		CHECK ( (it == minterm_inequality_system_iterator<bdd_binding>::end) );
	}

	TEST_CASE("one inequality with one var") {
		const char* sample = "x != 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula);
		minterm_inequality_system_iterator<bdd_binding> it(sys);
		CHECK ( ++it == minterm_inequality_system_iterator<bdd_binding>::end );
	}

	TEST_CASE("one inequality with two vars") {
		const char* sample = "x | y != 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula);
		minterm_inequality_system_iterator<bdd_binding> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<bdd_binding>::end) n++;
		CHECK ( n == 3 );	}

	TEST_CASE("one inequality with three vars") {
		const char* sample = "x | y | z != 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula);
		minterm_inequality_system_iterator<bdd_binding> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<bdd_binding>::end) n++;
		CHECK ( n == 7 );
	}

	TEST_CASE("two inequalities with one var") {
		const char* sample1 = "a != 0.";
		auto sample_src1 = make_tau_source(sample1);
		nso<bdd_binding> sample_formula1 = make_nso_rr_using_factory<bdd_binding>(sample_src1).main;
		const char* sample2 = "x != 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_binding> sample_formula2 = make_nso_rr_using_factory<bdd_binding>(sample_src2).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_iterator<bdd_binding> it(sys);
		CHECK ( ++it == minterm_inequality_system_iterator<bdd_binding>::end );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a | b != 0.";
		auto sample_src1 = make_tau_source(sample1);
		nso<bdd_binding> sample_formula1 = make_nso_rr_using_factory<bdd_binding>(sample_src1).main;
		const char* sample2 = "x | y != 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_binding> sample_formula2 = make_nso_rr_using_factory<bdd_binding>(sample_src2).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_iterator<bdd_binding> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<bdd_binding>::end) n++;
		CHECK ( n == 9 );
	}

	TEST_CASE("two inequalities with three vars") {
		const char* sample1 = "a | b |c != 0.";
		auto sample_src1 = make_tau_source(sample1);
		nso<bdd_binding> sample_formula1 = make_nso_rr_using_factory<bdd_binding>(sample_src1).main;
		const char* sample2 = "x | y | z!= 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_binding> sample_formula2 = make_nso_rr_using_factory<bdd_binding>(sample_src2).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_iterator<bdd_binding> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<bdd_binding>::end) n++;
		CHECK ( n == 49 );
	}
}

TEST_SUITE("minterm_inequality_system_range") {

	TEST_CASE("no inequality") {
		inequality_system<bdd_binding> sys;
		minterm_inequality_system_range<bdd_binding> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 0 );
	}

	TEST_CASE("one inequality with one var") {
		const char* sample = "x != 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula);
		minterm_inequality_system_range<bdd_binding> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 1 );
	}

	TEST_CASE("one inequality with two vars") {
		const char* sample = "x | y != 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula);
		minterm_inequality_system_range<bdd_binding> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 3 );
	}

	TEST_CASE("one inequality with three vars") {
		const char* sample = "x | y | z != 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula);
		minterm_inequality_system_range<bdd_binding> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 7 );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a != 0.";
		auto sample_src1 = make_tau_source(sample1);
		nso<bdd_binding> sample_formula1 = make_nso_rr_using_factory<bdd_binding>(sample_src1).main;
		const char* sample2 = "x != 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_binding> sample_formula2 = make_nso_rr_using_factory<bdd_binding>(sample_src2).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_range<bdd_binding> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 1 );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a | b != 0.";
		auto sample_src1 = make_tau_source(sample1);
		nso<bdd_binding> sample_formula1 = make_nso_rr_using_factory<bdd_binding>(sample_src1).main;
		const char* sample2 = "x | y != 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_binding> sample_formula2 = make_nso_rr_using_factory<bdd_binding>(sample_src2).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_range<bdd_binding> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 9 );
	}

	TEST_CASE("three inequalities with two vars") {
		const char* sample1 = "a | b | c != 0.";
		auto sample_src1 = make_tau_source(sample1);
		nso<bdd_binding> sample_formula1 = make_nso_rr_using_factory<bdd_binding>(sample_src1).main;
		const char* sample2 = "x | y | z!= 0.";
		auto sample_src2 = make_tau_source(sample2);
		nso<bdd_binding> sample_formula2 = make_nso_rr_using_factory<bdd_binding>(sample_src2).main;
		inequality_system<bdd_binding> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_range<bdd_binding> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 49 );
	}
}

TEST_SUITE("find_solution") {

	bool test_find_solution(const char* src) {
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		#endif // DEBUG
		auto equation = bdd_make_nso(src);
		auto solution = find_solution(equation);
		return ( check_solution(equation, solution.value()));
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

	TEST_CASE("two var: x + y = 1.") {
		const char* sample = "x + y = 1.";
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

TEST_SUITE("lgrs") {

	bool test_lgrs(const char* src) {
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		#endif // DEBUG
		auto equation = bdd_make_nso(src);
		auto solution = lgrs(equation);
		return ( check_solution(equation, solution.value()) );
	}

	TEST_CASE("two var: x | y = 0.") {
		const char* sample = "x | y = 0.";
		CHECK ( test_lgrs(sample) );
	}

	TEST_CASE("two var: x & y = 0.") {
		const char* sample = "x & y = 0.";
		CHECK ( test_lgrs(sample) );
	}
}

TEST_SUITE("solve_minterm_system") {

	bool test_solve_minterm_system(const std::vector<std::string> minterms) {
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		#endif // DEBUG
		bdd_init<Bool>();
		minterm_system<bdd_binding> system;
		for (const auto& minterm: minterms)
			system.insert(bdd_make_nso(minterm));
		auto solution = solve_minterm_system<bdd_binding>(system, splitter_one_bdd());
		bool check = true;
		for (const auto& equation: system)
			check = check ? check_solution(equation, solution.value()) : false;
		return check;
	}

	TEST_CASE("one var: {bdd: a} x != 0 && {bdd: a}' x != 0.") {
		const std::vector<std::string> sample =
			{ "{bdd: a} x != 0.", "{bdd: a}' x != 0."};
		CHECK ( test_solve_minterm_system(sample) );
	}

	TEST_CASE("one var: {bdd: a} x != 0 && {bdd: b} y != 0.") {
		const std::vector<std::string> sample =
			{"{bdd: a} x != 0." , "{bdd: b} y != 0."};
		CHECK( test_solve_minterm_system(sample) );
	}
}

TEST_SUITE("solve_inequality_system") {

	bool test_solve_inequality_system(const std::vector<std::string> inequalities) {
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		#endif // DEBUG
		bdd_init<Bool>();
		inequality_system<bdd_binding> system;
		for (const auto& inequality: inequalities) {
			system.insert(bdd_make_nso(inequality));
		}
		auto solution = solve_inequality_system<bdd_binding>(system, splitter_one_bdd());
		bool check = true;
		for (const auto& equation: system)
			check = check ? check_solution(equation, solution.value()) : false;
		return check;
	}

	// Case 1 of add_minterm_to_disjoint: d = {bdd: a} x and m = {bdd: a} x'
	// both have the same exponent
	TEST_CASE("one var: {bdd: a} x != 0 && {bdd: b} x != 0.") {
		const std::vector<std::string> sample =
			{"{bdd: a} x != 0." , "{bdd: b} x != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}

	// Case 2 of add_minterm_to_disjoint: d = ({bdd: a}|{bdd:b}) x and
	// m = {bdd: a} x', both have different exponents and d_cte & m_cte != false
	// and d_cte & ~m_cte != false
	TEST_CASE("one var (using splitter of a bdd): ({bdd: a}|{bdd:b}) x != 0 && {bdd: a} x' != 0.") {
		const std::vector<std::string> sample =
			{"({bdd: a}|{bdd:b}) x != 0." , "{bdd: a} x' != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}

	// Case 3 of add_minterm_to_disjoint: d = {bdd: a} x and
	// m = ({bdd: a}|{bdd: b}) x', both have different exponents and
	// d_cte & m_cte != false and ~d_cte & m_cte != false
	TEST_CASE("one var (using splitter of a bdd): {bdd: a} x != 0 && ({bdd: a}|{bdd:b}) x' != 0.") {
		const std::vector<std::string> sample =
			{"{bdd: a} x != 0." , "({bdd: a}|{bdd:b}) x' != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}

	// Case 4.1 of add_minterm_to_disjoint: d = x and m = x' both have different
	// exponents and d_cte & m_cte != false, d_cte & ~m_cte = false,
	// ~d_cte & m_cte = false and d_cte = 1
	TEST_CASE("one var (using splitter of a bdd): x != 0 && x' != 0.") {
		const std::vector<std::string> sample =
			{"x != 0." , "x' != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}

	// Case 4.2 of add_minterm_to_disjoint: d = ({bdd: a}&{bdd: b}) x and
	// m = ({bdd: a}|{bdd: b}) x' both have different exponents and
	// d_cte & m_cte != false, d_cte & ~m_cte = false,  ~d_cte & m_cte = false
	// and d_cte != 1
	TEST_CASE("one var (using splitter of one): ({bdd: a}&{bdd: b}) x != 0 && ({bdd: a}|{bdd: b}) x' != 0.") {
		const std::vector<std::string> sample =
			{"({bdd: a}&{bdd: b}) x != 0." , "({bdd: a}|{bdd: b}) x' != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}

	// Case 5 of add_minterm_to_disjoint
	TEST_CASE("one var (using splitter of a bdd): {bdd: a} x != 0 && {bdd: a}' x' != 0.") {
		const std::vector<std::string> sample =
			{"{bdd: a} x != 0." , "{bdd: a} x' != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}
}

TEST_SUITE("solve_system") {

	bool test_solve_system(const std::string equality,
			const std::vector<std::string> inequalities) {
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		#endif // DEBUG
		bdd_init<Bool>();
		equation_system<bdd_binding> system;
		if (equality.size() != 0) system.first = bdd_make_nso(equality);
		for (const auto& inequality: inequalities)
		system.second.insert(bdd_make_nso(inequality));
		auto solution = solve_system<bdd_binding>(system, splitter_one_bdd());
		bool check = system.first
			? check_solution(system.first.value(), solution.value())
			: false;
		for (const auto& equation: system.second)
			check = check ? check_solution(equation, solution.value()) : false;
		return check;
	}

	TEST_CASE("one var: {bdd: a} x = 0 && {bdd: a} x'  != 0.") {
		const char* equality = "{bdd: a} x = 0.";
		const std::vector<std::string> inequalities =
			{ "{bdd: a} x' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("one var: {bdd: a} x y = 0 && {bdd: a} y != 0.") {
		const char* equality = "{bdd: a} x y = 0.";
		const std::vector<std::string> inequalities =
			{ "{bdd: a} y != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("one var: {bdd: a} x | {bdd: a} y = 0 && {bdd: b} y x != 0.") {
		const char* equality = "{bdd: a} x | {bdd: a} y = 0.";
		const std::vector<std::string> inequalities =
			{ "{bdd: b} y x != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}
}
