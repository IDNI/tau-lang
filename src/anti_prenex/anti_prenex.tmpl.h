// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file anti_prenex.tmpl.h
 * @brief Template implementation for anti_prenex.h: the phases of §3
 * `ANTI_PRENEX`, each one call into the module below, in the order the spec
 * fixes.
 *
 * Phase 4, `PROCESS_ALL_BLOCKS`, is the identity here. Phases 1, 2 and 5 pass
 * the EMPTY order: nothing is BDD-backed outside a component (§3), so
 * `simplify`'s propagation guard is vacuous and the plain chain resolver
 * takes no order at all.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_TMPL_H__

#include <cassert>

namespace idni::tau_lang::anti_prenexing {

namespace anti_prenex_detail {

/// Does `phi` carry a quantifier anywhere: a formula binder (`wff_ex` /
/// `wff_all`) or a term-level functional one (`bf_fex` / `bf_fall`)? Both
/// count, because phases 0 and 1 work on functional chains too (§3). The
/// search enters everything, temporal bodies and reference arguments
/// included.
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
	// 1. Resolve the functional chains the callback does not keep, then
	//    NNF, then the one `ref_args` simplification of the run, which
	//    establishes invariant 6 for reference arguments (§3).
	phi = resolve_functional_quantifiers_plain<node>(phi, kf);
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
	// 5. The close: simplify, drop the binders that bind nothing, and
	//    canonicalise the ids again (§3).
	phi = simplify<node>(phi);
	phi = fold_degenerate_binders<node>(phi);
	return canonicalise_binder_ids<node>(phi);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_TMPL_H__
