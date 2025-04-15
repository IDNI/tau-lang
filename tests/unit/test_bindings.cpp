// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "test_helpers.h"


TEST_SUITE("named bindings") {

	TEST_CASE_FIXTURE(named_binder_fixture, "binding: given one statement with no bindigns, the binding process returns the same statement.") {
		const char* sample = "$X := $X.";
		tref t1 = tau::get<Bool>(sample, { .start = tau_parser::library });
		tref t2 = tau::get<bool_binder>(binder, sample, { .start = tau_parser::library });
		CHECK( t1 == t2 );
	}

	TEST_CASE_FIXTURE(named_binder_fixture, "binding: given one statement with one binding, the binding process returns the statement with the binding replaced.") {
		const char* sample = "{ binding } := { binding }.";
		tref bound = tau::get<bool_binder>(binder, sample, { .start = tau_parser::library });
		// if (bound) tau::get(bound).dump(std::cout);
		// if (bound) tau::get(bound).print_tree(std::cout << "result: ") << "\n";
		auto rul = tt(bound) | tau::rules | tau::rule | tau::bf_rule;
		auto c1 = rul | tau::bf_matcher | tau::bf | tau::bf_constant
			| tt::template ba_constant<Bool>;
		auto c2 = rul | tau::bf_body | tau::bf | tau::bf_constant
			| tt::template ba_constant<Bool>;
		CHECK(c1 == c2);
	}

	TEST_CASE_FIXTURE(named_binder_fixture, "binding: given one statement with one non-matching binding, the binding process returns the original statement.") {
		const char* sample = "{ nonmatching } := { nonmatching }.";
		tref bound = tau::get<bool_binder>(binder, sample, { .start = tau_parser::library });
		CHECK( bound == nullptr );
	}
}

TEST_SUITE("factory bindings") {

	TEST_CASE("binding: given one statement with no bindigns, the binding process returns the same statement.") {
		const char* sample = "$X := $X.";
		tref t = tau::get<Bool>(sample, { .start = tau_parser::library });
		CHECK( tau::get(t).is(tau::library) );
	}

	TEST_CASE("binding: given one statement with one binding, the binding process returns the statement with the binding replaced.") {
		const char* sample = "$X := { some_source_code } : bool.";
		tref t = tau::get<Bool>(sample, { .start = tau_parser::library });
		CHECK( tau::get(t).is(tau::library) );
	}

	TEST_CASE("binding: given one statement with one non-matching binding, the binding process returns the original statement.") {
		const char* sample = "$X := { some_source_code } : nonbool.";
		tref t = tau::get<Bool>(sample, { .start = tau_parser::library });
		CHECK( t == nullptr );
	}
}
