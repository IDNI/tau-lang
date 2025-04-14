// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __Z3_SOLVER_H__
#define __Z3_SOLVER_H__

#include "nso_rr.h"

namespace idni::tau_lang {

template<typename...BAs>
using var = tau<BAs...>;

template<typename...BAs>
using solution = std::map<var<BAs...>, tau<BAs...>>;

struct z3_solver_options {
	enum output_format { hexadecimal, binary, decimal };
	bool control_underflow_overflow;
	output_format format = hexadecimal;
};

template <typename...BAs>
z3::expr eval_z3(const tau<BAs...>& form, std::map<tau<BAs...>, z3::expr>& vars, const z3_solver_options& options);

template<typename...BAs>
std::optional<solution<BAs...>> solve_z3(const tau<BAs...>& form, const z3_solver_options& options = { .control_underflow_overflow = false});

} // namespace idni::tau_lang

 #include "z3_solver.tmpl.h"

 #endif // __Z3_SOLVER_H__