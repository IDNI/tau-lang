// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file satisfiability.h
 * @brief Temporal satisfiability checking for Tau formulas.
 *
 * Declares the public interface: `fm_at_time_point`,
 * `get_uninterpreted_constants_constraints`, `transform_to_execution`,
 * `is_tau_formula_sat`, `is_tau_impl`, `are_tau_equivalent`, and
 * `simp_tau_unsat_valid`. Template implementations and internal helpers
 * reside in `satisfiability.tmpl.h`.
 */

#ifndef __IDNI__TAU__SATISFIABILITY_H__
#define __IDNI__TAU__SATISFIABILITY_H__

#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @brief Instantiate @p original_fm for IO variables at @p time_point.
 * @tparam node Tree node type.
 * @param original_fm Formula template to instantiate.
 * @param io_vars IO variable nodes to substitute.
 * @param time_point Time step to substitute into IO variables.
 * @return Formula with IO variables instantiated at @p time_point.
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
tref fm_at_time_point(tref original_fm, const trefs &io_vars, int_t time_point);

/**
 * @brief Compute constraints for uninterpreted constants in an unbounded continuation.
 *
 * Assumes @p fm is an unbound continuation formula.
 * @tparam node Tree node type.
 * @param fm Unbounded continuation formula.
 * @param io_vars IO variable nodes (updated with any new variables).
 * @param start_time Time step at which the continuation was started.
 * @return Formula constraining uninterpreted constants, or `T` if none exist.
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
tref get_uninterpreted_constants_constraints(tref fm, trefs& io_vars, int_t start_time);

/**
 * @brief Transform a normalized Tau formula into execution form.
 * @tparam node Tree node type.
 * @param fm Normalized Tau formula.
 * @param start_time Starting time step (default: 0).
 * @param output When `true`, print diagnostic messages (default: `false`).
 * @return Formula ready for step-by-step execution.
 *
 * @par Example
 * @code{.cpp}
 * // Unsatisfiable clause: o1 is always 0, yet must sometimes be both 0 and 1
 * // (see tests/integration/test_integration-satisfiability1.cpp:24-27)
 * tref fm = create_spec(
 *     "(always o1[t] = 0) && (sometimes o1[t] = 0) && (sometimes o1[t] = 1).");
 * CHECK(transform_to_execution<node_t>(fm) == tau::_F());
 *
 * // Satisfiable bitvector conditional: holds for every possible input
 * // (see tests/integration/test_integration-satisfiability3.cpp:38-40)
 * tref fm2 = create_spec(
 *     "(always i1[t]:bv[16] = { 1 } ? o1[t]:bv[16] = { 0 } : o1[t]:bv[16] = { 1 }).");
 * CHECK(transform_to_execution<node_t>(fm2) != tau::_F());
 * @endcode
 */
template <NodeType node>
tref transform_to_execution(tref fm, const int_t start_time = 0,
					const bool output = false);

/**
 * @brief Check whether a Tau formula is satisfiable.
 * @tparam node Tree node type.
 * @param fm Tau formula to test.
 * @param start_time Starting time step (default: 0).
 * @param output When `true`, print diagnostic messages (default: `false`).
 * @return `true` if the formula is satisfiable.
 *
 * @par Example
 * @code{.cpp}
 * // Unsatisfiable: "always" forces o1 to be 0 at every time step, but the
 * // "sometimes" clause demands a time step where o1[t] = 1 while o1[t-1] = 0
 * // (see tests/integration/test_integration-satisfiability1.cpp:13-14)
 * tref fm_unsat = create_spec(
 *     "(always o1[t-1] = 0) && (sometimes o1[t] = 1 && o1[t-1] = 0).");
 * CHECK(!is_tau_formula_sat<node_t>(fm_unsat));
 *
 * // Satisfiable: "always" pins o1 to the constant 1, and "sometimes" only
 * // constrains the unrelated stream o2
 * // (see tests/integration/test_integration-satisfiability1.cpp:17-18)
 * tref fm_sat = create_spec(
 *     "(always o1[t] = o1[t-1] && o1[t-1] = 1) && (sometimes o2[t] = 0).");
 * CHECK(is_tau_formula_sat<node_t>(fm_sat));
 * @endcode
 */
template <NodeType node>
bool is_tau_formula_sat(tref fm, const int_t start_time = 0,
	const bool output = false);

/**
 * @brief Check whether temporal formula @p f1 implies @p f2.
 * @tparam node Tree node type.
 * @param f1 Antecedent formula.
 * @param f2 Consequent formula.
 * @return `true` if every model of @p f1 satisfies @p f2.
 *
 * @par Example
 * @code{.cpp}
 * // o1[t] = 1 trivially implies o1[t] = 1 || o2[t] = 0, regardless of o2:
 * // negating the implication gives (always o1[t] = 1) && (sometimes
 * // o1[t] != 1 && ...), which contradicts the "always" part and is
 * // therefore unsatisfiable, so the implication holds. This mirrors the
 * // "is fm valid/a tautology" idiom used at src/api.tmpl.h:439
 * // (`is_tau_impl<node>(tau::_T(), normalize_formula(fm))`) and
 * // src/boolean_algebras/tau_ba.tmpl.h:105.
 * tref f1 = create_spec("always o1[t] = 1.");
 * tref f2 = create_spec("always (o1[t] = 1 || o2[t] = 0).");
 * bool result = is_tau_impl<node_t>(f1, f2);
 * // CHECK(result == true);
 * @endcode
 */
template <NodeType node>
bool is_tau_impl(tref f1, tref f2);

/**
 * @brief Check whether two closed temporal formulas are logically equivalent.
 *
 * The formulas must be closed (no free variables).
 * @tparam node Tree node type.
 * @param f1 First formula (closed).
 * @param f2 Second formula (closed).
 * @return `true` if @p f1 and @p f2 have identical models.
 *
 * @par Example
 * @code{.cpp}
 * // o1[t] = 1 and its double negation !(o1[t] != 1) describe the same
 * // models: negating the biconditional and checking satisfiability yields
 * // an unsatisfiable formula, so the two are equivalent.
 * tref f1 = create_spec("always o1[t] = 1.");
 * tref f2 = create_spec("always !(o1[t] != 1).");
 * bool result = are_tau_equivalent<node_t>(f1, f2);
 * // CHECK(result == true);
 * @endcode
 */
template <NodeType node>
bool are_tau_equivalent(tref f1, tref f2);

/**
 * @brief Simplify @p fm by removing unsatisfiable or valid temporal sub-formulas.
 * @tparam node Tree node type.
 * @param fm Formula to simplify.
 * @param start_time Starting time step (default: 0).
 * @param output When `true`, print diagnostic messages (default: `false`).
 * @return Simplified formula.
 *
 * @par Example
 * @code{.cpp}
 * // First disjunct is unsatisfiable (o2[t] cannot be both 0 and 1 at the
 * // same time step), second disjunct is satisfiable. Tracing the
 * // implementation: is_tau_impl<node>(T, fm) fails first (fm is not
 * // valid), so fm is normalized into DNF disjuncts and each disjunct whose
 * // transform_to_execution(...) is not F is kept; the unsatisfiable first
 * // disjunct is therefore dropped from the result.
 * tref fm = create_spec(
 *     "(always (o2[t] = 0 && o2[t] = 1)) || (always o1[t] = 1).");
 * tref result = simp_tau_unsat_valid<node_t>(fm);
 * // The unsatisfiable disjunct is dropped; result is satisfiability-
 * // equivalent to create_spec("always o1[t] = 1."). The exact printed
 * // form of `result` is not asserted here since simp_tau_unsat_valid does
 * // not guarantee a particular normal form for the surviving disjuncts.
 * @endcode
 */
template <NodeType node>
tref simp_tau_unsat_valid(tref fm, const int_t start_time = 0,
				const bool output = false);

} // namespace idni::tau_lang

#include "satisfiability.tmpl.h"

#endif // __IDNI__TAU__SATISFIABILITY_H__
