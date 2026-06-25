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
 */
template <NodeType node>
tref simp_tau_unsat_valid(tref fm, const int_t start_time = 0,
				const bool output = false);

} // namespace idni::tau_lang

#include "satisfiability.tmpl.h"

#endif // __IDNI__TAU__SATISFIABILITY_H__
