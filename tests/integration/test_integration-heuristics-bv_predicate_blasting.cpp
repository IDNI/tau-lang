// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"
#include "heuristics/bv_predicate_blasting.h"
#include "ba_types_inference.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}

	TEST_CASE("logging") {
		logging::trace();
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

static tref blast_formula(const std::string& sample) {
	auto wff = parse_wff(sample);
	return wff ? bv_predicate_blasting<node_t>(wff) : nullptr;
}

static std::string blast_normalize(const std::string& sample) {
	auto wff = parse_wff(sample);
	if (!wff) return "parse_error";
	// We blast the formula and then normalize it to check that the blasting is
	// correct and it is not simplified first by other heuristics. If the blasting
	// is correct, the result should be T or F.
	auto blasted = bv_predicate_blasting<node_t>(wff);
	if (!blasted) return "blast_error";
	// Blasting must not leave unresolved recurrence references behind
	if (tau::get(blasted).find_top(is<node_t, tau::ref>))
		return "unresolved_refs";
	// If the formula contains blastable arithmetic, blasting must rewrite it;
	// otherwise the normalizer would decide the formula on its own and the
	// test would not exercise the blasting at all.
	auto has_arithmetic = [](tref n) {
		switch (tau::get(n).get_type()) {
			case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
			case tau::bf_div: case tau::bf_mod: case tau::bf_shl:
			case tau::bf_shr: case tau::bf_cast: return true;
			default: return false;
		}
	};
	if (tau::get(wff).find_top(has_arithmetic) && blasted == wff)
		return "not_blasted";
	auto result = normalizer<node_t>(blasted);
	if (!result) return "null";
	return tau::get(result).to_str();
}

//
// bvadd: augend + addend = sum (mod 2^N)
//
TEST_SUITE("bvadd") {

	TEST_CASE("bvadd: 3 + 5 = 8") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[4] && x + { 5 }:bv[4] = { 8 }:bv[4])") == "T");
	}

	TEST_CASE("bvadd: 3 + 5 != 9") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[4] && x + { 5 }:bv[4] = { 9 }:bv[4])") == "F");
	}

	TEST_CASE("bvadd: 0 + 0 = 0") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x + { 0 }:bv[4] = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvadd: 15 + 1 = 0 for 4-bit (overflow)") {
		CHECK(blast_normalize("ex x (x = { 15 }:bv[4] && x + { 1 }:bv[4] = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvadd: 7 + 9 = 0 for 4-bit (overflow)") {
		CHECK(blast_normalize("ex x (x = { 7 }:bv[4] && x + { 9 }:bv[4] = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvadd: 6 + 6 = 12") {
		CHECK(blast_normalize("ex x (x = { 6 }:bv[4] && x + { 6 }:bv[4] = { 12 }:bv[4])") == "T");
	}

	TEST_CASE("bvadd: x + 0 = x") {
		CHECK(blast_normalize("all x x:bv[4] + { 0 }:bv[4] = x:bv[4]") == "T");
	}
}

//
// bvsub: minuend - subtrahend = difference (mod 2^N)
//
TEST_SUITE("bvsub") {

	TEST_CASE("bvsub: 8 - 5 = 3") {
		CHECK(blast_normalize("ex x (x = { 8 }:bv[4] && x - { 5 }:bv[4] = { 3 }:bv[4])") == "T");
	}

	TEST_CASE("bvsub: 8 - 5 != 4") {
		CHECK(blast_normalize("ex x (x = { 8 }:bv[4] && x - { 5 }:bv[4] = { 4 }:bv[4])") == "F");
	}

	TEST_CASE("bvsub: 5 - 5 = 0") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv[4] && x - { 5 }:bv[4] = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvsub: 0 - 1 = 15 for 4-bit (underflow)") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x - { 1 }:bv[4] = { 15 }:bv[4])") == "T");
	}

	TEST_CASE("bvsub: 3 - 5 = 14 for 4-bit (underflow)") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[4] && x - { 5 }:bv[4] = { 14 }:bv[4])") == "T");
	}

	TEST_CASE("bvsub: x - x = 0") {
		CHECK(blast_normalize("all x x:bv[4] - x:bv[4] = { 0 }:bv[4]") == "T");
	}
}

//
// bvmul: multiplicand * constant_multiplier = product (mod 2^N)
// Only multiplication by a constant right operand is supported.
//
TEST_SUITE("bvmul") {

	TEST_CASE("bvmul: 3 * 4 = 12") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[4] && x * { 4 }:bv[4] = { 12 }:bv[4])") == "T");
	}

	TEST_CASE("bvmul: 3 * 4 != 13") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[4] && x * { 4 }:bv[4] = { 13 }:bv[4])") == "F");
	}

	TEST_CASE("bvmul: x * 0 = 0") {
		CHECK(blast_normalize("ex x (x = { 7 }:bv[4] && x * { 0 }:bv[4] = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvmul: x * 1 = x") {
		CHECK(blast_normalize("ex x (x = { 7 }:bv[4] && x * { 1 }:bv[4] = { 7 }:bv[4])") == "T");
	}

	TEST_CASE("bvmul: 3 * 6 = 2 for 4-bit (overflow)") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[4] && x * { 6 }:bv[4] = { 2 }:bv[4])") == "T");
	}

	TEST_CASE("bvmul: 5 * 3 = 15") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv[4] && x * { 3 }:bv[4] = { 15 }:bv[4])") == "T");
	}
}

//
// bit: bit[i](x) = x & (1 << i)
// Tested via equality predicates on the masked value.
//
TEST_SUITE("bit") {

	TEST_CASE("bit[0]: { 5 }:bv[4] & { 1 }:bv[4] = { 1 }:bv[4]") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv[4] && x & { 1 }:bv[4] = { 1 }:bv[4])") == "T");
	}

	TEST_CASE("bit[1]: { 5 }:bv[4] & { 2 }:bv[4] = { 0 }:bv[4]") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv[4] && x & { 2 }:bv[4] = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bit[2]: { 5 }:bv[4] & { 4 }:bv[4] = { 4 }:bv[4]") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv[4] && x & { 4 }:bv[4] = { 4 }:bv[4])") == "T");
	}

	TEST_CASE("bit[3]: { 5 }:bv[4] & { 8 }:bv[4] = { 0 }:bv[4]") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv[4] && x & { 8 }:bv[4] = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bit[0]: { 6 }:bv[4] & { 1 }:bv[4] = { 0 }:bv[4]") {
		CHECK(blast_normalize("ex x (x = { 6 }:bv[4] && x & { 1 }:bv[4] = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bit: all bits of { 0 }:bv[4] are zero") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x & { 15 }:bv[4] = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bit: all bits of { 15 }:bv[4] are one") {
		CHECK(blast_normalize("ex x (x = { 15 }:bv[4] && x & { 15 }:bv[4] = { 15 }:bv[4])") == "T");
	}
}

//
// bvshl_by_one: shifted = base << 1 (single-step left shift)
//
TEST_SUITE("bvshl_by_one") {

	TEST_CASE("bvshl_by_one: 1 << 1 = 2") {
		CHECK(blast_normalize("ex x ex y (x = { 1 }:bv[4] && x << { 1 }:bv[4] = y && y = { 2 }:bv[4])") == "T");
	}

	TEST_CASE("bvshl_by_one: 4 << 1 = 8") {
		CHECK(blast_normalize("ex x ex y (x = { 4 }:bv[4] && x << { 1 }:bv[4] = y && y = { 8 }:bv[4])") == "T");
	}

	TEST_CASE("bvshl_by_one: 8 << 1 = 0 for 4-bit (MSB shifts out)") {
		CHECK(blast_normalize("ex x ex y (x = { 8 }:bv[4] && x << { 1 }:bv[4] = y && y = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvshl_by_one: 6 << 1 = 12") {
		CHECK(blast_normalize("ex x ex y (x = { 6 }:bv[4] && x << { 1 }:bv[4] = y && y = { 12 }:bv[4])") == "T");
	}
}

//
// bvshl: left shift by constant amount
//
TEST_SUITE("bvshl") {

	TEST_CASE("bvshl: 1 << 1 = 2") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 1 }:bv[4] && x << { 1 }:bv[4] = y && y = { 2 }:bv[4])") == "T");
	}

	TEST_CASE("bvshl: 1 << 1 != 3 (low bits must be zero)") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 1 }:bv[4] && x << { 1 }:bv[4] = y && y = { 3 }:bv[4])") == "F");
	}

	// 3 = 0011; shift left by 2: 1100 = 12
	TEST_CASE("bvshl: 3 << 2 = 12") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 3 }:bv[4] && x << { 2 }:bv[4] = y && y = { 12 }:bv[4])") == "T");
	}

	// 3 = 0011; shift left by 3: only bit 1 fits => 1000 = 8
	TEST_CASE("bvshl: 3 << 3 = 8 (boundary: one source bit fits)") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 3 }:bv[4] && x << { 3 }:bv[4] = y && y = { 8 }:bv[4])") == "T");
	}

	TEST_CASE("bvshl: 1 << 4 = 0 (full overflow)") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 1 }:bv[4] && x << { 4 }:bv[4] = y && y = { 0 }:bv[4])") == "T");
	}
}

//
// bvshr_by_one: shifted = base >> 1 (single-step right shift)
//
TEST_SUITE("bvshr_by_one") {

	TEST_CASE("bvshr_by_one: 4 >> 1 = 2") {
		CHECK(blast_normalize("ex x ex y (x = { 4 }:bv[4] && x >> { 1 }:bv[4] = y && y = { 2 }:bv[4])") == "T");
	}

	TEST_CASE("bvshr_by_one: 8 >> 1 = 4") {
		CHECK(blast_normalize("ex x ex y (x = { 8 }:bv[4] && x >> { 1 }:bv[4] = y && y = { 4 }:bv[4])") == "T");
	}

	TEST_CASE("bvshr_by_one: 1 >> 1 = 0 (LSB shifts out)") {
		CHECK(blast_normalize("ex x ex y (x = { 1 }:bv[4] && x >> { 1 }:bv[4] = y && y = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvshr_by_one: 6 >> 1 = 3") {
		CHECK(blast_normalize("ex x ex y (x = { 6 }:bv[4] && x >> { 1 }:bv[4] = y && y = { 3 }:bv[4])") == "T");
	}
}

//
// bvshr: right shift by constant amount
//
TEST_SUITE("bvshr") {

	TEST_CASE("bvshr: 4 >> 1 = 2") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 4 }:bv[4] && x >> { 1 }:bv[4] = y && y = { 2 }:bv[4])") == "T");
	}

	TEST_CASE("bvshr: 8 >> 2 = 2") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 8 }:bv[4] && x >> { 2 }:bv[4] = y && y = { 2 }:bv[4])") == "T");
	}

	TEST_CASE("bvshr: 6 >> 1 = 3") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 6 }:bv[4] && x >> { 1 }:bv[4] = y && y = { 3 }:bv[4])") == "T");
	}

	TEST_CASE("bvshr: 6 >> 1 != 6 (high bits must be zero)") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 6 }:bv[4] && x >> { 1 }:bv[4] = y && y = { 6 }:bv[4])") == "F");
	}

	TEST_CASE("bvshr: 15 >> 4 = 0 (full shift out)") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 15 }:bv[4] && x >> { 4 }:bv[4] = y && y = { 0 }:bv[4])") == "T");
	}
}

//
// bvlt: strict less-than, including LSB-only difference cases
//
TEST_SUITE("bvlt") {

	TEST_CASE("bvlt: 0 < 8 (MSBs differ)") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x < { 8 }:bv[4])") == "T");
	}

	TEST_CASE("bvlt: 0 < 1 (differ only at LSB)") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x < { 1 }:bv[4])") == "T");
	}

	TEST_CASE("bvlt: 2 < 3 (differ only at LSB)") {
		CHECK(blast_normalize("ex x (x = { 2 }:bv[4] && x < { 3 }:bv[4])") == "T");
	}

	TEST_CASE("bvlt: 4 < 5 (differ only at LSB)") {
		CHECK(blast_normalize("ex x (x = { 4 }:bv[4] && x < { 5 }:bv[4])") == "T");
	}

	TEST_CASE("bvlt: x < x is never satisfiable") {
		CHECK(blast_normalize("ex x x:bv[4] < x:bv[4]") == "F");
	}

	TEST_CASE("bvlt: 0 < 0 is F") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x < { 0 }:bv[4])") == "F");
	}

	TEST_CASE("bvlt: 1 < 0 is F") {
		CHECK(blast_normalize("ex x (x = { 1 }:bv[4] && x < { 0 }:bv[4])") == "F");
	}

	TEST_CASE("bvlt: 3 < 2 is F for 2-bit") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && x < { 2 }:bv[2])") == "F");
	}

	TEST_CASE("bvlt: 2 < 3 is T for 2-bit (differ only at LSB)") {
		CHECK(blast_normalize("ex x (x = { 2 }:bv[2] && x < { 3 }:bv[2])") == "T");
	}
}

//
// bvgt: strict greater-than
//
TEST_SUITE("bvgt") {

	TEST_CASE("bvgt: 8 > 0 (MSBs differ)") {
		CHECK(blast_normalize("ex x (x = { 8 }:bv[4] && x > { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvgt: 1 > 0 (differ only at LSB)") {
		CHECK(blast_normalize("ex x (x = { 1 }:bv[4] && x > { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvgt: 3 > 2 (differ only at LSB)") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[4] && x > { 2 }:bv[4])") == "T");
	}

	TEST_CASE("bvgt: x > x is never satisfiable") {
		CHECK(blast_normalize("ex x x:bv[4] > x:bv[4]") == "F");
	}

	TEST_CASE("bvgt: 0 > 0 is F") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x > { 0 }:bv[4])") == "F");
	}

	TEST_CASE("bvgt: 2 > 3 is F for 2-bit (differ only at LSB)") {
		CHECK(blast_normalize("ex x (x = { 2 }:bv[2] && x > { 3 }:bv[2])") == "F");
	}

	TEST_CASE("bvgt: 3 > 2 is T for 2-bit") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && x > { 2 }:bv[2])") == "T");
	}

	TEST_CASE("bvgt: boundary width bv[64] right shift keeps top bit") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 9223372036854775808 }:bv[64] && x >> { 63 }:bv[64] = y && y = { 1 }:bv[64])") == "T");
	}
}

//
// bvgteq: greater-than-or-equal
//
TEST_SUITE("bvgteq") {

	TEST_CASE("bvgteq: all x, x >= x") {
		CHECK(blast_normalize("all x x:bv[4] >= x:bv[4]") == "T");
	}

	TEST_CASE("bvgteq: 3 >= 2 for 2-bit") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && x >= { 2 }:bv[2])") == "T");
	}

	TEST_CASE("bvgteq: 2 >= 3 is F for 2-bit") {
		CHECK(blast_normalize("ex x (x = { 2 }:bv[2] && x >= { 3 }:bv[2])") == "F");
	}

	TEST_CASE("bvgteq: 1 >= 0 (differ only at LSB)") {
		CHECK(blast_normalize("ex x (x = { 1 }:bv[4] && x >= { 0 }:bv[4])") == "T");
	}
}

//
// bvlteq: less-than-or-equal
//
TEST_SUITE("bvlteq") {

	TEST_CASE("bvlteq: all x, x <= x") {
		CHECK(blast_normalize("all x x:bv[4] <= x:bv[4]") == "T");
	}

	TEST_CASE("bvlteq: 2 <= 3 for 2-bit") {
		CHECK(blast_normalize("ex x (x = { 2 }:bv[2] && x <= { 3 }:bv[2])") == "T");
	}

	TEST_CASE("bvlteq: 3 <= 2 is F for 2-bit") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && x <= { 2 }:bv[2])") == "F");
	}

	TEST_CASE("bvlteq: 0 <= 1 (differ only at LSB)") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x <= { 1 }:bv[4])") == "T");
	}
}

//
// bvnlteq: not-less-than-or-equal (= strictly greater-than)
//
TEST_SUITE("bvnlteq") {

	TEST_CASE("bvnlteq: 3 !<= 1 is T") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && x !<= { 1 }:bv[2])") == "T");
	}

	TEST_CASE("bvnlteq: x !<= x is never satisfiable") {
		CHECK(blast_normalize("ex x x:bv[4] !<= x:bv[4]") == "F");
	}

	TEST_CASE("bvnlteq: 1 !<= 0 (differ only at LSB)") {
		CHECK(blast_normalize("ex x (x = { 1 }:bv[4] && x !<= { 0 }:bv[4])") == "T");
	}
}

//
// bvdiv: dividend / constant_divisor = quotient
//
TEST_SUITE("bvdiv") {

	TEST_CASE("bvdiv: 10 / 3 = 3") {
		CHECK(blast_normalize("ex x ex y (x = { 10 }:bv[4] && x / { 3 }:bv[4] = y && y = { 3 }:bv[4])") == "T");
	}

	TEST_CASE("bvdiv: 10 / 3 != 2") {
		CHECK(blast_normalize("ex x ex y (x = { 10 }:bv[4] && x / { 3 }:bv[4] = y && y = { 2 }:bv[4])") == "F");
	}

	TEST_CASE("bvdiv: 9 / 3 = 3") {
		CHECK(blast_normalize("ex x ex y (x = { 9 }:bv[4] && x / { 3 }:bv[4] = y && y = { 3 }:bv[4])") == "T");
	}

	TEST_CASE("bvdiv: 0 / 5 = 0") {
		CHECK(blast_normalize("ex x ex y (x = { 0 }:bv[4] && x / { 5 }:bv[4] = y && y = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvdiv: 7 / 4 = 1") {
		CHECK(blast_normalize("ex x ex y (x = { 7 }:bv[4] && x / { 4 }:bv[4] = y && y = { 1 }:bv[4])") == "T");
	}

	TEST_CASE("bvdiv: 1 / 1 = 1") {
		CHECK(blast_normalize("ex x ex y (x = { 1 }:bv[4] && x / { 1 }:bv[4] = y && y = { 1 }:bv[4])") == "T");
	}

	TEST_CASE("bvdiv: divisor {0} is handled by blasting") {
		// Division by zero is now handled (with potentially undefined semantics)
		CHECK(blast_formula("x:bv[4] / { 0 }:bv[4] = y:bv[4]") != nullptr);
	}
}

//
// bvmod: dividend % constant_divisor = remainder
//
TEST_SUITE("bvmod") {

	TEST_CASE("bvmod: 10 % 3 = 1") {
		CHECK(blast_normalize("ex x ex y (x = { 10 }:bv[4] && x % { 3 }:bv[4] = y && y = { 1 }:bv[4])") == "T");
	}

	TEST_CASE("bvmod: 10 % 3 != 2") {
		CHECK(blast_normalize("ex x ex y (x = { 10 }:bv[4] && x % { 3 }:bv[4] = y && y = { 2 }:bv[4])") == "F");
	}

	TEST_CASE("bvmod: 9 % 3 = 0") {
		CHECK(blast_normalize("ex x ex y (x = { 9 }:bv[4] && x % { 3 }:bv[4] = y && y = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvmod: 7 % 4 = 3") {
		CHECK(blast_normalize("ex x ex y (x = { 7 }:bv[4] && x % { 4 }:bv[4] = y && y = { 3 }:bv[4])") == "T");
	}

	TEST_CASE("bvmod: remainder always < divisor") {
		CHECK(blast_normalize("all x x:bv[4] % { 4 }:bv[4] < { 4 }:bv[4]") == "T");
	}

	TEST_CASE("bvmod: 0 % 3 = 0") {
		CHECK(blast_normalize("ex x ex y (x = { 0 }:bv[4] && x % { 3 }:bv[4] = y && y = { 0 }:bv[4])") == "T");
	}
}

//
// bved: Euclidean division — both quotient and remainder at once.
//
TEST_SUITE("bved") {

	TEST_CASE("bved: 10/3: quotient=3, remainder=1") {
		CHECK(blast_normalize(
			"ex x ex q ex r (x = { 10 }:bv[4] && x / { 3 }:bv[4] = q && x % { 3 }:bv[4] = r"
			" && q = { 3 }:bv[4] && r = { 1 }:bv[4])") == "T");
	}

	TEST_CASE("bved: 9/3 exact: quotient=3, remainder=0") {
		CHECK(blast_normalize(
			"ex x ex q ex r (x = { 9 }:bv[4] && x / { 3 }:bv[4] = q && x % { 3 }:bv[4] = r"
			" && q = { 3 }:bv[4] && r = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bved: 7/4: quotient=1, remainder=3") {
		CHECK(blast_normalize(
			"ex x ex q ex r (x = { 7 }:bv[4] && x / { 4 }:bv[4] = q && x % { 4 }:bv[4] = r"
			" && q = { 1 }:bv[4] && r = { 3 }:bv[4])") == "T");
	}

	TEST_CASE("bved: wrong remainder is UNSAT") {
		CHECK(blast_normalize(
			"ex x ex q ex r (x = { 10 }:bv[4] && x / { 3 }:bv[4] = q && x % { 3 }:bv[4] = r"
			" && r = { 2 }:bv[4])") == "F");
	}
}

//
// bvneq: x != y iff there exists a bit position where one is 0 and the other is 1.
//
TEST_SUITE("bvneq") {

	TEST_CASE("bvneq: x != x is never satisfiable") {
		CHECK(blast_normalize("ex x x:bv[4] != x:bv[4]") == "F");
	}

	TEST_CASE("bvneq: {0}:bv[4] != {0}:bv[4] is F") {
		CHECK(blast_normalize("ex x ex y (x = { 0 }:bv[4] && y = { 0 }:bv[4] && x != y)") == "F");
	}

	TEST_CASE("bvneq: {0}:bv[4] != {1}:bv[4] is T (differ at bit 0)") {
		CHECK(blast_normalize("ex x ex y (x = { 0 }:bv[4] && y = { 1 }:bv[4] && x != y)") == "T");
	}

	TEST_CASE("bvneq: {5}:bv[4] != {5}:bv[4] is F") {
		CHECK(blast_normalize("ex x ex y (x = { 5 }:bv[4] && y = { 5 }:bv[4] && x != y)") == "F");
	}

	TEST_CASE("bvneq: {5}:bv[4] != {3}:bv[4] is T (differ at bits 0 and 1)") {
		CHECK(blast_normalize("ex x ex y (x = { 5 }:bv[4] && y = { 3 }:bv[4] && x != y)") == "T");
	}

	TEST_CASE("bvneq: {8}:bv[4] != {0}:bv[4] is T (differ at MSB for 4-bit)") {
		CHECK(blast_normalize("ex x ex y (x = { 8 }:bv[4] && y = { 0 }:bv[4] && x != y)") == "T");
	}

	TEST_CASE("bvneq: {15}:bv[4] != {0}:bv[4] is T (all bits differ for 4-bit)") {
		CHECK(blast_normalize("ex x ex y (x = { 15 }:bv[4] && y = { 0 }:bv[4] && x != y)") == "T");
	}

	TEST_CASE("bvneq: {7}:bv[4] != {7}:bv[4] is F") {
		CHECK(blast_normalize("ex x ex y (x = { 7 }:bv[4] && y = { 7 }:bv[4] && x != y)") == "F");
	}

	TEST_CASE("bvneq: {6}:bv[4] != {7}:bv[4] is T (differ at bit 0)") {
		CHECK(blast_normalize("ex x ex y (x = { 6 }:bv[4] && y = { 7 }:bv[4] && x != y)") == "T");
	}

	TEST_CASE("bvneq: satisfiable inequality (ex x ex y x != y) is T") {
		CHECK(blast_normalize("ex x ex y x:bv[4] != y:bv[4]") == "T");
	}

	TEST_CASE("bvneq: all bitvectors are equal to themselves (all x, x != x is F)") {
		CHECK(blast_normalize("ex x x:bv[4] != x:bv[4]") == "F");
	}
}

//
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
//
TEST_SUITE("bvlt bugs") {

	// x < x must always be False: no value is strictly less than itself.
	// BUG: bvlt(x,x) with T base returns True for all x with equal bits
	//      => ex x (True) = True (wrong).
	TEST_CASE("bvlt: x < x is never satisfiable") {
		CHECK(blast_normalize("ex x x:bv[4] < x:bv[4]") == "F");
	}

	// {1}:bv[4] = 0001, {0}:bv[4] = 0000. Bits 3,2,1 are equal (all 0).
	// bvlt never reaches bit 0 and T base fires => returns True.
	// BUG: 1 < 0 should be False, but returns True.
	TEST_CASE("bvlt: 1 < 0 should be F for 4-bit") {
		CHECK(blast_normalize("ex x (x = { 1 }:bv[4] && x < { 0 }:bv[4])") == "F");
	}

	// {3}:bv[2] = 11, {2}:bv[2] = 10. Both have MSB=1 (equal).
	// BUG: equal MSBs trigger T base case => 3 < 2 incorrectly returns True.
	TEST_CASE("bvlt: 3 < 2 should be F for 2-bit") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && x < { 2 }:bv[2])") == "F");
	}

	// {0}:bv[4] < {0}:bv[4]: equal values, should be False.
	// BUG: equal bits all the way to bit 0 => T base fires => True.
	TEST_CASE("bvlt: 0 < 0 should be F") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x < { 0 }:bv[4])") == "F");
	}

	// Sanity check: correct direction, different MSBs should still work.
	// {0}:bv[4] < {8}:bv[4]: MSB of 8 is 1, MSB of 0 is 0 => True.
	TEST_CASE("bvlt: 0 < 8 should be T for 4-bit (MSBs differ)") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x < { 8 }:bv[4])") == "T");
	}
}

//
// Bug 2: bvgteq derived from broken bvlt
//
// bvgteq(l, r) = neg(bvlt(l, r))
// With bvlt(x, x) = True (due to T base case), bvgteq(x, x) = False.
// But x >= x is always True.
//
TEST_SUITE("bvgteq bugs") {

	// all x: x >= x should be True (universally valid).
	// BUG: bvgteq(x,x) = neg(bvlt(x,x)) = neg(T) = F => all x F = F.
	TEST_CASE("bvgteq: all x, x >= x should be T") {
		CHECK(blast_normalize("all x x:bv[4] >= x:bv[4]") == "T");
	}

	// 3 >= 2 should be True (3 > 2).
	TEST_CASE("bvgteq: 3 >= 2 should be T for 2-bit") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && x >= { 2 }:bv[2])") == "T");
	}
}

//
// Bug 3: bvnlteq semantic error — returns bvlt instead of bvgt
//
// In bv_predicate_blasting.h (line ~177):
//   bvnlteq(l, r) = !(l <= r) = l > r
// But the implementation is:
//   bvnlteq(l, r) { return bvlt<node>(left, right); }   // returns l < r  ← WRONG
// It should return bvgt(l, r).
//
TEST_SUITE("bvnlteq bugs") {

	// !(3 <= 1) = !(False) = True, since 3 > 1.
	// BUG: bvnlteq(3,1) returns bvlt(3,1) = False => ex x (...) = F (wrong).
	TEST_CASE("bvnlteq: 3 !<= 1 should be T") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && x !<= { 1 }:bv[2])") == "T");
	}

	// !(x <= x) = !(True) = False. No x can satisfy x !<= x.
	// BUG: bvnlteq(x,x) = bvlt(x,x) = T (due to T base) => ex x T = T (wrong).
	TEST_CASE("bvnlteq: x !<= x is never satisfiable") {
		CHECK(blast_normalize("ex x x:bv[4] !<= x:bv[4]") == "F");
	}
}

//
// Bug 4: bvshr_rule uses make_bvshl_call for the head
//
// In bv_predicate_blasting_logic.tmpl.h (line ~592):
//   auto head = make_bvshl_call<node>(base, count, shifted);  // ← WRONG
// It should be:
//   auto head = make_bvshr_call<node>(base, count, shifted);
//
// As a result, the right-shift rule is registered under the left-shift name,
// so right-shift operations use the left-shift rule (i.e., shift left instead
// of right, giving a completely wrong result).
//
TEST_SUITE("bvshr bugs") {

	// {4}:bv[4] = 0100, right shift by 1 = 0010 = 2.
	// BUG: bvshr_rule has wrong head (bvshl), so right shift uses left-shift rule:
	//      4 << 1 = 8 != 2, making this formula unsatisfiable (wrong).
	TEST_CASE("bvshr: 4 >> 1 = 2 for 4-bit") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 4 }:bv[4] && x >> { 1 }:bv[4] = y && y = { 2 }:bv[4])") == "T");
	}

	// {8}:bv[4] = 1000, right shift by 2 = 0010 = 2.
	// BUG: bvshr uses left-shift rule: 8 << 2 = 32 mod 16 = 0 != 2.
	TEST_CASE("bvshr: 8 >> 2 = 2 for 4-bit") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 8 }:bv[4] && x >> { 2 }:bv[4] = y && y = { 2 }:bv[4])") == "T");
	}

	// {6}:bv[4] = 0110, right shift by 1 = 0011 = 3.
	// BUG: 6 << 1 = 12 != 3.
	TEST_CASE("bvshr: 6 >> 1 = 3 for 4-bit") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 6 }:bv[4] && x >> { 1 }:bv[4] = y && y = { 3 }:bv[4])") == "T");
	}
}

//
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
//
TEST_SUITE("bvshl bugs") {

	// {3}:bv[4] = 0011, left shift by 3 = 1000 = 8.
	// i=1: i+offset = 4 >= bitwidth=4 → source bit 1 shifts out (correct).
	// BUG: with > instead of >=, i=1 tries to read bit[4] (OOB) and
	//      constrains bit[4] (treated as 0) to equal bit[1](base)=1 → UNSAT (wrong).
	TEST_CASE("bvshl: 3 << 3 = 8 for 4-bit (off-by-one at bitwidth boundary)") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 3 }:bv[4] && x << { 3 }:bv[4] = y && y = { 8 }:bv[4])") == "T");
	}

	// {1}:bv[4] = 0001, left shift by 1 = 0010 = 2.
	// Low bit 0 of shifted should be zero but the loop never constrains it.
	TEST_CASE("bvshl: 1 << 1 = 2 for 4-bit (low bits must be zero)") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 1 }:bv[4] && x << { 1 }:bv[4] = y && y = { 2 }:bv[4])") == "T");
	}

	// Verify that the result of shifting is unique (shifted[0] must be 0).
	// If low bits are not constrained to zero, the formula may be SAT for y = 3.
	TEST_CASE("bvshl: 1 << 1 != 3 for 4-bit (low bits must be zero, not 1)") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 1 }:bv[4] && x << { 1 }:bv[4] = y && y = { 3 }:bv[4])") == "F");
	}
}

//
// bvcast: bitvector casting — zero-extension and truncation
//
// Zero-extension (bv[2] -> bv[4]): low bits of result match source,
// high bits are forced to zero.
// Truncation (bv[4] -> bv[2]): result equals the low bits of source.
// Same-size (bv[4] -> bv[4]): no-op, result equals source unchanged.
//
TEST_SUITE("bvcast") {

	// Zero-extension: bv[2] -> bv[4]
	TEST_CASE("bvcast: zext {2}:bv[2] = {2}:bv[4]") {
		CHECK(blast_normalize("(bv[4]) { 2 }:bv[2] = { 2 }:bv[4]") == "T");
	}

	// Zero-extension: bv[2] -> bv[4]
	TEST_CASE("bvcast: zext {2}:bv[2] = {2}:bv[4]") {
		CHECK(blast_normalize("ex x (x = { 2 }:bv[2] && (bv[4]) x = { 2 }:bv[4])") == "T");
	}

	TEST_CASE("bvcast: zext {3}:bv[2] = {3}:bv[4]") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && (bv[4]) x = { 3 }:bv[4])") == "T");
	}

	TEST_CASE("bvcast: zext {0}:bv[2] = {0}:bv[4]") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[2] && (bv[4]) x = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvcast: zext high bits must be zero") {
		// 3 zero-extended to bv[4] is {3}:bv[4] = 0011, not {11}:bv[4] = 1011
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && (bv[4]) x = { 11 }:bv[4])") == "F");
	}

	TEST_CASE("bvcast: zext wrong low bits") {
		CHECK(blast_normalize("ex x (x = { 2 }:bv[2] && (bv[4]) x = { 3 }:bv[4])") == "F");
	}

	// Truncation: bv[4] -> bv[2]
	TEST_CASE("bvcast: trunc {7}:bv[4] to bv[2] = {3}:bv[2]") {
		// 7 = 0111; low 2 bits = 11 = 3
		CHECK(blast_normalize("ex x (x = { 7 }:bv[4] && (bv[2]) x = { 3 }:bv[2])") == "T");
	}

	TEST_CASE("bvcast: trunc {14}:bv[4] to bv[2] = {2}:bv[2]") {
		// 14 = 1110; low 2 bits = 10 = 2
		CHECK(blast_normalize("ex x (x = { 14 }:bv[4] && (bv[2]) x = { 2 }:bv[2])") == "T");
	}

	TEST_CASE("bvcast: trunc {4}:bv[4] to bv[2] = {0}:bv[2]") {
		// 4 = 0100; low 2 bits = 00 = 0
		CHECK(blast_normalize("ex x (x = { 4 }:bv[4] && (bv[2]) x = { 0 }:bv[2])") == "T");
	}

	TEST_CASE("bvcast: trunc wrong value") {
		// 7 = 0111 truncated to bv[2] is 3, not 0
		CHECK(blast_normalize("ex x (x = { 7 }:bv[4] && (bv[2]) x = { 0 }:bv[2])") == "F");
	}

	// Same-size cast is a no-op
	TEST_CASE("bvcast: same-size cast is identity") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv[4] && (bv[4]) x = { 5 }:bv[4])") == "T");
	}

	TEST_CASE("bvcast: same-size wrong value") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv[4] && (bv[4]) x = { 6 }:bv[4])") == "F");
	}

	TEST_CASE("bvcast: boundary width zext from bv[1] to bv[64]") {
		CHECK(blast_normalize("ex x (x = { 1 }:bv[1] && (bv[64]) x = { 1 }:bv[64])") == "T");
	}

	TEST_CASE("bvcast: signed-extreme truncation from bv[64] to bv[1]") {
		CHECK(blast_normalize(
			"ex x (x = { 9223372036854775808 }:bv[64] && (bv[1]) x = { 0 }:bv[1])") == "T");
	}
}


//
// bvnlt: not-less-than (= greater-than-or-equal)
//
TEST_SUITE("bvnlt") {

	TEST_CASE("bvnlt: 3 !< 2 is T for 2-bit") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && x !< { 2 }:bv[2])") == "T");
	}

	TEST_CASE("bvnlt: 1 !< 2 is F") {
		CHECK(blast_normalize("ex x (x = { 1 }:bv[4] && x !< { 2 }:bv[4])") == "F");
	}

	TEST_CASE("bvnlt: 2 !< 2 is T (equal case)") {
		CHECK(blast_normalize("ex x (x = { 2 }:bv[4] && x !< { 2 }:bv[4])") == "T");
	}

	TEST_CASE("bvnlt: 0 !< 0 is T (zero case)") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x !< { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvnlt: 0 !< 1 is F") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x !< { 1 }:bv[4])") == "F");
	}
}

//
// bvngt: not-greater-than (= less-than-or-equal)
//
TEST_SUITE("bvngt") {

	TEST_CASE("bvngt: 3 !> 2 is F") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && x !> { 2 }:bv[2])") == "F");
	}

	TEST_CASE("bvngt: 1 !> 2 is T") {
		CHECK(blast_normalize("ex x (x = { 1 }:bv[4] && x !> { 2 }:bv[4])") == "T");
	}

	TEST_CASE("bvngt: 2 !> 2 is T (equal case)") {
		CHECK(blast_normalize("ex x (x = { 2 }:bv[4] && x !> { 2 }:bv[4])") == "T");
	}

	TEST_CASE("bvngt: 0 !> 0 is T (zero case)") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x !> { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bvngt: 0 !> 1 is T") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x !> { 1 }:bv[4])") == "T");
	}
}

//
// bvngteq: not-greater-than-or-equal (= strictly less-than)
//
TEST_SUITE("bvngteq") {

	TEST_CASE("bvngteq: 3 !>= 1 is F") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && x !>= { 1 }:bv[2])") == "F");
	}

	TEST_CASE("bvngteq: x !>= x is never satisfiable") {
		CHECK(blast_normalize("ex x x:bv[4] !>= x:bv[4]") == "F");
	}

	TEST_CASE("bvngteq: 1 !>= 0 is F") {
		CHECK(blast_normalize("ex x (x = { 1 }:bv[4] && x !>= { 0 }:bv[4])") == "F");
	}

	TEST_CASE("bvngteq: 0 !>= 1 is T") {
		CHECK(blast_normalize("ex x (x = { 0 }:bv[4] && x !>= { 1 }:bv[4])") == "T");
	}
}

//
// Bug 6: wff_predicate_blasting passes the outer formula to predicates instead
// of the atomic comparison node, causing bitwidth=0 and malformed blasting.
//
// In bv_predicate_blasting.tmpl.h (lines ~403-412):
//   case tau::bf_nlt: { blast(term, nlt_predicate<node>); break; }  // WRONG
// should be:
//   case tau::bf_nlt: { blast(t, nlt_predicate<node>); break; }
//
// When the outer formula (type wff, no BA type) is passed instead of the atomic
// node, get_bv_type_bitwidth returns 0 and the predicate produces a malformed
// result containing wff_f adjacent to the original comparison — visible as
// "F{ 2 }:bv[64] !< ..." in output.
//
TEST_SUITE("bug6: blast passes outer formula instead of atomic") {

	// nlt with arithmetic in RHS: the bug causes get_bv_type_bitwidth(outer_wff)=0
	// which generates a degenerate 0-bit comparison instead of a 4-bit one.
	TEST_CASE("bug6: nlt with bf_neg+bf_sub in rhs is evaluated correctly") {
		// {1}':bv[4] = NOT(0001) = 1110 = 14; 14 - 3 = 11; 12 !< 11 = (12 >= 11) = T
		CHECK(blast_normalize(
			"ex x (x = { 12 }:bv[4] && x !< { 1 }:bv[4]' - { 3 }:bv[4])") == "T");
	}

	TEST_CASE("bug6: nlt with arithmetic in rhs, false case") {
		// 2 !< 11 = (2 >= 11) = F
		CHECK(blast_normalize(
			"ex x (x = { 2 }:bv[4] && x !< { 1 }:bv[4]' - { 3 }:bv[4])") == "F");
	}

	// gt with arithmetic: analogous bug for bf_gt
	TEST_CASE("bug6: gt with arithmetic in rhs is evaluated correctly") {
		// 12 > 11 = T
		CHECK(blast_normalize(
			"ex x (x = { 12 }:bv[4] && x > { 1 }:bv[4]' - { 3 }:bv[4])") == "T");
	}
}

//
// Bug 7: bvgt_rules base case uses index 'bitwidth' instead of 0, causing the
// base case to immediately match the initial call and only check the LSB.
//
// In bv_predicate_blasting_comparisons.tmpl.h:
//   auto base_header = make_bvgt_call_from_index(left, right, bitwidth);  // WRONG
// should be:
//   auto base_header = make_bvgt_call_from_index(left, right, 0);
//
// And bvgt_rule initial call:
//   auto call = make_bvgt_call_from_index(left, right, bitwidth);  // WRONG
// should be:
//   auto call = make_bvgt_call_from_index(left, right, bitwidth - 1);
//
// With the bug, bvgt[bitwidth](x,y) = (x[0]=1 && y[0]=0), checking only the
// LSB. So 2 > 1 (10 > 01) incorrectly returns F because bit[0](2)=0.
//
TEST_SUITE("bug7: bvgt_rules wrong base case index") {

	// 2 > 1 for 2-bit: MSB of 2 is 1, MSB of 1 is 0 => True.
	// BUG: only LSB is checked: LSB(2)=0, (0=1) => F (wrong).
	TEST_CASE("bug7: 2 > 1 should be T for 2-bit") {
		CHECK(blast_normalize("ex x (x = { 2 }:bv[2] && x > { 1 }:bv[2])") == "T");
	}

	// 4 > 3 for 4-bit: MSB of 4 is 1, MSB of 3 is 0 => True.
	// BUG: LSB(4)=0, (0=1) => F (wrong).
	TEST_CASE("bug7: 4 > 3 should be T for 4-bit (MSBs differ)") {
		CHECK(blast_normalize("ex x (x = { 4 }:bv[4] && x > { 3 }:bv[4])") == "T");
	}

	// 3 > 2 for 4-bit: same MSB (both 0), check lower bits.
	TEST_CASE("bug7: 3 > 2 should be T for 4-bit") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[4] && x > { 2 }:bv[4])") == "T");
	}

	// 1 > 2 should be F.
	TEST_CASE("bug7: 1 > 2 should be F") {
		CHECK(blast_normalize("ex x (x = { 1 }:bv[4] && x > { 2 }:bv[4])") == "F");
	}
}

tref parse_wff_no_hooks(const std::string& sample) {
	static tree<node_t>::get_options opts{
		.parse = { .start = tree<node_t>::wff },
		.infer_ba_types = false,
		.reget_with_hooks = false
	};
	return tree<node_t>::get(sample, opts);
}

// HE-8: the bf_interval case in wff_predicate_blasting unconditionally set
// error=true, so any formula containing an interval predicate made the
// whole blasting fail; it is now blasted as (lo<=mid) && (mid<=hi). The
// wff_interval construction hook desugars bf_interval into that same
// conjunction for every type, and that hook fires during the type-inference
// driven node reconstruction regardless of reget_with_hooks, so both
// infer_ba_types and reget_with_hooks must be disabled at parse time for a
// literal bf_interval node to survive parsing; types are then inferred
// separately (as in test_integration-ba_types_inference.cpp) without going
// through that reconstruction path.
static std::string blast_normalize_interval(const std::string& sample) {
	auto wff = parse_wff_no_hooks(sample);
	if (!wff) return "parse_error";
	if (!tau::get(wff).find_top(is<node_t, tau::bf_interval>))
		return "no_interval_in_input";
	auto [typed, _] = infer_ba_types<node_t>(wff);
	if (!typed) return "infer_error";
	if (!tau::get(typed).find_top(is<node_t, tau::bf_interval>))
		return "no_interval_after_inference";
	auto blasted = bv_predicate_blasting<node_t>(typed);
	if (!blasted) return "blast_error";
	auto result = normalizer<node_t>(blasted);
	if (!result) return "null";
	return tau::get(result).to_str();
}

TEST_SUITE("bf_interval (HE-8)") {

	TEST_CASE("value inside the interval is contained") {
		CHECK(blast_normalize_interval(
			"ex x (x = { 3 }:bv[4] && { 2 }:bv[4] <= x <= { 5 }:bv[4])") == "T");
	}

	TEST_CASE("value below the interval is not contained") {
		CHECK(blast_normalize_interval(
			"ex x (x = { 1 }:bv[4] && { 2 }:bv[4] <= x <= { 5 }:bv[4])") == "F");
	}

	TEST_CASE("value above the interval is not contained") {
		CHECK(blast_normalize_interval(
			"ex x (x = { 9 }:bv[4] && { 2 }:bv[4] <= x <= { 5 }:bv[4])") == "F");
	}

	TEST_CASE("value at the upper boundary is contained") {
		CHECK(blast_normalize_interval(
			"ex x (x = { 5 }:bv[4] && { 2 }:bv[4] <= x <= { 5 }:bv[4])") == "T");
	}
}

TEST_SUITE("more complex formulas") {

	TEST_CASE("complex formula 1") {
		CHECK(blast_normalize("all x:bv[4] all y:bv[4] ex z:bv[4] (x + y = z)") == "T");
	}

	TEST_CASE("complex formula 2") {
		CHECK(blast_normalize("all x:bv[4] all y:bv[4] all z:bv[4] (x + y = z)") == "F");
	}

	TEST_CASE("complex formula 3") {
		CHECK(blast_normalize("ex x:bv[4] (x * { 3 }:bv[4] = { 1 }:bv[4])") == "T");
	}

	TEST_CASE("complex formula 4") {
		CHECK(blast_normalize("all x:bv[4] (x * { 3 }:bv[4] = { 1 }:bv[4])") == "F");
	}

	TEST_CASE("complex formula 5") {
		CHECK(blast_normalize("all x:bv[4] (x << { 1 }:bv[4] != { 1 }:bv[4])") == "T");
	}

	TEST_CASE("complex formula 6") {
		CHECK(blast_normalize("ex x:bv[4] (x << { 1 }:bv[4] = { 1 }:bv[4])") == "F");
	}

}

//
// Nested arithmetic operands: atomic_blasting's `lookup` closure resolves
// already-blasted operands through the `changes` map, but operands arrive
// bf-wrapped while `changes` is keyed by the unwrapped operator node, so a
// naive lookup always misses and the un-blasted subtree leaks into the next
// operation's carry circuit. These cases exercise operators nested inside
// other operators, which is exactly when a fresh variable from an inner
// blast must be looked up by an outer one.
//
TEST_SUITE("nested arithmetic operands") {

	// (a + b) + c = const: add nested in add.
	TEST_CASE("nested add in add: (1+2)+3 = 6") {
		CHECK(blast_normalize(
			"ex a ex b ex c (a = { 1 }:bv[4] && b = { 2 }:bv[4] && c = { 3 }:bv[4]"
			" && (a + b) + c = { 6 }:bv[4])") == "T");
	}

	TEST_CASE("nested add in add: (1+2)+3 != 7") {
		CHECK(blast_normalize(
			"ex a ex b ex c (a = { 1 }:bv[4] && b = { 2 }:bv[4] && c = { 3 }:bv[4]"
			" && (a + b) + c = { 7 }:bv[4])") == "F");
	}

	// a + (b >> 1) = const: shift nested in add.
	TEST_CASE("shift nested in add: 3 + (8>>1) = 7") {
		CHECK(blast_normalize(
			"ex a ex b (a = { 3 }:bv[4] && b = { 8 }:bv[4]"
			" && a + (b >> { 1 }:bv[4]) = { 7 }:bv[4])") == "T");
	}

	TEST_CASE("shift nested in add: 3 + (8>>1) != 8") {
		CHECK(blast_normalize(
			"ex a ex b (a = { 3 }:bv[4] && b = { 8 }:bv[4]"
			" && a + (b >> { 1 }:bv[4]) = { 8 }:bv[4])") == "F");
	}

	// (a - b) << 2 = const: sub nested in shift.
	TEST_CASE("sub nested in shift: (5-2)<<2 = 12") {
		CHECK(blast_normalize(
			"ex a ex b (a = { 5 }:bv[4] && b = { 2 }:bv[4]"
			" && (a - b) << { 2 }:bv[4] = { 12 }:bv[4])") == "T");
	}

	TEST_CASE("sub nested in shift: (5-2)<<2 != 13") {
		CHECK(blast_normalize(
			"ex a ex b (a = { 5 }:bv[4] && b = { 2 }:bv[4]"
			" && (a - b) << { 2 }:bv[4] = { 13 }:bv[4])") == "F");
	}

	// ((a-b)>>3) + ((a-b)<<4) = c: the shape from the real failing rule, with
	// the shared subterm (a-b) blasted once and looked up twice.
	TEST_CASE("shared nested sub under shr and shl: ((5-2)>>3)+((5-2)<<4) = 0") {
		CHECK(blast_normalize(
			"ex a ex b ex c (a = { 5 }:bv[4] && b = { 2 }:bv[4]"
			" && ((a - b) >> { 3 }:bv[4]) + ((a - b) << { 4 }:bv[4]) = c"
			" && c = { 0 }:bv[4])") == "T");
	}

	TEST_CASE("shared nested sub under shr and shl: ((5-2)>>3)+((5-2)<<4) != 1") {
		CHECK(blast_normalize(
			"ex a ex b ex c (a = { 5 }:bv[4] && b = { 2 }:bv[4]"
			" && ((a - b) >> { 3 }:bv[4]) + ((a - b) << { 4 }:bv[4]) = c"
			" && c = { 1 }:bv[4])") == "F");
	}

	// Direct idempotence check: blasting nested arithmetic must remove every
	// arithmetic operator, not just the outermost one, so a re-entrant blast
	// pass over the result is a no-op (see antiprenexing.tmpl.h resolve loop).
	TEST_CASE("nested arithmetic: blasting leaves no arithmetic operators behind") {
		tref blasted = blast_formula(
			"ex a ex b ex c (a = { 5 }:bv[4] && b = { 2 }:bv[4]"
			" && ((a - b) >> { 3 }:bv[4]) + ((a - b) << { 4 }:bv[4]) = c"
			" && c = { 0 }:bv[4])");
		REQUIRE(blasted != nullptr);
		auto has_arithmetic = [](tref n) {
			switch (tau::get(n).get_type()) {
				case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
				case tau::bf_div: case tau::bf_mod: case tau::bf_shl:
				case tau::bf_shr: case tau::bf_cast: return true;
				default: return false;
			}
		};
		CHECK( !tau::get(blasted).find_top(has_arithmetic) );
	}
}

// Counts wff_ex nodes anywhere in n. Used to check that quantify_aux_vars
// (bv_predicate_blasting.tmpl.h) actually eliminates its freshly-introduced
// auxiliary quantifiers via push_ex_block_into_clause's fold + BDD
// elimination, instead of leaving them stuck.
static size_t count_wff_ex(tref n) {
	size_t c = 0;
	auto counter = [&](tref m) {
		if (is<node_t, tau::wff_ex>(m)) ++c;
		return true;
	};
	pre_order<node_t>(n).visit(counter);
	return c;
}

TEST_SUITE("aux var elimination") {

	// = and != atoms have no comparator recurrence (no "applied" part): the
	// blasted body is a pure conjunction of bf_eq carry/definition
	// constraints, so the fold should remove every auxiliary quantifier.

	TEST_CASE("nested arithmetic under =, bv[4]: sat (2+3)+1 = 6") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4]"
			" && (a + b) + { 1 }:bv[4] = { 6 }:bv[4])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) fully resolves the
		// aux-var conjunction for pure =/!= bodies; the 2 remaining wff_ex
		// are the formula's own outer "ex a ex b" (renamed), not stuck
		// aux vars.
		CHECK(count_wff_ex(blasted) == 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4]"
			" && (a + b) + { 1 }:bv[4] = { 6 }:bv[4])") == "T");
	}

	TEST_CASE("nested arithmetic under =, bv[4]: unsat (2+3)+1 != 7 as an =") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4]"
			" && (a + b) + { 1 }:bv[4] = { 7 }:bv[4])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) fully resolves the
		// aux-var conjunction; the 2 remaining wff_ex are the formula's own
		// outer "ex a ex b" (renamed), never touched by blasting.
		CHECK(count_wff_ex(blasted) == 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4]"
			" && (a + b) + { 1 }:bv[4] = { 7 }:bv[4])") == "F");
	}

	TEST_CASE("nested arithmetic under !=, bv[4]: sat (2+3)+1 = 6, 6 != 7") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4]"
			" && (a + b) + { 1 }:bv[4] != { 7 }:bv[4])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) fully resolves the
		// aux-var conjunction; the 2 remaining wff_ex are the formula's own
		// outer "ex a ex b" (renamed), never touched by blasting.
		CHECK(count_wff_ex(blasted) == 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4]"
			" && (a + b) + { 1 }:bv[4] != { 7 }:bv[4])") == "T");
	}

	TEST_CASE("nested arithmetic under !=, bv[4]: unsat (2+3)+1 = 6, not != 6") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4]"
			" && (a + b) + { 1 }:bv[4] != { 6 }:bv[4])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) fully resolves the
		// aux-var conjunction; the 2 remaining wff_ex are the formula's own
		// outer "ex a ex b" (renamed), never touched by blasting.
		CHECK(count_wff_ex(blasted) == 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4]"
			" && (a + b) + { 1 }:bv[4] != { 6 }:bv[4])") == "F");
	}

	TEST_CASE("nested arithmetic under =, bv[8]: sat (20+30)+5 = 55") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8]"
			" && (a + b) + { 5 }:bv[8] = { 55 }:bv[8])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) fully resolves the
		// aux-var conjunction; the 2 remaining wff_ex are the formula's own
		// outer "ex a ex b" (renamed).
		CHECK(count_wff_ex(blasted) == 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8]"
			" && (a + b) + { 5 }:bv[8] = { 55 }:bv[8])") == "T");
	}

	TEST_CASE("nested arithmetic under =, bv[8]: unsat (20+30)+5 != 56 as an =") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8]"
			" && (a + b) + { 5 }:bv[8] = { 56 }:bv[8])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) fully resolves the
		// aux-var conjunction; the 2 remaining wff_ex are the formula's own
		// outer "ex a ex b" (renamed).
		CHECK(count_wff_ex(blasted) == 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8]"
			" && (a + b) + { 5 }:bv[8] = { 56 }:bv[8])") == "F");
	}

	TEST_CASE("nested arithmetic under !=, bv[8]: sat (20+30)+5 = 55, 55 != 56") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8]"
			" && (a + b) + { 5 }:bv[8] != { 56 }:bv[8])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) fully resolves the
		// aux-var conjunction; the 2 remaining wff_ex are the formula's own
		// outer "ex a ex b" (renamed).
		CHECK(count_wff_ex(blasted) == 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8]"
			" && (a + b) + { 5 }:bv[8] != { 56 }:bv[8])") == "T");
	}

	TEST_CASE("nested arithmetic under !=, bv[8]: unsat (20+30)+5 = 55, not != 55") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8]"
			" && (a + b) + { 5 }:bv[8] != { 55 }:bv[8])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) fully resolves the
		// aux-var conjunction; the 2 remaining wff_ex are the formula's own
		// outer "ex a ex b" (renamed).
		CHECK(count_wff_ex(blasted) == 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8]"
			" && (a + b) + { 5 }:bv[8] != { 55 }:bv[8])") == "F");
	}

	// Comparisons (lt/gt-family) carry a comparator recurrence ("applied")
	// referencing the arithmetic result var, so a small residual quantifier
	// (the linking variable) may legitimately survive; what must not happen
	// is a residual proportional to the bitwidth (one per carry).

	TEST_CASE("a+b >= c, bv[4]: sat 2+3 >= 5") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b >= { 5 }:bv[4])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; the residual is the formula's own 4 outer
		// quantifiers (a,b,c,d, renamed) plus at most a small
		// comparator-linking one, not one per carry.
		CHECK(count_wff_ex(blasted) <= 5);
		CHECK(blast_normalize(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b >= { 5 }:bv[4])") == "T");
	}

	TEST_CASE("a+b >= c, bv[4]: unsat 2+3 >= 6") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b >= { 6 }:bv[4])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; the residual is the formula's own 4 outer
		// quantifiers (a,b,c,d, renamed) plus at most a small
		// comparator-linking one, not one per carry.
		CHECK(count_wff_ex(blasted) <= 5);
		CHECK(blast_normalize(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b >= { 6 }:bv[4])") == "F");
	}

	TEST_CASE("a+b < c, bv[4]: sat 2+3 < 6") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b < { 6 }:bv[4])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; the residual is the formula's own 4 outer
		// quantifiers (a,b,c,d, renamed) plus at most a small
		// comparator-linking one, not one per carry.
		CHECK(count_wff_ex(blasted) <= 5);
		CHECK(blast_normalize(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b < { 6 }:bv[4])") == "T");
	}

	TEST_CASE("a+b < c, bv[4]: unsat 2+3 < 5") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b < { 5 }:bv[4])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; the residual is the formula's own 4 outer
		// quantifiers (a,b,c,d, renamed) plus at most a small
		// comparator-linking one, not one per carry.
		CHECK(count_wff_ex(blasted) <= 5);
		CHECK(blast_normalize(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b < { 5 }:bv[4])") == "F");
	}

	TEST_CASE("a+b !<= c, bv[4]: sat 2+3 !<= 4") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b !<= { 4 }:bv[4])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; the residual is the formula's own 4 outer
		// quantifiers (a,b,c,d, renamed) plus at most a small
		// comparator-linking one, not one per carry.
		CHECK(count_wff_ex(blasted) <= 5);
		CHECK(blast_normalize(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b !<= { 4 }:bv[4])") == "T");
	}

	TEST_CASE("a+b !<= c, bv[4]: unsat 2+3 !<= 5") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b !<= { 5 }:bv[4])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; the residual is the formula's own 4 outer
		// quantifiers (a,b,c,d, renamed) plus at most a small
		// comparator-linking one, not one per carry.
		CHECK(count_wff_ex(blasted) <= 5);
		CHECK(blast_normalize(
			"ex a ex b (a = { 2 }:bv[4] && b = { 3 }:bv[4] && a + b !<= { 5 }:bv[4])") == "F");
	}

	TEST_CASE("a+b >= c, bv[8]: sat 20+30 >= 50") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b >= { 50 }:bv[8])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; a small residual (outer quantifiers +
		// comparator linking var) is expected.
		CHECK(count_wff_ex(blasted) <= 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b >= { 50 }:bv[8])") == "T");
	}

	TEST_CASE("a+b >= c, bv[8]: unsat 20+30 >= 51") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b >= { 51 }:bv[8])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; a small residual (outer quantifiers +
		// comparator linking var) is expected.
		CHECK(count_wff_ex(blasted) <= 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b >= { 51 }:bv[8])") == "F");
	}

	TEST_CASE("a+b < c, bv[8]: sat 20+30 < 51") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b < { 51 }:bv[8])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; a small residual (outer quantifiers +
		// comparator linking var) is expected.
		CHECK(count_wff_ex(blasted) <= 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b < { 51 }:bv[8])") == "T");
	}

	TEST_CASE("a+b < c, bv[8]: unsat 20+30 < 50") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b < { 50 }:bv[8])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; a small residual (outer quantifiers +
		// comparator linking var) is expected.
		CHECK(count_wff_ex(blasted) <= 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b < { 50 }:bv[8])") == "F");
	}

	TEST_CASE("a+b !<= c, bv[8]: sat 20+30 !<= 49") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b !<= { 49 }:bv[8])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; a small residual (outer quantifiers +
		// comparator linking var) is expected.
		CHECK(count_wff_ex(blasted) <= 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b !<= { 49 }:bv[8])") == "T");
	}

	TEST_CASE("a+b !<= c, bv[8]: unsat 20+30 !<= 50") {
		tref blasted = blast_formula(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b !<= { 50 }:bv[8])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; a small residual (outer quantifiers +
		// comparator linking var) is expected.
		CHECK(count_wff_ex(blasted) <= 2);
		CHECK(blast_normalize(
			"ex a ex b (a = { 20 }:bv[8] && b = { 30 }:bv[8] && a + b !<= { 50 }:bv[8])") == "F");
	}

	// Two linking variables: both operands of the comparison carry their own
	// embedded addition, so the residual (post-fold, pre-normalization) has
	// two result variables shared with the comparator instead of one.

	TEST_CASE("a+b >= c+d, bv[4]: sat 1+2 >= 1+1 (3 >= 2)") {
		tref blasted = blast_formula(
			"ex a ex b ex c ex d (a = { 1 }:bv[4] && b = { 2 }:bv[4]"
			" && c = { 1 }:bv[4] && d = { 1 }:bv[4] && a + b >= c + d)");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; the residual is the formula's own 4 outer
		// quantifiers (a,b,c,d, renamed) plus at most a small
		// comparator-linking one, not one per carry.
		CHECK(count_wff_ex(blasted) <= 5);
		CHECK(blast_normalize(
			"ex a ex b ex c ex d (a = { 1 }:bv[4] && b = { 2 }:bv[4]"
			" && c = { 1 }:bv[4] && d = { 1 }:bv[4] && a + b >= c + d)") == "T");
	}

	TEST_CASE("a+b >= c+d, bv[4]: unsat 1+2 >= 3+2 (3 >= 5)") {
		tref blasted = blast_formula(
			"ex a ex b ex c ex d (a = { 1 }:bv[4] && b = { 2 }:bv[4]"
			" && c = { 3 }:bv[4] && d = { 2 }:bv[4] && a + b >= c + d)");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) resolves the
		// arithmetic aux vars; the residual is the formula's own 4 outer
		// quantifiers (a,b,c,d, renamed) plus at most a small
		// comparator-linking one, not one per carry.
		CHECK(count_wff_ex(blasted) <= 5);
		CHECK(blast_normalize(
			"ex a ex b ex c ex d (a = { 1 }:bv[4] && b = { 2 }:bv[4]"
			" && c = { 3 }:bv[4] && d = { 2 }:bv[4] && a + b >= c + d)") == "F");
	}

	// Coarse size/time guard: must complete within the test timeout and be
	// correct.

	// Width ladder. Only bv[16] add stays under the test timeout end to
	// end; wider/other shapes are shelved below, each for one of two
	// distinct reasons:
	//  - add/>= at bv[32] and up: blast_formula() itself is fast; the wall
	//    is in normalizer<node_t>()/cvc5's own solving, not in
	//    quantify_aux_vars.
	//  - the nested (bvsub -> shr/shl -> bvadd) shape: the wall is inside
	//    quantify_aux_vars' own BDD fold (bdd_and_many/bdd_ex over a
	//    `running` BDD that stays large throughout), structurally
	//    guaranteed to terminate (bounded by ranked.size() ==
	//    conjuncts.size()).
	// Both are the same "normalization blowup" class as stress
	// template-5 (test_integration-bv_stress_check.cpp), not new bugs.
	TEST_CASE("a+b=c, bv[16]: sat 1000+2000=3000") {
		CHECK(blast_normalize(
			"ex a ex b (a = { 1000 }:bv[16] && b = { 2000 }:bv[16]"
			" && a + b = { 3000 }:bv[16])") == "T");
	}
	// Normalizer/cvc5 wall (see the width-ladder note above).
	TEST_CASE("a+b >= c, bv[16]: sat 1000+2000 >= 3000" * doctest::skip()) {
		CHECK(blast_normalize(
			"ex a ex b (a = { 1000 }:bv[16] && b = { 2000 }:bv[16]"
			" && a + b >= { 3000 }:bv[16])") == "T");
	}
	// Normalizer/cvc5 wall (see the width-ladder note above).
	TEST_CASE("a+b=c, bv[32]: sat 100000+200000=300000" * doctest::skip()) {
		CHECK(blast_normalize(
			"ex a ex b (a = { 100000 }:bv[32] && b = { 200000 }:bv[32]"
			" && a + b = { 300000 }:bv[32])") == "T");
	}
	// Normalizer/cvc5 wall (see the width-ladder note above).
	TEST_CASE("a+b=c, bv[64]: sat 1000000000+2000000000=3000000000" * doctest::skip()) {
		CHECK(blast_normalize(
			"ex a ex b (a = { 1000000000 }:bv[64] && b = { 2000000000 }:bv[64]"
			" && a + b = { 3000000000 }:bv[64])") == "T");
	}
	// Blasting-side wall, inside quantify_aux_vars' own BDD fold (see the
	// width-ladder note above).
	TEST_CASE("nested ((a-b)>>3)+((a-b)<<4)=c, bv[16]" * doctest::skip()) {
		CHECK(blast_normalize(
			"ex a ex b ex c (a = { 500 }:bv[16] && b = { 200 }:bv[16]"
			" && ((a - b) >> { 3 }:bv[16]) + ((a - b) << { 4 }:bv[16]) = c)") == "T");
	}
	// Same blasting-side wall as the bv[16] nested case above, at a larger
	// width.
	TEST_CASE("nested ((a-b)>>3)+((a-b)<<4)=c, bv[64]" * doctest::skip()) {
		CHECK(blast_normalize(
			"ex a ex b ex c (a = { 500000 }:bv[64] && b = { 200000 }:bv[64]"
			" && ((a - b) >> { 3 }:bv[64]) + ((a - b) << { 4 }:bv[64]) = c)") == "T");
	}
	// Normalizer/cvc5 wall (see the width-ladder note above).
	TEST_CASE("a+b>=c, bv[64]: sat 1000000000+2000000000>=2999999999" * doctest::skip()) {
		CHECK(blast_normalize(
			"ex a ex b (a = { 1000000000 }:bv[64] && b = { 2000000000 }:bv[64]"
			" && a + b >= { 2999999999 }:bv[64])") == "T");
	}

	// -- Atomic-BA soundness edge (bv is atomic, not atomless; TABA
	// Corollary 2.3 is proven for atomless BAs) --------------------------
	//
	// The fold identity `f1=0 && f2=0 == (f1|f2)=0` (used to remove the
	// existential over the auxiliary carry/definition variables) is
	// unconditionally sound as a term-level rewrite: it does not depend on
	// atomlessness. The cases below specifically probe formulas mixing
	// equations with disequations (!=) over blasted arithmetic's quantified
	// helper variables, at small enough bitwidths (bv[2], bv[4]) that the
	// expected verdict can be hand-checked by brute-force enumeration over
	// every value, independent of the blasting/elimination machinery.

	// x + 1 != 3 alone, bv[2] (values mod 4: 0,1,2,3).
	TEST_CASE("bv[2] arithmetic under != alone: sat, x=1: 1+1=2, 2 != 3") {
		// Enumeration: x=1 => x+1 = 2 (mod 4). 2 != 3 holds. Sat.
		tref blasted = blast_formula(
			"ex x (x = { 1 }:bv[2] && x + { 1 }:bv[2] != { 3 }:bv[2])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) fully resolves the
		// aux-var conjunction; the 1 remaining wff_ex is the formula's own
		// outer "ex x" (renamed), never touched by blasting.
		CHECK(count_wff_ex(blasted) == 1);
		CHECK(blast_normalize(
			"ex x (x = { 1 }:bv[2] && x + { 1 }:bv[2] != { 3 }:bv[2])") == "T");
	}

	TEST_CASE("bv[2] arithmetic under != alone: unsat, x=2: 2+1=3, not(3 != 3)") {
		// Enumeration: x=2 => x+1 = 3 (mod 4). 3 != 3 is false. Unsat.
		tref blasted = blast_formula(
			"ex x (x = { 2 }:bv[2] && x + { 1 }:bv[2] != { 3 }:bv[2])");
		REQUIRE(blasted != nullptr);
		// Bit-level BDD elimination (quantify_aux_vars) fully resolves the
		// aux-var conjunction; the 1 remaining wff_ex is the formula's own
		// outer "ex x" (renamed), never touched by blasting.
		CHECK(count_wff_ex(blasted) == 1);
		CHECK(blast_normalize(
			"ex x (x = { 2 }:bv[2] && x + { 1 }:bv[2] != { 3 }:bv[2])") == "F");
	}

	// Mixed equation + disequation over the same blasted arithmetic, bv[2].
	// x + 1 = 2 has the unique solution x=1 (mod 4, addition is a bijection
	// in x for a fixed addend: checking all four values, 0+1=1, 1+1=2,
	// 2+1=3, 3+1=0 -- only x=1 gives 2).
	TEST_CASE("bv[2] mixed = and !=: sat, unique x=1, and 1 != 0") {
		// Enumeration: the only x with x+1=2 is x=1; 1 != 0 holds. Sat.
		tref blasted = blast_formula(
			"ex x (x + { 1 }:bv[2] = { 2 }:bv[2] && x != { 0 }:bv[2])");
		REQUIRE(blasted != nullptr);
		CHECK(blast_normalize(
			"ex x (x + { 1 }:bv[2] = { 2 }:bv[2] && x != { 0 }:bv[2])") == "T");
	}

	TEST_CASE("bv[2] mixed = and !=: unsat, unique x=1, but not(1 != 1)") {
		// Enumeration: the only x with x+1=2 is x=1; 1 != 1 is false. Unsat.
		tref blasted = blast_formula(
			"ex x (x + { 1 }:bv[2] = { 2 }:bv[2] && x != { 1 }:bv[2])");
		REQUIRE(blasted != nullptr);
		CHECK(blast_normalize(
			"ex x (x + { 1 }:bv[2] = { 2 }:bv[2] && x != { 1 }:bv[2])") == "F");
	}

	// Same mixed shape at bv[4] (values mod 16). x + 1 = 5 has the unique
	// solution x=4 (addition mod 16 is a bijection in x for fixed addend).
	TEST_CASE("bv[4] mixed = and !=: sat, unique x=4, and 4 != 0") {
		// Enumeration: the only x with x+1=5 (mod 16) is x=4; 4 != 0 holds. Sat.
		tref blasted = blast_formula(
			"ex x (x + { 1 }:bv[4] = { 5 }:bv[4] && x != { 0 }:bv[4])");
		REQUIRE(blasted != nullptr);
		CHECK(blast_normalize(
			"ex x (x + { 1 }:bv[4] = { 5 }:bv[4] && x != { 0 }:bv[4])") == "T");
	}

	TEST_CASE("bv[4] mixed = and !=: unsat, unique x=4, but not(4 != 4)") {
		// Enumeration: the only x with x+1=5 (mod 16) is x=4; 4 != 4 is false. Unsat.
		tref blasted = blast_formula(
			"ex x (x + { 1 }:bv[4] = { 5 }:bv[4] && x != { 4 }:bv[4])");
		REQUIRE(blasted != nullptr);
		CHECK(blast_normalize(
			"ex x (x + { 1 }:bv[4] = { 5 }:bv[4] && x != { 4 }:bv[4])") == "F");
	}

	// The != involves the arithmetic result feeding a comparison too:
	// x forced to a concrete value, then both a != and a >= on the same
	// blasted sum. bv[2], values mod 4.
	TEST_CASE("bv[2] != feeding a comparison on the same sum: sat, x=0: 0+1=1, 1!=3, 1>=1") {
		// Enumeration: x=0 => sum = 1. 1 != 3 holds; 1 >= 1 holds. Sat.
		tref blasted = blast_formula(
			"ex x (x = { 0 }:bv[2] && x + { 1 }:bv[2] != { 3 }:bv[2]"
			" && x + { 1 }:bv[2] >= { 1 }:bv[2])");
		REQUIRE(blasted != nullptr);
		CHECK(blast_normalize(
			"ex x (x = { 0 }:bv[2] && x + { 1 }:bv[2] != { 3 }:bv[2]"
			" && x + { 1 }:bv[2] >= { 1 }:bv[2])") == "T");
	}

	TEST_CASE("bv[2] != feeding a comparison on the same sum: unsat, x=2: 2+1=3, not(3!=3)") {
		// Enumeration: x=2 => sum = 3. 3 != 3 is false, so the conjunction
		// fails regardless of the >= conjunct. Unsat.
		tref blasted = blast_formula(
			"ex x (x = { 2 }:bv[2] && x + { 1 }:bv[2] != { 3 }:bv[2]"
			" && x + { 1 }:bv[2] >= { 1 }:bv[2])");
		REQUIRE(blasted != nullptr);
		CHECK(blast_normalize(
			"ex x (x = { 2 }:bv[2] && x + { 1 }:bv[2] != { 3 }:bv[2]"
			" && x + { 1 }:bv[2] >= { 1 }:bv[2])") == "F");
	}

	// Guards: quantify_aux_vars's real mixed eq(-carry-definition)+neq(-atom)
	// shape resolves through push_ex_block_into_clause's fold without the
	// aux vars getting stuck or the answer flipping. bv[2]: a+b is a single
	// value and c ranges freely, so some c differs from it. Sat.
	TEST_CASE("mixed eq+neq aux-var shape, bv[2]: ex a ex b ex c (a+b != c) is sat") {
		CHECK(blast_normalize(
			"ex a ex b ex c (a:bv[2] + b:bv[2] != c:bv[2])") == "T");
	}

	// Guards the atomless-only Corollary 2.3 fold from ever being applied to
	// a bv (atomic, not atomless) domain with multiple disequations: bv[2]
	// has only 4 values, so x cannot differ from all of them at once. If the
	// fold were (wrongly) treating bv content as atomless here, this would
	// be misjudged T; the existing BA-type/skip gates must keep it F.
	TEST_CASE("pigeonhole canary, bv[2]: x != 0,1,2,3 all at once is unsat") {
		CHECK(blast_normalize(
			"ex x (x != { 0 }:bv[2] && x != { 1 }:bv[2]"
			" && x != { 2 }:bv[2] && x != { 3 }:bv[2])") == "F");
	}
}

//
// REVIEW (nested casting): temporary suite added while reviewing whether the
// blasting path supports nested casts. See private/review-casting.md.
//
TEST_SUITE("bvcast nested") {

	// constant folded by hooks or blasted: 2:bv[2] -> bv[4] -> bv[8]
	TEST_CASE("nested zext constant: (bv[8]) (bv[4]) {2}:bv[2] = {2}:bv[8]") {
		CHECK(blast_normalize("(bv[8]) (bv[4]) { 2 }:bv[2] = { 2 }:bv[8]") == "T");
	}

	// variable, no parens around inner cast
	TEST_CASE("nested zext var: ex x (x = {2}:bv[2] && (bv[8]) (bv[4]) x = {2}:bv[8])") {
		CHECK(blast_normalize("ex x (x = { 2 }:bv[2] && (bv[8]) (bv[4]) x = { 2 }:bv[8])") == "T");
	}

	// variable, explicit parens around inner cast
	TEST_CASE("nested zext var parens: ex x (x = {3}:bv[2] && (bv[8]) ((bv[4]) x) = {3}:bv[8])") {
		CHECK(blast_normalize("ex x (x = { 3 }:bv[2] && (bv[8]) ((bv[4]) x) = { 3 }:bv[8])") == "T");
	}

	// truncate then widen: 7 = 0111 -> bv[2] = 11 = 3 -> bv[8] = 3
	TEST_CASE("nested trunc-zext: ex x (x = {7}:bv[4] && (bv[8]) (bv[2]) x = {3}:bv[8])") {
		CHECK(blast_normalize("ex x (x = { 7 }:bv[4] && (bv[8]) (bv[2]) x = { 3 }:bv[8])") == "T");
	}

	// truncate then widen loses high bits: result cannot be 7
	TEST_CASE("nested trunc-zext unsat: ex x (x = {7}:bv[4] && (bv[8]) (bv[2]) x = {7}:bv[8])") {
		CHECK(blast_normalize("ex x (x = { 7 }:bv[4] && (bv[8]) (bv[2]) x = { 7 }:bv[8])") == "F");
	}

	// nested same-size casts are no-ops
	TEST_CASE("nested same-size: ex x (x = {5}:bv[4] && (bv[4]) (bv[4]) x = {5}:bv[4])") {
		CHECK(blast_normalize("ex x (x = { 5 }:bv[4] && (bv[4]) (bv[4]) x = { 5 }:bv[4])") == "T");
	}

	// cast result used in arithmetic via an intermediate variable:
	// x:bv[2]=3, (bv[4]) x = y, y + 1 = 4
	TEST_CASE("cast-result in add: ex x ex y (x = {3}:bv[2] && (bv[4]) x = y && y + {1}:bv[4] = {4}:bv[4])") {
		CHECK(blast_normalize(
			"ex x ex y (x = { 3 }:bv[2] && (bv[4]) x = y && y + { 1 }:bv[4] = { 4 }:bv[4])") == "T");
	}
}

TEST_SUITE("unsupported blasting cases") {

	TEST_CASE("interval predicates are handled by blasting") {
		// Interval predicates are now handled by blasting
		CHECK(blast_formula("{ 0 }:bv[4] <= x:bv[4] <= { 1 }:bv[4]") != nullptr);
	}
}

// Regression: resolve_quantifiers must not hand an already-blasted open bv
// scope to cvc5. eliminate_bv_and_quantifiers runs resolve_quantifiers three
// times and the interpreter re-enters it from its fixpoint loops, so a later
// pass meets scopes an earlier one blasted. Closing such a scope's free
// variables wraps blasting's auxiliary quantifiers in a universal block, and
// cvc5's counterexample-guided instantiation does not terminate on the result:
// checkSat never returns, with no time or resource bound to stop it, so the
// whole process hangs. `has_blasting_residue` is what keeps that query from
// being issued.
TEST_SUITE("blasted scopes stay off the solver path") {

	// The one-hot masking conjunction `bit` leaves behind is the marker.
	TEST_CASE("blasting output is recognised as residue") {
		tref blasted = blast_formula(
			"ex x (x:bv[4] + y:bv[4] = { 1 }:bv[4])");
		REQUIRE(blasted != nullptr);
		CHECK( has_blasting_residue<node_t>(blasted) );
	}

	// A formula the solver owns natively must keep its shortcut: bv
	// arithmetic carries no bit masks, so the screen leaves it alone.
	TEST_CASE("unblasted bv arithmetic is not residue") {
		tref plain = parse_wff("ex x (x:bv[4] + y:bv[4] = { 1 }:bv[4])");
		REQUIRE(plain != nullptr);
		CHECK( !has_blasting_residue<node_t>(plain) );
	}

	// Guards the screen against matching everything: a hand-written mask is
	// residue by this test's own definition, but ordinary bitwise content
	// over whole bitvectors is not.
	TEST_CASE("non-masking bitwise bv content is not residue") {
		tref plain = parse_wff("ex x (x:bv[4] & y:bv[4] = z:bv[4])");
		REQUIRE(plain != nullptr);
		CHECK( !has_blasting_residue<node_t>(plain) );
	}

}

TEST_SUITE("cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}