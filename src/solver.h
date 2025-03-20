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

#ifndef __SOLVER_H__
#define __SOLVER_H__

#include <stack>
#include <set>
#include <map>
#include <optional>
#include <string>

//#include "boolean_algebras/tau_ba.h"
//#include "normal_forms.h"
//#include "boolean_algebras/bdds/bdd_handle.h"
//#include "splitter.h"
#include "nso_rr.h"

namespace idni::tau_lang {

/**
 * @typedef typed_nso
 * @brief Alias for tau template with variadic template parameters.
 */
template<typename...BAs>
using typed_nso = tau<BAs...>;

/**
 * @typedef var
 * @brief Alias for tau template with variadic template parameters.
 */
template<typename...BAs>
using var = tau<BAs...>;

/**
 * @typedef minterm
 * @brief Alias for tau template with variadic template parameters.
 */
template<typename...BAs>
using minterm = tau<BAs...>;

/**
 * @typedef equality
 * @brief Alias for tau template with variadic template parameters.
 */
template<typename...BAs>
using equality = tau<BAs...>;

/**
 * @typedef inequality
 * @brief Alias for tau template with variadic template parameters.
 */
template<typename...BAs>
using inequality = tau<BAs...>;

/**
 * @typedef equation
 * @brief Alias for tau template with variadic template parameters.
 */
template<typename...BAs>
using equation = tau<BAs...>;

/**
 * @typedef equations
 * @brief Alias for a set of tau templates with variadic template parameters.
 */
template<typename...BAs>
using equations = std::set<tau<BAs...>>;

/**
 * @typedef equation_system
 * @brief Alias for a pair consisting of an optional equality and a set of inequalities.
 */
template<typename...BAs>
using equation_system = std::pair<std::optional<equality<BAs...>>, std::set<inequality<BAs...>>>;

/**
 * @typedef inequality_system
 * @brief Alias for a set of inequalities.
 */
template<typename...BAs>
using inequality_system = std::set<inequality<BAs...>>;

/**
 * @typedef minterm_system
 * @brief Alias for a set of inequalities.
 */
template<typename...BAs>
using minterm_system = std::set<inequality<BAs...>>;

/**
 * @typedef solution
 * @brief Alias for a map of variables to tau templates.
 */
template<typename...BAs>
using solution = std::map<var<BAs...>, tau<BAs...>>;

/**
 * @enum solver_mode
 * @brief Enumeration for solver modes.
 */
enum solver_mode {
	maximum, /**< Maximum mode */
	minimum, /**< Minimum mode */
	general, /**< General mode */
	z3 /**< Z3 mode */
};

/**
 * @struct solver_options
 * @brief Structure for solver options.
 */
template<typename...BAs>
struct solver_options {
	tau<BAs...> splitter_one = nullptr; /**< Splitter option */
	solver_mode mode = solver_mode::general; /**< Solver mode option */
	std::string type = ""; /**< Type option */
};

/**
 * @brief Finds a solution for the given equality.
 *
 * @tparam BAs Variadic template parameters.
 * @param eq The equality to solve.
 * @return An optional solution.
 */
template<typename...BAs>
std::optional<solution<BAs...>> find_solution(const equality<BAs...>& eq);

/**
 * @brief Solves the given equality using LGRS algorithm.
 *
 * @tparam BAs Variadic template parameters.
 * @param equality The equality to solve.
 * @return An optional solution.
 */
template<typename...BAs>
std::optional<solution<BAs...>> lgrs(const equality<BAs...>& equality);

/**
 * @brief Solves the given minterm system.
 *
 * @tparam BAs Variadic template parameters.
 * @param system The minterm system to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template<typename...BAs>
std::optional<solution<BAs...>> solve_minterm_system(const minterm_system<BAs...>& system,
	const solver_options<BAs...>& options);

/**
 * @brief Solves the given inequality system.
 *
 * @tparam BAs Variadic template parameters.
 * @param system The inequality system to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template<typename...BAs>
std::optional<solution<BAs...>> solve_inequality_system(const inequality_system<BAs...>& system,
	const solver_options<BAs...>& options);

/**
 * @brief Solves the given equation system.
 *
 * @tparam BAs Variadic template parameters.
 * @param system The equation system to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template<typename...BAs>
std::optional<solution<BAs...>> solve_system(const equation_system<BAs...>& system,
	const solver_options<BAs...>& options);

/**
 * @brief Solves the given set of equations.
 *
 * @tparam BAs Variadic template parameters.
 * @param eqs The set of equations to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template<typename...BAs>
std::optional<solution<BAs...>> solve(const equations<BAs...>& eqs,
	const solver_options<BAs...>& options);

/**
 * @brief Solves the given tau form.
 *
 * @tparam BAs Variadic template parameters.
 * @param form The tau form to solve.
 * @param options The solver options.
 * @return An optional solution.
 */
template<typename...BAs>
std::optional<solution<BAs...>> solve(const tau<BAs...>& form,
	const solver_options<BAs...>& options);

} // idni::tau_lang namespace

#include "solver.tmpl.h"
#endif // __SOLVER_H__