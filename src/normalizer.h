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

#include "nso_rr.h"

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau_lang {

/**
 * @brief Normalize a Tau formula, handling both temporal and non-temporal cases.
 *
 * For formulas without temporal quantifiers (`always`/`sometimes`), applies
 * `eliminate_bv_and_quantifiers` (see normalizer.tmpl.h), whose five steps are:
 *  1. `scope_out_independent_conjuncts` -- lift every conjunct that does not
 *     mention a quantified variable out of that variable's scope, so a
 *     foreign-typed sibling conjunct cannot stop a bitvector scope from being
 *     recognised as closed and solvable;
 *  2. `resolve_quantifiers` -- decide or blast bitvector-typed scopes;
 *  3. `anti_prenex_block` skipping bitvector-typed content and any variable
 *     entangled with an unresolved `wff_ref`, then `resolve_quantifiers` again;
 *  4. `anti_prenex_block` once more, skipping bitvector-typed content, the
 *     bitvector arithmetic residue blasting could not resolve, and the
 *     `wff_ref`-entangled variables, then a final `resolve_quantifiers`;
 *  5. if the result is closed and bitvector-solvable, ask the solver for a
 *     definite `sat`/`unsat` and collapse to `T`/`F` on one -- an `unknown`
 *     or a failed translation leaves the formula as it is, quantifiers
 *     included, since "cannot decide" is not "false".
 *
 * For formulas with temporal quantifiers, the same pipeline is applied to
 * each inner formula below a temporal quantifier, then the temporal layer
 * is normalized via `normalize_temporal_quantifiers`.
 *
 * @tparam node Tree node type.
 * @param form The formula to normalize.
 * @return Normalized formula.
 *
 * @par Example
 * @code{.cpp}
 * // Non-temporal tautology reduces to T
 * tref fm1 = get_nso_rr("x = 0 || x != 0.").value().main->get();
 * CHECK( tau::get(normalize<node_t>(fm1)).equals_T() );
 *
 * // Temporal case: "ex t [t > 3]" normalizes into a formula wrapped in
 * // "always" (see tests/integration/test_integration-wff_normalization.cpp:34-37,
 * // which checks this via the fuller normalizer<node_t> pipeline).
 * tref fm2 = get_nso_rr("ex t [t > 3].").value().main->get();
 * tref res2 = normalize<node_t>(fm2);
 * CHECK( tau::get(res2).child_is(tau::wff_always) );
 * @endcode
 */
template <NodeType node>
tref normalize(tref form);

/**
 * @brief Fold trivial quantifiers and Boolean identities in a WFF.
 *
 * Simplifies:
 *   - `ex x T → T`, `ex x F → F`, `all x T → T`, `all x F → F`
 *   - `T && A → A`, `A && T → A`, `F && A → F`, `A && F → F`
 *   - `T || A → T`, `A || T → T`, `F || A → A`, `A || F → A`
 *
 * @tparam node Tree node type.
 * @param fm Formula to simplify.
 * @return Simplified formula.
 *
 * @par Example
 * @code{.cpp}
 * // ex x T -> T ; a non-trivial quantifier body is left untouched
 * // (see tests/unit/test_normal_forms.cpp:261-266, 289-294).
 * tref x = build_variable<node_t>("x", tau_type_id<node_t>());
 * tref fm1 = tau::build_wff_ex(x, tau::_T(), false);
 * CHECK( tau::get(fold_trivial_quantifiers<node_t>(fm1)).equals_T() );
 *
 * tref fm2 = get_nso_rr("ex x x = 0.").value().main->get();
 * tref res2 = fold_trivial_quantifiers<node_t>(fm2);
 * CHECK( tau::get(res2).find_top(is_quantifier<node_t>) != nullptr );
 * @endcode
 */
template <NodeType node>
tref fold_trivial_quantifiers(tref fm);

/**
 * @brief Normalize a non-temporal formula.
 *
 * Assumes the formula contains no `always`/`sometimes` quantifiers. Applies
 * `eliminate_bv_and_quantifiers` (see `normalize` and normalizer.tmpl.h),
 * then `term_boole_normal_form`.
 *
 * Note: `fold_trivial_quantifiers` is deliberately omitted to preserve bitwidth
 * subtypes (see NOTE in implementation); residual trivial quantifiers are folded
 * later by `normalize_with_temp_simp`.
 *
 * @tparam node Tree node type.
 * @param fm Non-temporal formula to normalize.
 * @return Normalized formula.
 *
 * @par Example
 * @code{.cpp}
 * // See tests/integration/test_integration-wff_normalization.cpp:38-42.
 * tref fm = get_nso_rr(
 *     "{ !i5[t] = <:x> || o5[t] = <:y> } : tau = u[0].").value().main->get();
 * tref res = normalize_non_temp<node_t>(fm);
 * // tau::get(res).to_str() ==
 * //   "u[0]:tau = { always i5[t]:tau != <:x> || o5[t]:tau = <:y> }:tau"
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // Existing ":split1" and ":split3" -> next fresh name is ":split4"
 * // (see tests/unit/test_normal_forms.cpp:247-255).
 * tref c1 = build_bf_uconst<node_t>("", "split1", tau_type_id<node_t>());
 * tref c3 = build_bf_uconst<node_t>("", "split3", tau_type_id<node_t>());
 * tref fm = tau::build_bf_or(c1, c3);
 * tref result = get_new_uninterpreted_constant<node_t>(fm, "split", tau_type_id<node_t>());
 * trefs names = tau::get(result).select_top(is<node_t, tau::uconst_name>);
 * // names.size() == 1 && tau::get(names[0]).get_string() == ":split4"
 * @endcode
 */
template <NodeType node>
tref get_new_uninterpreted_constant(tref fm, const std::string& name, size_t type);

/**
 * @brief Check that a formula does not use Boolean combinations of models.
 *
 * What is actually checked (NF-9): nested `wff_always` and nested `wff_F`
 * only — nested `wff_sometimes` is NOT detected, so e.g.
 * `(sometimes a) && (sometimes b)` passes the predicate; and any formula
 * containing a full-LTL / CTL* operator (F/U/R/W/A/E/semantic_neg) is
 * exempted and returns `true` unconditionally (those manage their own
 * temporal scope). Callers use this only in DBG asserts, so the gap
 * weakens a debug guard rather than a runtime result.
 * @tparam node Tree node type.
 * @param n Formula to inspect.
 * @return `true` if no Boolean combination of models is present.
 *
 * @par Example
 * @code{.cpp}
 * // A single top-level "always" satisfies the predicate; conjoining two
 * // "always"-wrapped models violates it
 * // (see tests/unit/test_normal_forms.cpp:307-318).
 * tref fm1 = get_nso_rr("always x = 0.").value().main->get();
 * CHECK( has_no_boolean_combs_of_models<node_t>(fm1) );
 *
 * tref fm2 = get_nso_rr(
 *     "(always x = 0) && (always y = 0).").value().main->get();
 * CHECK( !has_no_boolean_combs_of_models<node_t>(fm2) );
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * tref fm1 = get_nso_rr("x = 0.").value().main->get();
 * CHECK( is_non_temp_nso_satisfiable<node_t>(fm1) );
 *
 * tref fm2 = get_nso_rr("x = 0 && x != 0.").value().main->get();
 * CHECK( !is_non_temp_nso_satisfiable<node_t>(fm2) );
 * @endcode
 */
template <NodeType node>
bool is_non_temp_nso_satisfiable(tref n);

/**
 * @brief Find a relative offset in a definition that its head cannot bind.
 *
 * A head declaring no offsets binds no offset variable, so a reference in the
 * body carrying a relative offset is free — `f(x) := o1[n] = r[n](x)` has
 * nothing to give `n` a value. Expanding such a definition can only put a
 * relative offset into the main formula, which `is_valid` rejects; but the
 * expansion is driven by that very offset (`r[n]` → `r[n-1]` → `r[n-1-1]` →
 * …) and never terminates, so the main never gets far enough to be checked.
 * Callers reject the definition up front instead.
 *
 * Offsets on stream variables (`o1[n]`) are unaffected: those are resolved per
 * time step by the interpreter, not by unfolding a definition.
 * @tparam node Tree node type.
 * @param head The definition's head (its `ref`, or a node wrapping one).
 * @param body The definition's body.
 * @return The offending `ref` node, or `nullptr` when the definition is fine.
 *
 * @par Example
 * @code{.cpp}
 * // `f` declares no offset, so the `n` in `r[n](x)` is unbound
 * auto spec = get_nso_rr("r[0](x) := 1. r[n](x) := r[n-1](x)'."
 *     "f(x) := o1[n] = r[n](x). f(1).").value();
 * const auto& f = spec.rec_relations[2];
 * CHECK( get_unbindable_relative_offset<node_t>(
 *     f.first->get(), f.second->get()) != nullptr );
 * // `r` declares `[n]`, which binds the `n-1` in its own body
 * const auto& r = spec.rec_relations[1];
 * CHECK( get_unbindable_relative_offset<node_t>(
 *     r.first->get(), r.second->get()) == nullptr );
 * @endcode
 */
template <NodeType node>
tref get_unbindable_relative_offset(tref head, tref body);

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
 *
 * @par Example
 * @code{.cpp}
 * // x=0 and !(x!=0) are equivalent, but structurally different
 * // (see tests/unit/test_normal_forms.cpp:335-349).
 * tref n1 = get_nso_rr("x = 0.").value().main->get();
 * tref n2 = get_nso_rr("!(x != 0).").value().main->get();
 * CHECK( are_nso_equivalent<node_t>(n1, n2) );
 *
 * tref n3 = get_nso_rr("y = 0.").value().main->get();
 * CHECK( !are_nso_equivalent<node_t>(n1, n3) );
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // x=0 && y=0 implies x=0, but not vice versa (y is unconstrained)
 * // (see tests/unit/test_normal_forms.cpp:352-363).
 * tref n1 = get_nso_rr("x = 0 && y = 0.").value().main->get();
 * tref n2 = get_nso_rr("x = 0.").value().main->get();
 * CHECK( is_nso_impl<node_t>(n1, n2) );
 * CHECK( !is_nso_impl<node_t>(n2, n1) );
 * @endcode
 */
template <NodeType node>
bool is_nso_impl(tref n1, tref n2);

/**
 * @brief Normalize a formula with temporal simplifications.
 *
 * Full normalization pipeline including:
 *   0. `flatten_always_conjuncts` — merges top-level `(G A) && (G B)` into
 *      `G(A && B)` first; load-bearing (NF-10: without it the second G is
 *      silently dropped downstream, which can flip a satisfiable spec).
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
 *
 * @par Example
 * @code{.cpp}
 * // Same pipeline that backs normalizer(tref) (a thin wrapper around this
 * // function): a tautology reduces to T.
 * tref fm = get_nso_rr("x = 0 || x != 0.").value().main->get();
 * CHECK( tau::get(normalize_with_temp_simp<node_t>(fm)).equals_T() );
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // "1 & 0" -> bf_f ; "X | X'" -> bf_t
 * // (see tests/integration/test_integration-bf_normalization.cpp:18-25, 31-36).
 * auto pbf = parse_bf();
 * tref fm1 = tau::get("1 & 0", pbf);
 * auto nso_rr1 = get_nso_rr<node_t>(fm1).value();
 * tref res1 = bf_normalizer_without_rec_relation<node_t>(nso_rr1.main->get());
 * CHECK( tau::get(res1).child_is(tau::bf_f) );
 *
 * tref fm2 = tau::get("X | X'", pbf);
 * auto nso_rr2 = get_nso_rr<node_t>(fm2).value();
 * tref res2 = bf_normalizer_without_rec_relation<node_t>(nso_rr2.main->get());
 * CHECK( tau::get(res2).child_is(tau::bf_t) );
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // h(X):tau := 1., query h(Y): unfolds the recurrence to just "1"
 * // (see tests/integration/test_integration-bf_normalization.cpp:88-96, "Simple case (y1)").
 * auto nso_rr = get_bf_nso_rr("h(X):tau := 1.", "h(Y)").value();
 * tref res = bf_normalizer_with_rec_relation<node_t>(nso_rr);
 * CHECK( tau::get(res).child_is(tau::bf_t) );
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * // See tests/integration/test_integration-wff_normalization.cpp:7-9 ("Normalizer" / "1").
 * const char* sample =
 *     "all a,b,c,d a'c|b'd = 0 <-> a & b' & d | a' & c | b' & c' & d = 0.";
 * auto nso_rr = get_nso_rr(sample).value();
 * tref res = normalizer<node_t>(nso_rr);
 * CHECK( tau::get(res).child_is(tau::wff_t) );
 * @endcode
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
 *
 * @par Example
 * @code{.cpp}
 * tref fm = get_nso_rr("x = 0 || x != 0.").value().main->get();
 * CHECK( tau::get(normalizer<node_t>(fm)).equals_T() );
 * @endcode
 */
template <NodeType node>
tref normalizer(tref fm);

/**
 * @brief Normalize temporal quantifiers (`always`/`sometimes`) in a formula.
 *
 * Converts the temporal layer of the formula to DNF and simplifies by
 * squeezing `always` statements, then (when `normalize_scopes` is `true`)
 * normalizes the inner formulas below temporal quantifiers.
 *
 * @tparam node Tree node type.
 * @tparam normalize_scopes When `true` (default) also normalize inner formulas.
 * @param fm Formula to normalize.
 * @return Formula with normalized temporal quantifiers.
 *
 * @par Example
 * @code{.cpp}
 * // "(always x=0) && (always x=0)": the duplicate always-clauses squeeze
 * // together and the (now redundant) temporal wrapper is dropped entirely.
 * tref fm = get_nso_rr(
 *     "(always x = 0) && (always x = 0).").value().main->get();
 * tref res = normalize_temporal_quantifiers<node_t>(fm);
 * // tau::get(res).to_str() == "x = 0"
 * CHECK( !tau::get(res).child_is(tau::wff_always) );
 * @endcode
 */
template <NodeType node, bool normalize_scopes = true>
tref normalize_temporal_quantifiers(tref fm);

} // namespace idni::tau_lang

#include "normalizer.tmpl.h"

#endif // __IDNI__TAU__NORMALIZER_H__
