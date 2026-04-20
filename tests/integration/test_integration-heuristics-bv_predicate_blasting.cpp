// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"
#include "heuristics/bv_predicate_blasting.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}

	TEST_CASE("logging") {
		logging::trace();
	}

	TEST_CASE("bv_size") {
		default_bv_size = 4;
	}
}

tref parse_wff(const std::string& sample) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::wff }};
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

// Normalizes a BV wff formula string through the full pipeline and returns
// "T", "F", or another normalized form.
static std::string blast_normalize(const std::string& sample) {
	auto wff = parse_wff(sample);
	if (!wff) return "parse_error";
	auto result = normalizer<node_t>(wff);
	if (!result) return "null";
	return tau::get(result).to_str();
}

// Runs blast_normalize with a temporary bv size, then restores the original.
static std::string blast_normalize_with_size(size_t size, const std::string& sample) {
	auto prev = default_bv_size;
	default_bv_size = size;
	auto result = blast_normalize(sample);
	default_bv_size = prev;
	return result;
}

// ============================================================
// Correctness tests for bvadd, bvsub, bvmul, bit, bvshl,
// bvrhl_by_one, bvshl_by_one, bvdiv, bvmod, bved.
// ============================================================

// ------------------------------------------------------------
// bvadd: dividend + addend = sum (mod 2^N)
// ------------------------------------------------------------
TEST_SUITE("bvadd") {

	// 3 + 5 = 8  (no overflow)
	TEST_CASE("bvadd: 3 + 5 = 8") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv && x + { 5 }:bv = { 8 }:bv)") == "T");
	}

	// 3 + 5 ≠ 9
	TEST_CASE("bvadd: 3 + 5 != 9") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv && x + { 5 }:bv = { 9 }:bv)") == "F");
	}

	// 0 + 0 = 0
	TEST_CASE("bvadd: 0 + 0 = 0") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv && x + { 0 }:bv = { 0 }:bv)") == "T");
	}

	// 15 + 1 = 0  (4-bit wrap-around: 16 mod 16 = 0)
	TEST_CASE("bvadd: 15 + 1 = 0 for 4-bit (overflow)") {
		CHECK(blast_normalize("ex x (x = { 15 }:bv && x + { 1 }:bv = { 0 }:bv)") == "T");
	}

	// 7 + 9 = 0  (4-bit: 16 mod 16 = 0)
	TEST_CASE("bvadd: 7 + 9 = 0 for 4-bit (overflow)") {
		CHECK(blast_normalize("ex x (x = { 7 }:bv && x + { 9 }:bv = { 0 }:bv)") == "T");
	}

	// ex x: x + x = 2*x is always satisfiable (identity, not a fixed value)
	TEST_CASE("bvadd: ex x, x + x = x * 2") {
		// 6 + 6 = 12 for 4-bit (12 < 16, no overflow)
		CHECK(blast_normalize("ex x (x = { 6 }:bv && x + { 6 }:bv = { 12 }:bv)") == "T");
	}
}

// ------------------------------------------------------------
// bvsub: minuend - subtrahend = difference (mod 2^N)
// ------------------------------------------------------------
TEST_SUITE("bvsub") {

	// 8 - 5 = 3
	TEST_CASE("bvsub: 8 - 5 = 3") {
		CHECK(blast_normalize("ex x (x = { 8 }:bv && x - { 5 }:bv = { 3 }:bv)") == "T");
	}

	// 8 - 5 ≠ 4
	TEST_CASE("bvsub: 8 - 5 != 4") {
		CHECK(blast_normalize("ex x (x = { 8 }:bv && x - { 5 }:bv = { 4 }:bv)") == "F");
	}

	// 5 - 5 = 0
	TEST_CASE("bvsub: 5 - 5 = 0") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv && x - { 5 }:bv = { 0 }:bv)") == "T");
	}

	// 0 - 1 = 15  (4-bit underflow/wrap: -1 mod 16 = 15)
	TEST_CASE("bvsub: 0 - 1 = 15 for 4-bit (underflow)") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv && x - { 1 }:bv = { 15 }:bv)") == "T");
	}

	// 3 - 5 = 14  (4-bit: -2 mod 16 = 14)
	TEST_CASE("bvsub: 3 - 5 = 14 for 4-bit (underflow)") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv && x - { 5 }:bv = { 14 }:bv)") == "T");
	}
}

// ------------------------------------------------------------
// bvmul: multiplicand * constant_multiplier = product (mod 2^N)
// Only multiplication by a constant right operand is supported.
// ------------------------------------------------------------
TEST_SUITE("bvmul") {

	// 3 * 4 = 12
	TEST_CASE("bvmul: 3 * 4 = 12") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv && x * { 4 }:bv = { 12 }:bv)") == "T");
	}

	// 3 * 4 ≠ 13
	TEST_CASE("bvmul: 3 * 4 != 13") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv && x * { 4 }:bv = { 13 }:bv)") == "F");
	}

	// x * 0 = 0 for all x
	TEST_CASE("bvmul: x * 0 = 0") {
		CHECK(blast_normalize("ex x (x = { 7 }:bv && x * { 0 }:bv = { 0 }:bv)") == "T");
	}

	// x * 1 = x
	TEST_CASE("bvmul: x * 1 = x") {
		CHECK(blast_normalize("ex x (x = { 7 }:bv && x * { 1 }:bv = { 7 }:bv)") == "T");
	}

	// 3 * 6 = 2  (4-bit: 18 mod 16 = 2)
	TEST_CASE("bvmul: 3 * 6 = 2 for 4-bit (overflow)") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv && x * { 6 }:bv = { 2 }:bv)") == "T");
	}

	// 5 * 3 = 15
	TEST_CASE("bvmul: 5 * 3 = 15") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv && x * { 3 }:bv = { 15 }:bv)") == "T");
	}
}

// ------------------------------------------------------------
// bit: bit[i](x) = x & (1 << i)
// Tested indirectly via equality predicates on the masked value.
// ------------------------------------------------------------
TEST_SUITE("bit") {

	// bit[0](5) = 1  (5 = 0101 in 4-bit; bit 0 is 1)
	// Test: 5 & 1 = 1
	TEST_CASE("bit[0]: { 5 }:bv & { 1 }:bv = { 1 }:bv") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv && x & { 1 }:bv = { 1 }:bv)") == "T");
	}

	// bit[1](5) = 0  (5 = 0101; bit 1 is 0)
	// Test: 5 & 2 = 0
	TEST_CASE("bit[1]: { 5 }:bv & { 2 }:bv = { 0 }:bv") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv && x & { 2 }:bv = { 0 }:bv)") == "T");
	}

	// bit[2](5) = 1  (5 = 0101; bit 2 is 1)
	// Test: 5 & 4 = 4
	TEST_CASE("bit[2]: { 5 }:bv & { 4 }:bv = { 4 }:bv") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv && x & { 4 }:bv = { 4 }:bv)") == "T");
	}

	// bit[3](5) = 0  (5 = 0101; bit 3 is 0)
	// Test: 5 & 8 = 0
	TEST_CASE("bit[3]: { 5 }:bv & { 8 }:bv = { 0 }:bv") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv && x & { 8 }:bv = { 0 }:bv)") == "T");
	}

	// bit[0](6) = 0  (6 = 0110; bit 0 is 0)
	TEST_CASE("bit[0]: { 6 }:bv & { 1 }:bv = { 0 }:bv") {
		CHECK(blast_normalize("ex x (x = { 6 }:bv && x & { 1 }:bv = { 0 }:bv)") == "T");
	}

	// All bits of 0 are 0
	TEST_CASE("bit: all bits of { 0 }:bv are zero") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv && x & { 15 }:bv = { 0 }:bv)") == "T");
	}

	// All bits of 15 are 1
	TEST_CASE("bit: all bits of { 15 }:bv are one") {
		CHECK(blast_normalize("ex x (x = { 15 }:bv && x & { 15 }:bv = { 15 }:bv)") == "T");
	}
}

// ------------------------------------------------------------
// bvshl_by_one: shifted = base << 1 (single-step left shift)
// The LSB of shifted is 0; bit[i+1](shifted) = bit[i](base).
// ------------------------------------------------------------
TEST_SUITE("bvshl_by_one") {

	// 1 << 1 = 2  (0001 → 0010)
	TEST_CASE("bvshl_by_one: 1 << 1 = 2") {
		CHECK(blast_normalize("ex x ex y (x = { 1 }:bv && x << { 1 }:bv = y && y = { 2 }:bv)") == "T");
	}

	// 4 << 1 = 8  (0100 → 1000)
	TEST_CASE("bvshl_by_one: 4 << 1 = 8") {
		CHECK(blast_normalize("ex x ex y (x = { 4 }:bv && x << { 1 }:bv = y && y = { 8 }:bv)") == "T");
	}

	// 8 << 1 = 0  (1000 → 0000, MSB shifted out)
	TEST_CASE("bvshl_by_one: 8 << 1 = 0 for 4-bit (MSB shifts out)") {
		CHECK(blast_normalize("ex x ex y (x = { 8 }:bv && x << { 1 }:bv = y && y = { 0 }:bv)") == "T");
	}

	// 6 << 1 = 12  (0110 → 1100)
	TEST_CASE("bvshl_by_one: 6 << 1 = 12") {
		CHECK(blast_normalize("ex x ex y (x = { 6 }:bv && x << { 1 }:bv = y && y = { 12 }:bv)") == "T");
	}
}

// ------------------------------------------------------------
// bvrhl_by_one: shifted = base >> 1 (single-step right shift)
// The MSB of shifted is 0; bit[i-1](shifted) = bit[i](base).
// ------------------------------------------------------------
TEST_SUITE("bvrhl_by_one") {

	// 4 >> 1 = 2  (0100 → 0010)
	TEST_CASE("bvrhl_by_one: 4 >> 1 = 2") {
		CHECK(blast_normalize("ex x ex y (x = { 4 }:bv && x >> { 1 }:bv = y && y = { 2 }:bv)") == "T");
	}

	// 8 >> 1 = 4  (1000 → 0100)
	TEST_CASE("bvrhl_by_one: 8 >> 1 = 4") {
		CHECK(blast_normalize("ex x ex y (x = { 8 }:bv && x >> { 1 }:bv = y && y = { 4 }:bv)") == "T");
	}

	// 1 >> 1 = 0  (0001 → 0000, LSB shifts out)
	TEST_CASE("bvrhl_by_one: 1 >> 1 = 0 (LSB shifts out)") {
		CHECK(blast_normalize("ex x ex y (x = { 1 }:bv && x >> { 1 }:bv = y && y = { 0 }:bv)") == "T");
	}

	// 6 >> 1 = 3  (0110 → 0011)
	TEST_CASE("bvrhl_by_one: 6 >> 1 = 3") {
		CHECK(blast_normalize("ex x ex y (x = { 6 }:bv && x >> { 1 }:bv = y && y = { 3 }:bv)") == "T");
	}
}

// ------------------------------------------------------------
// bvdiv: dividend / constant_divisor = quotient
// Quotient satisfies: dividend = divisor * quotient + remainder,
// remainder < divisor.
// ------------------------------------------------------------
TEST_SUITE("bvdiv") {

	// 10 / 3 = 3  (3*3 + 1 = 10, remainder 1 < 3)
	TEST_CASE("bvdiv: 10 / 3 = 3") {
		CHECK(blast_normalize("ex x ex y (x = { 10 }:bv && x / { 3 }:bv = y && y = { 3 }:bv)") == "T");
	}

	// 10 / 3 ≠ 2
	TEST_CASE("bvdiv: 10 / 3 != 2") {
		CHECK(blast_normalize("ex x ex y (x = { 10 }:bv && x / { 3 }:bv = y && y = { 2 }:bv)") == "F");
	}

	// 9 / 3 = 3  (exact, no remainder)
	TEST_CASE("bvdiv: 9 / 3 = 3") {
		CHECK(blast_normalize("ex x ex y (x = { 9 }:bv && x / { 3 }:bv = y && y = { 3 }:bv)") == "T");
	}

	// 0 / 5 = 0
	TEST_CASE("bvdiv: 0 / 5 = 0") {
		CHECK(blast_normalize("ex x ex y (x = { 0 }:bv && x / { 5 }:bv = y && y = { 0 }:bv)") == "T");
	}

	// 7 / 4 = 1  (4*1 + 3 = 7, remainder 3 < 4)
	TEST_CASE("bvdiv: 7 / 4 = 1") {
		CHECK(blast_normalize("ex x ex y (x = { 7 }:bv && x / { 4 }:bv = y && y = { 1 }:bv)") == "T");
	}
}

// ------------------------------------------------------------
// bvmod: dividend % constant_divisor = remainder
// ------------------------------------------------------------
TEST_SUITE("bvmod") {

	// 10 % 3 = 1
	TEST_CASE("bvmod: 10 % 3 = 1") {
		CHECK(blast_normalize("ex x ex y (x = { 10 }:bv && x % { 3 }:bv = y && y = { 1 }:bv)") == "T");
	}

	// 10 % 3 ≠ 2
	TEST_CASE("bvmod: 10 % 3 != 2") {
		CHECK(blast_normalize("ex x ex y (x = { 10 }:bv && x % { 3 }:bv = y && y = { 2 }:bv)") == "F");
	}

	// 9 % 3 = 0  (exact division)
	TEST_CASE("bvmod: 9 % 3 = 0") {
		CHECK(blast_normalize("ex x ex y (x = { 9 }:bv && x % { 3 }:bv = y && y = { 0 }:bv)") == "T");
	}

	// 7 % 4 = 3
	TEST_CASE("bvmod: 7 % 4 = 3") {
		CHECK(blast_normalize("ex x ex y (x = { 7 }:bv && x % { 4 }:bv = y && y = { 3 }:bv)") == "T");
	}

	// remainder is always < divisor: all x, x % 4 < 4
	TEST_CASE("bvmod: remainder always < divisor") {
		CHECK(blast_normalize("all x x:bv % { 4 }:bv < { 4 }:bv") == "T");
	}
}

// ------------------------------------------------------------
// bved: Euclidean division — both quotient and remainder at once.
// bved(dividend, divisor, quotient, remainder) holds iff
//   dividend = divisor * quotient + remainder  &&  remainder < divisor.
// ------------------------------------------------------------
TEST_SUITE("bved") {

	// 10 = 3*3 + 1  (quotient=3, remainder=1)
	TEST_CASE("bved: bvdiv and bvmod are consistent with bved for 10/3") {
		// Both quotient and remainder are exposed; check they agree with bvdiv/bvmod.
		CHECK(blast_normalize(
			"ex x ex q ex r (x = { 10 }:bv && x / { 3 }:bv = q && x % { 3 }:bv = r"
			" && q = { 3 }:bv && r = { 1 }:bv)") == "T");
	}

	// 9 = 3*3 + 0  (exact: quotient=3, remainder=0)
	TEST_CASE("bved: 9/3 exact (quotient=3, remainder=0)") {
		CHECK(blast_normalize(
			"ex x ex q ex r (x = { 9 }:bv && x / { 3 }:bv = q && x % { 3 }:bv = r"
			" && q = { 3 }:bv && r = { 0 }:bv)") == "T");
	}

	// 7 = 4*1 + 3  (quotient=1, remainder=3)
	TEST_CASE("bved: 7/4 (quotient=1, remainder=3)") {
		CHECK(blast_normalize(
			"ex x ex q ex r (x = { 7 }:bv && x / { 4 }:bv = q && x % { 4 }:bv = r"
			" && q = { 1 }:bv && r = { 3 }:bv)") == "T");
	}

	// Wrong remainder: 10 / 3 with remainder=2 is UNSAT
	TEST_CASE("bved: wrong remainder is UNSAT") {
		CHECK(blast_normalize(
			"ex x ex q ex r (x = { 10 }:bv && x / { 3 }:bv = q && x % { 3 }:bv = r"
			" && r = { 2 }:bv)") == "F");
	}
}

// ------------------------------------------------------------
// Bug 1: bvlt base case returns T instead of F
//
// In bvlt_rules(), the base case at index 0 is set to T:
//   _bvlt[0](x, y) = T
// According to the algorithm comment it should be F:
//   _bvlt[0](x, y) = F  (no remaining bits => numbers are equal => not less-than)
//
// Additionally, the call starts at bitwidth-1 but the general rule checks bit[n]
// (not bit[n-1] as the comment states), meaning bit 0 (LSB) is never compared.
// With T as the base, any comparison where all higher bits are equal incorrectly
// returns True.
// ------------------------------------------------------------
TEST_SUITE("bvlt bugs") {

	// x < x must always be False: no value is strictly less than itself.
	// BUG: bvlt(x,x) with T base returns True for all x with equal bits
	//      => ex x (True) = True (wrong).
	TEST_CASE("bvlt: x < x is never satisfiable") {
		CHECK(blast_normalize("ex x x:bv < x:bv") == "F");
	}

	// {1}:bv[4] = 0001, {0}:bv[4] = 0000. Bits 3,2,1 are equal (all 0).
	// bvlt never reaches bit 0 and T base fires => returns True.
	// BUG: 1 < 0 should be False, but returns True.
	TEST_CASE("bvlt: 1 < 0 should be F for 4-bit") {
		CHECK(blast_normalize("ex x (x = { 1 }:bv && x < { 0 }:bv)") == "F");
	}

	// {3}:bv[2] = 11, {2}:bv[2] = 10. Both have MSB=1 (equal).
	// BUG: equal MSBs trigger T base case => 3 < 2 incorrectly returns True.
	TEST_CASE("bvlt: 3 < 2 should be F for 2-bit") {
		CHECK(blast_normalize_with_size(2, "ex x (x = { 3 }:bv && x < { 2 }:bv)") == "F");
	}

	// {0}:bv[4] < {0}:bv[4]: equal values, should be False.
	// BUG: equal bits all the way to bit 0 => T base fires => True.
	TEST_CASE("bvlt: 0 < 0 should be F") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv && x < { 0 }:bv)") == "F");
	}

	// Sanity check: correct direction, different MSBs should still work.
	// {0}:bv[4] < {8}:bv[4]: MSB of 8 is 1, MSB of 0 is 0 => True.
	TEST_CASE("bvlt: 0 < 8 should be T for 4-bit (MSBs differ)") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv && x < { 8 }:bv)") == "T");
	}
}

// ------------------------------------------------------------
// Bug 2: bvgteq derived from broken bvlt
//
// bvgteq(l, r) = neg(bvlt(l, r))
// With bvlt(x, x) = True (due to T base case), bvgteq(x, x) = False.
// But x >= x is always True.
// ------------------------------------------------------------
TEST_SUITE("bvgteq bugs") {

	// all x: x >= x should be True (universally valid).
	// BUG: bvgteq(x,x) = neg(bvlt(x,x)) = neg(T) = F => all x F = F.
	TEST_CASE("bvgteq: all x, x >= x should be T") {
		CHECK(blast_normalize("all x x:bv >= x:bv") == "T");
	}

	// 3 >= 2 should be True (3 > 2).
	TEST_CASE("bvgteq: 3 >= 2 should be T for 2-bit") {
		CHECK(blast_normalize_with_size(2, "ex x (x = { 3 }:bv && x >= { 2 }:bv)") == "T");
	}
}

// ------------------------------------------------------------
// Bug 3: bvnlteq semantic error — returns bvlt instead of bvgt
//
// In bv_predicate_blasting.h (line ~177):
//   bvnlteq(l, r) = !(l <= r) = l > r
// But the implementation is:
//   bvnlteq(l, r) { return bvlt<node>(left, right); }   // returns l < r  ← WRONG
// It should return bvgt(l, r).
// ------------------------------------------------------------
TEST_SUITE("bvnlteq bugs") {

	// !(3 <= 1) = !(False) = True, since 3 > 1.
	// BUG: bvnlteq(3,1) returns bvlt(3,1) = False => ex x (...) = F (wrong).
	TEST_CASE("bvnlteq: 3 !<= 1 should be T") {
		CHECK(blast_normalize_with_size(2, "ex x (x = { 3 }:bv && x !<= { 1 }:bv)") == "T");
	}

	// !(x <= x) = !(True) = False. No x can satisfy x !<= x.
	// BUG: bvnlteq(x,x) = bvlt(x,x) = T (due to T base) => ex x T = T (wrong).
	TEST_CASE("bvnlteq: x !<= x is never satisfiable") {
		CHECK(blast_normalize("ex x x:bv !<= x:bv") == "F");
	}
}

// ------------------------------------------------------------
// Bug 4: bvrhl_rule uses make_bvshl_call for the head
//
// In bv_predicate_blasting_logic.tmpl.h (line ~592):
//   auto head = make_bvshl_call<node>(base, count, shifted);  // ← WRONG
// It should be:
//   auto head = make_bvrhl_call<node>(base, count, shifted);
//
// As a result, the right-shift rule is registered under the left-shift name,
// so right-shift operations use the left-shift rule (i.e., shift left instead
// of right, giving a completely wrong result).
// ------------------------------------------------------------
TEST_SUITE("bvrhl bugs") {

	// {4}:bv[4] = 0100, right shift by 1 = 0010 = 2.
	// BUG: bvrhl_rule has wrong head (bvshl), so right shift uses left-shift rule:
	//      4 << 1 = 8 != 2, making this formula unsatisfiable (wrong).
	TEST_CASE("bvrhl: 4 >> 1 = 2 for 4-bit") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 4 }:bv && x >> { 1 }:bv = y && y = { 2 }:bv)") == "T");
	}

	// {8}:bv[4] = 1000, right shift by 2 = 0010 = 2.
	// BUG: bvrhl uses left-shift rule: 8 << 2 = 32 mod 16 = 0 != 2.
	TEST_CASE("bvrhl: 8 >> 2 = 2 for 4-bit") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 8 }:bv && x >> { 2 }:bv = y && y = { 2 }:bv)") == "T");
	}

	// {6}:bv[4] = 0110, right shift by 1 = 0011 = 3.
	// BUG: 6 << 1 = 12 != 3.
	TEST_CASE("bvrhl: 6 >> 1 = 3 for 4-bit") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 6 }:bv && x >> { 1 }:bv = y && y = { 3 }:bv)") == "T");
	}
}

// ------------------------------------------------------------
// Bug 5: bvshl_rule off-by-one and flawed loop logic
//
// In bv_predicate_blasting_logic.tmpl.h (line ~525):
//   if (i + offset > bitwidth) { ... }  // should be >= bitwidth
//
// With > instead of >=, when i + offset == bitwidth the code tries to
// access bit[bitwidth] of the shifted vector (out-of-bounds index).
// Furthermore, the loop constrains source-bit index i to zero when its
// destination overflows, but never zeroes the low destination bits
// (indices 0 to offset-1), leaving them unconstrained.
// ------------------------------------------------------------
TEST_SUITE("bvshl bugs") {

	// {3}:bv[4] = 0011, left shift by 3 = 1000 = 8.
	// i=1: i+offset = 4 >= bitwidth=4 → source bit 1 shifts out (correct).
	// BUG: with > instead of >=, i=1 tries to read bit[4] (OOB) and
	//      constrains bit[4] (treated as 0) to equal bit[1](base)=1 → UNSAT (wrong).
	TEST_CASE("bvshl: 3 << 3 = 8 for 4-bit (off-by-one at bitwidth boundary)") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 3 }:bv && x << { 3 }:bv = y && y = { 8 }:bv)") == "T");
	}

	// {1}:bv[4] = 0001, left shift by 1 = 0010 = 2.
	// Low bit 0 of shifted should be zero but the loop never constrains it.
	TEST_CASE("bvshl: 1 << 1 = 2 for 4-bit (low bits must be zero)") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 1 }:bv && x << { 1 }:bv = y && y = { 2 }:bv)") == "T");
	}

	// Verify that the result of shifting is unique (shifted[0] must be 0).
	// If low bits are not constrained to zero, the formula may be SAT for y = 3.
	TEST_CASE("bvshl: 1 << 1 != 3 for 4-bit (low bits must be zero, not 1)") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 1 }:bv && x << { 1 }:bv = y && y = { 3 }:bv)") == "F");
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
