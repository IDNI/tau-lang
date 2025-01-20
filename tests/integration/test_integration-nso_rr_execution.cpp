// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "test_integration_helpers.h"
#include "../unit/test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

bool normalize_and_test_for_value(const char* sample,
	tau_parser::nonterminal nt, bool expect_fail = false)
{
	auto sample_src = make_tau_source(sample);
	auto sample_formula = make_nso_rr_using_factory<sbf_ba>(sample_src);
	if (!sample_formula.has_value()) return expect_fail;
	auto result = normalizer<sbf_ba>(sample_formula.value());
	if (!result) return expect_fail;
	auto check = result | nt;
	return expect_fail ? !check.has_value() : check.has_value();
}

bool normalize_and_expect_fail(const char* sample, tau_parser::nonterminal nt) {
	return normalize_and_test_for_value(sample, nt, true);
}


TEST_SUITE("function execution: simple cases") {

	TEST_CASE("wff_rec_relation: direct substitution") {
		const char* sample =
			"g(Y) := T."
			"g(Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("bf_rec_relation: direct substitution") {
		const char* sample =
			"g(Y) := 1."
			"g(Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}

	TEST_CASE("wff_rec_relation: two substitutions") {
		const char* sample =
			"h(Y) := T."
			"g(Y) := h(Y)."
			"g(Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("bf_rec_relation: two substitutions") {
		const char* sample =
			"h(Y) := 1."
			"g(Y) := h(Y)."
			"g(Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}
}

TEST_SUITE("rec relations execution: simple cases") {

	TEST_CASE("wff_rec_relation: direct substitution") {
		const char* sample =
			"g[0](Y) := T."
			"g[0](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("bf_rec_relation: direct substitution") {
		const char* sample =
			"g[0](Y) := 1."
			"g[0](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}

	TEST_CASE("wff_rec_relation: two substitutions") {
		const char* sample =
			"g[0](Y) := T."
			"g[n](Y) := g[n - 1](Y)."
			"g[1](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("bf_rec_relation: two substitutions") {
		const char* sample =
			"g[0](Y) := 1."
			"g[n](Y) := g[n - 1](Y)."
			"g[1](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}
}

TEST_SUITE("rec_relations execution: types") {

	TEST_CASE("clashing name nso_rr wff_rec_relation and bf_rec_relation: type mismatch") {
		const char* sample =
			"g[0](Y) := 0."
			"g[0](Y) := T."
			"g[0](Y).";
		CHECK( normalize_and_expect_fail(sample, tau_parser::wff_f) );
	}

	TEST_CASE("clashing name nso_rr wff_rec_relation and bf_rec_relation: type mismatch") {
		const char* sample =
			"g[0](Y) := 0."
			"g[0](Y) := T."
			"g[0](Y) = 0.";
		CHECK( normalize_and_expect_fail(sample, tau_parser::wff_f) );
	}
}

TEST_SUITE("2d cases") {

	TEST_CASE("wff_rec_relation 2d: direct substitution") {
		const char* sample =
			"g[0, 0](Y) := T."
			"g[0, 0](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("bf_rec_relation 2d: direct substitution") {
		const char* sample =
			"g[0, 0](Y) := 0."
			"g[0, 0](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("wff_rec_relation 2d: two substitutions 1st coord.") {
		const char* sample =
			"g[0, 0](Y) := T."
			"g[n, 0](Y) := g[n - 1, 0](Y)."
			"g[1, 0](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("bf_rec_relation 2d: two substitutions 1st coord.") {
		const char* sample =
			"g[0, 0](Y) := 1."
			"g[n, 0](Y) := g[n - 1, 0](Y)."
			"g[1, 0](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}

	TEST_CASE("wff_rec_relation 2d: two substitutions 2nd coord.") {
		const char* sample =
			"g[0, 0](Y)  := T."
			"g[0, $n](Y) := g[0, $n - 1](Y)."
			"g[0, 1](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("bf_rec_relation 2d: two substitutions 2nd coord.") {
		const char* sample =
			"g[0, 0](Y)  := 1."
			"g[0, $n](Y) := g[0, $n - 1](Y)."
			"g[0, 1](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}
}

TEST_SUITE("rec relations execution: several relations") {

	TEST_CASE("direct substitution, wff_rec_relation case") {
		const char* sample =
			"g[0, 0](Y) := T."
			"h[0, 0](Y) := T."
			"g[0, 0](Y) && h[0, 0](1).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("direct substitution, bf_rec_relation case") {
		const char* sample =
			"g[0, 0](Y) := 1."
			"h[0, 0](Y) := 1."
			"g[0, 0](Y) & h[0,0](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}

	TEST_CASE("complex substitution, wff_rec_relation case.") {
		const char* sample =
			"g[0, 0](Y)  := T."
			"g[n, 0](Y)  := g[n - 1, 0](Y)."
			"h[0, 0](Y)  := T."
			"h[0, $m](Y) := h[0, $m - 1](Y)."
			"g[1, 0](Y) && h[0,1](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("complex substitution, bf_rec_relation case.") {
		const char* sample =
			"g[0, 0](Y)  := 1."
			"g[n, 0](Y)  := g[n - 1, 0](Y)."
			"h[0, 0](Y)  := 0."
			"h[0, $m](Y) := h[0, $m - 1](Y)."
			"g[1, 0](Y) & h[0,1](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}
}

TEST_SUITE("rec relations execution: longer offsets") {

	TEST_CASE("wff_rec_relation: case 1") {
		const char* sample =
			"g[0](Y) := T."
			"g[n](Y) := g[n - 2](Y)."
			"g[4](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("wff_rec_relation: case 2") {
		const char* sample =
			"g[0](Y) := T."
			"g[1](Y) := F."
			"g[n](Y) := g[n - 2](Y)."
			"g[5](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}

	TEST_CASE("bf_rec_relation: case 1") {
		const char* sample =
			"g[0](Y) := 1."
			"g[1](Y) := 0."
			"g[n](Y) := g[n - 2](Y)."
			"g[4](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}

	TEST_CASE("bf_rec_relation: case 2") {
		const char* sample =
			"g[0](Y) := 1."
			"g[1](Y) := 0."
			"g[n](Y) := g[n - 1](Y)."
			"g[5](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}
}

TEST_SUITE("mutual rec cases") {

	TEST_CASE("wff_rec_relation: case 1") {
		const char* sample =
			"g[0](Y) := T."
			"g[n](Y) := h[n - 1](Y)."
			"h[0](Y) := F."
			"h[n](Y) := g[n - 1](Y)."
			"g[4](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("wff_rec_relation: case 2") {
		const char* sample =
			"g[0](Y) := T."
			"g[n](Y) := h[n - 1](Y)."
			"h[0](Y) := F."
			"h[n](Y) := g[n - 1](Y)."
			"g[5](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}

	TEST_CASE("bf_rec_relation: case 1") {
		const char* sample =
			"g[0](Y) := 1."
			"g[n](Y) := h[n - 1](Y)."
			"h[0](Y) := 0."
			"h[n](Y) := g[n - 1](Y)."
			"g[4](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}

	TEST_CASE("bf_rec_relation: case 2") {
		const char* sample =
			"g[0](Y) := 1."
			"g[n](Y) := h[n - 1](Y)."
			"h[0](Y) := 0."
			"h[n](Y) := g[n - 1](Y)."
			"g[5](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}
}

TEST_SUITE("multiple rec relations") {

	TEST_CASE("wff_rec_relation: case 1") {
		const char* sample =
			"g[0](Y) := T."
			"h[0](Y) := F."
			"g[0](Y) && h[0](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}

	TEST_CASE("wff_rec_relation: case 2") {
		const char* sample =
			"g[0](Y) := T."
			"g[n](Y) := g[n - 1](Y)."
			"h[0](Y) := F."
			"h[n](Y) := h[n - 1](Y)."
			"g[1](Y) && h[1](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}

	TEST_CASE("wff_rec_relation: case 3") {
		const char* sample =
			"g[0](Y) := T."
			"g[n](Y) := g[n - 1](Y)."
			"h[0](Y) := T."
			"h[n](Y) := h[n - 1](Y)."
			"g[1](Y) && h[1](Y).";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}


	TEST_CASE("bf_rec_relation: case 1") {
		const char* sample =
			"g[0](Y) := 0."
			"h[0](Y) := 1."
			"g[0](Y) | h[0](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}

	TEST_CASE("bf_rec_relation: case 2") {
		const char* sample =
			"g[0](Y) := 1."
			"g[n](Y) := g[n - 1](Y)."
			"h[0](Y) := 0."
			"h[n](Y) := h[n - 1](Y)."
			"g[1](Y) & h[1](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_t) );
	}

	TEST_CASE("bf_rec_relation: case 3") {
		const char* sample =
			"g[0](Y) := 1."
			"g[n](Y) := g[n - 1](Y)."
			"h[0](Y) := 1."
			"h[n](Y) := h[n - 1](Y)."
			"g[1](Y) & h[1](Y) = 0.";
		CHECK( normalize_and_test_for_value(sample, tau_parser::wff_f) );
	}
}
