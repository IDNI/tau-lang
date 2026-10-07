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

#include "tau_diagnostics.h"
#include "tau_tree.h"
#include "solver_types.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

/// Above this many distinct variables, a partition of pure equalities over
/// an arithmetic type (bitvectors) is handed to the pack solver instead of
/// being squeezed per width and given a ground solution by `find_solution` /
/// `find_minimal_solution`, whose Boole expansion is exponential in the
/// variables (GitHub #121). `var = constant` conjuncts
/// are read off before the count. Default 8; SIZE_MAX = unlimited (0
/// through the setter); set via `api::set_lgrs_max_vars`, `--lgrs-max-vars` or the REPL
/// option `lgrsmaxvars`.
/// Environment fallback `TAU_LGRS_MAX_VARS` (0 = unlimited there too).
inline env_limit<size_t> lgrs_max_vars{ "TAU_LGRS_MAX_VARS", 8,
	env_zero::unlimited };

/// How many times a solver call gave up on a value because building it
/// passed `max_constant_size`; `solve` reads it to tell that apart from a
/// system without solutions.
inline size_t constant_size_hits = 0;

/**
 * @brief Finds a ground solution for the given equality.
 *
 * Seeds every variable with 1 and eliminates the variables one by one
 * (TABA Theorem 3.1), giving the maximal zero of the equality.
 * @tparam node Tree node type.
 * @param eq The equality `f = 0` to solve.
 * @return A constant for every variable of @p eq; an empty solution for a
 * variable-free equality that holds; nullopt when @p eq has no zero; the
 * report of a failed reduction otherwise.
 */
template <NodeType node>
result<std::optional<solution<node>>> find_solution(equality eq);

/**
 * @brief Löwenheim's general reproductive solution of the given equality.
 *
 * Each variable x_i maps to `z_i f(X) + x_i f'(X)`, where Z is the zero
 * find_solution gives; the values still mention the variables they solve.
 * @tparam node Tree node type.
 * @param equality The equality `f = 0` to solve; `T` gives an empty solution.
 * @return The solution; a `code::unsat` error when the equality has no zero,
 * a `code::invalid_argument` error when @p equality is null.
 */
template <NodeType node>
result<solution<node>> lgrs(equality equality);

/**
 * @brief Solves the given minterm system (TABA Corollary 3.2).
 *
 * Makes the coefficients of minterms with distinct exponents disjoint,
 * squeezes the result into one equality and solves it with find_solution.
 * @tparam node Tree node type.
 * @param system The minterm system `{m_i != 0}` to solve.
 * @param options The solver options; `type_id` names the algebra, and
 * `splitter_one` is needed to split a coefficient equal to 1.
 * @return A solution; nullopt when a needed splitter is missing or
 * degenerate, or the squeezed equality has no zero; an error from a BA
 * splitter.
 */
template <NodeType node>
result<std::optional<solution<node>>> solve_minterm_system(
	const minterm_system<node>& system, const solver_options& options);

/**
 * @brief Solves the given inequality system.
 *
 * Over an atomless type it delegates to solve_inequality_system_atomless;
 * otherwise it tries every choice of one non-zero minterm per inequality
 * with solve_minterm_system, which is exponential in the variables.
 * @tparam node Tree node type.
 * @param system The inequality system to solve.
 * @param options The solver options; `type_id` selects the algebra.
 * @return A solution (empty for an empty system); nullopt when no choice
 * of minterms is solved; an error from a BA splitter.
 */
template <NodeType node>
result<std::optional<solution<node>>> solve_inequality_system(
	const inequality_system<node>& system, const solver_options& options);

/**
 * @brief Solves an inequality system over an atomless BA without enumeration.
 *
 * Implements TABA cor. Multivariate-BFs-over: satisfiability is `m`
 * independent zero checks, and the witness is built by per-variable
 * elimination instead of searching the product of per-inequality choices.
 * Called by `solve_inequality_system` when `pack_type_is_atomless` holds.
 * With `options.ledger` set, a single-variable exclusion system is first
 * answered from the ledger's fresh region. Building a value past
 * `max_constant_size` gives up on it and counts it in `constant_size_hits`.
 *
 * @tparam node Tree node type.
 * @param system The inequality system to solve; an atom may be `l != r`.
 * @param options The solver options.
 * @return A solution (empty for an empty system); nullopt when some
 * inequality is identically zero, when no witness could be built, or when a
 * value was given up for its size; an error from a BA splitter.
 */
template <NodeType node>
result<std::optional<solution<node>>> solve_inequality_system_atomless(
	const inequality_system<node>& system, const solver_options& options);

/**
 * @brief Solves the given equation system.
 *
 * Tries the maximal solution of the equality (unless `options.mode` is
 * minimum), then the minimal one, keeping each unless an inequality
 * reduces to F under it; maximum and minimum mode stop there. General mode then
 * solves the inequalities alone, or composes them with the LGRS of the
 * equality.
 * @tparam node Tree node type.
 * @param system The equation system to solve.
 * @param options The solver options.
 * @return A solution; nullopt when none is found; an error from a BA
 * splitter.
 */
template <NodeType node>
result<std::optional<solution<node>>> solve_system(
	const equation_system<node>& system, const solver_options& options);

/**
 * @brief Solves the given set of equations.
 *
 * For a non-ABA omega-categorical type, whose variables are points, the
 * owning BA's point solver (`omcat_solve_inequality_system`) answers the
 * whole system, and its model is checked against every atom before it is
 * returned. Equalities are squeezed into one. Ordering atoms are left out
 * of the system and checked against its solution afterwards.
 * @tparam node Tree node type.
 * @param eqs The set of equations to solve, all of type `options.type_id`.
 * @param options The solver options.
 * @return A solution; nullopt when none is found, which with ordering atoms
 * means unsolved rather than unsat; a `code::solver_error` error when the
 * owner of a non-ABA omega-categorical type has no point solver or its model
 * fails an atom.
 */
template <NodeType node>
result<std::optional<solution<node>>> solve(const equations<node>& eqs,
					const solver_options& options);

/**
 * @brief Records the assignment @p var := @p term unless it would introduce
 * a loop among the assignments recorded so far.
 *
 * Returns `false`, leaving @p var_assignments unchanged, when @p var is
 * already assigned, when @p term contains @p var, or when some chain of
 * assignments would lead from @p term back to @p var.
 * @tparam node Tree node type.
 * @param var_assignments In-out map sending each assigned variable to the
 * transitively closed set of variables its term reaches
 * (`subtree_map<node, subtree_set<node>>`); updated on success.
 * @param var Variable node (a bf) to assign.
 * @param term Term to assign to @p var.
 * @return `true` if the assignment was recorded.
 */
template <NodeType node>
bool check_var_assignment(auto& var_assignments, tref var, tref term);

/**
 * @brief Record the assignment @p var → @p term, keeping assigned variables
 * out of every assigned term.
 *
 * Substitutes the existing assignments into @p term, then @p term for
 * @p var in the existing terms, and inserts the pair. No normal form is
 * computed. The caller checks the assignment with check_var_assignment
 * first.
 * @tparam node Tree node type.
 * @param var_assignments In-out map from variable to term.
 * @param var Variable node (a bf), not yet in @p var_assignments.
 * @param term Term to assign.
 */
template <NodeType node>
void normalize_and_add_assignment(subtree_map<node, tref>& var_assignments,
tref var, tref term);

// ------------------------------------------------------------
// result-based API
// ------------------------------------------------------------

/**
 * @brief Solves the given tau form.
 *
 * Normalizes @p form and solves its DNF clauses one by one, after reading
 * off the single-variable assignments of each and partitioning its atoms
 * by type. In general mode the first clause with a solution answers; in
 * minimum and maximum mode every clause is solved and the answer is one no
 * other clause's solution lies strictly below (above). A pure-equality
 * bitvector clause gets a ground zero of each equation (the least one in
 * `solver_mode::minimum`), so every value is a constant a caller can commit
 * as a model; `lgrs` returns the reproductive solution instead. Reads
 * `lgrs_max_vars` and `max_constant_size`, and `constant_size_hits` to tell
 * a value given up for its size from unsatisfiability.
 * @tparam node Tree node type.
 * @param form The tau form to solve; `always`/`sometimes` wrappers of an
 * atom are accepted, the full-LTL operators are not.
 * @param options The solver options; `type_id` is set per partition.
 * @return The solution. Errors: `code::unsat` when no clause has a
 * solution; `code::solver_error` when a value was given up for its size or
 * a point solver cannot decide; `code::unsupported_operation` for a
 * full-LTL operator or a clause the solver cannot handle;
 * `code::invalid_argument` for a null @p form; `code::internal_error` when
 * normalization fails.
 */
template <NodeType node>
result<solution<node>> solve(tref form, solver_options options);

/**
 * @brief Conjuncts @p forms and delegates to @ref solve(tref, solver_options).
 * @return As that overload; a `code::invalid_argument` error when @p forms
 * is empty.
 */
template <NodeType node>
result<solution<node>> solve(const trefs& forms, solver_options options);

} // namespace idni::tau_lang

#include "solver.tmpl.h"
#endif // __IDNI__TAU__SOLVER_H__