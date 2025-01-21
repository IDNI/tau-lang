// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __NORMALIZER2_H__
#define __NORMALIZER2_H__

#include <string>
#include <optional>
#include <boost/type.hpp>
#include <boost/log/trivial.hpp>

#include "parser.h"
#include "normal_forms.h"
#include "bdd_handle.h"
#include "variant_ba.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau_lang {

RULE(WFF_ELIM_FORALL, "all $X $Y ::= ! ex $X !$Y.")

template<typename... BAs>
static auto elim_for_all = make_library<BAs...>(
	WFF_ELIM_FORALL
);

// executes the normalizer on the given source code taking into account the
// bindings provided.
template<typename... BAs>
rr<tau<BAs...>> normalizer(std::string& source, bindings<BAs...>& binds) {
	auto form_source = make_tau_source(source, { .start = tau_parser::rr });
	auto form = make_nso_rr_using_bindings(form_source, binds);
	return normalizer(form);
}

// executes the normalizer on the given source code taking into account the
// provided factory.
template<typename factory_t, typename... BAs>
rr<tau<BAs...>> normalizer(std::string& source, factory_t& factory) {
	auto form_source = make_tau_source(source, { .start = tau_parser::rr });
	auto form = make_nso_rr_using_factory(form_source, factory);
	return normalizer(form);
}

// IDEA (HIGH) rewrite steps as a tuple to optimize the execution
template<typename ... BAs>
tau<BAs...> normalizer_step(const tau<BAs...>& form) {
	#ifdef TAU_CACHE
	static std::map<tau<BAs...>, tau<BAs...>> cache;
	if (auto it = cache.find(form); it != cache.end()) return it->second;
	#endif // TAU_CACHE

	auto result = form
		// Push all quantifiers in and eliminate them
		| (tau_transform<BAs...>)eliminate_quantifiers<BAs...>
		// After removal of quantifiers, only subformulas previously under the scope of a quantifier
		// are reduced
		| bf_reduce_canonical<BAs...>()
		// Normalize always and sometimes quantifiers and reduce Tau formula
		| sometimes_always_normalization<BAs...>();
	#ifdef TAU_CACHE
	cache.emplace(form, result);
	#endif // TAU_CACHE
	return result;
}

// Assumes that the formula passed does not have temporal quantifiers
// This normalization will non perform the temporal normalization
template<typename ... BAs>
tau<BAs...> normalize_non_temp(const tau<BAs...>& fm) {
	#ifdef TAU_CACHE
	static std::map<tau<BAs...>, tau<BAs...>> cache;
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
	#endif // TAU_CACHE
	auto result = fm
		// Push all quantifiers in and eliminate them
		| (tau_transform<BAs...>)eliminate_quantifiers<BAs...>
		// After removal of quantifiers, only subformulas previously under the scope of a quantifier
		// are reduced
		| bf_reduce_canonical<BAs...>();
	result = reduce_across_bfs(result, false);
	#ifdef TAU_CACHE
	cache.emplace(fm, result);
	#endif // TAU_CACHE
	return result;
}

template<typename... BAs>
auto get_vars_from_nso(const tau<BAs...>& n) {
	return select_top(n, is_var_or_capture<BAs...>);
}

// Given a tau<BAs...> produce a number such that the variable x_i is
// neither a bool_variable nor a variable nor a capture
template<typename... BAs>
int_t get_new_var_id (const tau<BAs...> fm) {
	auto var_nodes = get_vars_from_nso<BAs...>(fm);
	std::set vars{1};
	for (auto var : var_nodes) {
		if (auto tmp = make_string(tau_node_terminal_extractor<BAs...>, var); tmp[0] == 'x') {
			tmp.erase(0,1);
			if (!tmp.empty()) vars.insert(stoi(tmp));
		} else if (tmp[0] == '?' && tmp[1] == 'x') {
			tmp.erase(0,2);
			if (!tmp.empty()) vars.insert(stoi(tmp));
		}
	}
	return *vars.rbegin() + 1;
}


// Given a tau<BAs...> produce a number i such that the uninterpreted constant const_i is
// not present
template<typename... BAs>
tau<BAs...> get_new_uniter_const (const tau<BAs...> fm, const std::string& name) {
	auto uniter_consts = select_top(fm, is_non_terminal<tau_parser::uninterpreted_constant, BAs...>);
	std::set ids {0};
	for (auto uniter_const : uniter_consts) {
		if (auto tmp = make_string(tau_node_terminal_extractor<BAs...>, uniter_const); tmp.find(name) != std::string::npos) {
			std::string id = tmp.substr(name.length() + 1, tmp.size()-1);
			if (!tmp.empty()) ids.insert(std::stoi(id));
		}
	}
	std::string id = std::to_string(*ids.rbegin() + 1);
	auto uniter_const = build_bf_uniter_const<BAs...>("", name + id);
	return uniter_const;
}

static inline std::vector<std::string> rr_v{"dummy"};
static inline std::map<std::string, size_t> rr_m{};
inline size_t rr_dict(const std::string& s) {
	if (auto it = rr_m.find(s); it != rr_m.end()) return it->second;
	return rr_m.emplace(s, rr_v.size()), rr_v.push_back(s), rr_v.size() - 1;
};

// ref offset info. first is offset type (num/capture/shift/variable)
// and second is value of num, rr_dict id of capture or 0 for shift and variable
using offset_t = std::pair<size_t, size_t>;
// extracts ref info. returns pair of rr_dict id of ref symbol
// and vector of its offsets (offset_t)
template <typename... BAs>
std::pair<size_t, std::vector<offset_t>> get_ref_info(
	const tau<BAs...>& ref)
{
	//ptree<BAs...>(std::cout << "ref? ", ref) << "\n";
	std::pair<size_t, std::vector<offset_t>> ret{
		rr_dict(get_ref_name(ref)), {} };
	auto offsets = ref | tau_parser::offsets || tau_parser::offset;
	//BOOST_LOG_TRIVIAL(debug) << "(T) -- get_ref " << ref << " " << ret.first << " offsets.size: " << offsets.size();
	for (const auto& offset : offsets) {
		auto t = offset	| only_child_extractor<BAs...>
				| non_terminal_extractor<BAs...>
				| optional_value_extractor<size_t>;
		int_t d = 0;
		if (t == tau_parser::num)
			d = offset | tau_parser::num
				| only_child_extractor<BAs...>
				| size_t_extractor<BAs...>
				| optional_value_extractor<size_t>;
		else if (t == tau_parser::capture)
			d = rr_dict(make_string(
				tau_node_terminal_extractor<BAs...>,
				(offset | tau_parser::capture).value()));
		ret.second.emplace_back(t, d);
		break; // consider only first offset for now
		// TODO (LOW) support multiindex offsets
		// need to find a canonical way of enumeration first
	}
	return ret;
};

template <typename... BAs>
std::optional<tau<BAs...>> get_ref(const tau<BAs...>& n) {
	auto ref = std::optional<tau<BAs...>>(n);
	while (ref.has_value() && is_non_terminal_node<BAs...>(ref.value())
		&& (tau_parser::ref != (ref.value()
			| non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>)))
				ref = ref | only_child_extractor<BAs...>;
	if (!ref.has_value() || !is_non_terminal_node<BAs...>(ref.value())
		|| tau_parser::ref != (ref.value()
			| non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>))
				return std::optional<tau<BAs...>>();
	return ref;
}

// Check that the Tau formula does not use Boolean combinations of models
template<typename ...BAs>
bool has_no_boolean_combs_of_models(const tau<BAs...>& fm) {
	if (is_non_terminal(tau_parser::wff_always, trim(fm))) {
		// check that there is no wff_always or wff_sometimes in the subtree
		if (find_top(trim2(fm), is_non_terminal<tau_parser::wff_always, BAs...>))
			return false;
		if (find_top(trim2(fm), is_non_terminal<tau_parser::wff_sometimes, BAs...>))
			return false;
	} else {
		if (find_top(fm, is_non_terminal<tau_parser::wff_always, BAs...>))
			return false;
		if (find_top(fm, is_non_terminal<tau_parser::wff_sometimes, BAs...>))
			return false;
	}
	return true;
}

// This method was used before tau_parser::uninterpreted constant was moved
// under the tau_parser::variable node
// template<typename... BAs>
// tau<BAs...> convert_uconsts_to_var (const tau<BAs...>& fm) {
// 	auto uconsts = select_top(fm, is_non_terminal<tau_parser::uninterpreted_constant, BAs...>);
// 	std::map<tau<BAs...>, tau<BAs...>> changes;
// 	for (const auto& uc : uconsts) {
// 		std::stringstream ss; ss << uc;
// 		auto tvar = build_variable<BAs...>(ss.str());
// 		changes.emplace(uc, tvar);
// 	}
// 	return replace(fm, changes);
// }

template<typename... BAs>
bool is_non_temp_nso_satisfiable (const tau<BAs...>& fm) {
	assert(!find_top(fm, is_non_terminal<tau_parser::wff_always, BAs...>));
	assert(!find_top(fm, is_non_terminal<tau_parser::wff_sometimes, BAs...>));

	auto new_fm = fm;
	// Convert uninterpreted constants to variables for sat check
	// new_fm = convert_uconsts_to_var(new_fm);
	auto vars = get_free_vars_from_nso(new_fm);
	for(auto& v: vars) new_fm = build_wff_ex<BAs...>(v, new_fm);
	auto normalized = normalize_non_temp<BAs...>(new_fm);
	assert((normalized == _T<BAs...> || normalized == _F<BAs...>));
	return normalized == _T<BAs...>;
}

template <typename... BAs>
bool are_nso_equivalent(tau<BAs...> n1, tau<BAs...> n2) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin are_nso_equivalent";
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n1 " << n1;
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n2 " << n2;

	// If this method is called on a formula that has Boolean combinations of models, it is used incorrectly
	assert((has_no_boolean_combs_of_models(n1) && has_no_boolean_combs_of_models(n2)));

	if (is_non_terminal(tau_parser::wff_always, trim(n1)))
		n1 = trim2(n1);
	if (is_non_terminal(tau_parser::wff_always, trim(n2)))
		n2 = trim2(n2);

	if (n1 == n2) {
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: true (equiv nodes)";
		return true;
	}

	// equivalence of references
	auto r1opt = get_ref(n1), r2opt = get_ref(n2);
	if (r1opt && r2opt) { // both are refs
		bool equiv = get_ref_info(r1opt.value())
						== get_ref_info(r2opt.value());
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: "<<equiv<<" (equiv refs)";
		return equiv;
	}
	else if (r1opt || r2opt) { // one is a ref
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: false (ref and not ref)";
		return false;
	}

	auto vars = get_free_vars_from_nso(n1);
	auto vars2 = get_free_vars_from_nso(n2);
	vars.insert(vars2.begin(), vars2.end());

	tau<BAs...> imp1 = build_wff_imply<BAs...>(n1, n2);
	tau<BAs...> imp2 = build_wff_imply<BAs...>(n2, n1);

	for(auto& v: vars) {
		imp1 = build_wff_all<BAs...>(v, imp1);
		imp2 = build_wff_all<BAs...>(v, imp2);
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) -- wff: " << build_wff_and(imp1, imp2);

	auto dir1 = normalizer_step<BAs...>(imp1);
	assert((dir1 == _T<BAs...> || dir1 == _F<BAs...> || find_top(dir1,
		is_non_terminal<tau_parser::constraint, BAs...>)));
	if (dir1 == _F<BAs...>) {
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: " << dir1;
		return false;
	}
	auto dir2 = normalizer_step<BAs...>(imp2);
	assert((dir2 == _T<BAs...> || dir2 == _F<BAs...> || find_top(dir2,
		is_non_terminal<tau_parser::constraint, BAs...>)));
	bool res = (dir1 == _T<BAs...> && dir2 == _T<BAs...>);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: " << res;

	return res;
}

template <typename... BAs>
auto is_nso_equivalent_to_any_of(tau<BAs...>& n, std::vector<tau<BAs...>>& previous) {
	return std::any_of(previous.begin(), previous.end(), [n] (tau<BAs...>& p) {
			return are_nso_equivalent<BAs...>(n, p);
		});
}

template<typename... BAs>
bool is_nso_impl (tau<BAs...> n1, tau<BAs...> n2) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin is_nso_impl";
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n1 " << n1;
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n2 " << n2;

	// If this method is called on a formula that has Boolean combinations of models, it is used incorrectly
	assert((has_no_boolean_combs_of_models(n1) && has_no_boolean_combs_of_models(n2)));

	if (is_non_terminal(tau_parser::wff_always, trim(n1)))
		n1 = trim2(n1);
	if (is_non_terminal(tau_parser::wff_always, trim(n2)))
		n2 = trim2(n2);

	if (n1 == n2) {
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End is_nso_impl: true (n1 implies n2)";
		return true;
	}

	auto vars = get_free_vars_from_nso(n1);
	auto vars2 = get_free_vars_from_nso(n2);
	vars.insert(vars2.begin(), vars2.end());

	tau<BAs...> imp = build_wff_imply<BAs...>(n1, n2);

	for(auto& v: vars) {
		imp = build_wff_all<BAs...>(v, imp);
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) -- wff: " << imp;

	auto res = normalizer_step<BAs...>(imp);
	assert((res == _T<BAs...> || res == _F<BAs...> || find_top(res,
		is_non_terminal<tau_parser::constraint, BAs...>)));
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End is_nso_impl: " << res;
	return res == _T<BAs...>;
}

template<typename... BAs>
bool are_bf_equal(tau<BAs...> n1, tau<BAs...> n2) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin are_bf_equal";
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n1 " << n1;
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n2 " << n2;

	assert(is_non_terminal(tau_parser::bf, n1));
	assert(is_non_terminal(tau_parser::bf, n2));

	if (n1 == n2) {
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_bf_equal: true (equal bf)";
		return true;
	}

	auto vars = get_free_vars_from_nso(n1);
	auto vars2 = get_free_vars_from_nso(n2);
	vars.insert(vars2.begin(), vars2.end());

	tau<BAs...> bf_equal_fm = build_wff_eq(build_bf_xor(n1, n2));

	for(auto& v: vars) bf_equal_fm = build_wff_all<BAs...>(v, bf_equal_fm);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- wff: " << bf_equal_fm;

	auto normalized = normalizer_step<BAs...>(bf_equal_fm);
	BOOST_LOG_TRIVIAL(trace) << "(T) -- Normalized: " << normalized;
	auto check = normalized | tau_parser::wff_t;
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_bf_equal: " << check.has_value();

	return check.has_value();
}

template <typename... BAs>
auto is_bf_same_to_any_of(tau<BAs...>& n, std::vector<tau<BAs...>>& previous) {
	return std::any_of(previous.begin(), previous.end(), [n] (tau<BAs...>& p) {
			return are_bf_equal<BAs...>(n, p);
		});
}

// Normalize fm and additionally simplify implications between
// always, !always, sometimes and !sometimes statements
template<typename... BAs>
tau<BAs...> normalize_with_temp_simp (const tau<BAs...>& fm) {
	using p = tau_parser;
	auto trim_q = [](const auto& n) {
		if (is_child_non_terminal(p::wff_always, n))
			return trim2(n);
		if (is_child_non_terminal(p::wff_sometimes, n))
			return trim2(n);
		return n;
	};
	auto push_neg = [](const auto& n) {
		if (is_child_non_terminal(p::wff_neg, n) &&
			(is_child_non_terminal(p::wff_always, trim2(n)) ||
			is_child_non_terminal(p::wff_sometimes, trim2(n))))
			return build_wff_neg(trim2(trim2(n)));
		return n;
	};
	auto is_neg_st = [](const auto& n) {
		if (is_child_non_terminal(p::wff_neg, n) &&
			is_child_non_terminal(p::wff_sometimes, trim2(n)))
			return true;
		else return false;
	};
	auto is_neg_aw = [](const auto& n) {
		if (is_child_non_terminal(p::wff_neg, n) &&
			is_child_non_terminal(p::wff_always, trim2(n)))
			return true;
		else return false;
	};
	auto red_fm = normalizer_step(fm);
	auto clauses = get_dnf_wff_clauses(red_fm);
	tau<BAs...> new_fm;
	for (const auto& clause : clauses) {
		auto aw_parts = select_top_until(clause,
			is_child_non_terminal<p::wff_always, BAs...>,
			is_child_non_terminal<p::wff_neg, BAs...>);
		auto neg_aw_parts = select_top(clause, is_neg_aw);
		// TODO: activate once new sometimes definition is there
		// auto st_parts = select_top(clause, is_st);
		// std::ranges::for_each(st_parts, trim2);
		// auto neg_st_parts = select_top(clause, is_neg_st);

		// Replace (!)always and (!)sometimes parts by T
		std::map<tau<BAs...>, tau<BAs...>> changes;
		for (const auto& aw : aw_parts)
			changes.emplace(aw, _T<BAs...>);
		for (const auto& naw : neg_aw_parts)
			changes.emplace(naw, _T<BAs...>);
		// for (const auto& st : st_parts)
		// 	changes.emplace(st, _T<BAs...>);
		// for (const auto& nst : neg_st_parts)
		// 	changes.emplace(nst, _T<BAs...>);
		tau<BAs...> new_clause = replace(clause, changes);
		std::cout << "new_clause: " << new_clause << "\n";

		// First check if any always statements are implied by others
		for (size_t i = 0; i < aw_parts.size(); ++i) {
			for (size_t j = i+1; j < aw_parts.size(); ++j) {
				if (is_nso_impl(aw_parts[i], aw_parts[j]))
					aw_parts[j] = _T<BAs...>;
				else if (is_nso_impl(aw_parts[j], aw_parts[i]))
					aw_parts[i] = _T<BAs...>;
			}
		}
		// Next check if any always statement implies a !always statement
		for (const auto& aw : aw_parts) {
			for (auto& naw : neg_aw_parts) {
				if (is_nso_impl(aw, push_neg(naw)))
					naw = _T<BAs...>;
			}
		}
		// Now check if any !always statement implies another !always
		for (size_t i = 0; i < neg_aw_parts.size(); ++i) {
			for (size_t j = i+1; j < neg_aw_parts.size(); ++j) {
				if (is_nso_impl(push_neg(neg_aw_parts[i]), push_neg(neg_aw_parts[j])))
					neg_aw_parts[j] = _T<BAs...>;
				else if (is_nso_impl(push_neg(neg_aw_parts[j]), push_neg(neg_aw_parts[i])))
					neg_aw_parts[i] = _T<BAs...>;
			}
		}
		new_clause = build_wff_and(new_clause, build_wff_and(
						build_wff_and<BAs...>(aw_parts),
						build_wff_and<BAs...>(neg_aw_parts)));
		if (new_fm) new_fm = build_wff_or(new_fm, new_clause);
		else new_fm = new_clause;
	}
	assert(new_fm != nullptr);
	std::cout << "new_fm: " << new_fm << "\n";
	return new_fm;
}

template <typename... BAs>
size_t get_max_loopback_in_rr(const tau<BAs...>& form) {
	size_t max = 0;
	for (const auto& offset: select_top(form, is_non_terminal<tau_parser::offsets, BAs...>)) {
		auto current = offset
			| tau_parser::offset
			| tau_parser::num
			| only_child_extractor<BAs...>
			| offset_extractor<BAs...>;
		max = current.has_value() ? std::max(max, current.value()) : max;
	}
	return max;
}

template<typename... BAs>
tau<BAs...> build_shift_from_shift(tau<BAs...> shift, size_t step) {
	auto num = shift | tau_parser::num | optional_value_extractor<tau<BAs...>>;
	auto offset = num | only_child_extractor<BAs...> | offset_extractor<BAs...> | optional_value_extractor<size_t>;
	if (step == offset) return shift | tau_parser::capture | optional_value_extractor<tau<BAs...>>;
	std::map<tau<BAs...>, tau<BAs...>> changes{{num, build_num<BAs...>(step - offset)}};
	return replace<tau<BAs...>>(shift, changes);
}

template<typename... BAs>
tau<BAs...> build_main_step(const tau<BAs...>& form, size_t step) {
	std::map<tau<BAs...>, tau<BAs...>> changes;
	for (const auto& offset : select_top(form,
				is_non_terminal<tau_parser::offsets, BAs...>))
	{
		auto shift = offset | tau_parser::shift;
		if (!shift.has_value()) continue;
		auto nshift = build_shift_from_shift<BAs...>(shift.value(), step);
		changes[shift.value()] = nshift;
	}
	return replace<tau<BAs...>>(form, changes);
}

// Normalizes a Boolean function having no recurrence relation
template<typename... BAs>
tau<BAs...> bf_normalizer_without_rec_relation (const tau<BAs...>& bf) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin Boolean function normalizer";

	auto result = bf_boole_normal_form(bf);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End Boolean function normalizer";

	return result;
}

// Normalizes a Boolean function in which recurrence relations are present
template<typename... BAs>
tau<BAs...> bf_normalizer_with_rec_relation(const rr<tau<BAs...>> &bf) {
	BOOST_LOG_TRIVIAL(debug)<< "(I) -- Begin calculate recurrence relation";

	auto main = calculate_all_fixed_points(bf);
	if (!main) return nullptr;

	auto bf_unfolded = main | repeat_all<step<BAs...>, BAs...>(
					step<BAs...>(bf.rec_relations));

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End calculate recurrence relation";

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin Boolean function normalizer";

	auto result = bf_boole_normal_form(bf_unfolded);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End Boolean function normalizer";

	return result;
}

// enumerates index in main with step i - used for finding a fixed point
template <typename... BAs>
tau<BAs...> build_enumerated_main_step(const tau<BAs...>& form, size_t i,
	size_t offset_arity)
{
	auto r = form;
	std::map<tau<BAs...>, tau<BAs...>> changes;
	std::vector<tau<BAs...>> ofs; // create offsets node
	ofs.push_back(wrap<BAs...>(tau_parser::offset, build_num<BAs...>(i)));
	for (size_t o = 1; o < offset_arity; ++o)
		ofs.push_back(wrap<BAs...>(tau_parser::offset,
							build_num<BAs...>(0)));

	// create enumerated replacement
	auto ref = r | only_child_extractor<BAs...>
		| tau_parser::ref
		| optional_value_extractor<tau<BAs...>>;
	changes[ref] = rewriter::make_node<tau_sym<BAs...>>(ref->value, {
		ref->child[0],
		wrap<BAs...>(tau_parser::offsets, ofs), ref->child[1] });
	r = replace(r, changes);
	BOOST_LOG_TRIVIAL(debug) << "(F*) " << r;
	//DBG(ptree<BAs...>(std::cout << "enumerated main ", r) << "\n";)
	return build_main_step(r, i);
}

template <typename... BAs>
bool is_valid(const rr<tau<BAs...>>& nso_rr) {
	for (const auto& main_offsets : select_all(nso_rr.main,
		is_non_terminal<tau_parser::offsets, BAs...>))
			if (find_top(main_offsets,
				is_non_terminal<tau_parser::capture, BAs...>))
	{
		BOOST_LOG_TRIVIAL(error) << "(Error) Main " << nso_rr.main
					<< " cannot contain a relative offset "
					<< main_offsets;
		return false; // capture in main's offset
	}
	for (size_t ri = 0; ri != nso_rr.rec_relations.size(); ++ri) {
		const auto& r = nso_rr.rec_relations[ri];
		auto left = get_ref_info(get_ref(r.first).value());
		for (const auto& [ot, _] : left.second)
			if (ot == tau_parser::shift) {
				BOOST_LOG_TRIVIAL(error) << "(Error) Recurrence "
					"relation " << r.first << " cannot "
					"contain an offset shift";
				return false; // head ref cannot have shift
			}
		if (left.second.size() == 0) continue; // no offsets
		// take only first offset for consideration
		offset_t ho = left.second.front();
		//BOOST_LOG_TRIVIAL(debug) << "(T) -- head offset " << ho.first << " / " << ho.second;
		for (const auto& ref : select_all(r.second,
			is_non_terminal<tau_parser::ref, BAs...>))
		{
			auto right = get_ref_info(ref);
			if (right.second.size() == 0) continue; // no offsets
			auto& bo = right.second.front();
			//BOOST_LOG_TRIVIAL(debug) << "(T) -- body offset " << bo.first << " / " << bo.second;
			if (ho.first == tau_parser::num) {
				if (bo.first == tau_parser::capture) {
					BOOST_LOG_TRIVIAL(error)
						<< "(Error) Recurrence relation "
						<< r.first << " (having a fixed"
						" offset) cannot depend on a "
						"relative offset " << r.second;
					return false; // left num right capture
				}
				if (bo.first == tau_parser::num
					&& ho.second < bo.second) {
						BOOST_LOG_TRIVIAL(error)
							<<"(Error) Recurrence relation "
							<< r.first << " cannot "
							"depend on a future "
							"state " << r.second;
						return false; // l num < r num
				}
			}
		}
	}
	//BOOST_LOG_TRIVIAL(debug) << "(I) -- Recurrence relation is well founded";
	return true;
}

template <typename... BAs>
bool is_well_founded(const rr<tau<BAs...>>& nso_rr) {
	std::unordered_map<size_t, std::set<size_t>> graph;
	std::unordered_map<size_t, bool> visited, visiting;
	std::function<bool(size_t)> is_cyclic = [&](size_t n) {
		if (visiting[n]) return true;
		if (visited[n]) return false;
		visiting[n] = true;
		for (const auto& neighbor : graph[n])
			if (is_cyclic(neighbor)) return true;
		visiting[n] = false;
		visited[n]  = true;
		return false;
	};
	bool has_relative_rule = false;
	for (size_t ri = 0; ri != nso_rr.rec_relations.size(); ++ri) {
		const auto& r = nso_rr.rec_relations[ri];
		auto left = get_ref_info(get_ref(r.first).value());
		for (const auto& [ot, _] : left.second)
			if (ot == tau_parser::capture)
				has_relative_rule = true;
		if (left.second.size() == 0) continue; // no offsets
		// take only first offset for consideration
		offset_t ho = left.second.front();
		//BOOST_LOG_TRIVIAL(debug) << "(T) -- head offset " << ho.first << " / " << ho.second;
		for (const auto& ref : select_all(r.second,
			is_non_terminal<tau_parser::ref, BAs...>))
		{
			auto right = get_ref_info(ref);
			if (right.second.size() == 0) continue; // no offsets
			auto& bo = right.second.front();
			//BOOST_LOG_TRIVIAL(debug) << "(T) -- body offset " << bo.first << " / " << bo.second;
			if (ho == bo) graph[left.first].insert(right.first);
		}
		visited[left.first]  = false;
		visiting[left.first] = false;
	}
	if (!has_relative_rule) {
		BOOST_LOG_TRIVIAL(error) << "(Error) Recurrence relation has no rules "
						"other than initial conditions";
		return false;
	}
	for (const auto& [left, _] : graph)
		if (!visited[left] && is_cyclic(left)) {
			BOOST_LOG_TRIVIAL(error)
					<< "(Error) Recurrence relation is cyclic";
			return false;
		}
	BOOST_LOG_TRIVIAL(debug)<< "(I) -- Recurrence relation is well founded";
	return true;
}

template <typename... BAs>
tau<BAs...> calculate_fixed_point(const rr<tau<BAs...>>& nso_rr,
	const tau<BAs...>& form, tau_parser::nonterminal t, size_t offset_arity,
	const tau<BAs...>& fallback)
{
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Calculating fixed point: " << form;
	BOOST_LOG_TRIVIAL(debug) << "(F) " << nso_rr;
	//ptree<BAs...>(std::cout << "form: ", form) << "\n";

	auto ft = fallback | non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
	bool first = ft == tau_parser::first_sym,
		last = ft == tau_parser::last_sym;
	if (!first && !last && ft != t) {
		BOOST_LOG_TRIVIAL(error) << "(Error) Fallback type mismatch";
		return nullptr;
	}

	if (!is_well_founded(nso_rr)) return nullptr;

	std::vector<tau<BAs...>> previous;
	tau<BAs...> current;
	auto eos = "(I) -- End enumeration step";

	size_t max_loopback = 0;
	std::vector<size_t> loopbacks;
	for (const auto& r : nso_rr.rec_relations) {
		size_t loopback = std::max(get_max_loopback_in_rr(r.first),
					get_max_loopback_in_rr(r.second));
		loopbacks.push_back(loopback);
		max_loopback = std::max(max_loopback, loopback);
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) max loopback " << max_loopback;

	for (size_t i = max_loopback; ; i++) {
		current = build_enumerated_main_step<BAs...>(
					form, i, offset_arity);
		bool changed;
		do { // apply rec relation rules and check for cycle dependency
			changed = false;
			for (size_t ri = 0;
				ri != nso_rr.rec_relations.size(); ++ri)
			{
				const auto& r = nso_rr.rec_relations[ri];
				if (loopbacks[ri] > i) {
					BOOST_LOG_TRIVIAL(debug) << "(I) -- current step " << i << " < " << loopbacks[ri] << " loopback, skipping " << r;
					continue; // skip steps depending on future fixed offsets
				}
				auto prev = current;
				current = nso_rr_apply<BAs...>(r, prev);
				if (current != prev) changed = true;
			}
		} while (changed);

		BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin enumeration step";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;

		BOOST_LOG_TRIVIAL(debug) << "(I) -- Normalize step";
		current = t == tau_parser::wff ? normalizer_step(current)
					: bf_boole_normal_form(current);
		BOOST_LOG_TRIVIAL(debug) << "(T) -- Normalized step";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;

		if (previous.size()
		&& (t == tau_parser::wff
			? are_nso_equivalent<BAs...>(current, previous.back())
			: are_bf_equal(current, previous.back())))
		{
			BOOST_LOG_TRIVIAL(debug) << eos
				<< " - fixed point found at step: " << i;
			BOOST_LOG_TRIVIAL(debug) << "(F) " << previous.back();
			return previous.back();
		}
		else if (previous.size() > 1
			&& (t == tau_parser::wff
				? is_nso_equivalent_to_any_of(current, previous)
				: is_bf_same_to_any_of(current, previous)))
		{
			BOOST_LOG_TRIVIAL(debug) << eos
				<< " - loop (no fixed point) detected at step: "
				<< i << " returning fallback "
				<< (first ? "first" : last ? "last" : "");
			if (last) return previous.back();
			if (first) return current;
			BOOST_LOG_TRIVIAL(debug) << eos
				<< " - fallback: " << fallback;
			return fallback;
		}
		BOOST_LOG_TRIVIAL(debug) << eos
			<< " - no fixed point resolution at step: "
			<< i << " incrementing";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;
		previous.push_back(current);
	}
	DBG(assert(0);)
	return nullptr;
}

// calculate fixed points called from main and replace them by their results
template<typename... BAs>
struct fixed_point_transformer {

	fixed_point_transformer(const rr<tau<BAs...>>& defs,
		const rr_types& types) : defs(defs), types(types) {}

	tau<BAs...> operator()(const tau<BAs...>& n) {
		if (n->child.size() == 0) return n;
		if (auto it = changes.find(n); it != changes.end())
			return it->second;
		const auto& ref = n->child[0];
		bool is_ref = (is_non_terminal<tau_parser::wff, BAs...>(n)
			&& is_non_terminal<tau_parser::wff_ref, BAs...>(ref))
			|| (is_non_terminal<tau_parser::bf, BAs...>(n)
			&& is_non_terminal<tau_parser::bf_ref, BAs...>(ref));
		if (!is_ref) return n;
		auto [type, offset_arity] = get_type_info(types, ref);
		if (offset_arity) {
			auto fp = calculate_fixed_point(defs, n, type,
				offset_arity, get_fallback(type, ref));
			if (!fp) return nullptr;
			return changes.emplace(n, fp).first->second;
		}
		bool changed = false;
		std::vector<tau<BAs...>> child;
		if (changes.contains(ref))
			changed = true, child.push_back(changes[ref]);
		else child.push_back(ref);
		auto nn = make_node<tau_sym<BAs...>>(n->value, child);
		if (changed) changes[n] = nn;
		return nn;
	}

	std::pair<tau_parser::nonterminal, size_t> get_type_info(
		const rr_types& ts, const tau<BAs...>& fp_ref)
	{
		// size_t type = fp_ref | non_terminal_extractor<BAs...>
		// 	| optional_value_extractor<size_t>;
		auto ref = fp_ref | tau_parser::ref;
		if (ref && !(ref | tau_parser::offsets).has_value()) {
			auto fn = get_ref_name(ref.value());
			auto it = ts.types.find(fn);
			if (it != ts.types.end() && it->second.offset_arity)
				return { it->second.type,
						it->second.offset_arity };
		}
		return { tau_parser::wff, 0 };
	}

	tau<BAs...> get_fallback(tau_parser::nonterminal t,
		const tau<BAs...>& ref)
	{
		auto fallback = ref | tau_parser::ref | tau_parser::fp_fallback;
		if (!fallback) return t == tau_parser::wff
			? _F<BAs...> : _0<BAs...>;
		return fallback | only_child_extractor<BAs...>
			| optional_value_extractor<tau<BAs...>>;
	}

	std::map<tau<BAs...>, tau<BAs...>> changes;
	rr<tau<BAs...>> defs;
	rr_types types;
};

template<typename... BAs>
tau<BAs...> calculate_all_fixed_points(const rr<tau<BAs...>>& recrel) {
	// get types and do type checks
	rr_types types;
	bool success = true;
	get_rr_types(success, types, recrel);
	if (!success || !is_valid(recrel)) return nullptr;
	// transform fp calculation calls by calculation results
	fixed_point_transformer<BAs...> fpt(recrel, types);
	auto new_main = rewriter::post_order_traverser<decltype(fpt),
		rewriter::all_t, tau<BAs...>>(fpt, rewriter::all)(recrel.main);
	if (!new_main) return nullptr;
	if (fpt.changes.size()) {
		new_main = replace(new_main, fpt.changes);
		BOOST_LOG_TRIVIAL(debug) << "(I) -- Calculated fixed points. "
						"New main: " << new_main;
	}
	return new_main;
}

// This function applies the recurrence relations the formula comes with to
// the formula
template<typename... BAs>
tau<BAs...> apply_rr_to_formula (const rr<tau<BAs...>>& nso_rr) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Start apply_rr_to_formula";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << nso_rr;
	auto main = calculate_all_fixed_points(nso_rr);
	if (!main) return nullptr;
	// Substitute function and recurrence relation definitions
	auto new_main = main | repeat_all<step<BAs...>, BAs...>(
			    step<BAs...>(nso_rr.rec_relations));
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End apply_rr_to_formula";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << nso_rr;
	return new_main;
}

// REVIEW (HIGH) review overall execution
template <typename... BAs>
tau<BAs...> normalizer(const rr<tau<BAs...>>& nso_rr) {
	// IDEA extract this to an operator| overload

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin normalizer";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << nso_rr;

	auto fm = apply_rr_to_formula(nso_rr);
	if (!fm) return nullptr;
	auto res = normalize_with_temp_simp(fm);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End normalizer";
	return res;
}

template <typename... BAs>
tau<BAs...> normalizer(const tau<BAs...>& form) {
	rr<tau<BAs...>> nso_rr(form);
	return normalizer(nso_rr);
}

} // namespace idni::tau_lang

#endif // __NORMALIZER2_H__
