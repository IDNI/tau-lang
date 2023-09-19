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
// TODO (HIGH) write tests for resolve_type

// TODO (MEDIUM) write tests for get
// TODO (MEDIUM) write tests for get_children
// TODO (MEDIUM) write tests for get_child

// DOING (HIGH) write tests for callback_applier
TEST_SUITE("callback_applier") {

	// write formulas that use the callback functions on the right side, and
	// have the proper solution on the left, and check that the callback_applier
	// returns the proper solution.

	TEST_CASE("callback_applier: bf_neg_cb") {
		static constexpr char* sample =	"result = { bf_neg_cb value }.";
		auto src = make_tau_source(sample);
		bindings<Bool> bs { { "result", Bool(true) }, { "value", Bool(false) } };
		// bindings<Bool> bs { { "result", Bool(false) }, { "value", Bool(true) } };
		CHECK( true );
	} 
}

TEST_SUITE("operator|") {

	TEST_CASE("match zero nodes") {
		static constexpr char* sample =	"(?Z bf_and ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto args = lib 
			| tau_parser::main;
		CHECK( !args );
	}

	TEST_CASE("match one node") {
		static constexpr char* sample =	"(?Z bf_and ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto args = lib 
			| tau_parser::library
			| tau_parser::rules;
		CHECK( args );
	}
}

TEST_SUITE("operator||") {

	TEST_CASE("match zero nodes") {
		static constexpr char* sample =	"(?Z bf_and ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto args = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::wff_rule
			|| tau_parser::wff;
		CHECK( args.size() == 0 );
	}

	TEST_CASE("match one node") {
		static constexpr char* sample =	"(?Z bf_and ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto args = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			|| tau_parser::bf_rule;
		CHECK( args.size() == 1 );
	}

	// TODO simplify the test cases
	TEST_CASE("match several nodes") {
		static constexpr char* sample =	"((?Z bf_and ?Z) bf_and (?Z bf_and ?Z)) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto args = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_and
			|| tau_parser::bf
			|| tau_parser::bf_and
			|| tau_parser::bf;
		CHECK( args.size() == 4 );
		CHECK( args[0] == args[1] );		
		CHECK( args[0] == args[2] );		
		CHECK( args[0] == args[3] );		
	}
}

TEST_SUITE("bind") {

	TEST_CASE("binding: given one statement with no bindigns, the binding process returns the same statement.") {
		static constexpr char* sample =	"$X := $X.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_binding(statement, bs);
		CHECK( binded == statement );
	}

	TEST_CASE("binding: given one statement with one binding, the binding process returns the statement with the binding replaced.") {
		static constexpr char* sample =	"{ binding } := { binding }.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_binding(statement, bs);
		CHECK( binded != statement );
	}

	TEST_CASE("binding: given one statement with one non-matching binding, the binding process returns the original statement.") {
		static constexpr char* sample =	"{ non_matching } := { non_matching }.";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_binding(statement, bs);
		CHECK( binded == statement );
	}
}

// TODO (HIGH) add more unit tests for make_library
TEST_SUITE("make_library") {

	static constexpr char* sample =	"bf_neg ( bf_neg ( $X ) ) := $X .";

	TEST_CASE("make_library: one rule case") {
		auto src = make_tau_source(sample);
		auto lib = make_library<Bool>(src);
		// FIXME add sensible checks
		CHECK( true ); 
	}

}

// TODO (MEDIUM) write tests for make_formula_using_bindings
TEST_SUITE("make_formula_using_bindings") {

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


// TODO (HIGH) add more unit tests for make_tau_source
TEST_SUITE("make_tau_source") {

	/* static constexpr char* sample =	
		"bf_neg ( bf_neg ( $X ) ) = $X .";

	TEST_CASE("one_rule_n_main") {
		auto src = make_tau_source(sample);
		CHECK( true );
	}*/

}

// TODO write tests for tau_apply (with rule)
// TODO write tests for tau_apply (with rules)
// TODO write tests for make_tau in the future

// TODO write tests for is_non_terminal_predicate
// TODO write tests for is_non_terminal
// TODO write tests for is_terminal_predicate
// TODO write tests for is_terminal
// TODO write tests for get_node
// TODO write tests for get_nodes
// TODO write tests for value_extractor
// TODO write tests for get_value
// TODO write tests for get_values
// TODO write tests for terminal_extractor
// TODO write tests for get_terminal
// TODO write tests for get_terminals
// TODO write tests for non_terminal_extractor
// TODO write tests for get_non_terminal
// TODO write tests for get_non_terminals
// TODO write tests for ba_extractor
// TODO write tests for get_ba
// TODO write tests for get_bas

// TODO write tests for tauify
// TODO write tests for stringify
// TODO write tests for make_string_with_skip
// TODO write tests for make_string
// TODO write tests for operator<<(ostream, sp_tau_node<...>)
// TODO write tests for operator<<(ostream, sp_tau_source_node)
// TODO write tests for is_unresolved_predicate
// TODO write tests for is_resolved_predicate
// TODO write tests for is_unresolved
// TODO write tests for resolve_types (with rule)
// TODO write tests for resolve_types (with rules)
// TODO write tests for make rule



