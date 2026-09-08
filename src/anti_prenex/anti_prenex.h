// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file anti_prenex.h
 * @brief Anti-prenexing, written from `anti_prenex.md` (the spec).
 *
 * The module's single entry header: includes the layers in build order and
 * declares the top-level procedure `anti_prenex` (spec §3, `ANTI_PRENEX`).
 * The spec is the sole source of truth; every function carries a comment
 * naming the spec section it implements, and names mirror the spec one to
 * one in snake case (`PUSH_BLOCK` → `push_block`). The module map — every
 * directory and file of all nine layers — and the ground rules are in
 * the layer-0 plan (kept outside the repository) (§1 rules, §2 skeleton, §7 module map).
 *
 * Namespace `idni::tau_lang::anti_prenexing`: the spelling differs from the
 * old module's entry function `anti_prenex` (`src/antiprenexing/`), which
 * stays wired until this module is sound end to end (layer 4 milestone).
 * When the old module is deleted the namespace is renamed to `anti_prenex`.
 * Nothing here includes the old module.
 *
 * Inclusion order is the build order and is acyclic except inside `push/`:
 * foundations, normalisers, witness, shared, eliminate, push, driver, entry.
 * `ELIMINATE_BLOCK` never re-enters the push (§7), so `eliminate/` compiles
 * before `push/`; `push/push.h` is the one cluster header (the steps call
 * each other through `PUSH_BLOCK`).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_H__
#define __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_H__

// layer 0 — foundations
#include "foundations/fwd.h"
#include "foundations/options.h"
#include "foundations/dag.h"
#include "foundations/terms.h"
#include "foundations/subst.h"
#include "foundations/prims.h"
#include "foundations/ctx.h"
// layer 1 — normalisers/joins.h, nnf.h, simplify.h
// layer 2 — witness/witness.h
// layer 3 — shared/parts.h, cofactors.h; eliminate/conditions.h,
//           eliminate_block.h, atomless.h
// layer 4 — push/push.h; driver/blocks.h, component.h
// layers 5–6 — the remaining push steps (declared in push/push.h)
// layers 7–8 — eliminate/finite.h, bitvector.h

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 `ANTI_PRENEX(φ, keep_functional = false)` — the whole pipeline.
 *
 * Phases, in this fixed order: 0 `CANONICALISE_BINDER_IDS`; 1 `TO_NNF` then
 * `SIMPLIFY(φ, ref_args = true)`; 2 `ELIMINATE_BY_SUBSTITUTION` then
 * `SIMPLIFY`; 3 `NORMALIZE_OPERATORS`; 4 `PROCESS_ALL_BLOCKS`; 5 `SIMPLIFY`,
 * `FOLD_DEGENERATE_BINDERS`, `CANONICALISE_BINDER_IDS`. A formula without a
 * quantifier is returned as is. Defined at layer 2 (phase 4 is the identity
 * until layer 4).
 *
 * @param phi             a `wff` node
 * @param keep_functional §1 `ctx.keep_functional`: emit `∀_X`/`∃_X`
 *                        symbolically instead of discharging them
 * @return the anti-prenexed formula, a `wff` node
 */
template <NodeType node>
tref anti_prenex(tref phi, bool keep_functional = false);

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_H__
