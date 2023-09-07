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

// IMPORTANT: To check the parser (and the underlying grammar) we don't need 
// any special boolean algebra, so we use Bool to simplify our testing.

// The following test suites represent correct sentences of a tau formula or 
// library and as such must be parsed correctly and satisfy the structural checks
// we perform on the parsed formula (as the rest of the code assumes such structure).

TEST_SUITE("parser: formula") {

	static constexpr char* sample =	
		"?X := ?X."
		"?Y := ?Y."
		" ( ?Z = 0 ) .";
	auto src = make_tau_source(sample);
	auto frml = make_statement(src);

	TEST_CASE("formula") {
		auto formula = frml | tau_parser::formula ;
		CHECK( formula.has_value() );
	}

	TEST_CASE("rules") {
		auto first_rules = frml | tau_parser::formula | tau_parser::rules ;
		CHECK( first_rules.has_value() );
		auto second_rules = frml | tau_parser::formula | tau_parser::rules | tau_parser::rules ;
		CHECK( second_rules.has_value() );
		auto empty_rules = second_rules | tau_parser::rules ;
		CHECK( empty_rules.has_value() );
		auto no_more_rules = empty_rules | tau_parser::rules ;
		CHECK( !no_more_rules.has_value() );
		auto no_more_rule = empty_rules | tau_parser::rule ;
		CHECK( !no_more_rule.has_value() );
	}

	TEST_CASE("rule") {
		auto first_rule = frml | tau_parser::formula | tau_parser::rules | tau_parser::rule;
		CHECK( first_rule.has_value() );
		auto second_rule = frml | tau_parser::formula | tau_parser::rules | tau_parser::rules |tau_parser::rule;
		CHECK( second_rule.has_value() );
	}

	TEST_CASE("main") {
		auto main = frml | tau_parser::formula | tau_parser::main;
		CHECK( main.has_value() );
	}
}

TEST_SUITE("parser: library") {

	static constexpr char* sample =	
		"?X := ?X."
		"?Y := ?Y.";
	auto src = make_tau_source(sample);
	auto lib = make_statement(src);

	TEST_CASE("library") {
		auto library = lib | tau_parser::library ;
		CHECK( library.has_value() );
	}

	TEST_CASE("rules") {
		auto first_rules = lib | tau_parser::library | tau_parser::rules ;
		CHECK( first_rules.has_value() );
		auto second_rules = lib | tau_parser::library | tau_parser::rules | tau_parser::rules ;
		CHECK( second_rules.has_value() );
		auto empty_rules = second_rules | tau_parser::rules ;
		CHECK( empty_rules.has_value() );
		auto no_more_rules = empty_rules | tau_parser::rules ;
		CHECK( !no_more_rules.has_value() );
		auto no_more_rule = empty_rules | tau_parser::rule ;
		CHECK( !no_more_rule.has_value() );
	}

	TEST_CASE("rule") {
		auto first_rule = lib | tau_parser::library | tau_parser::rules | tau_parser::rule;
		CHECK( first_rule.has_value() );
		auto second_rule = lib | tau_parser::library | tau_parser::rules | tau_parser::rules |tau_parser::rule;
		CHECK( second_rule.has_value() );
	}
}

// TODO (HIGH) test variables (capture, ignore, var, i_, o_...)
// TODO (HIGH) test indexes

// TODO (HIGH) test wwf rule parsing structure
TEST_SUITE("parser: wwf formulas ") {
	
	TEST_CASE("wff_neg") {
		static constexpr char* sample =	
			"wff_neg ( ?Z = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);		
		auto neg_formula = frml 
			| tau_parser::formula 
			| tau_parser::main 
			| tau_parser::wff 
			| tau_parser::wff_neg;
		CHECK( neg_formula.has_value() );
		// TODO (MEDIUM) add checks for the negated formula
	}

	TEST_CASE("wff_and") {
		static constexpr char* sample =	
			"(( ?Z = 0 ) wff_and ( ?X = 0 )).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);		
		auto and_formula = frml 
			| tau_parser::formula 
			| tau_parser::main 
			| tau_parser::wff 
			| tau_parser::wff_and;
		CHECK( and_formula.has_value() );
		// TODO (MEDIUM) add checks for the and formula
	}

	TEST_CASE("wff_or") {
		static constexpr char* sample =	
			"(( ?Z = 0 ) wff_or ( ?X = 0 )).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);		
		auto or_formula = frml 
			| tau_parser::formula 
			| tau_parser::main 
			| tau_parser::wff 
			| tau_parser::wff_or;
		CHECK( or_formula.has_value() );
		// TODO (MEDIUM) add checks for the or formula
	}

	TEST_CASE("wff_xor") {
		static constexpr char* sample =	
			"(( ?Z = 0 ) wff_xor ( ?X = 0 )).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);		
		auto xor_formula = frml 
			| tau_parser::formula 
			| tau_parser::main 
			| tau_parser::wff 
			| tau_parser::wff_xor;
		CHECK( xor_formula.has_value() );
		// TODO (MEDIUM) add checks for the xor formula
	}

	TEST_CASE("wff_eq") {
		static constexpr char* sample =	
			"( ?Z = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);		
		auto eq_formula = frml 
			| tau_parser::formula 
			| tau_parser::main 
			| tau_parser::wff 
			| tau_parser::wff_eq;
		CHECK( eq_formula.has_value() );
		// TODO (MEDIUM) add checks for the eq formula
	}

	TEST_CASE("wff_all") {
		static constexpr char* sample =	
			"wff_all ?Z ( ?Z = 0 ) .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);		
		auto all_formula = frml 
			| tau_parser::formula 
			| tau_parser::main 
			| tau_parser::wff 
			| tau_parser::wff_all;
		CHECK( all_formula.has_value() );
		// TODO (MEDIUM) add checks for the all formula
	}

	// TODO (MEDIUM) test wff_all with multiple variables

	TEST_CASE("wff_ex") {
		static constexpr char* sample =	
			"wff_ex ?Z ( ?Z = 0 ) .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);		
		auto ex_formula = frml 
			| tau_parser::formula 
			| tau_parser::main 
			| tau_parser::wff 
			| tau_parser::wff_ex;
		CHECK( ex_formula.has_value() );
		// TODO (MEDIUM) add checks for the ex formula
	}

	// TODO (MEDIUM) test wff_ex with multiple variables

	// TODO (HIGH) test wwf refs
	TEST_CASE("wff_ref") {
		CHECK( false );
	}
}

// TODO (HIGH) test cbf rule parsing structure
TEST_SUITE("parser: cbf formulas ") {

	TEST_CASE("cbf_neg") {
		/*static constexpr char* sample =	
			"?Z := cbf_neg ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);		
		auto neg_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::cbf_def
			| tau_parser::cbf
			| tau_parser::cbf_neg; 
		CHECK( neg_rule.has_value() );*/
		CHECK( false );
	}

	TEST_CASE("cbf_and") {
		/*static constexpr char* sample =	
			"?Z := ( ?Z cbf_and ?Z ).";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);		
		auto and_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::cbf_def
			| tau_parser::cbf
			| tau_parser::cbf_and; 
		CHECK( and_rule.has_value() );*/
		CHECK( false );
	}

	TEST_CASE("cbf_or") {
		CHECK( false );
	}

	TEST_CASE("cbf_xor") {
		CHECK( false );
	}

	TEST_CASE("cbf_if") {
		CHECK( false );
	}

	// TODO (HIGH) test cbf refs
	TEST_CASE("cbf_ref") {
		CHECK( false );
	}
}

// TODO (HIGH) test bf rule parsing structure
TEST_SUITE("parser: bf formulas ") {

	TEST_CASE("bf_neg") {
		static constexpr char* sample =	"bf_neg ?Z := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);		
		auto neg_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::bf_rule
			| tau_parser::bf_head
			| tau_parser::bf
			| tau_parser::bf_neg; 
		CHECK( neg_rule.has_value() );
	}

	TEST_CASE("bf_and") {
		static constexpr char* sample =	"(?Z bf_and ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto and_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::bf_rule
			| tau_parser::bf_head
			| tau_parser::bf
			| tau_parser::bf_and;
		CHECK( and_rule.has_value() );
	}

	TEST_CASE("bf_or") {
		static constexpr char* sample =	"(?Z bf_or ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto or_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::bf_rule
			| tau_parser::bf_head
			| tau_parser::bf
			| tau_parser::bf_or;
		CHECK( or_rule.has_value() );
	}

	TEST_CASE("bf_xor") {
		static constexpr char* sample =	"(?Z bf_xor ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto xor_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::bf_rule
			| tau_parser::bf_head
			| tau_parser::bf
			| tau_parser::bf_xor;
		CHECK( xor_rule.has_value() );
	}

	TEST_CASE("bf_all") {
		static constexpr char* sample =	"bf_all ?Z ?Z := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto all_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::bf_rule
			| tau_parser::bf_head
			| tau_parser::bf
			| tau_parser::bf_all;
		CHECK( all_rule.has_value() );
	}

	TEST_CASE("bf_ex") {
		static constexpr char* sample =	"bf_ex ?Z ?Z := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto ex_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::bf_rule
			| tau_parser::bf_head
			| tau_parser::bf
			| tau_parser::bf_ex;
		CHECK( ex_rule.has_value() );
	}
}

// TODO (HIGH) test source binding parsing structure
// TODO (HIGH) test named binding parsing structure
// TODO (HIGH) test source binding type parsing structure
// TODO (HIGH) test source binding source parsing structure
// TODO (HIGH) test unresolved source binding
// TODO (HIGH) test resolved source binding
TEST_SUITE("parser: bindings ") {

	TEST_CASE("named binding") {
		static constexpr char* sample =	"{ binding } := { binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto named = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding
			| tau_parser::named_binding;
		CHECK( named.has_value() );
	}

	TEST_CASE("source binding") {
		static constexpr char* sample =	"{ type : binding } := { type : binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto source = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding
			| tau_parser::source_binding;
		CHECK( source.has_value() );
	}

	TEST_CASE("source binding type") {
		static constexpr char* sample =	"{ type : binding } := { type : binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto type = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding
			| tau_parser::source_binding
			| tau_parser::type;
		CHECK( type.has_value() );
	}

	TEST_CASE("source binding source") {
		static constexpr char* sample =	"{ type : binding } := { type : binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto source = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding
			| tau_parser::source_binding
			| tau_parser::source;
		CHECK( source.has_value() );
	}

	TEST_CASE("unresolved source binding") {
		static constexpr char* sample =	"{  : binding } := {  : binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto type = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule
			| tau_parser::bf_rule
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
		static constexpr char* sample =	"{ type : binding } := { type : binding }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto type = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule
			| tau_parser::bf_rule
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

// TODO (HIGH) test and callback parsing structure
// TODO (HIGH) test or callback parsing structure
// TODO (HIGH) test xor callback parsing structure
// TODO (HIGH) test neg callback parsing structure
// TODO (HIGH) test subs callback parsing structure
TEST_SUITE("parser: callbacks ") {

	TEST_CASE("and callback") {
		CHECK( false );
	}

	TEST_CASE("or callback") {
		CHECK( false );
	}

	TEST_CASE("xor callback") {
		CHECK( false );
	}

	TEST_CASE("neg callback") {
		CHECK( false );
	}

	TEST_CASE("subs callback") {
		CHECK( false );
	}
}





