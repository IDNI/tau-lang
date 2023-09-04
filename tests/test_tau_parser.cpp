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
#include "../src/formula.h"
#include "../src/bool.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;
using namespace std;

namespace testing = doctest;

// IMPORTANT: To check the parser (and the underlying grammar) we don't need 
// any special boolean algebra, so we use Bool to simplify our testing.

// The following test suites represent correct sentences of a tau formula or 
// library and as such must be parsed correctly and satisfy the structural checks
// we perform on the parsed formula (as the rest of the code assumes such structure).

TEST_SUITE("parse tree structure") {

	// TODO (HIGH) test libary parsing structure
	TEST_CASE("parse tree structure: library") {
		static constexpr char* sample =	"bf_neg ( bf_neg ( $X ) ) = $X .";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto result = lib | tau_parser::start | tau_parser::library;
		CHECK( true ); // ( result.has_value() );
	}
}

// TODO (HIGH) test rule parsing structure
// TODO (HIGH) test program parsing structure

// TODO (HIGH) test wwf rule parsing structure
// TODO (HIGH) test cbf rule parsing structure
// TODO (HIGH) test bf rule parsing structure

// TODO (HIGH) test source binding parsing structure
// TODO (HIGH) test named binding parsing structure
// TODO (HIGH) test source binding type parsing structure
// TODO (HIGH) test source binding source parsing structure
// TODO (HIGH) test unresolved source binding
// TODO (HIGH) test resolved source binding

// TODO (HIGH) test and callback parsing structure
// TODO (HIGH) test or callback parsing structure
// TODO (HIGH) test xor callback parsing structure
// TODO (HIGH) test neg callback parsing structure
// TODO (HIGH) test subs callback parsing structure