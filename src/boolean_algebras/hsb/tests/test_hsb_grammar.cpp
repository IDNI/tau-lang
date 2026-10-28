// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "boolean_algebras/hsb/parser/hsb_parser.generated.h"
#include "test_grammar_helpers.h"

using namespace idni::tau_lang;
using namespace hsb_parser_data;

TEST_SUITE("hsb grammar parsing") {

	// ═══════════════════════════════════════════════════════════════════════════
	// Top-level Constants
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE_TEMPLATE_INVOKE(grammar_constants, hsb_parser);

	// ═══════════════════════════════════════════════════════════════════════════
	// Single Halfspaces — Strict (<)
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse 'x[0] < 0'") { CHECK(grammar_parses<hsb_parser>("x[0] < 0")); }

	TEST_CASE("hsb: parse 'x[1] < 0' (second variable)") { CHECK(grammar_parses<hsb_parser>("x[1] < 0")); }

	TEST_CASE("hsb: parse '2*x[0] < 0' (integer coefficient, coeff*var)") { CHECK(grammar_parses<hsb_parser>("2*x[0] < 0")); }

	TEST_CASE("hsb: parse 'x[0]*3 < 0' (var*coeff form)") { CHECK(grammar_parses<hsb_parser>("x[0]*3 < 0")); }

	TEST_CASE("hsb: parse '1.5*x[0] < 0' (decimal coefficient)") { CHECK(grammar_parses<hsb_parser>("1.5*x[0] < 0")); }

	TEST_CASE("hsb: parse '1/2*x[0] < 0' (fraction coefficient)") { CHECK(grammar_parses<hsb_parser>("1/2*x[0] < 0")); }

	TEST_CASE("hsb: parse '-x[0] < 0' (unary minus on variable)") { CHECK(grammar_parses<hsb_parser>("-x[0] < 0")); }

	TEST_CASE("hsb: parse '-2*x[0] < 0' (unary minus with coefficient)") { CHECK(grammar_parses<hsb_parser>("-2*x[0] < 0")); }

	// ═══════════════════════════════════════════════════════════════════════════
	// Single Halfspaces — Non-strict (<=)
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse 'x[0] <= 0'") { CHECK(grammar_parses<hsb_parser>("x[0] <= 0")); }

	TEST_CASE("hsb: parse '-x[0] <= 0'") { CHECK(grammar_parses<hsb_parser>("-x[0] <= 0")); }

	TEST_CASE("hsb: parse '2*x[0] <= 0'") { CHECK(grammar_parses<hsb_parser>("2*x[0] <= 0")); }

	// ═══════════════════════════════════════════════════════════════════════════
	// Multi-variable Halfspaces
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse 'x[0] + x[1] < 0' (two variables, addition)") { CHECK(grammar_parses<hsb_parser>("x[0] + x[1] < 0")); }

	TEST_CASE("hsb: parse 'x[0] - x[1] < 0' (subtraction)") { CHECK(grammar_parses<hsb_parser>("x[0] - x[1] < 0")); }

	TEST_CASE("hsb: parse '2*x[0] + 3*x[1] < 0' (multiple coefficients)") { CHECK(grammar_parses<hsb_parser>("2*x[0] + 3*x[1] < 0")); }

	TEST_CASE("hsb: parse 'x[0] + x[1] + x[2] < 0' (3D halfspace)") { CHECK(grammar_parses<hsb_parser>("x[0] + x[1] + x[2] < 0")); }

	TEST_CASE("hsb: parse '-x[0] + x[1] < 0' (mixed signs)") { CHECK(grammar_parses<hsb_parser>("-x[0] + x[1] < 0")); }

	// ═══════════════════════════════════════════════════════════════════════════
	// Halfspaces with Bias (constant term)
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse 'x[0] + 1 < 0' (positive integer bias)") { CHECK(grammar_parses<hsb_parser>("x[0] + 1 < 0")); }

	TEST_CASE("hsb: parse 'x[0] - 1 < 0' (negative bias term)") { CHECK(grammar_parses<hsb_parser>("x[0] - 1 < 0")); }

	TEST_CASE("hsb: parse 'x[0] + 1/2 < 0' (fractional bias)") { CHECK(grammar_parses<hsb_parser>("x[0] + 1/2 < 0")); }

	TEST_CASE("hsb: parse 'x[0] + 2.5 < 0' (decimal bias)") { CHECK(grammar_parses<hsb_parser>("x[0] + 2.5 < 0")); }

	TEST_CASE("hsb: parse '-x[0] + 2.5 < 0' (negative lead + decimal bias)") { CHECK(grammar_parses<hsb_parser>("-x[0] + 2.5 < 0")); }

	TEST_CASE("hsb: parse '2*x[0] + x[1] - 3 <= 0' (multi-var + bias)") { CHECK(grammar_parses<hsb_parser>("2*x[0] + x[1] - 3 <= 0")); }

	// ═══════════════════════════════════════════════════════════════════════════
	// Negation
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse '~top'") { CHECK(grammar_parses<hsb_parser>("~top")); }

	TEST_CASE("hsb: parse '~bot'") { CHECK(grammar_parses<hsb_parser>("~bot")); }

	TEST_CASE("hsb: parse '~(x[0] < 0)' (negated halfspace)") { CHECK(grammar_parses<hsb_parser>("~(x[0] < 0)")); }

	TEST_CASE("hsb: parse '~~(x[0] < 0)' (double negation)") { CHECK(grammar_parses<hsb_parser>("~~(x[0] < 0)")); }

	TEST_CASE("hsb: parse '~(x[0] <= 0)'") { CHECK(grammar_parses<hsb_parser>("~(x[0] <= 0)")); }

	// ═══════════════════════════════════════════════════════════════════════════
	// Conjunction (parens required)
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse '(x[0] < 0 & x[1] < 0)' (basic conjunction)") { CHECK(grammar_parses<hsb_parser>("(x[0] < 0 & x[1] < 0)")); }

	TEST_CASE("hsb: parse '(top & bot)'") { CHECK(grammar_parses<hsb_parser>("(top & bot)")); }

	TEST_CASE("hsb: parse nested conjunction '(x[0] < 0 & (x[1] < 0 & x[2] < 0))'") { CHECK(grammar_parses<hsb_parser>("(x[0] < 0 & (x[1] < 0 & x[2] < 0))")); }

	TEST_CASE("hsb: parse '(x[0] + x[1] < 0 & x[2] <= 0)'") { CHECK(grammar_parses<hsb_parser>("(x[0] + x[1] < 0 & x[2] <= 0)")); }

	// ═══════════════════════════════════════════════════════════════════════════
	// Disjunction (parens required)
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse '(x[0] < 0 | x[1] < 0)' (basic disjunction)") { CHECK(grammar_parses<hsb_parser>("(x[0] < 0 | x[1] < 0)")); }

	TEST_CASE("hsb: parse '(top | bot)'") { CHECK(grammar_parses<hsb_parser>("(top | bot)")); }

	TEST_CASE("hsb: parse nested disjunction '((x[0] < 0 | x[1] < 0) | x[2] < 0)'") { CHECK(grammar_parses<hsb_parser>("((x[0] < 0 | x[1] < 0) | x[2] < 0)")); }

	// ═══════════════════════════════════════════════════════════════════════════
	// Mixed Boolean Combinations
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse '~(x[0] < 0 & x[1] < 0)' (not-and via paren)") { CHECK(grammar_parses<hsb_parser>("~(x[0] < 0 & x[1] < 0)")); }

	TEST_CASE("hsb: parse '(~(x[0] < 0) | ~(x[1] < 0))'") { CHECK(grammar_parses<hsb_parser>("(~(x[0] < 0) | ~(x[1] < 0))")); }

	TEST_CASE("hsb: parse '((x[0] < 0 & x[1] < 0) | x[2] < 0)'") { CHECK(grammar_parses<hsb_parser>("((x[0] < 0 & x[1] < 0) | x[2] < 0)")); }

	// ═══════════════════════════════════════════════════════════════════════════
	// Whitespace Handling
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse with leading/trailing whitespace") { CHECK(grammar_parses<hsb_parser>("  top  ")); }

	TEST_CASE("hsb: parse with extra spaces around operator") { CHECK(grammar_parses<hsb_parser>("x[0]  <  0")); }

	TEST_CASE("hsb: parse with spaces inside conjunction") { CHECK(grammar_parses<hsb_parser>("( x[0] < 0 & x[1] < 0 )")); }

	TEST_CASE("hsb: parse with spaces around +") { CHECK(grammar_parses<hsb_parser>("x[0]  +  x[1]  <  0")); }

	// ═══════════════════════════════════════════════════════════════════════════
	// Error Cases
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: reject 'x[0]' (no comparison)") { CHECK_FALSE(grammar_parses<hsb_parser>("x[0]")); }

	TEST_CASE("hsb: reject 'x[0] > 0' (unsupported operator)") { CHECK_FALSE(grammar_parses<hsb_parser>("x[0] > 0")); }

	// Both sides of a half-space are linear expressions since 2026-09-17;
	// the tree walk normalises `lhs op rhs` to `(lhs - rhs) op 0`.
	TEST_CASE("hsb: accept 'x[0] < 1' and 'x[0] <= x[1]' (linear rhs)") {
		for (const char* src : {"x[0] < 1", "x[0] <= x[1]",
		                        "2*x[0] <= x[1] - 1", "x[0] + 1/2 < x[1]"})
			CHECK(grammar_parses<hsb_parser>(src));
	}

	TEST_CASE("hsb: reject a chained comparison 'x[0] < 1 < 2'") { CHECK_FALSE(grammar_parses<hsb_parser>("x[0] < 1 < 2")); }

	TEST_CASE("hsb: reject 'x[0] < 0 & x[1] < 0' (& without parens)") { CHECK_FALSE(grammar_parses<hsb_parser>("x[0] < 0 & x[1] < 0")); }

	TEST_CASE("hsb: reject 'x[0] < 0 | x[1] < 0' (| without parens)") { CHECK_FALSE(grammar_parses<hsb_parser>("x[0] < 0 | x[1] < 0")); }

	TEST_CASE("hsb: reject empty string") { CHECK_FALSE(grammar_parses<hsb_parser>("")); }

	// ═══════════════════════════════════════════════════════════════════════════
	// Malformed numeric literals
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: reject '1//2*x[0] < 0' (double slash)") { CHECK_FALSE(grammar_parses<hsb_parser>("1//2*x[0] < 0")); }

	TEST_CASE("hsb: reject '1.2.3*x[0] < 0' (double dot)") { CHECK_FALSE(grammar_parses<hsb_parser>("1.2.3*x[0] < 0")); }

	TEST_CASE("hsb: reject '.5*x[0] < 0' (leading dot, no integer part)") { CHECK_FALSE(grammar_parses<hsb_parser>(".5*x[0] < 0")); }

	TEST_CASE("hsb: reject '1.*x[0] < 0' (trailing dot, no fractional part)") { CHECK_FALSE(grammar_parses<hsb_parser>("1.*x[0] < 0")); }

	TEST_CASE("hsb: reject '1/2/3*x[0] < 0' (chained slashes)") { CHECK_FALSE(grammar_parses<hsb_parser>("1/2/3*x[0] < 0")); }

	TEST_CASE("hsb: reject '1.2/3*x[0] < 0' (decimal then slash)") { CHECK_FALSE(grammar_parses<hsb_parser>("1.2/3*x[0] < 0")); }

	TEST_CASE("hsb: reject 'x[0] + 1//2 < 0' (double slash in bias)") { CHECK_FALSE(grammar_parses<hsb_parser>("x[0] + 1//2 < 0")); }

	TEST_CASE("hsb: reject 'x[0] + .5 < 0' (leading dot in bias)") { CHECK_FALSE(grammar_parses<hsb_parser>("x[0] + .5 < 0")); }

} // TEST_SUITE
