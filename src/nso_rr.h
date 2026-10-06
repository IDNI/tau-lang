// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file nso_rr.h
 * @brief Applying recurrence relations to NSO formulas.
 *
 * Declares the `nso_rr_apply` overloads: apply one rewriting rule or an
 * ordered sequence of rules to a tree, and unfold a full `rr<node>` —
 * calculating the fixed points its main formula calls and applying the
 * recurrence relation definitions until no rule fires (implementations
 * in nso_rr.tmpl.h).
 */

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
 * rewritten tree (unchanged if the rule does not match). The bound-variable
 * ids of the rule body are first shifted above those of @p n, so the
 * expansion cannot capture an argument. Counts the application (and the hit,
 * when the tree changes) while `rule_counting` is set; with `TAU_CACHE` the
 * result is memoized per (rule, tree, `pack_widening_active`).
 * @tparam node Tree node type.
 * @param r Rewriting rule to apply.
 * @param n Tree node to rewrite.
 * @return Rewritten node, or @p n if the rule does not apply.
 * @idea This could be implemented as `operator|`.
 */
template <NodeType node>
tref nso_rr_apply(const rewriter::rule& r, const tref& n);

/** @brief Application counts accumulated by @ref nso_rr_apply(const
 * rewriter::rule&, const tref&) while @ref rule_counting is set, keyed by
 * the rule's printable form. One map instance per @p node. */
template <NodeType node>
std::unordered_map<std::string, size_t>& rule_apply_counts();
/** @brief Hit counts (applications that changed the tree) accumulated by
 * @ref nso_rr_apply(const rewriter::rule&, const tref&) while
 * @ref rule_counting is set, keyed by the rule's printable form. One map
 * instance per @p node. */
template <NodeType node>
std::unordered_map<std::string, size_t>& rule_hit_counts();

/** @brief Emit one report::count() node per rule per metric for whatever
 * @ref rule_apply_counts / @ref rule_hit_counts have accumulated, then
 * clear both maps so the next normalization pass starts empty.
 * @param rep Report receiving the "<rule> applications" and
 *        "<rule> hits" counts. */
template <NodeType node>
void flush_rule_counts(report& rep);

/**
 * @brief Apply a sequence of rewriting rules to @p n, one after another.
 *
 * Applies each rule in @p rs once to the result of the previous application;
 * a rule whose head is a reference whose signature occurs neither in @p n
 * nor in an applied rule body is skipped. With `TAU_CACHE` the result is
 * memoized per (rules, tree, `pack_widening_active`).
 * @tparam node Tree node type.
 * @param rs Ordered set of rules to apply.
 * @param n Starting tree node.
 * @return The node after one pass over @p rs; not necessarily a fixed point
 *         (callers iterate, e.g. through `step`).
 * @idea This could be implemented as `operator|`.
 */
template <NodeType node>
tref nso_rr_apply(const rewriter::rules& rs, tref n);

/**
 * @brief Unfold a recurrence relation into a plain formula.
 *
 * Transforms reference arguments to captures, calculates all fixed points,
 * then applies all recurrence relation rules via `step` until no rule fires,
 * and finally renumbers the quantifier ids canonically when the main formula
 * changed. The time of both phases is recorded in the result's report.
 * @tparam node Tree node type.
 * @param nso_rr Recurrence relation to unfold.
 * @return The main formula with all recurrence relation definitions applied;
 *         an error when `calculate_all_fixed_points` fails or the rewriting
 *         does not reach a fixed point.
 */
template <NodeType node>
result<tref> nso_rr_apply(const rr<node>& nso_rr);


/**
 * @brief Replace every fixpoint call in @p nso_rr's main formula by its
 * calculated fixpoint (or fallback) value.
 *
 * A fixpoint call is an offset-free reference to a recurrence defined with
 * offsets, e.g. `g(y)` for `g[n](x) := ...`; the rest of the formula is
 * unchanged.
 * @tparam node Tree node type.
 * @param nso_rr Recurrence relation whose main formula is rewritten; its
 *        definitions should already be in capture form
 *        (`transform_ref_args_to_captures`).
 * @return The rewritten main formula; a `type_error` when @p nso_rr is not
 *         valid, an `internal_error` when a fixpoint calculation fails
 *         (multi-index call, non-well-founded definitions, exhausted
 *         enumeration budget, or rules that never apply to the call).
 */
template <NodeType node>
result<tref> calculate_all_fixed_points(const rr<node>& nso_rr);

/**
 * @brief Turn @p nso_rr's definitions into applicable rewrite rules.
 *
 * Variables in offset positions and in reference arguments become captures,
 * so a rule head matches any call. In each rule's head every variable
 * argument is converted; in its body only the variables that appeared in the
 * head are (a body variable of its own is a value, not a pattern hole). The
 * main formula only gets its offset variables converted. IO stream variables
 * are never touched. Types carried by the converted arguments are preserved.
 * @tparam node Tree node type.
 * @param nso_rr Recurrence relation to transform; not modified.
 * @return The transformed copy.
 */
template <NodeType node>
rr<node> transform_ref_args_to_captures(const rr<node>& nso_rr);

} // namespace idni::tau_lang

#include "nso_rr.tmpl.h"

#endif // __IDNI__TAU__NSO_RR_H__
