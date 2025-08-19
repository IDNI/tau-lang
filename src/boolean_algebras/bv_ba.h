// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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

// -----------------------------------------------------------------------------
// Basic helper methods

template<NodeType node>
size_t get_bv_size(const tref b);

void config_cvc5_solver(cvc5::Solver& solver) {
	// configure the solver
	solver.setOption("produce-models", "true");
	//solver.setOption("incremental", "true");
	solver.setLogic("BV");
}

// -----------------------------------------------------------------------------
// Advanced methods

template <NodeType node>
bv bv_eval_node(const typename tree<node>::traverser& form,
	subtree_map<node, bv> vars, subtree_map<node, bv>& free_vars,
	bool checked);

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

bv splitter(const cvc5::Term& fm, idni::tau_lang::splitter_type&) { return fm; }

// -----------------------------------------------------------------------------
// NSO Factory

template<typename...BAs>
requires BAsPack<BAs...>
struct bv_ba_factory {
	std::optional<constant_with_type<BAs...>> parse(const std::string& src);
};

} // namespace idni::tau_lang

#include "bv_ba.tmpl.h"

#endif // __IDNI__TAU__CVC5_H__
