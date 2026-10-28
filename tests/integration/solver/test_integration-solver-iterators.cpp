// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_sbf_ba_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "solver.h"
#endif
#include "test_integration-solver_helper.h"

TEST_SUITE("minterm_iterator") {


	TEST_CASE("with one var") {
		const char* sample = "x = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr)).value().main->get();
#ifdef DEBUG
		using node = node_t;
		std::cout << "sample: " << TAU_DUMP_TO_STR(n) << "\n";
#endif // DEBUG
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
		tref n = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr)).value().main->get();
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_iterator<node_t> it(fm);
#ifdef DEBUG
		using node = node_t;
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << TAU_DUMP_TO_STR(fm) << "\n";
		std::cout << "minterm: " << TAU_DUMP_TO_STR(*it) << "\n";
#endif // DEBUG
		size_t count = 1;
		++it;
		while (it != minterm_iterator<node_t>::end) {
#ifdef DEBUG
			std::cout << "minterm: " << TAU_DUMP_TO_STR(*it) << "\n";
#endif // DEBUG
			count++;
			++it;
		}
		CHECK ( count == 3 );
	}
	TEST_CASE("with three vars") {
		std::cout << "------------------------------------------------------\n";
		const char* sample = "x | y | z = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr)).value().main->get();
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_iterator<node_t> it(fm);
#ifdef DEBUG
		using node = node_t;
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << TAU_DUMP_TO_STR(fm) << "\n";
		std::cout << "minterm: " << TAU_DUMP_TO_STR(*it) << "\n";
#endif // DEBUG
		size_t count = 1;
		++it;
		while (it != minterm_iterator<node_t>::end) {
#ifdef DEBUG
			std::cout << "minterm: " << TAU_DUMP_TO_STR(*it) << "\n";
#endif // DEBUG
			count++;
			++it;
		}
		CHECK ( count == 7 );
	}
}

TEST_SUITE("minterm_range") {

	TEST_CASE("one var") {
		const char* sample = "x = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr)).value().main->get();
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_range<node_t> rng(fm);
		CHECK ( rng.begin() != rng.end() );
		CHECK ( ++rng.begin() == rng.end() );
	}

	TEST_CASE("two var") {
		const char* sample = "x | y = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr)).value().main->get();
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_range<node_t> rng(fm);
		size_t count = 0; for ([[maybe_unused]] const auto& i : rng) count++;
		CHECK ( count == 3 );
	}

	TEST_CASE("three var") {
		const char* sample = "x | y | z = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr)).value().main->get();
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_range<node_t> rng(fm);
		size_t count = 0; for ([[maybe_unused]] const auto& i : rng) count++;
		CHECK ( count == 7 );
	}

	TEST_CASE("first variable forced negative") {
		// x'y's single minterm carries a negated leading literal, so
		// reaching it walks the counter's skip-zero-coefficient path.
		// (update_choices_from's index-0 negative seeding is
		// unreachable: a false first choice at index 0 only coincides
		// with counter wrap-around, which exhausts first.)
		const char* sample = "x' & y = 0.";
		tref n = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr)).value().main->get();
		tref fm = tt(n) | tau::bf_eq | tau::bf | tt::ref;
		minterm_range<node_t> rng(fm);
		size_t count = 0; for ([[maybe_unused]] const auto& i : rng) count++;
		CHECK ( count == 1 );
	}
}

TEST_SUITE("minterm_inequality_system_iterator") {

	TEST_CASE("no inequality") {
		inequality_system<node_t> sys;
		minterm_inequality_system_iterator<node_t> it(sys);
		CHECK ( (it == minterm_inequality_system_iterator<node_t>::end) );
	}

	// NOTE: the constructor's exhausted-break arm (an inequality whose bf
	// has no minterms) is unreachable from parsed input -- the
	// construction hooks fold any identically-zero bf before a bf_neq
	// over it can survive, so it stays untested as a defensive guard.

	TEST_CASE("one inequality with one var") {
		const char* sample = "x != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_iterator<node_t> it(sys);
		CHECK ( ++it == minterm_inequality_system_iterator<node_t>::end );
	}

	TEST_CASE("one inequality with two vars") {
		const char* sample = "x | y != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_iterator<node_t> it(sys);
		size_t count = 1;
		++it;
		while (it != minterm_inequality_system_iterator<node_t>::end) { count++; ++it; }
		CHECK ( count == 3 );
	}

	TEST_CASE("one inequality with three vars") {
		const char* sample = "x | y | z != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_iterator<node_t> it(sys);
		size_t count = 1; ++it;
		while (it != minterm_inequality_system_iterator<node_t>::end) { count++; ++it; }
		CHECK ( count == 7 );
	}

	TEST_CASE("two inequalities with one var") {
		const char* sample1 = "a != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1).value_or(nullptr));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x != 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2).value_or(nullptr));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_iterator<node_t> it(sys);
		CHECK ( ++it == minterm_inequality_system_iterator<node_t>::end );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a | b != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1).value_or(nullptr));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x | y != 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2).value_or(nullptr));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_iterator<node_t> it(sys);
		size_t count = 1;
		++it;
		while (it != minterm_inequality_system_iterator<node_t>::end) { count++; ++it; }
		CHECK ( count == 9 );
	}

	TEST_CASE("two inequalities with three vars") {
		const char* sample1 = "a | b |c != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1).value_or(nullptr));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x | y | z!= 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2).value_or(nullptr));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_iterator<node_t> it(sys);
		size_t count = 1; ++it;
		while (it != minterm_inequality_system_iterator<node_t>::end) { count++; ++it; }
		CHECK ( count == 49 );
	}
}

TEST_SUITE("minterm_inequality_system_range") {

	TEST_CASE("no inequality") {
		inequality_system<node_t> sys;
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[maybe_unused]] auto& i : range) count++;
		CHECK ( count == 0 );
	}

	TEST_CASE("one inequality with one var") {
		const char* sample = "x != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[maybe_unused]] auto& i : range) count++;
		CHECK ( count == 1 );
	}

	TEST_CASE("one inequality with two vars") {
		const char* sample = "x | y != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[maybe_unused]] auto& i : range) count++;
		CHECK ( count == 3 );
	}

	TEST_CASE("one inequality with three vars") {
		const char* sample = "x | y | z != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample).value_or(nullptr));
		tref fm = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[maybe_unused]] auto& i : range) count++;
		CHECK ( count == 7 );
	}

	TEST_CASE("two inequalities with one var") {
		const char* sample1 = "a != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1).value_or(nullptr));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x != 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2).value_or(nullptr));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[maybe_unused]] auto& i : range) count++;
		CHECK ( count == 1 );
	}

	TEST_CASE("two inequalities with two vars") {
		const char* sample1 = "a | b != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1).value_or(nullptr));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x | y != 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2).value_or(nullptr));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[maybe_unused]] auto& i : range) count++;
		CHECK ( count == 9 );
	}

	TEST_CASE("two inequalities with three vars") {
		const char* sample1 = "a | b | c != 0.";
		auto nso_rr = get_nso_rr<node_t>(tau::get(sample1).value_or(nullptr));
		tref fm1 = nso_rr.value().main->get();
		const char* sample2 = "x | y | z!= 0.";
		nso_rr = get_nso_rr<node_t>(tau::get(sample2).value_or(nullptr));
		tref fm2 = nso_rr.value().main->get();
		inequality_system<node_t> sys; sys.insert(fm1); sys.insert(fm2);
		minterm_inequality_system_range<node_t> range(sys);
		size_t count = 0; for ([[maybe_unused]] auto& i : range) count++;
		CHECK ( count == 49 );
	}
}

// var_free_holds decides a variable-free equation; the full-LTL refusal of
// solve() used to be a DEBUG-only assertion and is a runtime error now.
TEST_SUITE("solver guards") {
	TEST_CASE("var_free_holds on constant equations") {
		tref t = get_nso_rr<node_t>(tau::get("1 = 1.").value_or(nullptr)).value().main->get();
		tref f = get_nso_rr<node_t>(tau::get("1 = 0.").value_or(nullptr)).value().main->get();
		CHECK( var_free_holds<node_t>(t).value() );
		CHECK_FALSE( var_free_holds<node_t>(f).value() );
	}
	TEST_CASE("solve refuses a formula with a full-LTL operator") {
		tref fm = get_nso_rr<node_t>(
			tau::get("(x = 1) U (y = 0).").value_or(nullptr)).value().main->get();
		REQUIRE( fm != nullptr );
		auto r = solve<node_t>(fm, solver_options{});
		CHECK_FALSE( r.has_value() );
	}
}

TEST_SUITE("minterm_iterator boundaries") {

	static tref bf_of(const char* src) {
		tref n = get_nso_rr<node_t>(tau::get(src).value_or(nullptr)).value().main->get();
		return tt(n) | tau::bf_eq | tau::bf | tt::ref;
	}

	TEST_CASE("a variable-free function has no minterm") {
		tref one = tau::_1(sbf_type_id<node_t>());
		minterm_iterator<node_t> it(one);
		CHECK ( it == minterm_iterator<node_t>::end );
		CHECK ( minterm_range<node_t>(one).empty() );
	}

	TEST_CASE("the all-negative minterm comes first when it is non-zero") {
		// (x | y)' = x'y': its only minterm is the first one tried
		tref f = (~tau::get(bf_of("x | y = 0."))).get();
		minterm_iterator<node_t> it(f);
		REQUIRE ( it != minterm_iterator<node_t>::end );
		CHECK ( ++it == minterm_iterator<node_t>::end );
	}

	TEST_CASE("a prefix whose cofactor is 0 skips the rest of its choices") {
		// (x | y | z)' vanishes once any variable is 1
		tref f = (~tau::get(bf_of("x | y | z = 0."))).get();
		minterm_range<node_t> rng(f);
		size_t count = 0; for ([[maybe_unused]] const auto& i : rng) count++;
		CHECK ( count == 1 );
	}
}

// solve_minterm_system over sbf, which needs options.type_id to read the
// coefficients: each minterm system with non-zero coefficients is solvable.
TEST_SUITE("solve_minterm_system over sbf") {

	static tref sbf_splitter_one_tref() {
		using node = tau_lang::node<bv, sbf_ba>;
		using tau = tree<node>;
		return tau::get(tau::bf, tau::get_ba_constant(typename tau::constant(sbf_splitter_one()), sbf_type<node>()));
	}

	static minterm_system<node_t> minterms_of(const std::vector<std::string>& src) {
		minterm_system<node_t> system;
		for (const auto& mt : src) system.insert(
			get_nso_rr<node_t>(tau::get(mt).value_or(nullptr)).value().main->get());
		return system;
	}

	static bool solves(const std::vector<std::string>& src) {
		auto system = minterms_of(src);
		solver_options options = {
			.splitter_one = sbf_splitter_one_tref(),
			.mode = solver_mode::general,
			.type_id = sbf_type_id<node_t>()
		};
		auto solution_r = solve_minterm_system<node_t>(system, options);
		if (!solution_r.has_value() || !solution_r.value().has_value())
			return false;
		auto solution = std::move(solution_r).value();
		bool check = true;
		for (tref equation : system)
			check = check && check_solution<node_t>(equation, solution.value());
		return check;
	}

	TEST_CASE("same exponent: {a} x and {b} x") {
		CHECK( solves({ "{a}:sbf x != 0.", "{b}:sbf x != 0." }) );
	}

	TEST_CASE("overlapping coefficients, one not within the other") {
		CHECK( solves({ "({a}:sbf | {b}:sbf) x != 0.", "{a}:sbf x' != 0." }) );
	}

	TEST_CASE("overlapping coefficients, the first within the second") {
		CHECK( solves({ "{a}:sbf x != 0.", "({a}:sbf | {b}:sbf) x' != 0." }) );
	}

	TEST_CASE("equal coefficients 1 take the splitter of one") {
		CHECK( solves({ "x : sbf != 0.", "(x : sbf)' != 0." }) );
	}

	TEST_CASE("equal coefficients other than 1 take their own splitter") {
		CHECK( solves({ "{a}:sbf x != 0.", "{a}:sbf x' != 0." }) );
	}

	TEST_CASE("disjoint coefficients are kept as they are") {
		CHECK( solves({ "{a}:sbf x != 0.", "{a}:sbf' x' != 0." }) );
	}

	TEST_CASE("equal coefficients 1 without a splitter of one give no solution") {
		auto system = minterms_of({ "x : sbf != 0.", "(x : sbf)' != 0." });
		solver_options options = {
			.splitter_one = nullptr,
			.mode = solver_mode::general,
			.type_id = sbf_type_id<node_t>()
		};
		auto solution_r = solve_minterm_system<node_t>(system, options);
		REQUIRE( solution_r.has_value() );
		CHECK( !solution_r.value().has_value() );
	}
}

TEST_SUITE("find_solution and solve_system edges") {

	static tref eq_of(const char* src) {
		return get_nso_rr<node_t>(tau::get(src).value_or(nullptr)).value().main->get();
	}

	TEST_CASE("find_solution of a disequality has no solution") {
		auto s = find_solution<node_t>(eq_of("x : sbf != 0."));
		REQUIRE( s.has_value() );
		CHECK( !s.value().has_value() );
	}

	TEST_CASE("find_solution of a variable-free equation has no solution") {
		tref eq;
		{
			use_hooks_guard<node_t> hooks_off(false);
			eq = tau::get(tau::wff, tau::get(tau::bf_eq, {
				tau::_1(sbf_type_id<node_t>()),
				tau::_0(sbf_type_id<node_t>()) }));
		}
		REQUIRE( tau::get(eq).child_is(tau::bf_eq) );
		auto s = find_solution<node_t>(eq);
		REQUIRE( s.has_value() );
		CHECK( !s.value().has_value() );
	}

	static solver_options sbf_options() {
		return { .mode = solver_mode::general,
			.type_id = sbf_type_id<node_t>() };
	}

	TEST_CASE("solve_system without an equality solves the inequalities") {
		equation_system<node_t> system;
		tref neq = eq_of("x : sbf != 0.");
		system.second.insert(neq);
		auto s = solve_system<node_t>(system, sbf_options());
		REQUIRE( s.has_value() );
		REQUIRE( s.value().has_value() );
		CHECK( check_solution<node_t>(neq, s.value().value()) );
	}

	TEST_CASE("solve_system without inequalities solves the equality") {
		equation_system<node_t> system;
		tref eq = eq_of("x : sbf = 0.");
		system.first = eq;
		auto s = solve_system<node_t>(system, sbf_options());
		REQUIRE( s.has_value() );
		REQUIRE( s.value().has_value() );
		CHECK( check_solution<node_t>(eq, s.value().value()) );
	}

	TEST_CASE("solve_system: an inequality the equality makes false") {
		// x = 0 leaves x != 0 no room
		equation_system<node_t> system;
		system.first = eq_of("x : sbf = 0.");
		system.second.insert(eq_of("x : sbf != 0."));
		auto s = solve_system<node_t>(system, sbf_options());
		REQUIRE( s.has_value() );
		CHECK( !s.value().has_value() );
	}
}

TEST_SUITE("atomless_choose_value_ledger declines") {

	static tref sbf_one() {
		using node = tau_lang::node<bv, sbf_ba>;
		using tau = tree<node>;
		return tau::get(tau::bf, tau::get_ba_constant(typename tau::constant(sbf_splitter_one()), sbf_type<node>()));
	}

	static tref reduced(tref t) {
		return bf_reduce_canonical<node_t>()(t).value();
	}

	// The bf node of the constant {a}:sbf, in canonical form.
	static tref constant_a() {
		tref eq = get_nso_rr<node_t>(tau::get("x : sbf = {a}:sbf.")
			.value_or(nullptr)).value().main->get();
		tref c = tau::get(eq).find_top(is_child<node_t, tau::ba_constant>);
		REQUIRE( c != nullptr );
		return reduced(c);
	}

	static solver_options ledger_options(fresh_element_ledger& ledger) {
		return { .splitter_one = sbf_one(), .mode = solver_mode::general,
			.type_id = sbf_type_id<node_t>(), .ledger = &ledger };
	}

	TEST_CASE("a row whose cofactors are not complements") {
		fresh_element_ledger ledger;
		const size_t sbf = sbf_type_id<node_t>();
		std::vector<std::pair<tref, tref>> cofactors{
			{ tau::_0(sbf), constant_a() } };
		auto x = atomless_choose_value_ledger<node_t>(cofactors, sbf,
			ledger_options(ledger));
		REQUIRE( x.has_value() );
		CHECK( !x.value().has_value() );
	}

	TEST_CASE("a region past the constant size budget") {
		fresh_element_ledger ledger;
		const size_t sbf = sbf_type_id<node_t>();
		tref a = constant_a();
		tref na = reduced((~tau::get(a)).get());
		std::vector<std::pair<tref, tref>> cofactors{ { a, na } };
		const size_t saved = max_constant_size;
		max_constant_size = 1;
		auto x = atomless_choose_value_ledger<node_t>(cofactors, sbf,
			ledger_options(ledger));
		max_constant_size = saved;
		REQUIRE( x.has_value() );
		CHECK( !x.value().has_value() );
	}

	TEST_CASE("exclusions that leave no fresh region") {
		fresh_element_ledger ledger;
		const size_t sbf = sbf_type_id<node_t>();
		tref a = constant_a();
		tref na = reduced((~tau::get(a)).get());
		tref nna = reduced((~tau::get(na)).get());
		std::vector<std::pair<tref, tref>> cofactors{ { a, na }, { na, nna } };
		auto x = atomless_choose_value_ledger<node_t>(cofactors, sbf,
			ledger_options(ledger));
		REQUIRE( x.has_value() );
		CHECK( !x.value().has_value() );
	}
}
