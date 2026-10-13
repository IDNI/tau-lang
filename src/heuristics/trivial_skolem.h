// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file trivial_skolem.h
 * @brief Trivial Skolemization heuristic for existentially quantified variables.
 *
 * Eliminates `ex v1 ... ex vn. phi` for any `vi` whose unique syntactic
 * occurrence in `phi` lies in an eligible atomic equality reachable without
 * crossing negation, wff-level xor/implication-style connectives, or nested
 * (including temporal) quantifiers, and where that equality isolates `vi`
 * via a chain of invertible term operators (negation, addition, subtraction,
 * xor) applied to expressions free of every `vj` in the input set. Variables
 * that cannot be isolated this way are kept, existentially quantified, around
 * whatever subformula remains.
 */

#ifndef __IDNI__TAU__TRIVIAL_SKOLEM_H__
#define __IDNI__TAU__TRIVIAL_SKOLEM_H__

namespace idni::tau_lang {

/**
 * @brief Eliminate the members of @p vars that are trivially Skolemizable in @p phi.
 *
 * @p vars are assumed to be existentially quantified over @p phi. Each
 * variable whose only syntactic occurrence in @p phi belongs to an eligible
 * atomic equality is dropped when that equality isolates the variable via
 * invertible term operators (negation, addition, subtraction, xor) using
 * only expressions free of every variable in @p vars. Eligible atoms may sit
 * below `wff_or`, but not below negation, wff-level xor/implication-style
 * connectives, or nested (including temporal) quantifiers. The remaining
 * variables are re-quantified with `ex` around whatever formula remains. If
 * every variable is eliminated and no subformula is left, returns `T`. A
 * variable of a non-ABA omega-categorical type (a point, such as qlt's) is
 * always kept, for its owner's elimination to decide.
 *
 * @tparam node Tree node type.
 * @param vars Existentially quantified variables to try to eliminate.
 * @param phi Matrix of the block; atoms under a nested quantifier are not
 * used.
 * @return Formula equivalent to `ex vars. phi`.
 *
 * @par Example
 * @code{.cpp}
 * // "a1 = c": a1's only occurrence directly isolates it, so "ex a1. phi"
 * // simplifies all the way to T (see the "direct assignment eliminates the
 * // variable" case of tests/integration/test_integration-heuristics-trivial_skolem.cpp).
 * auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
 * tref phi1 = get_nso_rr("a1 = c.").value().main->get();
 * CHECK( trivial_skolem_ex<node_t>({ a1 }, phi1) == tau::_T() );
 *
 * // "a1 = c && d = e": a1 is eliminated, the unrelated clause "d = e" survives
 * // (see the "leftover clause is preserved when a variable is eliminated"
 * // case of tests/integration/test_integration-heuristics-trivial_skolem.cpp).
 * tref phi2 = get_nso_rr("a1 = c && d = e.").value().main->get();
 * tref expected = get_nso_rr("d = e.").value().main->get();
 * CHECK( trivial_skolem_ex<node_t>({ a1 }, phi2) == expected );
 * @endcode
 */
template <NodeType node>
tref trivial_skolem_ex(const trefs& vars, tref phi);

/**
 * @brief `trivial_skolem_ex` for variables bound inside a longer quantifier
 * prefix: eliminates the members of @p vars it can and reports the rest,
 * without re-quantifying them.
 *
 * Each member of @p vars must be existentially bound in the prefix over
 * @p phi. A witness may mention no variable of @p bound, so it denotes the
 * same value under every binder of the prefix, of either kind, and the
 * eliminated quantifier can be dropped wherever it sits in the prefix.
 *
 * @tparam node Tree node type.
 * @param vars Existentially bound variables to try to eliminate.
 * @param bound Every variable the prefix binds, @p vars included.
 * @param phi Matrix of the prefix; atoms under a nested quantifier are not
 * used.
 * @param kept Output: the members of @p vars that were not eliminated,
 * appended in order.
 * @return @p phi with the defining atom of each eliminated variable replaced
 * by `T`.
 *
 * @par Example
 * @code{.cpp}
 * // "all b ex a1 (a1 = c || b = 0)": a1's witness c mentions no bound
 * // variable, so the prefix is equivalent to "all b T".
 * auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
 * auto b = build_variable<node_t>("b", tau_type_id<node_t>());
 * tref phi = get_nso_rr("a1 = c || b = 0.").value().main->get();
 * trefs kept;
 * CHECK( trivial_skolem_eliminate<node_t>({ a1 }, { b, a1 }, phi, kept) == tau::_T() );
 * CHECK( kept.empty() );
 * @endcode
 */
template <NodeType node>
tref trivial_skolem_eliminate(const trefs& vars, const trefs& bound,
	tref phi, trefs& kept);

} // namespace idni::tau_lang

#include "trivial_skolem.tmpl.h"

#endif // __IDNI__TAU__TRIVIAL_SKOLEM_H__
