// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

/**
 * @file bv_ba.h
 * @brief Bit-vector Boolean Algebra interface for tau-lang using cvc5.
 *
 * This header provides type aliases, helper functions, and advanced methods for
 * manipulating and solving bit-vector formulas using the cvc5 SMT solver within
 * the tau-lang framework.
 *
 * Type Aliases:
 * - bv: Alias for cvc5::Term representing a bit-vector term.
 * - sort: Alias for cvc5::Sort representing a sort/type.
 * - bvs: Alias for std::vector<bv>, a vector of bit-vector terms.
 * - solver: Alias for cvc5::Solver, the SMT solver instance.
 * - term_manager: Alias for cvc5::TermManager, manages terms in cvc5.
 * - solution<node>: Alias for subtree_map<node, tref>, representing a solution mapping.
 *
 * Helper Methods:
 * - get_bv_size: Returns the bit-width of a bit-vector term.
 * - config_cvc5_solver: Configures a cvc5::Solver for bit-vector logic and model production.
 *
 * Advanced Methods:
 * - bv_eval_node: Evaluates a (bv) tau tree to a bit-vector formula.
 * - is_bv_formula_sat: Checks satisfiability of a bit-vector formula.
 * - is_bv_formula_valid: Checks validity of a bit-vector formula.
 * - is_bv_formula_unsat: Checks unsatisfiability of a bit-vector formula.
 * - solve_bv: Attempts to solve a bit-vector formula, returning an optional solution.
 * - splitter: Dummy method included for completeness (variant ba). In the case
 * of atomless BAs, it computes the splitter of a given BA element.
 * - parse_bv: Parses a string into a bit-vector constant with type information.
 *
 * @note Implementation details are provided in "bv_ba.tmpl.h".
 */

#ifndef __IDNI__TAU__CVC5_H__
#define __IDNI__TAU__CVC5_H__

#include <cvc5/cvc5.h>

#include "boolean_algebras/cvc5/cvc5.h"
#include "tau_tree.h"
#include "splitter_types.h"

namespace idni::tau_lang {

using bv = cvc5::Term;
using sort = cvc5::Sort;
using bvs = std::vector<bv>;
using solver = cvc5::Solver;
using term_manager = cvc5::TermManager;

template<NodeType node>
using solution = subtree_map<node, tref>;

/**
 * @brief Returns the bit-vector size associated with the given tref.
 *
 * This function template retrieves the size (in bits) of the bit-vector
 * represented by the provided tref object, depending on the specified NodeType.
 *
 * @tparam node The type of node for which the bit-vector size is queried.
 * @param b The tref object representing the bit-vector.
 * @return The size of the bit-vector in bits.
 */
template<NodeType node>
size_t get_bv_size(const tref b);

/**
 * @brief Configures the given cvc5 solver instance for bit-vector logic.
 *
 * Sets the solver options to produce models and configures the logic to "BV" (Bit-Vector).
 * This function prepares the solver for solving bit-vector problems.
 *
 * @param solver Reference to a cvc5::Solver instance to be configured.
 */
void config_cvc5_solver(cvc5::Solver& solver) {
	// configure the solver
	solver.setOption("produce-models", "true");
	//solver.setOption("incremental", "true");
	solver.setLogic("BV");
}

/**
 * @brief Evaluates a (bv) tau tree to a bitvector formula.
 *
 * This function traverses the given tau tree node and computes its
 * bitvector value based on the provided variable mappings. It supports evaluation
 * with both bound and free variables, and can optionally perform additional checks.
 *
 * @param form Traverser for the boolean algebra tree node to be evaluated.
 * @param vars Mapping from tree nodes to bitvector values for bound variables.
 * @param free_vars Mapping from tree nodes to bitvector values for free variables (may be updated).
 * @param checked If true, performs additional consistency checks during evaluation.
 * @return An optional with the the evaluated bitvector value of the node if possible and
 * an empty optional if not.
 *
 */
template <NodeType node>
std::optional<bv> bv_eval_node(const typename tree<node>::traverser& form,
	subtree_map<node, bv> vars, subtree_map<node, bv>& free_vars,
	bool checked);

/**
 * @brief Checks if a given bit-vector formula is satisfiable.
 *
 * This function determines whether the provided bit-vector formula,
 * represented by the parameter `form`, has at least one assignment
 * that makes the formula true (i.e., is satisfiable).
 *
 * @param form The bit-vector formula to be checked for satisfiability.
 * @return true if the formula is satisfiable, false otherwise.
 */
template <NodeType node>
bool is_bv_formula_sat(tref form);

/**
 * @brief Checks whether a given bit-vector formula is valid.
 *
 * This function analyzes the provided formula and determines if it is valid
 * according to the semantics of bit-vector boolean algebra.
 *
 * @param form The formula to be checked for validity.
 * @return true if the formula is valid, false otherwise.
 */
template <NodeType node>
bool is_bv_formula_valid(tref form);

/**
 * @brief Checks whether a given bit-vector formula is unsatisfiable.
 *
 * This function analyzes the provided formula and determines if there is no possible assignment
 * to its variables that would make the formula evaluate to true.
 *
 * @param form The bit-vector formula to be checked for unsatisfiability.
 * @return true if the formula is unsatisfiable; false otherwise.
 */
template <NodeType node>
bool is_bv_formula_unsat(tref form);

/**
 * @brief Solves a Boolean algebra problem over bit-vectors using the provided CVC5 solver.
 *
 * This function attempts to find a solution for the given Boolean formula represented by `form`.
 * It utilizes the specified CVC5 solver instance to perform the computation.
 *
 * @param form The Boolean formula to be solved, represented as a `tref`.
 * @param solver Reference to a CVC5 solver instance used for solving the formula.
 * @return An optional solution of type `solution<node>`. If a solution exists, it is returned;
 *         otherwise, `std::nullopt` is returned.
 */
template <NodeType node>
std::optional<solution<node>> solve_bv(tref form, cvc5::Solver& solver);

/**
 * @brief Solves a boolean algebra problem over bit-vectors.
 *
 * Given a term reference representing a formula, attempts to find a solution
 * that satisfies the formula within the context of bit-vector boolean algebras.
 *
 * @param form The term reference representing the formula to solve.
 * @return An optional solution containing a mapping from nodes to values if a solution exists,
 *         or std::nullopt if no solution is found.
 */
template <NodeType node>
std::optional<solution<node>> solve_bv(tref form);

/**
 * @brief Solves a Boolean algebra problem over bit-vectors.
 *
 * This function attempts to find a solution to the given Boolean formula
 * represented by the parameter `form`, which is expressed in terms of bit-vectors.
 *
 * @param form The Boolean formula to solve, represented as a collection of term references.
 * @return An optional solution containing a mapping of nodes if a solution exists;
 *         std::nullopt otherwise.
 */
template <NodeType node>
std::optional<solution<node>> solve_bv(const trefs& form);

/**
 * @brief Dummy method included for completeness (variant ba). In the case
 * of atomless BAs, it computes the splitter of a given BA element.
 *
 * @param fm The cvc5::Term to be split.
 * @param splitter Reference to the splitter_type used for splitting.
 * @return bv The result of the split operation.
 */
bv splitter(const cvc5::Term& fm, idni::tau_lang::splitter_type&) { return fm; }

/**
 * @brief Parses a bit-vector constant from a string representation.
 *
 * This function attempts to parse the given string `src` as a bit-vector constant,
 * interpreting it according to the specified `size` (number of bits) and `base` (numerical base).
 * If parsing is successful, it returns an optional containing the parsed constant with its type;
 * otherwise, it returns an empty optional.
 *
 * @tparam BAs... Variadic template parameters representing Boolean Algebra types.
 * @param src The string representation of the bit-vector constant to parse.
 * @param size The size (in bits) of the bit-vector. Defaults to `cvc5_default_bv_size`.
 * @param base The numerical base to use for parsing (e.g., 2 for binary, 10 for decimal, 16 for hexadecimal). Defaults to 10.
 * @return std::optional<constant_with_type<BAs...>> The parsed bit-vector constant with type, or std::nullopt if parsing fails.
 */
template<typename...BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_bv(const std::string& src,
	size_t size = cvc5_default_bv_size, size_t base = 10);

} // namespace idni::tau_lang

#include "boolean_algebras/bv_ba.tmpl.h"

#endif // __IDNI__TAU__CVC5_H__
