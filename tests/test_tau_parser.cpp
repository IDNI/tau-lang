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

// TODO (HIGH) why "$X := { : src_code } bf_and { bool : src_code }." is parsed
// improperly? bf | bf_constant | ...

TEST_SUITE("parser: formula") {

	static constexpr char* sample =	
		"?X := ?X."
		"?Y := ?Y."
		" ( ?Z == F ) .";
	auto src = make_tau_source(sample);
	auto frml = make_statement(src);

	TEST_CASE("formula") {
		auto formula = frml | tau_parser::formula ;
		CHECK( formula.has_value() );
	}

	TEST_CASE("rules") {
		auto rules = frml | tau_parser::formula | tau_parser::rules ;
		CHECK( rules.has_value() );
	}

	TEST_CASE("rule") {
		auto rules = frml | tau_parser::formula | tau_parser::rules || tau_parser::rule;
		CHECK( rules.size() == 2 );
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
		auto rules = lib | tau_parser::library | tau_parser::rules ;
		CHECK( rules.has_value());
	}

	TEST_CASE("rule") {
		auto rules = lib | tau_parser::library | tau_parser::rules || tau_parser::rule;
		CHECK( rules.size() == 2 );
	}
}

// TODO (HIGH) test variables (capture, ignore, var, i_, o_...)
// TODO (HIGH) test indexes

TEST_SUITE("parser: wwf formulas ") {
	
	TEST_CASE("wff_neg") {
		static constexpr char* sample =	
			"wff_neg ( ?Z == F ).";
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
			"(( ?Z == F ) wff_and ( ?X == F )).";
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
			"(( ?Z == F ) wff_or ( ?X == F )).";
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
			"(( ?Z == F ) wff_xor ( ?X == F )).";
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
			"( ?Z == F ).";
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

	TEST_CASE("wff_imply") {
		static constexpr char* sample =	"(?Z wff_imply ?Z) := ?Z.";
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

	TEST_CASE("wff_coimply") {
		static constexpr char* sample =	"(?Z wff_coimply ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto coimply_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::wff_rule
			| tau_parser::wff_matcher
			| tau_parser::wff
			| tau_parser::wff_coimply;
		CHECK( coimply_rule.has_value() );
	}

	TEST_CASE("wff_equiv") {
		static constexpr char* sample =	"(?Z wff_equiv ?Z) := ?Z.";
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

	TEST_CASE("wff_all") {
		static constexpr char* sample =	
			"wff_all ?Z ( ?Z == F ) .";
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
			"wff_ex ?Z ( ?Z == F ) .";
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

	// TODO (MEDIUM) test wwf refs
	TEST_CASE("wff_ref") {
		//CHECK( false );
	}
}

TEST_SUITE("parser: cbf formulas ") {

	TEST_CASE("cbf_neg") {
		static constexpr char* sample =	"?Z := cbf_neg ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);		
		auto neg_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::cbf_rule
			| tau_parser::cbf
			| tau_parser::cbf_neg; 
		CHECK( neg_rule.has_value() );
		// TODO (MEDIUM) add checks for the neg formula
	}

	TEST_CASE("cbf_and") {
		static constexpr char* sample =	"?Z := ( ?Z cbf_and ?Z ).";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);		
		auto and_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::cbf_rule
			| tau_parser::cbf
			| tau_parser::cbf_and; 
		CHECK( and_rule.has_value() );
		// TODO (MEDIUM) add checks for the and formula
	}

	TEST_CASE("cbf_or") {
		static constexpr char* sample =	"?Z := ( ?Z cbf_or ?Z ).";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);		
		auto or_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::cbf_rule
			| tau_parser::cbf
			| tau_parser::cbf_or; 
		CHECK( or_rule.has_value() );
		// TODO (MEDIUM) add checks for the or formula
	}

	TEST_CASE("cbf_xor") {
		static constexpr char* sample =	"?Z := ( ?Z cbf_xor ?Z ).";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);		
		auto xor_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::cbf_rule
			| tau_parser::cbf
			| tau_parser::cbf_xor; 
		CHECK( xor_rule.has_value() );
		// TODO (MEDIUM) add checks for the xor formula
	}


	TEST_CASE("cbf_imply") {
		static constexpr char* sample =	"(?Z cbf_imply ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto imply_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::cbf_rule
			| tau_parser::cbf_matcher
			| tau_parser::cbf
			| tau_parser::cbf_imply;
		CHECK( imply_rule.has_value() );
	}

	TEST_CASE("cbf_coimply") {
		static constexpr char* sample =	"(?Z cbf_coimply ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto coimply_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::cbf_rule
			| tau_parser::cbf_matcher
			| tau_parser::cbf
			| tau_parser::cbf_coimply;
		CHECK( coimply_rule.has_value() );
	}

	TEST_CASE("cbf_equiv") {
		static constexpr char* sample =	"(?Z cbf_equiv ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto equiv_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::cbf_rule
			| tau_parser::cbf_matcher
			| tau_parser::cbf
			| tau_parser::cbf_equiv;
		CHECK( equiv_rule.has_value() );
	}

	TEST_CASE("cbf_if") {
		static constexpr char* sample =	"if $Y then $Z  else $Z := $X.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);		
		auto if_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::wff_rule
			| tau_parser::wff_matcher
			| tau_parser::cbf_if; 
		CHECK( if_rule.has_value() );
		// TODO (MEDIUM) add checks for the if formula
	}

	// TODO (MEDIUM) test cbf refs
	TEST_CASE("cbf_ref") {
		// CHECK( false );
	}
}

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
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_neg; 
		CHECK( neg_rule.has_value() );
		// TODO (MEDIUM) add checks for the neg formula
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
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_and;
		CHECK( and_rule.has_value() );
		// TODO (MEDIUM) add checks for the and formula
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
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_or;
		CHECK( or_rule.has_value() );
		// TODO (MEDIUM) add checks for the or formula
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
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_xor;
		CHECK( xor_rule.has_value() );
		// TODO (MEDIUM) add checks for the xor formula
	}

	TEST_CASE("bf_less") {
		static constexpr char* sample =	"(?Z bf_less ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto less_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_less;
		CHECK( less_rule.has_value() );
	}

	TEST_CASE("bf_less_equal") {
		static constexpr char* sample =	"(?Z bf_less_equal ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto less_equal_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_less_equal;
		CHECK( less_equal_rule.has_value() );
	}

	TEST_CASE("bf_greater") {
		static constexpr char* sample =	"(?Z bf_greater ?Z) := ?Z.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto greater_rule = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule 
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_greater;
		CHECK( greater_rule.has_value() );
	}

	TEST_CASE("bf_all") {
		static constexpr char* sample =	"bf_all ?Z $Z := ?Z.";
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
		// TODO (MEDIUM) add checks for the all formula
	}

	// TODO (MEDIUM) test bf_all with multiple variables

	TEST_CASE("bf_ex") {
		static constexpr char* sample =	"bf_ex ?Z $Z := $Z.";
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
		// TODO (MEDIUM) add checks for the ex formula
	}

	// TODO (MEDIUM) test bf_ex with multiple variables
}

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

TEST_SUITE("parser: callbacks ") {

	TEST_CASE("bf_and_cb") {
		static constexpr char* sample =	"$X := { $X bf_and_cb $X }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto and_cb = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::bf_and_cb;
		CHECK( and_cb.has_value() );
	}

	TEST_CASE("bf_or_cb") {
		static constexpr char* sample =	"$X := { $X bf_or_cb $X }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto or_cb = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::bf_or_cb;
		CHECK( or_cb.has_value() );
	}

	TEST_CASE("bf_xor_cb") {
		static constexpr char* sample =	"$X := { $X bf_xor_cb $X }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto xor_cb = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::bf_xor_cb;
		CHECK( xor_cb.has_value() );
	}

	TEST_CASE("bf_neg_cb") {
		static constexpr char* sample =	"$X := { bf_neg_cb $X }.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto neg_cb = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::bf_neg_cb;
		CHECK( neg_cb.has_value() );
	}

	TEST_CASE("bf_subs_cb") {
		static constexpr char* sample =	"$X := bf_subs_cb $X $X $X.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto subs_cb = lib 
			| tau_parser::library 
			| tau_parser::rules 
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf
			| tau_parser::bf_subs_cb;
		CHECK( subs_cb.has_value() );
	}
}





