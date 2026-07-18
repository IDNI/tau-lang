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
		tref n = get_nso_rr<node_t>(tau::get(sample)).value().main->get();
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
