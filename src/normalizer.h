// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#ifndef __NORMALIZER2_H__
#define __NORMALIZER2_H__

#include <string>
#include <optional>
#include <boost/type.hpp>
#include <boost/log/trivial.hpp>

#include "parser.h"
#include "nso_rr.h"
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

using namespace idni::rewriter;

namespace idni::tau {

RULE(WFF_REMOVE_EX_0, "ex $X $Y ::= wff_remove_existential_cb $X $Y.")
RULE(WFF_ELIM_FORALL, "all $X $Y ::= ! ex $X !$Y.")

template<typename... BAs>
static auto elim_for_all = make_library<BAs...>(
	WFF_ELIM_FORALL
);

template<typename... BAs>
static auto wff_remove_existential = make_library<BAs...>(
	WFF_REMOVE_EX_0
);

// executes the normalizer on the given source code taking into account the
// bindings provided.
template<typename... BAs>
rr<nso<BAs...>> normalizer(std::string& source, bindings<BAs...>& binds) {
	auto form_source = make_tau_source(source);
	auto form = make_nso_rr_using_bindings(form_source, binds);
	return normalizer(form);
}

// executes the normalizer on the given source code taking into account the
// provided factory.
template<typename factory_t, typename... BAs>
rr<nso<BAs...>> normalizer(std::string& source, factory_t& factory) {
	auto form_source = make_tau_source(source);
	auto form = make_nso_rr_using_factory(form_source, factory);
	return normalizer(form);
}

template<typename... BAs>
rr<nso<BAs...>> apply_once_definitions(const rr<nso<BAs...>>& nso_rr) {
	auto nmain = apply_once_definitions(nso_rr.main);
	rec_relations<nso<BAs...>> nrec_relations;
	for (const auto& r : nso_rr.rec_relations) {
		auto [matcher, body] = r;
		nrec_relations.emplace_back(matcher, apply_once_definitions(body));
	}
	return { nrec_relations, nmain };
}

template<typename... BAs>
struct remove_one_wff_existential {
	nso<BAs...> operator()(nso<BAs...> n) const {
		auto inner_fm = find_bottom(n, is_child_non_terminal<tau_parser::wff_ex, BAs...>);
		// As long as a quantifier is found
		while (inner_fm) {
			auto removed = trim(inner_fm.value())->child[1]
				// Reductions to prevent blow ups
				// and DNF conversion needed for quantifier removal
				| bf_reduce_canonical<BAs...>()
				| repeat_all<step<BAs...>, BAs...>(to_nnf_wff<BAs...>)
				| repeat_all<step<BAs...>, BAs...>(nnf_to_dnf_wff<BAs...>)
				| wff_reduce_dnf<BAs...>();
			removed = build_wff_ex(trim2(inner_fm.value()), removed)
				| wff_remove_existential<BAs...>;
			std::map<nso<BAs...>, nso<BAs...>> changes{{inner_fm.value(), removed}};
			n = replace(n, changes);
			inner_fm = find_bottom(n, is_child_non_terminal<tau_parser::wff_ex, BAs...>);
			// In case a quantifier cannot be removed, quantifier elimination needs to stop
			auto has_node = [&inner_fm](const auto& node){return node == inner_fm;};
			if (find_top(removed, has_node))
				break;
		}
		return n;
	}
};

template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& form, const remove_one_wff_existential<BAs...>& r) {
	return r(form);
}

// IDEA (HIGH) rewrite steps as a tuple to optimize the execution
template<typename ... BAs>
nso<BAs...> normalizer_step(const nso<BAs...>& form) {
	#ifdef TAU_CACHE
	static std::map<nso<BAs...>, nso<BAs...>> cache;
	if (auto it = cache.find(form); it != cache.end()) return it->second;
	#endif // TAU_CACHE
	auto result = form
		| repeat_all<step<BAs...>, BAs...>(step<BAs...>(apply_defs<BAs...>))
		| repeat_all<step<BAs...>, BAs...>(step<BAs...>(elim_for_all<BAs...>))
		| remove_one_wff_existential<BAs...>()
		// After removal of existentials, only subformulas previously under the scope of a quantifier
		// are reduced
		| bf_reduce_canonical<BAs...>()
		| repeat_all<step<BAs...>, BAs...>(elim_eqs<BAs...>)
		| sometimes_always_normalization<BAs...>();
	#ifdef TAU_CACHE
	cache[form] = result;
	#endif // TAU_CACHE
	return result;
}

// TODO (LOW) refactor and clean this structure
template<typename... BAs>
struct free_vars_collector {

	free_vars_collector(std::set<nso<BAs...>>& free_vars) : free_vars(free_vars) {}

	nso<BAs...> operator()(const nso<BAs...>& n) {
		if (is_quantifier<BAs...>(n)) {
			// IDEA using quantified_variable => variable | capture would simplify the code
			auto var = find_top(n, is_var_or_capture<BAs...>);
			if (var.has_value()) {
				if (auto it = free_vars.find(var.value()); it != free_vars.end())
					free_vars.erase(it);
			}
			BOOST_LOG_TRIVIAL(trace) << "(I) -- removing quantified var: " << var.value();
		}
		if (is_var_or_capture<BAs...>(n)) {
			if (auto check = n
					| tau_parser::io_var | only_child_extractor<BAs...> | tau_parser::offset
					| only_child_extractor<BAs...>;
					check.has_value() && is_var_or_capture<BAs...>(check.value())) {
				auto var = check.value();
				if (auto it = free_vars.find(var); it != free_vars.end()) {
					free_vars.erase(it);
					BOOST_LOG_TRIVIAL(trace) << "(I) -- removing var: " << var;
				}
			}
			free_vars.insert(n);
			BOOST_LOG_TRIVIAL(trace) << "(I) -- inserting var: " << n;
		}
		return n;
	}

	std::set<nso<BAs...>>& free_vars;
};

template<typename... BAs>
auto get_vars_from_nso(const nso<BAs...>& n) {
	return select_top(n, is_var_or_capture<BAs...>);
}

template<typename... BAs>
auto get_free_vars_from_nso(const nso<BAs...>& n) {
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin get_free_vars_from_nso of " << n;
	std::set<nso<BAs...>> free_vars;
	free_vars_collector<BAs...> collector(free_vars);
	post_order_traverser<
			free_vars_collector<BAs...>,
			all_t<nso<BAs...>>,
			nso<BAs...>>(collector, all<nso<BAs...>>)(n);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- End get_free_vars_from_nso";
	return free_vars;
}

// Given a nso<BAs...> produce a number such that the variable x_i is
// neither a bool_variable nor a variable nor a capture
template<typename... BAs>
int_t get_new_var_id (const nso<BAs...> fm) {
	auto var_nodes = get_vars_from_nso<BAs...>(fm);
	set vars{1};
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


// Given a nso<BAs...> produce a number i such that the uninterpreted constant const_i is
// not present
template<typename... BAs>
int_t get_new_uniter_const_id (const nso<BAs...> fm) {
	auto uniter_consts = select_top(fm, is_non_terminal<tau_parser::uninterpreted_constant, BAs...>);
	set ids {0};
	for (auto uniter_const : uniter_consts) {
		if (auto tmp = make_string(tau_node_terminal_extractor<BAs...>, uniter_const); tmp.find(":const") != string::npos) {
			string id = tmp.substr(6, tmp.size()-1);
			if (!tmp.empty()) ids.insert(stoi(id));
		}
	}
	return *ids.rbegin() + 1;
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
	const sp_tau_node<BAs...>& ref)
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
std::optional<sp_tau_node<BAs...>> get_ref(const sp_tau_node<BAs...>& n) {
	auto ref = std::optional<sp_tau_node<BAs...>>(n);
	while (ref.has_value() && is_non_terminal_node<BAs...>(ref.value())
		&& (tau_parser::ref != (ref.value()
			| non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>)))
				ref = ref | only_child_extractor<BAs...>;
	if (!ref.has_value() || !is_non_terminal_node<BAs...>(ref.value())
		|| tau_parser::ref != (ref.value()
			| non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>))
				return std::optional<sp_tau_node<BAs...>>();
	return ref;
}

// Check that the Tau formula does not use Boolean combinations of models
template<typename ...BAs>
bool has_no_boolean_combs_of_models(const nso<BAs...>& fm) {
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

template <typename... BAs>
bool are_nso_equivalent(nso<BAs...> n1, nso<BAs...> n2) {
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

	nso<BAs...> wff = build_wff_equiv<BAs...>(n1, n2);
	auto vars = get_free_vars_from_nso(wff);
	for(auto& v: vars) wff = build_wff_all<BAs...>(v, wff);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- wff: " << wff;

	auto normalized = normalizer_step<BAs...>(wff);
	auto check = normalized | tau_parser::wff_t;
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: " << check.has_value();

	return check.has_value();
}

template <typename... BAs>
auto is_nso_equivalent_to_any_of(nso<BAs...>& n, std::vector<nso<BAs...>>& previous) {
	return std::any_of(previous.begin(), previous.end(), [n] (nso<BAs...>& p) {
		return are_nso_equivalent<BAs...>(n, p);
	});
}

template<typename... BAs>
bool are_bf_equal(nso<BAs...> n1, nso<BAs...> n2) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin are_bf_equal";
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n1 " << n1;
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n2 " << n2;

	assert(is_non_terminal(tau_parser::bf, n1));
	assert(is_non_terminal(tau_parser::bf, n2));

	if (n1 == n2) {
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_bf_equal: true (equal bf)";
		return true;
	}

	nso<BAs...> bf_equal_fm = build_wff_eq(build_bf_xor(n1, n2));

	auto vars = get_free_vars_from_nso(bf_equal_fm);
	for(auto& v: vars) bf_equal_fm = build_wff_all<BAs...>(v, bf_equal_fm);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- wff: " << bf_equal_fm;

	auto normalized = normalizer_step<BAs...>(bf_equal_fm);
	auto check = normalized | tau_parser::wff_t;
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_bf_equal: " << check.has_value();

	return check.has_value();
}

template <typename... BAs>
auto is_bf_same_to_any_of(nso<BAs...>& n, std::vector<nso<BAs...>>& previous) {
	return std::any_of(previous.begin(), previous.end(), [n] (nso<BAs...>& p) {
		return are_bf_equal<BAs...>(n, p);
	});
}

template <typename... BAs>
size_t get_max_loopback_in_rr(const nso<BAs...>& form) {
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
nso<BAs...> build_shift_from_shift(nso<BAs...> shift, size_t step) {
	auto num = shift | tau_parser::num | optional_value_extractor<nso<BAs...>>;
	auto offset = num | only_child_extractor<BAs...> | offset_extractor<BAs...> | optional_value_extractor<size_t>;
	if (step == offset) return shift | tau_parser::capture | optional_value_extractor<nso<BAs...>>;
	std::map<nso<BAs...>, nso<BAs...>> changes{{num, build_num<BAs...>(step - offset)}};
	return replace<nso<BAs...>>(shift, changes);
}

template<typename... BAs>
nso<BAs...> build_main_step(const nso<BAs...>& form, size_t step) {
	std::map<nso<BAs...>, nso<BAs...>> changes;
	for (const auto& offset : select_top(form,
				is_non_terminal<tau_parser::offsets, BAs...>))
	{
		auto shift = offset | tau_parser::shift;
		if (!shift.has_value()) continue;
		auto nshift = build_shift_from_shift<BAs...>(shift.value(), step);
		changes[shift.value()] = nshift;
	}
	return replace<nso<BAs...>>(form, changes);
}

// Normalizes a Boolean function having no recurrence relation
template<typename... BAs>
nso<BAs...> bf_normalizer_without_rec_relation (const nso<BAs...>& bf) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin Boolean function normalizer";

	auto result = bf_boole_normal_form(bf);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End Boolean function normalizer";

	return result;
}

// Normalizes a Boolean function in which recurrence relations are present
template<typename... BAs>
nso<BAs...> bf_normalizer_with_rec_relation(const rr<nso<BAs...>> &bf) {
	BOOST_LOG_TRIVIAL(debug)<< "(I) -- Begin calculate recurrence relation";

	auto bf_unfolded = bf.main | repeat_all<step<BAs...>, BAs...>(
						step<BAs...>(bf.rec_relations));

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End calculate recurrence relation";

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin Boolean function normalizer";

	auto result = bf_boole_normal_form(bf_unfolded);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End Boolean function normalizer";

	return result;
}

// enumerates index in main with step i - used for finding a fixed point
template <typename... BAs>
nso<BAs...> build_enumerated_main_step(const nso<BAs...>& form, size_t i,
	size_t offset_arity)
{
	auto r = form;
	std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes;
	std::vector<sp_tau_node<BAs...>> ofs; // create offsets node
	ofs.push_back(wrap<BAs...>(tau_parser::offset, build_num<BAs...>(i)));
	for (size_t o = 1; o < offset_arity; ++o)
		ofs.push_back(wrap<BAs...>(tau_parser::offset,
							build_num<BAs...>(0)));

	// create enumerated replacement
	auto ref = r | only_child_extractor<BAs...>
		| tau_parser::ref
		| optional_value_extractor<sp_tau_node<BAs...>>;
	changes[ref] = make_node<tau_sym<BAs...>>(ref->value, { ref->child[0],
		wrap<BAs...>(tau_parser::offsets, ofs), ref->child[1] });
	r = replace(r, changes);
	BOOST_LOG_TRIVIAL(debug) << "(F*) " << r;
	//DBG(ptree<BAs...>(std::cout << "enumerated main ", r) << "\n";)
	return build_main_step(r, i);
}

template <typename... BAs>
bool is_valid(const rr<nso<BAs...>>& nso_rr) {
	for (const auto& main_offsets : select_all(nso_rr.main,
		is_non_terminal<tau_parser::offsets, BAs...>))
			if (find_top(main_offsets,
				is_non_terminal<tau_parser::capture, BAs...>))
	{
		BOOST_LOG_TRIVIAL(error) << "Main " << nso_rr.main
					<< " cannot contain a relative offset "
					<< main_offsets;
		return false; // capture in main's offset
	}
	for (size_t ri = 0; ri != nso_rr.rec_relations.size(); ++ri) {
		const auto& r = nso_rr.rec_relations[ri];
		auto left = get_ref_info(get_ref(r.first).value());
		for (const auto& [ot, _] : left.second)
			if (ot == tau_parser::shift) {
				BOOST_LOG_TRIVIAL(error) << "Recurrence "
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
						<< "Recurrence relation "
						<< r.first << " (having a fixed"
						" offset) cannot depend on a "
						"relative offset " << r.second;
					return false; // left num right capture
				}
				if (bo.first == tau_parser::num
					&& ho.second < bo.second) {
						BOOST_LOG_TRIVIAL(error)
							<<"Recurrence relation "
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
bool is_well_founded(const rr<nso<BAs...>>& nso_rr) {
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
		BOOST_LOG_TRIVIAL(error) << "Recurrence relation has no rules "
						"other than initial conditions";
		return false;
	}
	for (const auto& [left, _] : graph)
		if (!visited[left] && is_cyclic(left)) {
			BOOST_LOG_TRIVIAL(error)
					<< "Recurrence relation is cyclic";
			return false;
		}
	BOOST_LOG_TRIVIAL(debug)<< "(I) -- Recurrence relation is well founded";
	return true;
}

template <typename... BAs>
nso<BAs...> calculate_fixed_point(const rr<nso<BAs...>>& nso_rr,
	const nso<BAs...>& form, tau_parser::nonterminal t, size_t offset_arity)
{
	static auto fallback = t == tau_parser::wff ? _F<BAs...> : _0<BAs...>;

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Calculating fixed point: " << form;
	BOOST_LOG_TRIVIAL(debug) << "(F) " << nso_rr;
	//ptree<BAs...>(std::cout << "form: ", form) << "\n";

	if (!is_well_founded(nso_rr)) return fallback;

	std::vector<nso<BAs...>> previous;
	nso<BAs...> current;
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
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;

		if (previous.size()
			&& are_nso_equivalent<BAs...>(current, previous.back()))
		{
			BOOST_LOG_TRIVIAL(debug) << eos
				<< " - fixed point found at step: " << i;
			BOOST_LOG_TRIVIAL(debug) << "(F) " << previous.back();
			return previous.back();
		}
		else if (previous.size() > 1
			&& is_nso_equivalent_to_any_of(current, previous))
		{
			BOOST_LOG_TRIVIAL(debug) << eos
				<< " - loop (no fixed point) "
					"detected at step: " << i;
			return fallback;
		}
		BOOST_LOG_TRIVIAL(debug) << eos
			<< " - no fixed point resolution at step: "
			<< i << " incrementing";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;
		previous.push_back(current);
	}
	return fallback;
}

// calculate fixed points called from main and replace them by their results
template<typename... BAs>
struct fixed_point_transformer {

	fixed_point_transformer(const rr<nso<BAs...>>& defs,
		const rr_types& types) : defs(defs), types(types) {}

	sp_tau_node<BAs...> operator()(const sp_tau_node<BAs...>& n) {
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
		if (offset_arity) return changes.emplace(n,
			calculate_fixed_point(defs, n, type, offset_arity))
				.first->second;
		bool changed = false;
		std::vector<sp_tau_node<BAs...>> child;
		if (changes.contains(ref))
			changed = true, child.push_back(changes[ref]);
		else child.push_back(ref);
		auto nn = make_node<tau_sym<BAs...>>(n->value, child);
		if (changed) changes[n] = nn;
		return nn;
	}

	std::pair<tau_parser::nonterminal, size_t> get_type_info(
		const rr_types& ts, const sp_tau_node<BAs...>& fp_ref)
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

	std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes;
	rr<nso<BAs...>> defs;
	rr_types types;
};

// REVIEW (HIGH) review overall execution
template <typename... BAs>
nso<BAs...> normalizer(const rr<nso<BAs...>>& nso_rr) {
	// IDEA extract this to an operator| overload

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin normalizer";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << nso_rr;

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Apply once definitions to rules";
	rec_relations<nso<BAs...>> rrs;
	for (const auto& r : nso_rr.rec_relations) {
		auto [matcher, body] = r;
		rrs.emplace_back(matcher, apply_once_definitions(body));
	}
	rr<nso<BAs...>> defs{ rrs, nso_rr.main };
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Applied once definitions to rules";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << defs;

	// get types and do type checks
	rr_types types;
	bool success = true;
	get_rr_types(success, types, defs);
	if (!success || !is_valid(defs)) return _F<BAs...>;

	// transform fp calculation calls by calculation results
	fixed_point_transformer<BAs...> fpt(defs, types);
	defs.main = post_order_traverser<decltype(fpt), all_t<nso<BAs...>>,
		nso<BAs...>>(fpt, all<nso<BAs...>>)(defs.main);
	if (fpt.changes.size()) {
		defs.main = replace(defs.main, fpt.changes);
		BOOST_LOG_TRIVIAL(debug) << "(I) -- Calculated fixed points. "
						"New main: " << defs.main;
	}

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Apply once definitions to main";
	defs.main = apply_once_definitions(defs.main);
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Applied once definitions to main";
	if (defs.rec_relations.empty()) return normalizer_step(defs.main);
	BOOST_LOG_TRIVIAL(debug) << "(F) " << defs;

	std::vector<nso<BAs...>> previous;
	nso<BAs...> current;
	for (int i = get_max_loopback_in_rr(defs.main); ; i++) {
		current = build_main_step(defs.main, i)
			| repeat_all<step<BAs...>, BAs...>(
				step<BAs...>(defs.rec_relations));
		BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin normalizer step";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;
		current = normalizer_step(current);
		if (is_nso_equivalent_to_any_of(current, previous)) break;
		else previous.push_back(current);
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End normalizer step";
	}

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End normalizer";
	BOOST_LOG_TRIVIAL(debug) << "(O) " << current << "\n";
	return current;
}

template <typename... BAs>
nso<BAs...> normalizer(const nso<BAs...>& form) {
	rr<nso<BAs...>> nso_rr(form);
	return normalizer(nso_rr);
}

} // namespace idni::tau

#endif // __NORMALIZER2_H__
