// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file anti_prenex.tmpl.h
 * @brief Template implementation for anti_prenex.h: §3 `ANTI_PRENEX`, the
 * pipeline's phases in the one order the spec fixes. anti_prenex.h says what
 * the entry means; the comments here say how it is built.
 *
 * Every phase is ONE call into the module below it —
 * `canonicalise_binder_ids`, `resolve_functional_quantifiers`,
 * `fold_degenerate_binders` (foundations), `to_canonically_factored_nnf`,
 * `simplify`, `normalize_operators` (normalisers) and
 * `eliminate_by_substitution` (witness) — and this file adds nothing of its
 * own but the order. PHASE ORDER IS FIXED AS GIVEN (§3): phase 2 runs BEFORE
 * phase 3, so the deep pass still meets a `bf_neq` in either spelling, which
 * is why it reads both (witness.h).
 *
 * PHASE 4 IS THE IDENTITY: `PROCESS_ALL_BLOCKS` — the push, the elimination
 * and everything the `ctx` carries — is not called here, and the pipeline
 * runs end to end around it with every other phase the spec's.
 *
 * THE PLAIN REGIME THROUGHOUT: phases 1, 2 and 5 pass the EMPTY order, where
 * nothing is BDD-backed (§3) — `simplify`'s propagation guard is vacuous and
 * `resolve_functional_quantifiers` has no live order to quantify under. A
 * live order exists only inside a component (§5).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_TMPL_H__

#include <cassert>

namespace idni::tau_lang::anti_prenexing {

namespace anti_prenex_detail {

/// Does `phi` carry a QUANTIFIER anywhere — a formula binder (`wff_ex` /
/// `wff_all`) or a term-level FUNCTIONAL one (`bf_fex` / `bf_fall`)? §3's
/// entry test names both: phase 0 canonicalises a chain's subscripts and
/// phase 5 resolves its chains, so a formula whose only quantifier is a chain
/// still has work waiting. One `find_top` search, which enters everything —
/// a quantifier under a temporal operator or inside a reference argument
/// counts.
template <NodeType node>
bool carries_quantifier(tref phi) {
	using tau = tree<node>;
	return tau::get(phi).find_top([](tref n) {
		return is_child_quantifier<node>(n)
			|| is_functional_quantifier<node>(n);
	}) != nullptr;
}

} // namespace anti_prenex_detail

template <NodeType node>
tref anti_prenex(tref phi, const keep_functional_fn<node>& kf) {
	DBG(assert(phi != nullptr);)
	// Every chain built below relies on the construction hooks: a neutral
	// operand, and the n-ary builders' seed, fold away only while they are on.
	DBG(assert(tree<node>::use_hooks);)
	phi = tree<node>::trim_right_sibling(phi);
	// §3's entry test: with no quantifier of either kind there is nothing
	// to push and nothing to resolve, so the formula comes back as the
	// SAME node — no phase runs, and no atom is re-spelled either.
	if (!anti_prenex_detail::carries_quantifier<node>(phi)) return phi;
	// 0. Binder ids by depth: it makes every later substitution
	//    capture-safe and lets alpha-variants share one node (§3).
	phi = canonicalise_binder_ids<node>(phi);
	// 1. NNF, then the ONE `ref_args` call of the run: the canonical entry
	//    state for reference arguments, which establishes invariant 6.
	//    From here on only substitution dirties one, and it re-simplifies
	//    what it touched (§1).
	phi = to_canonically_factored_nnf<node>(phi);
	phi = simplify<node>(phi, {}, true);
	// 2. The deep witness pass, then the global simplification it leaves
	//    to its caller (§3, `TRY_WITNESS_DEEP`).
	phi = eliminate_by_substitution<node>(phi);
	phi = simplify<node>(phi);
	// 3. `f ≠ 0 ↦ ¬(f = 0)` and the fused comparisons, after phase 2 by
	//    the fixed order (§3).
	phi = normalize_operators<node>(phi);
	// 4. `PROCESS_ALL_BLOCKS(φ, keep_functional)` — the identity here
	//    (§3, §4).
	// 5. Every chain the callback does not keep, under NO live order, and
	//    the close: simplify, drop the binders that bind nothing, and
	//    canonicalise the ids again (§3).
	phi = resolve_functional_quantifiers<node>(phi, {}, kf);
	phi = simplify<node>(phi);
	phi = fold_degenerate_binders<node>(phi);
	return canonicalise_binder_ids<node>(phi);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_TMPL_H__
