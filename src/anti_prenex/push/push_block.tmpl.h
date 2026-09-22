// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file push_block.tmpl.h
 * @brief Template implementations for push_block.h. push_block.h says what
 * each procedure means; the comments here say how it is built.
 *
 * The pieces come from the files below: `fv_intersect` / `fv_meets` and the
 * shape predicates (dag.h) for the key canonicalisation and the dispatch,
 * `rewrap` (prims.h) for the key and for every graceful exit,
 * `simplified_and_join` (normalisers/joins.h) for the strip's two joins,
 * `memoised` (ctx.h) for the table, and `eliminate_block`
 * (eliminate/eliminate_block.h) for the leaves.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__PUSH__PUSH_BLOCK_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__PUSH__PUSH_BLOCK_TMPL_H__

#include <cassert>
#include <utility>

namespace idni::tau_lang::anti_prenexing {

// --- the wrapper ------------------------------------------------------------------

template <NodeType node>
tref push_block(tref phi, const block& X, ctx<node>& c) {
	using tau = tree<node>;
	DBG(assert(phi != nullptr);)
	// Nothing enters the key that `∃X` does not scope. `T` and `F` leave
	// here too: their free-variable set is empty.
	const block Xc = fv_intersect<node>(phi, X);
	if (Xc.empty()) return phi;
	// THE STRIP, one cached free-variable test per member:
	// `∃X(indep ∧ dep) = indep ∧ ∃X.dep`. The member view is taken for an
	// ∧-node alone — a formula that is an ∨-node is ONE conjunct.
	if (is_child<node>(phi, tau::wff_and)) {
		trefs indep, dep;
		for (tref m : members<node>(phi))
			(fv_meets<node>(m, Xc) ? dep : indep).push_back(m);
		if (!indep.empty()) {
			// `dep` is non-empty, every variable of `Xc` occurring
			// in some member; the recursive call narrows `Xc`
			// again, to what those members hold.
			indep.push_back(push_block<node>(
				simplified_and_join<node>(dep), Xc, c));
			return simplified_and_join<node>(indep);
		}
	}
	// The wrap node carries the ordered `X`, so formula and block are one
	// key part; the flag joins it because the entry embeds `DISCHARGE`'s
	// output, which the flag switches (§1).
	const std::pair<tref, bool> key{ rewrap<node>(phi, Xc),
		c.keep_functional };
	// The wrapper carries §1's taint rule, and is a plain computation
	// wherever the table does not exist.
	return memoised<node, table::push_memo>(key, [&] {
		return push_block_uncached<node>(phi, Xc, c); });
}

// --- the dispatch -----------------------------------------------------------------

template <NodeType node>
tref push_block_uncached(tref phi, const block& X, ctx<node>& c) {
	using tau = tree<node>;
	DBG(assert(!X.empty());)
	// A negative tree is an ∨-node, so it is classified ahead of the
	// junctions; the test is cached (§1).
	if (is_negative_tree<node>(phi)) return eliminate_block<node>(phi, X, c);
	// The push over a junction is not part of the module; the block
	// re-wraps, whatever the junction's shape (invariant 3).
	if (is_child<node>(phi, tau::wff_or)
		|| is_child<node>(phi, tau::wff_and))
			return rewrap<node>(phi, X);
	// A one-literal or one-unit clause, which the leaf reads as its own
	// one-member view (§7).
	if (is_literal<node>(phi) || is_child_quantifier<node>(phi))
		return eliminate_block<node>(phi, X, c);
	// A reference, a temporal operator, or `¬` over one: nothing here
	// reads into it, so this is as far as the block goes (invariant 3).
	return rewrap<node>(phi, X);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__PUSH_BLOCK_TMPL_H__
