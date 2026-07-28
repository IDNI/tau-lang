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

} // namespace idni::tau_lang

#include "block_squeeze.tmpl.h"

#endif // __BLOCK_SQUEEZE_H__
