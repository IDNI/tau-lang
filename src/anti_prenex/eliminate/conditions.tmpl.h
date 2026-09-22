// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file conditions.tmpl.h
 * @brief Template implementations for conditions.h. conditions.h says what
 * each procedure means; the comments here say how it is built.
 *
 * The pieces come from the files below: `parts` (§6) for every grouping,
 * `norm_equation`, `term_of`, `simplify_term` and `simplify_atom` (§3) for the
 * rewrite, `discharge` (§7) for every quantified atom that goes out, the
 * result joins (§3) for every formula, and `rewrap` (§3) for what is frozen.
 * The union of a list of terms is the hooked binary `build_bf_or`, which the
 * term simplifier then absorbs into the representation.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__CONDITIONS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__CONDITIONS_TMPL_H__

#include <cassert>
#include <utility>
#include <vector>

namespace idni::tau_lang::anti_prenexing {

namespace conditions_detail {

/// The BA type of the block, read off its first variable: a block is
/// type-homogeneous (invariant 2), and `SQUEEZE` has no ctx to read
/// `ctx.type` from. The id is needed for the empty union alone.
template <NodeType node>
ba_type_id block_type(const block& X) {
	return X.empty() ? 0 : find_ba_type<node>(X.front());
}

/**
 * @brief §7 `SQUEEZE` step 1 on ONE conjunct: the zero form.
 *
 * A (¬)equation goes through `NORM_EQUATION` and is simplified again —
 * `TERM_OF`'s ring sum makes leaves the path simplifier has not seen, and
 * invariant 6 wants every atom simplified. A NEGATIVE TREE is rewritten at its
 * LITERALS, by a content map handed to the library's `replace`: the
 * replacements sit at the leaves, so the tree's own `∧`/`∨` structure is
 * rebuilt as it stands and is not re-simplified. Anything else comes back
 * untouched.
 */
template <NodeType node>
tref zero_form(tref c, const var_order<node>& order) {
	using tau = tree<node>;
	if (is_child<node>(c, tau::bf_eq) || is_negated_equation<node>(c))
		return simplify_atom<node>(norm_equation<node>(c, order), order);
	if (!is_negative_tree<node>(c)) return c;
	// One walk down to the literals, each of them once: the map is keyed by
	// content, and every leaf of a negative tree is a negated equation (§1),
	// so nothing below a literal has to be entered.
	subtree_map<node, tref> zero;
	auto collect = [&zero, &order](tref m) {
		if (!is_negated_equation<node>(m)) return true;
		const tref z = simplify_atom<node>(
			norm_equation<node>(m, order), order);
		if (!tree<node>::subtree_equals(z, m))
			zero.emplace(tree<node>::trim_right_sibling(m), z);
		return false;
	};
	auto up = [](tref) {};
	pre_order<node>(c).visit_unique(collect, while_is_formula<node>, up);
	// Every literal was in zero form already: the tree is its own rewrite.
	if (zero.empty()) return c;
	return rewriter::replace<node>(c, zero);
}

} // namespace conditions_detail

// --- the freeze -------------------------------------------------------------------

template <NodeType node, typename Opaque>
freeze_result<node> freeze_opaque_components(const trefs& conjuncts,
	const block& X, Opaque&& opaque)
{
	using tau = tree<node>;
	// Every conjunct touches `X` (§7 method contract), so the grouping is
	// total and each conjunct lands in exactly one part.
	const std::vector<part<node>> ps = parts<node>(conjuncts, X);
	subtree_unordered_set<node> frozen_conjuncts, frozen_vars;
	for (const part<node>& p : ps) {
		bool any = false;
		for (tref c : p.conjuncts)
			if (opaque(c)) { any = true; break; }
		if (!any) continue;
		for (tref c : p.conjuncts) frozen_conjuncts.insert(c);
		for (tref x : p.vars) frozen_vars.insert(x);
	}
	if (frozen_conjuncts.empty())
		return freeze_result<node>{ _T<node>(), conjuncts, X };
	// The two splits, each keeping its input's order: the conjuncts by the
	// parts they belong to, `X` by `Xf`, the union of those parts' `vars`.
	// A part's `vars` is already `X ∩ FV`, so `Xf` is `X ∩ FV(⋀O)`.
	trefs frozen, rest;
	for (tref c : conjuncts)
		(frozen_conjuncts.contains(c) ? frozen : rest).push_back(c);
	block Xf, left;
	for (tref x : X)
		(frozen_vars.contains(x) ? Xf : left).push_back(
			tau::trim_right_sibling(x));
	// RE-WRAPPED, never decided (invariant 3). The BDD-backed terms inside
	// stay as they are; the component's finish spells them out under the
	// binder.
	return freeze_result<node>{
		rewrap<node>(simplified_and_join<node>(frozen), Xf),
		std::move(rest), std::move(left) };
}

// --- the squeeze ------------------------------------------------------------------

template <NodeType node>
squeeze_result<node> squeeze(const trefs& conjuncts, const block& X,
	const var_order<node>& order)
{
	using tau = tree<node>;
	using namespace conditions_detail;
	squeeze_result<node> r;
	// STEP 1 — the zero form, conjunct by conjunct. An atom that folds to
	// `T` says nothing and is dropped; one that folds to `F` decides the
	// clause, and nothing after it is computed.
	r.conjuncts.reserve(conjuncts.size());
	for (tref c : conjuncts) {
		const tref n = zero_form<node>(c, order);
		if (tau::get(n).equals_T()) continue;
		if (tau::get(n).equals_F())
			return r.conjuncts.clear(), r.decided_false = true, r;
		r.conjuncts.push_back(n);
	}
	// STEP 2 — the positives squeezed per component, the negatives read off
	// in the clause's order.
	trefs positives;
	for (tref c : r.conjuncts) {
		if (is_child<node>(c, tau::bf_eq)) positives.push_back(c);
		else if (is_negated_equation<node>(c)
			|| is_negative_tree<node>(c)) r.negatives.push_back(c);
	}
	const ba_type_id type = block_type<node>(X);
	std::vector<part<node>> ps = parts<node>(positives, X);
	trefs terms;
	r.comps.reserve(ps.size());
	for (part<node>& p : ps) {
		trefs part_terms;
		part_terms.reserve(p.conjuncts.size());
		for (tref a : p.conjuncts)
			part_terms.push_back(term_of<node>(a, order));
		const tref F = simplify_term<node>(
			build_bf_or<node>(part_terms, type), order);
		terms.push_back(F);
		r.comps.push_back(component<node>{ F, std::move(p.vars) });
	}
	// `f = ⋃_k F_k` over pairwise disjoint `X_k`, the whole positive part.
	r.f = simplify_term<node>(build_bf_or<node>(terms, type), order);
	return r;
}

// --- the three conditions ---------------------------------------------------------

template <NodeType node>
tref positive_condition(const std::vector<component<node>>& comps, ctx<node>& c)
{
	// One discharged atom per component, into one join: an empty `comps`
	// leaves the empty ∧-join, which is `T`, and a component with no common
	// zero decides `F` before the next one is discharged (invariant 7).
	join_builder<node, true> acc;
	for (const component<node>& k : comps)
		if (acc.insert(discharge<node>(k.F, false, k.vars, c))) break;
	return acc.result();
}

template <NodeType node>
tref negative_condition(tref g, const std::vector<component<node>>& comps,
	const block& X, ctx<node>& c)
{
	using tau = tree<node>;
	using namespace conditions_detail;
	DBG(assert(g != nullptr);)
	// `∃_X f′·g = (∀_X f)′ ≠ 0` under `pos`, whatever `X` is.
	if (tau::get(g).equals_1()) return _T<node>();
	// `K`, by the cached free-variable sets alone: `X_k ∩ FV(g) ≠ ∅`.
	trefs FK;
	subtree_unordered_set<node> vars;
	for (const component<node>& k : comps) {
		if (!fv_meets<node>(g, k.vars)) continue;
		FK.push_back(k.F);
		for (tref x : k.vars) vars.insert(x);
	}
	const block Xg = fv_intersect<node>(g, X);
	// No component touches `g`: every factor of the product is 1 under
	// `pos`, so `g` is discharged on its own. An `X`-free `g` lands here
	// with an empty `X_g` and emits `¬(g = 0)` itself — under §1's term
	// representation a BDD-backed `g` is canonical over the block, so
	// "independent of `X`" IS `FV(g) ∩ X = ∅`.
	if (FK.empty()) return discharge<node>(g, true, Xg, c);
	for (tref x : Xg) vars.insert(x);
	block vs;
	for (tref x : X)
		if (vars.contains(x)) vs.push_back(tau::trim_right_sibling(x));
	const tref F_K = simplify_term<node>(build_bf_or<node>(FK, c.type),
		c.order);
	const tref h = simplify_term<node>(
		build_bf_and<node>(build_bf_neg<node>(F_K), g), c.order);
	return discharge<node>(h, true, vs, c);
}

template <NodeType node>
tref tree_condition(tref t, const std::vector<component<node>>& comps,
	const block& X, ctx<node>& c)
{
	using tau = tree<node>;
	DBG(assert(is_negative_tree<node>(t));)
	// Nothing is rewritten on the way down: the walk is there for its
	// post-order callback, which sees every node over its finished
	// children.
	auto down = [](tref n) { return n; };
	auto up = [&comps, &X, &c](tref n) -> tref {
		if (is_child<node>(n, tau::wff_or))
			return simplified_or_join<node>(members<node>(n));
		if (is_child<node>(n, tau::wff_and))
			return simplified_and_join<node>(members<node>(n));
		if (!is_negated_equation<node>(n)) return n;
		return negative_condition<node>(term_of<node>(n, c.order),
			comps, X, c);
	};
	// The memo is the walk's own, local to this call, which is what makes
	// it exact: `comps` and `X` are fixed here and nowhere else. `up`
	// answers by the node alone, so the memo holds ITS result and a
	// repeated subtree is handed that result without being entered again.
	// The descent stops at the terms.
	return pre_order<node>(t).apply_unique_pure(down,
		while_is_formula<node>, up);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__CONDITIONS_TMPL_H__
