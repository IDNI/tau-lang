// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "defs.h"
#include "nso_rr.h"
#include "language.h"
#include "bool_ba.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;
using namespace std;

namespace testing = doctest;

TEST_SUITE("named bindings") {

	TEST_CASE("binding: given one statement with no bindigns, the binding process returns the same statement.") {
		const char* sample = "$X := $X.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_named_bindings(statement, bs);
		CHECK( binded == statement );
	}

	TEST_CASE("binding: given one statement with one binding, the binding process returns the statement with the binding replaced.") {
		const char* sample = "{ binding } := { binding }.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_named_bindings(statement, bs);
		CHECK( binded != statement );
	}

	TEST_CASE("binding: given one statement with one non-matching binding, the binding process returns the original statement.") {
		const char* sample = "{ nonmatching } := { nonmatching }.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_named_bindings(statement, bs);
		CHECK( binded == nullptr );
	}
}

TEST_SUITE("factory bindings") {

	TEST_CASE("binding: given one statement with no bindigns, the binding process returns the same statement.") {
		const char* sample = "$X := $X.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto statement = make_statement(src);
		auto binded = make_factory_bindings(statement);
		CHECK( binded == statement );
	}

	TEST_CASE("binding: given one statement with one binding, the binding process returns the statement with the binding replaced.") {
		const char* sample = "{ binding } := { some_source_sode } : bool.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto statement = make_statement(src);
		auto binded = make_factory_bindings(statement);
		CHECK( binded != statement );
	}

	TEST_CASE("binding: given one statement with one non-matching binding, the binding process returns the original statement.") {
		const char* sample = "{ nonmatching } := { some_source_code } : nonbool.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto statement = make_statement(src);
		auto binded = make_factory_bindings(statement);
		CHECK( binded == statement );
	}
}
