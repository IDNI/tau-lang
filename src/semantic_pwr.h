// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Semantic PWR (Optimal Mode): winning-region-based revision.
//
// Implements pwr-ltl.tex §11 "Semantic vs. syntactic revision":
//   θ = ψ ∧ G(Win)
// where Win encodes "the current product-game state is in the winning
// region W" and W comes from Zielonka on the product game for C ∧ ψ.
//
// Integration: fast mode by default; optimal mode as fallback when fast
// mode drops a clause entirely.  On safety formulas, the winning region
// collapses to the per-step atom formula (identical to fast mode).

#ifndef __IDNI__TAU__SEMANTIC_PWR_H__
#define __IDNI__TAU__SEMANTIC_PWR_H__

#include "algorithm_d_game.h"
#include "ltl_aba.h"
#include "omcat_constants.h"
#include "satisfiability.h"

namespace idni::tau_lang {

// ---------------------------------------------------------------------------
// Build the Win formula from a winning region.
//
// Win = ∨ over all D-patterns reachable from winning states of the
// conjunction of data-atom literals corresponding to that pattern.
//
// For a D-pattern bitmask p with K data atoms:
//   formula(p) = ∧_i  (atoms[i] if bit i is set, ¬atoms[i] otherwise)
// Win = ∨_p formula(p)
//
// This is exact for safety (single DPA state) and a sound approximation
// for liveness/nested temporal (ignoring DPA state tracking).
// ---------------------------------------------------------------------------

template <NodeType node>
tref build_win_formula(
	const alg_d::AlgDResult& result,
	const std::vector<std::pair<tref, std::string>>& atoms,
	const std::vector<omcat::QltType3>& T3,
	const std::vector<int>& type_A)
{
	const int K = result.K;
	const int T1_size = result.T1_size;

	// Only base product-game states encode valid (q, ρ) pairs.
	// Stub states (for transition-based edge acceptance) have indices
	// >= base_n and must be skipped.
	const int base_n = result.synth_game.num_states * T1_size;

	std::set<int> winning_rhos;
	for (int s : result.winning_region) {
		if (s >= base_n) continue;
		int rho = s % T1_size;
		winning_rhos.insert(rho);
	}

	// Collect all D-patterns reachable from winning ρ values.
	std::set<int> winning_patterns;
	for (int t = 0; t < (int)T3.size(); ++t) {
		if (winning_rhos.count(T3[t].pos_m))
			winning_patterns.insert(type_A[t]);
	}

	if (winning_patterns.empty()) return nullptr;

	// Build formula for each D-pattern and combine with disjunction.
	tref win = nullptr;
	for (int pat : winning_patterns) {
		// Build conjunction of atom literals for this pattern.
		tref conj = nullptr;
		for (int i = 0; i < K; ++i) {
			tref literal = (pat & (1 << i))
				? atoms[i].first
				: build_wff_neg<node>(atoms[i].first);
			conj = conj ? build_wff_and<node>(conj, literal) : literal;
		}
		if (!conj) continue;
		win = win ? build_wff_or<node>(win, conj) : conj;
	}
	return win;
}

// ---------------------------------------------------------------------------
// Build the Win_0 formula (initial state entry condition).
//
// Win_0 = ∨ over all ρ₀ such that (q_init, ρ₀) ∈ W of the data-atom
// condition for ρ₀.
// ---------------------------------------------------------------------------

template <NodeType node>
tref build_win0_formula(
	const alg_d::AlgDResult& result,
	const std::vector<std::pair<tref, std::string>>& atoms,
	const std::vector<omcat::QltType3>& T3,
	const std::vector<int>& type_A)
{
	const int K = result.K;
	const int T1_size = result.T1_size;
	const int q_init = result.synth_game.init;

	// Collect initial ρ₀ values where (q_init, ρ₀) ∈ W.
	// Initial states are always base states (index < base_n).
	std::set<int> init_rhos;
	for (int rho0 = 0; rho0 < T1_size; ++rho0) {
		int s0 = q_init * T1_size + rho0;
		if (result.winning_region.count(s0))
			init_rhos.insert(rho0);
	}

	// Collect D-patterns reachable from initial winning ρ₀ values.
	std::set<int> init_patterns;
	for (int t = 0; t < (int)T3.size(); ++t) {
		if (init_rhos.count(T3[t].pos_m))
			init_patterns.insert(type_A[t]);
	}

	if (init_patterns.empty()) return nullptr;

	// Build formula (same logic as build_win_formula but restricted to init).
	tref win0 = nullptr;
	for (int pat : init_patterns) {
		tref conj = nullptr;
		for (int i = 0; i < K; ++i) {
			tref literal = (pat & (1 << i))
				? atoms[i].first
				: build_wff_neg<node>(atoms[i].first);
			conj = conj ? build_wff_and<node>(conj, literal) : literal;
		}
		if (!conj) continue;
		win0 = win0 ? build_wff_or<node>(win0, conj) : conj;
	}
	return win0;
}

// ---------------------------------------------------------------------------
// Semantic PWR optimal mode: try Algorithm D on clause ∧ update.
//
// Given a spec clause C and full update ψ:
//   1. Extract data atoms from C ∧ ψ
//   2. Build propositional skeleton φ*(D_i)
//   3. Run Algorithm D (full) to get winning region W
//   4. Build θ = ψ ∧ G(Win)
//      (using Win_0 ∧ G(Win → X Win) ≡ G(Win) when Win_0 ⊆ Win)
//
// Returns nullptr if optimal mode is not applicable or fails.
// ---------------------------------------------------------------------------

template <NodeType node>
tref semantic_pwr_optimal(tref clause, tref update, const int_t start_time) {
	using tau = tree<node>;

	// Build the conjunction C ∧ ψ for Algorithm D.
	tref clause_and_update = build_wff_and<node>(clause, update);

	// Extract data atoms.
	auto atoms = extract_data_atoms<node>(clause_and_update);
	if (atoms.empty()) return nullptr;

	// Check applicability: all atoms must be qlt-type, no input vars,
	// lookback ≤ 1 (same gate as Algorithm D in ltl_aba).
	bool has_input = false;
	for (auto& [f, _] : atoms)
		if (atom_has_any_input<node>(f)) { has_input = true; break; }
	if (has_input) return nullptr;
	if (!is_algorithm_a_applicable<node>(atoms)) return nullptr;

	// Collect qlt constants and enumerate T3 types.
	auto constants = omcat::collect_qlt_constants<node>(clause_and_update);
	auto T3 = omcat::enumerate_qlt_T3(constants);
	int K = (int)atoms.size();
	int T1_size = 2 * (int)constants.size() + 1;
	if (T1_size <= 0 || K <= 0 || K > 20) return nullptr;

	// Compute D-bitmask for each T3 type.
	std::vector<int> type_A(T3.size(), 0);
	for (int i = 0; i < K; ++i) {
		for (int t = 0; t < (int)T3.size(); ++t) {
			auto h = qlt_atom_holds_in_type3<node>(
				atoms[i].first, T3[t], constants);
			if (h != false) type_A[t] |= (1 << i);
		}
	}

	// Build propositional skeleton φ*(D_i).
	std::string phi_star = ltl_skeleton<node>(clause_and_update, atoms);
	for (int i = K; i-- > 0; ) {
		std::string fp = "p" + std::to_string(i);
		std::string td = "d_" + std::to_string(i);
		size_t pos = 0;
		while ((pos = phi_star.find(fp, pos)) != std::string::npos) {
			size_t end = pos + fp.size();
			bool l_ok = pos == 0
				|| (!std::isalnum((unsigned char)phi_star[pos-1])
				    && phi_star[pos-1] != '_');
			bool r_ok = end >= phi_star.size()
				|| (!std::isalnum((unsigned char)phi_star[end])
				    && phi_star[end] != '_');
			if (l_ok && r_ok) {
				phi_star.replace(pos, fp.size(), td);
				pos += td.size();
			} else {
				pos = end;
			}
		}
	}

	LOG_DEBUG << "[semantic_pwr] trying optimal mode: K=" << K
	          << " T1=" << T1_size << " phi_star=" << phi_star;

	// Run Algorithm D (full) to get winning region.
	auto alg_result = alg_d::solve_algorithm_d_full(
		phi_star, T1_size, T3, type_A, K);

	if (!alg_result.realizable) {
		LOG_DEBUG << "[semantic_pwr] unrealizable via Algorithm D";
		return nullptr;
	}

	LOG_DEBUG << "[semantic_pwr] winning region size="
	          << alg_result.winning_region.size()
	          << " init_rho=" << alg_result.init_rho;

	// Build Win formula from winning region.
	tref win = build_win_formula<node>(alg_result, atoms, T3, type_A);
	if (!win) return nullptr;

	// Build θ = ψ ∧ G(Win)
	// Win_0 ∧ G(Win → X Win) ≡ G(Win) when Win_0 ⊆ Win.
	tref g_win = tau::build_wff_always(win);
	tref theta = build_wff_and<node>(update, g_win);

	// Verify θ is realizable.
	if (!is_tau_formula_sat<node>(theta, start_time)) {
		LOG_DEBUG << "[semantic_pwr] theta unrealizable; falling back";
		return nullptr;
	}

	LOG_DEBUG << "[semantic_pwr] optimal mode succeeded";
	return theta;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__SEMANTIC_PWR_H__
