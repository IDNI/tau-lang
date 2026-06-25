// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// TODO (LOW) rename file to msnso_rr.h
// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

#ifndef __IDNI__TAU__NSO_RR_H__
#define __IDNI__TAU__NSO_RR_H__

#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @brief Apply a single rewriting rule to @p n.
 *
 * Tries to match the rule `r` at every position of `n` and returns the
 * rewritten tree (unchanged if the rule does not match).
 * @tparam node Tree node type.
 * @param r Rewriting rule to apply.
 * @param n Tree node to rewrite.
 * @return Rewritten node, or @p n if the rule does not apply.
 * @idea This could be implemented as `operator|`.
 */
template <NodeType node>
tref nso_rr_apply(const rewriter::rule& r, const tref& n);

/**
 * @brief Apply a sequence of rewriting rules to @p n, one after another.
 *
 * Applies each rule in @p rs to the result of the previous application.
 * @tparam node Tree node type.
 * @param rs Ordered set of rules to apply.
 * @param n Starting tree node.
 * @return Fully rewritten node.
 * @idea This could be implemented as `operator|`.
 */
template <NodeType node>
tref nso_rr_apply(const rewriter::rules& rs, tref n);

/**
 * @brief Unfold a recurrence relation into a plain formula.
 *
 * Transforms reference arguments to captures, calculates all fixed points,
 * then applies all recurrence relation rules via `step` until no rule fires.
 * @tparam node Tree node type.
 * @param nso_rr Recurrence relation to unfold.
 * @return The main formula with all recurrence relation definitions applied.
 */
template <NodeType node>
tref nso_rr_apply(const rr<node>& nso_rr);

} // namespace idni::tau_lang

#include "nso_rr.tmpl.h"

#endif // __IDNI__TAU__NSO_RR_H__
