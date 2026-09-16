// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file normal_forms_transformations.h
 * @brief Shared formula-transformation helpers used by normal-form and interpreter code.
 *
 * This file declares the public rewrite and simplification helpers that support
 * normal-form conversion, quantifier elimination, and interpreter-specific
 * temporal formula alignment. Internal helpers reside in
 * normal_forms_transformations.tmpl.h.
 */

#ifndef __IDNI__TAU__NORMAL_FORMS_TRANSFORMATIONS_H__
#define __IDNI__TAU__NORMAL_FORMS_TRANSFORMATIONS_H__

#include "tau_tree.h"
// bv_predicate_blasting.h declares bv_blasting (a non-dependent global), which
// must be visible at template-definition time in normal_forms_transformations.tmpl.h.
#include "heuristics/bv_predicate_blasting.h"

namespace idni::tau_lang {

/**
 * @brief Memory slot identifiers for pre-order traversal caching.
 *
 * These slots are used as template parameters to `pre_order::apply_unique`
 * to select a specific static cache that avoids redundant rewriting on
 * already-visited sub-trees within a single traversal pass.
 */
enum MemorySlotPre {
	/// Not a slot. `apply_unique<0>` means "no persistent memo", so the
	/// first named slot must not be zero, or it silently gets a per-call
	/// cache instead of the one it asked for.
	no_pre_slot = 0,
	normalize_ba_m,          ///< Cache slot for normalize_ba traversals.
	push_negation_in_m,      ///< Cache slot for push_negation_in traversals.
	to_dnf_m,                ///< Cache slot for to_dnf traversals.
	to_cnf_m,                ///< Cache slot for to_cnf traversals.
	eliminate_quantifiers_m, ///< Cache slot for quantifier elimination traversals.
	anti_prenex_step_m,      ///< Cache slot for anti-prenex step traversals.
	/// Cache slot for `push_negation_in<node, is_wff, false>`. The two
	/// `fuse_atoms` instantiations rewrite an atom differently, so they must
	/// not share `push_negation_in_m`: one memo for both would hand a caller
	/// the other's tree.
	push_negation_in_nofuse_m,
	/// Cache slot for the anti-prenexing module's `TO_NNF`
	/// (`to_canonically_factored_nnf`, anti_prenex/normalisers/nnf.h).
	/// Separate from `push_negation_in_m`: that pass fuses a negated atom
	/// into `!=` and friends, which invariant 4 of the spec forbids.
	factored_nnf_m
};

/**
 * @brief Memory slot identifiers for post-order traversal caching.
 *
 * These slots are used as template parameters to `post_order::apply_unique`
 * to select a specific static cache.
 */
enum MemorySlotPost {
	/// Not a slot. `apply_unique<0>` means "no persistent memo", so the
	/// first named slot must not be zero, or it silently gets a per-call
	/// cache instead of the one it asked for.
	no_post_slot = 0,
	anti_prenex_m ///< Cache slot for anti-prenex post-order traversals.
};

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
 * @tparam fuse_atoms `true` (default) fuses a negated atom into its negated
 * operator (`!(x = y)` becomes `x != y`, `!(x < y)` becomes `x !< y`, ...);
 * `false` leaves every atom under its `!` untouched and pushes only through
 * the connectives, the binders, the temporal operators and the sugar. The
 * anti-prenexing module needs `false`: its invariant 4 forbids a `!=` and
 * any negated or mirrored order operator. Each instantiation gets its own
 * memo slot.
 * @param fm Formula to transform.
 * @return Formula with all negations pushed to literals.
 */
template <NodeType node, bool is_wff = true, bool fuse_atoms = true>
tref push_negation_in(tref fm);

/**
 * @brief Apply full syntactic simplifications to a formula.
 *
 * Combines `simplify_using_equality` (equality propagation) and
 * `syntactic_path_simplification` (path contradiction/tautology removal) in
 * sequence.
 *
 * @note This pass takes no `skip` predicate, and deliberately so: it used to
 * accept one for interface consistency with `anti_prenex_block`'s other steps
 * and then discard it, which read as a guarantee it never gave. Neither
 * `simplify_using_equality` nor `syntactic_path_simplification` has a
 * BV-specific check to guard, so a caller that skips bitvector content
 * elsewhere in the pipeline must not assume this step leaves it alone --
 * `simplify_using_equality` ends in `to_nnf`, which does rewrite bitvector
 * comparison atoms.
 * @tparam node Tree node type.
 * @param formula Formula to simplify.
 * @return Simplified formula.
 */
template <NodeType node>
tref syntactic_formula_simplification(tref formula);

// Forward declarations needed by .tmpl.h bodies.
// Full declarations/definitions come from their respective heuristic headers.
template <NodeType node>
tref simplify_using_equality(tref fm);

template <NodeType node>
tref syntactic_path_simplification(tref fm);

#include "normal_forms_transformations.tmpl.h"

} // namespace idni::tau_lang

#endif // __IDNI__TAU__NORMAL_FORMS_TRANSFORMATIONS_H__
