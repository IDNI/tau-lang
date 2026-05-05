// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Opt-6: decomposition by liveness (prune-then-solve).
//
// If φ = G(ψ_safe) ∧ ⋀_j GF(ψ_j), split into:
//   - safety_part:    the G(ψ_safe) invariant — it restricts the arena
//                     (prune types violating ψ_safe before solving liveness).
//   - liveness_parts: the individual GF obligations — each is a separate
//                     reachability condition solved by a single μX.Pre(X)
//                     fixpoint in the pruned arena.
//
// Returns the structured decomposition for the direct-game pipeline.
// Non-GR(1) formulas fall back to returning `overall = fm` and an empty
// `liveness_parts` vector — the caller should then dispatch to general
// LTL via the DPW product game (Algorithm D, Phase 1).

#ifndef __IDNI__TAU__LIVENESS_DECOMP_H__
#define __IDNI__TAU__LIVENESS_DECOMP_H__

#include <vector>

#include "gr1_detect.h"
#include "tau_tree.h"

namespace idni::tau_lang {

template <NodeType node>
struct LivenessDecomp {
	// The G(ψ_safe) conjunct, or nullptr if no safety.
	tref safety_part = nullptr;
	// Each GF(ψ_j)'s ψ_j body (one entry per GF obligation).
	std::vector<tref> liveness_parts;
	// True iff φ is a well-formed GR(1)-like conjunction of G(safety)
	// and GF(liveness) clauses (possibly nested under wff_and).  False
	// means the caller should dispatch to the general parity-game path.
	bool is_gr1 = false;
};

namespace liveness_decomp_internal {

template <NodeType node>
inline void gather_conjuncts(tref fm, std::vector<tref>& out) {
	using tau = tree<node>;
	if (!fm) return;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return;
	if (t[0].value.nt == tau::wff_and) {
		const auto& op = t[0];
		for (size_t i = 0; i < op.children_size(); ++i)
			gather_conjuncts<node>(op.child(i), out);
		return;
	}
	out.push_back(fm);
}

} // namespace liveness_decomp_internal

template <NodeType node>
inline LivenessDecomp<node> decompose_liveness(tref fm) {
	using tau = tree<node>;
	LivenessDecomp<node> d;
	if (!fm) return d;

	// Classify every conjunct.
	std::vector<tref> conjs;
	liveness_decomp_internal::gather_conjuncts<node>(fm, conjs);

	std::vector<tref> safety_bodies;
	for (tref c : conjs) {
		const auto& t = tau::get(c);
		if (!t.has_child()) { d.is_gr1 = false; return {}; }
		auto nt = t[0].value.nt;
		if (nt != tau::wff_always) { d.is_gr1 = false; return {}; }
		tref body = t[0].first();
		const auto& bt = tau::get(body);
		if (bt.has_child() && bt[0].value.nt == tau::wff_F) {
			// GF: record the inner body.
			d.liveness_parts.push_back(bt[0].first());
			continue;
		}
		if (gr1_detect_internal::is_non_temporal<node>(body)) {
			safety_bodies.push_back(body);
			continue;
		}
		// G with some non-trivial temporal inside — not GR(1).
		d.is_gr1 = false;
		return {};
	}
	d.is_gr1 = true;
	if (safety_bodies.size() == 1) {
		// Wrap as G(body).
		d.safety_part = tau::build_wff_always(safety_bodies[0]);
	} else if (safety_bodies.size() > 1) {
		// G(⋀ safety_bodies).
		tref acc = safety_bodies[0];
		for (size_t i = 1; i < safety_bodies.size(); ++i)
			acc = tau::build_wff_and(acc, safety_bodies[i]);
		d.safety_part = tau::build_wff_always(acc);
	}
	return d;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__LIVENESS_DECOMP_H__
