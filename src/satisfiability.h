// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file satisfiability.h
 * @brief Temporal satisfiability checking for Tau formulas.
 *
 * Declares the public interface: `fm_at_time_point`,
 * `get_uninterpreted_constants_constraints`, `transform_to_execution`,
 * `is_tau_formula_sat`, `pin_written_warm_ups`, `is_tau_impl`,
 * `are_tau_equivalent`, and `simp_tau_unsat_valid`. Template implementations and internal helpers
 * reside in `satisfiability.tmpl.h`.
 */

#ifndef __IDNI__TAU__SATISFIABILITY_H__
#define __IDNI__TAU__SATISFIABILITY_H__

#include "tau_tree.h"
#include "tau_diagnostics.h"
#include "bounded_call.h"

namespace idni::tau_lang {

/**
 * @brief How `transform_to_execution` reads the input streams of a
 * `sometimes` clause.
 */
enum class sometimes_inputs : bool {
	/// Universally at their time step, like the inputs of the `always`
	/// part: the system must make the clause true at some step whatever
	/// the inputs do (README "Satisfiability"). Satisfiability and
	/// execution use this reading.
	universal,
	/// Through a guard of uninterpreted constants: the clause only has to
	/// hold when the inputs equal constants that the check chooses. The
	/// implication and equivalence checks decide `unsat(f1 && !f2)`, whose
	/// negated side turns an `always` over inputs into a `sometimes`. Read
	/// universally, `sometimes i1[t] != 0` is unsatisfiable, and
	/// `always i1[t] = 0` would be implied by `T`.
	guarded
};

/**
 * @brief Instantiate @p original_fm for IO variables at @p time_point.
 *
 * Every variable of @p io_vars is replaced at once by its instantiation;
 * a variable already at a constant time point is kept.
 * @tparam node Tree node type.
 * @param original_fm Formula template to instantiate.
 * @param io_vars IO variable nodes to substitute.
 * @param time_point Time step to substitute into IO variables.
 * @return A result carrying the formula with IO variables instantiated at
 * @p time_point. A failed result means an IO variable reached no
 * input/output classification.
 *
 * @par Example
 * This function works on internal formula templates with generic IO
 * variables (as produced during fixpoint computation), not on a spec string
 * you would parse with `create_spec`, so no compilable snippet is given
 * here. Conceptually: given a formula template containing `o1[t]` and
 * `o1[t-1]`, calling `fm_at_time_point<node>(fm, io_vars, 5)` substitutes
 * `o1[t]` with the constant-time variable `o1[5]` and `o1[t-1]` with
 * `o1[4]` (i.e. `time_point` minus the variable's relative shift), while an
 * IO variable already at a constant time point (e.g. `o1[0]`) is left
 * unchanged.
 */
template <NodeType node>
result<tref> fm_at_time_point(tref original_fm, const trefs &io_vars, int_t time_point);

/**
 * @brief Compute constraints for uninterpreted constants in an unbounded continuation.
 *
 * Assumes @p fm is an unbound continuation formula. Instantiates @p fm at
 * its lookback plus @p start_time, quantifies its IO variables in
 * time-compatible order (inputs universally, outputs existentially),
 * existentially quantifies every other free variable except the
 * uninterpreted constants, and normalizes the result.
 * @tparam node Tree node type.
 * @param fm Unbounded continuation formula.
 * @param io_vars [in,out] IO variable nodes of @p fm on entry; consumed
 * (left empty) on exit.
 * @param start_time Time step at which the continuation was started.
 * @return A result carrying the formula constraining uninterpreted
 * constants, or `T` if none exist. A failed result means the continuation
 * could not be instantiated or normalized (e.g. a `bv_widening`
 * width-cap violation).
 *
 * @par Example
 * This function only operates on an already-transformed unbounded
 * continuation formula that contains uninterpreted "guard" constants (as
 * introduced internally by `create_guard`/`transform_to_eventual_variables`
 * to mimic an existentially quantified input), so there is no direct spec
 * string to round-trip through `create_spec` here. Conceptually: if the
 * continuation says an output stream must equal a guard constant that
 * stands for "whatever value input stream `i1` would have when the guard
 * fires", this function quantifies away all the ordinary IO variables of
 * the continuation and returns the residual formula relating only the
 * uninterpreted constants (e.g. forcing that guard constant to equal a
 * specific value if that is the only value consistent with the
 * continuation), defaulting any uninterpreted constant that drops out
 * entirely (no remaining constraint) to `0`, or simply `T` if no
 * uninterpreted constants are involved at all.
 */
template <NodeType node>
result<tref> get_uninterpreted_constants_constraints(tref fm, trefs& io_vars, int_t start_time);

/**
 * @brief Transform a normalized Tau formula into execution form.
 *
 * Replaces the always part by its unbounded continuation and folds the
 * `sometimes` clauses into eventual-variable flags whose raising is then
 * searched. With two or more `sometimes` clauses each is first decided
 * alone with the always part, and one refuted alone refutes @p fm. With
 * `TAU_CACHE` the result is memoized per (formula, start time) and per
 * @p inputs, and the memo is dropped when `verdict_budget_fingerprint`
 * changes. Bounded by `max_fixpoint_steps` and `max_flag_search_steps`.
 * @tparam node Tree node type.
 * @param fm Normalized Tau formula; a single DNF clause.
 * @param start_time Starting time step (default: 0).
 * @param output When `true`, print diagnostic messages (default: `false`).
 * @param inputs How the input streams of a `sometimes` clause are read
 * (default: universally, as satisfiability defines them).
 * @return Formula ready for step-by-step execution (`F` when @p fm has no
 * satisfiable continuation); an error when @p fm is null, still holds a
 * function or predicate reference, nests a temporal operator inside an
 * `always` or `sometimes` clause, or when a step cap, the time budget or a
 * normalization stops the search before a verdict.
 *
 * @par Example
 * @code{.cpp}
 * // Unsatisfiable clause: o1 is always 0, yet must sometimes be both 0 and 1
 * // (see the test case "equal_lookback_two_st" in
 * // tests/integration/satisfiability/test_integration-satisfiability-bool.cpp)
 * tref fm = create_spec(
 *     "(always o1[t] = 0) && (sometimes o1[t] = 0) && (sometimes o1[t] = 1).");
 * CHECK(transform_to_execution<node_t>(fm).value() == tau::_F());
 *
 * // Satisfiable bitvector conditional: holds for every possible input
 * // (see the REPL test realizable_cmd-bv-simple_conditional_case in
 * // tests/repl/commands/test_repl-realizable_cmd.cmake)
 * tref fm2 = create_spec(
 *     "(always i1[t]:bv[16] = { 1 } ? o1[t]:bv[16] = { 0 } : o1[t]:bv[16] = { 1 }).");
 * CHECK(transform_to_execution<node_t>(fm2).value() != tau::_F());
 * @endcode
 */
template <NodeType node>
result<tref> transform_to_execution(tref fm, const int_t start_time = 0,
	const bool output = false,
	const sometimes_inputs inputs = sometimes_inputs::universal);


/**
 * @brief Check whether a Tau formula is satisfiable.
 *
 * CTL* formulas are reduced to LTL first; full-LTL formulas (and Boolean
 * combinations of models the safety pipeline cannot read) go to the
 * LTL(ABA) realizability check; the rest is normalized and each DNF path is
 * decided by `transform_to_execution`. With `TAU_CACHE` the verdict is
 * memoized per (formula, start time) unless @p output is set, and the memo
 * is dropped when `verdict_budget_fingerprint` changes.
 * @tparam node Tree node type.
 * @param fm Tau formula to test; must not be null.
 * @param start_time Starting time step (default: 0).
 * @param output When `true`, print diagnostic messages (default: `false`).
 * @return `true` if the formula is satisfiable: it can be executed
 * indefinitely whatever the inputs (README "Satisfiability"), which for
 * full LTL (U/R/W/S/T, nested temporal operators) is realizability; an
 * error result when that could not be decided (backend failure, a cap that
 * gave up, a CTL* E over inputs whose encoding is unrealizable).
 *
 * @par Example
 * @code{.cpp}
 * // Unsatisfiable: "always" forces o1 to be 0 at every time step, but the
 * // "sometimes" clause demands a time step where o1[t] = 1 while o1[t-1] = 0
 * // (see the test case "equal_lookback_one_st" in
 * // tests/integration/satisfiability/test_integration-satisfiability-bool.cpp)
 * tref fm_unsat = create_spec(
 *     "(always o1[t-1] = 0) && (sometimes o1[t] = 1 && o1[t-1] = 0).");
 * CHECK(!is_tau_formula_sat<node_t>(fm_unsat).value());
 *
 * // Satisfiable: "always" pins o1 to the constant 1, and "sometimes" only
 * // constrains the unrelated stream o2
 * // (see the test case "smaller_lookback_one_st" in
 * // tests/integration/satisfiability/test_integration-satisfiability-bool.cpp)
 * tref fm_sat = create_spec(
 *     "(always o1[t] = o1[t-1] && o1[t-1] = 1) && (sometimes o2[t] = 0).");
 * CHECK(is_tau_formula_sat<node_t>(fm_sat).value());
 * @endcode
 */
template <NodeType node>
result<bool> is_tau_formula_sat(tref fm, const int_t start_time = 0,
	const bool output = false);


/**
 * @brief Keep the warm-up of each clause of @p fm at its lookback as written.
 *
 * A clause asks nothing before the deepest lookback it reads (README
 * "Lookback initialization"). The clauses of a written conjunction are its
 * always part (every always statement whose body has no temporal operator,
 * merged), each other conjunct headed by `always`, `sometimes`, `U`, `R` or
 * `W`, or the whole formula when it has no temporal operator. Normalization
 * can drop the literal that carries that lookback, a tautology such as
 * `o1[t-2] = o1[t-2]` or one absorbed by another always statement; whether
 * it does is read off the body rebuilt through the construction hooks,
 * the tree the decision procedures normalize. Such a clause gets the
 * literal `o__warmup[t-k] = 0` on a fresh output of the
 * Boolean carrier type, `k` the written lookback; execution never prints
 * this stream. A clause read positively gets it conjoined to its body. A
 * clause read under a negation gets its negation disjoined, so that the
 * marker is conjoined once the negation is pushed through the temporal
 * operator; there each always statement of the always part keeps the always
 * part's lookback. Clauses under `<->`, `^` or `?:`, full-LTL clauses under
 * a negation and clauses that call a definition are left as they are.
 *
 * Run it on the formula that is decided, as written: after any negation the
 * decision applies and before the construction hooks fold it (`tau::reget`,
 * `api::simplify`). A formula pinned for one polarity must not be negated
 * afterwards.
 * @tparam node Tree node type.
 * @param fm Formula, typed but not yet rebuilt through the hooks.
 * @return @p fm with the clauses pinned, built without the hooks, or the
 * error of a normalization that failed.
 *
 * @par Example
 * @code{.cpp}
 * // (always o2[t] = 1 && o1[t-2] = o1[t-2]) && (sometimes o2[t-1] = 0)
 * // becomes (always o2[t] = 1 && o1[t-2] = o1[t-2] && o__warmup[t-2] = 0)
 * //         && (sometimes o2[t-1] = 0)
 * // and under a negation the always body becomes
 * // (o2[t] = 1 && o1[t-2] = o1[t-2]) || o__warmup[t-2] != 0
 * @endcode
 */
template <NodeType node>
result<tref> pin_written_warm_ups(tref fm);

/**
 * @brief Check whether temporal formula @p f1 implies @p f2.
 *
 * The inputs of the negated implication are quantified universally, so
 * `false` means the system can keep @p f1 true and @p f2 false whatever the
 * inputs do; the inputs of its `sometimes` clauses are read through a guard
 * (`sometimes_inputs::guarded`). A trace validity check reads every input as an output first
 * (`inputs_as_outputs`, as `api::valid_spec` does).
 * @tparam node Tree node type.
 * @param f1 Antecedent formula.
 * @param f2 Consequent formula.
 * @return `true` if every model of @p f1 satisfies @p f2, `false` as soon
 * as one disjunct of the check is satisfiable; an error (UNKNOWN) when no
 * disjunct is satisfiable and one of them is undecided, when normalization
 * fails, or (`unsupported_operation`) when either formula holds a CTL*
 * operator or the check holds full-LTL operators.
 *
 * @par Example
 * @code{.cpp}
 * // o1[t] = 1 trivially implies o1[t] = 1 || o2[t] = 0, regardless of o2:
 * // negating the implication gives (always o1[t] = 1) && (sometimes
 * // o1[t] != 1 && ...), which contradicts the "always" part and is
 * // therefore unsatisfiable, so the implication holds. This mirrors the
 * // "is fm valid/a tautology" idiom of api::valid_spec
 * // (`is_tau_impl<node>(tau::_T(), nfm)`) and src/boolean_algebras/tau/tau_ba.tmpl.h.
 * tref f1 = create_spec("always o1[t] = 1.");
 * tref f2 = create_spec("always (o1[t] = 1 || o2[t] = 0).");
 * bool result = is_tau_impl<node_t>(f1, f2).value();
 * // CHECK(result == true);
 * @endcode
 */
template <NodeType node>
result<bool> is_tau_impl(tref f1, tref f2);

/**
 * @brief Check whether two closed temporal formulas are logically equivalent.
 *
 * The formulas must be closed (no free variables). The inputs of the
 * `sometimes` clauses of the check are read like in `is_tau_impl`.
 * @tparam node Tree node type.
 * @param f1 First formula (closed).
 * @param f2 Second formula (closed).
 * @return `true` if @p f1 and @p f2 have identical models, `false` as soon
 * as one disjunct of the check is satisfiable; an error (UNKNOWN) when no
 * disjunct is satisfiable and one of them is undecided, when normalization
 * fails, or (`unsupported_operation`) when the check holds CTL* or
 * full-LTL operators.
 *
 * @par Example
 * @code{.cpp}
 * // o1[t] = 1 and its double negation !(o1[t] != 1) describe the same
 * // models: negating the biconditional and checking satisfiability yields
 * // an unsatisfiable formula, so the two are equivalent.
 * tref f1 = create_spec("always o1[t] = 1.");
 * tref f2 = create_spec("always !(o1[t] != 1).");
 * bool result = are_tau_equivalent<node_t>(f1, f2).value();
 * // CHECK(result == true);
 * @endcode
 */
template <NodeType node>
result<bool> are_tau_equivalent(tref f1, tref f2);

// Support-component factoring (defined in boolean_algebras/tau/tau_ba.tmpl.h,
// same translation unit): factored_tau_sat and factored_tau_valid let
// simp_tau_unsat_valid below decide its validity and per-path
// satisfiability tests unit-wise where that is exact.
/// @brief Whether component factoring is on (the `ba_component_factoring`
/// flag, or the TAU_BA_COMPONENT_FACTORING environment variable).
inline bool ba_component_factoring_enabled();
/// @brief Unit-wise satisfiability of @p fm: 1 sat, 0 unsat, -1 not
/// applicable (the caller falls back to the monolithic check); no value when
/// the decision of a group failed.
template <typename node> static result<int> factored_tau_sat(tref fm);
/// @brief Unit-wise validity of @p fm: 1 valid, 0 not valid, -1 not
/// applicable (the caller falls back to the monolithic check); no value when
/// the decision of a unit failed.
template <typename node> static result<int> factored_tau_valid(tref fm);

/**
 * @brief Simplify @p fm by removing unsatisfiable or valid temporal sub-formulas.
 *
 * A valid @p fm becomes `T`. Otherwise @p fm is normalized and every DNF
 * path that is not unsatisfiable is kept; the kept paths are deduplicated
 * and absorbed by `simplify_dnf_clauses`. With component factoring on
 * (`pack_ba_component_factoring_enabled`) and @p start_time `0`, validity
 * and each path's satisfiability are decided unit-wise where that is exact.
 * An undecided validity only skips that simplification.
 * @tparam node Tree node type.
 * @param fm Formula to simplify; must not be null.
 * @param start_time Starting time step (default: 0).
 * @param output When `true`, print diagnostic messages (default: `false`).
 * @return Simplified formula (`F` when no path is satisfiable); an error
 * when normalization fails or a path's satisfiability cannot be decided.
 *
 * @par Example
 * @code{.cpp}
 * // First disjunct is unsatisfiable (o2[t] cannot be both 0 and 1 at the
 * // same time step), second disjunct is satisfiable. fm is not valid, so it
 * // is normalized into DNF paths and each path that is not unsatisfiable is
 * // kept; the unsatisfiable first disjunct is therefore dropped from the
 * // result.
 * tref fm = create_spec(
 *     "(always (o2[t] = 0 && o2[t] = 1)) || (always o1[t] = 1).");
 * tref result = simp_tau_unsat_valid<node_t>(fm).value();
 * // The unsatisfiable disjunct is dropped; result is satisfiability-
 * // equivalent to create_spec("always o1[t] = 1."). The exact printed
 * // form of `result` is not asserted here since simp_tau_unsat_valid does
 * // not guarantee a particular normal form for the surviving disjuncts.
 * @endcode
 */
template <NodeType node>
result<tref> simp_tau_unsat_valid(tref fm, const int_t start_time = 0,
				const bool output = false);

} // namespace idni::tau_lang

#include "satisfiability.tmpl.h"

#endif // __IDNI__TAU__SATISFIABILITY_H__
