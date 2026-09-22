// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file subst.tmpl.h
 * @brief Template implementations for subst.h. Included by subst.h. subst.h
 * says what each function means; the comments here say how it is built.
 *
 * `[atm ↦ T/F]` IS the library's `rewriter::replace_if`: content match, the
 * `atoms` guard as its descent predicate, hooked rebuild. It needs no walk of
 * its own, because its replacement is a CONSTANT and the hooks fold it out of
 * a chain in place.
 *
 * SPINE nodes — a `wff` wrapping the same connective as its parent operator
 * node, on either nesting side — get no `atoms_memo` row, the rule
 * `get_free_vars` follows as well: a chain of k members has k spine nodes,
 * each with a set of size O(k). A spine node met later STANDALONE is a root
 * and does get its row, which is why the fill uses `visit` with row-based
 * pruning rather than `visit_unique`. `[atm ↦ T/F]` recognises a spine node
 * by the ABSENCE of a row, since the guard's fill gave every other wrapper in
 * the reach one.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_TMPL_H__

#include <algorithm>

#include "ctx.h"

namespace idni::tau_lang::anti_prenexing {

namespace subst_detail {

/// The operator nodes `[atm ↦ T/F]` and `atoms` descend through: the two
/// connectives and the negation, their common REACH (§4; units, references
/// and temporal operators are opaque).
template <NodeType node>
bool reach_structural(tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	return t.is(tau::wff_and) || t.is(tau::wff_or) || t.is(tau::wff_neg);
}

/// `n` is a SPINE node: a `wff` wrapping the same connective as its parent
/// operator node `parent`, on either nesting side, as `members` flattens.
template <NodeType node>
bool is_spine(tref n, tref parent) {
	using tau = tree<node>;
	if (!parent) return false;
	const auto& p = tau::get(parent);
	if (p.is(tau::wff_and)) return is_child<node>(n, tau::wff_and);
	if (p.is(tau::wff_or))  return is_child<node>(n, tau::wff_or);
	return false;
}

} // namespace subst_detail

// --- φ[atm ↦ T/F] ------------------------------------------------------------------

template <NodeType node>
tref subst_atom(tref phi, tref atm, bool value) {
	using tau = tree<node>;
	using namespace subst_detail;
	DBG(assert(phi != nullptr && atm != nullptr);)
	DBG(assert(is_atomic_fm<node>(atm));)
	DBG(assert(tau::get(phi).is(tau::wff));)
	// The guard's first call fills `atoms_memo` for every non-spine wrapper
	// in the reach below `phi`, so every later guard is a lookup plus a
	// binary search.
	if (!has_atom<node>(phi, atm)) return phi;
	const tref cst = value ? tau::_T() : tau::_F();
	// Descent. A wrapper with a row is entered iff its vocabulary holds
	// `atm` — one lookup, never a walk (§10). A wrapper WITHOUT a row is a
	// spine node of the chain being walked (the fill gave every other
	// wrapper in the reach one) and is entered unconditionally. Below a
	// wrapper only the reach is entered: ∧, ∨ and ¬ (§4). A unit, a
	// reference and a temporal operator have the empty vocabulary and stop
	// at their wrapper.
	auto reach = [&](tref m) -> bool {
		if (!tau::get(m).is(tau::wff)) return reach_structural<node>(m);
		const tref_set* row = find<node, table::atoms_memo>(m);
		return row == nullptr || std::binary_search(row->items.begin(),
			row->items.end(), atm, tau::subtree_less);
	};
	// `replace_if`: an occurrence becomes the constant, nothing below it is
	// entered, and every changed parent is rebuilt through the hooked
	// constructor, which folds the constant out of a chain or collapses the
	// chain, and folds `¬T`/`¬F`. A shared subtree is rewritten once (the
	// traversal's cache).
	return rewriter::replace_if<node>(phi, atm, cst, reach);
}

// --- atoms_memo ----------------------------------------------------------------------

template <NodeType node>
const trefs& atoms(tref n) {
	using tau = tree<node>;
	using namespace subst_detail;
	DBG(assert(n != nullptr && tau::get(n).is(tau::wff));)
	if (const tref_set* row = find<node, table::atoms_memo>(n); row)
		return row->items;
	// Post-order fill over the reach: a wrapper's row is computed in `up`
	// from its children's rows — a chain top from its full member view, which
	// never names a spine node — and stored at once. A spine node gets no
	// row, the queried root always does, and a wrapper that has a row already
	// is not entered.
	auto descend = [](tref m) -> bool {
		if (tau::get(m).is(tau::wff))
			return find<node, table::atoms_memo>(m) == nullptr;
		return reach_structural<node>(m);
	};
	auto visitor = [](tref) { return true; };
	auto up = [](tref m, tref parent) {
		const tau& tm = tau::get(m);
		if (!tm.is(tau::wff) || is_spine<node>(m, parent)) return;
		if (find<node, table::atoms_memo>(m)) return;
		trefs items;
		if (is_atomic_fm<node>(m)) items.push_back(tau::trim_right_sibling(m));
		else if (is_child<node>(m, tau::wff_and) || is_child<node>(m, tau::wff_or)) {
			// Every member is a non-spine wrapper of the reach, filled
			// before its chain top.
			subtree_set<node> merged;
			for (tref mm : members<node>(m)) {
				const tref_set* row = find<node, table::atoms_memo>(mm);
				DBG(assert(row != nullptr);)
				if (row) merged.insert(row->items.begin(), row->items.end());
			}
			items.assign(merged.begin(), merged.end());
		} else if (tm.child_is(tau::wff_neg)) {
			const tref_set* row =
				find<node, table::atoms_memo>(tau::trim2(m));
			DBG(assert(row != nullptr);)
			if (row) items = row->items;
		}
		// A binder, a reference, a temporal operator, a constant, anything
		// else: opaque, with the empty vocabulary (§4).
		store<node, table::atoms_memo>(m, tref_set{ std::move(items) });
	};
	pre_order<node>(n).visit(visitor, descend, up);
	const tref_set* row = find<node, table::atoms_memo>(n);
	DBG(assert(row != nullptr);)
	return row->items;
}

template <NodeType node>
bool has_atom(tref n, tref atm) {
	const trefs& as = atoms<node>(n);
	return std::binary_search(as.begin(), as.end(), atm,
		tree<node>::subtree_less);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_TMPL_H__
