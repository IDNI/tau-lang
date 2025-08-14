// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__CVC5_H__
#define __IDNI__TAU__CVC5_H__

#include <cvc5/cvc5.h>

#include "tau_tree.h"

namespace idni::tau_lang {

using bv = cvc5::Term;
using sort = cvc5::Sort;
using bvs = std::vector<bv>;
using solver = cvc5::Solver;

static solver bv_solver;
static size_t bv_default_size = 64;

static void bv_config() {
	// configure the solver
	bv_solver.setOption("produce-models", "true");
	//bv_solver.setOption("incremental", "true");
	bv_solver.setLogic("BV");
}

// -----------------------------------------------------------------------------
// Basic helper methods

bool is_synthetic_one(const bv& b);
bool is_synthetic_zero(const bv& b);
size_t get_bv_size(const bv& b);

template<NodeType node>
size_t get_bv_size(const tref b);

// -----------------------------------------------------------------------------
// Basic Boolean algebra operations on bitvectors
bv operator|(const bv& lhs, const bv& rhs);
bv operator&(const bv& lhs, const bv& rhs);
bv operator^(const bv& lhs, const bv& rhs);
bv operator~(const bv& operand);
bv operator+(const bv& lhs, const bv& rhs);
bv operator-(const bv& lhs, const bv& rhs);
bv operator*(const bv& lhs, const bv& rhs);
bv operator/(const bv& lhs, const bv& rhs);
bv operator%(const bv& lhs, const bv& rhs);
bv operator<<(const bv& lhs, const bv& rhs);
bv operator>>(const bv& lhs, const bv& rhs);

// -----------------------------------------------------------------------------
// Builders

inline bv make_term_not(const bv& operand);
inline bv make_term_and(const bv& lhs, const bv& rhs);
inline bv make_term_or(const bv& lhs, const bv& rhs);
inline bv make_term_forall(const bv& var, const bv& form);
inline bv make_term_forall(const bvs& vars, const bv& form);
inline bv make_term_exists(const bv& var, const bv& form);
inline bv make_term_exists(const bvs& vars, const bv& form);
inline bv make_term_equal(const bv& lhs, const bv& rhs);
inline bv make_term_distinct(const bv& lhs, const bv& rhs);
inline bv make_term_less_equal(const bv& lhs, const bv& rhs);
inline bv make_term_greater_equal(const bv& lhs, const bv& rhs);
inline bv make_term_less(const bv& lhs, const bv& rhs);
inline bv make_term_greater(const bv& lhs, const bv& rhs);
inline bv make_bv_var(const sort& s, const std::string& name);
inline bv make_bv_not(const bv& operand);
inline bv make_bv_and(const bv& lhs, const bv& rhs);
inline bv make_bv_nand(const bv& lhs, const bv& rhs);
inline bv make_bv_or(const bv& lhs, const bv& rhs);
inline bv make_bv_nor(const bv& lhs, const bv& rhs);
inline bv make_bv_xor(const bv& lhs, const bv& rhs);
inline bv make_bv_xnor(const bv& lhs, const bv& rhs);
inline bv make_bv_add(const bv& lhs, const bv& rhs);
inline bv make_bv_sub(const bv& lhs, const bv& rhs);
inline bv make_bv_mul(const bv& lhs, const bv& rhs);
inline bv make_bv_div(const bv& lhs, const bv& rhs);
inline bv make_bv_mod(const bv& lhs, const bv& rhs);
inline bv make_bv_shl(const bv& lhs, const bv& rhs);
inline bv make_bv_shr(const bv& lhs, const bv& rhs);

// -----------------------------------------------------------------------------
// Queries

// -----------------------------------------------------------------------------
// Advanced methods

template<NodeType node>
using solution = subtree_map<node, tref>;

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

// -----------------------------------------------------------------------------
// NSO Factory

template<typename...BAs>
requires BAsPack<BAs...>
struct bv_ba_factory {
	std::optional<constant_with_type<BAs...>> parse(const std::string& src);
};

} // namespace idni::tau_lang

#include "bv_ba.tmpl.h"
#include "bv_ba_helpers.tmpl.h"
#include "bv_ba_builders.tmpl.h"
#include "bv_ba_advanced.tmpl.h"

#endif // __IDNI__TAU__CVC5_H__
