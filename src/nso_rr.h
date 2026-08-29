// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// TODO (LOW) rename file to msnso_rr.h
// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

#ifndef __IDNI__TAU__NSO_RR_H__
#define __IDNI__TAU__NSO_RR_H__

#include "tau_tree.h"
#include "tau_diagnostics.h"

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

/** @brief Application/hit counts accumulated by @ref nso_rr_apply(const
 * rewriter::rule&, const tref&) while @ref rule_counting is set, keyed by
 * the rule's printable form. One map instance per @p node. */
template <NodeType node>
std::unordered_map<std::string, size_t>& rule_apply_counts();
template <NodeType node>
std::unordered_map<std::string, size_t>& rule_hit_counts();

/** @brief Emit one report::count() node per rule per metric for whatever
 * @ref rule_apply_counts / @ref rule_hit_counts have accumulated, then
 * clear both maps so the next normalization pass starts empty. */
template <NodeType node>
void flush_rule_counts(report& rep);

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
 * @return The main formula with all recurrence relation definitions applied,
 *         or an error result on failure.
 */
template <NodeType node>
result<tref> nso_rr_apply(const rr<node>& nso_rr);


/** @brief Replace every fp-calculation ref in @p nso_rr's main with its
 * computed fixed point (RR-9: previously defined only in the tmpl). */
template <NodeType node>
tref calculate_all_fixed_points(const rr<node>& nso_rr);

/** @brief Return @p nso_rr with each rec-relation head's ref args turned
 * into capture patterns (RR-9). */
template <NodeType node>
rr<node> transform_ref_args_to_captures(const rr<node>& nso_rr);

} // namespace idni::tau_lang

#include "nso_rr.tmpl.h"

#endif // __IDNI__TAU__NSO_RR_H__
