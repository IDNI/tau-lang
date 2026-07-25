# Blasting on/off equivalence -- migrated from test_integration-blasting_correctness_check1..5.cpp

include(add_blasting_test)


# blasting_simple_arith -- test_integration-blasting_correctness_check1.cpp
add_blasting_equiv_test(blasting_equiv-simple_arith-mul_zero
	"all a:bv[8] a * { 0 }:bv[8] = { 0 }:bv[8]") # test_integration-blasting_correctness_check1.cpp:22
add_blasting_equiv_test(blasting_equiv-simple_arith-mul_identity
	"all a:bv[8] a * { 1 }:bv[8] = a") # test_integration-blasting_correctness_check1.cpp:27
add_blasting_equiv_test(blasting_equiv-simple_arith-mul_sat
	"ex a:bv[8] a * { 3 }:bv[8] = { 6 }:bv[8]") # test_integration-blasting_correctness_check1.cpp:32
add_blasting_equiv_test(blasting_equiv-simple_arith-div_identity
	"all a:bv[8] a / { 1 }:bv[8] = a") # test_integration-blasting_correctness_check1.cpp:37
add_blasting_equiv_test(blasting_equiv-simple_arith-div_sat
	"ex a:bv[8] a / { 4 }:bv[8] = { 3 }:bv[8]") # test_integration-blasting_correctness_check1.cpp:42
add_blasting_equiv_test(blasting_equiv-simple_arith-mod_zero
	"all a:bv[8] a % { 1 }:bv[8] = { 0 }:bv[8]") # test_integration-blasting_correctness_check1.cpp:47
add_blasting_equiv_test(blasting_equiv-simple_arith-mod_sat
	"ex a:bv[8] a % { 7 }:bv[8] = { 3 }:bv[8]") # test_integration-blasting_correctness_check1.cpp:52

# blasting_8bit_4var -- test_integration-blasting_correctness_check1.cpp
add_blasting_equiv_test(blasting_equiv-8bit_4var-add_commutativity
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] (a + b) + (c + d) = (d + c) + (b + a)") # test_integration-blasting_correctness_check1.cpp:65
add_blasting_equiv_test(blasting_equiv-8bit_4var-chain_ineq_gap
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a < b && b < c && c < d && d - a >= { 3 }:bv[8]") # test_integration-blasting_correctness_check1.cpp:70
add_blasting_equiv_test(blasting_equiv-8bit_4var-distributivity
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] a & (b | c) | d = (a & b) | (a & c) | d") # test_integration-blasting_correctness_check1.cpp:75
add_blasting_equiv_test(blasting_equiv-8bit_4var-arith_composition
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a > { 0 }:bv[8] && b = a * { 3 }:bv[8] && c = b + a && d = c >> { 2 }:bv[8] && d > { 0 }:bv[8]") # test_integration-blasting_correctness_check1.cpp:80
add_blasting_equiv_test(blasting_equiv-8bit_4var-xor_assoc
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] (a ^ b) ^ c = a ^ (b ^ c) && a | d = d | a") # test_integration-blasting_correctness_check1.cpp:85
add_blasting_equiv_test(blasting_equiv-8bit_4var-mod_existence
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a % { 5 }:bv[8] = { 2 }:bv[8] && b = a + { 3 }:bv[8] && c = a | b && d = c & a && d !>= b") # test_integration-blasting_correctness_check1.cpp:90
add_blasting_equiv_test(blasting_equiv-8bit_4var-or_and_order
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] a | b >= a && a | b >= b && c & d <= c && c & d <= d") # test_integration-blasting_correctness_check1.cpp:95
add_blasting_equiv_test(blasting_equiv-8bit_4var-shift_existence
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a > { 0 }:bv[8] && b = a << { 1 }:bv[8] && b > a && c = b << { 1 }:bv[8] && d = c >> { 1 }:bv[8] && d = b") # test_integration-blasting_correctness_check1.cpp:100
add_blasting_equiv_test(blasting_equiv-8bit_4var-alt_quant_negation
	"all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] a + b = { 0 }:bv[8] && c + d = { 0 }:bv[8]") # test_integration-blasting_correctness_check1.cpp:105
add_blasting_equiv_test(blasting_equiv-8bit_4var-alt_quant_xor_dep
	"all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] b = a ^ { 255 }:bv[8] && d = c ^ b") # test_integration-blasting_correctness_check1.cpp:110
add_blasting_equiv_test(blasting_equiv-8bit_4var-alt_quant_shift_bound
	"all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] b = a >> { 1 }:bv[8] && b <= a && d = c << { 1 }:bv[8]") # test_integration-blasting_correctness_check1.cpp:115

# blasting_128bit_8var -- test_integration-blasting_correctness_check1.cpp
add_blasting_equiv_test(blasting_equiv-128bit_8var-add_or_commutativity
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] a + b + c + d = d + c + b + a && e | f | g | h = h | g | f | e") # test_integration-blasting_correctness_check1.cpp:129
add_blasting_equiv_test(blasting_equiv-128bit_8var-chain_ineq_gap
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h - a >= { 7 }:bv[128]") # test_integration-blasting_correctness_check1.cpp:134
add_blasting_equiv_test(blasting_equiv-128bit_8var-and_commutativity
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] (a & b) | (c & d) | (e & f) | (g & h) = (b & a) | (d & c) | (f & e) | (h & g)") # test_integration-blasting_correctness_check1.cpp:139
add_blasting_equiv_test(blasting_equiv-128bit_8var-arith_composition
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a > { 0 }:bv[128] && b = a * { 2 }:bv[128] && b > a && c = b + a && d = c >> { 1 }:bv[128] && e = d + a && f = e % { 7 }:bv[128] && g = f + a && h = g & a && h > { 0 }:bv[128]") # test_integration-blasting_correctness_check1.cpp:144
add_blasting_equiv_test(blasting_equiv-128bit_8var-xor_xnor_nand_commutativity
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] a ^ b ^ c ^ d = d ^ c ^ b ^ a && e !^ f = f !^ e && g !& h = h !& g") # test_integration-blasting_correctness_check1.cpp:149
add_blasting_equiv_test(blasting_equiv-128bit_8var-xor_and_sat
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a < b && b < c && c < d && e = a + d && f = b + c && g = e ^ f && h = g & { 255 }:bv[128] && h > { 0 }:bv[128]") # test_integration-blasting_correctness_check1.cpp:154
add_blasting_equiv_test(blasting_equiv-128bit_8var-add_commutativity_nested
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] ((a & b) | (c ^ d)) + ((e | f) & (g !^ h)) = ((e | f) & (g !^ h)) + ((a & b) | (c ^ d))") # test_integration-blasting_correctness_check1.cpp:159
add_blasting_equiv_test(blasting_equiv-128bit_8var-shift_mul_div_sat
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a > { 0 }:bv[128] && b = a << { 1 }:bv[128] && b > a && c = b << { 1 }:bv[128] && d = c >> { 1 }:bv[128] && d = b && e = a * { 3 }:bv[128] && f = e / { 3 }:bv[128] && f = a && g = a | b && h = g & c && h > { 0 }:bv[128]") # test_integration-blasting_correctness_check1.cpp:164
add_blasting_equiv_test(blasting_equiv-128bit_8var-alt_quant_negation
	"all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] all e:bv[128] ex f:bv[128] all g:bv[128] ex h:bv[128] a + b = { 0 }:bv[128] && c + d = { 0 }:bv[128] && e + f = { 0 }:bv[128] && g + h = { 0 }:bv[128]") # test_integration-blasting_correctness_check1.cpp:169
add_blasting_equiv_test(blasting_equiv-128bit_8var-alt_quant_cross_dep
	"all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] all e:bv[128] ex f:bv[128] all g:bv[128] ex h:bv[128] b = a + { 1 }:bv[128] && d = b + c && f = d ^ e && h = f & g") # test_integration-blasting_correctness_check1.cpp:174

# blasting_8bit_16var -- test_integration-blasting_correctness_check2.cpp
add_blasting_equiv_test(blasting_equiv-8bit_16var-add_or_commutativity
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] all i:bv[8] all j:bv[8] all k:bv[8] all l:bv[8] all m:bv[8] all o:bv[8] all p:bv[8] all q:bv[8] a + b + c + d + e + f + g + h = h + g + f + e + d + c + b + a && i | j | k | l | m | o | p | q = q | p | o | m | l | k | j | i") # test_integration-blasting_correctness_check2.cpp:22
add_blasting_equiv_test(blasting_equiv-8bit_16var-chain_ineq_gap
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] ex i:bv[8] ex j:bv[8] ex k:bv[8] ex l:bv[8] ex m:bv[8] ex o:bv[8] ex p:bv[8] ex q:bv[8] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h < i && i < j && j < k && k < l && l < m && m < o && o < p && p < q && q - a >= { 15 }:bv[8]") # test_integration-blasting_correctness_check2.cpp:27
add_blasting_equiv_test(blasting_equiv-8bit_16var-and_commutativity
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] all i:bv[8] all j:bv[8] all k:bv[8] all l:bv[8] all m:bv[8] all o:bv[8] all p:bv[8] all q:bv[8] (a & b) | (c & d) | (e & f) | (g & h) | (i & j) | (k & l) | (m & o) | (p & q) = (b & a) | (d & c) | (f & e) | (h & g) | (j & i) | (l & k) | (o & m) | (q & p)") # test_integration-blasting_correctness_check2.cpp:32
add_blasting_equiv_test(blasting_equiv-8bit_16var-arith_composition
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] ex i:bv[8] ex j:bv[8] ex k:bv[8] ex l:bv[8] ex m:bv[8] ex o:bv[8] ex p:bv[8] ex q:bv[8] a > { 0 }:bv[8] && b = a * { 2 }:bv[8] && b > a && c = b + a && d = c >> { 1 }:bv[8] && e = d + a && f = e % { 7 }:bv[8] && g = f + a && h = g & a && h > { 0 }:bv[8] && i = a | b && j = i ^ c && k = j & d && l = k + e && m = l >> { 1 }:bv[8] && o = m | f && p = o ^ g && q = p & h && q > { 0 }:bv[8]") # test_integration-blasting_correctness_check2.cpp:37
add_blasting_equiv_test(blasting_equiv-8bit_16var-xor_xnor_nand_nor_commutativity
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] all i:bv[8] all j:bv[8] all k:bv[8] all l:bv[8] all m:bv[8] all o:bv[8] all p:bv[8] all q:bv[8] a ^ b ^ c ^ d ^ e ^ f ^ g ^ h = h ^ g ^ f ^ e ^ d ^ c ^ b ^ a && i !^ j = j !^ i && k !& l = l !& k && m !| o = o !| m && p | q >= p && p | q >= q") # test_integration-blasting_correctness_check2.cpp:42
add_blasting_equiv_test(blasting_equiv-8bit_16var-xor_and_sat
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] ex i:bv[8] ex j:bv[8] ex k:bv[8] ex l:bv[8] ex m:bv[8] ex o:bv[8] ex p:bv[8] ex q:bv[8] a < b && b < c && c < d && d < e && e < f && f < g && g < h && i = a + h && j = b + g && k = c + f && l = d + e && m = i ^ j && o = k ^ l && p = m ^ o && q = p + a && q > a") # test_integration-blasting_correctness_check2.cpp:47
add_blasting_equiv_test(blasting_equiv-8bit_16var-or_and_order
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] all i:bv[8] all j:bv[8] all k:bv[8] all l:bv[8] all m:bv[8] all o:bv[8] all p:bv[8] all q:bv[8] a | b >= a && c | d >= c && e | f >= e && g | h >= g && i & j <= i && k & l <= k && m & o <= m && p & q <= p") # test_integration-blasting_correctness_check2.cpp:52
add_blasting_equiv_test(blasting_equiv-8bit_16var-shift_mul_div_sat
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] ex i:bv[8] ex j:bv[8] ex k:bv[8] ex l:bv[8] ex m:bv[8] ex o:bv[8] ex p:bv[8] ex q:bv[8] a > { 0 }:bv[8] && b = a << { 1 }:bv[8] && b > a && c = b << { 1 }:bv[8] && d = c >> { 1 }:bv[8] && d = b && e = a * { 3 }:bv[8] && f = e / { 3 }:bv[8] && f = a && g = a | b && h = g & c && h > { 0 }:bv[8] && i = a + b && j = i % { 7 }:bv[8] && k = j + a && l = k & b && m = l | c && o = m ^ d && p = o + a && q = p >> { 1 }:bv[8] && q > { 0 }:bv[8]") # test_integration-blasting_correctness_check2.cpp:57
add_blasting_equiv_test(blasting_equiv-8bit_16var-alt_quant_negation
	"all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] all e:bv[8] ex f:bv[8] all g:bv[8] ex h:bv[8] all i:bv[8] ex j:bv[8] all k:bv[8] ex l:bv[8] all m:bv[8] ex o:bv[8] all p:bv[8] ex q:bv[8] a + b = { 0 }:bv[8] && c + d = { 0 }:bv[8] && e + f = { 0 }:bv[8] && g + h = { 0 }:bv[8] && i + j = { 0 }:bv[8] && k + l = { 0 }:bv[8] && m + o = { 0 }:bv[8] && p + q = { 0 }:bv[8]") # test_integration-blasting_correctness_check2.cpp:62
add_blasting_equiv_test(blasting_equiv-8bit_16var-alt_quant_cross_dep
	"all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] all e:bv[8] ex f:bv[8] all g:bv[8] ex h:bv[8] all i:bv[8] ex j:bv[8] all k:bv[8] ex l:bv[8] all m:bv[8] ex o:bv[8] all p:bv[8] ex q:bv[8] b = a ^ { 255 }:bv[8] && d = c + b && f = e & b && h = g | d && j = i ^ f && l = k + j && o = m & l && q = p | o") # test_integration-blasting_correctness_check2.cpp:67

# blasting_32bit_4var -- test_integration-blasting_correctness_check3.cpp
add_blasting_equiv_test(blasting_equiv-32bit_4var-add_commutativity
	"all a:bv[32], b:bv[32], c:bv[32], d:bv[32] (a + b) + (c + d) = (d + c) + (b + a)") # test_integration-blasting_correctness_check3.cpp:22
add_blasting_equiv_test(blasting_equiv-32bit_4var-chain_ineq_gap
	"ex a:bv[32], b:bv[32], c:bv[32], d:bv[32] a < b && b < c && c < d && d - a >= { 3 }:bv[32]") # test_integration-blasting_correctness_check3.cpp:27
add_blasting_equiv_test(blasting_equiv-32bit_4var-distributivity
	"all a:bv[32], b:bv[32], c:bv[32], d:bv[32] a & (b | c) | d = (a & b) | (a & c) | d") # test_integration-blasting_correctness_check3.cpp:32
add_blasting_equiv_test(blasting_equiv-32bit_4var-arith_composition
	"ex a:bv[32], b:bv[32], c:bv[32], d:bv[32] a > { 0 }:bv[32] && b = a * { 3 }:bv[32] && c = b + a && d = c >> { 2 }:bv[32] && d > { 0 }:bv[32]") # test_integration-blasting_correctness_check3.cpp:37
add_blasting_equiv_test(blasting_equiv-32bit_4var-xor_assoc
	"all a:bv[32], b:bv[32], c:bv[32], d:bv[32] (a ^ b) ^ c = a ^ (b ^ c) && a | d = d | a") # test_integration-blasting_correctness_check3.cpp:42
add_blasting_equiv_test(blasting_equiv-32bit_4var-mod_existence
	"ex a:bv[32], b:bv[32], c:bv[32], d:bv[32] a % { 5 }:bv[32] = { 2 }:bv[32] && b = a + { 3 }:bv[32] && c = a | b && d = c & a && d !>= b") # test_integration-blasting_correctness_check3.cpp:47
add_blasting_equiv_test(blasting_equiv-32bit_4var-or_and_order
	"all a:bv[32], b:bv[32], c:bv[32], d:bv[32] a | b >= a && a | b >= b && c & d <= c && c & d <= d") # test_integration-blasting_correctness_check3.cpp:52
add_blasting_equiv_test(blasting_equiv-32bit_4var-shift_existence
	"ex a:bv[32], b:bv[32], c:bv[32], d:bv[32] a > { 0 }:bv[32] && b = a << { 1 }:bv[32] && b > a && c = b << { 1 }:bv[32] && d = c >> { 1 }:bv[32] && d = b") # test_integration-blasting_correctness_check3.cpp:57
add_blasting_equiv_test(blasting_equiv-32bit_4var-alt_quant_negation
	"all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] a + b = { 0 }:bv[32] && c + d = { 0 }:bv[32]") # test_integration-blasting_correctness_check3.cpp:62
add_blasting_equiv_test(blasting_equiv-32bit_4var-alt_quant_xor_dep
	"all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] b = a ^ { 4294967295 }:bv[32] && d = c ^ b") # test_integration-blasting_correctness_check3.cpp:67
add_blasting_equiv_test(blasting_equiv-32bit_4var-alt_quant_shift_bound
	"all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] b = a >> { 1 }:bv[32] && b <= a && d = c << { 1 }:bv[32]") # test_integration-blasting_correctness_check3.cpp:72

# blasting_32bit_8var -- test_integration-blasting_correctness_check3.cpp
add_blasting_equiv_test(blasting_equiv-32bit_8var-add_or_commutativity
	"all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] a + b + c + d = d + c + b + a && e | f | g | h = h | g | f | e") # test_integration-blasting_correctness_check3.cpp:85
add_blasting_equiv_test(blasting_equiv-32bit_8var-chain_ineq_gap
	"ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h - a >= { 7 }:bv[32]") # test_integration-blasting_correctness_check3.cpp:90
add_blasting_equiv_test(blasting_equiv-32bit_8var-and_commutativity
	"all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] (a & b) | (c & d) | (e & f) | (g & h) = (b & a) | (d & c) | (f & e) | (h & g)") # test_integration-blasting_correctness_check3.cpp:95
add_blasting_equiv_test(blasting_equiv-32bit_8var-arith_composition
	"ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a > { 0 }:bv[32] && b = a * { 2 }:bv[32] && b > a && c = b + a && d = c >> { 1 }:bv[32] && e = d + a && f = e % { 7 }:bv[32] && g = f + a && h = g & a && h > { 0 }:bv[32]") # test_integration-blasting_correctness_check3.cpp:100
add_blasting_equiv_test(blasting_equiv-32bit_8var-xor_xnor_nand_commutativity
	"all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] a ^ b ^ c ^ d = d ^ c ^ b ^ a && e !^ f = f !^ e && g !& h = h !& g") # test_integration-blasting_correctness_check3.cpp:105
add_blasting_equiv_test(blasting_equiv-32bit_8var-xor_and_sat
	"ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a < b && b < c && c < d && e = a + d && f = b + c && g = e ^ f && h = g & { 255 }:bv[32] && h > { 0 }:bv[32]") # test_integration-blasting_correctness_check3.cpp:110
add_blasting_equiv_test(blasting_equiv-32bit_8var-add_commutativity_nested
	"all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] ((a & b) | (c ^ d)) + ((e | f) & (g !^ h)) = ((e | f) & (g !^ h)) + ((a & b) | (c ^ d))") # test_integration-blasting_correctness_check3.cpp:115
add_blasting_equiv_test(blasting_equiv-32bit_8var-shift_mul_div_sat
	"ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a > { 0 }:bv[32] && b = a << { 1 }:bv[32] && b > a && c = b << { 1 }:bv[32] && d = c >> { 1 }:bv[32] && d = b && e = a * { 3 }:bv[32] && f = e / { 3 }:bv[32] && f = a && g = a | b && h = g & c && h > { 0 }:bv[32]") # test_integration-blasting_correctness_check3.cpp:120
add_blasting_equiv_test(blasting_equiv-32bit_8var-alt_quant_negation
	"all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] all e:bv[32] ex f:bv[32] all g:bv[32] ex h:bv[32] a + b = { 0 }:bv[32] && c + d = { 0 }:bv[32] && e + f = { 0 }:bv[32] && g + h = { 0 }:bv[32]") # test_integration-blasting_correctness_check3.cpp:125
add_blasting_equiv_test(blasting_equiv-32bit_8var-alt_quant_mixed
	"all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] all e:bv[32] ex f:bv[32] all g:bv[32] ex h:bv[32] b = a ^ { 4294967295 }:bv[32] && d = c + { 1 }:bv[32] && f = e >> { 1 }:bv[32] && f <= e && h = g | { 1 }:bv[32]") # test_integration-blasting_correctness_check3.cpp:130
add_blasting_equiv_test(blasting_equiv-32bit_8var-alt_quant_cross_dep
	"all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] all e:bv[32] ex f:bv[32] all g:bv[32] ex h:bv[32] b = a + { 1 }:bv[32] && d = b + c && f = d ^ e && h = f & g") # test_integration-blasting_correctness_check3.cpp:135

# blasting_8bit_8var -- test_integration-blasting_correctness_check3.cpp
add_blasting_equiv_test(blasting_equiv-8bit_8var-add_or_commutativity
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] a + b + c + d = d + c + b + a && e | f | g | h = h | g | f | e") # test_integration-blasting_correctness_check3.cpp:147
add_blasting_equiv_test(blasting_equiv-8bit_8var-chain_ineq_gap
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h - a >= { 7 }:bv[8]") # test_integration-blasting_correctness_check3.cpp:152
add_blasting_equiv_test(blasting_equiv-8bit_8var-and_commutativity
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] (a & b) | (c & d) | (e & f) | (g & h) = (b & a) | (d & c) | (f & e) | (h & g)") # test_integration-blasting_correctness_check3.cpp:157
add_blasting_equiv_test(blasting_equiv-8bit_8var-arith_composition
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a > { 0 }:bv[8] && b = a * { 2 }:bv[8] && b > a && c = b + a && d = c >> { 1 }:bv[8] && e = d + a && f = e % { 7 }:bv[8] && g = f + a && h = g & a && h > { 0 }:bv[8]") # test_integration-blasting_correctness_check3.cpp:162
add_blasting_equiv_test(blasting_equiv-8bit_8var-xor_xnor_nand_commutativity
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] a ^ b ^ c ^ d = d ^ c ^ b ^ a && e !^ f = f !^ e && g !& h = h !& g") # test_integration-blasting_correctness_check3.cpp:167
add_blasting_equiv_test(blasting_equiv-8bit_8var-xor_and_sat
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a < b && b < c && c < d && e = a + d && f = b + c && g = e ^ f && h = g & { 15 }:bv[8] && h > { 0 }:bv[8]") # test_integration-blasting_correctness_check3.cpp:172
add_blasting_equiv_test(blasting_equiv-8bit_8var-add_commutativity_nested
	"all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] ((a & b) | (c ^ d)) + ((e | f) & (g !^ h)) = ((e | f) & (g !^ h)) + ((a & b) | (c ^ d))") # test_integration-blasting_correctness_check3.cpp:177
add_blasting_equiv_test(blasting_equiv-8bit_8var-shift_mul_div_sat
	"ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a > { 0 }:bv[8] && b = a << { 1 }:bv[8] && b > a && c = b << { 1 }:bv[8] && d = c >> { 1 }:bv[8] && d = b && e = a * { 3 }:bv[8] && f = e / { 3 }:bv[8] && f = a && g = a | b && h = g & c && h > { 0 }:bv[8]") # test_integration-blasting_correctness_check3.cpp:182
add_blasting_equiv_test(blasting_equiv-8bit_8var-alt_quant_negation
	"all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] all e:bv[8] ex f:bv[8] all g:bv[8] ex h:bv[8] a + b = { 0 }:bv[8] && c + d = { 0 }:bv[8] && e + f = { 0 }:bv[8] && g + h = { 0 }:bv[8]") # test_integration-blasting_correctness_check3.cpp:187
add_blasting_equiv_test(blasting_equiv-8bit_8var-alt_quant_mixed
	"all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] all e:bv[8] ex f:bv[8] all g:bv[8] ex h:bv[8] b = a ^ { 255 }:bv[8] && d = c + { 1 }:bv[8] && f = e >> { 1 }:bv[8] && f <= e && h = g | { 1 }:bv[8]") # test_integration-blasting_correctness_check3.cpp:192
add_blasting_equiv_test(blasting_equiv-8bit_8var-alt_quant_cross_dep
	"all a:bv[8] ex b:bv[8] all c:bv[8] ex d:bv[8] all e:bv[8] ex f:bv[8] all g:bv[8] ex h:bv[8] b = a + { 1 }:bv[8] && d = b + c && f = d ^ e && h = f & g") # test_integration-blasting_correctness_check3.cpp:197

# blasting_32bit_16var -- test_integration-blasting_correctness_check4.cpp
add_blasting_equiv_test(blasting_equiv-32bit_16var-add_or_commutativity
	"all a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a + b + c + d + e + f + g + h = h + g + f + e + d + c + b + a && i | j | k | l | m | o | p | q = q | p | o | m | l | k | j | i") # test_integration-blasting_correctness_check4.cpp:22
add_blasting_equiv_test(blasting_equiv-32bit_16var-chain_ineq_gap
	"ex a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h < i && i < j && j < k && k < l && l < m && m < o && o < p && p < q && q - a >= { 15 }:bv[32]") # test_integration-blasting_correctness_check4.cpp:27
add_blasting_equiv_test(blasting_equiv-32bit_16var-and_commutativity
	"all a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] (a & b) | (c & d) | (e & f) | (g & h) | (i & j) | (k & l) | (m & o) | (p & q) = (b & a) | (d & c) | (f & e) | (h & g) | (j & i) | (l & k) | (o & m) | (q & p)") # test_integration-blasting_correctness_check4.cpp:32
add_blasting_equiv_test(blasting_equiv-32bit_16var-arith_composition
	"ex a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a > { 0 }:bv[32] && b = a * { 2 }:bv[32] && b > a && c = b + a && d = c >> { 1 }:bv[32] && e = d + a && f = e % { 7 }:bv[32] && g = f + a && h = g & a && h > { 0 }:bv[32] && i = a | b && j = i ^ c && k = j & d && l = k + e && m = l >> { 1 }:bv[32] && o = m | f && p = o ^ g && q = p & h && q > { 0 }:bv[32]") # test_integration-blasting_correctness_check4.cpp:37
add_blasting_equiv_test(blasting_equiv-32bit_16var-xor_xnor_nand_nor_commutativity
	"all a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a ^ b ^ c ^ d ^ e ^ f ^ g ^ h = h ^ g ^ f ^ e ^ d ^ c ^ b ^ a && i !^ j = j !^ i && k !& l = l !& k && m !| o = o !| m && p | q >= p && p | q >= q") # test_integration-blasting_correctness_check4.cpp:42
add_blasting_equiv_test(blasting_equiv-32bit_16var-xor_and_sat
	"ex a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a < b && b < c && c < d && d < e && e < f && f < g && g < h && i = a + h && j = b + g && k = c + f && l = d + e && m = i ^ j && o = k ^ l && p = m ^ o && q = p + a && q > a") # test_integration-blasting_correctness_check4.cpp:47
add_blasting_equiv_test(blasting_equiv-32bit_16var-or_and_order
	"all a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a | b >= a && c | d >= c && e | f >= e && g | h >= g && i & j <= i && k & l <= k && m & o <= m && p & q <= p") # test_integration-blasting_correctness_check4.cpp:52
add_blasting_equiv_test(blasting_equiv-32bit_16var-shift_mul_div_sat
	"ex a:bv[32], b:bv[32], c:bv[32], d:bv[32], e:bv[32], f:bv[32], g:bv[32], h:bv[32], i:bv[32], j:bv[32], k:bv[32], l:bv[32], m:bv[32], o:bv[32], p:bv[32], q:bv[32] a > { 0 }:bv[32] && b = a << { 1 }:bv[32] && b > a && c = b << { 1 }:bv[32] && d = c >> { 1 }:bv[32] && d = b && e = a * { 3 }:bv[32] && f = e / { 3 }:bv[32] && f = a && g = a | b && h = g & c && h > { 0 }:bv[32] && i = a + b && j = i % { 7 }:bv[32] && k = j + a && l = k & b && m = l | c && o = m ^ d && p = o + a && q = p >> { 1 }:bv[32] && q > { 0 }:bv[32]") # test_integration-blasting_correctness_check4.cpp:57
add_blasting_equiv_test(blasting_equiv-32bit_16var-alt_quant_negation
	"all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] all e:bv[32] ex f:bv[32] all g:bv[32] ex h:bv[32] all i:bv[32] ex j:bv[32] all k:bv[32] ex l:bv[32] all m:bv[32] ex o:bv[32] all p:bv[32] ex q:bv[32] a + b = { 0 }:bv[32] && c + d = { 0 }:bv[32] && e + f = { 0 }:bv[32] && g + h = { 0 }:bv[32] && i + j = { 0 }:bv[32] && k + l = { 0 }:bv[32] && m + o = { 0 }:bv[32] && p + q = { 0 }:bv[32]") # test_integration-blasting_correctness_check4.cpp:62
add_blasting_equiv_test(blasting_equiv-32bit_16var-alt_quant_cross_dep
	"all a:bv[32] ex b:bv[32] all c:bv[32] ex d:bv[32] all e:bv[32] ex f:bv[32] all g:bv[32] ex h:bv[32] all i:bv[32] ex j:bv[32] all k:bv[32] ex l:bv[32] all m:bv[32] ex o:bv[32] all p:bv[32] ex q:bv[32] b = a ^ { 4294967295 }:bv[32] && d = c + b && f = e & b && h = g | d && j = i ^ f && l = k + j && o = m & l && q = p | o") # test_integration-blasting_correctness_check4.cpp:67

# blasting_128bit_4var -- test_integration-blasting_correctness_check4.cpp
add_blasting_equiv_test(blasting_equiv-128bit_4var-add_commutativity
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] (a + b) + (c + d) = (d + c) + (b + a)") # test_integration-blasting_correctness_check4.cpp:80
add_blasting_equiv_test(blasting_equiv-128bit_4var-chain_ineq_gap
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a < b && b < c && c < d && d - a >= { 3 }:bv[128]") # test_integration-blasting_correctness_check4.cpp:85
add_blasting_equiv_test(blasting_equiv-128bit_4var-distributivity
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] a & (b | c) | d = (a & b) | (a & c) | d") # test_integration-blasting_correctness_check4.cpp:90
add_blasting_equiv_test(blasting_equiv-128bit_4var-arith_composition
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a > { 0 }:bv[128] && b = a * { 3 }:bv[128] && c = b + a && d = c >> { 2 }:bv[128] && d > { 0 }:bv[128]") # test_integration-blasting_correctness_check4.cpp:95
add_blasting_equiv_test(blasting_equiv-128bit_4var-xor_assoc
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] (a ^ b) ^ c = a ^ (b ^ c) && a | d = d | a") # test_integration-blasting_correctness_check4.cpp:100
add_blasting_equiv_test(blasting_equiv-128bit_4var-mod_existence
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a % { 5 }:bv[128] = { 2 }:bv[128] && b = a + { 3 }:bv[128] && c = a | b && d = c & a && d !>= b") # test_integration-blasting_correctness_check4.cpp:105
add_blasting_equiv_test(blasting_equiv-128bit_4var-or_and_order
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] a | b >= a && a | b >= b && c & d <= c && c & d <= d") # test_integration-blasting_correctness_check4.cpp:110
add_blasting_equiv_test(blasting_equiv-128bit_4var-shift_existence
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a > { 0 }:bv[128] && b = a << { 1 }:bv[128] && b > a && c = b << { 1 }:bv[128] && d = c >> { 1 }:bv[128] && d = b") # test_integration-blasting_correctness_check4.cpp:115
add_blasting_equiv_test(blasting_equiv-128bit_4var-alt_quant_negation
	"all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] a + b = { 0 }:bv[128] && c + d = { 0 }:bv[128]") # test_integration-blasting_correctness_check4.cpp:120
add_blasting_equiv_test(blasting_equiv-128bit_4var-alt_quant_and_dep
	"all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] b = a + { 1 }:bv[128] && d = b ^ c") # test_integration-blasting_correctness_check4.cpp:125
add_blasting_equiv_test(blasting_equiv-128bit_4var-alt_quant_shift_bound
	"all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] b = a >> { 1 }:bv[128] && b <= a && d = c << { 1 }:bv[128]") # test_integration-blasting_correctness_check4.cpp:130

# blasting_128bit_16var -- test_integration-blasting_correctness_check5.cpp
add_blasting_equiv_test(blasting_equiv-128bit_16var-add_or_commutativity
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] all i:bv[128] all j:bv[128] all k:bv[128] all l:bv[128] all m:bv[128] all o:bv[128] all p:bv[128] all q:bv[128] a + b + c + d + e + f + g + h = h + g + f + e + d + c + b + a && i | j | k | l | m | o | p | q = q | p | o | m | l | k | j | i") # test_integration-blasting_correctness_check5.cpp:22
add_blasting_equiv_test(blasting_equiv-128bit_16var-chain_ineq_gap
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] ex i:bv[128] ex j:bv[128] ex k:bv[128] ex l:bv[128] ex m:bv[128] ex o:bv[128] ex p:bv[128] ex q:bv[128] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h < i && i < j && j < k && k < l && l < m && m < o && o < p && p < q && q - a >= { 15 }:bv[128]") # test_integration-blasting_correctness_check5.cpp:27
add_blasting_equiv_test(blasting_equiv-128bit_16var-and_commutativity
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] all i:bv[128] all j:bv[128] all k:bv[128] all l:bv[128] all m:bv[128] all o:bv[128] all p:bv[128] all q:bv[128] (a & b) | (c & d) | (e & f) | (g & h) | (i & j) | (k & l) | (m & o) | (p & q) = (b & a) | (d & c) | (f & e) | (h & g) | (j & i) | (l & k) | (o & m) | (q & p)") # test_integration-blasting_correctness_check5.cpp:32
add_blasting_equiv_test(blasting_equiv-128bit_16var-arith_composition
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] ex i:bv[128] ex j:bv[128] ex k:bv[128] ex l:bv[128] ex m:bv[128] ex o:bv[128] ex p:bv[128] ex q:bv[128] a > { 0 }:bv[128] && b = a * { 2 }:bv[128] && b > a && c = b + a && d = c >> { 1 }:bv[128] && e = d + a && f = e % { 7 }:bv[128] && g = f + a && h = g & a && h > { 0 }:bv[128] && i = a | b && j = i ^ c && k = j & d && l = k + e && m = l >> { 1 }:bv[128] && o = m | f && p = o ^ g && q = p & h && q > { 0 }:bv[128]") # test_integration-blasting_correctness_check5.cpp:37
add_blasting_equiv_test(blasting_equiv-128bit_16var-xor_xnor_nand_nor_commutativity
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] all i:bv[128] all j:bv[128] all k:bv[128] all l:bv[128] all m:bv[128] all o:bv[128] all p:bv[128] all q:bv[128] a ^ b ^ c ^ d ^ e ^ f ^ g ^ h = h ^ g ^ f ^ e ^ d ^ c ^ b ^ a && i !^ j = j !^ i && k !& l = l !& k && m !| o = o !| m && p | q >= p && p | q >= q") # test_integration-blasting_correctness_check5.cpp:42
add_blasting_equiv_test(blasting_equiv-128bit_16var-xor_and_sat
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] ex i:bv[128] ex j:bv[128] ex k:bv[128] ex l:bv[128] ex m:bv[128] ex o:bv[128] ex p:bv[128] ex q:bv[128] a < b && b < c && c < d && d < e && e < f && f < g && g < h && i = a + h && j = b + g && k = c + f && l = d + e && m = i ^ j && o = k ^ l && p = m ^ o && q = p + a && q > a") # test_integration-blasting_correctness_check5.cpp:47
add_blasting_equiv_test(blasting_equiv-128bit_16var-or_and_order
	"all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] all i:bv[128] all j:bv[128] all k:bv[128] all l:bv[128] all m:bv[128] all o:bv[128] all p:bv[128] all q:bv[128] a | b >= a && c | d >= c && e | f >= e && g | h >= g && i & j <= i && k & l <= k && m & o <= m && p & q <= p") # test_integration-blasting_correctness_check5.cpp:52
add_blasting_equiv_test(blasting_equiv-128bit_16var-shift_mul_div_sat
	"ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] ex i:bv[128] ex j:bv[128] ex k:bv[128] ex l:bv[128] ex m:bv[128] ex o:bv[128] ex p:bv[128] ex q:bv[128] a > { 0 }:bv[128] && b = a << { 1 }:bv[128] && b > a && c = b << { 1 }:bv[128] && d = c >> { 1 }:bv[128] && d = b && e = a * { 3 }:bv[128] && f = e / { 3 }:bv[128] && f = a && g = a | b && h = g & c && h > { 0 }:bv[128] && i = a + b && j = i % { 7 }:bv[128] && k = j + a && l = k & b && m = l | c && o = m ^ d && p = o + a && q = p >> { 1 }:bv[128] && q > { 0 }:bv[128]") # test_integration-blasting_correctness_check5.cpp:57
add_blasting_equiv_test(blasting_equiv-128bit_16var-alt_quant_negation
	"all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] all e:bv[128] ex f:bv[128] all g:bv[128] ex h:bv[128] all i:bv[128] ex j:bv[128] all k:bv[128] ex l:bv[128] all m:bv[128] ex o:bv[128] all p:bv[128] ex q:bv[128] a + b = { 0 }:bv[128] && c + d = { 0 }:bv[128] && e + f = { 0 }:bv[128] && g + h = { 0 }:bv[128] && i + j = { 0 }:bv[128] && k + l = { 0 }:bv[128] && m + o = { 0 }:bv[128] && p + q = { 0 }:bv[128]") # test_integration-blasting_correctness_check5.cpp:62
add_blasting_equiv_test(blasting_equiv-128bit_16var-alt_quant_cross_dep
	"all a:bv[128] ex b:bv[128] all c:bv[128] ex d:bv[128] all e:bv[128] ex f:bv[128] all g:bv[128] ex h:bv[128] all i:bv[128] ex j:bv[128] all k:bv[128] ex l:bv[128] all m:bv[128] ex o:bv[128] all p:bv[128] ex q:bv[128] b = a + { 1 }:bv[128] && d = b + c && f = e & b && h = g | d && j = i ^ f && l = k + j && o = m & l && q = p | o") # test_integration-blasting_correctness_check5.cpp:67
