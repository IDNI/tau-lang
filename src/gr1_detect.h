// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Static classifier for the GR(1) fragment (Opt-3).
//
// A formula is in the GR(1) fragment (here, "simple GR(1)") when it is a
// conjunction of:
//   - zero or more G(φ_safe)     — safety invariants
//   - zero or more GF(φ_live)   — liveness (infinitely-often) conditions
//
// where each φ_safe, φ_live is non-temporal.  This covers the common
// request-grant / fairness / mutex-under-safety patterns and admits a
// polynomial solver via round-robin evaluation of the Pre-based fixpoints
// (see LTL_ABA_DESIGN.md Phase 3, GR(1) case: O(|T_1|² · k)).
//
// The classifier is AST-only; it does not touch the type spaces.

#ifndef __IDNI__TAU__GR1_DETECT_H__
#define __IDNI__TAU__GR1_DETECT_H__

#include "tau_tree.h"

namespace idni::tau_lang {

namespace gr1_detect_internal {

template <NodeType node>
inline bool is_non_temporal(tref fm) {
	using tau = tree<node>;
	if (!fm) return true;
	return tau::get(fm).find_top([](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.has_child()) return false;
		auto nt = t[0].value.nt;
		return nt == tau::wff_always  || nt == tau::wff_sometimes
		    || nt == tau::wff_F       || nt == tau::wff_U
		    || nt == tau::wff_R       || nt == tau::wff_W
		    || nt == tau::wff_S       || nt == tau::wff_T;
	}) == nullptr;
}

enum class Gr1Conjunct { Safety, Liveness, Other };

// Classify a single top-level conjunct.
template <NodeType node>
inline Gr1Conjunct classify_conjunct(tref fm) {
	using tau = tree<node>;
	if (!fm) return Gr1Conjunct::Other;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return Gr1Conjunct::Other;
	auto nt = t[0].value.nt;
	if (nt == tau::wff_always) {
		// G(φ).  If φ is non-temporal ⇒ Safety.
		// If φ is F(ψ) with non-temporal ψ ⇒ Liveness (GF).
		const auto& inner = t[0][0];
		if (!inner.has_child()) return Gr1Conjunct::Other;
		auto inner_nt = inner[0].value.nt;
		if (inner_nt == tau::wff_F) {
			const auto& ff = inner[0][0];
			if (is_non_temporal<node>(ff.get()))
				return Gr1Conjunct::Liveness;
			return Gr1Conjunct::Other;
		}
		if (is_non_temporal<node>(t[0].first()))
			return Gr1Conjunct::Safety;
		return Gr1Conjunct::Other;
	}
	return Gr1Conjunct::Other;
}

// Walk top-level && chain, classifying each leaf.  Supports wff_and with
// arbitrary arity — the parser may flatten A && B && C to a single 3-child
// wff_and or keep it left/right-associated; we handle both.
template <NodeType node>
inline bool is_gr1_impl(tref fm, int& n_safety, int& n_liveness) {
	using tau = tree<node>;
	if (!fm) return true;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return false;
	auto nt = t[0].value.nt;
	if (nt == tau::wff_and) {
		const auto& op = t[0];
		for (size_t i = 0; i < op.children_size(); ++i) {
			if (!is_gr1_impl<node>(op.child(i), n_safety, n_liveness))
				return false;
		}
		return true;
	}
	switch (classify_conjunct<node>(fm)) {
		case Gr1Conjunct::Safety:   ++n_safety;   return true;
		case Gr1Conjunct::Liveness: ++n_liveness; return true;
		default: return false;
	}
}

} // namespace gr1_detect_internal

// Returns true iff the formula is a conjunction of G(safe) and GF(live)
// with non-temporal bodies.  Writes the count of each to out-params.
template <NodeType node>
inline bool is_gr1_fragment(tref fm, int& n_safety, int& n_liveness) {
	n_safety = 0; n_liveness = 0;
	return gr1_detect_internal::is_gr1_impl<node>(fm, n_safety, n_liveness);
}

template <NodeType node>
inline bool is_gr1_fragment(tref fm) {
	int s = 0, l = 0;
	return is_gr1_fragment<node>(fm, s, l);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__GR1_DETECT_H__
