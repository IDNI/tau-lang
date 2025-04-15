// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __Z3_H__
#define __Z3_H__

#include "nso_rr.h"

namespace idni::tau_lang {

static z3::context z3_context;

void z3_config() {
	z3::set_param("pp.bv-literals", false);
	z3::set_param("smt.mbqi", true);
}

template<typename...BAs>
using var = tau<BAs...>;

template<typename...BAs>
using solution = std::map<var<BAs...>, tau<BAs...>>;

//struct z3_solver_options {
//	enum output_format { hexadecimal, binary, decimal };
//	bool control_underflow_overflow;
//	output_format format = hexadecimal;
//};

template <typename...BAs>
z3::expr eval_z3(const tau<BAs...>& form, std::map<tau<BAs...>, z3::expr>& vars, bool checked = false);

template<typename...BAs>
std::optional<solution<BAs...>> solve_z3(const tau<BAs...>& form);

} // namespace idni::tau_lang

 #include "z3.tmpl.h"

 #endif // __Z3_H__