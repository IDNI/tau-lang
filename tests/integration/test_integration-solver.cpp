// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "doctest.h"
#include "solver.h"

#include "test_integration_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;
using namespace boost::log;

namespace testing = doctest;

auto splitter_one_bdd() {
	static sbf_ba_factory<sbf_ba> factory;
	return build_bf_constant(factory.splitter_one(), build_type<sbf_ba>("sbf"));
}

template<typename...BAs>
bool check_solution(const tau_depreciating<BAs...>& equation, const std::map<tau_depreciating<BAs...>, tau_depreciating<BAs...>>& solution) {
	auto substitution = replace(equation, solution);
	auto check = snf_wff(substitution);
	#ifdef DEBUG
	std::cout << "check_solution/solution: " << solution;
	std::cout << "check_solution/equation: " << equation << "\n";
	std::cout << "check_solution/substitution: " << substitution << "\n";
	std::cout << "check_solution/check: " << check << "\n";
	#endif // DEBUG
	return check == _T<BAs...>;
}

TEST_SUITE("minterm_iterator") {

	TEST_CASE("with one var") {
		const char* sample = "x = 0.";
		auto sample_src = make_tau_source(sample);
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<tau_depreciating<sbf_ba>>;
		minterm_iterator<sbf_ba> it(sample_formula);
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << sample_formula << "\n";
		std::cout << "minterm: " << *it << "\n";
		#endif // DEBUG
		CHECK ( ++it == minterm_iterator<sbf_ba>::end );
	}

	TEST_CASE("with two vars") {
		const char* sample = "x | y = 0.";
		auto sample_src = make_tau_source(sample);
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<tau_depreciating<sbf_ba>>;
		minterm_iterator<sbf_ba> it(sample_formula);
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << sample_formula << "\n";
		std::cout << "minterm: " << *it << "\n";
		#endif // DEBUG
		size_t n = 1 ; while (it++ != minterm_iterator<sbf_ba>::end) {
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
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<tau_depreciating<sbf_ba>>;
		minterm_iterator<sbf_ba> it(sample_formula);
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << sample_formula << "\n";
		std::cout << "minterm: " << *it << "\n";
		#endif // DEBUG
		size_t n = 1 ; while (it++ != minterm_iterator<sbf_ba>::end) {
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
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<tau_depreciating<sbf_ba>>;
		minterm_range<sbf_ba> range(sample_formula);
		CHECK ( range.begin() != range.end() );
		CHECK ( ++range.begin() == range.end() );
	}

	TEST_CASE("two var") {
		const char* sample = "x | y = 0.";
		auto sample_src = make_tau_source(sample);
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<tau_depreciating<sbf_ba>>;
		minterm_range<sbf_ba> range(sample_formula);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 3 );
	}

	TEST_CASE("three var") {
		const char* sample = "x | y | z = 0.";
		auto sample_src = make_tau_source(sample);
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main
			| tau_parser::bf_eq | tau_parser::bf | optional_value_extractor<tau_depreciating<sbf_ba>>;
		minterm_range<sbf_ba> range(sample_formula);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 7 );
	}
}

TEST_SUITE("minterm_inequality_system_iterator") {

	TEST_CASE("no inequality") {
		inequality_system<sbf_ba> sys;
		minterm_inequality_system_iterator<sbf_ba> it(sys);
		CHECK ( (it == minterm_inequality_system_iterator<sbf_ba>::end) );
	}

	TEST_CASE("one inequality with one var") {
		const char* sample = "x != 0.";
		auto sample_src = make_tau_source(sample);
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula);
		minterm_inequality_system_iterator<sbf_ba> it(sys);
		CHECK ( ++it == minterm_inequality_system_iterator<sbf_ba>::end );
	}

	TEST_CASE("one inequality with two vars") {
		const char* sample = "x | y != 0.";
		auto sample_src = make_tau_source(sample);
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula);
		minterm_inequality_system_iterator<sbf_ba> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<sbf_ba>::end) n++;
		CHECK ( n == 3 );	}

	TEST_CASE("one inequality with three vars") {
		const char* sample = "x | y | z != 0.";
		auto sample_src = make_tau_source(sample);
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula);
		minterm_inequality_system_iterator<sbf_ba> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<sbf_ba>::end) n++;
		CHECK ( n == 7 );
	}

	TEST_CASE("two inequalities with one var") {
		const char* sample1 = "a != 0.";
		auto sample_src1 = make_tau_source(sample1);
		tau_depreciating<sbf_ba> sample_formula1 = make_nso_rr_using_factory<sbf_ba>(sample_src1).value().main;
		const char* sample2 = "x != 0.";
		auto sample_src2 = make_tau_source(sample2);
		tau_depreciating<sbf_ba> sample_formula2 = make_nso_rr_using_factory<sbf_ba>(sample_src2).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_iterator<sbf_ba> it(sys);
		CHECK ( ++it == minterm_inequality_system_iterator<sbf_ba>::end );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a | b != 0.";
		auto sample_src1 = make_tau_source(sample1);
		tau_depreciating<sbf_ba> sample_formula1 = make_nso_rr_using_factory<sbf_ba>(sample_src1).value().main;
		const char* sample2 = "x | y != 0.";
		auto sample_src2 = make_tau_source(sample2);
		tau_depreciating<sbf_ba> sample_formula2 = make_nso_rr_using_factory<sbf_ba>(sample_src2).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_iterator<sbf_ba> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<sbf_ba>::end) n++;
		CHECK ( n == 9 );
	}

	TEST_CASE("two inequalities with three vars") {
		const char* sample1 = "a | b |c != 0.";
		auto sample_src1 = make_tau_source(sample1);
		tau_depreciating<sbf_ba> sample_formula1 = make_nso_rr_using_factory<sbf_ba>(sample_src1).value().main;
		const char* sample2 = "x | y | z!= 0.";
		auto sample_src2 = make_tau_source(sample2);
		tau_depreciating<sbf_ba> sample_formula2 = make_nso_rr_using_factory<sbf_ba>(sample_src2).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_iterator<sbf_ba> it(sys);
		size_t n = 1 ; while (it++ != minterm_inequality_system_iterator<sbf_ba>::end) n++;
		CHECK ( n == 49 );
	}
}

TEST_SUITE("minterm_inequality_system_range") {

	TEST_CASE("no inequality") {
		inequality_system<sbf_ba> sys;
		minterm_inequality_system_range<sbf_ba> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 0 );
	}

	TEST_CASE("one inequality with one var") {
		const char* sample = "x != 0.";
		auto sample_src = make_tau_source(sample);
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula);
		minterm_inequality_system_range<sbf_ba> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 1 );
	}

	TEST_CASE("one inequality with two vars") {
		const char* sample = "x | y != 0.";
		auto sample_src = make_tau_source(sample);
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula);
		minterm_inequality_system_range<sbf_ba> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 3 );
	}

	TEST_CASE("one inequality with three vars") {
		const char* sample = "x | y | z != 0.";
		auto sample_src = make_tau_source(sample);
		tau_depreciating<sbf_ba> sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula);
		minterm_inequality_system_range<sbf_ba> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 7 );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a != 0.";
		auto sample_src1 = make_tau_source(sample1);
		tau_depreciating<sbf_ba> sample_formula1 = make_nso_rr_using_factory<sbf_ba>(sample_src1).value().main;
		const char* sample2 = "x != 0.";
		auto sample_src2 = make_tau_source(sample2);
		tau_depreciating<sbf_ba> sample_formula2 = make_nso_rr_using_factory<sbf_ba>(sample_src2).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_range<sbf_ba> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 1 );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a | b != 0.";
		auto sample_src1 = make_tau_source(sample1);
		tau_depreciating<sbf_ba> sample_formula1 = make_nso_rr_using_factory<sbf_ba>(sample_src1).value().main;
		const char* sample2 = "x | y != 0.";
		auto sample_src2 = make_tau_source(sample2);
		tau_depreciating<sbf_ba> sample_formula2 = make_nso_rr_using_factory<sbf_ba>(sample_src2).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_range<sbf_ba> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 9 );
	}

	TEST_CASE("three inequalities with two vars") {
		const char* sample1 = "a | b | c != 0.";
		auto sample_src1 = make_tau_source(sample1);
		tau_depreciating<sbf_ba> sample_formula1 = make_nso_rr_using_factory<sbf_ba>(sample_src1).value().main;
		const char* sample2 = "x | y | z!= 0.";
		auto sample_src2 = make_tau_source(sample2);
		tau_depreciating<sbf_ba> sample_formula2 = make_nso_rr_using_factory<sbf_ba>(sample_src2).value().main;
		inequality_system<sbf_ba> sys; sys.insert(sample_formula1); sys.insert(sample_formula2);
		minterm_inequality_system_range<sbf_ba> range(sys);
		size_t n = 0; for ( [[gnu::unused]] auto& i: range) n++;
		CHECK ( n == 49 );
	}
}


TEST_SUITE("configuration") {

	/*TEST_CASE("logging") {
		core::get()->set_filter(trivial::severity >= trivial::error);
		add_console_log(std::cout, keywords::format =
			expressions::stream << expressions::smessage);
	}*/

	TEST_CASE("bdd initialization") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("find_solution") {

	bool test_find_solution(const char* src) {
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		#endif // DEBUG
		auto equation = sbf_make_nso(src);
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

	//  number 1 of test_integration-interpreter.cpp
	TEST_CASE("<:a> z + <:b> z' = 0") {
		const char* sample = "<:a> z + <:b> z' = 0.";
		CHECK ( test_find_solution(sample) );
	}
}

TEST_SUITE("lgrs") {

	bool test_lgrs(const char* src) {
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		#endif // DEBUG
		auto equation = sbf_make_nso(src);
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
		minterm_system<sbf_ba> system;
		for (const auto& minterm: minterms)
			system.insert(sbf_make_nso(minterm));
		solver_options<sbf_ba> options = {
			.splitter_one = splitter_one_bdd(),
			.mode = solver_mode::general
		};
		auto solution = solve_minterm_system<sbf_ba>(system, options);
		bool check = true;
		for (const auto& equation: system)
			check = check ? check_solution(equation, solution.value()) : false;
		return check;
	}

	TEST_CASE("one var: {a}:sbf x != 0 && {a}:sbf' x != 0.") {
		const std::vector<std::string> sample =
			{ "{a}:sbf x != 0.", "{a}:sbf' x != 0."};
		CHECK ( test_solve_minterm_system(sample) );
	}

	TEST_CASE("one var: {a}:sbf x != 0 && {b}:sbf y != 0.") {
		const std::vector<std::string> sample =
			{"{a}:sbf x != 0." , "{b}:sbf y != 0."};
		CHECK( test_solve_minterm_system(sample) );
	}
}

TEST_SUITE("solve_inequality_system") {

	bool test_solve_inequality_system(const std::vector<std::string> inequalities) {
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		#endif // DEBUG
		inequality_system<sbf_ba> system;
		for (const auto& inequality: inequalities) {
			system.insert(sbf_make_nso(inequality));
		}

		// setting the proper options
		solver_options<sbf_ba> options = {
			.splitter_one = splitter_one_bdd(),
			.mode = solver_mode::general
		};

		auto solution = solve_inequality_system<sbf_ba>(system, options);
		bool check = true;
		for (const auto& equation: system)
			check = check ? check_solution(equation, solution.value()) : false;
		return check;
	}

	// Case 1 of add_minterm_to_disjoint: d = {a}:sbf x and m = {a}:sbf x'
	// both have the same exponent
	TEST_CASE("one var: {a}:sbf x != 0 && {b}:sbf x != 0.") {
		const std::vector<std::string> sample =
			{"{a}:sbf x != 0." , "{b}:sbf x != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}

	// Case 2 of add_minterm_to_disjoint: d = ({a}:sbf|{b}:sbf) x and
	// m = {a}:sbf x', both have different exponents and d_cte & m_cte != false
	// and d_cte & ~m_cte != false
	TEST_CASE("one var (using splitter of sbf): ({a}:sbf|{b}:sbf) x != 0 && {a}:sbf x' != 0.") {
		const std::vector<std::string> sample =
			{"({a}:sbf|{b}:sbf) x != 0." , "{a}:sbf x' != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}

	// Case 3 of add_minterm_to_disjoint: d = {a}:sbf x and
	// m = ({a}:sbf|{b}:sbf) x', both have different exponents and
	// d_cte & m_cte != false and ~d_cte & m_cte != false
	TEST_CASE("one var (using splitter of a sbf): {a}:sbf x != 0 && ({a}:sbf|{b}:sbf) x' != 0.") {
		const std::vector<std::string> sample =
			{"{a}:sbf x != 0." , "({a}:sbf|{b}:sbf) x' != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}

	// Case 4.1 of add_minterm_to_disjoint: d = x and m = x' both have different
	// exponents and d_cte & m_cte != false, d_cte & ~m_cte = false,
	// ~d_cte & m_cte = false and d_cte = 1
	TEST_CASE("one var (using splitter of a sbf): x != 0 && x' != 0.") {
		const std::vector<std::string> sample =
			{"x != 0." , "x' != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}

	// Case 4.2 of add_minterm_to_disjoint: d = {a}:sbf&{b}:sbf x and
	// m = ({a}:sbf|{b}:sbf) x' both have different exponents and
	// d_cte & m_cte != false, d_cte & ~m_cte = false,  ~d_cte & m_cte = false
	// and d_cte != 1
	TEST_CASE("one var (using splitter of one): {a}:sbf&{b}:sbf x != 0 && ({a}:sbf|{b}:sbf) x' != 0.") {
		const std::vector<std::string> sample =
			{"{a}:sbf&{b}:sbf x != 0." , "({a}:sbf|{b}:sbf) x' != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}

	// Case 5 of add_minterm_to_disjoint
	TEST_CASE("one var (using splitter of a sbf): {a}:sbf x != 0 && {a}:sbf' x' != 0.") {
		const std::vector<std::string> sample =
			{"{a}:sbf x != 0." , "{a}:sbf x' != 0."};
		CHECK( test_solve_inequality_system(sample) );
	}

	// increasing monotonicity (4)
	TEST_CASE("x > y && x != 1") {
		const std::vector<std::string> inequalities =
			{ "x y' != 0.",  "x' != 0." };
		CHECK ( test_solve_inequality_system(inequalities) );
	}
}

TEST_SUITE("solve_system") {

	bool test_solve_system(const std::string equality,
			const std::vector<std::string> inequalities) {
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		#endif // DEBUG
		equation_system<sbf_ba> system;
		if (equality.size() != 0) system.first = sbf_make_nso(equality);
		#ifdef DEBUG
		if (system.first)
			std::cout << "test_solve_system/system.first: " << system.first.value() << "\n";
		#endif // DEBUG
		for (const auto& inequality: inequalities) {
			system.second.insert(sbf_make_nso(inequality));
			#ifdef DEBUG
			std::cout << "test_solve_system/system.second: " << sbf_make_nso(inequality) << "\n";
			#endif // DEBUG
		}

		// setting the proper options
		solver_options<sbf_ba> options = {
			.splitter_one = splitter_one_bdd(),
			.mode = solver_mode::general
		};

		// calling the solver function
		auto solution = solve_system<sbf_ba>(system, options);

		#ifdef DEBUG
		if (solution)
			std::cout << "test_solve_system/solution: " << solution.value() << "\n";
		#endif // DEBUG
		auto copy = solution.value();
		bool check = system.first
			? check_solution(system.first.value(), copy)
			: false;
		for (const auto& equation: system.second) {
			auto copy = solution.value();
			check = check && check_solution(equation, copy);
		}
		return check;
	}

	TEST_CASE("one var: {a}:sbf x = 0 && {a}:sbf x'  != 0.") {
		const char* equality = "{a}:sbf x = 0.";
		const std::vector<std::string> inequalities =
			{ "{a}:sbf x' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("two var: {a}:sbf x y = 0 && {a}:sbf y != 0.") {
		const char* equality = "{a}:sbf x y = 0.";
		const std::vector<std::string> inequalities =
			{ "{a}:sbf y != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("two var: {a}:sbf x | {a}:sbf y = 0 && {b}:sbf y x != 0.") {
		// TODo (HIGH) check this one too
		//const char* equality = "{a}:sbf x | {a}:sbf y = 0.";
		//const std::vector<std::string> inequalities =
		//	{ "{b}:sbf y x != 0." };
		const char* equality = "{a}:sbf x | {a}:sbf y = 0.";
		const std::vector<std::string> inequalities =
			{ "{b}:sbf y x != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("two var: x < y.") {
		const char* equality = "x & y' = 0.";
		const std::vector<std::string> inequalities =
			{ "x' & y | x & y' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("two var: y = {a}:sbf && x < y.") {
		const char* equality = "y' & {a}:sbf | y & {a}:sbf' | x y' = 0.";
		const std::vector<std::string> inequalities =
			{ "x' & y | x & y' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("two var: y < x && z < y.") {
		const char* equality = "y & x' | z & y' = 0.";
		const std::vector<std::string> inequalities =
			{ "y & x' | x & y' != 0.", "y & z' | z & y' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("two var: y < x && z < y && w < z.") {
		const char* equality = "x'y | y'z | wz' = 0.";
		const std::vector<std::string> inequalities =
			{ " w'z|wz' != 0.", "xy' != 0.", "yz' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("trace:on") {
		core::get()->set_filter(trivial::severity >= trivial::trace);
		add_console_log(std::cout, keywords::format =
			expressions::stream << expressions::smessage);
	}

	// increasing monotonicity (2)
	//
	// This test fails due to a failing computation of the lgrs substitution.
	// The same test works fine when calling the solve methos directly instead
	// of constructing the equation system by hand. As the failing point is
	// when we compute the lgrs solution using the replace method (used all
	// over the code), maybe is an issue with the parser or in the hooks that
	// somehow change the behavior of the replace method.
	//
	// The working test for this system has a comment to track it. We also have
	// created an issue to track this problem.
	/*TEST_CASE("x = {a}:sbf && z < y && y != 1") {
		const char* equality = "x' {a}:sbf | y' z | {a'}:sbf x = 0.";
		const std::vector<std::string> inequalities =
			{ "y z' != 0.", "y' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}*/

	TEST_CASE("trace:off") {
		core::get()->set_filter(trivial::severity >= trivial::error);
		add_console_log(std::cout, keywords::format =
			expressions::stream << expressions::smessage);
	}


	// increasing monotonicity (3)
	TEST_CASE("x = 0 && z < y && y != 1") {
		const char* equality = "x | z y' = 0.";
		const std::vector<std::string> inequalities =
			{ "y z' | y' z != 0.", "y' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	// increasing monotonicity (3 y1)
	TEST_CASE("x = 0 && x < y && y != 1") {
		const char* equality = "x | x y' = 0.";
		const std::vector<std::string> inequalities =
			{ "y x' | y'x != 0.", "y' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}
}

TEST_SUITE("solve") {

	bool test_solve(const std::string system, const solver_options<tau_ba<sbf_ba>, sbf_ba>& options) {
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		#endif // DEBUG
		auto form = tau_make_nso_test(system);
		auto solution = solve<tau_ba<sbf_ba>, sbf_ba>(form, options);
		return solution ? check_solution(form, solution.value()) : false;
	}

	bool test_solve_min(const std::string system,  const std::string type = "") {
		solver_options<tau_ba<sbf_ba>, sbf_ba> options = {
			.splitter_one = nso_factory<tau_ba<sbf_ba>, sbf_ba>::instance().splitter_one(type),
			.mode = solver_mode::minimum
		};
		return test_solve(system, options);
	}

	bool test_solve_max(const std::string system, const std::string type = "") {
		solver_options<tau_ba<sbf_ba>, sbf_ba> options = {
			.splitter_one = nso_factory<tau_ba<sbf_ba>, sbf_ba>::instance().splitter_one(type),
			.mode = solver_mode::maximum
		};
		return test_solve(system, options);
	}

	bool test_solve(const std::string system, const std::string type = "") {
		solver_options<tau_ba<sbf_ba>, sbf_ba> options = {
			.splitter_one = nso_factory<tau_ba<sbf_ba>, sbf_ba>::instance().splitter_one(type),
			.mode = solver_mode::general
		};
		return test_solve(system, options);
	}

	// increasing monotonicity (1)
	TEST_CASE("o1[t] > {<:split1> != 0} && o1[t] != 1") {
		const char* system = "o1[t] > {<:split1> != 0} && o1[t] != 1.";
		CHECK ( test_solve(system) );
	}

	TEST_CASE("two var: y < x && y = {a}:sbf && x' != 0") {
		const char* system = " y < x && y = {a}:sbf && x' != 0.";
		CHECK ( test_solve(system, "sbf") );
	}

	// increasing monotonicity (2)
	//
	// This test case fails if we try to build the equation system by hand
	// and call the solve_system method (see above).
	TEST_CASE("x = {a}:sbf && x < y && y != 1") {
		const char* system = "x = {a}:sbf && x < y && y != 1.";
		CHECK ( test_solve(system, "sbf") );
	}

	// increasing monotonicity (2 y2)
	TEST_CASE("x = {a}:sbf {b}:sbf && x < y && y != 1") {
		const char* system = "x = {a}:sbf {b}:sbf && x < y && y != 1.";
		CHECK ( test_solve(system, "sbf") );
	}

	TEST_CASE("x != 0") {
		const char* system = "x != 0.";
		CHECK ( test_solve(system) );
		CHECK ( !test_solve_min(system) );
		CHECK ( test_solve_max(system) );
	}

	TEST_CASE("x != 0 && x != 1") {
		const char* system = "x != 0 && x != 1.";
		CHECK ( test_solve(system) );
		CHECK ( !test_solve_min(system) );
		CHECK ( !test_solve_max(system) );
	}

	TEST_CASE("x != 0 && y != 0") {
		const char* system = "x != 0 && y != 0.";
		CHECK ( test_solve(system) );
		CHECK ( !test_solve_min(system) );
		CHECK ( test_solve_max(system) );
	}

	// Lucca's failing interpreter case
	TEST_CASE("{<:z> = 0 && <:w> = 0} = o1[t] && o2[2] = { <:x> = 0 } && o3[1] = { <:y> = 0 }") {
		const char* system = "{<:z> = 0 && <:w> = 0} = o1[t] && o2[2] = { <:x> = 0 } && o3[1] = { <:y> = 0 }.";
		CHECK ( test_solve(system) );
		CHECK ( test_solve_min(system) );
		CHECK ( test_solve_max(system) );
	}

	// Lucca's failing interpreter case simplified to sbf
	TEST_CASE("{a & b}:sbf = o1[t] && o2[2] = { c }:sbf && o3[1] = { d }:sbf") {
		const char* system = "{a & b}:sbf = o1[t] && o2[2] = { c }:sbf && o3[1] = { d }:sbf.";
		CHECK ( test_solve(system) );
		CHECK ( test_solve_min(system) );
		CHECK ( test_solve_max(system) );
	}

	// Lucca's failing interpreter case (further) simplified to sbf
	TEST_CASE("x = {a}:sbf && y = {b}:sbf") {
		const char* system = "x = {a}:sbf && y = {b}:sbf.";
		CHECK ( test_solve(system) );
		CHECK ( test_solve_min(system) );
		CHECK ( test_solve_max(system) );
	}
}