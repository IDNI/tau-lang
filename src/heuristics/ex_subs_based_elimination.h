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
 * Takes the first equation `var = t` (or `t = var`) reached from the root of
 * @p ex_clause through `wff_and` alone whose `t` does not contain @p var
 * (occurs-check) and, for a non-ABA omega-categorical type, denotes a point.
 * Replaces @p var by `t` everywhere except under a quantifier binding @p var,
 * and rebuilds the result with the construction hooks. The substitution is
 * declined when a variable of `t` is re-bound by a quantifier inside
 * @p ex_clause (capture-check). With `TAU_CACHE` the result is memoized per
 * (var, ex_clause) content.
 *
 * @tparam node Tree node type.
 * @param var The existentially quantified variable to eliminate.
 * @param ex_clause The clause body.
 * @return `ex_clause[var := t]`, or @p ex_clause itself (the same tref, so a
 *         caller may test for a change by identity) when no witness applies.
 *
 * @par Example
 * @code{.cpp}
 * // Substitution applied: "x = a && y = b" offers "x = a" as a witness, so
 * // a is substituted for x and the result differs from the input (see the
 * // test case "simple case subs applied (y1)" in
 * // tests/integration/test_integration-heuristics-ex_subs_based_elimination.cpp).
 * auto var = build_variable<node_t>("x", tau_type_id<node_t>());
 * tref ex_clause = get_nso_rr("x = a && y = b.").value().main->get();
 * tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
 * CHECK( result != ex_clause );
 *
 * // Occurs-check rejection: the only equality mentioning x is "x = x|y",
 * // and x occurs on its own right-hand side, so no substitution is applied
 * // (see the test case "occurs check: no subs when candidate contains var" in
 * // tests/integration/test_integration-heuristics-ex_subs_based_elimination.cpp).
 * tref ex_clause2 = get_nso_rr("x = x | y && y = b.").value().main->get();
 * tref result2 = ex_subs_based_elimination<node_t>(var, ex_clause2);
 * CHECK( result2 == ex_clause2 );
 * @endcode
 */
template <NodeType node>
tref ex_subs_based_elimination(tref var, tref ex_clause);

/**
 * @brief Eliminate existential quantifiers in a formula by substitution.
 *
 * Visits @p fm bottom-up and replaces each `ex var scope` for which the
 * two-argument overload finds a witness by the substituted scope; any other
 * quantifier is kept as is.
 *
 * @tparam node Tree node type.
 * @param fm The formula containing existential quantifiers to eliminate.
 * @return @p fm with every eliminable existential quantifier removed.
 */
template <NodeType node>
tref ex_subs_based_elimination(tref fm);

} // namespace idni::tau_lang

#include "ex_subs_based_elimination.tmpl.h"

#endif // __IDNI__TAU__EX_SUBS_BASED_ELIMINATION_H__