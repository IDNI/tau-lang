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
 *
 * @par Example
 * @code{.cpp}
 * // "x = y" and "y = x" describe the same equation; ONF w.r.t. x rewrites
 * // both to the same canonical interval form (see
 * // tests/unit/test_normal_forms.cpp:218-227).
 * tref x = build_variable<node_t>("x", tau_type_id<node_t>());
 * tref fm_lhs = get_nso_rr("x = y.").value().main->get();
 * tref fm_rhs = get_nso_rr("y = x.").value().main->get();
 * tref result_lhs = onf<node_t>(fm_lhs, x);
 * tref result_rhs = onf<node_t>(fm_rhs, x);
 * // tau::get(result_lhs).to_str() == tau::get(result_rhs).to_str()
 * //   == "yx' = 0 && xy' = 0"
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // A contradiction reduces to F, a tautology to T
 * // (see tests/unit/test_normal_forms.cpp:771-784).
 * tref contradiction = get_nso_rr("x = 0 && x != 0.").value().main->get();
 * tref tautology     = get_nso_rr("x = 0 || x != 0.").value().main->get();
 * CHECK( tau::get(reduce<node_t>(contradiction)).equals_F() );
 * CHECK( tau::get(reduce<node_t>(tautology)).equals_T() );
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // ab|ab' = a(b|b') = a; ab|a'b = (a|a')b = b
 * // (see tests/unit/test_normal_forms.cpp:786-802).
 * tref fm1 = get_nso_rr("ab|ab' = 0.").value().main->get();
 * tref bf1 = tau::get(fm1)[0].first();
 * CHECK( tau::get(bf_reduced_dnf<node_t>(bf1)).to_str() == "a" );
 *
 * tref fm2 = get_nso_rr("ab|a'b = 0.").value().main->get();
 * tref bf2 = tau::get(fm2)[0].first();
 * CHECK( tau::get(bf_reduced_dnf<node_t>(bf2)).to_str() == "b" );
 * @endcode
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
	///
	/// @par Example
	/// @code{.cpp}
	/// // A tautology over uninterpreted constants <:a>, <:b>, <:c> reduces to T
	/// // (see tests/unit/test_normal_forms.cpp:96-108).
	/// tref fm = tt(tau::get(uninterp_constants_sample))
	///     | tau::spec | tau::main | tau::wff
	///     | bf_reduce_canonical<node_t>() | tt::ref;
	/// CHECK( tau::get(fm) == tau::get_T() );
	/// @endcode
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
	///
	/// @par Example
	/// @code{.cpp}
	/// // Thin wrapper over reduce<node, false>: a contradiction reduces to F
	/// tref fm = get_nso_rr("x = 0 && x != 0.").value().main->get();
	/// tref res = tt(fm) | wff_reduce_dnf<node_t>() | tt::ref;
	/// CHECK( tau::get(res).equals_F() );
	/// @endcode
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
	///
	/// @par Example
	/// @code{.cpp}
	/// // Thin wrapper over reduce<node, true>: a tautology reduces to T
	/// tref fm = get_nso_rr("x = 0 || x != 0.").value().main->get();
	/// tref res = tt(fm) | wff_reduce_cnf<node_t>() | tt::ref;
	/// CHECK( tau::get(res).equals_T() );
	/// @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // x=0 && (y=0 || z=0) distributes to (x=0 && y=0) || (x=0 && z=0)
 * // (see tests/unit/test_normal_forms.cpp:740-750).
 * tref fm = get_nso_rr("x = 0 && (y = 0 || z = 0).").value().main->get();
 * tref res = to_dnf<node_t, true>(fm);
 * // tau::get(res).to_str() == "x = 0 && y = 0 || x = 0 && z = 0"
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // (x=0||y=0) && (z=0||w=0) is already in CNF, so to_cnf is a no-op here
 * // (see tests/unit/test_normal_forms.cpp:762-768).
 * tref fm = get_nso_rr("(x = 0 || y = 0) && (z = 0 || w = 0).").value().main->get();
 * tref res = to_cnf<node_t, true>(fm);
 * // tau::get(res).to_str() == "(x = 0 || y = 0) && (z = 0 || w = 0)"
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // Double negation collapses; De Morgan's law distributes negation over &&
 * // (see tests/unit/test_normal_forms.cpp:712-727).
 * tref fm1 = get_nso_rr("!!(a = 0).").value().main->get();
 * tref res1 = to_nnf<node_t>(fm1);
 * // tau::get(res1).to_str() == "a = 0"
 *
 * tref fm2 = get_nso_rr("!(a = 0 && b = 0).").value().main->get();
 * tref res2 = to_nnf<node_t>(fm2);
 * // tau::get(res2).to_str() == "a != 0 || b != 0"
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // f(0,0)f(0,1)=0 && f(1,1)f(1,0)=0 && (f(1,0)f(1,1)|f(0,1)f(0,0) != 0) is
 * // unsatisfiable: the first two conjuncts force every f(i,j) product to be
 * // 0, contradicting the third (see
 * // tests/integration/test_integration-wff_normalization.cpp:288-293).
 * tref fm = get_nso_rr(
 *     "f(0, 0)f(0, 1) = 0 && f(1, 1)f(1, 0) = 0 && "
 *     "f(1, 0)f(1, 1)|f(0, 1)f(0, 0) != 0.").value().main->get();
 * tref res = boole_normal_form<node_t>(fm);
 * CHECK( tau::get(res).equals_F() );
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // xy|xy' = x(y|y') = x
 * tref fm = get_nso_rr("xy|xy' = 0.").value().main->get();
 * tref res = term_boole_normal_form<node_t>(fm);
 * // tau::get(res).to_str() == "x = 0"
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // The inner "ex o2[1],o1[1] o1[1]o2[1]=0" is always satisfiable (pick
 * // o1[1]=o2[1]=0), so the whole formula reduces to a tautology once the
 * // quantifier is pushed in and resolved (see
 * // tests/integration/test_integration-wff_normalization.cpp:131-136).
 * tref fm = get_nso_rr(
 *     "all o1[0], o2[0] !o1[0]o2[0] = 0 || o1[0]o2[0] = 0 && "
 *     "(ex o2[1], o1[1] o1[1]o2[1] = 0).").value().main->get();
 * tref res = anti_prenex<node_t>(fm);
 * CHECK( tau::get(res).equals_T() );
 * @endcode
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
 *
 * @warning Not implemented yet (normal_forms.tmpl.h): the current body prints
 * "Not implemented yet." and returns @p n unchanged. No worked example is
 * given here since the function is currently the identity.
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
 *
 * @warning Not implemented yet (normal_forms.tmpl.h): the current body prints
 * "Not implemented yet." and returns @p n unchanged. No worked example is
 * given here since the function is currently the identity.
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
#include "heuristics/bv_arithmetic_scan.h"
#include "heuristics/trivial_skolem.h"
#include "normal_forms.tmpl.h"

#endif // __IDNI__TAU__NORMAL_FORMS_H__
