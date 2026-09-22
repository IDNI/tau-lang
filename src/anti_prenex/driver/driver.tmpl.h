// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file driver.tmpl.h
 * @brief Template implementations for driver.h. driver.h says what each
 * procedure means; the comments here say how it is built.
 *
 * The pieces come from the files below: `binder_kind`, `binder_var`,
 * `binder_body` and `members` (foundations/dag.h) with `trim_right_sibling`
 * (src/tau_tree.h) for the walk down a binder chain,
 * `pre_order::apply_unique_pure` (the parser's tree.h) for the pass itself,
 * `simplified_and_join` / `simplified_or_join` (normalisers/joins.h) for
 * folding a result into the context it stood in, and `process_block`
 * (block.h) for the elimination.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__DRIVER__DRIVER_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__DRIVER__DRIVER_TMPL_H__

#include <cassert>

namespace idni::tau_lang::anti_prenexing {

namespace driver_detail {

/**
 * @brief §4 `PROCESS_NODE`'s two mutually recursive halves with the state
 * they share: the caller's callback and the memo of run heads.
 *
 * ONE MEMO FOR THE WHOLE PASS, across every walk it starts. Its keys are RUN
 * HEADS, the only costly nodes; a junction re-join is cheap and stays with
 * the walk that made it, whose own per-call memo holds it. A head's entry is
 * exact — the result is a pure function of the head and of `kf` — and the
 * garbage collector is suspended while a traversal runs, so the stored nodes
 * stay alive for the pass.
 *
 * The recursion goes as deep as runs NEST, not as deep as the tree is.
 */
template <NodeType node>
struct pass {
	const keep_functional_fn<node>& kf;
	subtree_unordered_map<node, tref> memo{};

	/// One walk over @p n that stops above every binder, so that a run is
	/// met as ONE unit; the walk rebuilds everything else over its
	/// processed children.
	tref process_node(tref n);
	/// The run from the head @p h, over a matrix a nested walk has made
	/// final first.
	tref process_run(tref h);
};

template <NodeType node>
tref pass<node>::process_node(tref n) {
	using tau = tree<node>;
	// Nothing happens on the way down: the pass is post-order.
	auto down = [](tref m) { return m; };
	// The walk enters ∧, ∨, ¬, references — whose arguments are terms, so
	// there is nothing in one to process — and temporal operators. It
	// stops at a TERM (§4) and at a binder's own `wff_ex` / `wff_all`
	// node, which leaves the head's `wff` wrapper entered and the binder's
	// body untouched.
	//
	// Stopping one level INSIDE the head, rather than at it, is what makes
	// the head's `up` fire: the traversal calls `up` on every node it
	// applied `down` to, and a node this predicate turns away gets
	// neither. Stopping at the head instead would hand it back unlooked
	// at.
	auto guard = [](tref m) {
		return while_is_formula<node>(m) && !is_quantifier<node>(m);
	};
	auto up = [this](tref r) -> tref {
		// The whole run, collected and eliminated as one unit.
		if (is_child_quantifier<node>(r)) return process_run(r);
		// A result folded into its context through the joins
		// (invariant 6): a chain of members it brought with it is
		// flattened and the result comes back canonical, which is what
		// keeps the pass an identity where nothing changed.
		if (is_child<node>(r, tau::wff_and))
			return simplified_and_join<node>(members<node>(r));
		if (is_child<node>(r, tau::wff_or))
			return simplified_or_join<node>(members<node>(r));
		return r;
	};
	return pre_order<node>(n).apply_unique_pure(down, guard, up);
}

template <NodeType node>
tref pass<node>::process_run(tref h) {
	if (const auto it = memo.find(h); it != memo.end()) return it->second;
	run<node> blk = collect_run<node>(h);
	// POST-ORDER: the matrix is final before the run is eliminated over
	// it. A NESTED walk, because it starts below the head — which is also
	// why an intermediate binder of the run is never seen as a head, the
	// walk above having stopped at the head and this one starting under
	// the whole chain.
	blk.matrix = process_node(blk.matrix);
	const tref r = process_block<node>(blk, kf);
	memo.emplace(h, r);
	return r;
}

} // namespace driver_detail

// --- the run ----------------------------------------------------------------------

template <NodeType node>
run<node> collect_run(tref h) {
	using tau = tree<node>;
	DBG(assert(is_child_quantifier<node>(h));)
	run<node> blk;
	blk.kind = binder_kind<node>(h);
	tref n = h;
	// Down the chain while the kind holds; whatever stops the loop is the
	// matrix. A binder's variable and body carry their right siblings, so
	// both are trimmed on the way out.
	while (is_child_quantifier<node>(n)
		&& binder_kind<node>(n) == blk.kind)
	{
		blk.vars.push_back(
			tau::trim_right_sibling(binder_var<node>(n)));
		n = tau::trim_right_sibling(binder_body<node>(n));
	}
	blk.matrix = n;
	return blk;
}

// --- the pass ---------------------------------------------------------------------

template <NodeType node>
tref process_all_blocks(tref phi, const keep_functional_fn<node>& kf) {
	DBG(assert(phi != nullptr);)
	driver_detail::pass<node> p{ kf };
	return p.process_node(phi);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__DRIVER__DRIVER_TMPL_H__
