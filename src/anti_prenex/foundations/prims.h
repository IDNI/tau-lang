// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file prims.h
 * @brief Anti-prenexing foundations (layer 0), package D: the formula-level
 * primitives of §3 — `CANONICALISE_BINDER_IDS`, `REWRAP`,
 * `FOLD_DEGENERATE_BINDERS`.
 *
 * Reuse: `canonize_quantifier_ids` (tau_tree_builders.tmpl.h) numbers binders
 * by depth (id = maximum quantifier depth below + 1) and handles shadowing.
 * It covers functional quantifiers too, over ONE id space shared with the
 * formula binders, so the parser, the api layer and this module all number
 * alike — and ids are part of every memo key.
 *
 * What the ids buy: an outer binder's id is strictly greater than the id of
 * any binder below it, whatever the kinds, so `FV(t)` meets no binder on the
 * path and `[x ← t]` is capture-safe inside a bound scope (the library's
 * `term_handle<node>::substitute`). Ids are not unique — siblings share one
 * — and need not be; strict decrease along a
 * path is the whole property. Numbering starts at 1 and ignores free
 * variables that are themselves numerically named: such names arise only from
 * this pass, the module never introduces a binder that was not already there,
 * and strict decrease keeps a fragment's ids below every enclosing one.
 *
 * This module never renames a bound variable. `build_wff_ex` and
 * `build_wff_all` rename by default (`calculate_quant_id = true`), so every
 * builder call here passes `false`. The `_many` builders always rename and
 * are never used.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_H__

#include <algorithm>

#include "fwd.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 `CANONICALISE_BINDER_IDS(φ)`: rename every binder — formula
 * binders AND functional-quantifier subscripts alike — to a canonical id
 * derived from its position, so that alpha-variants become one node. It is
 * phase 0's normaliser, phase 5's last step, and what makes a formula usable
 * as the key of `ASK` and `DECIDE_FINITE` (§7). This is
 * `canonize_quantifier_ids` under its spec name.
 */
template <NodeType node>
tref canonicalise_binder_ids(tref phi);

/**
 * @brief §3 `REWRAP(φ, X)`: re-attach `∃X` (or `∀X`) around `φ`, in `X`'s
 * order, with `X[0]` outermost. It is every graceful exit of invariant 3, and
 * it builds the keys of `push_memo` and `elim_memo`.
 *
 * NO narrowing happens here; the callers narrow (§5's acceptance passes
 * `P ∩ FV(dep)`, §6 `PUSH_BLOCK` narrows first). Bound variables keep their
 * ids (`calculate_quant_id = false`). Nothing is published: `|·|` and `FV`
 * are computed lazily on the first query. `X` must be duplicate-free, since a
 * repeated variable would build a vacuous inner binder and a non-canonical
 * memo key. An empty `X` returns `φ`.
 */
template <NodeType node>
tref rewrap(tref phi, const block& X,
	quantifier<node> kind = tau_term_bdd<node>::ex);

/**
 * @brief §3 `FOLD_DEGENERATE_BINDERS(φ)`: drop a binder whose scope is a
 * constant, or whose variable is absent from its body (`x ∉ FV(body)`). Every
 * binder of `φ`, in one memoised pass. Phase 5 (§3).
 */
template <NodeType node>
tref fold_degenerate_binders(tref phi);

} // namespace idni::tau_lang::anti_prenexing

#include "prims.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_H__
