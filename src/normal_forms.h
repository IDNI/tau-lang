// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__NORMAL_FORMS_H__
#define __IDNI__TAU__NORMAL_FORMS_H__

#include <list>
#include <numeric>

#include "execution.h"

namespace idni::tau_lang {

/**
 * This enum holds the possible memory slots for traversals using the
 * pre_order class
 */
enum MemorySlotPre {
	normalize_ba_m,
	push_negation_in_m,
	to_dnf_m,
	to_cnf_m,
	eliminate_quantifiers_m,
	anti_prenex_step_m,
	synt_path_simp_m
};

/**
 * This enum holds the possible memory slots for traversals using the
 * post_order class
 */
enum MemorySlotPost {
	anti_prenex_m
};

template <NodeType node>
tref unequal_to_not_equal(tref fm);

template <NodeType node>
tref not_equal_to_unequal(tref fm);

template <NodeType node>
tref normalize_atomic_formula_operators(tref fm);

template <NodeType node>
tref unsqueeze_wff(const tref& fm);

template <NodeType node>
tref squeeze_wff(const tref& fm);

template <NodeType node>
tref unsqueeze_wff_pos(tref fm);

template <NodeType node>
tref squeeze_wff_pos(tref fm);

template <NodeType node>
tref unsqueeze_wff_neg(tref fm);

template <NodeType node>
tref squeeze_wff_neg(tref fm);

template <NodeType node, bool is_wff = true>
tref to_dnf(tref);

template <NodeType node, bool is_wff = true>
tref to_cnf(tref);

template <NodeType node, bool is_wff = true>
tref push_negation_in(tref);

template <NodeType node>
tref to_nnf(tref fm);

template <NodeType node>
tref norm_equation(tref eq);

template <NodeType node>
tref norm_trimmed_equation(tref eq);

template <NodeType node>
tref denorm_equation(tref eq);

template <NodeType node>
tref norm_all_equations(tref fm);

template <NodeType node>
tref apply_xor_def(tref fm);

template <NodeType node>
tref apply_all_xor_def(tref fm);

// // -----------------------------------------------------------------------------
// // General operator for tref function application by pipe
// template <NodeType node>
// using tt_fn = typename tree<node>::traverser(*)(
// 					const typename tree<node>::traverser&);

template <NodeType node>
using tau_f = tref(*)(tref);

// template <NodeType node>
// typename tree<node>::traverser operator|(
// 	const typename tree<node>::traverser& fm, const tau_f<node>& func);

// typename tree<node>::traverser operator|(
// 	const typename tree<node>::traverser& fm, const tau_f<node>& func);

// -----------------------------------------------------------------------------

// This function traverses n and normalizes coefficients in a BF
template <NodeType node>
tref normalize_ba(tref fm);

template <NodeType node>
struct bf_reduce_canonical;

// return the inner quantifier or the top wff if the formula is not quantified
template <NodeType node>
std::pair<typename tree<node>::traverser, tref> get_inner_quantified_wff(tref n);

template <NodeType node>
struct onf_wff {
	explicit onf_wff(tref _var);
	tref operator()(tref n) const;
private:
	tref onf_subformula(tref n) const;
	tref var = nullptr;
};

template <NodeType node>
using onf_wff_t = onf_wff<node>;

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t, const onf_wff_t<node>& r);

template <NodeType node>
tref onf(tref n, tref var);

// Reduce currrent dnf due to update by coeff and variable assignment i
bool reduce_paths(std::vector<int_t>& i, std::vector<std::vector<int_t>>& paths,
	int_t p, bool surface = true);

void join_paths(std::vector<std::vector<int_t>>& paths);

// Ordering function for variables from nso formula
template <NodeType node>
auto lex_var_comp = [](tref x, tref y) {
#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, tref>, bool,
				subtree_pair_less<node, tref>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find({x,y}); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	// TODO (QUESTION) strings have unique id, use .data() instead?
	auto xx = tree<node>::get(x).to_str();
	auto yy = tree<node>::get(y).to_str();
#ifdef TAU_CACHE
	return cache.emplace(std::make_pair(x, y), xx < yy).first->second;
#endif // TAU_CACHE
	return xx < yy;
};

// In conversions to bdd, the following atomic formulas and terms are treated
// as variables
// TODO: Extend properly for full grammar
template <NodeType node>
inline auto is_wff_bdd_var = [](tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	DBG(assert(!t.is(tau::bf_neq));)
	return t.child_is(tau::bf_eq)
		|| t.child_is(tau::wff_ref)
		|| t.child_is(tau::wff_ex)
		|| t.child_is(tau::wff_sometimes)
		|| t.child_is(tau::wff_always)
		|| t.child_is(tau::wff_all)
		|| t.child_is(tau::constraint);
};

template <NodeType node>
inline auto is_bf_bdd_var = [](tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	return t.child_is(tau::variable)
		|| t.child_is(tau::capture)
		|| t.child_is(tau::bf_ref)
		|| t.child_is(tau::ba_constant)
		|| t.child_is(tau::bf_fall)
		|| t.child_is(tau::bf_fex);
};
// ------------------------------

// Starting from variable at position p+1 in vars write to i which variables are irrelevant in assignment
template <NodeType node>
void elim_vars_in_assignment(tref fm, const auto& vars, auto& i,
	const int_t p, const auto& is_var);

template <NodeType node, bool is_cnf = false>
tref reduce(tref fm);

// Create assignment in formula and reduce resulting clause
template <NodeType node>
bool assign_and_reduce(tref fm, const trefs& vars, std::vector<int_t>& i,
	auto& dnf, const auto& is_var, int_t p = 0, bool is_wff = false);

// Given a BF b, calculate the Boole normal form (DNF corresponding to the pathes to true in the BDD) of b
// where the variable order is given by the function lex_var_comp
template <NodeType node>
tref bf_reduced_dnf(tref fm, bool make_paths_disjoint = false);

// The needed class in order to make bf_to_reduce_dnf work with rule applying process
template <NodeType node>
struct bf_reduce_canonical {
	tref operator()(tref fm) const;
};

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t,
	const bf_reduce_canonical<node>& r);

// template<typename... BAs>
// std::optional<tref> operator|(const std::optional<tref>& fm, const bf_reduce_canonical<BAs...>& r) {
// 	return fm.has_value() ? r(fm.value()) : std::optional<tref>{};
// }

inline bool is_contained_in(const std::vector<int_t>& i, auto& paths);

template <NodeType node>
std::pair<std::vector<int_t>, bool> clause_to_vector(tref clause,
	const auto& var_pos, const bool wff, const bool is_cnf);

template <NodeType node>
std::vector<std::vector<int_t>> collect_paths(tref new_fm, bool wff,
	const auto& vars, bool& decided, bool is_cnf, bool all_reductions = true);

template <NodeType node>
tref build_reduced_formula(const auto& paths, const auto& vars, bool is_cnf,
	bool wff, size_t type_id);

template <NodeType node>
std::pair<std::vector<std::vector<int_t>>, trefs> dnf_cnf_to_reduced(
	tref fm, bool is_cnf = false);

template <NodeType node>
tref group_dnf_expression(tref fm);

template <NodeType node>
bool is_ordered_subset(const auto& v1, const auto& v2);

template <NodeType node>
bool is_ordered_overlap_at_least(size_t i, const trefs& v1, const trefs& v2);

template <NodeType node>
int_t get_ordered_overlap(const trefs& v1, const trefs& v2);

template <NodeType node>
struct wff_reduce_dnf {
	tref operator() (tref fm) const;
};

template <NodeType node>
struct wff_reduce_cnf {
	tref operator() (tref fm) const;
};

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const wff_reduce_dnf<node>& r);

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const wff_reduce_cnf<node>& r);

template <NodeType node>
tref conjunct_dnfs_to_dnf(tref d1, tref d2);

template <NodeType node>
tref disjunct_cnfs_to_cnf(tref c1, tref c2);

// Can be used for Tau formula and Boolean function
template <NodeType node, bool is_wff = true>
tref push_negation_one_in(tref fm);

// Can be used for Tau formula and Boolean function
template <NodeType node, bool is_wff>
tref push_negation_in(tref fm);

// Conversion to dnf while applying reductions during the process
template <NodeType node, bool is_wff>
tref to_dnf(tref fm);

// Conversion to cnf while applying reductions during the process
template <NodeType node, bool is_wff>
tref to_cnf(tref fm);

// Shift the lookback in a formula
template <NodeType node>
tref shift_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift);

template <NodeType node>
tref shift_const_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift);

// Adjust the lookback before conjunction of fm1 and fm2
template <NodeType node>
tref always_conjunction (tref fm1_aw, tref fm2_aw);

template <NodeType node>
tref push_existential_quantifier_one(tref fm, subtree_set<node>* excluded = nullptr);

template <NodeType node>
tref push_universal_quantifier_one(tref fm);

// Squeeze all equalities found in n
template <NodeType node>
tref squeeze_positives(tref n, size_t type_id);

template <NodeType node>
tref replace_free_vars_by(tref fm, tref val);

template <NodeType node>
struct simplify_using_equality;

template <NodeType node>
class syntactic_path_simplification;

template <NodeType node>
tref syntactic_variable_simplification(tref atomic_fm, tref var);

template <NodeType node>
tref syntactic_formula_simplification(tref formula);

template <NodeType node>
tref syntactic_atomic_formula_simplification (tref atomic_formula);

template <NodeType node>
tref squeeze_absorb (tref formula);

template <NodeType node>
tref squeeze_absorb (tref formula, tref var);

template <NodeType node>
tref term_boole_decomposition(tref term, tref var);

template <NodeType node>
tref rec_term_boole_decomposition(tref term, const trefs& vars, const int_t idx);

template <NodeType node>
tref term_boole_decomposition(tref term);

template <NodeType node>
tref rec_boole_decomposition(tref formula, const trefs& vars, const int_t idx);

template <NodeType node>
tref boole_normal_form(tref formula);

template <NodeType node>
tref term_boole_normal_form(tref formula);

template <NodeType node>
tref ex_quantified_boole_decomposition(tref ex_quant_fm, auto& pool,
	auto& quant_pattern, subtree_set<node>* excluded, bool& no_atms);

template <NodeType node>
tref treat_ex_quantified_clause(tref ex_clause, bool& quant_eliminated);

template <NodeType node>
tref anti_prenex(tref formula);

template <NodeType node>
tref resolve_quantifiers(tref formula);

template <NodeType node, bool normalize_scopes = true>
tref normalize_temporal_quantifiers(tref fm);

template <NodeType node>
tref build_split_wff_using(typename node::type type, tref a, tref b);

template <NodeType node, size_t type>
tref anf(tref n);

template <NodeType node>
tref pnf(tref n);

} // namespace idni::tau_lang

#include "normal_forms.tmpl.h"

#endif // __IDNI__TAU__NORMAL_FORMS_H__
