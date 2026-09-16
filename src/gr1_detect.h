// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Static classifier for the GR(1) fragment (Opt-3).
//
// LG-18 status: NOT YET WIRED into production -- the solve_ltl_aba dispatch
// only handles TAU_LTL_ALG in {A, B, D}; the only current consumers of this
// header are unit tests. Staged work.
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

/// @brief The one temporal-operator predicate shared by the three classifiers
/// (gr1_detect, liveness_decomp, spec.h).  GR-R1/GR-4: the CTL* quantifiers
/// A/E and the semantic negation `-phi` are temporal in every sense that
/// matters here (they nest a path formula), so G(A phi) is NOT a safety
/// invariant.
template <NodeType node>
inline bool is_temporal_operator_node(tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	if (!t.has_child()) return false;
	auto nt = t[0].value.nt;
	return nt == tau::wff_always  || nt == tau::wff_sometimes
	    || nt == tau::wff_until       || nt == tau::wff_release
	    || nt == tau::wff_weak_until       || nt == tau::wff_since
	    || nt == tau::wff_trigger       || nt == tau::wff_A
	    || nt == tau::wff_E       || nt == tau::wff_semantic_neg;
}

/// @brief True iff `n` is the eventually operator.
template <NodeType node>
inline bool is_eventually_node(tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	if (!t.has_child()) return false;
	auto nt = t[0].value.nt;
	return nt == tau::wff_sometimes;
}

/// @brief True iff no temporal operator node occurs in `fm` (a null tref
/// counts as non-temporal).
template <NodeType node>
inline bool is_non_temporal(tref fm) {
	using tau = tree<node>;
	if (!fm) return true;
	return tau::get(fm).find_top([](tref n) {
		return is_temporal_operator_node<node>(n);
	}) == nullptr;
}

/// @brief Kind of a top-level conjunct: G(safe), GF(live) or anything else.
enum class gr1_conjunct { Safety, Liveness, Other };

/// @brief Classify a single top-level conjunct.
template <NodeType node>
inline gr1_conjunct classify_conjunct(tref fm) {
	using tau = tree<node>;
	if (!fm) return gr1_conjunct::Other;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return gr1_conjunct::Other;
	auto nt = t[0].value.nt;
	if (nt == tau::wff_always) {
		// G(φ).  If φ is non-temporal ⇒ Safety.
		// If φ is F(ψ) with non-temporal ψ ⇒ Liveness (GF).
		const auto& inner = t[0][0];
		if (!inner.has_child()) return gr1_conjunct::Other;
		if (is_eventually_node<node>(t[0].first())) {
			const auto& ff = inner[0][0];
			if (is_non_temporal<node>(ff.get()))
				return gr1_conjunct::Liveness;
			return gr1_conjunct::Other;
		}
		if (is_non_temporal<node>(t[0].first()))
			return gr1_conjunct::Safety;
		return gr1_conjunct::Other;
	}
	return gr1_conjunct::Other;
}

/// @brief Walk top-level && chain, classifying each leaf.  Supports wff_and
/// with arbitrary arity -- the parser may flatten A && B && C to a single
/// 3-child wff_and or keep it left/right-associated; we handle both.
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
		case gr1_conjunct::Safety:   ++n_safety;   return true;
		case gr1_conjunct::Liveness: ++n_liveness; return true;
		default: return false;
	}
}

} // namespace gr1_detect_internal

/**
 * @brief Returns true iff the formula is a conjunction of G(safe) and
 * GF(live) with non-temporal bodies.
 *
 * Writes the count of each to out-params.
 * @tparam node Tree node type.
 * @param fm Formula to classify.
 * @param n_safety Receives the number of G(safe) conjuncts.
 * @param n_liveness Receives the number of GF(live) conjuncts.
 * @return `true` iff @p fm is in the simple GR(1) fragment.
 */
template <NodeType node>
inline bool is_gr1_fragment(tref fm, int& n_safety, int& n_liveness) {
	n_safety = 0; n_liveness = 0;
	return gr1_detect_internal::is_gr1_impl<node>(fm, n_safety, n_liveness);
}

/**
 * @brief Same as the three-argument overload, discarding the counts.
 * @tparam node Tree node type.
 * @param fm Formula to classify.
 * @return `true` iff @p fm is in the simple GR(1) fragment.
 */
template <NodeType node>
inline bool is_gr1_fragment(tref fm) {
	int s = 0, l = 0;
	return is_gr1_fragment<node>(fm, s, l);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__GR1_DETECT_H__
