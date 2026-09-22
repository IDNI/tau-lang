// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file junctions.h
 * @brief §6 the push over a JUNCTION: 2d, which hands `∃X` to each disjunct
 * of a disjunction, and the conjunction's ladder — scope narrowing, the fast
 * paths, the consistency check, the witness step and the case witness —
 * whose floor is a re-wrap.
 *
 * WHAT ARRIVES is what `PUSH_BLOCK`'s wrapper leaves: `X` non-empty and
 * narrowed to `X ∩ FV(φ)`, and, for a conjunction, every conjunct touching
 * `X`, the strip having hoisted the others outside the block already. Every
 * re-entry goes back through that wrapper, which narrows and strips again, so
 * nothing here re-establishes the invariant itself.
 *
 * CHEAPEST CLASS FIRST (invariant 8): the narrowing's two moves and the fast
 * paths are one traversal each, the consistency check is one question to §7,
 * and the binder-killing steps come last — the pin, then the case pin, the
 * one step here that copies anything, and it copies one branch at a time.
 *
 * `push/push_block.h` is the hub that includes this file, after `push_block`
 * is declared: the template bodies here call it back.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__PUSH__JUNCTIONS_H__
#define __IDNI__TAU__ANTI_PRENEX__PUSH__JUNCTIONS_H__

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/terms.h"
#include "../foundations/subst.h"
#include "../foundations/prims.h"
#include "../normalisers/joins.h"
#include "../normalisers/simplify.h"
#include "../shared/parts.h"
#include "../witness/witness.h"
#include "../eliminate/eliminate_block.h"
#include "fast_paths.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §6 `PUSH_OVER_DISJUNCTION(⋁dᵢ, X, ctx)`, 2d: `∃X.⋁dᵢ` as the
 * ∨-join of the disjuncts pushed one by one.
 *
 * A SHORT-CIRCUIT RACE. The pushes are independent and the join's members
 * are order-invariant, so the order changes only the work spent before some
 * disjunct decides `T`. SMALLEST FIRST (invariant 8): `|d|` predicts a
 * disjunct's push cost, and the race is won by reaching a cheap decider, so
 * a `T` short-circuits before anything dearer is pushed at all. The insert
 * between two disjuncts is the paper's "unit elimination after each
 * disjunct".
 */
template <NodeType node>
tref push_over_disjunction(tref phi, const block& X, ctx<node>& c);

/**
 * @brief §6 `PUSH_OVER_CONJUNCTION(ψ, X, ctx)`: `∃X.ψ` through the ladder of
 * steps a conjunction offers, each one-way — it decides or falls through.
 *
 * 1. SCOPE NARROWING, one incidence pass (`shared/parts.h`) and two moves.
 *    The SPLIT `∃X(ψ₁ ∧ ψ₂) = ∃X₁ψ₁ ∧ ∃X₂ψ₂` pushes each connected part
 *    under its own variables, before the fast paths, so that 2b's cross
 *    product is paid per part. The SETTLE MOVE
 *    `∃X(A ∧ B) = ∃(X∖Xs)(∃Xs.A ∧ B)` hands the conjuncts of the variables
 *    whose scope is final to §7 as a SUB-BLOCK — branch-independent work
 *    hoisted above the branching, and what drains a variable out of `X`.
 *    Both conditions are monotone along the push, which only ever removes
 *    connections and disjunct occurrences, so they are asked here and not
 *    only at block entry.
 * 2. THE FAST PATHS (`push/fast_paths.h`), on the narrowed `ψ`.
 * 3. THE CONSISTENCY CHECK: whether the PURE positive equations alone have a
 *    common zero in `X`, asked of §7 before anything multiplies. Routing,
 *    not reasoning (invariant 1): only the verdict `F` is read, a non-`F`
 *    answer being the solvability condition the leaves restate more
 *    strongly.
 * 4. THE WITNESS STEP, one pin scan per block variable: a pin deletes its
 *    binder, and what is left is pushed on under the smaller block.
 * 5. THE CASE WITNESS, whose rewrite is emitted branch by branch, so that a
 *    branch deciding `T` leaves every later copy unbuilt.
 * 6. THE FLOOR: what none of them takes is re-wrapped (invariant 3).
 */
template <NodeType node>
tref push_over_conjunction(tref psi, const block& X, ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

#include "junctions.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__JUNCTIONS_H__
