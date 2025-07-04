// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __CVC5_H__
#define __CVC5_H__

#include "boolean_algebras/tau_ba.h"
//#include "nso_rr.h"

namespace idni::tau_lang {

static cvc5::Solver cvc5_solver;
static size_t bv_default_size = 64;

static void bv_config() {
	// configure the solver
	cvc5_solver.setOption("produce-models", "true");
	//cvc5_solver.setOption("incremental", "true");
	cvc5_solver.setLogic("BV");
}

template<typename...BAs>
using var = tau<BAs...>;

template<typename...BAs>
using solution = std::map<var<BAs...>, tau<BAs...>>;

template <typename...BAs>
cvc5::Term eval_bv(const tau<BAs...>& form, std::map<tau<BAs...>, cvc5::Term> vars,
	std::map<tau<BAs...>, cvc5::Term>& free_vars, bool checked = false);

template <typename...BAs>
bool is_bv_formula_sat(const tau<BAs...>& form);

template <typename...BAs>
bool is_bv_formula_valid(const tau<BAs...>& form);

template <typename...BAs>
bool is_bv_formula_unsat(const tau<BAs...>& form);

template<typename...BAs>
std::optional<solution<BAs...>> solve_bv(const tau<BAs...>& form, cvc5::Solver& solver);

template<typename...BAs>
std::optional<solution<BAs...>> solve_bv(const tau<BAs...>& form);

template<typename...BAs>
std::optional<solution<BAs...>> solve_bv(const std::vector<tau<BAs...>>& form);

template<typename...BAs>
struct bv_ba_factory {
	std::optional<tau<BAs...>> parse(const std::string& src);

private:

	inline static std::map<std::string, tau<BAs...>> cache;
};

} // namespace idni::tau_lang

#include "cvc5.tmpl.h"

#endif // __CVC5_H__
