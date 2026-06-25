// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file normal_forms_transformations.h
 * @brief Shared formula-transformation helpers used by normal-form and interpreter code.
 *
 * This file declares reusable rewrites and simplification helpers that support
 * normal-form conversion, quantifier elimination, and interpreter-specific
 * temporal formula alignment.
 */

#ifndef __IDNI__TAU__NORMAL_FORMS_TRANSFORMATIONS_H__
#define __IDNI__TAU__NORMAL_FORMS_TRANSFORMATIONS_H__

#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @brief Convert `!=` (bf_neq) atoms to negated equalities `!(= 0)`.
 *
 * Rewrites every sub-formula of the form `$X != 0` into `!($X = 0)`.
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Transformed formula with all `bf_neq` replaced by negated `bf_eq`.
 */
template <NodeType node>
tref unequal_to_not_equal(tref fm);


/**
 * @brief Normalize an equality or disequality to the form `expr (!)= 0`.
 *
 * Rewrites `X = Y` to `(X + Y) = 0` and `X != Y` to `(X + Y) != 0`
 * by introducing an XOR term.
 * @tparam node Tree node type.
 * @param eq The equation (wrapped in wff) to normalize.
 * @return Normalized equation with zero right-hand side.
 */
template <NodeType node>
tref norm_equation(tref eq);


/**
 * @brief Normalize all equalities and disequalities in `fm` to `expr (!)= 0`.
 *
 * Applies `norm_equation` to every `bf_eq`/`bf_neq` sub-formula via a
 * pre-order traversal.
 * @tparam node Tree node type.
 * @param fm The formula containing equations to normalize.
 * @return Formula with all equations in normalized form.
 */
template <NodeType node>
tref norm_all_equations(tref fm);


/**
 * @brief Expand all XOR sub-expressions throughout `fm`.
 *
 * Applies `apply_xor_def` recursively to every `bf_xor` node via pre-order.
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Formula with all XOR nodes expanded.
 */
template <NodeType node>
tref apply_all_xor_def(tref fm);


/**
 * @brief Push all negations to the literal level (NNF pre-step for DNF).
 *
 * Applies `push_negation_one_in` via `pre_order` until no negation remains
 * above a non-literal node. Operates on wff or bf depending on `is_wff`.
 * @tparam node Tree node type.
 * @tparam is_wff `true` for wff, `false` for bf (default: `true`).
 * @param fm Formula to transform.
 * @return Formula with all negations pushed to literals.
 */
template <NodeType node, bool is_wff = true>
tref push_negation_in(tref fm);


/**
 * @brief Shift all relative (non-initial) IO variable offsets in `fm` by `shift`.
 *
 * Increases the lookback of every `io_var` in `io_vars` that is not an initial
 * condition by adding `shift` to its time offset. Does nothing if `shift <= 0`.
 * @tparam node Tree node type.
 * @param fm Formula whose IO variables are to be shifted.
 * @param io_vars Collection of IO variable nodes to consider.
 * @param shift Number of time steps to add to the lookback.
 * @return Formula with adjusted IO variable offsets.
 */
template <NodeType node>
tref shift_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift);


/**
 * @brief Shift initial-condition IO variables in `fm` by `shift`.
 *
 * Adds `shift` to the absolute time point of every `io_var` that *is* an
 * initial condition. Returns `F` if any resulting time point would be negative.
 * Does nothing if `shift <= 0`.
 * @tparam node Tree node type.
 * @param fm Formula whose initial IO variables are to be shifted.
 * @param io_vars Collection of IO variable nodes to consider.
 * @param shift Time offset to add.
 * @return Formula with adjusted initial IO variable time points, or `F`.
 */
template <NodeType node>
tref shift_const_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift);


/**
 * @brief Conjoin two `always`-quantified formulas, aligning their lookbacks.
 *
 * Strips the `always` wrapper from `fm1_aw` and `fm2_aw`, determines their
 * maximum shift, and adjusts the formula with the shorter lookback by adding
 * the difference, then returns their conjunction.
 * @tparam node Tree node type.
 * @param fm1_aw First (possibly `always`-wrapped) formula.
 * @param fm2_aw Second (possibly `always`-wrapped) formula.
 * @return Conjunction of the two formulas with aligned lookbacks.
 */
template <NodeType node>
tref always_conjunction(tref fm1_aw, tref fm2_aw);


/**
 * @brief Collect all positive equalities in `n` of the given BA type and merge them.
 *
 * Finds all `bf_eq` atoms whose BA type matches `type_id`, normalizes each
 * to `f = 0` form, and combines their left-hand sides into a single disjunction
 * `f1 | f2 | ...`. Returns the resulting BF, or `nullptr` if no match is found.
 * @tparam node Tree node type.
 * @param n Formula to search for equalities.
 * @param type_id BA type identifier selecting which equalities to merge.
 * @return Disjunction of all matching left-hand sides, or `nullptr`.
 */
template <NodeType node>
tref squeeze_positives(tref n, size_t type_id);


/**
 * @brief Replace all free variables in `fm` by the constant `val`.
 *
 * Collects the free variables of `fm` and substitutes each with `val`.
 * @tparam node Tree node type.
 * @param fm Formula whose free variables are to be replaced.
 * @param val The constant value to substitute.
 * @return `fm` with all free variables replaced by `val`.
 */
template <NodeType node>
tref replace_free_vars_by(tref fm, tref val);


/**
 * @brief Apply full syntactic simplifications to a formula.
 *
 * Combines `simplify_using_equality` (equality propagation) and
 * `syntactic_path_simplification` (path contradiction/tautology removal) in
 * sequence.
 * @tparam node Tree node type.
 * @param formula Formula to simplify.
 * @return Simplified formula.
 */
template <NodeType node>
tref syntactic_formula_simplification(tref formula);



/**
 * @brief Process a single existentially quantified clause in the anti-prenex algorithm.
 *
 * Applies `ex_quantified_boole_decomposition` repeatedly until the quantifier is
 * eliminated or no further simplification is possible.
 * @tparam node Tree node type.
 * @param ex_clause An existentially quantified formula (a single clause).
 * @param[out] quant_eliminated Set to `true` if the quantifier was removed.
 * @return Simplified (possibly quantifier-free) formula.
 */
template <NodeType node>
tref treat_ex_quantified_clause(tref ex_clause, bool& quant_eliminated);


/**
 * @brief Resolve/eliminate quantifiers in a formula.
 *
 * Pushes quantifiers inward (via `push_quantifiers_in`), then applies Boole
 * decomposition to eliminate remaining existential quantifiers. Handles
 * bitvector formulas by delegating to the CVC5 solver when the formula is closed.
 * @tparam node Tree node type.
 * @param formula Formula containing quantifiers to eliminate.
 * @return Quantifier-free formula, or a formula with as few quantifiers as possible.
 */
template<NodeType node>
tref resolve_quantifiers(tref formula);

} // namespace idni::tau_lang

// NOTE: normal_forms_transformations.tmpl.h is included at the end of
// normal_forms.tmpl.h, after the definitions it depends on are available.

#endif // __IDNI__TAU__NORMAL_FORMS_TRANSFORMATIONS_H__
