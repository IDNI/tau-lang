// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// First-class decomposed_spec { transient; invariant; reactive } pipeline type (#5).
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
// collecting G clauses, detecting temporal shapes).  `decomposed_spec` is the shared
// type for that decomposition, with a helper `decompose_spec` that does
// the split once.
//
// This header is additive: existing code paths continue to use `rr`.
//
// NOT YET WIRED into production (GR-N1, same status as gr1_detect.h,
// liveness_decomp.h and mealy_extract.h): at HEAD the only consumers of
// `decompose_spec` are unit tests.  The intended consumers (the direct
// game pipeline, GR(1) detection) would opt in by calling
// `decompose_spec(main)`; until one does, treat this as staged work.

#ifndef __IDNI__TAU__DECOMPOSED_SPEC_H__
#define __IDNI__TAU__DECOMPOSED_SPEC_H__

#include "gr1_detect.h"
#include "ltl_aba.h"
#include "tau_tree.h"

namespace idni::tau_lang {

template <NodeType node>
struct decomposed_spec {
	// Non-temporal constraints holding at t=0 only (initial conditions).
	// May be a null tref when the formula has no transient component.
	tref transient = nullptr;
	// G(safety) conjunction.  Null when the formula has no G clauses.
	tref invariant = nullptr;
	// Temporal response (F/U/R/W/GF/FG).  Null when no reactive content.
	tref reactive  = nullptr;
};

// Walk a top-level && chain classifying each conjunct as transient,
// invariant, or reactive; return the resulting decomposed_spec.  The conjuncts are
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
inline decomposed_spec<node> decompose_spec(tref main_fm) {
	using tau = tree<node>;
	decomposed_spec<node> s;
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
				// RR-10: wff_sometimes is F's canonical spelling
				// (the normalizer rewrites wff_F to it), so
				// G(sometimes phi) is the same GF reactive shape.
				// RR-11: only wff_sometimes can appear here --
				// the enclosing !has_ltl_operators(body) gate
				// already guarantees no wff_F exists, so the
				// old wff_F half of this scan was dead (the
				// sometimes half became live with RR-10).
				// GR-4 / GR-R1: A/E/`-phi` nest a path formula
				// and are not invariants either; the shared
				// predicate keeps the three classifiers in step.
				bool has_F = false;
				tau::get(body).find_top([&](tref n) {
					if (gr1_detect_internal::
						is_temporal_operator_node<node>(n))
						has_F = true;
					return false;
				});
				if (has_F) append(s.reactive);
				else       append(s.invariant);
			} else {
				append(s.reactive);
			}
			return;
		}
		if (nt == tau::wff_F || nt == tau::wff_sometimes
		 || nt == tau::wff_U
		 || nt == tau::wff_R || nt == tau::wff_W
		 || nt == tau::wff_S || nt == tau::wff_T
		 || nt == tau::wff_A || nt == tau::wff_E
		 || nt == tau::wff_semantic_neg) {
			// RR-10: wff_sometimes included -- a top-level
			// `sometimes phi` is a liveness obligation, not
			// transient.
			append(s.reactive);
			return;
		}
		// GR-N2: a compound conjunct with temporal content below a
		// Boolean top node — `F(a) || G(b)`, `!G(a)`, `G(a) -> F(b)` —
		// used to fall through to the transient default and be filed
		// as an initial-only constraint. Any temporal operator
		// anywhere inside makes it a temporal obligation: reactive.
		// (Shared predicate: the same one gr1_detect and the two
		// other classifiers use.)
		if (!gr1_detect_internal::is_non_temporal<node>(fm)) {
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

#endif // __IDNI__TAU__DECOMPOSED_SPEC_H__
