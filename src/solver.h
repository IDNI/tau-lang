// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __SOLVER_H__
#define __SOLVER_H__

#include <stack>

#include "tau_ba.h"
#include "normal_forms.h"
#include "bdd_handle.h"
#include "splitter.h"

// In what follows we use the algorithms and notations of TABA book (cf.
// Section 3.2).Chek (https://github.com/IDNI/tau-lang/blob/main/docs/taba.pdf)
// for the details.

namespace idni::tau_lang {

template<typename...BAs>
using typed_nso = tau<BAs...>;
template<typename...BAs>
using var = tau<BAs...>;
template<typename...BAs>
using minterm = tau<BAs...>;

template<typename...BAs>
using equality = tau<BAs...>;

template<typename...BAs>
using inequality = tau<BAs...>;

template<typename...BAs>
using equation = tau<BAs...>;

template<typename...BAs>
using equations = std::set<tau<BAs...>>;

template<typename...BAs>
using equation_system = std::pair<std::optional<equality<BAs...>>, std::set<inequality<BAs...>>>;

template<typename...BAs>
using inequality_system = std::set<inequality<BAs...>>;

template<typename...BAs>
using minterm_system = std::set<inequality<BAs...>>;

template<typename...BAs>
using solution = std::map<var<BAs...>, tau<BAs...>>;

using type = std::string;

enum solver_mode {
	maximum, minimum, general
};

template<typename...BAs>
struct solver_options {
	tau<BAs...> splitter_one = nullptr;
	solver_mode engine = solver_mode::general;
};

template<typename...BAs>
std::optional<solution<BAs...>> find_solution(const equality<BAs...>& eq,
	const solver_mode engine = solver_mode::general);

template<typename...BAs>
std::optional<solution<BAs...>> lgrs(const equality<BAs...>& equality);

template<typename...BAs>
std::optional<solution<BAs...>> solve_minterm_system(const minterm_system<BAs...>& system,
		const solver_options<BAs...>& options);

template<typename...BAs>
std::optional<solution<BAs...>> solve_inequality_system(const inequality_system<BAs...>& system,
		const solver_options<BAs...>& options);

template<typename...BAs>
std::optional<solution<BAs...>> solve_system(const equation_system<BAs...>& system,
		const solver_options<BAs...>& options);

template<typename...BAs>
std::optional<solution<BAs...>> solve(const equations<BAs...>& eqs,
		const solver_options<BAs...>& options);

template<typename...BAs>
std::optional<solution<BAs...>> solve(const tau<BAs...>& form,
		const solver_options<BAs...>& options);

} // idni::tau_lang namespace

#include "solver.tmpl.h"
#endif // __SOLVER_H__