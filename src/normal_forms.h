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
	to_dnf2_m,
	to_cnf2_m,
	eliminate_quantifiers_m,
	anti_prenex_step_m
};

/**
 * This enum holds the possible memory slots for traversals using the
 * post_order class
 */
enum MemorySlotPost {
	anti_prenex_m
};

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

template <NodeType node, node::type type>
struct reduce_deprecated {
	using tau = tree<node>;
	using tt = tau::traverser;

	// TODO (VERY_HIGH) properly implement it
	tref operator()(tref form) const;
	tt operator()(const tt& t) const;

private:
	using literals = subtree_set<node>;

	void get_literals(tref clause, literals& lits) const;

	literals get_literals(tref clause) const;

	std::pair<literals, literals> get_positive_negative_literals(
		tref clause) const;

	subtree_set<node> get_dnf_clauses(tref n,
					subtree_set<node> clauses = {}) const;

	tref build_dnf_clause_from_literals(const literals& positives,
					const literals& negatives) const;

	tref to_minterm(tref clause) const;

	tref build_dnf_from_clauses(const subtree_set<node>& clauses) const;

	tref simplify(tref form) const;
};

template <NodeType node>
static const reduce_deprecated<node, tau_parser::bf> reduce_bf_deprecated;
template <NodeType node>
using reduce_bf_t = reduce_deprecated<node, tau_parser::bf>;

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t, const reduce_bf_t<node>& r);

template <NodeType node>
static const reduce_deprecated<node, tau_parser::wff> reduce_wff_deprecated;
template <NodeType node>
using reduce_wff_t = reduce_deprecated<node, tau_parser::wff>;

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t, const reduce_wff_t<node>& r);

// return the inner quantifier or the top wff if the formula is not quantified
template <NodeType node>
std::pair<typename tree<node>::traverser, tref> get_inner_quantified_wff(tref n);

template <NodeType node>
struct onf_wff {
	onf_wff(tref _var);
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
static const auto is_not_eq_or_neq_to_zero_predicate = [](tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	const auto& t = tau::get(n);
	if (!(t.child_is(tau::bf_eq) || t.child_is(tau::bf_neq))) return true;
	auto check = (tt(t.only_child()) || tau::bf)[1] || tau::bf_f;
	return check.empty();
};

template <NodeType node>
using is_not_eq_or_neq_predicate_t = decltype(is_not_eq_or_neq_to_zero_predicate<node>);

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
		|| t.child_is(tau::constraint)
		|| t.child_is(tau::bv_eq)
		|| t.child_is(tau::bv_neq)
		|| t.child_is(tau::bv_lteq)
		|| t.child_is(tau::bv_nlteq)
		|| t.child_is(tau::bv_gt)
		|| t.child_is(tau::bv_ngt)
		|| t.child_is(tau::bv_gteq)
		|| t.child_is(tau::bv_ngteq)
		|| t.child_is(tau::bv_lt)
		|| t.child_is(tau::bv_nlt);
};

template <NodeType node>
inline auto is_bf_bdd_var = [](tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	return t.child_is(tau::variable)
		|| t.child_is(tau::capture)
		|| t.child_is(tau::bf_ref)
		|| t.child_is(tau::bf_constant);
};
// ------------------------------

// Starting from variable at position p+1 in vars write to i which variables are irrelevant in assignment
template <NodeType node>
void elim_vars_in_assignment(tref fm, const auto& vars, auto& i,
	const int_t p, const auto& is_var);

// Declaration of functions used in assign_and_reduce which are implemented later
template <NodeType node>
tref reduce(tref fm, size_t type, bool is_cnf = false,
	bool all_reductions = true, bool enable_sort = true);

// Create assignment in formula and reduce resulting clause
template <NodeType node>
bool assign_and_reduce(tref fm, const trefs& vars, std::vector<int_t>& i,
	auto& dnf, const auto& is_var, int_t p = 0, bool is_wff = false);

// Given a BF b, calculate the Boole normal form (DNF corresponding to the pathes to true in the BDD) of b
// where the variable order is given by the function lex_var_comp
template <NodeType node>
tref bf_boole_normal_form(tref fm, bool make_paths_disjoint = false);

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
	bool wff);

template <NodeType node>
tref sort_var(tref var);

template <NodeType node>
std::pair<std::vector<std::vector<int_t>>, trefs> dnf_cnf_to_bdd(
	tref fm, size_t type, bool is_cnf = false,
	bool all_reductions = true, bool enable_sort = true);
template <NodeType node>
tref group_dnf_expression(tref fm);

// Simplifies for example abx'|aby'|xy to ab|xy
// General idea is to eliminate (xyz...)' | xyz... after factorization
template <NodeType node>
tref simp_general_excluded_middle(tref fm);
// Assume that fm is in DNF (or CNF -> set is_cnf to true)
// TODO: Normalize Tau constants in case type == bf
template <NodeType node>
tref reduce(tref fm, size_t type, bool is_cnf,
	bool all_reductions, bool enable_sort);
template <NodeType node>
tref reduce_terms(tref fm, bool with_sorting = false);

// Apply equalities across clauses for further simplifications
// Assumes that fm is in DNF with equalities pushed in and
// inequalities pushed out
template <NodeType node>
tref apply_eqs_across_clauses(tref fm);

template <NodeType node>
trefs push_eq_and_get_vars(trefs fm);
// Requires fm to have Boolean functions in DNF and equalities pushed in
// vars contains variable order
template <NodeType node>
std::vector<std::vector<int_t>> wff_to_bdd(tref fm, auto& vars = {});

template <NodeType node>
bool is_ordered_subset(const auto& v1, const auto& v2);

template <NodeType node>
std::vector<std::vector<trefs>> get_cnf_inequality_lits(tref fm);

template <NodeType node>
std::pair<std::vector<int_t>, bool> simplify_path(
	const std::vector<int_t>& path, const trefs& vars);

template <NodeType node>
std::pair<tref, bool> group_paths_and_simplify(
	std::vector<std::vector<int_t>>& paths, const auto& vars);

template <NodeType node>
tref reduce_across_bfs(tref fm, bool to_cnf);

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

template <NodeType node>
tref single_dnf_lift(tref fm);

// Conversion to cnf while applying reductions during the process
template <NodeType node, bool is_wff>
tref to_cnf(tref fm);

// Assumes that fm is a single DNF always clause
template <NodeType node>
tref rm_temporary_lookback(tref fm);

// Assumes a single sometimes DNF clause with negation pushed in containing no wff_or with max nesting depth 1
template <NodeType node>
tref extract_sometimes(tref fm);

// Assumes a single DNF always clause in cnf with negation pushed in containing no wff_and with max nesting depth 1
template <NodeType node>
tref extract_always(tref fm);

// Recursively extract non-dependend formulas under sometimes
template <NodeType node>
tref push_sometimes_always_in(tref fm, auto& visited);

// Shift the lookback in a formula
template <NodeType node>
tref shift_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift);

template <NodeType node>
tref shift_const_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift);

// Assumes a single DNF clause and normalizes the "always" parts into one
template <NodeType node>
tref pull_always_out(tref fm);

// We assume that there is no nesting of "sometimes" and "always" in fm
// and that fm is in DNF
template <NodeType node>
tref pull_sometimes_always_out(tref fm);

// Adjust the lookback before conjunction of fm1 and fm2
template <NodeType node>
tref always_conjunction (tref fm1_aw, tref fm2_aw);

template <NodeType node>
struct sometimes_always_normalization {
	tref operator()(tref fm) const;
};

// Assumes that fm is normalized
template <NodeType node>
tref pull_always_out_for_inf(tref fm);

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const sometimes_always_normalization<node>& r);

template <NodeType node>
tref push_existential_quantifier_one(tref fm);

template <NodeType node>
tref push_universal_quantifier_one(tref fm);

// Squeeze all equalities found in n
//TODO: make it type depended
template <NodeType node>
tref squeeze_positives(tref n);

template <NodeType node>
tref wff_remove_existential(tref var, tref wff);

template <NodeType node>
tref eliminate_existential_quantifier(tref inner_fm, tref scoped_fm);

template <NodeType node>
tref eliminate_universal_quantifier(tref inner_fm, tref scoped_fm);

// Pushes all universal and existential quantifiers as deep as possible into the formula
// and then eliminate them, returning a quantifier free formula
template <NodeType node>
tref eliminate_quantifiers(tref fm);

// fm is assumed to be quantifier free
template <NodeType node>
tref get_eq_with_most_quant_vars(tref fm, const auto& quant_vars);

template <NodeType node>
std::pair<tref, bool> anti_prenex_finalize_ex(tref q, tref scoped_fm);

template <NodeType node>
tref anti_prenex(const tref& fm);

template <NodeType node>
tref replace_free_vars_by(tref fm, tref val);

template <NodeType node>
struct simplify_using_equality;

// We assume that the input is a formula is in MNF (with no quantifiers whatsoever).
// We implicitly transformed into BDD form and compute one step of the SNF transformation.
template <NodeType node>
struct to_snf_step {
	using tau = tree<node>;
	using tt = tau::traverser;
	using constant = typename node::constant;

	using vars = subtree_set<node>;
	using exponent = subtree_set<node>;
	using literal = tref;
	using literals = subtree_set<node>;
	using partition = std::map<exponent, literals>;
	using bdd_path = std::pair<partition /* positive */, partition /* negatives */>;

	tref operator()(tref form) const;

private:

	static constexpr auto _or = overloaded(
		[]<typename T>(const T& l, const T& r) -> constant {
			return l | r;
		},
		[](const auto&, const auto&) -> constant {
			throw std::logic_error("wrong types");
		});
	static constexpr auto _and = overloaded(
		[]<typename T>(const T& l, const T& r) -> constant {
			return l & r;
		},
		[](const auto&, const auto&) -> constant {
			throw std::logic_error("wrong types");
		});

	static constexpr auto _neg = overloaded(
		[]<typename T>(const T& l) -> constant {
			return ~l;
		},
		[](const auto&) -> constant {
			throw std::logic_error("wrong types");
		});

	static constexpr auto _neq_zero = overloaded(
		[]<typename T>(const T& l) -> bool {
			return l != false;
		},
		[](const auto&) -> bool {
			throw std::logic_error("wrong types");
		});

	static constexpr auto _leq = overloaded(
		[]<typename T>(const T& l, const T& r) -> bool {
			return (l & ~r) == false;
		},
		[](const auto&, const auto&) -> bool {
			throw std::logic_error("wrong types");
		});

	tref bdd_path_to_snf(const bdd_path& path, tref form) const;

	bdd_path add_to_negative_path(const bdd_path& path, literal lit) const;
	bdd_path add_to_positive_path(const bdd_path& path, literal lit) const;

	tref traverse(const bdd_path& path, const literals& remaining, tref form) const;

	exponent get_exponent(const tref n) const;
	tref get_bf_constant(literal lit) const;
	std::optional<constant> get_constant(literal lit) const;
	partition make_partition_by_exponent(const literals& s) const;

	tref squeeze_positives(const literals& positives, const exponent& exp) const;

	bool is_less_eq_than(literal ll, const exponent& le, literal rl,
		const exponent& re) const;

	// remove redundant positives
	std::map<exponent, literal> remove_redundant_positives(const std::map<exponent, literal>& positives) const;
	partition remove_redundant_negatives(const partition& negatives) const;

	// squeezed positives as much as possible.
	std::map<exponent, literal> squeeze_positives(const partition& positives) const;
	bool is_less_eq_than(literal l, literal r) const;

	literal normalize(const literals& negatives, const literal positive, const exponent& exp) const;
	// normalize each bdd path applying Corollary 3.1 from TABA book with few
	// improvements related to the handling of negative literals.
	tref normalize(const bdd_path& path) const;

	bdd_path get_relative_path(const bdd_path& path, literal lit) const;

	tref normalize_positive(const bdd_path& path, literal positive) const;

	tref normalize_negative(const bdd_path& path, literal negative) const;
};

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& n,
	const to_snf_step<node>& r);

template <NodeType node>
tref snf_bf(tref n);

// We mostly follow the Remark 3.5 from the TABA book. However, we deviate at
// some points. In particular, we assume that the formula is in MNF,
// instead of in MNF+BDD. The reason behind this is that we want to avoid the
// construction of the BDD form and then traverse it. Our aim is to build the
// BDD form and traverse it afterwards.
template <NodeType node>
tref snf_wff(tref n);

template <NodeType node>
tref build_split_wff_using(typename node::type type, tref a, tref b);

template <NodeType node, size_t type>
tref anf(tref n);

template <NodeType node>
tref pnf(tref n);

} // namespace idni::tau_lang

#include "normal_forms.tmpl.h"

#endif // __IDNI__TAU__NORMAL_FORMS_H__
