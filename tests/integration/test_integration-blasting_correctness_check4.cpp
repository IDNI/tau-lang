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
// blasting-32bit-16var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_32bit_16var") {

	TEST_CASE("add_or_commutativity") {
		const char* f = "all a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a + b + c + d + e + f + g + h = h + g + f + e + d + c + b + a && i | j | k | l | m | o | p | q = q | p | o | m | l | k | j | i";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h < i && i < j && j < k && k < l && l < m && m < o && o < p && p < q && q - a >= { 15 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("and_commutativity") {
		const char* f = "all a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] (a & b) | (c & d) | (e & f) | (g & h) | (i & j) | (k & l) | (m & o) | (p & q) = (b & a) | (d & c) | (f & e) | (h & g) | (j & i) | (l & k) | (o & m) | (q & p)";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a > { 0 }:bv[32] && b = a * { 2 }:bv[32] && b > a && c = b + a && d = c >> { 1 }:bv[32] && e = d + a && f = e % { 7 }:bv[32] && g = f + a && h = g & a && h > { 0 }:bv[32] && i = a | b && j = i ^ c && k = j & d && l = k + e && m = l >> { 1 }:bv[32] && o = m | f && p = o ^ g && q = p & h && q > { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_xnor_nand_nor_commutativity") {
		const char* f = "all a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a ^ b ^ c ^ d ^ e ^ f ^ g ^ h = h ^ g ^ f ^ e ^ d ^ c ^ b ^ a && i !^ j = j !^ i && k !& l = l !& k && m !| o = o !| m && p | q >= p && p | q >= q";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_and_sat") {
		const char* f = "ex a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a < b && b < c && c < d && d < e && e < f && f < g && g < h && i = a + h && j = b + g && k = c + f && l = d + e && m = i ^ j && o = k ^ l && p = m ^ o && q = p + a && q > a";
		check_blasting_correctness(f);
	}

	TEST_CASE("or_and_order") {
		const char* f = "all a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a | b >= a && c | d >= c && e | f >= e && g | h >= g && i & j <= i && k & l <= k && m & o <= m && p & q <= p";
		check_blasting_correctness(f);
	}

	// TEST_CASE("shift_mul_div_sat") {
	//	const char* f = "ex a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a > { 0 }:bv[32] && b = a << { 1 }:bv[32] && b > a && c = b << { 1 }:bv[32] && d = c >> { 1 }:bv[32] && d = b && e = a * { 3 }:bv[32] && f = e / { 3 }:bv[32] && f = a && g = a | b && h = g & c && h > { 0 }:bv[32] && i = a + b && j = i % { 7 }:bv[32] && k = j + a && l = k & b && m = l | c && o = m ^ d && p = o + a && q = p >> { 1 }:bv[32] && q > { 0 }:bv[32]";
	//	check_blasting_correctness(f);
	// }

	TEST_CASE("alt_quant_negation") {
		const char* f = "all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] all e:bv[32] ex f:bv[32] all g:bv[32] ex h:bv[32] all i:bv[32] ex j:bv[32] all k:bv[32] ex l:bv[32] all m:bv[32] ex o:bv[32] all p:bv[32] ex q:bv[32] a + b = { 0 }:bv[32] && c + d = { 0 }:bv[32] && e + f = { 0 }:bv[32] && g + h = { 0 }:bv[32] && i + j = { 0 }:bv[32] && k + l = { 0 }:bv[32] && m + o = { 0 }:bv[32] && p + q = { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_cross_dep") {
		const char* f = "all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] all e:bv[32] ex f:bv[32] all g:bv[32] ex h:bv[32] all i:bv[32] ex j:bv[32] all k:bv[32] ex l:bv[32] all m:bv[32] ex o:bv[32] all p:bv[32] ex q:bv[32] b = a ^ { 4294967295 }:bv[32] && d = c + b && f = e & b && h = g | d && j = i ^ f && l = k + j && o = m & l && q = p | o";
		check_blasting_correctness(f);
	}

}

// ---------------------------------------------------------------------------
// blasting-128bit-4var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_128bit_4var") {

	TEST_CASE("add_commutativity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] (a + b) + (c + d) = (d + c) + (b + a)";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a < b && b < c && c < d && d - a >= { 3 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("distributivity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] a & (b | c) | d = (a & b) | (a & c) | d";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a > { 0 }:bv[128] && b = a * { 3 }:bv[128] && c = b + a && d = c >> { 2 }:bv[128] && d > { 0 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_assoc") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] (a ^ b) ^ c = a ^ (b ^ c) && a | d = d | a";
		check_blasting_correctness(f);
	}

	TEST_CASE("mod_existence") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a % { 5 }:bv[128] = { 2 }:bv[128] && b = a + { 3 }:bv[128] && c = a | b && d = c & a && d !>= b";
		check_blasting_correctness(f);
	}

	TEST_CASE("or_and_order") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] a | b >= a && a | b >= b && c & d <= c && c & d <= d";
		check_blasting_correctness(f);
	}

	TEST_CASE("shift_existence") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a > { 0 }:bv[128] && b = a << { 1 }:bv[128] && b > a && c = b << { 1 }:bv[128] && d = c >> { 1 }:bv[128] && d = b";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_negation") {
		const char* f = "all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] a + b = { 0 }:bv[128] && c + d = { 0 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_and_dep") {
		const char* f = "all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] b = a + { 1 }:bv[128] && d = b ^ c";
		check_blasting_correctness(f);
	}

	TEST_CASE("alt_quant_shift_bound") {
		const char* f = "all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] b = a >> { 1 }:bv[128] && b <= a && d = c << { 1 }:bv[128]";
		check_blasting_correctness(f);
	}

}
