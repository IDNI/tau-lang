// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file trivial_skolem.h
 * @brief Trivial Skolemization heuristic for existentially quantified variables.
 *
 * Eliminates `ex v1 ... ex vn. phi` for any `vi` that occurs in exactly one
 * top-level conjunct of `phi`, where that conjunct is an equation isolating
 * `vi` via a chain of invertible term operators (negation, addition,
 * subtraction, xor) applied to expressions free of every `vj` in the input
 * set. Variables that can't be isolated this way are kept, existentially
 * quantified, around whatever conjuncts remain.
 */

#ifndef __IDNI__TAU__TRIVIAL_SKOLEM_H__
#define __IDNI__TAU__TRIVIAL_SKOLEM_H__

namespace idni::tau_lang {

/**
 * @brief Eliminate the members of @p vars that are trivially Skolemizable in @p phi.
 *
 * @p vars are assumed to be existentially quantified over @p phi. Each
 * variable that occurs in exactly one top-level conjunct of @p phi, where
 * that conjunct is an equation isolating the variable via invertible term
 * operators (negation, addition, subtraction, xor) using only expressions
 * free of every variable in @p vars, is dropped along with its conjunct.
 * The remaining variables are re-quantified with `ex` around whatever
 * conjuncts remain (their own included). If every variable is eliminated
 * and no conjunct is left, returns the constant `T`.
 *
 * @tparam node Tree node type.
 * @param vars Existentially quantified variables to try to eliminate.
 * @param phi Quantifier-free matrix.
 * @return Formula equivalent to `ex vars. phi`.
 */
template <NodeType node>
tref trivial_skolem_ex(const trefs& vars, tref phi);

} // namespace idni::tau_lang

#include "trivial_skolem.tmpl.h"

#endif // __IDNI__TAU__TRIVIAL_SKOLEM_H__
