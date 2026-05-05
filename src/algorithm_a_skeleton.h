// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Algorithm A (D_i + R_ρ encoding) skeleton builder — paper's main optimization.
//
// Replaces the old binary T_3 Q-type encoding with:
//   R_ρ  — ⌈log₂|T₁|⌉ output bits binary-encoding the T₁ type of the current output y
//   D_i  — K output bits, one per data subformula (passed verbatim to φ*)
//
// Input:
//   T1_size        |T₁| = 2k+1 (k = number of named constants)
//   K              number of data subformulas
//   feasible_set   (σ, ρ, A): σ=T₁ type of prev y (memory), ρ=T₁ type of curr y,
//                  A=D-bitmask (bit i set ↔ D_i holds in this T₃ type)
//   phi_star_ltl   LTL skeleton over D_0..D_{K-1} in Spot syntax; passed verbatim
//
// Output formula:
//   (1) G(!r_encode(ρ))                                for ρ ∈ [T1_size, 2^n_rbits) — R-validity
//   (2) G(!(r_encode(ρ) & d_pattern(A)))               for (ρ,A) infeasible for all σ
//   (3) G(r_encode(σ) → X(∧ !(r_encode(ρ) & d_pattern(A))))  per σ
//   (4) (phi_star_ltl)
//
// outs: r_0..r_{n_rbits-1} then D_0..D_{K-1}

#ifndef __IDNI__TAU__ALGORITHM_A_SKELETON_H__
#define __IDNI__TAU__ALGORITHM_A_SKELETON_H__

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace idni::tau_lang::alg_a {

inline std::string R_name(int j) { return "r_" + std::to_string(j); }

// Encode T₁ index rho as a conjunction of n_rbits R-literals.
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

// Conjunction of D_i / !D_i literals for bitmask A over K atoms.
// Returns "true" when K == 0.
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

// Negated infeasibility atom: !(r_encode(rho) & d_pattern(A)).
// When K==0 collapses to !r_encode(rho) (avoids "& true").
inline std::string neg_atom(int rho, int A, int K, int n_rbits) {
	if (K == 0) return "!(" + r_encode(rho, n_rbits) + ")";
	return "!(" + r_encode(rho, n_rbits) + " & " + d_pattern(A, K) + ")";
}

struct SkeletonBundle {
	std::string formula;
	std::vector<std::string> outs; // r_0..r_{n_rbits-1} then D_0..D_{K-1}
	int n_rbits = 0;
	int T1_size = 0;
	int K       = 0;
};

inline SkeletonBundle build_algorithm_a_skeleton(
	int T1_size,
	int K,
	const std::vector<std::tuple<int,int,int>>& feasible_set, // (sigma, rho, A)
	const std::string& phi_star_ltl)
{
	SkeletonBundle b;
	b.T1_size = T1_size;
	b.K = K;

	// ⌈log₂T1_size⌉ R-bits, minimum 1.
	int n_rbits = 1;
	if (T1_size > 2) while ((1 << n_rbits) < T1_size) ++n_rbits;
	b.n_rbits = n_rbits;

	for (int j = 0; j < n_rbits; ++j) b.outs.push_back(R_name(j));
	for (int i = 0; i < K; ++i)       b.outs.push_back("d_" + std::to_string(i));

	// Index feasible triples by (rho, A) and by (sigma, rho) → set of A.
	std::set<std::pair<int,int>>      feasible_rho_A;
	// Per (sigma, rho), collect feasible A-masks.
	std::map<std::pair<int,int>, std::set<int>> sigma_rho_feasA;
	// Per rho, collect all feasible A-masks (union over all sigma).
	std::map<int, std::set<int>> rho_feasA;
	for (const auto& [s, r, a] : feasible_set) {
		feasible_rho_A.emplace(r, a);
		sigma_rho_feasA[{s, r}].insert(a);
		rho_feasA[r].insert(a);
	}

	const int A_max = 1 << K;

	std::ostringstream formula;
	bool first = true;

	auto append = [&](const std::string& clause) {
		if (!first) formula << " & ";
		first = false;
		formula << clause;
	};

	// (1) R-validity: G(!r_encode(rho)) for rho in [T1_size, 2^n_rbits).
	for (int rho = T1_size; rho < (1 << n_rbits); ++rho)
		append("G(!(" + r_encode(rho, n_rbits) + "))");

	// (2) Unconditional infeasibility: for each rho, constrain D-bits
	// to only feasible A-masks.  Compact form: emit a disjunction of
	// feasible D-patterns when that is smaller than enumerating the
	// infeasible complement.
	for (int rho = 0; rho < T1_size; ++rho) {
		auto it = rho_feasA.find(rho);
		int n_feas = it != rho_feasA.end() ? (int)it->second.size() : 0;
		int n_infeas = A_max - n_feas;
		if (n_infeas == 0) continue;
		if (n_feas == 0) {
			// rho is fully infeasible — forbid this R-encoding.
			append("G(!(" + r_encode(rho, n_rbits) + "))");
		} else if (n_feas <= n_infeas) {
			// Compact: emit G(r_encode(rho) → (feas_A1 | feas_A2 | ...))
			std::string body;
			bool bf = true;
			for (int A : it->second) {
				if (!bf) body += " | ";
				bf = false;
				body += "(" + d_pattern(A, K) + ")";
			}
			append("G((" + r_encode(rho, n_rbits) + ") -> (" + body + "))");
		} else {
			// Original: enumerate infeasible masks.
			for (int A = 0; A < A_max; ++A)
				if (!feasible_rho_A.count({rho, A}))
					append("G(" + neg_atom(rho, A, K, n_rbits) + ")");
		}
	}

	// (3) Conditional per sigma: G(r_encode(sigma) -> X(body)).
	// Compact: for each (sigma, rho), use feasible A-set disjunction
	// when it is smaller than the infeasible complement.
	for (int sigma = 0; sigma < T1_size; ++sigma) {
		std::string body;
		bool body_first = true;
		for (int rho = 0; rho < T1_size; ++rho) {
			auto key = std::make_pair(sigma, rho);
			auto it = sigma_rho_feasA.find(key);
			int n_feas = it != sigma_rho_feasA.end() ? (int)it->second.size() : 0;
			int n_infeas = A_max - n_feas;
			if (n_infeas == 0) continue;
			if (n_feas == 0) {
				// rho is fully infeasible given sigma — forbid this R-encoding.
				if (!body_first) body += " & ";
				body_first = false;
				body += "!(" + r_encode(rho, n_rbits) + ")";
			} else if (n_feas <= n_infeas) {
				// Compact: (r_encode(rho) → (feas_A1 | feas_A2 | ...))
				std::string disj;
				bool df = true;
				for (int A : it->second) {
					if (!df) disj += " | ";
					df = false;
					disj += "(" + d_pattern(A, K) + ")";
				}
				if (!body_first) body += " & ";
				body_first = false;
				body += "((" + r_encode(rho, n_rbits) + ") -> (" + disj + "))";
			} else {
				// Enumerate infeasible masks for this (sigma, rho).
				for (int A = 0; A < A_max; ++A) {
					if (it != sigma_rho_feasA.end()
							&& it->second.count(A))
						continue;
					if (!body_first) body += " & ";
					body_first = false;
					body += neg_atom(rho, A, K, n_rbits);
				}
			}
		}
		if (!body.empty())
			append("G((" + r_encode(sigma, n_rbits) + ") -> X(" + body + "))");
	}

	// (4) phi* verbatim.
	append("(" + (phi_star_ltl.empty() ? std::string("true") : phi_star_ltl) + ")");

	b.formula = formula.str();
	return b;
}

} // namespace idni::tau_lang::alg_a

#endif // __IDNI__TAU__ALGORITHM_A_SKELETON_H__
