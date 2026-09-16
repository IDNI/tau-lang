// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file nnf.h
 * @brief Anti-prenexing normalisers (layer 1), package N: §3 `TO_NNF`, its
 * `NEG` with FACTORED NEGATION, and §3 `NORMALIZE_OPERATORS`.
 *
 * NEGATION AT FORMULA LEVEL (invariant 4). The module never builds a `bf_neq`
 * and never a negated or mirrored order operator: `¬(f = 0)` stays spelled
 * that way, and an atom under a `¬` keeps its `¬`. That is the one thing this
 * file adds to the existing `to_nnf` of the codebase, which fuses a negated
 * atom into `!=`, `!<`, … — hence the separate pass, the separate memo slot
 * and the `fuse_atoms = false` instantiation of `push_negation_one_in`.
 *
 * ONE WALK. `TO_NNF` is a single `pre_order::apply_unique` over the formula,
 * the shape of `push_negation_in`: `down` rewrites ONE negation at the node
 * it stands on and hands the result back for the walk to descend into, `up`
 * re-emits what the walk rebuilt. Phase 1 meets a raw formula and rebuilds it
 * bottom-up in this one memoised pass; from phase 3 on every node is in NNF
 * already and the only work left is `NEG`, cached on the node as `neg(φ)`
 * (§1, dag.h's `neg_of` / `set_neg`).
 *
 * WHY `up` CARRIES NO MARKER. `up` re-emits EVERY `wff_and` / `wff_or` it
 * closes through the matching join, not only the ones the walk changed. A
 * marker stack pushed in `down` and popped in `up` would desynchronise:
 * `pre_order::traverse` consults its slot memo on the node `down` RETURNED,
 * so on a memo hit `down` has already run while `up` is skipped entirely
 * (measured: a formula with one shared subtree gives 13 `down` calls against
 * 12 `up` calls). Unconditional re-emission needs no marker, and it is the
 * spec's own wording — "∧ / ∨ ↦ re-emitted through the joins over the
 * normalised members — phase 1 canonicalises the raw input's chains". It
 * costs nothing on an already canonical chain, which the joins return
 * unchanged, and the slot memo makes a repeat encounter one lookup.
 *
 * THE NEG SLOT is filled by `canonically_factored_neg`, the entry point that
 * asks for one, and not at the inner `¬ψ` nodes a walk happens to pass: a
 * node's slot is filled on first DEMAND (§1), which is exactly what that
 * entry is.
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
 * the joins over the normalised members, which is what canonicalises phase
 * 1's raw input — a chain comes back deduplicated, flattened and in content
 * order.
 *
 * The name avoids hiding `tau_lang::to_nnf`, the codebase's own NNF pass:
 * that one fuses a negated atom into `!=` and the negated order operators,
 * which this module must never build (the `norm_equation` clash terms.h
 * documents is the same hazard one level down).
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
 * Phase 3 (§3). The negated rewrites are total-order laws and are unsound for
 * a lattice order, which is why the guard is on the type and not on the
 * operator alone. Postcondition for arithmetic-typed content: only un-negated
 * `≤` and `<` remain, so after this phase "negated equation" names every
 * negative leaf (§1).
 *
 * It is `normalize_atomic_formula_operators<node, true, true>` under its spec
 * name; the pass has one static cache per instantiation, so the flag keeps
 * this module's results apart from every other caller's for free.
 */
template <NodeType node>
tref normalize_operators(tref phi);

} // namespace idni::tau_lang::anti_prenexing

#include "nnf.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__NORMALISERS__NNF_H__
