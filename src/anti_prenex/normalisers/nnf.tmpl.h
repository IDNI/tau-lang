// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file nnf.tmpl.h
 * @brief Template implementations for nnf.h (package N). Included by nnf.h.
 * nnf.h says what each function means; the comments here say how it is built.
 *
 * What is reused: `push_negation_one_in<node, true, false>` for the cases
 * that are one level of the existing push (`¬¬`, the binders, the temporal
 * operators, the sugar connectives) — its binder cases already pass
 * `calculate_quant_id = false` and its temporal cases already dualise, so
 * nothing here re-implements them; `normalize_atomic_formula_operators` for
 * `NORMALIZE_OPERATORS`; the package-J joins for every emission; `members`,
 * `neg_of` / `set_neg` from dag.h.
 *
 * What `down` builds is RAW: a `build_wff_or` / `build_wff_and` chain whose
 * members are still negations. The walk descends into it and normalises every
 * member, and `up` re-emits the finished junction through the join — together
 * exactly the spec's `SIMPLIFIED_OR_JOIN(NEG(mⱼ) : j)`, without a second pass
 * and without building the join's result before its members are in NNF.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__NORMALISERS__NNF_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__NORMALISERS__NNF_TMPL_H__

#include <cassert>
#include <utility>
#include <vector>

// What this pass reuses: `push_negation_one_in`
// (normal_forms_transformations.tmpl.h), which has no declaration of its own,
// `normalize_atomic_formula_operators` (normal_forms.tmpl.h, declared in
// heuristics/syntactic_path_simplification.tmpl.h) and the `MemorySlotPre`
// slots. `normal_forms.h` is the one header that assembles all three in the
// right order — terms.tmpl.h reaches its own two the same way.
#include "normal_forms.h"

namespace idni::tau_lang::anti_prenexing {

namespace detail {

/// The raw chain `down` hands to the walk: the members folded LEFT-NESTED
/// through the hooked binary builder, neither sorted nor deduplicated —
/// content order and deduplication are `up`'s join, once the members are
/// normalised. Every member here is freshly built, so none carries a right
/// sibling.
template <NodeType node, bool conj>
tref raw_chain(const trefs& ms) {
	DBG(assert(!ms.empty());)
	tref r = ms.front();
	for (size_t i = 1; i < ms.size(); ++i)
		r = conj ? build_wff_and<node>(r, ms[i])
			 : build_wff_or<node>(r, ms[i]);
	return r;
}

/// `¬m` for every member, in the member view's order.
template <NodeType node>
trefs negated_members(const trefs& ms) {
	trefs out;
	out.reserve(ms.size());
	for (tref m : ms) out.push_back(build_wff_neg<node>(m));
	return out;
}

/**
 * @brief §3 `NEG`'s FACTORED NEGATION of `⋁ⱼ mⱼ`, one level.
 *
 * `C = ⋂ⱼ parts(mⱼ)` over the members' conjuncts — the same PARTS the
 * ∨-join's absorption pass takes, so `parts_of<node, false>` (joins.tmpl.h)
 * is the one definition. `C = ∅` falls back to De Morgan, a `wff_and` chain
 * of the negated members. Otherwise
 *
 *     ¬⋁ⱼ(C ∧ restⱼ)  =  ⋁_{c∈C} ¬c  ∨  ¬⋁ⱼ restⱼ
 *
 * which holds in any BA: the residue is assembled through the joins right
 * here (its members are already simplified, invariant 6), and the outer
 * disjunction is raw, for the walk to negate member by member. A member equal
 * to `C` leaves the empty ∧, which is `T`, so the residue join decides `T`
 * and its `¬` folds to `F` through the hooks and drops out of the chain.
 */
template <NodeType node>
tref factored_neg_of_disjunction(const trefs& ms) {
	std::vector<trefs> parts;
	std::vector<subtree_unordered_set<node>> in_part;
	parts.reserve(ms.size());
	in_part.reserve(ms.size());
	for (tref m : ms) {
		trefs p = parts_of<node, false>(m);
		in_part.emplace_back(p.begin(), p.end());
		parts.push_back(std::move(p));
	}
	// `shared` doubles as C's membership test below and as the duplicate
	// guard here: a raw input member may spell the same conjunct twice.
	subtree_unordered_set<node> shared;
	trefs C;
	for (tref p : parts.front()) {
		if (shared.contains(p)) continue;
		bool in_all = true;
		for (size_t j = 1; j < ms.size() && in_all; ++j)
			in_all = in_part[j].contains(p);
		if (in_all) C.push_back(p), shared.insert(p);
	}
	if (C.empty())
		return raw_chain<node, true>(negated_members<node>(ms));
	trefs residues;
	residues.reserve(ms.size());
	for (size_t j = 0; j < ms.size(); ++j) {
		trefs rest;
		for (tref p : parts[j])
			if (!shared.contains(p)) rest.push_back(p);
		residues.push_back(simplified_and_join<node>(rest));
	}
	const tref rests = simplified_or_join<node>(residues);
	trefs out;
	out.reserve(C.size() + 1);
	for (tref c : C) out.push_back(build_wff_neg<node>(c));
	out.push_back(build_wff_neg<node>(rests));
	return raw_chain<node, false>(out);
}

/// §3 `TO_NNF`'s `down`: ONE negation rewritten at the node it stands on, the
/// result handed back for the walk to descend into. Every other node is
/// returned unchanged — the traversal itself rebuilds the binders and the
/// temporal operators over their normalised bodies.
template <NodeType node>
tref nnf_down(tref n) {
	using tau = tree<node>;
	if (!is_child<node>(n, tau::wff_neg)) return n;
	const tref psi = tau::trim2(n);
	// `neg(ψ)` IS the NNF of `¬ψ` (§1), so a hit ends this subtree: the
	// walk descends into a formula that is already normalised.
	if (const tref hit = neg_of<node>(psi); hit != nullptr) return hit;
	if (is_child<node>(psi, tau::wff_and))
		return raw_chain<node, false>(
			negated_members<node>(members<node>(psi)));
	if (is_child<node>(psi, tau::wff_or))
		return factored_neg_of_disjunction<node>(members<node>(psi));
	// The rest is one level of the existing push: `¬¬` folds, a binder
	// flips (never renaming — its builders pass `calculate_quant_id =
	// false`), `always` / `sometimes` dualise, a sugar connective
	// dualises. With `fuse_atoms = false` an ATOM comes back unchanged,
	// which is invariant 4: no `bf_neq`, no negated or mirrored order
	// operator is ever built here. A reference falls through the same way.
	return push_negation_one_in<node, true, false>(n);
}

/// §3 `TO_NNF`'s `up`: every `wff_and` / `wff_or` the walk closes re-emitted
/// through the matching join over its normalised members. Unconditional — see
/// nnf.h on why no marker says whether the node changed. Anything else is
/// returned as it stands, the term nodes included: `while_is_formula` stops
/// the descent at a term, and such a node still reaches `up`.
template <NodeType node>
tref nnf_up(tref r) {
	using tau = tree<node>;
	if (is_child<node>(r, tau::wff_and))
		return simplified_and_join<node>(members<node>(r));
	if (is_child<node>(r, tau::wff_or))
		return simplified_or_join<node>(members<node>(r));
	return r;
}

} // namespace detail

template <NodeType node>
tref to_canonically_factored_nnf(tref phi) {
	DBG(assert(phi != nullptr);)
	auto down = [](tref n) { return detail::nnf_down<node>(n); };
	auto up = [](tref n) { return detail::nnf_up<node>(n); };
	// The slot is this pass's own: `push_negation_in_m` would hand back
	// the fused normalisation of the other pass (invariant 4).
	return pre_order<node>(phi)
		.template apply_unique<MemorySlotPre::factored_nnf_m>(
			down, while_is_formula<node>, up);
}

template <NodeType node>
tref canonically_factored_neg(tref psi) {
	DBG(assert(psi != nullptr);)
	if (const tref hit = neg_of<node>(psi); hit != nullptr) return hit;
	// `build_wff_neg` folds `¬¬ψ′` and the two constants through the hooks
	// before the walk even starts; what the walk gets is the spec's
	// `TO_NNF(¬ψ)`, and this is the first DEMAND for `neg(ψ)`, so this is
	// where the slot is filled.
	const tref negated = to_canonically_factored_nnf<node>(
		build_wff_neg<node>(psi));
	set_neg<node>(psi, negated);
	return negated;
}

template <NodeType node>
tref normalize_operators(tref phi) {
	DBG(assert(phi != nullptr);)
	return normalize_atomic_formula_operators<node, true, true>(phi);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__NORMALISERS__NNF_TMPL_H__
