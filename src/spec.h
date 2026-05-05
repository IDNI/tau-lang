// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// First-class Spec { transient; invariant; reactive } pipeline type (#5).
//
// A tau LTL(ABA) specification decomposes cleanly into three pieces:
//
//   - transient:  initial-only constraints (values at t = 0 or near the
//                 initial lookback window; do not propagate temporally).
//   - invariant:  G(safety) clauses that must hold at every time step.
//   - reactive:   temporal response obligations (F, U, R, W, GF, FG)
//                 that bind the system's choices to environment behaviour.
//
// The existing `rr<node>` carries recurrence relations and a single `main`
// formula — it does not surface this decomposition.  Many parts of the
// synthesis pipeline effectively re-derive it (splitting conjuncts,
// collecting G clauses, detecting temporal shapes).  `Spec` is the shared
// type for that decomposition, with a helper `decompose_spec` that does
// the split once.
//
// This header is additive: existing code paths continue to use `rr`; new
// code (Algorithms A/B/C/D, GR(1) detection, etc.) opt in by calling
// `decompose_spec(main)` to get the three components explicitly.

#ifndef __IDNI__TAU__SPEC_H__
#define __IDNI__TAU__SPEC_H__

#include "ltl_aba.h"
#include "tau_tree.h"

namespace idni::tau_lang {

template <NodeType node>
struct Spec {
	// Non-temporal constraints holding at t=0 only (initial conditions).
	// May be a null tref when the formula has no transient component.
	tref transient = nullptr;
	// G(safety) conjunction.  Null when the formula has no G clauses.
	tref invariant = nullptr;
	// Temporal response (F/U/R/W/GF/FG).  Null when no reactive content.
	tref reactive  = nullptr;
};

// Walk a top-level && chain classifying each conjunct as transient,
// invariant, or reactive; return the resulting Spec.  The conjuncts are
// joined back up with wff_and within each category.
//
// Classification rules (conservative):
//   - wff_always(φ)       → invariant component if φ is non-temporal,
//                           else reactive (G(F(...)) etc.).
//   - wff_F / U / R / W   → reactive.
//   - wff_S / T           → reactive (past-LTL, compiled via S/T pass).
//   - non-temporal        → transient (hold at t=0; their G-lift, if any,
//                           is handled by the caller's initial-conditions
//                           logic).
template <NodeType node>
inline Spec<node> decompose_spec(tref main_fm) {
	using tau = tree<node>;
	Spec<node> s;
	if (!main_fm) return s;

	std::function<void(tref)> visit = [&](tref fm) {
		if (!fm) return;
		const auto& t = tau::get(fm);
		if (!t.has_child()) { return; }
		auto nt = t[0].value.nt;

		// Flatten top-level wff_and conjuncts.
		if (nt == tau::wff_and) {
			const auto& op = t[0];
			for (size_t i = 0; i < op.children_size(); ++i)
				visit(op.child(i));
			return;
		}

		// Classify.
		auto append = [&](tref& slot) {
			slot = slot ? tau::build_wff_and(slot, fm) : fm;
		};

		if (nt == tau::wff_always) {
			// G(body).  If body has no temporal, it's an invariant; else
			// it's reactive (e.g., G(F(...))).
			tref body = t[0].first();
			if (!has_ltl_operators<node>(body)) {
				// Pure-safety G.  Also filter out G(F(...)) = GF pattern:
				// F inside G is a reactive liveness, not invariant.
				bool has_F = false;
				tau::get(body).find_top([&](tref n) {
					const auto& nt_node = tree<node>::get(n);
					if (nt_node.has_child()
					    && nt_node[0].value.nt == tau::wff_F) {
						has_F = true;
					}
					return false;
				});
				if (has_F) append(s.reactive);
				else       append(s.invariant);
			} else {
				append(s.reactive);
			}
			return;
		}
		if (nt == tau::wff_F || nt == tau::wff_U
		 || nt == tau::wff_R || nt == tau::wff_W
		 || nt == tau::wff_S || nt == tau::wff_T
		 || nt == tau::wff_A || nt == tau::wff_E
		 || nt == tau::wff_semantic_neg) {
			append(s.reactive);
			return;
		}
		// Otherwise — non-temporal at the top level.  Transient.
		append(s.transient);
	};
	visit(main_fm);
	return s;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__SPEC_H__
