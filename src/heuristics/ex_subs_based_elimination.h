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
 *
 * @par Example
 * @code{.cpp}
 * // Substitution applied: "x = a && y = b" offers "x = a" as a witness, so
 * // a is substituted for x and the result differs from the input (see
 * // tests/integration/test_integration-heuristics-ex_subs_based_elimination.cpp:26-33).
 * auto var = build_variable<node_t>("x", tau_type_id<node_t>());
 * tref ex_clause = get_nso_rr("x = a && y = b.").value().main->get();
 * tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
 * CHECK( result != ex_clause );
 *
 * // Occurs-check rejection: the only equality mentioning x is "x = x|y",
 * // and x occurs on its own right-hand side, so no substitution is applied
 * // (see tests/integration/test_integration-heuristics-ex_subs_based_elimination.cpp:71-80).
 * tref ex_clause2 = get_nso_rr("x = x | y && y = b.").value().main->get();
 * tref result2 = ex_subs_based_elimination<node_t>(var, ex_clause2);
 * CHECK( result2 == ex_clause2 );
 * @endcode
 */
template <NodeType node>
tref ex_subs_based_elimination(tref var, tref ex_clause);

} // namespace idni::tau_lang

#include "ex_subs_based_elimination.tmpl.h"

#endif // __IDNI__TAU__EX_SUBS_BASED_ELIMINATION_H__