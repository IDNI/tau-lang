/**
 * @file ex_subs_based_elimination.h
 * @brief Existential-quantifier elimination via substitution.
 *
 * Eliminates `ex var. clause` by scanning `clause` for substitution witnesses
 * compatible with `var` and applying them.  Falls back to the original clause
 * when no compatible substitution is found.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__EX_SUBS_BASED_ELIMINATION_H__
#define __IDNI__TAU__EX_SUBS_BASED_ELIMINATION_H__

namespace idni::tau_lang {

/**
 * @brief Eliminate `ex var. ex_clause` by substitution.
 *
 * Searches @p ex_clause for a witness that can substitute @p var and returns
 * the resulting formula.  Returns @p ex_clause unchanged if no compatible
 * substitution exists.
 *
 * @tparam node Tree node type.
 * @param var The existentially quantified variable to eliminate.
 * @param ex_clause The clause body.
 * @return Quantifier-free formula, or @p ex_clause on failure.
 */
template <NodeType node>
tref ex_subs_based_elimination(tref var, tref ex_clause);

} // namespace idni::tau_lang

#include "ex_subs_based_elimination.tmpl.h"

#endif // __IDNI__TAU__EX_SUBS_BASED_ELIMINATION_H__