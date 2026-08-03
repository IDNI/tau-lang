// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-blasting_correctness_check.helper.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}

	TEST_CASE("logging") {
		logging::trace();
	}
}

// ---------------------------------------------------------------------------
// blasting-32bit-4var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_32bit_4var") {

	TEST_CASE("add_commutativity") {
		const char* f = "all a:bv[32], b:bv[32], c:bv[32], d:bv[32] (a + b) + (c + d) = (d + c) + (b + a)";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[32], b:bv[32], c:bv[32], d:bv[32] a < b && b < c && c < d && d - a >= { 3 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("distributivity") {
		const char* f = "all a:bv[32], b:bv[32], c:bv[32], d:bv[32] a & (b | c) | d = (a & b) | (a & c) | d";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[32], b:bv[32], c:bv[32], d:bv[32] a > { 0 }:bv[32] && b = a * { 3 }:bv[32] && c = b + a && d = c >> { 2 }:bv[32] && d > { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_assoc") {
		const char* f = "all a:bv[32], b:bv[32], c:bv[32], d:bv[32] (a ^ b) ^ c = a ^ (b ^ c) && a | d = d | a";
		check_blasting_correctness(f);
	}

	TEST_CASE("mod_existence") {
		const char* f = "ex a:bv[32], b:bv[32], c:bv[32], d:bv[32] a % { 5 }:bv[32] = { 2 }:bv[32] && b = a + { 3 }:bv[32] && c = a | b && d = c & a && d !>= b";
		check_blasting_correctness(f);
	}

	TEST_CASE("or_and_order") {
		const char* f = "all a:bv[32], b:bv[32], c:bv[32], d:bv[32] a | b >= a && a | b >= b && c & d <= c && c & d <= d";
		check_blasting_correctness(f);
	}

	TEST_CASE("shift_existence") {
		const char* f = "ex a:bv[32], b:bv[32], c:bv[32], d:bv[32] a > { 0 }:bv[32] && b = a << { 1 }:bv[32] && b > a && c = b << { 1 }:bv[32] && d = c >> { 1 }:bv[32] && d = b";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_negation") {
		const char* f = "all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] a + b = { 0 }:bv[32] && c + d = { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_xor_dep") {
		const char* f = "all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] b = a ^ { 4294967295 }:bv[32] && d = c ^ b";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_shift_bound") {
		const char* f = "all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] b = a >> { 1 }:bv[32] && b <= a && d = c << { 1 }:bv[32]";
		check_blasting_correctness(f);
	}

}

// ---------------------------------------------------------------------------
// blasting-32bit-8var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_32bit_8var") {

	TEST_CASE("add_or_commutativity") {
		const char* f = "all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] a + b + c + d = d + c + b + a && e | f | g | h = h | g | f | e";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h - a >= { 7 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("and_commutativity") {
		const char* f = "all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] (a & b) | (c & d) | (e & f) | (g & h) = (b & a) | (d & c) | (f & e) | (h & g)";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a > { 0 }:bv[32] && b = a * { 2 }:bv[32] && b > a && c = b + a && d = c >> { 1 }:bv[32] && e = d + a && f = e % { 7 }:bv[32] && g = f + a && h = g & a && h > { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_xnor_nand_commutativity") {
		const char* f = "all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] a ^ b ^ c ^ d = d ^ c ^ b ^ a && e !^ f = f !^ e && g !& h = h !& g";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_and_sat") {
		const char* f = "ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a < b && b < c && c < d && e = a + d && f = b + c && g = e ^ f && h = g & { 255 }:bv[32] && h > { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("add_commutativity_nested") {
		const char* f = "all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] ((a & b) | (c ^ d)) + ((e | f) & (g !^ h)) = ((e | f) & (g !^ h)) + ((a & b) | (c ^ d))";
		check_blasting_correctness(f);
	}

	// TEST_CASE("shift_mul_div_sat") {
	// 	const char* f = "ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a > { 0 }:bv[32] && b = a << { 1 }:bv[32] && b > a && c = b << { 1 }:bv[32] && d = c >> { 1 }:bv[32] && d = b && e = a * { 3 }:bv[32] && f = e / { 3 }:bv[32] && f = a && g = a | b && h = g & c && h > { 0 }:bv[32]";
	// 	check_blasting_correctness(f);
	// }

	TEST_CASE("alt_quant_negation") {
		const char* f = "all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] all e:bv[32] ex f:bv[32] all g:bv[32] ex h:bv[32] a + b = { 0 }:bv[32] && c + d = { 0 }:bv[32] && e + f = { 0 }:bv[32] && g + h = { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_mixed") {
		const char* f = "all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] all e:bv[32] ex f:bv[32] all g:bv[32] ex h:bv[32] b = a ^ { 4294967295 }:bv[32] && d = c + { 1 }:bv[32] && f = e >> { 1 }:bv[32] && f <= e && h = g | { 1 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_cross_dep") {
		const char* f = "all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] all e:bv[32] ex f:bv[32] all g:bv[32] ex h:bv[32] b = a + { 1 }:bv[32] && d = b + c && f = d ^ e && h = f & g";
		check_blasting_correctness(f);
	}
}

// ---------------------------------------------------------------------------
// blasting-8bit-8var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_8bit_8var") {

	TEST_CASE("add_or_commutativity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] a + b + c + d = d + c + b + a && e | f | g | h = h | g | f | e";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h - a >= { 7 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("and_commutativity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] (a & b) | (c & d) | (e & f) | (g & h) = (b & a) | (d & c) | (f & e) | (h & g)";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a > { 0 }:bv[8] && b = a * { 2 }:bv[8] && b > a && c = b + a && d = c >> { 1 }:bv[8] && e = d + a && f = e % { 7 }:bv[8] && g = f + a && h = g & a && h > { 0 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_xnor_nand_commutativity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] a ^ b ^ c ^ d = d ^ c ^ b ^ a && e !^ f = f !^ e && g !& h = h !& g";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_and_sat") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a < b && b < c && c < d && e = a + d && f = b + c && g = e ^ f && h = g & { 15 }:bv[8] && h > { 0 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("add_commutativity_nested") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] ((a & b) | (c ^ d)) + ((e | f) & (g !^ h)) = ((e | f) & (g !^ h)) + ((a & b) | (c ^ d))";
		check_blasting_correctness(f);
	}

	//TEST_CASE("shift_mul_div_sat") {
	//	const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a > { 0 }:bv[8] && b = a << { 1 }:bv[8] && b > a && c = b << { 1 }:bv[8] && d = c >> { 1 }:bv[8] && d = b && e = a * { 3 }:bv[8] && f = e / { 3 }:bv[8] && f = a && g = a | b && h = g & c && h > { 0 }:bv[8]";
	//	check_blasting_correctness(f);
	//}

	TEST_CASE("alt_quant_negation") {
		const char* f = "all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] all e:bv[8] ex f:bv[8] all g:bv[8] ex h:bv[8] a + b = { 0 }:bv[8] && c + d = { 0 }:bv[8] && e + f = { 0 }:bv[8] && g + h = { 0 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_mixed") {
		const char* f = "all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] all e:bv[8] ex f:bv[8] all g:bv[8] ex h:bv[8] b = a ^ { 255 }:bv[8] && d = c + { 1 }:bv[8] && f = e >> { 1 }:bv[8] && f <= e && h = g | { 1 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_cross_dep") {
		const char* f = "all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] all e:bv[8] ex f:bv[8] all g:bv[8] ex h:bv[8] b = a + { 1 }:bv[8] && d = b + c && f = d ^ e && h = f & g";
		check_blasting_correctness(f);
	}
}
