// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file anti_prenex.h
 * @brief Anti-prenexing, written from `anti_prenex.md` (the spec).
 *
 * This is the module's single entry header. It includes the layers in build
 * order and, at the end, declares the top-level procedure `anti_prenex`
 * (spec §3, `ANTI_PRENEX`). The spec is the sole source of truth: every
 * function of the module carries a comment naming the spec section it
 * implements, and names mirror the spec one to one in snake case
 * (`PUSH_BLOCK` → `push_block`).
 *
 * The module lives in the namespace `idni::tau_lang::anti_prenexing`. The
 * spelling differs from the old module's entry function `anti_prenex`
 * (`src/antiprenexing/`), which stays wired until this module is sound end
 * to end (layer 4 milestone); nothing here includes the old module.
 *
 * MODULE MAP — directories follow the spec's own division, files follow
 * functionality (one cluster of spec procedures each). Only `foundations/`
 * exists at the time of writing; the rest is created layer by layer.
 *
 *   anti_prenex.h                 ANTI_PRENEX phases 0–5 (§3)
 *   foundations/   layer 0 — §1, §3 primitives, §10
 *     fwd.h                       aliases, small value types, the D1–D5 record
 *     options.h                   the six knobs, inline defaults (§1 ctx table)
 *     dag.h                       |φ|, FV, h, content order, member view, raw
 *                                 canonical chains, neg slot, node classification
 *     terms.h                     PREPARE_TERMS/finish, cofactor, ∀_X ∃_X, the
 *                                 functional-quantifier term, subst_term,
 *                                 SIMPLIFY_TERM, SIMPLIFY_ATOM, TERM_OF,
 *                                 NORM_EQUATION, ‖·‖
 *     subst.h                     φ[x←t], φ[atm↦T/F], atoms_memo (§3, §10)
 *     prims.h                     CANONICALISE_BINDER_IDS, REWRAP,
 *                                 FOLD_DEGENERATE_BINDERS
 *     ctx.h                       ctx, the seven global tables, the memo
 *                                 wrapper, taint, flush
 *   normalisers/   layer 1 — §3
 *     joins.h                     SIMPLIFIED_OR_JOIN, SIMPLIFIED_AND_JOIN
 *     nnf.h                       TO_NNF, NEG with factored negation,
 *                                 NORMALIZE_OPERATORS
 *     simplify.h                  SIMPLIFY: folding, absorption, one-sweep unit
 *                                 elimination, equality propagation
 *   witness/       layer 2 — §3
 *     witness.h                   TRY_WITNESS (spelled + COF mode),
 *                                 TRY_WITNESS_DEEP, TRY_CASE_WITNESS,
 *                                 ELIMINATE_BY_SUBSTITUTION
 *   shared/        layer 3 — read by both tracks (§6, §7 shared helpers)
 *     parts.h                     PARTS, INCIDENCE, CONNECTED_COMPONENTS
 *     cofactors.h                 COF/cof_memo, SETTLE_FUNCTIONAL,
 *                                 COFACTOR_REDUCE, DISCHARGE/quant_memo,
 *                                 FOLD_DECIDED
 *   eliminate/     the engine track — §7 (layers 3, 7, 8)
 *     conditions.h                SQUEEZE, SUBSUME_NEGATIVES, POSITIVE_/
 *                                 NEGATIVE_/TREE_CONDITION,
 *                                 FREEZE_OPAQUE_COMPONENTS
 *     eliminate_block.h           ELIMINATE_BLOCK wrapper, pre-steps, elim_memo;
 *                                 the type table (METHOD,
 *                                 EX_DISTRIBUTES_OVER_NEGATIVES)
 *     atomless.h                  ELIMINATE_ATOMLESS_CLAUSE
 *     finite.h                    BIT_BLAST, DECIDE_FINITE, ASK, FINITE_QUERY,
 *                                 FINITE_TIER2, ELIMINATE_FINITE_CLAUSE
 *     bitvector.h                 SPLIT_ARITHMETIC, SOLVE_ARITHMETIC,
 *                                 ELIMINATE_BITVECTOR_CLAUSE, the §9 black boxes
 *   push/          the push track — §6 (layers 4, 5, 6); the one cycle
 *     push.h                      cluster header: all push declarations, then
 *                                 all push implementations (the one deviation
 *                                 from "each .h includes its own .tmpl.h":
 *                                 the steps call each other through PUSH_BLOCK)
 *     push_block.h                PUSH_BLOCK wrapper (strip, key, taint) and
 *                                 dispatcher (floor: REWRAP)
 *     census.h                    SIGN_CENSUS
 *     fast_paths.h                TRY_FAST_PATHS 2a/2b, DISTRIBUTE_TO_ATOMS,
 *                                 SQUEEZE_POSITIVES
 *     disjunction.h               PUSH_OVER_DISJUNCTION (2d)
 *     conjunction.h               PUSH_OVER_CONJUNCTION: split, settle,
 *                                 consistency, witness step, case witness
 *     expand.h                    EXPAND
 *     decompose.h                 TRY_DECOMPOSE, DECOMPOSE_ARMS
 *   driver/        layer 4 — §4, §5
 *     blocks.h                    PROCESS_ALL_BLOCKS, PROCESS_NODE, COLLECT_RUN,
 *                                 PROCESS_BLOCK (dualisation)
 *     component.h                 PUSH_EX_BLOCK: components, ctx setup,
 *                                 PREPARE_TERMS, size acceptance
 *
 * Inclusion order is the build order and is acyclic except inside `push/`:
 * foundations, normalisers, witness, shared, eliminate, push, driver, entry.
 * `ELIMINATE_BLOCK` never re-enters the push (§7), so `eliminate/` compiles
 * before `push/`. Tests: `tests/unit/test_anti_prenex_<file>.cpp`, one per
 * source file, plus `test_anti_prenex.cpp` (end to end) and
 * `test_anti_prenex_regression.cpp` (the carried-over corpus).
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
