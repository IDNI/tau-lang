// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file solver.h
 * @brief Header file for the solver component of the tau-lang project.
 *
 * This file contains the declarations for various solver-related templates
 * and functions. The algorithms and notations used are based on the TABA book
 * (cf. Section 3.2). For more details, refer to the documentation
 * at https://github.com/IDNI/tau-lang/blob/main/docs/taba.pdf.
 */

#ifndef __IDNI__TAU__SOLVER_H__
#define __IDNI__TAU__SOLVER_H__

#include "tau_tree.h"
#include "solver_types.h"

namespace idni::tau_lang {

/**
 * @brief Finds a solution for the given equality.
 *
 * @tparam node Tree node type.
 * @param eq The equality to solve.
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> find_solution(equality eq);

/**
 * @brief Solves the given equality using LGRS algorithm.
 *
 * @tparam node Tree node type.
 * @param equality The equality to solve.
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> lgrs(equality equality);

/**
 * @brief Solves the given minterm system.
 *
 * @tparam node Tree node type.
 * @param system The minterm system to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> solve_minterm_system(
	const minterm_system<node>& system, const solver_options& options);

/**
 * @brief Solves the given inequality system.
 *
 * @tparam node Tree node type.
 * @param system The inequality system to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> solve_inequality_system(
	const inequality_system<node>& system, const solver_options& options);

/**
 * @brief Solves the given equation system.
 *
 * @tparam node Tree node type.
 * @param system The equation system to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> solve_system(
	const equation_system<node>& system, const solver_options& options);

/**
 * @brief Solves the given set of equations.
 *
 * @tparam node Tree node type.
 * @param eqs The set of equations to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> solve(const equations<node>& eqs,
					const solver_options& options);

/**
 * @brief Check that assigning @p term to @p var is consistent with existing assignments.
 *
 * Returns `false` (and does not modify @p var_assignments) if the assignment
 * contradicts an existing entry.
 * @tparam node Tree node type.
 * @param var_assignments Current variable-to-term map (modified on success).
 * @param var Variable node to assign.
 * @param term Term to assign to @p var.
 * @return `true` if the assignment is consistent and was recorded.
 */
template <NodeType node>
bool check_var_assignment(auto& var_assignments, tref var, tref term);

/**
 * @brief Normalize @p term and record the assignment @p var → @p term.
 *
 * Normalizes the term before inserting it in @p var_assignments so that
 * later consistency checks compare canonical forms.
 * @tparam node Tree node type.
 * @param var_assignments Map to update.
 * @param var Variable node.
 * @param term Term to normalize and assign.
 */
template <NodeType node>
void normalize_and_add_assignment(subtree_map<node, tref>& var_assignments,
tref var, tref term);

/**
 * @brief Solves the given tau form.
 *
 * @tparam node Tree node type.
 * @param form The tau form to solve.
 * @param options The solver options.
 * @param error Reports if a clause is found that is not supported within solving
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> solve(tref form, solver_options options, bool& error);

// (SO-7: trefs overload deleted -- zero callers.)

} // namespace idni::tau_lang

#include "solver.tmpl.h"
#endif // __IDNI__TAU__SOLVER_H__