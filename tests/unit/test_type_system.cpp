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
#include "defs.h"
#include "nso_rr.h"
#include "bool_ba.h"
#include "test_helpers.h"

#include "debug_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;
using namespace std;

namespace testing = doctest;

sp_tau_node<Bool> infer(const char* sample) {
	auto src = make_tau_source(sample);
	auto stmt = make_statement(src);
	return infer_constant_types<Bool>(stmt);
}

bool expect_infer_fail(const char* sample) {
	auto x = infer(sample);
	return x.get() == 0;
}

bool are_all_typed_as(const sp_tau_node<Bool>& n, const std::string& type) {
	for (const auto& c : select_all(n,
				is_non_terminal<tau_parser::bf_constant, Bool>))
	{
		auto tn = c | tau_parser::type;
		if ((!tn && type.size()) || ((tn && type != make_string<
			tau_node_terminal_extractor_t<Bool>, sp_tau_node<Bool>>(
				tau_node_terminal_extractor<Bool>, tn.value()))))
					return false;
	}
	return true;
}

TEST_SUITE("constant types") {
	TEST_CASE("all typed") {
		auto n = infer("{ 0 } : bool = { 1 } : bool & { 0 } : bool.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( are_all_typed_as(n, "bool") );
	}

	TEST_CASE("some typed") {
		auto n = infer("{ 0 } : bool = { 1 } { 0 } : bool.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( are_all_typed_as(n, "bool") );
	}

	TEST_CASE("only 1 typed") {
		auto n = infer("{ 0 } = { 1 } & { 0 } : bool.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( are_all_typed_as(n, "bool") );
	}

	TEST_CASE("only 1 typed sbf") {
		auto n = infer("{ x } = { y } { 0 } : sbf.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( are_all_typed_as(n, "sbf") );
	}

	TEST_CASE("only 1 typed tau") {
		auto n = infer("{ F. } = { T. } { F. } : tau.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( are_all_typed_as(n, "tau") );
	}

	TEST_CASE("none typed") {
		auto n = infer("{ F. } = { T. } { F. }.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( are_all_typed_as(n, "bool") );
	}

	TEST_CASE("type mismatch") {
		auto n = expect_infer_fail("{ F. }:tau = { 0 }:sbf { 1 }:sbf.");
		// ptree<Bool>(std::cout << "inferred: ", n) << "\n";
		CHECK( n );
	}

}
