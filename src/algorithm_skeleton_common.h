// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The propositional encodings Algorithms A and B share: the R-bits of a T_1
// index, the P-bits of a T_2 index, and the d_i pattern of a D-bitmask.

#ifndef __IDNI__TAU__ALGORITHM_SKELETON_COMMON_H__
#define __IDNI__TAU__ALGORITHM_SKELETON_COMMON_H__

#include <sstream>
#include <string>

namespace idni::tau_lang::alg_common {

/// @brief Name of the j-th P-bit input proposition, "p_<j>".
inline std::string P_name(int j) { return "p_" + std::to_string(j); }
/// @brief Name of the j-th R-bit output proposition, "r_<j>".
inline std::string R_name(int j) { return "r_" + std::to_string(j); }
/// @brief Name of the i-th data subformula proposition, "d_<i>".
inline std::string D_name(int i) { return "d_" + std::to_string(i); }

/**
 * @brief The conjunction of @p n literals of the propositions @p name
 * names, the j-th positive iff bit j of @p bits is set.
 * @return The conjunction in Spot syntax; "true" when @p n is 0.
 */
template <typename Name>
std::string bits_encode(int bits, int n, Name name) {
	if (n == 0) return "true";
	std::ostringstream ss;
	for (int j = 0; j < n; ++j) {
		if (j > 0) ss << " & ";
		if ((bits >> j) & 1) ss << name(j);
		else                 ss << "!" << name(j);
	}
	return ss.str();
}

/**
 * @brief Encode T_2 index sigma as a conjunction of n_pbits P-literals.
 * @param sigma T_2 index to encode.
 * @param n_pbits Number of P-bits; 0 yields "true".
 * @return The conjunction in Spot syntax.
 */
inline std::string p_encode(int sigma, int n_pbits) {
	return bits_encode(sigma, n_pbits, P_name);
}

/**
 * @brief Encode T_1 index rho as a conjunction of n_rbits R-literals.
 * @param rho T_1 index to encode.
 * @param n_rbits Number of R-bits; 0 yields "true".
 * @return The conjunction in Spot syntax.
 */
inline std::string r_encode(int rho, int n_rbits) {
	return bits_encode(rho, n_rbits, R_name);
}

/**
 * @brief Conjunction of d_i / !d_i literals for bitmask A over K atoms.
 * @param A D-bitmask.
 * @param K Number of data atoms; 0 yields "true".
 * @return The conjunction in Spot syntax.
 */
inline std::string d_pattern(int A, int K) {
	return bits_encode(A, K, D_name);
}

/**
 * @brief The bits that encode @p size indices, ⌈log₂ size⌉, at least
 * @p min_bits.
 */
inline int encoding_bits(int size, int min_bits) {
	int n = min_bits;
	while ((1 << n) < size) ++n;
	return n;
}

} // namespace idni::tau_lang::alg_common

#endif // __IDNI__TAU__ALGORITHM_SKELETON_COMMON_H__
