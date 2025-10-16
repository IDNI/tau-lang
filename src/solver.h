// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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

#include "splitter.h"

namespace idni::tau_lang {

/**
 * @typedef typed_nso
 * @brief Alias for tau tree node.
 */
using typed_nso = tref;

/**
 * @typedef var
 * @brief Alias for tau tree node.
 */
using var = tref;

/**
 * @typedef minterm
 * @brief Alias for tau tree node.
 */
using minterm = tref;

/**
 * @typedef equality
 * @brief Alias for tau tree node.
 */
using equality = tref;

/**
 * @typedef inequality
 * @brief Alias for tau tree node.
 */
using inequality = tref;

/**
 * @typedef equation
 * @brief Alias for tau tree node.
 */
using equation = tref;

/**
 * @typedef equations
 * @brief Alias for a set of tau tree nodes.
 */
template <NodeType node>
using equations = subtree_set<node>;

/**
 * @typedef equation_system
 * @brief Alias for a pair consisting of an optional equality and a set of inequalities.
 */
template<NodeType node>
using equation_system = std::pair<std::optional<equality>,
				  std::set<inequality, subtree_less<node>>>;

/**
 * @typedef inequality_system
 * @brief Alias for a set of inequalities.
 */
template<NodeType node>
using inequality_system = subtree_set<node>;

/**
 * @typedef minterm_system
 * @brief Alias for a set of inequalities.
 */
template<NodeType node>
using minterm_system = subtree_set<node>;

/**
 * @typedef solution
 * @brief Alias for a map of variables to tau templates.
 */
template<NodeType node>
using solution = subtree_map<node, tref>;

/**
 * @enum solver_mode
 * @brief Enumeration for solver modes.
 */
enum solver_mode {
	maximum, /**< Maximum mode */
	minimum, /**< Minimum mode */
	general, /**< General mode */
	bitvector /**< cvc5 mode */
};

/**
 * @struct solver_options
 * @brief Structure for solver options.
 */
struct solver_options {
	tref splitter_one = nullptr; /**< Splitter option */
	solver_mode mode = solver_mode::general; /**< Solver mode option */
};

/**
 * @brief Finds a solution for the given equality.
 *
 * @tparam BAs Variadic template parameters.
 * @param eq The equality to solve.
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> find_solution(equality eq);

/**
 * @brief Solves the given equality using LGRS algorithm.
 *
 * @tparam BAs Variadic template parameters.
 * @param equality The equality to solve.
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> lgrs(equality equality);

/**
 * @brief Solves the given minterm system.
 *
 * @tparam BAs Variadic template parameters.
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
 * @tparam BAs Variadic template parameters.
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
 * @tparam BAs Variadic template parameters.
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
 * @tparam BAs Variadic template parameters.
 * @param eqs The set of equations to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> solve(const equations<node>& eqs,
					const solver_options& options);

/**
 * @brief Solves the given tau form.
 *
 * @tparam BAs Variadic template parameters.
 * @param form The tau form to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> solve(tref form, solver_options options);

/**
 * @brief Solves the given tau forms.
 *
 * @tparam BAs Variadic template parameters.
 * @param forms The tau forms to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template <NodeType node>
std::optional<solution<node>> solve(const trefs& forms, solver_options options);

} // namespace idni::tau_lang

#include "solver.tmpl.h"
#endif // __IDNI__TAU__SOLVER_H__