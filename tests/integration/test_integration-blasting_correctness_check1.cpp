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
// blasting-simple-arith (8-bit, 1 variable)
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_simple_arith") {

	TEST_CASE("mul_zero") {
		const char* f = "all a:bv[8] a * { 0 }:bv[8] = { 0 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("mul_identity") {
		const char* f = "all a:bv[8] a * { 1 }:bv[8] = a";
		check_blasting_correctness(f);
	}

	TEST_CASE("mul_sat") {
		const char* f = "ex a:bv[8] a * { 3 }:bv[8] = { 6 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("div_identity") {
		const char* f = "all a:bv[8] a / { 1 }:bv[8] = a";
		check_blasting_correctness(f);
	}

	TEST_CASE("div_sat") {
		const char* f = "ex a:bv[8] a / { 4 }:bv[8] = { 3 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("mod_zero") {
		const char* f = "all a:bv[8] a % { 1 }:bv[8] = { 0 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("mod_sat") {
		const char* f = "ex a:bv[8] a % { 7 }:bv[8] = { 3 }:bv[8]";
		check_blasting_correctness(f);
	}

}

// ---------------------------------------------------------------------------
// blasting-8bit-4var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_8bit_4var") {

	TEST_CASE("add_commutativity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] (a + b) + (c + d) = (d + c) + (b + a)";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a < b && b < c && c < d && d - a >= { 3 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("distributivity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] a & (b | c) | d = (a & b) | (a & c) | d";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a > { 0 }:bv[8] && b = a * { 3 }:bv[8] && c = b + a && d = c >> { 2 }:bv[8] && d > { 0 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_assoc") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] (a ^ b) ^ c = a ^ (b ^ c) && a | d = d | a";
		check_blasting_correctness(f);
	}

	TEST_CASE("mod_existence") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a % { 5 }:bv[8] = { 2 }:bv[8] && b = a + { 3 }:bv[8] && c = a | b && d = c & a && d !>= b";
		check_blasting_correctness(f);
	}

	TEST_CASE("or_and_order") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] a | b >= a && a | b >= b && c & d <= c && c & d <= d";
		check_blasting_correctness(f);
	}

	TEST_CASE("shift_existence") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a > { 0 }:bv[8] && b = a << { 1 }:bv[8] && b > a && c = b << { 1 }:bv[8] && d = c >> { 1 }:bv[8] && d = b";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_negation") {
		const char* f = "all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] a + b = { 0 }:bv[8] && c + d = { 0 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_xor_dep") {
		const char* f = "all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] b = a ^ { 255 }:bv[8] && d = c ^ b";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_shift_bound") {
		const char* f = "all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] b = a >> { 1 }:bv[8] && b <= a && d = c << { 1 }:bv[8]";
		check_blasting_correctness(f);
	}

}


// ---------------------------------------------------------------------------
// blasting-128bit-8var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_128bit_8var") {

	TEST_CASE("add_or_commutativity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] a + b + c + d = d + c + b + a && e | f | g | h = h | g | f | e";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h - a >= { 7 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("and_commutativity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] (a & b) | (c & d) | (e & f) | (g & h) = (b & a) | (d & c) | (f & e) | (h & g)";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a > { 0 }:bv[128] && b = a * { 2 }:bv[128] && b > a && c = b + a && d = c >> { 1 }:bv[128] && e = d + a && f = e % { 7 }:bv[128] && g = f + a && h = g & a && h > { 0 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_xnor_nand_commutativity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] a ^ b ^ c ^ d = d ^ c ^ b ^ a && e !^ f = f !^ e && g !& h = h !& g";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_and_sat") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a < b && b < c && c < d && e = a + d && f = b + c && g = e ^ f && h = g & { 255 }:bv[128] && h > { 0 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("add_commutativity_nested") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] ((a & b) | (c ^ d)) + ((e | f) & (g !^ h)) = ((e | f) & (g !^ h)) + ((a & b) | (c ^ d))";
		check_blasting_correctness(f);
	}

	// TEST_CASE("shift_mul_div_sat") {
	// 	const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a > { 0 }:bv[128] && b = a << { 1 }:bv[128] && b > a && c = b << { 1 }:bv[128] && d = c >> { 1 }:bv[128] && d = b && e = a * { 3 }:bv[128] && f = e / { 3 }:bv[128] && f = a && g = a | b && h = g & c && h > { 0 }:bv[128]";
	// 	check_blasting_correctness(f);
	// }

	TEST_CASE("alt_quant_negation") {
		const char* f = "all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] all e:bv[128] ex f:bv[128] all g:bv[128] ex h:bv[128] a + b = { 0 }:bv[128] && c + d = { 0 }:bv[128] && e + f = { 0 }:bv[128] && g + h = { 0 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_cross_dep") {
		const char* f = "all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] all e:bv[128] ex f:bv[128] all g:bv[128] ex h:bv[128] b = a + { 1 }:bv[128] && d = b + c && f = d ^ e && h = f & g";
		check_blasting_correctness(f);
	}

}
