// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_semantic_pwr.tmpl.h
 * @brief qlt's winning-region revision, the optimal mode of pointwise revision.
 *
 * Implements θ = ψ ∧ G(Win), where Win says the product-game
 * state is in the winning region computed by Zielonka over the T_3 order types.
 * Every part of that is (Q,<) theory, so all of it lives here and core reaches
 * it through one capability.
 *
 * Included from qlt_descriptor.tmpl.h and nowhere else. satisfiability.h and
 * ltl_aba.h are unreachable here (both lead to normalizer.h), so what core
 * supplies is forward-declared and completed at instantiation.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_SEMANTIC_PWR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_SEMANTIC_PWR_TMPL_H__

#include <set>
#include <string>
#include <vector>

#include "algorithm_d_game.h"
#include "boolean_algebras/qlt/qlt_ltl_synthesis.tmpl.h"
#include "boolean_algebras/qlt/omcat_constants.h"

namespace idni::tau_lang {

// The `pwr_semantic_fallback` runtime toggle for this fallback lives in
// core's `pointwise_revision.h`, the only file that reads it -- not here.

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
	const alg_d::alg_d_result& result,
	const std::vector<std::pair<tref, std::string>>& atoms,
	const std::vector<omcat::qlt_type3>& T3,
	const std::vector<int>& type_A)
{
	const int K = result.K;
	const size_t T1_size = result.T1_size;

	// Only base product-game states encode valid (q, ρ) pairs.
	// Stub states (for transition-based edge acceptance) have indices
	// >= base_n and must be skipped.
	const size_t base_n = result.synth_game.num_states * T1_size;

	std::set<int> winning_rhos;
	for (size_t s : result.winning_region) {
		if (s >= base_n) continue;
		// The memory index is the low T₁ part of the product state.
		int rho = static_cast<int>(s % T1_size);
		winning_rhos.insert(rho);
	}

	// Collect all D-patterns reachable from winning ρ values.
	std::set<int> winning_patterns;
	for (size_t t = 0; t < T3.size(); ++t) {
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
			// i is a loop index, so it is a valid atom index here.
			const auto& atom = atoms[(size_t) i];
			tref literal = (pat & (1 << i))
				? atom.first
				: build_wff_neg<node>(atom.first);
			conj = conj ? build_wff_and<node>(conj, literal) : literal;
		}
		if (!conj) continue;
		win = win ? build_wff_or<node>(win, conj) : conj;
	}
	return win;
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
// Returns a null tree if optimal mode is not applicable or fails. The caller
// checks that θ is realizable -- that question is not order-type theory.
// ---------------------------------------------------------------------------

template <NodeType node>
result<tref> qlt_semantic_pwr_optimal(tref clause, tref update) {
	using tau = tree<node>;
	result<tref> r;

	// Build the conjunction C ∧ ψ for Algorithm D.
	tref clause_and_update = build_wff_and<node>(clause, update);

	// Extract data atoms.
	auto atoms = extract_data_atoms<node>(clause_and_update);
	if (atoms.empty()) return r.with_value(nullptr);

	// Check applicability: all atoms must be qlt-type, no input vars,
	// lookback ≤ 1 (same gate as Algorithm D in ltl_aba).
	bool has_input = false;
	for (auto& [f, _] : atoms)
		if (atom_has_any_input<node>(f)) { has_input = true; break; }
	if (has_input) return r.with_value(nullptr);
	if (!is_algorithm_a_applicable<node>(atoms)) return r.with_value(nullptr);

	// The encoding below is Algorithm A's T_3 encoding, so it needs the two
	// soundness guards qlt_try_propositional_synthesis applies to that same
	// encoding, for the same reasons:
	//
	//   * an atom no T_3 type can classify (a `{top}:qlt` / `{bot}:qlt`
	//     constant) gives `qlt_atom_holds_in_type3 == atom_verdict::undecided`
	//     for every type, and the `h != atom_verdict::fails` test below maps
	//     undecided to "the atom holds" — so the atom is silently asserted
	//     rather than left out;
	//   * two or more distinct output variables share the single Y slot, so
	//     `o1 < c && o2 > c` collapses to a constraint on one witness.
	//
	// Either way `type_A` and the winning region are garbage, and the final
	// `is_tau_formula_sat(theta)` does not catch it: that only checks that θ
	// is realizable, not that G(Win) encodes the real winning region.  Fall
	// back to fast mode instead.
	TAU_TRY(auto can_classify,
		alg_a_can_classify<node>(clause_and_update, atoms));
	if (!can_classify) {
		LOG_DEBUG << "[semantic_pwr] atom outside T_3 (top/bot qlt "
		             "constant?) — optimal mode not applicable";
		return r.with_value(nullptr);
	}
	if (size_t n_out = count_distinct_output_vars<node>(atoms); n_out > 1) {
		LOG_DEBUG << "[semantic_pwr] " << n_out << " output variables — "
		             "Algorithm A's single Y/M slot would conflate them; "
		             "optimal mode not applicable";
		return r.with_value(nullptr);
	}

	// Collect qlt constants and enumerate T3 types.
	TAU_TRY(auto constants,
		omcat::collect_qlt_constants<node>(clause_and_update));
	auto T3 = omcat::enumerate_qlt_T3(constants);
	int K = (int)atoms.size();
	size_t T1_size = 2 * constants.size() + 1;
	// LS-11: named cap + a log line when it trips (the silent gate hid
	// why optimal mode never ran for >= 21 atoms). The cap is qlt's
	// runtime option `qlt-t3-cap` (qlt_t3_encoding_cap, qlt.h).
	const int semantic_pwr_max_atoms = qlt_t3_encoding_cap_effective();
	if (T1_size == 0 || K <= 0 || K > semantic_pwr_max_atoms) {
		if (K > semantic_pwr_max_atoms)
			TAU_LOG_DEBUG << "[semantic_pwr] optimal mode skipped: "
				<< K << " atoms exceed the cap ("
				<< semantic_pwr_max_atoms << ")";
		return r.with_value(nullptr);
	}

	// Compute D-bitmask for each T3 type and build the propositional
	// skeleton φ*(D_i) (LS-12: shared helpers in qlt_ltl_synthesis.tmpl.h).
	TAU_TRY(auto type_A,
		qlt_type_A_bitmasks<node>(atoms, T3, constants));
	auto phi_star_skel_r = ltl_skeleton<node>(clause_and_update, atoms);
	// A CTL* node with no sound propositional encoding reached the skeleton
	// walk: fall back to fast mode, same as every other not-applicable or
	// backend-failure exit in this function (see the Algorithm D check below).
	if (!phi_star_skel_r.has_value()) return r.with_value(nullptr);
	std::string phi_star = rename_skeleton_props_to_d(
		std::move(phi_star_skel_r.value()), K);

	LOG_DEBUG << "[semantic_pwr] trying optimal mode: K=" << K
	          << " T1=" << T1_size << " phi_star=" << phi_star;

	// Run Algorithm D (full) to get winning region.  LG-12: fixed initial
	// memory ρ₀ = type_of(0), the interpreter's lookback-at-t=0 convention.
	auto alg_result_r = alg_d::solve_algorithm_d_full(
		phi_star, T1_size, T3, type_A, K,
		alg_d::initial_memory(constants));
	// A backend failure here is undecided, not unrealizable; fall back
	// to fast mode the same way any other not-applicable case does.
	if (!alg_result_r.has_value()) return r.with_value(nullptr);
	auto& alg_result = alg_result_r.value();

	if (!alg_result.realizable) {
		LOG_DEBUG << "[semantic_pwr] unrealizable via Algorithm D";
		return r.with_value(nullptr);
	}

	LOG_DEBUG << "[semantic_pwr] winning region size="
	          << alg_result.winning_region.size()
	          << " init_rho=" << alg_result.init_rho;

	// Build Win formula from winning region.
	tref win = build_win_formula<node>(alg_result, atoms, T3, type_A);
	if (!win) return r.with_value(nullptr);

	// Build θ = ψ ∧ G(Win)
	// Win_0 ∧ G(Win → X Win) ≡ G(Win) when Win_0 ⊆ Win.
	tref g_win = tau::build_wff_always(win);
	tref theta = build_wff_and<node>(update, g_win);

	// Whether θ is realizable is an ordinary satisfiability question, so the
	// caller asks it; nothing here is (Q,<) theory any more.
	LOG_DEBUG << "[semantic_pwr] optimal mode produced a revision";
	return r.with_value(theta);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_SEMANTIC_PWR_TMPL_H__
