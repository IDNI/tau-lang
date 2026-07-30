// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file solver_types.h
 * @brief The solver's vocabulary types, without the solver's templates.
 *
 * Split out of solver.h so a BA plugin can name a solution or a solver_options
 * in its descriptor: solver.h ends by including solver.tmpl.h, whose folds call
 * back into the descriptors, so a descriptor including it would close a cycle.
 */

#ifndef __IDNI__TAU__SOLVER_TYPES_H__
#define __IDNI__TAU__SOLVER_TYPES_H__

#include "tau_tree.h"

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
	size_t type_id = 0;
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__SOLVER_TYPES_H__
