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

#include "../src/doctest.h"
#include "../src/program.h"
#include "../src/bool.h"
#include "../src/bdd_handle.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

// TODO write tests for is_non_terminal_predicate
// TODO write tests for is_ignore_predicate
// TODO write tests for is_capture_predicate

// TODO (MEDIUM) write tests for get
// TODO (MEDIUM) write tests for get_children
// TODO (MEDIUM) write tests for get_child

// TODO (HIGH) write tests for callback_applier
// TODO write tests for tauify
// TODO write tests for stringify
// TODO write tests for make_string_with_skip
// TODO write tests for make_string
// TODO write tests for operator<<(ostream, sp_tau_node<...>)
// TODO write tests for operator<<(ostream, sp_tau_source_node)

TEST_SUITE("bind") {

	sp_tau_node<Bool> make_statement(const sp_tau_source_node& source) {
	tauify<Bool> tf;
	map_transformer<decltype(tf), sp_tau_source_node, sp_tau_node<Bool>> transform(tf);
	return post_order_traverser<decltype(transform), decltype(all<sp_tau_source_node>),
		sp_node<tau_source_sym>, sp_tau_node<Bool>>(transform, all<sp_tau_source_node>)(source);
	}

	sp_tau_node<Bool> make_binding(const sp_tau_node<Bool>& statement, const bindings<Bool>& bs) {
		true_predicate<sp_tau_node<Bool>> always;
		name_binder<Bool> nb(bs);
		bind_transformer<decltype(nb), Bool> binder(nb); 
		return post_order_traverser<decltype(binder), decltype(all<sp_tau_node<Bool>>), sp_tau_node<Bool>>(binder, all<sp_tau_node<Bool>>)(statement);
	}

	TEST_CASE("binding: given one statement with no bindigns, the binding process returns the same statement.") {
		static constexpr char* sample =	"bf_neg ( bf_neg ( $X ) ) = $X .";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_binding(statement, bs);
		CHECK( binded == statement );
	}

	TEST_CASE("binding: given one statement with one binding, the binding process returns the statement with the binding replaced.") {
		static constexpr char* sample =	"bf_neg ( bf_neg ( { binding } ) ) = { binding } .";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_binding(statement, bs);
		CHECK( binded != statement );
	}

	TEST_CASE("binding: given one statement with one non-matching binding, the binding process returns the original statement.") {
		static constexpr char* sample =	"bf_neg ( bf_neg ( { non_matching } ) ) = { non_matching } .";
		auto src = make_tau_source(sample);
		auto statement = make_statement(src);
		bindings<Bool> bs; bs["binding"] = { Bool(true) };
		auto binded = make_binding(statement, bs);
		CHECK( binded == statement );
	}
}

// TODO (HIGH) write tests for factory_binder
// TODO write tests for is_unresolved_predicate
// TODO write tests for is_resolved_predicate
// TODO write tests for is_unresolved
// TODO (HIGH) write tests for resolve_type
// TODO write tests for resolve_types (with rule)
// TODO write tests for resolve_types (with rules)

// TODO write tests for make rule
TEST_SUITE("make_library") {

	static constexpr char* sample =	"bf_neg ( bf_neg ( $X ) ) = $X .";

	TEST_CASE("one_rule") {
		auto src = make_tau_source(sample);
		auto lib = make_library<Bool>(src);
		CHECK( true );
	}
	// TODO add more unit tests
}

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

	// TODO add more unit tests
}

// TODO write tests for tau_apply (with rule)
// TODO write tests for tau_apply (with rules)
// TODO write tests for make_tau in the future


TEST_SUITE("make_tau_source") {

	/* static constexpr char* sample =	
		"bf_neg ( bf_neg ( $X ) ) = $X .";

	TEST_CASE("one_rule_n_main") {
		auto src = make_tau_source(sample);
		CHECK( true );
	}*/

	// TODO add more unit tests
}


