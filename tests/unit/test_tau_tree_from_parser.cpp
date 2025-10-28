// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

// The following test suites represent correct sentences of a tau nso_rr or
// library and as such must be parsed correctly and satisfy the structural checks
// we perform on the parsed nso_rr (as the rest of the code assumes such structure).

// TODO (LOW) why "$X := { : src_code } bf_and { bool : src_code }." is parsed
//
// improperly? bf | ba_constant | ...
//
// Check if still hapopens after the latest changes to the grammar.

tt get_main(const char* sample) {
	return tt(tau::get(sample, { .reget_with_hooks = false })) | tau::spec | tau::main;
}

tt get_bf(const char* sample) {
	return tt(tau::get(sample, {
		.parse = { .start = tau::bf },
		.reget_with_hooks = false }));

}

tt get_rec_relation(const char* sample) {
	tref n = tau::get(sample, tau::get_options{ .reget_with_hooks = false});
	return tt(n) | tau::spec | tau::definitions | tau::rec_relation;
}


TEST_SUITE("parsing only main") {

	TEST_CASE("nso_rr main") {
		const char* sample = " Z = 0 .";
		CHECK( get_main(sample).has_value() );
	}
}

TEST_SUITE("parsing basic wff nso_rr") {

	TEST_CASE("nso_rr wff_rec_relation y1") {
		const char* sample =
			"g[0](Y) := T."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y2") {
		const char* sample =
			"g[0](Y) := T."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y3") {
		const char* sample =
			"g[N](Y) := T."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y4") {
		const char* sample =
			"g[N](Y) := T."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y5") {
		const char* sample =
			"g[N - 1](Y) := T."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y6") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[0](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y7") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[0](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y8") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y9") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y10") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y11") {
		const char* sample =
			"h[0](Y) := T."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}
}

TEST_SUITE("parsing basic bf nso_rr") {

	TEST_CASE("nso_rr bf_rec_relation y1") {
		const char* sample =
			"g[0](Y) := 1."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y2") {
		const char* sample =
			"g[0](Y) := 1."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y3") {
		const char* sample =
			"g[N](Y) := 1."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y4") {
		const char* sample =
			"g[N](Y) := 1."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y5") {
		const char* sample =
			"g[N - 1](Y) := 1."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y6") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[0](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y7") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[0](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y8") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y9") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y10") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y11") {
		const char* sample =
			"h[0](Y) := 1."
			"g[0](Y) := h[N - 1](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}
}

TEST_SUITE("parsing basic wff nso_rr in main") {

	TEST_CASE("nso_rr wff_rec_relation in main y1") {
		const char* sample =
			"g[0](Y) := T."
			"g[0](Y).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y2") {
		const char* sample =
			"g[0](Y) := T."
			"g[0](Y).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y3") {
		const char* sample =
			"g[0](Y) := T."
			"g[N](Y).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y4") {
		const char* sample =
			"g[0](Y) := T."
			"g[N](Y).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y5") {
		const char* sample =
			"g[0](Y) := T."
			"g[N - 1](Y).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation in main y6") {
		const char* sample =
			"g[0](Y) := T."
			"g[N - 1](Y).";
		CHECK( get_main(sample).has_value() );
	}
}

TEST_SUITE("parsing basic bf nso_rr in main") {

	TEST_CASE("nso_rr bf_rec_relation in main y1") {
		const char* sample =
			"(g[0, 0](Y) = 0).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y2") {
		const char* sample =
			"(g[0, 0](Y) = 0).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y3") {
		const char* sample =
			"(g[N, 0](Y) = 0).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y4") {
		const char* sample =
			"(g[N, 0](Y) = 0).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y5") {
		const char* sample =
			"(g[N - 1](Y) = 0).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation in main y6") {
		const char* sample =
			"(g[N - 1](Y) = 0).";
		CHECK( get_main(sample).has_value() );
	}

}

TEST_SUITE("parsing basic wff nso_rr (2d): 1th var check") {

	TEST_CASE("nso_rr wff_rec_relation y1") {
		const char* sample =
			"g[0, 0](Y) := T."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y2") {
		const char* sample =
			"g[0, 0](Y) := T."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y3") {
		const char* sample =
			"g[N, 0](Y) := T."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y4") {
		const char* sample =
			"g[N, 0](Y) := T."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y5") {
		const char* sample =
			"g[N - 1, 0](Y) := T."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y6") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y7") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y8") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y9") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y10") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr wff_rec_relation y11") {
		const char* sample =
			"h[0](Y)    := T."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}
}

TEST_SUITE("parsing basic bf nso_rr (2d): 1th var check") {

	TEST_CASE("nso_rr bf_rec_relation y1") {
		const char* sample =
			"g[0, 0](Y) := 1."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y2") {
		const char* sample =
			"g[0, 0](Y) := 1."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y3") {
		const char* sample =
			"g[N, 0](Y) := 1."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y4") {
		const char* sample =
			"g[N, 0](Y) := 1."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y5") {
		const char* sample =
			"g[N - 1, 0](Y) := 1."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y6") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y7") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[0](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y8") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y9") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y10") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}

	TEST_CASE("nso_rr bf_rec_relation y11") {
		const char* sample =
			"h[0](Y)    := 1."
			"g[0, 0](Y) := h[N - 1](Y)."
			"T.";
		CHECK( get_rec_relation(sample).has_value() );
	}
}

TEST_SUITE("parsing variables") {
	// done inderectly
}

TEST_SUITE("parsing captures") {
	// done inderectly
}

TEST_SUITE("parsing inputs") {

	TEST_CASE("fix lookback") {
		const char* sample = "( i1[0] = 0 ).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("capture lookback") {
		const char* sample = "( i1[n] = 0 ).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("variable lookback") {
		const char* sample = "( i1[t] = 0 ).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("shift capture lookback") {
		const char* sample = "( i1[n - 1] = 0 ).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("shift variable lookback") {
		const char* sample = "( i1[t - 1] = 0 ).";
		CHECK( get_main(sample).has_value() );
	}
}

TEST_SUITE("parsing outputs") {

	TEST_CASE("fix lookback") {
		const char* sample = "( o1[0] = 0 ).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("capture lookback") {
		const char* sample = "( o1[n] = 0 ).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("variable lookback") {
		const char* sample = "( o1[t] = 0 ).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("shift capture lookback") {
		const char* sample = "( o1[n - 1] = 0 ).";
		CHECK( get_main(sample).has_value() );
	}

	TEST_CASE("shift variable lookback") {
		const char* sample = "( o1[t - 1] = 0 ).";
		CHECK( get_main(sample).has_value() );
	}
}

TEST_SUITE("parsing offsets"){
	// done inderectly
}

TEST_SUITE("parsing wwf formulas ") {

	TEST_CASE("!") {
		const char* sample = "! ( Z = 0 ).";
		auto neg_formula = get_main(sample) | tau::wff | tau::wff_neg;
		CHECK( neg_formula.has_value() );
	}

	TEST_CASE("&&") {
		const char* sample = "( Z = 0 ) && ( X = 0 ).";
		auto and_formula = get_main(sample) | tau::wff | tau::wff_and;
		CHECK( and_formula.has_value() );
	}

	TEST_CASE("||") {
		const char* sample = "( Z = 0 ) || ( X = 0 ).";
		auto or_formula = get_main(sample) | tau::wff | tau::wff_or;
		CHECK( or_formula.has_value() );
	}

	TEST_CASE("^^") {
		const char* sample = "( Z = 0 ) ^^ ( X = 0 ).";
		auto xor_formula = get_main(sample) | tau::wff | tau::wff_xor;
		CHECK( xor_formula.has_value() );
	}

	TEST_CASE("?") {
		const char* sample = "$X ? $Y : $Z.";
		auto cond_formula = get_main(sample) | tau::wff | tau::wff_conditional;
		CHECK( cond_formula.has_value() );
	}

	TEST_CASE("= 0") {
		const char* sample = " Z = 0 .";
		auto eq_formula = get_main(sample) | tau::wff | tau::bf_eq;
		CHECK( eq_formula.has_value() );
	}

	TEST_CASE("!= 0") {
		const char* sample = " Z != 0 .";
		auto neq_formula = get_main(sample) | tau::wff | tau::bf_neq;
		CHECK( neq_formula.has_value() );
	}

	TEST_CASE("<") {
		const char* sample = " X < Y .";
		auto lt_formula = get_main(sample) | tau::wff | tau::bf_lt;
		CHECK( lt_formula.has_value() );
	}

	TEST_CASE("<=") {
		const char* sample = " X <= Y .";
		auto lteq_formula = get_main(sample) | tau::wff | tau::bf_lteq;
		CHECK( lteq_formula.has_value() );
	}

	TEST_CASE(">") {
		const char* sample = "X > Y.";
		auto gt_formula = get_main(sample) | tau::wff | tau::bf_gt;
		CHECK( gt_formula.has_value() );
	}

	TEST_CASE("all") {
		const char* sample =
			"all Z ( Z = 0 ) .";
		auto all_formula = get_main(sample) | tau::wff 	| tau::wff_all;
		CHECK( all_formula.has_value() );
	}

	TEST_CASE("ex") {
		const char* sample =
			"ex Z ( Z = 0 ) .";
		auto ex_formula = get_main(sample) | tau::wff 	| tau::wff_ex;
		CHECK( ex_formula.has_value() );
	}
}

TEST_SUITE("parsing bf formulas ") {

	TEST_CASE("fall") {
		const char* sample =
			"fall x, y x & y";
		auto fall_formula = get_bf(sample) | tau::bf_fall;
		CHECK( fall_formula.has_value() );
	}

	TEST_CASE("fex") {
		const char* sample =
			"fex x, y x | y";
		auto fex_formula = get_bf(sample) | tau::bf_fex;
		CHECK( fex_formula.has_value() );
	}
}

TEST_SUITE("regression/disallow T and F as variables") {

	TEST_CASE("T is not a variable") {
		const char* sample =
			"T = 0.";
		auto src = tau::get(sample);
		CHECK( src == nullptr );
	}

	TEST_CASE("F is not a variable") {
		const char* sample =
			"F = 0.";
		auto src = tau::get(sample);
		CHECK( src == nullptr );
	}

}
