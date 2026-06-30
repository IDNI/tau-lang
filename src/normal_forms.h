// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file normal_forms.h
 * @brief Normal-form conversion interfaces and supporting Tau formula helpers.
 *
 * This file declares the core normal-form conversion APIs, traversal cache
 * enums, and helper types used by Tau's normal-form algorithms. Shared
 * transformation utilities are declared in normal_forms_transformations.h,
 * while template implementations remain in normal_forms.tmpl.h.
 */

#ifndef __IDNI__TAU__NORMAL_FORMS_H__
#define __IDNI__TAU__NORMAL_FORMS_H__

#include <list>
#include <numeric>

#include "normal_forms_transformations.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------

/// @cond FORWARD_DECL
template <NodeType node>
struct bf_reduce_canonical;
/// @endcond

/**
 * @brief Functor that converts a formula to Ordered Normal Form (ONF) w.r.t. a variable.
 *
 * Assumes the input formula is already in DNF. For each conjunct it rewrites
 * every equality `f = 0` (resp. `f != 0`) that contains the given variable
 * into an interval constraint `f[0] <= var <= f[1]` (resp. `f[0] > var || var > f[1]`),
 * where `f[0]` and `f[1]` are obtained by substituting 0 and 1 for the variable.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct onf_wff {
	/// @brief Construct with the variable to order with respect to.
	explicit onf_wff(tref _var);
	/// @brief Apply ONF conversion to formula `n`.
	tref operator()(tref n) const;
private:
	tref onf_subformula(tref n) const;
	tref var = nullptr;
};

/// @brief Type alias for `onf_wff<node>`.
template <NodeType node>
using onf_wff_t = onf_wff<node>;

/**
 * @brief Pipe operator: apply `onf_wff` to a tree traverser.
 * @tparam node Tree node type.
 */
template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t, const onf_wff_t<node>& r);

/**
 * @brief Convert formula `n` to Ordered Normal Form with respect to `var`.
 *
 * First converts to DNF, then applies `onf_wff` twice (to stabilize).
 * @tparam node Tree node type.
 * @param n Formula to convert.
 * @param var The variable that defines the ordering dimension.
 * @return Formula in ONF with respect to `var`.
 */
template <NodeType node>
tref onf(tref n, tref var);

/**
 * @brief Reduce a DNF or CNF formula by removing redundant clauses.
 *
 * Converts the formula to a path representation and eliminates dominated or
 * tautological clauses via `dnf_cnf_to_reduced`. Returns the simplified formula.
 * @tparam node Tree node type.
 * @tparam is_cnf When `true`, treat `fm` as CNF instead of DNF.
 * @param fm Formula in DNF (or CNF when `is_cnf` is `true`).
 * @return Reduced formula.
 */
template <NodeType node, bool is_cnf = false>
tref reduce(tref fm);

/**
 * @brief Compute the reduced DNF of a Boolean function.
 *
 * Enumerates all variable assignments via `assign_and_reduce` using the
 * lexicographic variable order `lex_var_comp`, then reconstructs the minimal
 * DNF from the resulting coefficient-path map.
 * @tparam node Tree node type.
 * @param fm Boolean function to reduce.
 * @param make_paths_disjoint When `true`, skip the `join_paths` merging step
 *        so that every surviving path is explicitly disjoint.
 * @return Reduced DNF of `fm`.
 */
template <NodeType node>
tref bf_reduced_dnf(tref fm, bool make_paths_disjoint = false);

/**
 * @brief Functor adapter that applies `bf_reduced_dnf` to every `bf` sub-tree.
 *
 * Allows `bf_reduce_canonical` to be used in the tree traversal pipeline via
 * `operator|`.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct bf_reduce_canonical {
	/// @brief Apply canonical DNF reduction to `fm`.
	tref operator()(tref fm) const;
};

/**
 * @brief Pipe operator: apply `bf_reduce_canonical` to a tree traverser.
 * @tparam node Tree node type.
 */
template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t,
	const bf_reduce_canonical<node>& r);

/**
 * @brief Functor that reduces a wff to DNF.
 *
 * Wraps `reduce<node, false>` so it can be used in the traversal pipeline.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct wff_reduce_dnf {
	/// @brief Reduce `fm` to DNF.
	tref operator() (tref fm) const;
};

/**
 * @brief Functor that reduces a wff to CNF.
 *
 * Wraps `reduce<node, true>` so it can be used in the traversal pipeline.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct wff_reduce_cnf {
	/// @brief Reduce `fm` to CNF.
	tref operator() (tref fm) const;
};

/**
 * @brief Pipe operator: apply `wff_reduce_dnf` to a tree traverser.
 * @tparam node Tree node type.
 */
template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const wff_reduce_dnf<node>& r);

/**
 * @brief Pipe operator: apply `wff_reduce_cnf` to a tree traverser.
 * @tparam node Tree node type.
 */
template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const wff_reduce_cnf<node>& r);

/**
 * @brief Convert a formula to Disjunctive Normal Form (DNF).
 *
 * Pushes negations in, then distributes conjunctions over disjunctions while
 * interleaving reductions via `wff_reduce_dnf` (or `reduce` for bf).
 * @tparam node Tree node type.
 * @tparam is_wff `true` for wff, `false` for bf (default: `true`).
 * @param fm Formula to convert.
 * @return Equivalent formula in DNF.
 */
template <NodeType node, bool is_wff = true>
tref to_dnf(tref fm);

/**
 * @brief Convert a formula to Conjunctive Normal Form (CNF).
 *
 * Pushes negations in, then distributes disjunctions over conjunctions while
 * interleaving reductions via `wff_reduce_cnf` (or `reduce<true>` for bf).
 * @tparam node Tree node type.
 * @tparam is_wff `true` for wff, `false` for bf (default: `true`).
 * @param fm Formula to convert.
 * @return Equivalent formula in CNF.
 */
template <NodeType node, bool is_wff = true>
tref to_cnf(tref fm);

/**
 * @brief Convert a formula to Negation Normal Form (NNF).
 *
 * Convenience wrapper around `push_negation_in<node, true>`.
 * @tparam node Tree node type.
 * @param fm Formula to convert.
 * @return Equivalent formula in NNF.
 */
template <NodeType node>
tref to_nnf(tref fm);

/**
 * @brief Convert a formula to Boole Normal Form (full procedure).
 *
 * The full four-step procedure:
 *   1. Syntactic simplification.
 *   2. Squeeze-absorb pre-processing.
 *   3. Term Boole decomposition for all equations.
 *   4. Formula-level Boole decomposition (`rec_boole_decomposition`).
 * Finally applies `to_nnf` and `simplify_using_equality`.
 * @tparam node Tree node type.
 * @param formula Formula to normalize.
 * @return Formula in Boole normal form.
 */
template <NodeType node>
tref boole_normal_form(tref formula);

/**
 * @brief Convert a formula to term-level Boole Normal Form.
 *
 * A lighter variant of `boole_normal_form` that only applies syntactic
 * simplification and term-level Boole decomposition, skipping the
 * formula-level BDD step.
 * @tparam node Tree node type.
 * @param formula Formula to normalize.
 * @return Formula with all terms in Boole normal form.
 */
template <NodeType node>
tref term_boole_normal_form(tref formula);

/**
 * @brief Apply the anti-prenex transformation to a formula.
 *
 * Drives the full anti-prenex procedure: converts to NNF, identifies
 * quantifier blocks, and calls `anti_prenex_block` to push quantifiers into
 * the formula structure as deeply as possible.
 * @tparam node Tree node type.
 * @param formula Formula to anti-prenex.
 * @return Formula with quantifiers pushed in as far as possible.
 */
template <NodeType node>
tref anti_prenex(tref formula);


/**
 * @brief Convert a formula to Algebraic Normal Form (ANF) for a given type.
 *
 * The ANF is the XOR-AND normal form where each variable appears at most once
 * per monomial. This is used for specific BA types identified by `type`.
 * @tparam node Tree node type.
 * @tparam type BA type identifier.
 * @param n Formula to convert.
 * @return Formula in ANF.
 */
template <NodeType node, size_t type>
tref anf(tref n);

/**
 * @brief Convert a formula to Prenex Normal Form (PNF).
 *
 * Moves all quantifiers to the outermost position, producing a formula of the
 * form `Q1 x1. Q2 x2. ... Qn xn. matrix` where the matrix is quantifier-free.
 * @tparam node Tree node type.
 * @param n Formula to convert.
 * @return Formula in PNF.
 */
template <NodeType node>
tref pnf(tref n);

} // namespace idni::tau_lang

// This is the proper way to include heuristics as the headre mustbe independent
// of the heuristics themselves and also they could need definitions from the
// header (as is the case in 'heuristicsbv_ba_simplification.h'. Also, they
// need to be included before the definitions as they can be used in there.
#include "heuristics/ex_subs_based_elimination.h"
#include "heuristics/syntactic_path_simplification.h"
#include "normal_forms.tmpl.h"

#endif // __IDNI__TAU__NORMAL_FORMS_H__
