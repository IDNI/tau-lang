// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Algorithm B: P_σ binary encoding — extends Algorithm A (D_i + R_ρ) with
// ⌈log₂|T₂|⌉ input propositions encoding the T₂ = (pos_m, pos_x, rel_mx) type.
//
// This is needed for SOUNDNESS when the formula contains input-variable atoms
// (e.g. G(o1 > i1)): without P_σ the system cannot observe the input type and
// the oracle may fail to realize the propositional strategy.
//
// Propositional variables:
//   p_0..p_{n_pbits-1}  INPUT bits encoding T₂ index σ ∈ [0, T₂_size)
//   r_0..r_{n_rbits-1}  OUTPUT bits encoding T₁ index ρ ∈ [0, T₁_size)
//   d_0..d_{K-1}        OUTPUT bits, one per data subformula δ_i
//
// Formula structure (implication form so Ψ_I and Φ_I are env assumptions):
//   (Φ_I ∧ Ψ_I) → (Φ_O^R ∧ Φ_δ ∧ φ*)
//
//   Φ_I  (P-validity, assume): G(!p_encode(σ)) for σ ∈ [T₂_size, 2^n_pbits)
//   Ψ_I  (memory consistency, assume):
//            G( r_encode(ρ) → X(∨_{σ: pos_m[σ]=ρ} p_encode(σ)) )
//   Φ_O^R (R-validity, guarantee): G(!r_encode(ρ)) for ρ ∈ [T₁_size, 2^n_rbits)
//   Φ_δ  (infeasibility, guarantee), per (σ,ρ) with an infeasible A:
//            G(( p_encode(σ) ∧ r_encode(ρ) ) → ∨_{feasible A} d_pattern(A)),
//            or one G(!( p_encode(σ) ∧ r_encode(ρ) ∧ d_pattern(A) )) per
//            infeasible A when those are fewer
//   φ*   (LTL skeleton, guarantee): verbatim

#ifndef __IDNI__TAU__ALGORITHM_B_SKELETON_H__
#define __IDNI__TAU__ALGORITHM_B_SKELETON_H__

#include "algorithm_skeleton_common.h"

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

namespace idni::tau_lang::alg_b {

using alg_common::P_name;
using alg_common::R_name;
using alg_common::p_encode;
using alg_common::r_encode;
using alg_common::d_pattern;

/// @brief Output of `build_algorithm_b_skeleton`: the formula, its input
/// and output propositions and the encoding sizes.
struct b_skeleton_bundle {
	/// The synthesis formula in Spot syntax.
	std::string formula;
	std::vector<std::string> outs;  // r_0..r_{n_rbits-1} then d_0..d_{K-1}
	std::vector<std::string> ins;   // p_0..p_{n_pbits-1}
	/// Number of R-bits, at least 1.
	int n_rbits = 0;
	/// Number of P-bits, 0 when T2_size <= 1.
	int n_pbits = 0;
	/// |T_1| as passed to the builder.
	int T1_size = 0;
	/// |T_2| as passed to the builder.
	int T2_size = 0;
	/// Number of data subformulas d_i.
	int K       = 0;
};

/**
 * @brief Build the P_sigma-augmented synthesis formula.
 *
 * feasible_set_b: triples (T2_idx, rho, A) that are feasible
 *                (i.e. ∃T₃ type τ with 2-type(τ.m,τ.x)=T2[T2_idx] and 1-type(τ.y)=rho
 *                 and D-bitmask of τ = A).
 * t2_pos_m[σ]:   pos_m field of T₂ type σ (needed for Ψ_I grouping).
 * phi_star_ltl:  LTL skeleton over d_0..d_{K-1} in Spot format.
 * Complexity: per (sigma, rho), the shorter of the feasible and the
 * infeasible D-patterns, so the formula grows with
 * T2_size * T1_size * min(feasible, infeasible) patterns.
 * @param T1_size |T_1|.
 * @param T2_size |T_2|.
 * @param K Number of data subformulas.
 * @param feasible_set_b Feasible (T2_idx, rho, A) triples.
 * @param t2_pos_m pos_m field per T_2 type.
 * @param phi_star_ltl LTL skeleton over d_0..d_{K-1}; empty means "true".
 * @return The bundle {formula, outs, ins, n_rbits, n_pbits, T1_size,
 * T2_size, K}; the formula is `(assume) -> (guarantee)`, or the guarantee
 * alone when there is nothing to assume.
 */
inline b_skeleton_bundle build_algorithm_b_skeleton(
	int T1_size,
	int T2_size,
	int K,
	const std::vector<std::tuple<int,int,int>>& feasible_set_b,
	const std::vector<int>& t2_pos_m,
	const std::string& phi_star_ltl)
{
	b_skeleton_bundle b;
	b.T1_size = T1_size;
	b.T2_size = T2_size;
	b.K       = K;

	// ⌈log₂ T2_size⌉ P-bits (0 when T2_size ≤ 1)
	int n_pbits = alg_common::encoding_bits(T2_size, 0);
	b.n_pbits = n_pbits;

	// ⌈log₂ T1_size⌉ R-bits, minimum 1
	int n_rbits = alg_common::encoding_bits(T1_size, 1);
	b.n_rbits = n_rbits;

	for (int j = 0; j < n_pbits; ++j) b.ins.push_back(P_name(j));
	for (int j = 0; j < n_rbits; ++j) b.outs.push_back(R_name(j));
	for (int i = 0; i < K; ++i)       b.outs.push_back(alg_common::D_name(i));

	const int A_max   = 1 << K;

	std::vector<std::string> assume_parts, guarantee_parts;

	// ── Φ_I: P-validity (assume) ─────────────────────────────────────────
	for (int sigma = T2_size; sigma < (1 << n_pbits); ++sigma)
		assume_parts.push_back("G(!(" + p_encode(sigma, n_pbits) + "))");

	// ── Ψ_I: memory consistency (assume) ─────────────────────────────────
	// G( r_encode(ρ) → X(∨_{σ: t2_pos_m[σ]=ρ} p_encode(σ)) )
	for (int rho = 0; rho < T1_size; ++rho) {
		std::vector<int> matching;
		for (size_t sigma = 0; sigma < t2_pos_m.size()
				&& sigma < (size_t)T2_size; ++sigma)
			if (t2_pos_m[sigma] == rho)
				matching.push_back(static_cast<int>(sigma));
		if (matching.empty()) continue;

		std::string disj;
		for (size_t i = 0; i < matching.size(); ++i) {
			if (i > 0) disj += " | ";
			disj += "(" + p_encode(matching[i], n_pbits) + ")";
		}
		assume_parts.push_back(
			"G((" + r_encode(rho, n_rbits) + ") -> X(" + disj + "))");
	}

	// ── Φ_O^R: R-validity (guarantee) ────────────────────────────────────
	for (int rho = T1_size; rho < (1 << n_rbits); ++rho)
		guarantee_parts.push_back("G(!(" + r_encode(rho, n_rbits) + "))");

	// ── Φ_δ: infeasibility (guarantee) ───────────────────────────────────
	// Per (σ,ρ): forbid the pair when no A is feasible; otherwise list the
	// feasible D-patterns when they are no more than the infeasible ones,
	// G(p(σ) ∧ r(ρ) → ∨ d_pattern(A)), and forbid each infeasible one,
	// G(!(p(σ) ∧ r(ρ) ∧ d_pattern(A))), when they are fewer.
	std::map<std::pair<int,int>, std::set<int>> sigma_rho_feasA;
	for (const auto& [s, r, a] : feasible_set_b)
		sigma_rho_feasA[{s, r}].insert(a);
	for (int sigma = 0; sigma < T2_size; ++sigma) {
		for (int rho = 0; rho < T1_size; ++rho) {
			auto it = sigma_rho_feasA.find({sigma, rho});
			const int n_feas = it != sigma_rho_feasA.end()
				? (int)it->second.size() : 0;
			const int n_infeas = A_max - n_feas;
			if (n_infeas == 0) continue;
			const std::string pr = p_encode(sigma, n_pbits) + " & "
				+ r_encode(rho, n_rbits);
			if (n_feas == 0 || K == 0) {
				guarantee_parts.push_back("G(!(" + pr + "))");
			} else if (n_feas <= n_infeas) {
				std::string disj;
				for (int A : it->second) {
					if (!disj.empty()) disj += " | ";
					disj += "(" + d_pattern(A, K) + ")";
				}
				guarantee_parts.push_back(
					"G((" + pr + ") -> (" + disj + "))");
			} else {
				for (int A = 0; A < A_max; ++A) {
					if (it->second.count(A)) continue;
					guarantee_parts.push_back("G(!(" + pr + " & "
						+ d_pattern(A, K) + "))");
				}
			}
		}
	}

	// ── φ* verbatim (guarantee) ───────────────────────────────────────────
	guarantee_parts.push_back(
		"(" + (phi_star_ltl.empty() ? std::string("true") : phi_star_ltl) + ")");

	// ── Assemble: (∧assume) → (∧guarantee) ───────────────────────────────
	auto join = [](const std::vector<std::string>& v) {
		std::string s;
		for (size_t i = 0; i < v.size(); ++i) {
			if (i) s += " & ";
			s += v[i];
		}
		return s;
	};

	std::string assume_str    = join(assume_parts);
	std::string guarantee_str = join(guarantee_parts);

	if (assume_str.empty())
		b.formula = guarantee_str;
	else
		b.formula = "(" + assume_str + ") -> (" + guarantee_str + ")";

	return b;
}

} // namespace idni::tau_lang::alg_b
#endif // __IDNI__TAU__ALGORITHM_B_SKELETON_H__
