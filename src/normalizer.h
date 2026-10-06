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

#include "tau_diagnostics.h"

#include "nso_rr.h"

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau_lang {

/**
 * @brief Normalize a Tau formula, handling both temporal and non-temporal cases.
 *
 * Functional quantifiers are evaluated first (`eliminate_functional_quantifiers`).
 * For formulas without temporal quantifiers (`always`/`sometimes`), applies
 * `eliminate_arithmetic_and_quantifiers` (see normalizer.tmpl.h), whose steps are:
 *  0. the pack's definitional-existential elimination and case split
 *     (`pack_eliminate_definitional_existentials`,
 *     `pack_case_split_quantifiers`);
 *  1. `scope_out_independent_conjuncts` -- lift every conjunct that does not
 *     mention a quantified variable out of that variable's scope, so a
 *     foreign-typed sibling conjunct cannot stop an arithmetic scope from
 *     being recognised as closed and solvable;
 *  2. `resolve_quantifiers` -- decide or blast arithmetic-typed scopes;
 *  3. an eliminability analysis of the formula, then `anti_prenex` guided by
 *     it (skipping arithmetic-typed content the analysis marks as the
 *     solver's to decide), then `resolve_quantifiers` again;
 *  4. a second eliminability pass -- narrower where a foreign Boolean
 *     algebra's constant means the solver cannot own the content -- drives
 *     `anti_prenex` and `resolve_quantifiers` once more; when the pack has an
 *     arithmetic theory, an optional whole-formula preprocessing attempt
 *     follows (`preprocess_placement == per_formula`);
 *  5. with an arithmetic theory in the pack, if the result is closed and
 *     solvable, ask the solver for a definite
 *     `sat`/`unsat` and collapse to `T`/`F` on one -- an `unknown` or a
 *     failed translation leaves the formula as it is, quantifiers included,
 *     since "cannot decide" is not "false".
 *
 * For formulas with temporal quantifiers, the same pipeline is applied to
 * each inner formula below an `always`/`sometimes`; a full-LTL or CTL*
 * operator keeps its shape and only has its temporal-free operands folded to
 * `T`/`F` where `normalize_non_temp` decides them. Then the temporal layer is
 * normalized via `normalize_temporal_quantifiers`. Cached per input under
 * `TAU_CACHE`.
 *
 * @tparam node Tree node type.
 * @param form The formula to normalize; must not be null.
 * @return Normalized formula; an `invalid_argument` error for a null
 * @p form, `internal_error` when no formula results, or the error of a
 * failing step.
 *
 * @par Example
 * @code{.cpp}
 * // Non-temporal tautology reduces to T
 * tref fm1 = get_nso_rr("x = 0 || x != 0.").value().main->get();
 * CHECK( tau::get(normalize<node_t>(fm1).value()).equals_T() );
 *
 * // Temporal case: "ex t [t > 3]" normalizes into a formula wrapped in
 * // "always" (the "Normalizer" test case "7" in
 * // tests/integration/test_integration-wff_normalization.cpp checks this via
 * // the fuller normalizer<node_t> pipeline).
 * tref fm2 = get_nso_rr("ex t [t > 3].").value().main->get();
 * tref res2 = normalize<node_t>(fm2).value();
 * CHECK( tau::get(res2).child_is(tau::wff_always) );
 * @endcode
 */
template <NodeType node>
result<tref> normalize(tref form);

/**
 * @brief Fold trivial quantifiers and Boolean identities in a WFF.
 *
 * Simplifies, bottom-up:
 *   - `ex x T → T`, `ex x F → F`, `all x T → T`, `all x F → F`
 *   - `!T → F`, `!F → T`
 *   - `T && A → A`, `A && T → A`, `F && A → F`, `A && F → F`
 *   - `T || A → T`, `A || T → T`, `F || A → A`, `A || F → A`
 *
 * Such residues are left by substitution-based eliminations, which rebuild
 * nodes without running the construction hooks.
 * @tparam node Tree node type.
 * @param fm Formula to simplify.
 * @return Simplified formula.
 *
 * @par Example
 * @code{.cpp}
 * // ex x T -> T ; a non-trivial quantifier body is left untouched
 * // (see the FoldTrivialQuantifiers suite in tests/unit/test_normal_forms.cpp).
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
 * Assumes the formula contains no `always`/`sometimes` quantifiers. Evaluates
 * functional quantifiers, applies `eliminate_arithmetic_and_quantifiers` (see
 * `normalize` and normalizer.tmpl.h), then `term_boole_normal_form`, then
 * `fold_trivial_quantifiers`, and finally lets the owning BA decide a formula
 * with no variable whose constants are all of one type
 * (`pack_decide_ground`). Cached under `TAU_CACHE`.
 *
 * `tau::reget` is deliberately not applied to the result: it would strip the
 * explicit bitwidth subtypes of bitvector-typed nodes.
 *
 * A BA offering arithmetic widening (see `pack_widen_arithmetic`,
 * ba_pack_traits.h) elaborates the formula's atoms first, so the cache
 * is keyed on the already-widened formula.
 *
 * @tparam node Tree node type.
 * @param fm Non-temporal formula to normalize; must not be null.
 * @return Normalized formula; an `invalid_argument` error for a null @p fm,
 * an `internal_error` when the widening answers no formula (an atom exceeds
 * the width cap) or the Boole normal form produces none, or the error of a
 * failing step.
 *
 * @par Example
 * @code{.cpp}
 * // See the "Normalizer" test case "8" in
 * // tests/integration/test_integration-wff_normalization.cpp.
 * tref fm = get_nso_rr(
 *     "{ !i5[t] = <:x> || o5[t] = <:y> } : tau = u[0].").value().main->get();
 * tref res = normalize_non_temp<node_t>(fm).value();
 * // tau::get(res).to_str() ==
 * //   "u[0]:tau = { always i5[t]:tau != <:x> || o5[t]:tau = <:y> }:tau"
 * @endcode
 */
template <NodeType node>
result<tref> normalize_non_temp(tref fm);

/**
 * @brief Evaluates `fex x f` to `f[x:=0] | f[x:=1]` and `fall x f` to
 * `f[x:=0] & f[x:=1]` wherever the body is Boolean, and gives every binder it
 * must keep a name that occurs nowhere else in @p fm.
 *
 * Every normalization entry runs it first: the Boole decomposition that
 * follows substitutes a variable without regard to a functional binder of the
 * same name. A body with arithmetic, casts, min/max, function references or
 * a nested functional quantifier keeps its quantifier; such a binder is
 * renamed (`fq<N>`) only when its name also occurs free in @p fm.
 * @tparam node Tree node type.
 * @param fm Formula to rewrite.
 * @return The rewritten formula, or @p fm itself when it has no functional
 * quantifier.
 */
template <NodeType node>
tref eliminate_functional_quantifiers(tref fm);

/**
 * @brief Build a fresh uninterpreted constant of the given BA type not present in `fm`.
 *
 * Scans the `uconst_name` nodes in `fm` spelled `:<name><digits>` and
 * combines the largest such index (0 when none) with a process-wide,
 * mutex-guarded per-family counter, returning `name<i>` one past whichever is
 * larger. A suffix too long for `int` is ignored.
 * @tparam node Tree node type.
 * @param fm Formula to inspect for existing uninterpreted constants.
 * @param name Base name prefix for the new constant.
 * @param type BA type identifier for the new constant node.
 * @return A fresh uninterpreted constant node.
 *
 * @par Example
 * @code{.cpp}
 * // Existing ":split1" and ":split3" -> next fresh name is ":split4"
 * // (see the GetNewUninterpretedConstant suite in
 * // tests/unit/test_normal_forms.cpp).
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
 * What is actually checked: nested `wff_always` only — any formula
 * containing `wff_sometimes` or another full-LTL / CTL* operator
 * (U/R/W/S/T/A/E/semantic_neg) anywhere is exempted by the outer scan and
 * returns `true` unconditionally (those manage their own temporal scope),
 * so e.g. `(sometimes a) && (sometimes b)` passes the predicate. Otherwise a
 * formula passes when it is a single top-level `always` with no temporal
 * quantifier below it, or has no temporal quantifier at all. Besides DBG
 * asserts, the LTL(ABA) fast path and `to_unbounded_continuation` read it.
 * @tparam node Tree node type.
 * @param n Formula to inspect; must not be null.
 * @return `true` if no Boolean combination of models is present; an
 * `invalid_argument` error for a null @p n.
 *
 * @par Example
 * @code{.cpp}
 * // A single top-level "always" satisfies the predicate; conjoining two
 * // "always"-wrapped models violates it
 * // (see the HasNoBooleanCombsOfModels suite in tests/unit/test_normal_forms.cpp).
 * tref fm1 = get_nso_rr("always x = 0.").value().main->get();
 * CHECK( has_no_boolean_combs_of_models<node_t>(fm1).value() );
 *
 * tref fm2 = get_nso_rr(
 *     "(always x = 0) && (always y = 0).").value().main->get();
 * CHECK( !has_no_boolean_combs_of_models<node_t>(fm2).value() );
 * @endcode
 */
template <NodeType node>
result<bool> has_no_boolean_combs_of_models(tref n);

/**
 * @brief Determine whether a non-temporal NSO formula is satisfiable.
 *
 * Wraps all free variables of `n` with existential quantifiers, normalizes via
 * `normalize_non_temp`, and returns `true` if the result is `T`. A
 * conjunction of `capture = 0` / `capture != 0` atoms, one free capture each,
 * is decided directly without normalization; setting the environment
 * variable `TAU_LEAN_DECIDE_CROSSCHECK` runs the normalization as well and
 * reports an `internal_error` if the two disagree.
 * @tparam node Tree node type.
 * @param n Non-temporal formula to test (must not contain `always`/`sometimes`).
 * @return `true` if satisfiable, `false` if not; an error (UNKNOWN,
 * `code::solver_error`) when normalization leaves the closed formula
 * undecided, `internal_error` when normalization fails, `invalid_argument`
 * for a null @p n.
 *
 * @par Example
 * @code{.cpp}
 * tref fm1 = get_nso_rr("x = 0.").value().main->get();
 * CHECK( is_non_temp_nso_satisfiable<node_t>(fm1).value() );
 *
 * tref fm2 = get_nso_rr("x = 0 && x != 0.").value().main->get();
 * CHECK( !is_non_temp_nso_satisfiable<node_t>(fm2).value() );
 * @endcode
 */
template <NodeType node>
result<bool> is_non_temp_nso_satisfiable(tref n);

/**
 * @brief Checks whether a non-temporal NSO formula is unsatisfiable.
 *
 * Wraps free variables with existential quantifiers, normalizes via
 * `normalize_non_temp`, and returns `true` if the result is `F`.
 * @tparam node Tree node type.
 * @param n The non-temporal formula to test (must not contain
 * `always`/`sometimes`).
 * @return `true` if the formula is unsatisfiable, `false` if it is
 * satisfiable; an error (UNKNOWN, `code::solver_error`) when normalization
 * leaves it undecided, `internal_error` when normalization fails,
 * `invalid_argument` for a null @p n.
 *
 * @par Example
 * @code{.cpp}
 * tref fm = get_nso_rr("x = 0 && x != 0.").value().main->get();
 * CHECK( is_non_temp_nso_unsat<node_t>(fm).value() );
 * @endcode
 */
template <NodeType node>
result<bool> is_non_temp_nso_unsat(tref n);

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
 * A top-level `always` is stripped from each side first. Then:
 *   1. Structural equality answers `true`.
 *   2. When both reach a `ref` through single-child nodes, they are
 *      equivalent only as equal trees; a ref against a non-ref is `false`.
 *   3. Otherwise `all vars (n1 -> n2)` and `all vars (n2 -> n1)` are
 *      normalized with `normalize_non_temp`.
 *
 * DBG-asserts that neither formula uses Boolean combinations of models.
 * @tparam node Tree node type.
 * @param n1 First formula.
 * @param n2 Second formula.
 * @return `true` if `n1` and `n2` are equivalent; `false` also when
 * normalization fails or leaves a direction undecided (a logged,
 * conservative fallback, not a proof).
 *
 * @par Example
 * @code{.cpp}
 * // x=0 and !(x!=0) are equivalent, but structurally different
 * // (see the AreNsoEquivalentAndIsNsoImpl suite in
 * // tests/unit/test_normal_forms.cpp).
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
 * Decides the validity of `all x. (n1 => n2)` (with `x` ranging over all
 * free variables). A top-level `always` is stripped from each side, equal
 * sides answer `true`, and consequent conjuncts that are literally
 * antecedent conjuncts are dropped. The conjuncts of both sides are then
 * grouped into variable-disjoint components and each component's
 * implication is normalized on its own, an unsatisfiable other component's
 * antecedent being checked only on demand; with at most two conjuncts in
 * all, the whole implication is normalized in one piece.
 * @tparam node Tree node type.
 * @param n1 Antecedent formula; must not be null.
 * @param n2 Consequent formula; must not be null.
 * @return `true` if `n1 => n2` is valid, `false` if not; an error
 * (UNKNOWN, `code::solver_error`) when normalization leaves an implication
 * undecided, `internal_error` when it fails, `invalid_argument` for a null
 * argument.
 *
 * @par Example
 * @code{.cpp}
 * // x=0 && y=0 implies x=0, but not vice versa (y is unconstrained)
 * // (see the AreNsoEquivalentAndIsNsoImpl suite in
 * // tests/unit/test_normal_forms.cpp).
 * tref n1 = get_nso_rr("x = 0 && y = 0.").value().main->get();
 * tref n2 = get_nso_rr("x = 0.").value().main->get();
 * CHECK( is_nso_impl<node_t>(n1, n2).value() );
 * CHECK( !is_nso_impl<node_t>(n2, n1).value() );
 * @endcode
 */
template <NodeType node>
result<bool> is_nso_impl(tref n1, tref n2);

/**
 * @brief Normalize a formula with temporal simplifications.
 *
 * Full normalization pipeline including:
 *   0. `eliminate_functional_quantifiers`, then `flatten_always_conjuncts` —
 *      merges top-level `(G A) && (G B)` into `G(A && B)`; load-bearing
 *      (without it the second G is dropped downstream, which can flip a
 *      satisfiable spec).
 *   1. `normalize` (with temporal quantifiers).
 *   2. `fold_trivial_quantifiers` (remove vacuous quantifiers after substitution).
 *   3. Late `resolve_quantifiers` for residual arithmetic sub-formulas, and
 *      a block-local re-elimination of each surviving quantifier block,
 *      adopted only when it leaves no quantifier.
 *   4. Application of registered function/predicate definitions until none
 *      applies (bounded by `max_def_passes`; an oscillating or unbounded
 *      expansion is an error).
 *   5. A formula left without temporal quantifiers goes to the owning BA's
 *      ground decision; one with a full-LTL or CTL* operator is returned as
 *      is; otherwise temporal layer simplification removes implied
 *      `always`/`sometimes` parts per DNF clause.
 *
 * A BA offering arithmetic widening (see `pack_widen_arithmetic`,
 * ba_pack_traits.h) elaborates the atoms before step 0.
 *
 * @tparam node Tree node type.
 * @param fm Formula to normalize; must not be null.
 * @return Fully normalized formula; an `invalid_argument` error for a null
 * @p fm, an `internal_error` when the widening answers no formula (an atom
 * exceeds the width cap) or normalization fails, or the error of a failing
 * step.
 *
 * @par Example
 * @code{.cpp}
 * // Same pipeline that backs normalizer(tref) (a thin wrapper around this
 * // function): a tautology reduces to T.
 * tref fm = get_nso_rr("x = 0 || x != 0.").value().main->get();
 * CHECK( tau::get(normalize_with_temp_simp<node_t>(fm).value()).equals_T() );
 * @endcode
 */
template <NodeType node>
result<tref> normalize_with_temp_simp(tref fm);

/**
 * @brief Normalize a Boolean function that has no recurrence relation.
 *
 * Applies `syntactic_path_simplification` followed by `bf_reduced_dnf`.
 * Also resolves any present function/predicate definitions iteratively
 * (bounded by `max_def_passes`).
 * @tparam node Tree node type.
 * @param bf Boolean function (without recurrence relations).
 * @return Normalized Boolean function in reduced DNF, or the error of the
 * reduction or of a definition expansion that never settles.
 *
 * @par Example
 * @code{.cpp}
 * // "1 & 0" -> bf_f ; "X | X'" -> bf_t
 * // (see the "True and False" and "X or X'" cases in
 * // tests/integration/test_integration-bf_normalization.cpp).
 * auto pbf = parse_bf();
 * tref fm1 = tau::get("1 & 0", pbf);
 * auto nso_rr1 = get_nso_rr<node_t>(fm1).value();
 * tref res1 = bf_normalizer_without_rec_relation<node_t>(nso_rr1.main->get()).value();
 * CHECK( tau::get(res1).child_is(tau::bf_f) );
 *
 * tref fm2 = tau::get("X | X'", pbf);
 * auto nso_rr2 = get_nso_rr<node_t>(fm2).value();
 * tref res2 = bf_normalizer_without_rec_relation<node_t>(nso_rr2.main->get()).value();
 * CHECK( tau::get(res2).child_is(tau::bf_t) );
 * @endcode
 */
template <NodeType node>
result<tref> bf_normalizer_without_rec_relation(tref bf);

/**
 * @brief Normalize a Boolean function that includes recurrence relations.
 *
 * Unfolds the recurrence with `nso_rr_apply` (which transforms reference
 * arguments to captures, calculates all fixed points via
 * `calculate_all_fixed_points` and applies the rules with `step`), and
 * delegates to `bf_normalizer_without_rec_relation` for the final normalization.
 * @tparam node Tree node type.
 * @param bf Recurrence relation structure containing the Boolean function.
 * @return Normalized Boolean function, or the error of either stage.
 *
 * @par Example
 * @code{.cpp}
 * // h(X):tau := 1., query h(Y): unfolds the recurrence to just "1"
 * // (see "Simple case (y1)" in tests/integration/test_integration-bf_normalization.cpp).
 * auto nso_rr = get_bf_nso_rr("h(X):tau := 1.", "h(Y)").value();
 * tref res = bf_normalizer_with_rec_relation<node_t>(nso_rr).value();
 * CHECK( tau::get(res).child_is(tau::bf_t) );
 * @endcode
 */
template <NodeType node>
result<tref> bf_normalizer_with_rec_relation(const rr<node> &bf);

/**
 * @brief Full normalizer for a recurrence relation.
 *
 * Combines `nso_rr_apply` (to unfold the recurrence) with
 * `normalize_with_temp_simp` (to normalize the resulting formula including
 * temporal simplifications). When `rule_counting` is set, the per-rule hit
 * counts are flushed into the returned report.
 * @tparam node Tree node type.
 * @param nso_rr The complete recurrence relation structure.
 * @return Fully normalized formula, or an `internal_error` report when the
 * recurrence cannot be applied or normalization fails.
 *
 * @par Example
 * @code{.cpp}
 * // See the "Normalizer" test case "1" in
 * // tests/integration/test_integration-wff_normalization.cpp.
 * const char* sample =
 *     "all a,b,c,d a'c|b'd = 0 <-> a & b' & d | a' & c | b' & c' & d = 0.";
 * auto nso_rr = get_nso_rr(sample).value();
 * tref res = normalizer<node_t>(nso_rr).value();
 * CHECK( tau::get(res).child_is(tau::wff_t) );
 * @endcode
 */
template <NodeType node>
result<tref> normalizer(const rr<node>& nso_rr);

/**
 * @brief Full normalizer for a plain formula (no recurrence relation).
 *
 * Convenience overload that wraps `normalize_with_temp_simp`, flushing the
 * per-rule hit counts into the report when `rule_counting` is set.
 * @tparam node Tree node type.
 * @param fm Formula to normalize; must not be null.
 * @return Fully normalized formula; an `invalid_argument` error for a null
 * @p fm, an `internal_error` when normalization fails.
 *
 * @par Example
 * @code{.cpp}
 * tref fm = get_nso_rr("x = 0 || x != 0.").value().main->get();
 * CHECK( tau::get(normalizer<node_t>(fm).value()).equals_T() );
 * @endcode
 */
template <NodeType node>
result<tref> normalizer(tref fm);

/**
 * @brief Normalize temporal quantifiers (`always`/`sometimes`) in a formula.
 *
 * A formula holding a full-LTL or CTL* operator is returned unchanged. One
 * without temporal variables loses its `always`/`sometimes` wrappers; one
 * with temporal variables but no temporal quantifier is wrapped in
 * `always`. Otherwise the temporal layer is converted to DNF and reduced, the
 * `always` parts of each clause are squeezed into one, and the clauses
 * without temporal variables are gathered under one `always`. When
 * `normalize_scopes` is `true`, the formulas below the temporal quantifiers
 * are brought to `term_boole_normal_form`.
 *
 * @tparam node Tree node type.
 * @tparam normalize_scopes When `true` (default) also normalize inner formulas.
 * @param fm Formula to normalize.
 * @return Formula with normalized temporal quantifiers, or the error of the
 * DNF conversion or reduction.
 *
 * @par Example
 * @code{.cpp}
 * // "(always x=0) && (always x=0)": the duplicate always-clauses squeeze
 * // together and the (now redundant) temporal wrapper is dropped entirely.
 * tref fm = get_nso_rr(
 *     "(always x = 0) && (always x = 0).").value().main->get();
 * tref res = normalize_temporal_quantifiers<node_t>(fm).value();
 * // tau::get(res).to_str() == "x = 0"
 * CHECK( !tau::get(res).child_is(tau::wff_always) );
 * @endcode
 */
template <NodeType node, bool normalize_scopes = true>
result<tref> normalize_temporal_quantifiers(tref fm);

} // namespace idni::tau_lang

#include "normalizer.tmpl.h"

#endif // __IDNI__TAU__NORMALIZER_H__
