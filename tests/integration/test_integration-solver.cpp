// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_sbf_ba_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "solver.h"
#endif

tref splitter_one_bdd() {
	using node = tau_lang::node<bv, sbf_ba>;
	using tau = tree<node>;
	static sbf_ba_factory<bv, sbf_ba> factory;
	return tau::get(tau::bf, tau::get_ba_constant(factory.splitter_one(), sbf_type<node>()));
}

template <NodeType node>
bool check_solution(tref eq, const solution<node>& sol) {
	using tau = tree<node>;
	DBG(TAU_LOG_TRACE << "check_solution/sol:\n" << dump_to_str(sol);)
	tref substitution = rewriter::replace<node>(eq, sol);
	tref check = normalizer<node>(substitution);
#ifdef DEBUG
	// std::cout << "check_solution/solution: " << dump<node>(sol) << "\n";
	std::cout << "check_solution/equation: " << TAU_DUMP_TO_STR(eq) << "\n";
	std::cout << "check_solution/substitution: " << TAU_DUMP_TO_STR(substitution) << "\n";
	std::cout << "check_solution/check: " << TAU_DUMP_TO_STR(check) << "\n";
#endif // DEBUG
	return tau::get(check).equals_T();
}

TEST_SUITE("minterm_iterator") {


	TEST_CASE("with one var") {
		const char* sample = "x = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample)).value().main->get();
		using node = node_t;
		std::cout << "sample: " << TAU_DUMP_TO_STR(n) << "\n";
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_iterator<node_t> it(fm);
#ifdef DEBUG
		using node = node_t;
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << TAU_DUMP_TO_STR(fm) << "\n";
		std::cout << "minterm: " << TAU_DUMP_TO_STR(*it) << "\n";
#endif // DEBUG
		CHECK ( ++it == minterm_iterator<node_t>::end );
	}

	TEST_CASE("with two vars") {
		const char* sample = "x | y = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample)).value().main->get();
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_iterator<node_t> it(fm);
#ifdef DEBUG
		using node = node_t;
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << TAU_DUMP_TO_STR(fm) << "\n";
		std::cout << "minterm: " << TAU_DUMP_TO_STR(*it) << "\n";
#endif // DEBUG
		size_t count = 1;
		while (it++ != minterm_iterator<node_t>::end) {
#ifdef DEBUG
			std::cout << "minterm: " << TAU_DUMP_TO_STR(*it) << "\n";
#endif // DEBUG
			count++;
		}
		CHECK ( count == 3 );
	}
	TEST_CASE("with three vars") {
		std::cout << "------------------------------------------------------\n";
		const char* sample = "x | y | z = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample)).value().main->get();
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_iterator<node_t> it(fm);
#ifdef DEBUG
		using node = node_t;
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << TAU_DUMP_TO_STR(fm) << "\n";
		std::cout << "minterm: " << TAU_DUMP_TO_STR(*it) << "\n";
#endif // DEBUG
		size_t count = 1;
		while (it++ != minterm_iterator<node_t>::end) {
#ifdef DEBUG
			std::cout << "minterm: " << TAU_DUMP_TO_STR(*it) << "\n";
#endif // DEBUG
			count++;
		}
		CHECK ( count == 7 );
	}
}

TEST_SUITE("minterm_range") {

	TEST_CASE("one var") {
		const char* sample = "x = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample)).value().main->get();
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_range<node_t> rng(fm);
		CHECK ( rng.begin() != rng.end() );
		CHECK ( ++rng.begin() == rng.end() );
	}

	TEST_CASE("two var") {
		const char* sample = "x | y = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample)).value().main->get();
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_range<node_t> rng(fm);
		size_t count = 0; for ([[gnu::unused]] const auto& i : rng) count++;
		CHECK ( count == 3 );
	}

	TEST_CASE("three var") {
		const char* sample = "x | y | z = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample)).value().main->get();
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_range<node_t> rng(fm);
		size_t count = 0; for ([[gnu::unused]] const auto& i : rng) count++;
		CHECK ( count == 7 );
	}
}

TEST_SUITE("minterm_inequality_system_iterator") {

	TEST_CASE("no inequality") {
		inequality_system<node_t> sys;
		minterm_inequality_system_iterator<node_t> it(sys);
		CHECK ( (it == minterm_inequality_system_iterator<node_t>::end) );
	}

	TEST_CASE("one inequality with one var") {
		const char* sample = "x != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_iterator<node_t> it(sys);
		CHECK ( ++it == minterm_inequality_system_iterator<node_t>::end );
	}

	TEST_CASE("one inequality with two vars") {
		const char* sample = "x | y != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_iterator<node_t> it(sys);
		size_t count = 1;
		while (it++ != minterm_inequality_system_iterator<node_t>::end) count++;
		CHECK ( count == 3 );
	}

	TEST_CASE("one inequality with three vars") {
		const char* sample = "x | y | z != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_iterator<node_t> it(sys);
		size_t count = 1 ; while (it++ != minterm_inequality_system_iterator<node_t>::end) count++;
		CHECK ( count == 7 );
	}

	TEST_CASE("two inequalities with one var") {
		const char* sample1 = "a != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x != 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_iterator<node_t> it(sys);
		CHECK ( ++it == minterm_inequality_system_iterator<node_t>::end );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a | b != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x | y != 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_iterator<node_t> it(sys);
		size_t count = 1;
		while (it++ != minterm_inequality_system_iterator<node_t>::end) count++;
		CHECK ( count == 9 );
	}

	TEST_CASE("two inequalities with three vars") {
		const char* sample1 = "a | b |c != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x | y | z!= 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_iterator<node_t> it(sys);
		size_t count = 1 ; while (it++ != minterm_inequality_system_iterator<node_t>::end) count++;
		CHECK ( count == 49 );
	}
}

TEST_SUITE("minterm_inequality_system_range") {

	TEST_CASE("no inequality") {
		inequality_system<node_t> sys;
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[gnu::unused]] auto& i : range) count++;
		CHECK ( count == 0 );
	}

	TEST_CASE("one inequality with one var") {
		const char* sample = "x != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[gnu::unused]] auto& i : range) count++;
		CHECK ( count == 1 );
	}

	TEST_CASE("one inequality with two vars") {
		const char* sample = "x | y != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[gnu::unused]] auto& i : range) count++;
		CHECK ( count == 3 );
	}

	TEST_CASE("one inequality with three vars") {
		const char* sample = "x | y | z != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[gnu::unused]] auto& i : range) count++;
		CHECK ( count == 7 );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x != 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[gnu::unused]] auto& i : range) count++;
		CHECK ( count == 1 );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a | b != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x | y != 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[gnu::unused]] auto& i : range) count++;
		CHECK ( count == 9 );
	}

	TEST_CASE("three inequalities with two vars") {
		const char* sample1 = "a | b | c != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x | y | z!= 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[gnu::unused]] auto& i : range) count++;
		CHECK ( count == 49 );
	}
}


TEST_SUITE("configuration") {

	TEST_CASE("bdd initialization") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("find_solution") {

	bool test_find_solution(const char* src) {
#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
#endif // DEBUG
		tref equation = get_nso_rr<node_t>(tau::get(src)).value().main->get();
		equation = norm_all_equations<node_t>(equation);
		equation = apply_all_xor_def<node_t>(equation);
		auto solution = find_solution<node_t>(equation);
		return ( check_solution<node_t>(equation, solution.value()));
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

	TEST_CASE("two var: x ^ y = 1.") {
		const char* sample = "x ^ y = 1.";
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
	TEST_CASE("<:a> z ^ <:b> z' = 0") {
		const char* sample = "<:a> z ^ <:b> z' = 0.";
		CHECK ( test_find_solution(sample) );
	}
}

TEST_SUITE("lgrs") {

	bool test_lgrs(const char* src) {
#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
#endif // DEBUG
		tref equation = get_nso_rr<node_t>(tau::get(src)).value().main->get();
		auto solution = lgrs<node_t>(equation);
		return ( check_solution<node_t>(equation, solution.value()) );
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
		minterm_system<node_t> system;
		for (const auto& mt : minterms) system.insert(
				get_nso_rr<node_t>(tau::get(mt)).value().main->get());
		solver_options options = {
			.splitter_one = splitter_one_bdd(),
			.mode = solver_mode::general
		};
		auto solution = solve_minterm_system<node_t>(system, options);
		bool check = true;
		for (tref equation : system)
			check = check ? check_solution<node_t>(
					equation, solution.value()) : false;
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
		inequality_system<node_t> system;
		for (const auto& ineq : inequalities)
			system.insert(get_nso_rr<node_t>(
				tau::get(ineq)).value().main->get());

		// setting the proper options
		solver_options options = {
			.splitter_one = splitter_one_bdd(),
			.mode = solver_mode::general
		};

		auto solution = solve_inequality_system<node_t>(system, options);
		bool check = true;
		for (tref equation : system)
			check = check ? check_solution<node_t>(
					equation, solution.value()) : false;
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
		bdd_init<Bool>();
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
		bdd_init<Bool>();
		const std::vector<std::string> sample =
			{"x : sbf != 0." , "(x : sbf)' != 0."};
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
			{ "x:sbf y' != 0.",  "(x : sbf)' != 0." };
		CHECK ( test_solve_inequality_system(inequalities) );
	}
}

TEST_SUITE("solve_system") {

	bool test_solve_system(const std::string equality,
			const std::vector<std::string> inequalities)
	{
#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
#endif // DEBUG
		equation_system<node_t> system;
		if (equality.size() != 0) system.first = get_nso_rr<node_t>(tau::get(equality)).value().main->get();
#ifdef DEBUG
		if (system.first)
			std::cout << "test_solve_system/system.first: " << system.first.value() << "\n";
#endif // DEBUG
		for (const auto& ineq : inequalities) {
			system.second.insert(get_nso_rr<node_t>(tau::get(ineq)).value().main->get());
#ifdef DEBUG
			std::cout << "test_solve_system/system.second: " << tau::get(get_nso_rr<node_t>(tau::get(ineq)).value().main).dump_to_str() << "\n";
#endif // DEBUG
		}

		// setting the proper options
		solver_options options = {
			.splitter_one = splitter_one_bdd(),
			.mode = solver_mode::general
		};

		// calling the solver function
		auto solution = solve_system<node_t>(system, options);

#ifdef DEBUG
		// if (solution)
		// 	std::cout << "test_solve_system/solution: " << solution.value() << "\n";
#endif // DEBUG
		auto copy = solution.value();
		bool check = system.first
			? check_solution<node_t>(system.first.value(), copy)
			: false;
		for (const auto& equation: system.second) {
			auto copy = solution.value();
			check = check && check_solution<node_t>(equation, copy);
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
		const char* equality = "x:sbf & y' = 0.";
		const std::vector<std::string> inequalities =
			{ "x' & y:sbf | x & y' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("two var: y = {a}:sbf && x < y.") {
		const char* equality = "y' & {a}:sbf | y & {a}:sbf' | x y' = 0.";
		const std::vector<std::string> inequalities =
			{ "x' & y | x & y' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("two var: y < x && z < y.") {
		bdd_init<Bool>();
		const char* equality = "y:sbf & x' | z & y' = 0.";
		const std::vector<std::string> inequalities =
			{ "y:sbf & x' | x & y' != 0.", "y:sbf & z' | z & y' != 0." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	TEST_CASE("two var: y < x && z < y && w < z.") {
		const char* equality = "x'y:sbf | y'z | wz' = 0.";
		const std::vector<std::string> inequalities =
			{ " w'z|wz' != 0:sbf.", "xy' != 0:sbf.", "yz' != 0:sbf." };
		CHECK ( test_solve_system(equality, inequalities) );
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

	// TEST_CASE("trace:off") {
	// 	logging_initializer.error();
	// }

	// increasing monotonicity (3)
	TEST_CASE("x = 0 && z < y && y != 1") {
		const char* equality = "x:sbf | z y' = 0.";
		const std::vector<std::string> inequalities =
			{ "y:sbf z' | y' z != 0.", "y' != 0:sbf." };
		CHECK ( test_solve_system(equality, inequalities) );
	}

	// increasing monotonicity (3 y1)
	TEST_CASE("x = 0 && x < y && y != 1") {
		const char* equality = "x:sbf | x y' = 0.";
		const std::vector<std::string> inequalities =
			{ "y x' | y'x != 0:sbf.", "y' != 0:sbf." };
		CHECK ( test_solve_system(equality, inequalities) );
	}
}

TEST_SUITE("solve") {
	// This test is for the type tau_ba<sbf_ba>, sbf_ba
	using node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
	using tau = tree<node_t>;


	bool test_solve(const std::string system, const solver_options& options) {
#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
#endif // DEBUG
		tref form = get_nso_rr<node_t>(tau::get(system)).value().main->get();
		bool solve_error = false;
		auto solution = solve<node_t>(form, options, solve_error);
		return solution ? check_solution<node_t>(form, solution.value()) : false;
	}

	bool test_solve_min(const std::string system, const tref type = tau_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::nso_factory::splitter_one(type),
			.mode = solver_mode::minimum
		};
		return test_solve(system, options);
	}

	bool test_solve_max(const std::string system, const tref type = tau_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::nso_factory::splitter_one(type),
			.mode = solver_mode::maximum
		};
		return test_solve(system, options);
	}

	bool test_solve(const std::string system, const tref type = tau_type<node_t>()) {
		solver_options options = {
			.splitter_one = node_t::nso_factory::splitter_one(type),
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
		CHECK ( test_solve(system, sbf_type<node_t>()) );
	}

	// increasing monotonicity (2)
	//
	// This test case fails if we try to build the equation system by hand
	// and call the solve_system method (see above).
	TEST_CASE("x = {a}:sbf && x < y && y != 1") {
		const char* system = "x = {a}:sbf && x < y && y != 1.";
		CHECK ( test_solve(system, sbf_type<node_t>()) );
	}

	// increasing monotonicity (2 y2)
	TEST_CASE("x = {a}:sbf {b}:sbf && x < y && y != 1") {
		const char* system = "x = {a}:sbf {b}:sbf && x < y && y != 1.";
		CHECK ( test_solve(system, sbf_type<node_t>()) );
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
