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
//   Φ_δ  (infeasibility, guarantee):
//            G(!( p_encode(σ) ∧ r_encode(ρ) ∧ d_pattern(A) )) for infeasible (σ,ρ,A)
//   φ*   (LTL skeleton, guarantee): verbatim

#ifndef __IDNI__TAU__ALGORITHM_B_SKELETON_H__
#define __IDNI__TAU__ALGORITHM_B_SKELETON_H__

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace idni::tau_lang::alg_b {

inline std::string P_name(int j) { return "p_" + std::to_string(j); }
inline std::string R_name(int j) { return "r_" + std::to_string(j); }

inline std::string p_encode(int sigma, int n_pbits) {
	if (n_pbits == 0) return "true";
	std::ostringstream ss;
	for (int j = 0; j < n_pbits; ++j) {
		if (j > 0) ss << " & ";
		if ((sigma >> j) & 1) ss << P_name(j);
		else                   ss << "!" << P_name(j);
	}
	return ss.str();
}

inline std::string r_encode(int rho, int n_rbits) {
	if (n_rbits == 0) return "true";
	std::ostringstream ss;
	for (int j = 0; j < n_rbits; ++j) {
		if (j > 0) ss << " & ";
		if ((rho >> j) & 1) ss << R_name(j);
		else                ss << "!" << R_name(j);
	}
	return ss.str();
}

inline std::string d_pattern(int A, int K) {
	if (K == 0) return "true";
	std::ostringstream ss;
	for (int i = 0; i < K; ++i) {
		if (i > 0) ss << " & ";
		if ((A >> i) & 1) ss << "d_" << i;
		else               ss << "!d_" << i;
	}
	return ss.str();
}

struct BSkeletonBundle {
	std::string formula;
	std::vector<std::string> outs;  // r_0..r_{n_rbits-1} then d_0..d_{K-1}
	std::vector<std::string> ins;   // p_0..p_{n_pbits-1}
	int n_rbits = 0;
	int n_pbits = 0;
	int T1_size = 0;
	int T2_size = 0;
	int K       = 0;
};

// Build the P_σ-augmented synthesis formula.
//
// feasible_set_b: triples (T2_idx, rho, A) that are feasible
//                (i.e. ∃T₃ type τ with 2-type(τ.m,τ.x)=T2[T2_idx] and 1-type(τ.y)=rho
//                 and D-bitmask of τ = A).
// t2_pos_m[σ]:   pos_m field of T₂ type σ (needed for Ψ_I grouping).
// phi_star_ltl:  LTL skeleton over d_0..d_{K-1} in Spot format.
inline BSkeletonBundle build_algorithm_b_skeleton(
	int T1_size,
	int T2_size,
	int K,
	const std::vector<std::tuple<int,int,int>>& feasible_set_b,
	const std::vector<int>& t2_pos_m,
	const std::string& phi_star_ltl)
{
	BSkeletonBundle b;
	b.T1_size = T1_size;
	b.T2_size = T2_size;
	b.K       = K;

	// ⌈log₂ T2_size⌉ P-bits (0 when T2_size ≤ 1)
	int n_pbits = 0;
	if (T2_size > 1) { n_pbits = 1; while ((1 << n_pbits) < T2_size) ++n_pbits; }
	b.n_pbits = n_pbits;

	// ⌈log₂ T1_size⌉ R-bits, minimum 1
	int n_rbits = 1;
	if (T1_size > 2) while ((1 << n_rbits) < T1_size) ++n_rbits;
	b.n_rbits = n_rbits;

	for (int j = 0; j < n_pbits; ++j) b.ins.push_back(P_name(j));
	for (int j = 0; j < n_rbits; ++j) b.outs.push_back(R_name(j));
	for (int i = 0; i < K; ++i)       b.outs.push_back("d_" + std::to_string(i));

	std::set<std::tuple<int,int,int>> feasible_idx(
		feasible_set_b.begin(), feasible_set_b.end());

	const int A_max   = 1 << K;

	std::vector<std::string> assume_parts, guarantee_parts;

	// ── Φ_I: P-validity (assume) ─────────────────────────────────────────
	for (int sigma = T2_size; sigma < (1 << n_pbits); ++sigma)
		assume_parts.push_back("G(!(" + p_encode(sigma, n_pbits) + "))");

	// ── Ψ_I: memory consistency (assume) ─────────────────────────────────
	// G( r_encode(ρ) → X(∨_{σ: t2_pos_m[σ]=ρ} p_encode(σ)) )
	for (int rho = 0; rho < T1_size; ++rho) {
		std::vector<int> matching;
		for (int sigma = 0; sigma < T2_size && sigma < (int)t2_pos_m.size(); ++sigma)
			if (t2_pos_m[sigma] == rho) matching.push_back(sigma);
		if (matching.empty()) continue;

		std::string disj;
		for (int i = 0; i < (int)matching.size(); ++i) {
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
	// G(!( p_encode(σ) ∧ r_encode(ρ) ∧ d_pattern(A) )) for infeasible (σ,ρ,A)
	for (int sigma = 0; sigma < T2_size; ++sigma) {
		for (int rho = 0; rho < T1_size; ++rho) {
			for (int A = 0; A < A_max; ++A) {
				if (feasible_idx.count({sigma, rho, A})) continue;
				std::string atom;
				if (K == 0) {
					atom = p_encode(sigma, n_pbits) + " & " + r_encode(rho, n_rbits);
				} else {
					atom = p_encode(sigma, n_pbits) + " & "
					     + r_encode(rho, n_rbits) + " & "
					     + d_pattern(A, K);
				}
				guarantee_parts.push_back("G(!(" + atom + "))");
			}
		}
	}

	// ── φ* verbatim (guarantee) ───────────────────────────────────────────
	guarantee_parts.push_back(
		"(" + (phi_star_ltl.empty() ? std::string("true") : phi_star_ltl) + ")");

	// ── Assemble: (∧assume) → (∧guarantee) ───────────────────────────────
	auto join = [](const std::vector<std::string>& v) {
		std::string s;
		for (int i = 0; i < (int)v.size(); ++i) {
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
