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
#define OUTPUT_APPLY_RULES

#include "../../src/doctest.h"
#include "../../src/defs.h"
#include "../../src/nso_rr.h"
#include "../../src/bool.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;
using namespace std;

namespace testing = doctest;

// IMPORTANT: To check the parser (and the underlying grammar) we don't need
// any special boolean algebra, so we use Bool to simplify our testing.

// The following test suites represent correct sentences of a tau nso_rr or
// library and as such must be parsed correctly and satisfy the structural checks
// we perform on the parsed nso_rr (as the rest of the code assumes such structure).

// TODO (LOW) why "$X := { : src_code } bf_and { bool : src_code }." is parsed
//
// improperly? bf | bf_constant | ...
//
// Check if still hapopens after the latest changes to the grammar.

TEST_SUITE("parsing basic nso_rr") {

	const char* sample =
		"f [0] (X) ::= X."
		"g [0] (Y) := T."
		" ( Z = 0 ) .";
	auto src = make_tau_source(sample);
	auto frml = make_statement(src);

	TEST_CASE("nso_rr main") {
		const char* sample =
			" ( Z = 0 ) .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
			auto nso_rr = frml | tau_parser::nso_rr | tau_parser::main ;
			CHECK( nso_rr.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation") {
		const char* sample =
			"g [0] (Y) ::= T."
			" T .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::nso_rr | tau_parser::rec_relations | tau_parser::rec_relation | tau_parser::wff_rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation") {
		const char* sample =
			"g [0] (Y) := 1."
			" T .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::nso_rr | tau_parser::rec_relations | tau_parser::rec_relation | tau_parser::bf_rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("main") {
		const char* sample =
			" T .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto main = frml | tau_parser::nso_rr | tau_parser::main;
		CHECK( main.has_value() );
	}
}

TEST_SUITE("parsing builders") {

	TEST_CASE("one capture") {
		const char* sample =	"( $X ) := ($X && $X).";
		auto src = make_builder<Bool>(sample);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("two capture") {
		const char* sample =	"( $X $Y ) := ($X && $Y).";
		auto src = make_builder<Bool>(sample);
		auto left = src.first;
		auto right = src.first;
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}
}

TEST_SUITE("parsing library") {

	const char* sample =
		"X := X."
		"Y := Y.";
	auto src = make_tau_source(sample);
	auto lib = make_statement(src);

	TEST_CASE("library") {
		auto library = lib | tau_parser::library ;
		CHECK( library.has_value() );
	}

	TEST_CASE("rules") {
		auto rules = lib | tau_parser::library | tau_parser::rules ;
		CHECK( rules.has_value());
	}

	TEST_CASE("rule") {
		auto rules = lib | tau_parser::library | tau_parser::rules || tau_parser::rule;
		CHECK( rules.size() == 2 );
	}
}

TEST_SUITE("parsing rewriting rules") {

	TEST_CASE("wff rewriting rules") {
		const char* sample = "T ::= T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rule = frml | tau_parser::library | tau_parser::rules | tau_parser::rule | tau_parser::wff_rule;
		CHECK( wff_rule.has_value() );
	}

	TEST_CASE("bf rewriting rules") {
		const char* sample = "1 := 1.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rule = frml | tau_parser::library | tau_parser::rules | tau_parser::rule | tau_parser::bf_rule;
		CHECK( bf_rule.has_value() );
	}
}

TEST_SUITE("parsing variables") {
	// done inderectly
}

TEST_SUITE("parsing captures") {
	// done inderectly
}

TEST_SUITE("parsing inputs") {
	// TODO (LOW) write tests for inputs parsing
}

TEST_SUITE("parsing outputs") {
	// TODO (LOW) write tests for outputs parsing
}

TEST_SUITE("parsing offsets"){
	// TODO (LOW) write tests for indexes parsing
}

TEST_SUITE("parsing wwf formulas ") {

	TEST_CASE("!") {
		const char* sample =	"! ( Z = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto neg_formula = frml
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_neg;
		CHECK( neg_formula.has_value() );
	}

	TEST_CASE("&&") {
		const char* sample =	"(( Z = 0 ) && ( X = 0 )).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto and_formula = frml
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_and;
		CHECK( and_formula.has_value() );
	}

	TEST_CASE("||") {
		const char* sample =	"(( Z = 0 ) || ( X = 0 )).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto or_formula = frml
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_or;
		CHECK( or_formula.has_value() );
	}

	TEST_CASE("^") {
		const char* sample =	"(( Z = 0 ) ^ ( X = 0 )).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto xor_formula = frml
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_xor;
		CHECK( xor_formula.has_value() );
	}

	TEST_CASE("?") {
		const char* sample =	"($Z ? $Z : $Z).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto xor_formula = frml
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_conditional;
		CHECK( xor_formula.has_value() );
	}

	TEST_CASE("= 0") {
		const char* sample =	"( Z = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto eq_formula = frml
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::bf_eq;
		CHECK( eq_formula.has_value() );
	}

	TEST_CASE("!= 0") {
		const char* sample =	"( Z != 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto eq_formula = frml
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::bf_neq;
		CHECK( eq_formula.has_value() );
	}
	TEST_CASE("<") {
		const char* sample =	"( Z < Z ).";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto less_rule = lib
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::bf_less;
		CHECK( less_rule.has_value() );
	}

	TEST_CASE("<=") {
		const char* sample =	"( Z <= Z ).";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto less_equal_rule = lib
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::bf_less_equal;
		CHECK( less_equal_rule.has_value() );
	}

	TEST_CASE(">") {
		const char* sample =	"(Z > Z).";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto greater_rule = lib
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::bf_greater;
		CHECK( greater_rule.has_value() );
	}

	TEST_CASE("->") {
		const char* sample =	"($Z -> $Z) ::= $Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto imply_rule = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule
			| tau_parser::wff_matcher
			| tau_parser::wff
			| tau_parser::wff_imply;
		CHECK( imply_rule.has_value() );
	}

	TEST_CASE("<->") {
		const char* sample =	"($Z <-> $Z) ::= $Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto equiv_rule = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule
			| tau_parser::wff_matcher
			| tau_parser::wff
			| tau_parser::wff_equiv;
		CHECK( equiv_rule.has_value() );
	}

	TEST_CASE("all") {
		const char* sample =
			"all Z ( Z = 0 ) .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto all_formula = frml
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_all;
		CHECK( all_formula.has_value() );
	}

	TEST_CASE("ex") {
		const char* sample =
			"ex Z ( Z = 0 ) .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto ex_formula = frml
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_ex;
		CHECK( ex_formula.has_value() );
	}

	TEST_CASE("ball") {
		const char* sample =
			"ball ?Z ?Z.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto all_formula = frml
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_ball;
		CHECK( all_formula.has_value() );
	}

	TEST_CASE("bex") {
		const char* sample =
			"bex ?Z ?Z.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto ex_formula = frml
			| tau_parser::nso_rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_bex;
		CHECK( ex_formula.has_value() );
	}

	// TODO (LOW) write test wwf refs
	TEST_CASE("wff_ref") {
		//CHECK( false );
	}
}

TEST_SUITE("parsing bf formulas ") {

	TEST_CASE("~") {
		const char* sample =	"~ Z := Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto neg_rule = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_neg;
		CHECK( neg_rule.has_value() );
	}

	TEST_CASE("&") {
		const char* sample =	"(Z & Z) := Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto and_rule = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_and;
		CHECK( and_rule.has_value() );
	}

	TEST_CASE("|") {
		const char* sample =	"(Z | Z) := Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto or_rule = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_or;
		CHECK( or_rule.has_value() );
	}

	TEST_CASE("+") {
		const char* sample =	"(Z + Z) := Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto xor_rule = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_xor;
		CHECK( xor_rule.has_value() );
	}

	TEST_CASE("fall") {
		const char* sample =	"fall Z Z := Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto all_rule = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_all;
		CHECK( all_rule.has_value() );
	}

	TEST_CASE("fex") {
		const char* sample =	"fex Z Z := Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto ex_rule = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_ex;
		CHECK( ex_rule.has_value() );
	}

	// TODO (LOW) write test bf refs
	TEST_CASE("bf_ref") {
		// CHECK( false );
	}
}

TEST_SUITE("parsing bindings ") {

	TEST_CASE("named binding") {
		const char* sample =	"{ binding } := { binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto named = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding
			| tau_parser::named_binding;
		CHECK( named.has_value() );
	}

	TEST_CASE("source binding") {
		const char* sample =	"{ type : binding } := { type : binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto source = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding
			| tau_parser::source_binding;
		CHECK( source.has_value() );
	}

	TEST_CASE("source binding type") {
		const char* sample =	"{ type : binding } := { type : binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto type = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding
			| tau_parser::source_binding
			| tau_parser::type;
		CHECK( type.has_value() );
	}

	TEST_CASE("source binding source") {
		const char* sample =	"{ type : binding } := { type : binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto source = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding
			| tau_parser::source_binding
			| tau_parser::source;
		CHECK( source.has_value() );
	}

	TEST_CASE("unresolved source binding") {
		const char* sample =	"{  : binding } := {  : binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto type = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding
			| tau_parser::source_binding
			| tau_parser::type
			| tau_parser::chars;
		CHECK( !type.has_value() );
	}

	TEST_CASE("resolved source binding") {
		const char* sample =	"{ type : binding } := { type : binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto type = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding
			| tau_parser::source_binding
			| tau_parser::type
			| tau_parser::chars;
		CHECK( type.has_value() );
	}
}

TEST_SUITE("parsing callbacks ") {

	TEST_CASE("bf_and_cb") {
		const char* sample =	"$X := { $X bf_and_cb $X }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto and_cb = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::bf_and_cb;
		CHECK( and_cb.has_value() );
	}

	TEST_CASE("bf_or_cb") {
		const char* sample =	"$X := { $X bf_or_cb $X }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto or_cb = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::bf_or_cb;
		CHECK( or_cb.has_value() );
	}

	TEST_CASE("bf_xor_cb") {
		const char* sample =	"$X := { $X bf_xor_cb $X }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto xor_cb = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::bf_xor_cb;
		CHECK( xor_cb.has_value() );
	}

	TEST_CASE("bf_neg_cb") {
		const char* sample =	"$X := { bf_neg_cb $X }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto neg_cb = lib
			| tau_parser::library
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::bf_neg_cb;
		CHECK( neg_cb.has_value() );
	}

	// TODO (MEDIUM) write tests for parsing bf_is_zero_cb
	// TODO (MEDIUM) write tests for parsing bf_is_one_cb
	// TODO (MEDIUM) write tests for parsing bf_has_clashing_subformulas_cb
	// TODO (MEDIUM) write tests for parsing bf_has_subformula_cb
	// TODO (MEDIUM) write tests for parsing bf_remove_fexistential_cb
	// TODO (MEDIUM) write tests for parsing bf_remove_funiversal_cb

	// TODO (MEDIUM) write tests for parsing bf_eq_cb
	// TODO (MEDIUM) write tests for parsing bf_neq_cb
	// TODO (MEDIUM) write tests for parsing wff_remove_existential_cb
	// TODO (MEDIUM) write tests for parsing wff_remove_bexistential_cb
	// TODO (MEDIUM) write tests for parsing wff_remove_buniversal_cb.
	// TODO (MEDIUM) write tests for parsing wff_has_clashing_subformulas_cb
	// TODO (MEDIUM) write tests for parsing wff_has_subformula_cb
	//
	// Callbacks are a crucial part of the execution of the normalizer and should
	// be tested properly. However, they are extensively tested inderectly in
	// other unit/integration tests. However, it is better to have explicit tests
	// for each of them.
}

TEST_SUITE("parsing rules") {
	// TODO (VERY_LOW) writes tests for rules parsing
	//
	// It is categorized as LOW because we have lot of tests parsing specific rules
}

TEST_SUITE("parsing rec relations") {
	// TODO (MEDIUM) writes tests for recursive relations execution
	//
	// The tests should range from simple direct substitutions to more complex
	// substitutions (involving multiple variables and indexes).
}