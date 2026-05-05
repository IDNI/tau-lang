// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Heuristic picker between Algorithms A, B, C, and D (#6).
//
// Given the formula's structural parameters (|T_1|, |T_2|, |T_3|, K=|Δ|,
// |Q| from the DPA for the φ* skeleton), estimate the skeleton size each
// algorithm would produce and return the smallest.  The picker runs in
// O(1); estimation formulas come from LTL_ABA_DESIGN.md per-algorithm
// complexity analyses.
//
// Callers can override the pick with an env var `TAU_LTL_ALG ∈
// {A, B, C, D, auto}`; `auto` invokes this heuristic.

#ifndef __IDNI__TAU__ALGORITHM_PICKER_H__
#define __IDNI__TAU__ALGORITHM_PICKER_H__

#include <cstdint>

namespace idni::tau_lang::algpick {

enum class Algorithm { A, B, C, D };

// Estimated skeleton size, in symbols.  Tight-ish upper bound from the
// design-doc formulas; good enough to order-of-magnitude compare.
inline uint64_t estimate_size_A(uint64_t T3, uint64_t phi) {
	// O(|φ| · log|T_3| + |T_3|²).  Approximate log as ceil(log2).
	uint64_t log_T3 = 1;
	for (uint64_t x = T3; x > 1; x >>= 1) ++log_T3;
	return phi * log_T3 + T3 * T3;
}
inline uint64_t estimate_size_B(uint64_t T1, uint64_t T2, uint64_t T3,
                                uint64_t K, uint64_t phi)
{
	// O(|φ| + |T_2|·|T_1|·|T_3|·|Δ|).
	return phi + T1 * T2 * T3 * K;
}
inline uint64_t estimate_size_C(uint64_t T1, uint64_t K, uint64_t phi) {
	// |T_1|·2^K env inputs, K+|T_1| outputs, middle clause O(|T_1|·2^K·K).
	if (K > 31) K = 31;  // saturate to avoid overflow on huge K.
	uint64_t pow2K = 1ull << K;
	return phi + T1 * pow2K * K + T1 * T1 /* Φ_R */;
}
inline uint64_t estimate_size_D(uint64_t T1, uint64_t T2, uint64_t T3,
                                uint64_t Q)
{
	// Parity-game vertices (|T_1|+|T_2|)·|Q|, Pre scans T_2·T_3 per call,
	// alternation depth bounded.  Dominant: game-graph size.
	return (T1 + T2) * Q + T2 * T3;
}

// Pick the algorithm with smallest estimated skeleton/arena size.
//
// has_input_vars: true iff the formula contains input variables.
//   Algorithms A and D are output-only.  A lacks P_σ encoding, so the system
//   cannot observe the environment's data type; D's direct parity-game product
//   currently models only output D_i choices.  When has_input_vars is true,
//   both are excluded from consideration.
inline Algorithm pick(uint64_t T1, uint64_t T2, uint64_t T3,
                      uint64_t K, uint64_t Q, uint64_t phi,
                      bool has_input_vars = false)
{
	// Start with B as the baseline (always sound).
	uint64_t sB = estimate_size_B(T1, T2, T3, K, phi);
	uint64_t best = sB; Algorithm a = Algorithm::B;

	// A is only sound for output-only formulas.
	if (!has_input_vars) {
		uint64_t sA = estimate_size_A(T3, phi);
		if (sA < best) { best = sA; a = Algorithm::A; }
	}

	uint64_t sC = estimate_size_C(T1, K, phi);
	if (sC < best) { best = sC; a = Algorithm::C; }
	if (!has_input_vars) {
		uint64_t sD = estimate_size_D(T1, T2, T3, Q);
		if (sD < best) { best = sD; a = Algorithm::D; }
	}
	return a;
}

} // namespace idni::tau_lang::algpick

#endif // __IDNI__TAU__ALGORITHM_PICKER_H__
