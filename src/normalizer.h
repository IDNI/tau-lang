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
 * @brief Collect all variable and capture nodes reachable from `n`.
 *
 * Selects the topmost (non-nested) variable or capture nodes from the sub-tree
 * rooted at `n`.
 * @tparam node Tree node type.
 * @param n Formula to inspect.
 * @return Collection of variable/capture `tref` nodes.
 */
template <NodeType node>
trefs get_vars_from_nso(tref n);

/**
 * @brief Generate a fresh variable identifier not already used in `fm`.
 *
 * Scans all `x<N>` and `?x<N>` variables in `fm` and returns the smallest
 * integer `i` such that `x_i` is not yet present (starting from 1).
 * @tparam node Tree node type.
 * @param fm Formula to inspect for existing variable names.
 * @return A fresh variable index `i`.
 */
template <NodeType node>
int_t get_new_var_id(tref fm);

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
 * @brief Extract the signature and offset list of a reference node.
 *
 * Parses `ref` to produce a pair of:
 *   - `rr_sig`: the recurrence-relation signature (name id + arity + offset arity).
 *   - `vector<offset_t>`: the sequence of offset descriptors.
 *
 * Currently only the first offset is considered (see TODO in implementation).
 * @tparam node Tree node type.
 * @param ref Reference node (`wff_ref` or `bf_ref`).
 * @return `{signature, offsets}`.
 */
template <NodeType node>
std::pair<rr_sig, std::vector<offset_t>> get_ref_info(tref ref);

/**
 * @brief Find the innermost `ref` node reachable from `n`.
 *
 * Descends through single-child nodes until a `ref` is found or the path ends.
 * @tparam node Tree node type.
 * @param n Starting node.
 * @return The `ref` node if found, otherwise `nullptr`.
 */
template <NodeType node>
tref get_ref(tref n);

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
 * @brief Determine whether a non-temporal NSO formula is unsatisfiable.
 *
 * Wraps all free variables with existential quantifiers, normalizes, and returns
 * `true` if the result is `F`.
 * @tparam node Tree node type.
 * @param n Non-temporal formula to test.
 * @return `true` if unsatisfiable.
 */
template <NodeType node>
bool is_non_temp_nso_unsat(tref n);

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
 * @brief Check whether two Boolean functions are semantically equal.
 *
 * Normalizes `all x. (n1 xor n2) = 0` and returns `true` if the result is `T`.
 * @tparam node Tree node type.
 * @param n1 First Boolean function.
 * @param n2 Second Boolean function.
 * @return `true` if `n1` and `n2` are semantically equal as Boolean functions.
 */
template <NodeType node>
bool are_bf_equal(tref n1, tref n2);

/**
 * @brief Check whether Boolean function `n` equals any formula in `previous`.
 *
 * Returns `true` if `are_bf_equal(n, p)` is `true` for any `p` in `previous`.
 * @tparam node Tree node type.
 * @param n Boolean function to test.
 * @param previous Collection of previously seen Boolean functions.
 * @return `true` if `n` is equal to some element of `previous`.
 */
template <NodeType node>
bool is_bf_same_to_any_of(tref n, trefs& previous);

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
 * @brief Determine the maximum lookback offset among all `offsets` nodes in `fm`.
 *
 * Scans all `offsets` nodes and extracts the largest integer offset value.
 * Used to determine the minimum starting step for fixed-point iteration.
 * @tparam node Tree node type.
 * @param fm Formula to inspect.
 * @return Maximum integer offset value found, or 0 if none.
 */
template <NodeType node>
size_t get_max_lookback_in_rr(tref fm);

/**
 * @brief Build a new shift node by adjusting an existing shift by `step`.
 *
 * If the current shift offset equals `step`, the result is the capture child
 * of the shift (resolving the offset completely). Otherwise, a new `shift` node
 * is created with `step - offset` as its numeric value.
 * @tparam node Tree node type.
 * @param shift The existing shift node.
 * @param step The target step value.
 * @return Adjusted shift node, or a capture reference if the offset resolves to zero.
 */
template <NodeType node>
tref build_shift_from_shift(tref shift, size_t step);

/**
 * @brief Substitute a concrete step index into all shift offsets of `form`.
 *
 * Replaces every `shift` offset in `form` by the result of
 * `build_shift_from_shift(shift, step)`, materializing the `step`-th unrolling
 * of the recurrence relation.
 * @tparam node Tree node type.
 * @param form Recurrence relation main formula.
 * @param step The step index to instantiate.
 * @return Formula with all shift offsets resolved to `step`.
 */
template <NodeType node>
tref build_main_step(tref form, size_t step);

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
 * @brief Build the `i`-th enumerated main step of a recurrence relation.
 *
 * Creates a main-step formula for enumeration index `i` with the given
 * `offset_arity`. The first offset of the main formula is replaced by an
 * integer offset of value `i`, and then `build_main_step` is called to
 * resolve shift offsets.
 * @tparam node Tree node type.
 * @param form Main formula of the recurrence relation.
 * @param i Enumeration index.
 * @param offset_arity Number of offset dimensions.
 * @return Enumerated main step formula.
 */
template <NodeType node>
tref build_enumerated_main_step(tref form, size_t i, size_t offset_arity);

/**
 * @brief Validate a recurrence relation structure.
 *
 * Checks that:
 *   - The main formula contains no relative (capture) offsets.
 *   - No rule head contains a shift offset.
 *   - A rule with a fixed head offset does not depend on a relative body offset.
 *   - A rule with a fixed head offset does not depend on a future fixed offset.
 *
 * @tparam node Tree node type.
 * @param nso_rr The recurrence relation to validate.
 * @return `true` if the recurrence relation is valid.
 */
template <NodeType node>
bool is_valid(const rr<node>& nso_rr);

/**
 * @brief Check whether a recurrence relation is well-founded (acyclic).
 *
 * Builds a dependency graph of same-offset rules and performs a DFS cycle
 * detection. Also verifies that at least one rule is a proper relative rule
 * (has a capture offset), not merely a collection of initial conditions.
 * @tparam node Tree node type.
 * @param nso_rr The recurrence relation to check.
 * @return `true` if the recurrence relation is well-founded (no cycles).
 */
template <NodeType node>
bool is_well_founded(const rr<node>& nso_rr);

/**
 * @brief Compute a fixed point of a recurrence relation for a given type.
 *
 * Iterates from `max_lookback` upward, applying the recurrence rules at each
 * step and normalizing. Stops when two consecutive steps are equivalent
 * (`are_nso_equivalent` or `are_bf_equal`). If a loop (non-converging cycle)
 * is detected, returns the `fallback` value.
 * @tparam node Tree node type.
 * @param nso_rr Recurrence relation definition.
 * @param form Main formula being iterated.
 * @param nt Type (`tau::wff` or `tau::bf`) of the formula.
 * @param offset_arity Number of offset dimensions.
 * @param fallback Value to return when no fixed point is found (loop).
 * @return The fixed-point formula, or `nullptr` if the relation is ill-founded.
 */
template <NodeType node>
tref calculate_fixed_point(const rr<node>& nso_rr, tref form, typename node::type t,
	size_t offset_arity, tref fallback);

/**
 * @brief Compute fixed points for all fixed-point calls in the recurrence relation.
 *
 * Validates the recurrence relation, then uses `fixed_point_transformer` to
 * replace every zero-offset reference call (a "fixed-point call") with its
 * computed fixed point. Returns the transformed main formula, or `nullptr`
 * on failure.
 * @tparam node Tree node type.
 * @param nso_rr The recurrence relation structure.
 * @return Transformed main formula with all fixed-point calls resolved.
 */
template <NodeType node>
tref calculate_all_fixed_points(const rr<node>& nso_rr);

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
tref apply_rr_to_formula(const rr<node>& nso_rr);

/**
 * @brief Full normalizer for a recurrence relation.
 *
 * Combines `apply_rr_to_formula` (to unfold the recurrence) with
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
