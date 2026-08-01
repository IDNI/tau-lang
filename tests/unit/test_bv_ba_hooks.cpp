// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for the bitvector BA construction-time hooks in
// src/boolean_algebras/bv_ba_hooks.tmpl.h.
//
// Coverage measurement (2026-08-01) showed 214 of 519 lines in that file cold,
// concentrated in two areas that no existing test reached:
//
//  1. The eight comparison hooks (wff_bv_lt/nlt/lteq/nlteq/gt/ngt/gteq/ngteq)
//     and the `compare_bv_consts` helper they all share -- the helper was 5/5
//     lines uncovered, i.e. entirely untested. These hooks only fire when BOTH
//     operands are bitvector constants; every pre-existing test put a variable
//     on at least one side (`X:bv[16] >= X`, `x:bv[8] < y:bv[8]`), so the
//     constant-folding path never ran.
//
//  2. The arms of term_add/sub/mul/div/mod/shr/shl that take the BA top element
//     `1:bv[N]` (all ones) or bottom element `0:bv[N]` as an *operand*.
//     tests/integration/test_integration-bv_hooks.cpp exercises these operators
//     thoroughly with concrete `{N}:bv[8]` constants, and uses `1:bv[8]` /
//     `0:bv[8]` only as *expected* values -- never as inputs.
//
// Expected values below are computed independently from bitvector semantics
// (width-8 unsigned, wrapping mod 256, top element = 255) rather than read off
// the implementation, so a mismatch indicates a real defect rather than a
// stale expectation.

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"

using tau_api = api<node_t>;

TEST_SUITE("Configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

namespace {

// Parses a Boolean function term. Hooks run at construction, so the returned
// tref is already the folded form.
tref bf(const std::string& sample) {
	static tau::get_options opts{ .parse = { .start = tau::bf } };
	tref src = tau::get(sample, opts);
	if (src == nullptr) TAU_LOG_ERROR << "bf parsing failed for: " << sample;
	REQUIRE(src != nullptr);
	return src;
}

// Parses a well-formed formula; used for the comparison hooks, which are
// dispatched from the wff-level hooks in src/hooks.tmpl.h.
tref wff(const std::string& sample) {
	tref src = tau_api::get_formula(sample);
	if (src == nullptr) TAU_LOG_ERROR << "wff parsing failed for: " << sample;
	REQUIRE(src != nullptr);
	return src;
}

} // namespace

// ---------------------------------------------------------------------------
// compare_bv_consts + the eight comparison hooks
// ---------------------------------------------------------------------------

TEST_SUITE("bv comparison hooks: constant folding") {

	// compare_bv_consts compares the base-2 strings from
	// cvc5's getBitVectorValue(2). That is only equivalent to unsigned
	// numeric comparison if the strings are zero-padded to the full width:
	// unpadded, "1010" (10) would compare LESS than "11" (3). These two
	// cases pin that down.
	TEST_CASE("comparison is by value, not by unpadded bit-string length") {
		CHECK(wff("{3}:bv[8] < {10}:bv[8]") == wff("T"));
		CHECK(wff("{10}:bv[8] < {3}:bv[8]") == wff("F"));
	}

	// 200 as a signed 8-bit value is -56, which would be < 100.
	// Bitvector comparison must be unsigned.
	TEST_CASE("comparison is unsigned across the sign bit") {
		CHECK(wff("{200}:bv[8] < {100}:bv[8]") == wff("F"));
		CHECK(wff("{100}:bv[8] < {200}:bv[8]") == wff("T"));
		CHECK(wff("{254}:bv[8] > {253}:bv[8]") == wff("T"));
	}

	// An all-ones literal is canonicalized to the BA top element, so it is a
	// bf_t node rather than a ba_constant.
	TEST_CASE("an all-ones literal is the top element") {
		CHECK(bf("{255}:bv[8]") == bf("1:bv[8]"));
		CHECK(bf("{15}:bv[4]") == bf("1:bv[4]"));
	}

	// Consequence of the above: the comparison hooks require BOTH operands to
	// satisfy is_ba_constant(), so a top- or bottom-element operand is left
	// unfolded even though the comparison is decidable. The arithmetic hooks
	// (term_add etc.) do handle bf_t/bf_f operands explicitly -- this
	// asymmetry is characterized here rather than asserted to be correct.
	TEST_CASE("top/bottom element operands are not folded by comparisons") {
		CHECK(wff("1:bv[8] > {254}:bv[8]") != wff("T"));
		CHECK(wff("1:bv[8] > {254}:bv[8]") != wff("F"));
		CHECK(wff("{254}:bv[8] < 1:bv[8]") != wff("T"));
		CHECK(wff("{254}:bv[8] < 1:bv[8]") != wff("F"));
	}

	TEST_CASE("less than") {
		CHECK(wff("{10}:bv[8] < {20}:bv[8]") == wff("T"));
		CHECK(wff("{20}:bv[8] < {10}:bv[8]") == wff("F"));
	}

	TEST_CASE("not less than") {
		CHECK(wff("{10}:bv[8] !< {20}:bv[8]") == wff("F"));
		CHECK(wff("{20}:bv[8] !< {10}:bv[8]") == wff("T"));
	}

	TEST_CASE("less or equal") {
		CHECK(wff("{10}:bv[8] <= {20}:bv[8]") == wff("T"));
		CHECK(wff("{20}:bv[8] <= {10}:bv[8]") == wff("F"));
	}

	TEST_CASE("not less or equal") {
		CHECK(wff("{10}:bv[8] !<= {20}:bv[8]") == wff("F"));
		CHECK(wff("{20}:bv[8] !<= {10}:bv[8]") == wff("T"));
	}

	TEST_CASE("greater than") {
		CHECK(wff("{20}:bv[8] > {10}:bv[8]") == wff("T"));
		CHECK(wff("{10}:bv[8] > {20}:bv[8]") == wff("F"));
	}

	TEST_CASE("not greater than") {
		CHECK(wff("{20}:bv[8] !> {10}:bv[8]") == wff("F"));
		CHECK(wff("{10}:bv[8] !> {20}:bv[8]") == wff("T"));
	}

	TEST_CASE("greater or equal") {
		CHECK(wff("{20}:bv[8] >= {10}:bv[8]") == wff("T"));
		CHECK(wff("{10}:bv[8] >= {20}:bv[8]") == wff("F"));
	}

	TEST_CASE("not greater or equal") {
		CHECK(wff("{20}:bv[8] !>= {10}:bv[8]") == wff("F"));
		CHECK(wff("{10}:bv[8] !>= {20}:bv[8]") == wff("T"));
	}

	// The comparison hooks require both operands to share one bv type; a
	// variable operand must leave the comparison unfolded.
	TEST_CASE("a variable operand is not folded") {
		CHECK(wff("x:bv[8] < {20}:bv[8]") != wff("T"));
		CHECK(wff("x:bv[8] < {20}:bv[8]") != wff("F"));
	}
}

// ---------------------------------------------------------------------------
// Arithmetic with the BA top element `1:bv[8]` (= 255) and bottom `0:bv[8]`
// ---------------------------------------------------------------------------

TEST_SUITE("bv term_add: top element operands") {

	TEST_CASE("1 + X") {                       // 255 + 10 = 265 -> 9
		CHECK(bf("1:bv[8] + {10}:bv[8]") == bf("{9}:bv[8]"));
	}

	TEST_CASE("1 + 1") {                       // 255 + 255 = 510 -> 254
		CHECK(bf("1:bv[8] + 1:bv[8]") == bf("{254}:bv[8]"));
	}
}

TEST_SUITE("bv term_sub: top and bottom element operands") {

	TEST_CASE("1 - X") {                       // 255 - 10 = 245
		CHECK(bf("1:bv[8] - {10}:bv[8]") == bf("{245}:bv[8]"));
	}

	TEST_CASE("1 - 1") {                       // folds to the bottom element
		CHECK(bf("1:bv[8] - 1:bv[8]") == bf("0:bv[8]"));
	}

	TEST_CASE("0 - 1") {                       // 0 - 255 = -255 -> 1
		CHECK(bf("0:bv[8] - 1:bv[8]") == bf("{1}:bv[8]"));
	}

	TEST_CASE("X - 1") {                       // 10 - 255 = -245 -> 11
		CHECK(bf("{10}:bv[8] - 1:bv[8]") == bf("{11}:bv[8]"));
	}

	// The bf_neg arm: a complemented constant as the left operand.
	// ~5 = 250 at width 8.
	TEST_CASE("{c}' - 1") {                    // 250 - 255 = -5 -> 251
		CHECK(bf("{5}:bv[8]' - 1:bv[8]") == bf("{251}:bv[8]"));
	}

	TEST_CASE("{c}' - 0") {                    // 250 - 0 = 250
		CHECK(bf("{5}:bv[8]' - 0:bv[8]") == bf("{250}:bv[8]"));
	}
}

TEST_SUITE("bv term_mul: top element operands") {

	TEST_CASE("1 * X") {                       // 255 * 10 = 2550 -> 246
		CHECK(bf("1:bv[8] * {10}:bv[8]") == bf("{246}:bv[8]"));
	}

	TEST_CASE("1 * 1") {                       // 255 * 255 = 65025 -> 1
		CHECK(bf("1:bv[8] * 1:bv[8]") == bf("{1}:bv[8]"));
	}

	TEST_CASE("X * 1") {                       // 10 * 255 = 2550 -> 246
		CHECK(bf("{10}:bv[8] * 1:bv[8]") == bf("{246}:bv[8]"));
	}
}

TEST_SUITE("bv term_div: top and bottom element operands") {

	TEST_CASE("1 / X") {                       // 255 / 10 = 25
		CHECK(bf("1:bv[8] / {10}:bv[8]") == bf("{25}:bv[8]"));
	}

	TEST_CASE("1 / 1") {                       // 255 / 255 = 1
		CHECK(bf("1:bv[8] / 1:bv[8]") == bf("{1}:bv[8]"));
	}

	TEST_CASE("1 / 0 is the top element") {    // division by zero -> all ones
		CHECK(bf("1:bv[8] / 0:bv[8]") == bf("1:bv[8]"));
	}

	TEST_CASE("X / 1") {                       // 10 / 255 = 0
		CHECK(bf("{10}:bv[8] / 1:bv[8]") == bf("0:bv[8]"));
	}
}

TEST_SUITE("bv term_mod: top and bottom element operands") {

	TEST_CASE("1 % X") {                       // 255 % 10 = 5
		CHECK(bf("1:bv[8] % {10}:bv[8]") == bf("{5}:bv[8]"));
	}

	TEST_CASE("1 % 1 is zero") {
		CHECK(bf("1:bv[8] % 1:bv[8]") == bf("0:bv[8]"));
	}

	// Note: this arm builds the bitvector *value* 1, not the top element.
	TEST_CASE("1 % 0 is the value one") {
		CHECK(bf("1:bv[8] % 0:bv[8]") == bf("{1}:bv[8]"));
	}

	TEST_CASE("X % 1") {                       // 10 % 255 = 10
		CHECK(bf("{10}:bv[8] % 1:bv[8]") == bf("{10}:bv[8]"));
	}
}

TEST_SUITE("bv term_shr: top and bottom element operands") {

	TEST_CASE("1 >> X") {                      // 255 >> 2 = 63
		CHECK(bf("1:bv[8] >> {2}:bv[8]") == bf("{63}:bv[8]"));
	}

	// Shifting by 255 is a shift of at least the width, which yields zero.
	TEST_CASE("1 >> 1 is zero") {
		CHECK(bf("1:bv[8] >> 1:bv[8]") == bf("0:bv[8]"));
	}

	TEST_CASE("0 >> X is zero") {
		CHECK(bf("0:bv[8] >> {2}:bv[8]") == bf("0:bv[8]"));
	}

	TEST_CASE("X >> 1 is zero") {
		CHECK(bf("{128}:bv[8] >> 1:bv[8]") == bf("0:bv[8]"));
	}
}

TEST_SUITE("bv term_shl: top and bottom element operands") {

	TEST_CASE("1 << X") {                      // 255 << 2 = 1020 -> 252
		CHECK(bf("1:bv[8] << {2}:bv[8]") == bf("{252}:bv[8]"));
	}

	TEST_CASE("1 << 1 is zero") {
		CHECK(bf("1:bv[8] << 1:bv[8]") == bf("0:bv[8]"));
	}

	TEST_CASE("0 << X is zero") {
		CHECK(bf("0:bv[8] << {2}:bv[8]") == bf("0:bv[8]"));
	}

	TEST_CASE("X << 1 is zero") {
		CHECK(bf("{1}:bv[8] << 1:bv[8]") == bf("0:bv[8]"));
	}
}

// ---------------------------------------------------------------------------
// A non-8-bit width, to confirm the width is read from the type rather than
// assumed. At width 4 the top element is 15.
// ---------------------------------------------------------------------------

TEST_SUITE("bv hooks: width is taken from the bv type") {

	TEST_CASE("top element at width 4 is 15") {
		CHECK(bf("1:bv[4] + {1}:bv[4]") == bf("0:bv[4]"));   // 15+1 = 16 -> 0
		CHECK(bf("1:bv[4] - {1}:bv[4]") == bf("{14}:bv[4]"));
		CHECK(bf("1:bv[4] / {5}:bv[4]") == bf("{3}:bv[4]")); // 15/5 = 3
	}

	TEST_CASE("comparison at width 16") {
		CHECK(wff("{300}:bv[16] > {299}:bv[16]") == wff("T"));
		CHECK(wff("{300}:bv[16] < {299}:bv[16]") == wff("F"));
	}
}

// ---------------------------------------------------------------------------
// Fall-through arms: a top-element operand paired with a NON-constant operand.
// Each hook's `case tau::bf_t:` arm requires the other operand to be a
// constant (or bf_t); when it is a variable the arm breaks out and the symbol
// is returned unfolded. These breaks were the last uncovered lines in the
// arithmetic hooks.
// ---------------------------------------------------------------------------

namespace {

// True if the folded term still mentions the variable, i.e. was left unfolded.
bool unfolded(const std::string& sample) {
	return tau::get(bf(sample)).to_str().find('x') != std::string::npos;
}

} // namespace

TEST_SUITE("bv hooks: top element with a variable operand is not folded") {

	TEST_CASE("addition") {
		CHECK(unfolded("1:bv[8] + x:bv[8]"));
	}

	TEST_CASE("subtraction") {
		CHECK(unfolded("1:bv[8] - x:bv[8]"));
		CHECK(unfolded("x:bv[8] - 1:bv[8]"));
	}

	TEST_CASE("division") {
		CHECK(unfolded("1:bv[8] / x:bv[8]"));
		CHECK(unfolded("x:bv[8] / 1:bv[8]"));
	}

	TEST_CASE("modulo") {
		CHECK(unfolded("1:bv[8] % x:bv[8]"));
		CHECK(unfolded("x:bv[8] % 1:bv[8]"));
	}

	TEST_CASE("shift right") {
		CHECK(unfolded("1:bv[8] >> x:bv[8]"));
		CHECK(unfolded("x:bv[8] >> 1:bv[8]"));
	}

	TEST_CASE("shift left") {
		CHECK(unfolded("1:bv[8] << x:bv[8]"));
		CHECK(unfolded("x:bv[8] << 1:bv[8]"));
	}
}

// The bottom-element-shifted-by-top-element arms of shr/shl.
TEST_SUITE("bv shifts: bottom element shifted by top element") {

	TEST_CASE("0 >> 1 is zero") {
		CHECK(bf("0:bv[8] >> 1:bv[8]") == bf("0:bv[8]"));
	}

	TEST_CASE("0 << 1 is zero") {
		CHECK(bf("0:bv[8] << 1:bv[8]") == bf("0:bv[8]"));
	}
}

// ---------------------------------------------------------------------------
// bv_term_cast operand forms
// ---------------------------------------------------------------------------

TEST_SUITE("bv_term_cast: operand forms") {

	// Regular constant and bf_t operands are already covered by
	// tests/unit/test_bv_ba-cast.cpp; these are the remaining forms.

	TEST_CASE("casting the bottom element yields zero at the target width") {
		CHECK(bf("((bv[16]) 0:bv[8])") == bf("0:bv[16]"));
	}

	TEST_CASE("casting a complemented variable is left unfolded") {
		CHECK(unfolded("((bv[16]) x:bv[8]')"));
	}

	// ~{const} at width 8: ~5 = 250, zero-extended to 16 bits stays 250.
	TEST_CASE("casting a complemented constant") {
		CHECK(bf("((bv[16]) {5}:bv[8]')") == bf("{250}:bv[16]"));
	}

	// bv_term_cast has arms for a bf_neg operand whose inner value is bf_t
	// (~all-ones) or bf_f (~zero). Those arms are UNREACHABLE through source
	// syntax: a complemented top/bottom element is already folded to the
	// opposite element at construction time, so by the time the cast hook
	// runs the operand is a plain bf_f/bf_t rather than a bf_neg. These two
	// assertions record why those lines stay uncovered, so the gap is a
	// documented dead branch rather than a missing test.
	TEST_CASE("complemented top/bottom elements fold before any cast") {
		CHECK(bf("1:bv[8]'") == bf("0:bv[8]"));
		CHECK(bf("0:bv[8]'") == bf("1:bv[8]"));
		// Hence the cast sees bf_f / bf_t, taking the earlier arms:
		CHECK(bf("((bv[16]) 1:bv[8]')") == bf("0:bv[16]"));
		CHECK(bf("((bv[16]) 0:bv[8]')") == bf("{255}:bv[16]"));
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
