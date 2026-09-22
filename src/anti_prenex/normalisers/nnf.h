// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file nnf.h
 * @brief §3 `TO_NNF`, its `NEG` with FACTORED NEGATION, and §3
 * `NORMALIZE_OPERATORS`.
 *
 * NEGATION AT FORMULA LEVEL (invariant 4): no `bf_neq` and no negated or
 * mirrored order operator is ever built here, so `¬(f = 0)` stays spelled
 * that way and an atom under a `¬` keeps it. That is what separates this pass
 * from `tau_lang::to_nnf`, which fuses a negated atom into `!=`, `!<`, … —
 * hence its own memo slot and the `fuse_atoms = false` instantiation of
 * `push_negation_one_in`.
 *
 * ONE WALK: a single `pre_order::apply_unique_pure`, where `down` rewrites ONE
 * negation at the node it stands on and hands the result back for the walk to
 * descend into, and `up` re-emits every `wff_and`/`wff_or` it closes through
 * the matching join, whether or not the walk changed it. `up` carries no
 * marker: a marker stack pushed in `down` and popped in `up` would
 * desynchronise, because `apply_unique_pure` skips `up` on a memo hit.
 * Unconditional re-emission costs nothing on an already canonical chain,
 * which the joins return unchanged.
 *
 * THE NEG SLOT `neg(φ)` (dag.h) is filled by `canonically_factored_neg`, on
 * first demand, and not at the inner `¬ψ` nodes a walk happens to pass.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__NORMALISERS__NNF_H__
#define __IDNI__TAU__ANTI_PRENEX__NORMALISERS__NNF_H__

#include "../foundations/fwd.h"
#include "../foundations/dag.h"
#include "joins.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 `TO_NNF(φ)`: every `¬` pushed down to the atoms, at most one per
 * atom (invariant 4), the connective chains re-emitted through the result
 * joins.
 *
 * `¬ψ ↦ NEG(ψ)`; an atom stays as written; a binder and a temporal operator
 * are rebuilt over the normalised body; `∧` and `∨` are re-emitted through
 * the joins over the normalised members, so a chain comes back deduplicated,
 * flattened and in content order.
 *
 * The name does not hide `tau_lang::to_nnf`, the codebase's own NNF pass:
 * that one fuses a negated atom into `!=` and the negated order operators,
 * which this module must never build.
 */
template <NodeType node>
tref to_canonically_factored_nnf(tref phi);

/**
 * @brief §3 `NEG(ψ)`: the NNF of `¬ψ`, a pure function of the node, computed
 * on first demand and kept in the `neg(φ)` slot (dag.h `neg_of` / `set_neg`).
 *
 * `T ↦ F`, `F ↦ T`, `¬ψ′ ↦ TO_NNF(ψ′)`; an atom or a reference gets its `¬`
 * as written; a binder flips (`∃ ↔ ∀`, never renaming); `always` and
 * `sometimes` dualise; `⋀ⱼ mⱼ` is De Morgan; `⋁ⱼ mⱼ` is the FACTORED
 * negation — the shared conjuncts pulled out first, so that the enclosing
 * `∃` distributes over a flat disjunction instead of expanding a conjunction
 * of disjunctions.
 */
template <NodeType node>
tref canonically_factored_neg(tref psi);

/**
 * @brief §3 `NORMALIZE_OPERATORS(φ)`: `f ≠ 0 ↦ ¬(f = 0)` for every type, and
 * the six comparison rewrites (`≰ ≮ ≥ > ≱ ≯`) only where the operator denotes
 * ARITHMETIC comparison — today a bitvector type.
 *
 * The negated rewrites are total-order laws and are unsound for a lattice
 * order, which is why the guard is on the type and not on the operator alone.
 * Postcondition for arithmetic-typed content: only un-negated `≤` and `<`
 * remain, so afterwards "negated equation" names every negative leaf (§1).
 *
 * It is `normalize_atomic_formula_operators<node, true, true>` under its spec
 * name; that pass caches per instantiation, so the flag keeps these results
 * apart from every other caller's.
 */
template <NodeType node>
tref normalize_operators(tref phi);

} // namespace idni::tau_lang::anti_prenexing

#include "nnf.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__NORMALISERS__NNF_H__
