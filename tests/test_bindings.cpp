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

#include "../src/doctest.h"
#include "../src/defs.h"
#include "../src/formula.h"
#include "../src/bool.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;
using namespace std;

namespace testing = doctest;

TEST_SUITE("named bindings") {

	TEST_CASE("binding: given one statement with no bindigns, the binding process returns the same statement.") {
		const char* sample =	"$X := $X.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_named_bindings(statement, bs);
		CHECK( binded == statement );
	}

	TEST_CASE("binding: given one statement with one binding, the binding process returns the statement with the binding replaced.") {
		const char* sample =	"{ binding } := { binding }.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_named_bindings(statement, bs);
		CHECK( binded != statement );
	}

	TEST_CASE("binding: given one statement with one non-matching binding, the binding process returns the original statement.") {
		const char* sample =	"{ non_matching } := { non_matching }.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_named_bindings(statement, bs);
		CHECK( binded == statement );
	}
}

TEST_SUITE("factory bindings") {

	struct dummy_factory {

		sp_tau_node<Bool> build(const std::string type_name, const sp_tau_node<Bool>& n) {
			return type_name == "bool" ? make_node<tau_sym<Bool>>(Bool(true), {}) : n;
		}
	};

	static auto factory = dummy_factory();

	TEST_CASE("binding: given one statement with no bindigns, the binding process returns the same statement.") {
		const char* sample =	"$X := $X.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		auto binded = make_factory_bindings<dummy_factory>(statement, factory);
		CHECK( binded == statement );
	}

	TEST_CASE("binding: given one statement with one binding, the binding process returns the statement with the binding replaced.") {
		const char* sample =	"{ binding } := { bool : some_source_sode }.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		auto binded = make_factory_bindings<dummy_factory>(statement, factory);
		CHECK( binded != statement );
	}

	TEST_CASE("binding: given one statement with one non-matching binding, the binding process returns the original statement.") {
		const char* sample =	"{ non_matching } := { non_bool: some_source_code }.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		auto binded = make_factory_bindings<dummy_factory>(statement, factory);
		CHECK( binded == statement );
	}
}
