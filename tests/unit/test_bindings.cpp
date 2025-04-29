// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "test_helpers.h"

using tau = tree<node<Bool>>;
using tt = tau::traverser;

struct bool_binder_fixture {
	bacb binder;
};

struct named_bool_binder_fixture {
	bacb binder;
	named_bool_binder_fixture() : binder({
		{ "true_binding",  bac::get(Bool(true),  "bool") },
		{ "false_binding", bac::get(Bool(false), "bool") } }) {}
};

// TEST_SUITE("configuration") {
// 	TEST_CASE("set trace logging level") {
// 		initialize_logging.trace();
// 	}
// }

TEST_SUITE("named bindings") {

	TEST_CASE_FIXTURE(named_bool_binder_fixture,
		"binding: given one statement with no bindigns, the binding "
		"process returns the same statement.")
	{
		const char* sample = "$X := $X.";
		tau::parse_options opts; opts.start = tau::library;
		tref t1 = tau::get(sample, opts);
		tref t2 = tau::get<bacb>(binder, sample, opts);
		CHECK( t1 == t2 );
	}

	TEST_CASE_FIXTURE(named_bool_binder_fixture,
		"binding: given one statement with one binding, the binding "
		"process returns the statement with the binding replaced.")
	{
		const char* sample = "{ true_binding } := { true_binding }.";
		tau::parse_options opts; opts.start = tau::library;
		tref bound = tau::get<bacb>(binder, sample, opts);
		if (bound) tau::get(bound).dump(std::cout);
		if (bound) tau::get(bound).print_tree(std::cout << "result: ") << "\n";
		auto rul = tt(bound) | tau::rules | tau::rule | tau::bf_rule;
		auto c1 = rul | tau::bf_matcher | tau::bf | tt::Tree;
		auto c2 = rul | tau::bf_body | tau::bf | tt::Tree;
		CHECK(c1 == c2);
	}

	TEST_CASE_FIXTURE(named_bool_binder_fixture, "binding: given one statement with one non-matching binding, the binding process returns the original statement.") {
		const char* sample = "{ nonmatching } := { nonmatching }.";
		tau::parse_options opts; opts.start = tau::library;
		tref bound = tau::get<bacb>(binder, sample, opts);
		CHECK( bound == nullptr );
	}
}

TEST_SUITE("factory bindings") {

	TEST_CASE("binding: given one statement with no bindigns, the binding process returns the same statement.") {
		const char* sample = "$X := $X.";
		tau::parse_options opts; opts.start = tau::library;
		tref t = tau::get(sample, opts);
		CHECK( tau::get(t).is(tau::library) );
	}

	TEST_CASE("binding: given one statement with one binding, the binding process returns the statement with the binding replaced.") {
		const char* sample = "$X := { true } : bool.";
		tau::parse_options opts; opts.start = tau::library;
		tref t = tau::get(sample, opts);
		CHECK( tau::get(t).is(tau::library) );
	}

	TEST_CASE("binding: given one statement with one non-matching binding, the binding process returns the original statement.") {
		const char* sample = "$X := { some_source_code } : nonbool.";
		tau::parse_options opts; opts.start = tau::library;
		tref t = tau::get(sample, opts);
		CHECK( t == nullptr );
	}
}
