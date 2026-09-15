// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file prims.h
 * @brief Anti-prenexing foundations (layer 0), package D: the formula-level
 * primitives of §3 — `CANONICALISE_BINDER_IDS`, `REWRAP`,
 * `FOLD_DEGENERATE_BINDERS`.
 *
 * Reuse: `canonize_quantifier_ids` (tau_tree_builders.tmpl.h) numbers binders
 * by depth (id = max quantifier depth below + 1) and handles shadowing. It is
 * extended in place, not duplicated, to cover functional quantifiers over ONE
 * id space shared with the formula binders (ruling, Sep 9 2026): the parser,
 * `tree::substitute`, the api layer and this module then number alike, and
 * ids are part of every memo key.
 *
 * What the ids buy: an outer binder's id is strictly greater than the id of
 * any binder below it, whatever the kinds, so `FV(t)` meets no binder on the
 * path and `[x ← t]` is capture-safe inside a bound scope (subst.h). Ids are
 * not unique — siblings share — and need not be; strict decrease along a
 * path is the whole property. Numbering starts at 1 and ignores free
 * variables that are themselves numerically named: those arise only from
 * this pass, the module never introduces a binder that was not already
 * there, and strict decrease keeps a fragment's ids below every enclosing
 * one (ruling 1b, Sep 9 2026).
 *
 * Never rename a bound variable (ground rule 4 of the module): `build_wff_ex` /
 * `build_wff_all` rename by default (`calculate_quant_id = true`); every
 * builder call in this module passes `false`. The `_many` builders always
 * rename and are never used.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_H__

#include <algorithm>

#include "fwd.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 `CANONICALISE_BINDER_IDS(φ)`: renames every binder — formula
 * binders AND functional-quantifier subscripts alike — to canonical ids by
 * position, so alpha-variants are one node: phase 0's normaliser, phase 5's
 * last step, and the key of `ASK` and `DECIDE_FINITE` (§7). This is
 * `canonize_quantifier_ids` under its spec name.
 */
template <NodeType node>
tref canonicalise_binder_ids(tref phi);

/**
 * @brief §3 `REWRAP(φ, X)`: re-attach `∃X` (or `∀X`) around `φ`, in `X`'s
 * order (`X[0]` outermost) — every graceful exit of invariant 3, and (D3)
 * the key builder of `push_memo`/`elim_memo`. NO narrowing: the callers
 * narrow (§5 acceptance passes `P ∩ FV(dep)`, §6 `PUSH_BLOCK` narrows first).
 * Bound variables keep their ids (`calculate_quant_id = false`). Nothing is
 * published: `|·|` and `FV` are computed lazily on first query. `X` must be
 * duplicate-free (a repeated variable would build a vacuous inner binder and
 * a non-canonical D3 key). An empty `X` returns `φ`.
 */
template <NodeType node>
tref rewrap(tref phi, const block& X,
	quantifier<node> kind = tau_term_bdd<node>::ex);

/**
 * @brief §3 `FOLD_DEGENERATE_BINDERS(φ)`: drop a binder over a constant
 * scope or over a variable absent from its body (`x ∉ FV(body)`), every
 * binder of `φ`, one memoised pass. Phase 5 (§3).
 */
template <NodeType node>
tref fold_degenerate_binders(tref phi);

} // namespace idni::tau_lang::anti_prenexing

#include "prims.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__PRIMS_H__
