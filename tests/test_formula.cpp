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
#include "../src/normalizer2.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("make_library") {

	TEST_CASE("make_library: one rule case") {
		const auto sample = WFF_ELIM_DOUBLE_NEGATION_0;
		auto lib = make_library<Bool>(sample);
		CHECK( lib.size() == 1 );
	}
}

// TODO (LOW) add more unit tests for make_library
// TODO (LOW) write tests to check make_rule
// TODO (LOW) write tests to check make_tau_source
// TODO (LOW) write tests to check make_tau_code
// TODO (LOW) write tests to check make_builder
// TODO (LOW) write tests for make_formula_using_factory
// TODO (LOW) write tests for make_formula_using_bindings
// TODO (LOW) write tests for make_tau_source
// TODO (LOW) write tests for tau_apply
// TODO (LOW) write tests for tau_apply_builder
// TODO (LOW) write tests for is_non_terminal
// TODO (LOW) write tests for is_terminal
// TODO (LOW) write tests for value_extractor
// TODO (LOW) write tests for terminal_extractor
// TODO (LOW) write tests for non_terminal_extractor
// TODO (LOW) write tests for ba_extractor
// TODO (LOW) write tests for stringify
// TODO (LOW) write tests for make_string_with_skip
// TODO (LOW) write tests for make_string
// TODO (LOW) write tests for operator<<(ostream, sp_tau_node<...>)
// TODO (LOW) write tests for operator<<(ostream, sp_tau_source_node)
//
// All of them are extensively tested inderectly in other unit/integration tests.
// However, it is better to have explicit tests


TEST_SUITE("callbacks") {

	TEST_CASE("BF_CALLBACK_SUBS") {
		static constexpr char* sample =	"bf_subs_cb F T F := F.";
		auto sample_src = make_tau_source(sample);
		auto sample_statement = make_statement(sample_src);
		callback_applier<Bool> cb_applier;
		auto extracted = sample_statement 
			| tau_parser::library | tau_parser::rules 
			| tau_parser::rule | tau_parser::bf_rule 
			| tau_parser::bf_matcher | tau_parser::bf | tau_parser::bf_subs_cb 
			| optional_value_extractor<sp_tau_node<Bool>>;
		auto result = cb_applier(extracted);
		pretty_print_sp_tau_node(std::cout, result) << " result: " << std::endl;
		auto check = result | tau_parser::bf_t;
		CHECK( check.has_value() );	
	}

	// TODO (MEDIUM) add explicit unit tests for the remaining callbacks
	//
	// All of them are tested inderectly when appying the rules of the normalizer
	// in the file test_rules.cpp. However, it is better to have explicit tests
}

