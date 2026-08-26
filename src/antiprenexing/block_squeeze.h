// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __BLOCK_SQUEEZE_H__
#define __BLOCK_SQUEEZE_H__

#include <optional>

#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @internal
 * @brief Chapter 5 step 2a: distributes an existential block over every
 * connective of a formula whose atoms are all negated.
 *
 * `∃X (A ∧ B) ≡ ∃X A ∧ ∃X B` holds for negated atoms in an atomless Boolean
 * algebra (Corollary 5.1 with `J₁ = ∅`); combined with the unconditional
 * `∃X (A ∨ B) ≡ ∃X A ∨ ∃X B`, the block can be pushed all the way down to the
 * individual atoms in one step, with no Boole decomposition at all.
 *
 * Atoms in which no block variable occurs free are left bare rather than being
 * given a vacuous binder.
 *
 * The caller is responsible for having established the precondition, i.e.
 * `profile_block_atoms(formula, skip).all_negated()`. This function does not
 * re-check it.
 * @tparam node Tree node type.
 * @param formula Matrix, all of whose atoms are negated equations.
 * @param block Block variables, outermost first.
 * @return The formula with `∃block` distributed onto each dependent atom.
 * @endinternal
 */
template<NodeType node>
tref distribute_block_over_atoms(tref formula, const trefs& block);

/// Maximum number of squeezed disjuncts chapter 5 step 2b will produce before
/// declining and letting the general Boole decomposition run instead; 0 =
/// unlimited (the default: always squeeze). The cross product of an `∧` of
/// `∨`s is multiplicative, so a bound caps the fast path's own blow-up — set
/// it via `--block-squeeze-cap`, REPL `squeezecap`, or
/// `api::set_block_squeeze_cap`. Runtime-tunable per the runtime-parameter
/// policy; NOT thread-safe, like every knob in this pass.
inline size_t block_squeeze_cap = 0;

/**
 * @internal
 * @brief Chapter 5 step 2b.i: squeezes an all-positive formula into a plain
 * disjunction of single equations.
 *
 * Returns the term list `[t₁, …, tₖ]` meaning `t₁ = 0 ∨ … ∨ tₖ = 0`, computed
 * by:
 *  - atom `f = 0`  -> `[f]`
 *  - `A ∨ B`       -> `squeeze(A) ++ squeeze(B)`
 *  - `A ∧ B`       -> `[ a ∪ b : a ∈ squeeze(A), b ∈ squeeze(B) ]`
 *    -- this is the "distribute conjunctions over disjunctions" the paper
 *    calls for, performed on terms rather than on the formula, via the BA
 *    identity `f₁ = 0 ∧ f₂ = 0 ≡ f₁ ∪ f₂ = 0`
 *  - `T`           -> `[0]`   (`0 = 0` is true)
 *  - `F`           -> `[]`    (the empty disjunction is false)
 *
 * Returns `std::nullopt` when the cross product would exceed
 * `block_squeeze_cap`, or when a node that is not one of the above is
 * reached. The caller must have established
 * `profile_block_atoms(formula, skip).all_positive()`.
 * @tparam node Tree node type.
 * @param formula All-positive matrix.
 * @param ba_type Boolean-algebra type id used to build the `0` constant.
 * @return The squeezed term list, or `std::nullopt` if the fast path declines.
 * @endinternal
 */
template<NodeType node>
std::optional<trefs> squeeze_positive_disjuncts(tref formula, size_t ba_type);

} // namespace idni::tau_lang

#include "block_squeeze.tmpl.h"

#endif // __BLOCK_SQUEEZE_H__
