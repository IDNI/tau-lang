// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;
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

TEST_SUITE("parsing only main") {

	TEST_CASE("nso_rr main") {
		const char* sample =
			" Z = 0 .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
			auto nso_rr = frml | tau_parser::rr | tau_parser::main;
			CHECK( nso_rr.has_value() );
	}
}

TEST_SUITE("parsing basic wff nso_rr") {

	TEST_CASE("nso_rr wff_rec_relation y1") {
		const char* sample =
			"g[0](Y) := T."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y2") {
		const char* sample =
			"g[0](Y) := T."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y3") {
		const char* sample =
			"g[N](Y) := T."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y4") {
		const char* sample =
			"g[N](Y) := T."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y5") {
		const char* sample =
			"g[N - 1](Y) := T."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y6") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[0](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y7") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[0](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y8") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y9") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y10") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y11") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}
}

TEST_SUITE("parsing basic bf nso_rr") {

	TEST_CASE("nso_rr bf_rec_relation y1") {
		const char* sample =
			"g[0](Y) := 1."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y2") {
		const char* sample =
			"g[0](Y) := 1."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y3") {
		const char* sample =
			"g[N](Y) := 1."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y4") {
		const char* sample =
			"g[N](Y) := 1."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y5") {
		const char* sample =
			"g[N - 1](Y) := 1."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y6") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[0](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y7") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[0](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y8") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y9") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y10") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y11") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}
}

TEST_SUITE("parsing basic wff nso_rr in main") {

	TEST_CASE("nso_rr wff_rec_relation in main y1") {
		const char* sample =
			"g[0](Y) := T."
			"g[0](Y).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y2") {
		const char* sample =
			"g[0](Y) := T."
			"g[0](Y).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y3") {
		const char* sample =
			"g[0](Y) := T."
			"g[N](Y).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y4") {
		const char* sample =
			"g[0](Y) := T."
			"g[N](Y).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y5") {
		const char* sample =
			"g[0](Y) := T."
			"g[N - 1](Y).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y6") {
		const char* sample =
			"g[0](Y) := T."
			"g[N - 1](Y).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}
}

TEST_SUITE("parsing basic bf nso_rr in main") {

	TEST_CASE("nso_rr bf_rec_relation in main y1") {
		const char* sample =
			"(g[0, 0](Y) = 0).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y2") {
		const char* sample =
			"(g[0, 0](Y) = 0).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y3") {
		const char* sample =
			"(g[N, 0](Y) = 0).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y4") {
		const char* sample =
			"(g[N, 0](Y) = 0).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y5") {
		const char* sample =
			"(g[N - 1](Y) = 0).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y6") {
		const char* sample =
			"(g[N - 1](Y) = 0).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::main;
		CHECK( bf_rec_relation.has_value() );
	}

}

TEST_SUITE("parsing basic wff nso_rr (2d): 1th var check") {

	TEST_CASE("nso_rr wff_rec_relation y1") {
		const char* sample =
			"g[0, 0](Y) := T."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y2") {
		const char* sample =
			"g[0, 0](Y) := T."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y3") {
		const char* sample =
			"g[N, 0](Y) := T."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y4") {
		const char* sample =
			"g[N, 0](Y) := T."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y5") {
		const char* sample =
			"g[N - 1, 0](Y) := T."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y6") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y7") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y8") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y9") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y10") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y11") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto wff_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( wff_rec_relation.has_value() );
	}
}

TEST_SUITE("parsing basic bf nso_rr (2d): 1th var check") {

	TEST_CASE("nso_rr bf_rec_relation y1") {
		const char* sample =
			"g[0, 0](Y) := 1."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y2") {
		const char* sample =
			"g[0, 0](Y) := 1."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y3") {
		const char* sample =
			"g[N, 0](Y) := 1."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y4") {
		const char* sample =
			"g[N, 0](Y) := 1."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y5") {
		const char* sample =
			"g[N - 1, 0](Y) := 1."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y6") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y7") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y8") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y9") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y10") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y11") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto bf_rec_relation = frml | tau_parser::rr | tau_parser::rec_relations | tau_parser::rec_relation;
		CHECK( bf_rec_relation.has_value() );
	}
}

// TODO (MEDIUM) ass tests for multiple arguments in functions and rec relation wwf
// TODO (MEDIUM) ass tests for multiple arguments in functions and rec relation bf

TEST_SUITE("parsing builders") {

	TEST_CASE("one capture") {
		const char* sample = "( $X ) =:: ($X && $X).";
		auto src = make_builder<Bool>(sample);
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}

	TEST_CASE("two capture") {
		const char* sample = "( $X $Y ) =:: ($X && $Y).";
		auto src = make_builder<Bool>(sample);
		auto left = src.first;
		auto right = src.first;
		CHECK( is_non_terminal<tau_parser::builder_head, Bool>(src.first) );
		CHECK( is_non_terminal<tau_parser::wff, Bool>(src.second) );
	}
}

TEST_SUITE("parsing library") {

	TEST_CASE("one rule") {
	const char* sample =
		"X := X.";
	auto src = make_tau_source(sample, { .start = tau_parser::library });
	auto lib = make_statement(src);
		auto rules = lib | tau_parser::rules || tau_parser::rule;
		CHECK( rules.size() == 1 );
	}

	TEST_CASE("two rule") {
	const char* sample =
		"X := X."
		"Y := Y.";
	auto src = make_tau_source(sample, { .start = tau_parser::library });
	auto lib = make_statement(src);
		auto rules = lib | tau_parser::rules || tau_parser::rule;
		CHECK( rules.size() == 2 );
	}
}

TEST_SUITE("parsing rewriting rules") {

	TEST_CASE("wff rewriting rules") {
		const char* sample = "T ::= T.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto frml = make_statement(src);
		auto wff_rule = frml | tau_parser::rules | tau_parser::rule | tau_parser::wff_rule;
		CHECK( wff_rule.has_value() );
	}

	TEST_CASE("bf rewriting rules") {
		const char* sample = "1 := 1.";
		auto src = make_tau_source(sample, { .start = tau_parser::library });
		auto frml = make_statement(src);
		auto bf_rule = frml | tau_parser::rules | tau_parser::rule | tau_parser::bf_rule;
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
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto input = frml | tau_parser::rr | tau_parser::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("capture loopback") {
		const char* sample = "( i1[n] = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto input = frml | tau_parser::rr | tau_parser::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("variable loopback") {
		const char* sample = "( i1[t] = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto input = frml | tau_parser::rr | tau_parser::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("shift capture loopback") {
		const char* sample = "( i1[n - 1] = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto input = frml | tau_parser::rr | tau_parser::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("shift variable loopback") {
		const char* sample = "( i1[t - 1] = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto input = frml | tau_parser::rr | tau_parser::main;
		CHECK( input.has_value() );
	}
}

TEST_SUITE("parsing outputs") {

	TEST_CASE("fix loopback") {
		const char* sample = "( o1[0] = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto input = frml | tau_parser::rr | tau_parser::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("capture loopback") {
		const char* sample = "( o1[n] = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto input = frml | tau_parser::rr | tau_parser::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("variable loopback") {
		const char* sample = "( o1[t] = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto input = frml | tau_parser::rr | tau_parser::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("shift capture loopback") {
		const char* sample = "( o1[n - 1] = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto input = frml | tau_parser::rr | tau_parser::main;
		CHECK( input.has_value() );
	}

	TEST_CASE("shift variable loopback") {
		const char* sample = "( o1[t - 1] = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto input = frml | tau_parser::rr | tau_parser::main;
		CHECK( input.has_value() );
	}
}

TEST_SUITE("parsing offsets"){
	// done inderectly
}

TEST_SUITE("parsing wwf formulas ") {

	TEST_CASE("!") {
		const char* sample = "! ( Z = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto neg_formula = frml
			| tau_parser::rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_neg;
		CHECK( neg_formula.has_value() );
	}

	TEST_CASE("&&") {
		const char* sample = "( Z = 0 ) && ( X = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto and_formula = frml
			| tau_parser::rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_and;
		CHECK( and_formula.has_value() );
	}

	TEST_CASE("||") {
		const char* sample = "( Z = 0 ) || ( X = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto or_formula = frml
			| tau_parser::rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_or;
		CHECK( or_formula.has_value() );
	}

	TEST_CASE("^") {
		const char* sample = "( Z = 0 ) ^ ( X = 0 ).";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto xor_formula = frml
			| tau_parser::rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_or;
		CHECK( xor_formula.has_value() );
	}

	TEST_CASE("?") {
		const char* sample = "$X ? $Y : $Z.";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto xor_formula = frml
			| tau_parser::rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_and;
		CHECK( xor_formula.has_value() );
	}

	TEST_CASE("= 0") {
		const char* sample = " Z = 0 .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto eq_formula = frml
			| tau_parser::rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::bf_eq;
		CHECK( eq_formula.has_value() );
	}

	TEST_CASE("!= 0") {
		const char* sample = " Z != 0 .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto eq_formula = frml
			| tau_parser::rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::bf_neq;
		CHECK( eq_formula.has_value() );
	}

	TEST_CASE("<") {
		const char* sample = " X < Y .";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto less_rule = lib
			| tau_parser::rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_and;
		CHECK( less_rule.has_value() );
	}

	TEST_CASE("<=") {
		const char* sample = " X <= Y .";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto less_equal_rule = lib
			| tau_parser::rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::bf_eq;
		CHECK( less_equal_rule.has_value() );
	}

	TEST_CASE(">") {
		const char* sample = "X > Y.";
		auto src = make_tau_source(sample);
		auto lib = make_statement(src);
		auto greater_rule = lib
			| tau_parser::rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_and;
		CHECK( greater_rule.has_value() );
	}

	TEST_CASE("->") {
		const char* sample = "$Z -> $X ::= $Z.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto imply_rule = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule
			| tau_parser::wff_matcher
			| tau_parser::wff
			| tau_parser::wff_or;
		CHECK( imply_rule.has_value() );
	}

	TEST_CASE("<->") {
		const char* sample = "$Z <-> $X ::= $Z.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto equiv_rule = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::wff_rule
			| tau_parser::wff_matcher
			| tau_parser::wff
			| tau_parser::wff_and;
		CHECK( equiv_rule.has_value() );
	}

	TEST_CASE("all") {
		const char* sample =
			"all Z ( Z = 0 ) .";
		auto src = make_tau_source(sample);
		auto frml = make_statement(src);
		auto all_formula = frml
			| tau_parser::rr
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
			| tau_parser::rr
			| tau_parser::main
			| tau_parser::wff
			| tau_parser::wff_ex;
		CHECK( ex_formula.has_value() );
	}
}

TEST_SUITE("parsing bf formulas ") {

	TEST_CASE("'") {
		const char* sample = "Z' := Z.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto neg_rule = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_neg;
		CHECK( neg_rule.has_value() );
	}

	TEST_CASE("&") {
		const char* sample = "Z & X := Z.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto and_rule = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_and;
		CHECK( and_rule.has_value() );
	}

	TEST_CASE("|") {
		const char* sample = "Z | X := Z.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto or_rule = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_or;
		CHECK( or_rule.has_value() );
	}

	TEST_CASE("+") {
		const char* sample = "Z + X := Z.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto xor_rule = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_matcher
			| tau_parser::bf
			| tau_parser::bf_or;
		CHECK( xor_rule.has_value() );
	}
}

TEST_SUITE("parsing bindings ") {

	TEST_CASE("binding") {
		const char* sample = "{ binding } := { binding }.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto named = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding;
		CHECK( named.has_value() );
	}

	TEST_CASE("typed binding") {
		const char* sample = "{ binding } : type := { binding } : type.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto source = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding;
		CHECK( source.has_value() );
	}

	TEST_CASE("binding type") {
		const char* sample = "{ binding } : type := { binding } : type.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto type = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::type;
		CHECK( type.has_value() );
	}

	TEST_CASE("binding source") {
		const char* sample = "{ binding } : type := { binding } : type.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto source = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::constant
			| tau_parser::binding
			| tau_parser::source;
		CHECK( source.has_value() );
	}

	TEST_CASE("unresolved binding") {
		const char* sample = "{ binding } := { binding }.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto type = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::type
			| tau_parser::chars;
		CHECK( !type.has_value() );
	}

	TEST_CASE("resolved binding") {
		const char* sample = "{ binding } : type := { binding } : type.";
		auto src = make_tau_source(sample, {
						.start = tau_parser::library });
		auto lib = make_statement(src);
		auto type = lib
			| tau_parser::rules
			| tau_parser::rule
			| tau_parser::bf_rule
			| tau_parser::bf_body
			| tau_parser::bf
			| tau_parser::bf_constant
			| tau_parser::type;
		CHECK( type.has_value() );
	}

	TEST_CASE("multiple constants") {
		const char* sample = "{ type : binding } & { type : binding2 }";
		auto src = make_tau_source(sample, {
						.start = tau_parser::bf_and });
		auto constants = make_statement(src) || tau_parser::bf
						|| tau_parser::bf_constant;
		CHECK( constants.size() == 2 );
	}
}


TEST_SUITE("regression/disallow T and F as variables") {

	TEST_CASE("T is not a variable") {
		const char* sample =
			"T = 0.";
		auto src = make_tau_source(sample);
		CHECK( src == nullptr );
	}

	TEST_CASE("F is not a variable") {
		const char* sample =
			"F = 0.";
		auto src = make_tau_source(sample);
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
