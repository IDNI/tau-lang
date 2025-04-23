// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "test_helpers.h"

using tau = tree<node<Bool>>;

// The following test suites represent correct sentences of a tau nso_rr or
// library and as such must be parsed correctly and satisfy the structural checks
// we perform on the parsed nso_rr (as the rest of the code assumes such structure).

// TODO (LOW) why "$X := { : src_code } bf_and { bool : src_code }." is parsed
//
// improperly? bf | bf_constant | ...
//
// Check if still hapopens after the latest changes to the grammar.

TEST_SUITE("parsing only main") {

	TEST_CASE("nso_rr main") {
		const char* sample = " Z = 0 .";
		auto fm = bmake_tt(sample);
		auto nso_rr = fm | tau::spec | tau::main;
		CHECK( nso_rr.has_value() );
	}
}

TEST_SUITE("parsing basic wff nso_rr") {

	TEST_CASE("nso_rr wff_rec_relation y1") {
		const char* sample =
			"g[0](Y) := T."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y2") {
		const char* sample =
			"g[0](Y) := T."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y3") {
		const char* sample =
			"g[N](Y) := T."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y4") {
		const char* sample =
			"g[N](Y) := T."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y5") {
		const char* sample =
			"g[N - 1](Y) := T."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y6") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[0](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y7") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[0](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y8") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y9") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y10") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y11") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}
}

TEST_SUITE("parsing basic bf nso_rr") {

	TEST_CASE("nso_rr bf_rec_relation y1") {
		const char* sample =
			"g[0](Y) := 1."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y2") {
		const char* sample =
			"g[0](Y) := 1."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y3") {
		const char* sample =
			"g[N](Y) := 1."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y4") {
		const char* sample =
			"g[N](Y) := 1."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y5") {
		const char* sample =
			"g[N - 1](Y) := 1."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y6") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[0](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y7") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[0](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y8") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y9") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y10") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y11") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations
			| tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}
}

TEST_SUITE("parsing basic wff nso_rr in main") {

	TEST_CASE("nso_rr wff_rec_relation in main y1") {
		const char* sample =
			"g[0](Y) := T."
			"g[0](Y).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y2") {
		const char* sample =
			"g[0](Y) := T."
			"g[0](Y).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y3") {
		const char* sample =
			"g[0](Y) := T."
			"g[N](Y).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y4") {
		const char* sample =
			"g[0](Y) := T."
			"g[N](Y).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y5") {
		const char* sample =
			"g[0](Y) := T."
			"g[N - 1](Y).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y6") {
		const char* sample =
			"g[0](Y) := T."
			"g[N - 1](Y).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}
}

TEST_SUITE("parsing basic bf nso_rr in main") {

	TEST_CASE("nso_rr bf_rec_relation in main y1") {
		const char* sample =
			"(g[0, 0](Y) = 0).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y2") {
		const char* sample =
			"(g[0, 0](Y) = 0).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y3") {
		const char* sample =
			"(g[N, 0](Y) = 0).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y4") {
		const char* sample =
			"(g[N, 0](Y) = 0).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y5") {
		const char* sample =
			"(g[N - 1](Y) = 0).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y6") {
		const char* sample =
			"(g[N - 1](Y) = 0).";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::main;
		CHECK( bf_rec_relation.has_value() );
	}

}

TEST_SUITE("parsing basic wff nso_rr (2d): 1th var check") {

	TEST_CASE("nso_rr wff_rec_relation y1") {
		const char* sample =
			"g[0, 0](Y) := T."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y2") {
		const char* sample =
			"g[0, 0](Y) := T."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y3") {
		const char* sample =
			"g[N, 0](Y) := T."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y4") {
		const char* sample =
			"g[N, 0](Y) := T."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y5") {
		const char* sample =
			"g[N - 1, 0](Y) := T."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y6") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y7") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y8") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y9") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y10") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y11") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto wff_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}
}

TEST_SUITE("parsing basic bf nso_rr (2d): 1th var check") {

	TEST_CASE("nso_rr bf_rec_relation y1") {
		const char* sample =
			"g[0, 0](Y) := 1."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y2") {
		const char* sample =
			"g[0, 0](Y) := 1."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y3") {
		const char* sample =
			"g[N, 0](Y) := 1."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y4") {
		const char* sample =
			"g[N, 0](Y) := 1."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y5") {
		const char* sample =
			"g[N - 1, 0](Y) := 1."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y6") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y7") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y8") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y9") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y10") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y11") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		auto fm = bmake_tt(sample);
		auto bf_rec_relation = fm | tau::spec | tau::rec_relations | tau::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}
}

// TODO (MEDIUM) ass tests for multiple arguments in functions and rec relation wwf
// TODO (MEDIUM) ass tests for multiple arguments in functions and rec relation bf

TEST_SUITE("parsing builders") {

	TEST_CASE("one capture") {
		const char* sample = "( $X ) =:: ($X && $X).";
		auto bldr = tau::get_builder(sample);
		CHECK( tau::get(bldr.first).is(tau::builder_head) );
		CHECK( tau::get(bldr.second).is(tau::wff) );
	}

	TEST_CASE("two capture") {
		const char* sample = "( $X $Y ) =:: ($X && $Y).";
		auto bldr = tau::get_builder(sample);
		CHECK( tau::get(bldr.first).is(tau::builder_head) );
		CHECK( tau::get(bldr.second).is(tau::wff) );
	}
}

TEST_SUITE("parsing library") {

	TEST_CASE("one rule") {
		const char* sample = "X := X.";
		auto lib = bmake_tt(sample, { .start = tau::library });
		auto rules = lib | tau::rules || tau::rule;
		CHECK( rules.size() == 1 );
	}

	TEST_CASE("two rule") {
		const char* sample =
			"X := X."
			"Y := Y.";
		auto lib = bmake_tt(sample, { .start = tau::library });
		auto rules = lib | tau::rules || tau::rule;
		CHECK( rules.size() == 2 );
	}
}

TEST_SUITE("parsing rewriting rules") {

	TEST_CASE("wff rewriting rules") {
		const char* sample = "T ::= T.";
		auto lib = bmake_tt(sample, { .start = tau::library });
		auto wff_rule = lib | tau::rules | tau::rule | tau::wff_rule;
		CHECK( wff_rule.has_value() );
	}

	TEST_CASE("bf rewriting rules") {
		const char* sample = "1 := 1.";
		auto lib = bmake_tt(sample, { .start = tau::library });
		auto bf_rule = lib | tau::rules | tau::rule | tau::bf_rule;
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

	TEST_CASE("fix loopback") {
		const char* sample = "( i1[0] = 0 ).";
		auto fm = bmake_tt(sample);
		auto input = fm | tau::spec | tau::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("capture loopback") {
		const char* sample = "( i1[n] = 0 ).";
		auto fm = bmake_tt(sample);
		auto input = fm | tau::spec | tau::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("variable loopback") {
		const char* sample = "( i1[t] = 0 ).";
		auto fm = bmake_tt(sample);
		auto input = fm | tau::spec | tau::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("shift capture loopback") {
		const char* sample = "( i1[n - 1] = 0 ).";
		auto fm = bmake_tt(sample);
		auto input = fm | tau::spec | tau::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("shift variable loopback") {
		const char* sample = "( i1[t - 1] = 0 ).";
		auto fm = bmake_tt(sample);
		auto input = fm | tau::spec | tau::main;
		CHECK( input.has_value() );
	}
}

TEST_SUITE("parsing outputs") {

	TEST_CASE("fix loopback") {
		const char* sample = "( o1[0] = 0 ).";
		auto fm = bmake_tt(sample);
		auto input = fm | tau::spec | tau::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("capture loopback") {
		const char* sample = "( o1[n] = 0 ).";
		auto fm = bmake_tt(sample);
		auto input = fm | tau::spec | tau::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("variable loopback") {
		const char* sample = "( o1[t] = 0 ).";
		auto fm = bmake_tt(sample);
		auto input = fm | tau::spec | tau::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("shift capture loopback") {
		const char* sample = "( o1[n - 1] = 0 ).";
		auto fm = bmake_tt(sample);
		auto input = fm | tau::spec | tau::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("shift variable loopback") {
		const char* sample = "( o1[t - 1] = 0 ).";
		auto fm = bmake_tt(sample);
		auto input = fm | tau::spec | tau::main;
		CHECK( input.has_value() );
	}
}

TEST_SUITE("parsing offsets"){
	// done inderectly
}

TEST_SUITE("parsing wwf formulas ") {

	TEST_CASE("!") {
		const char* sample = "! ( Z = 0 ).";
		auto fm = bmake_tt(sample);
		auto neg_formula = fm | tau::spec | tau::main | tau::wff
								| tau::wff_neg;
		CHECK( neg_formula.has_value() );
	}

	TEST_CASE("&&") {
		const char* sample = "( Z = 0 ) && ( X = 0 ).";
		auto fm = bmake_tt(sample);
		auto and_formula = fm | tau::spec | tau::main | tau::wff
								| tau::wff_and;
		CHECK( and_formula.has_value() );
	}

	TEST_CASE("||") {
		const char* sample = "( Z = 0 ) || ( X = 0 ).";
		auto fm = bmake_tt(sample);
		auto or_formula = fm | tau::spec | tau::main | tau::wff
								| tau::wff_or;
		CHECK( or_formula.has_value() );
	}

	TEST_CASE("^") {
		const char* sample = "( Z = 0 ) ^ ( X = 0 ).";
		auto fm = bmake_tt(sample);
		auto xor_formula = fm | tau::spec | tau::main | tau::wff
								| tau::wff_xor;
		CHECK( xor_formula.has_value() );
	}

	TEST_CASE("?") {
		const char* sample = "$X ? $Y : $Z.";
		auto fm = bmake_tt(sample);
		auto cond_formula = fm | tau::spec | tau::main | tau::wff
							| tau::wff_conditional;
		CHECK( cond_formula.has_value() );
	}

	TEST_CASE("= 0") {
		const char* sample = " Z = 0 .";
		auto fm = bmake_tt(sample);
		auto eq_formula = fm | tau::spec | tau::main | tau::wff
								| tau::bf_eq;
		CHECK( eq_formula.has_value() );
	}

	TEST_CASE("!= 0") {
		const char* sample = " Z != 0 .";
		auto fm = bmake_tt(sample);
		auto neq_formula = fm | tau::spec | tau::main | tau::wff
								| tau::bf_neq;
		CHECK( neq_formula.has_value() );
	}

	TEST_CASE("<") {
		const char* sample = " X < Y .";
		auto fm = bmake_tt(sample);
		auto less_formula = fm | tau::spec | tau::main | tau::wff
								| tau::bf_less;
		CHECK( less_formula.has_value() );
	}

	TEST_CASE("<=") {
		const char* sample = " X <= Y .";
		auto fm = bmake_tt(sample);
		auto less_equal_formula = fm | tau::spec | tau::main | tau::wff
							| tau::bf_less_equal;
		CHECK( less_equal_formula.has_value() );
	}

	TEST_CASE(">") {
		const char* sample = "X > Y.";
		auto fm = bmake_tt(sample);
		auto greater_formula = fm | tau::spec | tau::main | tau::wff
							| tau::bf_greater;
		CHECK( greater_formula.has_value() );
	}

	TEST_CASE("->") {
		const char* sample = "$Z -> $X ::= $Z.";
		auto lib = bmake_tt(sample, { .start = tau_parser::library });
		auto imply_rule = lib | tau::rules | tau::rule | tau::wff_rule
				| tau::wff_matcher | tau::wff | tau::wff_imply;
		CHECK( imply_rule.has_value() );
	}

	TEST_CASE("<->") {
		const char* sample = "$Z <-> $X ::= $Z.";
		auto lib = bmake_tt(sample, { .start = tau_parser::library });
		auto equiv_rule = lib | tau::rules | tau::rule | tau::wff_rule
				| tau::wff_matcher | tau::wff | tau::wff_equiv;
		CHECK( equiv_rule.has_value() );
	}

	TEST_CASE("all") {
		const char* sample =
			"all Z ( Z = 0 ) .";
		auto fm = bmake_tt(sample);
		auto all_formula = fm | tau::spec | tau::main | tau::wff
								| tau::wff_all;
		CHECK( all_formula.has_value() );
	}

	TEST_CASE("ex") {
		const char* sample =
			"ex Z ( Z = 0 ) .";
		auto fm = bmake_tt(sample);
		auto ex_formula = fm | tau::spec | tau::main | tau::wff
								| tau::wff_ex;
		CHECK( ex_formula.has_value() );
	}
}

TEST_SUITE("parsing bf formulas ") {

	TEST_CASE("'") {
		const char* sample = "Z' := Z.";
		auto lib = bmake_tt(sample, { .start = tau_parser::library });
		auto neg_rule = lib | tau::rules | tau::rule | tau::bf_rule
				| tau::bf_matcher | tau::bf | tau::bf_neg;
		CHECK( neg_rule.has_value() );
	}

	TEST_CASE("&") {
		const char* sample = "Z & X := Z.";
		auto lib = bmake_tt(sample, { .start = tau_parser::library });
		auto and_rule = lib | tau::rules | tau::rule | tau::bf_rule
				| tau::bf_matcher | tau::bf | tau::bf_and;
		CHECK( and_rule.has_value() );
	}

	TEST_CASE("|") {
		const char* sample = "Z | X := Z.";
		auto lib = bmake_tt(sample, { .start = tau_parser::library });
		auto or_rule = lib | tau::rules | tau::rule | tau::bf_rule
				| tau::bf_matcher | tau::bf | tau::bf_or;
		CHECK( or_rule.has_value() );
	}

	TEST_CASE("+") {
		const char* sample = "Z + X := Z.";
		auto lib = bmake_tt(sample, { .start = tau_parser::library });
		auto xor_rule = lib | tau::rules | tau::rule | tau::bf_rule
				| tau::bf_matcher | tau::bf | tau::bf_xor;
		CHECK( xor_rule.has_value() );
	}
}

TEST_SUITE("regression/disallow T and F as variables") {

	TEST_CASE("T is not a variable") {
		const char* sample =
			"T = 0.";
		auto src = bmake(sample);
		CHECK( src == nullptr );
	}

	TEST_CASE("F is not a variable") {
		const char* sample =
			"F = 0.";
		auto src = bmake(sample);
		CHECK( src == nullptr );
	}

}


// TODO (VERY_LOW) write tests for parsing bf_is_one_cb
// TODO (VERY_LOW) write tests for parsing bf_has_clashing_subformulas_cb
// TODO (VERY_LOW) write tests for parsing bf_has_subformula_cb
// TODO (VERY_LOW) write tests for parsing bf_remove_fexistential_cb
// TODO (VERY_LOW) write tests for parsing bf_remove_funiversal_cb
// TODO (VERY_LOW) write tests for parsing wff_remove_existential_cb
// TODO (VERY_LOW) write tests for parsing wff_remove_bexistential_cb
// TODO (VERY_LOW) write tests for parsing wff_remove_buniversal_cb.
// TODO (VERY_LOW) write tests for parsing wff_has_clashing_subformulas_cb
// TODO (VERY_LOW) write tests for parsing wff_has_subformula_cb
//
// Callbacks are a crucial part of the execution of the normalizer and should
// be tested properly. However, they are extensively tested inderectly in
// other unit/integration tests. In any case, it is better to have explicit tests
// for each of them parsing.

TEST_SUITE("parsing rules") {
	// TODO (VERY_LOW) writes tests for rules parsing
	//
	// It is categorized as VERY_LOW because we have lot of tests parsing specific rules
}
