// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file atomless.tmpl.h
 * @brief Template implementations for atomless.h. atomless.h says what the
 * method means; the comments here say how it is built.
 *
 * Every piece comes from conditions.h — the freeze, the squeeze and the three
 * conditions — so what is written here is the spec's own step order and
 * nothing besides. The method's own contribution is `atomless_opaque`, the
 * predicate the freeze is parameterised by.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ATOMLESS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ATOMLESS_TMPL_H__

#include <algorithm>

namespace idni::tau_lang::anti_prenexing {

namespace atomless_detail {

/// §1 the LEAF HAZARD test, `LEAF_FV(t) ∩ X ≠ ∅`: one binary search per
/// `x ∈ X` over the sorted set `leaf_fv` hands back (terms.h). `fv_meets`
/// (dag.h) cannot serve, since it intersects a NODE's cached set and this
/// intersection is against a computed one.
template <NodeType node>
bool leaf_hides(tref t, const block& X) {
	const trefs ls = leaf_fv<node>(t);
	for (tref x : X)
		if (std::binary_search(ls.begin(), ls.end(), x,
			tree<node>::subtree_less)) return true;
	return false;
}

} // namespace atomless_detail

template <NodeType node>
bool atomless_opaque(tref conjunct, const block& X,
	const var_order<node>& order)
{
	using tau = tree<node>;
	using namespace atomless_detail;
	// A (¬)EQUATION, at either polarity: usable unless its term hides a
	// block variable in a leaf. An order atom is no equation and falls
	// through to the last line.
	if (is_child<node>(atom_of<node>(conjunct), tau::bf_eq))
		return leaf_hides<node>(term_of<node>(conjunct, order), X);
	if (is_negative_tree<node>(conjunct)) {
		// LITERAL-WISE: one walk down to the negated equations, each of
		// them once, stopping at the first hidden one. Every leaf of a
		// negative tree is a negated equation (§1), and the descent
		// stops at the terms, so nothing below a literal is entered.
		bool hidden = false;
		auto check = [&hidden, &X, &order](tref m) {
			if (!is_negated_equation<node>(m)) return true;
			if (!leaf_hides<node>(term_of<node>(m, order), X))
				return true;
			return hidden = true, false;
		};
		auto up = [](tref) {};
		pre_order<node>(conjunct).search_unique(check,
			while_is_formula<node>, up);
		return hidden;
	}
	// A reference, an order atom, EVERY binder unit, a temporal operator:
	// this engine answers no queries, so it can swallow none of them.
	return true;
}

template <NodeType node>
tref eliminate_atomless_clause(const trefs& conjuncts, const block& X,
	ctx<node>& c)
{
	using tau = tree<node>;
	// 1. FREEZE, by connected component: what is left is a clause this
	// engine can read through its terms alone.
	const freeze_result<node> fr = freeze_opaque_components<node>(conjuncts,
		X, [&X, &c](tref n) {
			return atomless_opaque<node>(n, X, c.order); });
	// Every conjunct mentions some `x ∈ X` (the method contract), so an
	// empty block means every component was frozen and `frozen` is the
	// whole clause, re-wrapped.
	if (fr.X.empty()) return fr.frozen;
	// 2. SQUEEZE, then the conditions. `O1` and `O2` inside
	// `NEGATIVE_CONDITION` reason under `pos` as a sibling assumption, so
	// `pos` is computed first and its `F` ends the clause.
	const squeeze_result<node> s = squeeze<node>(fr.conjuncts, fr.X,
		c.order);
	if (s.decided_false) return _F<node>();
	const tref pos = positive_condition<node>(s.comps, c);
	if (tau::get(pos).equals_F()) return _F<node>();
	// ONE CONDITION PER NEGATIVE is the other use of atomlessness
	// (invariant 1); each condition on its own holds in any Boolean
	// algebra. A condition that is `F` decides the clause, and the
	// following negatives are never computed (invariant 7).
	join_builder<node, true> neg;
	for (tref n : s.negatives)
		if (neg.insert(is_negative_tree<node>(n)
			? tree_condition<node>(n, s.comps, fr.X, c)
			: negative_condition<node>(term_of<node>(n, c.order),
				s.comps, fr.X, c)))
			return _F<node>();
	return simplified_and_join<node>(trefs{ fr.frozen, pos, neg.result() });
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ATOMLESS_TMPL_H__
