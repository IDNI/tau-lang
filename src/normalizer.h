// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file normalizer.h
 * @brief Top-level Tau formula normalizer and related utilities.
 *
 * This file declares the main entry points for normalizing Tau language
 * formulas: `normalize` (temporal + non-temporal), `normalize_non_temp`
 * (non-temporal only), and the `normalizer` family that additionally unfolds
 * recurrence relations. It also provides auxiliary predicates for satisfiability,
 * equivalence, well-foundedness, and fixed-point computation over recurrence
 * relations.
 *
 * Template implementations reside in normalizer.tmpl.h, which is included at
 * the bottom of this file.
 */

#ifndef __IDNI__TAU__NORMALIZER_H__
#define __IDNI__TAU__NORMALIZER_H__

#include "normal_forms.h"
#include "definitions.h"

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau_lang {

/**
 * @brief Descriptor of a single reference offset.
 *
 * The first element identifies the offset kind (one of `tau_parser::nonterminal`
 * values: `num`, `capture`, `shift`, or `variable`). The second element holds
 * the numeric value for `num`, the `rr_dict` identifier for `capture`, or 0 for
 * `shift` and `variable`.
 */
using offset_t = std::pair<tau_parser::nonterminal, size_t>;

/**
 * @brief Normalize a Tau formula, handling both temporal and non-temporal cases.
 *
 * For formulas without temporal quantifiers (`always`/`sometimes`):
 *   1. Resolves closed quantified bitvector sub-formulas (`resolve_quantifiers`).
 *   2. Applies anti-prenex (`anti_prenex`).
 *   3. Resolves remaining quantifiers.
 *
 * For formulas with temporal quantifiers, the same three steps are applied
 * to each inner formula below a temporal quantifier, then the temporal layer
 * is normalized via `normalize_temporal_quantifiers`.
 *
 * @tparam node Tree node type.
 * @param form The formula to normalize.
 * @return Normalized formula.
 */
template <NodeType node>
tref normalize(tref form);

/**
 * @brief Normalize a non-temporal formula.
 *
 * Assumes the formula contains no `always`/`sometimes` quantifiers. Applies
 * the following pipeline in order:
 *   1. `resolve_quantifiers`
 *   2. `anti_prenex`
 *   3. `resolve_quantifiers`
 *   4. `term_boole_normal_form`
 *
 * @tparam node Tree node type.
 * @param fm Non-temporal formula to normalize.
 * @return Normalized formula.
 */
template <NodeType node>
tref normalize_non_temp(tref fm);

/**
 * @brief Build a fresh uninterpreted constant of the given BA type not present in `fm`.
 *
 * Scans all `uconst_name` nodes in `fm` whose name starts with `name` and
 * returns a new BF node `name<i>` where `i` is one past the largest existing
 * suffix index (starting from 1).
 * @tparam node Tree node type.
 * @param fm Formula to inspect for existing uninterpreted constants.
 * @param name Base name prefix for the new constant.
 * @param type BA type identifier for the new constant node.
 * @return A fresh uninterpreted constant node.
 */
template <NodeType node>
tref get_new_uninterpreted_constant(tref fm, const std::string& name, size_t type);

/**
 * @brief Check that a formula does not use Boolean combinations of models.
 *
 * Returns `false` if the formula (or its inner formula when wrapped in `always`)
 * contains any nested `wff_always` or `wff_sometimes` quantifier, which would
 * constitute a Boolean combination of models — an unsupported construct.
 * @tparam node Tree node type.
 * @param n Formula to inspect.
 * @return `true` if no Boolean combination of models is present.
 */
template <NodeType node>
bool has_no_boolean_combs_of_models(tref n);

/**
 * @brief Determine whether a non-temporal NSO formula is satisfiable.
 *
 * Wraps all free variables of `n` with existential quantifiers, normalizes via
 * `normalize_non_temp`, and returns `true` if the result is `T`.
 * @tparam node Tree node type.
 * @param n Non-temporal formula to test (must not contain `always`/`sometimes`).
 * @return `true` if satisfiable.
 */
template <NodeType node>
bool is_non_temp_nso_satisfiable(tref n);

/**
 * @brief Check whether two non-temporal NSO formulas are logically equivalent.
 *
 * Handles three fast paths:
 *   1. Structural equality.
 *   2. Reference-signature equality (when both are `wff_ref`/`bf_ref`).
 *   3. Full normalization of `n1 => n2` and `n2 => n1`.
 *
 * Asserts that neither formula uses Boolean combinations of models.
 * @tparam node Tree node type.
 * @param n1 First formula.
 * @param n2 Second formula.
 * @return `true` if `n1` and `n2` are equivalent.
 */
template <NodeType node>
bool are_nso_equivalent(tref n1, tref n2);

/**
 * @brief Check whether `n1` implies `n2` as non-temporal NSO formulas.
 *
 * Normalizes `all x. (n1 => n2)` (with `x` ranging over all free variables)
 * and returns `true` if the result is `T`.
 * @tparam node Tree node type.
 * @param n1 Antecedent formula.
 * @param n2 Consequent formula.
 * @return `true` if `n1 => n2` is valid.
 */
template <NodeType node>
bool is_nso_impl(tref n1, tref n2);

/**
 * @brief Normalize a formula with temporal simplifications.
 *
 * Full normalization pipeline including:
 *   1. `normalize` (with temporal quantifiers).
 *   2. `fold_trivial_quantifiers` (remove vacuous quantifiers after substitution).
 *   3. Late `resolve_quantifiers` for residual bitvector sub-formulas.
 *   4. Application of registered function/predicate definitions (iterating until
 *      a fixed point).
 *   5. Temporal layer simplification: removes implied `always`/`sometimes` parts.
 *
 * @tparam node Tree node type.
 * @param fm Formula to normalize.
 * @return Fully normalized formula.
 */
template <NodeType node>
tref normalize_with_temp_simp(tref fm);

/**
 * @brief Normalize a Boolean function that has no recurrence relation.
 *
 * Applies `syntactic_path_simplification` followed by `bf_reduced_dnf`.
 * Also resolves any present function/predicate definitions iteratively.
 * @tparam node Tree node type.
 * @param bf Boolean function (without recurrence relations).
 * @return Normalized Boolean function in reduced DNF.
 */
template <NodeType node>
tref bf_normalizer_without_rec_relation(tref bf);

/**
 * @brief Normalize a Boolean function that includes recurrence relations.
 *
 * Transforms reference arguments to captures, calculates all fixed points via
 * `calculate_all_fixed_points`, unfolds the recurrence with `step`, and
 * delegates to `bf_normalizer_without_rec_relation` for the final normalization.
 * @tparam node Tree node type.
 * @param bf Recurrence relation structure containing the Boolean function.
 * @return Normalized Boolean function.
 */
template <NodeType node>
tref bf_normalizer_with_rec_relation(const rr<node> &bf);

/**
 * @brief Full normalizer for a recurrence relation.
 *
 * Combines `nso_rr_apply` (to unfold the recurrence) with
 * `normalize_with_temp_simp` (to normalize the resulting formula including
 * temporal simplifications).
 * @tparam node Tree node type.
 * @param nso_rr The complete recurrence relation structure.
 * @return Fully normalized formula.
 */
template <NodeType node>
tref normalizer(const rr<node>& nso_rr);

/**
 * @brief Full normalizer for a plain formula (no recurrence relation).
 *
 * Convenience overload that wraps `normalize_with_temp_simp`.
 * @tparam node Tree node type.
 * @param fm Formula to normalize.
 * @return Fully normalized formula.
 */
template <NodeType node>
tref normalizer(tref fm);

} // namespace idni::tau_lang

#include "normalizer.tmpl.h"

#endif // __IDNI__TAU__NORMALIZER_H__
