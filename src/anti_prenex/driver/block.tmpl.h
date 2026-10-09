// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file block.tmpl.h
 * @brief Template implementations for block.h. block.h says what each
 * procedure means; the comments here say how it is built.
 *
 * The pieces come from the files below: `union_find_with_sets`
 * (src/union_find_with_sets.h) for the partition, `is_atomic_fm` and
 * `while_is_formula` (src/tau_tree.h) for the one walk that feeds it,
 * `find_ba_type` (src/ba_types.h) and `ctx::for_component` (ctx.h) for a
 * component's setup, `push_block` (push/push_block.h) for the push itself,
 * `formula_size`, `fv_meets`, `fv_intersect` and `members`
 * (foundations/dag.h) with `simplified_and_join` (normalisers/joins.h) and
 * `rewrap` (foundations/prims.h) for the size acceptance, and
 * `canonically_factored_neg` with `holds_fused_operator` (normalisers/nnf.h)
 * for the dualisation.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__DRIVER__BLOCK_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__DRIVER__BLOCK_TMPL_H__

#include <algorithm>
#include <cassert>

#include "../../union_find_with_sets.h"

namespace idni::tau_lang::anti_prenexing {

// --- the partition ----------------------------------------------------------------

template <NodeType node>
std::vector<block> connected_components(const block& X, tref body) {
	using tau = tree<node>;
	// The comparator picks the root of a merged class and is the content
	// order (§1), so the roots are content-derived; it is held BY
	// REFERENCE inside the union-find and outlives every call made on it.
	auto root_less = [](tref l, tref r) { return tau::subtree_less(l, r); };
	union_find_with_sets<decltype(root_less), node> uf(root_less);
	// Every block variable is a class from the start, so one that no atom
	// mentions comes back below as a singleton.
	for (tref x : X) uf.insert(x);
	// ONE walk: `while_is_formula` enters ∧, ∨, ¬, binder units and
	// temporal operators and stops at terms, so an atom under a nested
	// binder connects like any other. A reference is entered and holds no
	// atom — `is_atomic_fm` rejects it and its arguments are terms — so it
	// merges nothing without a special case. An atom has nothing below it
	// to find, hence the `false`.
	auto merge_atom = [&uf, &X](tref n) {
		if (!is_atomic_fm<node>(n)) return true;
		// One intersection of cached sets per atom (§1), never a
		// descent.
		const block vs = fv_intersect<node>(n, X);
		for (size_t i = 1; i < vs.size(); ++i) uf.merge(vs[0], vs[i]);
		return false;
	};
	auto up = [](tref) {};
	pre_order<node>(body).visit_unique(merge_atom, while_is_formula<node>,
		up);
	// `X` in ITS OWN order, one `find` per variable: each variable is
	// appended to the component of its class, and a component opens on a
	// root seen first — so members and components both come out in `X`'s
	// order. The union-find's `get_set` hands a class over directly, but
	// in its ring's order, which would have to be sorted back into `X`'s.
	std::vector<block> ps;
	subtree_unordered_map<node, size_t> of_root;
	for (tref x : X) {
		auto [it, fresh] = of_root.try_emplace(uf.find(x), ps.size());
		if (fresh) ps.push_back(block{});
		// Trimmed, like `fv_intersect`'s own result: a block variable
		// read off a binder carries the body as its right sibling.
		ps[it->second].push_back(tau::trim_right_sibling(x));
	}
	return ps;
}

// --- the ∃ push over the components -----------------------------------------------

template <NodeType node>
tref push_ex_block(tref body, const block& X,
	const keep_functional_fn<node>& kf)
{
	using tau = tree<node>;
	// A variable the body does not mention binds nothing: no component is
	// set up for it, and the callback's node carries the narrowed block.
	const block Xc = fv_intersect<node>(body, X);
	if (Xc.empty()) return body;
	for (const block& P : connected_components<node>(Xc, body)) {
		// The callback, per component, on the ∃ wrap of the whole
		// narrowed block over the body AS IT STANDS — a throwaway
		// node, never a memo key.
		const bool keep = kf(rewrap<node>(body, Xc));
		// `for_component` is §5's whole ctx setup: `prio` over `P` and
		// the knobs. The type is the component's by invariant 2. The
		// callback rides along for the chains `DISCHARGE` emits.
		ctx<node> c = ctx<node>::for_component(P,
			find_ba_type<node>(P[0]), keep);
		c.kf = kf;
		const tref prev = body;
		body = push_block<node>(body, P, c);
		// SIZE ACCEPTANCE: a result cheap to build and ruinous to
		// carry is discarded for its own input, re-wrapped (§5,
		// inv. 3). Both sizes are cached, so the comparison is O(1).
		if (formula_size<node>(body) > std::max(
			c.accept_growth * formula_size<node>(prev),
			c.accept_floor))
		{
			// `∃P(dep ∧ indep) = ∃P.dep ∧ indep`, the push's own
			// strip, so the reject withholds nothing a later
			// component could still push. The member view is
			// taken for an ∧-node alone — a formula that is an
			// ∨-node is ONE conjunct.
			const trefs ms = is_child<node>(prev, tau::wff_and)
				? members<node>(prev) : trefs{ prev };
			trefs dep, indep;
			for (tref m : ms)
				(fv_meets<node>(m, P) ? dep : indep)
					.push_back(m);
			const tref dj = simplified_and_join<node>(dep);
			indep.push_back(rewrap<node>(dj,
				fv_intersect<node>(dj, P)));
			body = simplified_and_join<node>(indep);
		}
		// The close does nothing to terms: every chain was decided at
		// its emission (§7 `DISCHARGE`), and the spelling waits for
		// phase 5 (§3 `FINISH_TERMS`).
	}
	return body;
}

// --- the run --------------------------------------------------------------------

template <NodeType node>
tref process_block(const run<node>& blk, const keep_functional_fn<node>& kf) {
	if (blk.kind != tau_term_bdd<node>::all)
		return push_ex_block<node>(blk.matrix, blk.vars, kf);
	const tref body = canonically_factored_neg<node>(blk.matrix);
	DBG(assert(!holds_fused_operator<node>(body));)
	const tref r = push_ex_block<node>(body, blk.vars, kf);
	const tref negated = canonically_factored_neg<node>(r);
	DBG(assert(!holds_fused_operator<node>(negated));)
	return negated;
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__DRIVER__BLOCK_TMPL_H__
