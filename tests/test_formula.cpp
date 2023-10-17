// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentiTd cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "../src/doctest.h"
#include "../src/formula.h"
#include "../src/bool.h"
#include "../src/bdd_handle.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

// TODO (HIGH) write tests for factory_binder

// TODO (LOW) write tests for get

// TODO (LOW) add more unit tests for make_library
TEST_SUITE("make_library") {

	static constexpr char* sample =	"bf_neg ( bf_neg ( $X ) ) := $X .";

	TEST_CASE("make_library: one rule case") {
		auto src = make_tau_source(sample);
		auto lib = make_library<Bool>(src);
		// FIXME add sensible checks
		//CHECK( true ); 
	}

}

// TODO (MEDIUM) write tests for make_formula_using_factory
TEST_SUITE("make_formula_using_factory") {

	/* static constexpr char* sample =	
		"bf_neg ( bf_neg ( $X ) ) = $X ."
		" { 0 } .";

	TEST_CASE("one_rule_n_main") {
		auto src = make_tau_source(sample);
		bindings<Bool> bs;
		auto lib = make_formula_using_bindings<Bool>(src, bs);
		CHECK( true );
	}*/

}


// TODO (LOW) add more unit tests for make_tau_source
TEST_SUITE("make_tau_source") {

	/* static constexpr char* sample =	
		"bf_neg ( bf_neg ( $X ) ) = $X .";

	TEST_CASE("one_rule_n_main") {
		auto src = make_tau_source(sample);
		CHECK( true );
	}*/

}

// TODO write tests for make_tau in the future

// TODO write tests for is_non_terminal
// TODO write tests for is_terminal
// TODO write tests for get_nodes
// TODO write tests for value_extractor
// TODO write tests for terminal_extractor
// TODO write tests for non_terminal_extractor
// TODO write tests for ba_extractor

// TODO write tests for stringify
// TODO write tests for make_string_with_skip
// TODO write tests for make_string
// TODO write tests for operator<<(ostream, sp_tau_node<...>)
// TODO write tests for operator<<(ostream, sp_tau_source_node)



