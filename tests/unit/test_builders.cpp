// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "test_helpers.h"

#include "boolean_algebras/bdds/babdd.h"

// TODO (MEDIUM) split into test_builder-parsing and test_builder-execution

TEST_SUITE("builders parsing") {

	TEST_CASE("BLDR_WFF_EQ") {
		rewriter::builder bldr = tau::get_builder<Bool>(BLDR_WFF_EQ);
		CHECK( tau::get(bldr.first).is(tau::builder_head) );
		CHECK( tau::get(bldr.second).is(tau::wff) );
	}

	TEST_CASE("BLDR_WFF_NEQ") {
		rewriter::builder bldr = tau::get_builder<Bool>(BLDR_WFF_NEQ);
		CHECK( tau::get(bldr.first).is(tau::builder_head) );
		CHECK( tau::get(bldr.second).is(tau::wff) );
	}

	TEST_CASE("BLDR_WFF_ALL") {
		rewriter::builder bldr = tau::get_builder<Bool>(BLDR_WFF_ALL);
		CHECK( tau::get(bldr.first).is(tau::builder_head) );
		CHECK( tau::get(bldr.second).is(tau::wff) );
	}

	TEST_CASE("BLDR_WFF_EX") {
		rewriter::builder bldr = tau::get_builder<Bool>(BLDR_WFF_EX);
		CHECK( tau::get(bldr.first).is(tau::builder_head) );
		CHECK( tau::get(bldr.second).is(tau::wff) );
	}

	TEST_CASE("BLDR_BF_SPLITTER") {
		rewriter::builder bldr = tau::get_builder<Bool>(BLDR_BF_SPLITTER);
		CHECK( tau::get(bldr.first).is(tau::builder_head) );
		CHECK( tau::get(bldr.second).is(tau::bf) );
	}

	TEST_CASE("BLDR_BF_NOT_LESS_EQUAL") {
		auto bldr = tau::get_builder<Bool>(BLDR_BF_NOT_LESS_EQUAL);
		CHECK( tau::get(bldr.first).is(tau::builder_head) );
		CHECK( tau::get(bldr.second).is(tau::wff) );
	}

}

// TODO (HIGH) check builded structures deeply in execution tests

struct builders_exe_fixture {
	builders_exe_fixture() {
		const char* sample = " X = 0 .";
		auto fm = make_tt<Bool>(sample);
		auto bfs = (fm | tau::spec | tau::main | tau::wff | tau::bf_eq || tau::bf)();
		X = bfs[0] | tau::variable | tt::ref;
		F = bfs[1] | tau::bf_f | tt::ref;
	}
	tref X, F;
};

TEST_SUITE("builders execution") {

	TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_WFF_EQ") {
		auto bldr = tau::get_builder<Bool>(BLDR_WFF_EQ);
		trefs args = { X };
		auto check = tt(tau::apply_builder(bldr, args)) | tau::bf_eq;
		CHECK( check.has_value() );
	}

	TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_WFF_NEQ") {
		auto bldr = tau::get_builder<Bool>(BLDR_WFF_NEQ);
		trefs args = { X };
		auto check = tt(tau::apply_builder(bldr, args)) | tau::bf_neq;
		CHECK( check.has_value() );
	}

	TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_WFF_XOR") {
		// TODO (LOW) write proper test
	}

	TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_WFF_IMPLY") {
		// TODO (LOW) write proper test
	}

	TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_WFF_EQUIV") {
		// TODO (LOW) write proper test
	}

	TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_WFF_ALL") {
		auto bldr = tau::get_builder<Bool>(BLDR_WFF_ALL);
		trefs args = { X, F };
		auto check = tt(tau::apply_builder(bldr, args)) | tau::wff_all;
		CHECK( check.has_value() );
	}

	TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_WFF_EX") {
		auto bldr = tau::get_builder<Bool>(BLDR_WFF_EX);
		trefs args = { X, F };
		auto check = tt(tau::apply_builder(bldr, args)) | tau::wff_ex;
		CHECK( check.has_value() );
	}

	TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_BF_SPLITTER") {
		auto bldr = tau::get_builder<Bool>(BLDR_BF_SPLITTER);
		trefs args = { F };
		auto check = tt(tau::apply_builder(bldr, args)) | tau::bf_splitter;
		CHECK( check.has_value() );
	}

	TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_BF_LESS") {
		// TODO (LOW) write proper test
	}

	TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_BF_LESS_EQUAL") {
		// TODO (LOW) write proper test
	}

	// TODO (HIGH) implement update this tests to cover upper and lower bounds
	/*TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_BF_NOT_LESS_EQUAL") {
		auto bldr = tau::get_builder(BLDR_BF_NOT_LESS_EQUAL);
		trefs args = { F, F };
		auto check = tt(tau::apply_builder(bldr, args)) | tau::bf_not_less_equal;
		CHECK( check.has_value() );
	}*/

	TEST_CASE_FIXTURE(builders_exe_fixture, "BLDR_BF_GREATER") {
		// TODO (LOW) write proper test
	}
}
