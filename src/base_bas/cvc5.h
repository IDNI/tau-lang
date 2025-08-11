// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__CVC5_H__
#define __IDNI__TAU__CVC5_H__

#include <cvc5/cvc5.h>

#include "tau_tree.h"

namespace idni::tau_lang {

static cvc5::Solver cvc5_solver;
static size_t bv_default_size = 64;

static void bv_config() {
	// configure the solver
	cvc5_solver.setOption("produce-models", "true");
	//cvc5_solver.setOption("incremental", "true");
	cvc5_solver.setLogic("BV");
}

//using tau = tree<node>;

template<NodeType node>
using solution = subtree_map<node, tref>;

template <NodeType node>
std::optional<cvc5::Term> bv_eval_node(const typename tau::traverser& t, subtree_map<tref, cvc5::Term> vars,
	subtree_map<tref, cvc5::Term>& free_vars, bool checked = false);

template <NodeType node>
bool is_bv_formula_sat(tref form);

template <NodeType node>
bool is_bv_formula_valid(tref form);

template <NodeType node>
bool is_bv_formula_unsat(tref form);

template <NodeType node>
std::optional<solution<node>> solve_bv(tref form, cvc5::Solver& solver);

template <NodeType node>
std::optional<solution<node>> solve_bv(tref form);

template <NodeType node>
std::optional<solution<node>> solve_bv(trefs form);

/*template<typename...BAs>
requires BAsPack<BAs...>
struct bv_ba_factory {
	std::optional<constant_with_type<BAs...>> parse(const std::string& src);

private:

	inline static std::map<size_t, tref> cache;
};*/

} // namespace idni::tau_lang

#include "cvc5.tmpl.h"

#endif // __IDNI__TAU__CVC5_H__
