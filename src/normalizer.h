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
	nso<BAs...> operator()(const nso<BAs...>& n) const {
		auto ex_quantifier = [](const auto& n) -> bool {
			return (n | tau_parser::wff | tau_parser::wff_ex).has_value();
		};
		if (auto wff = find_bottom(n, ex_quantifier); wff) {
			auto removed = wff.value()
				| repeat_all<step<BAs...>, BAs...>(
					to_dnf_wff<BAs...>
					| simplify_wff<BAs...>)
				| wff_remove_existential<BAs...>;
			std::map<nso<BAs...>, nso<BAs...>> changes{{wff.value(), removed}};
			return replace(n, changes);
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
	#ifdef CACHE
	static std::map<nso<BAs...>, nso<BAs...>> cache;
	if (auto it = cache.find(form); it != cache.end()) return it->second;
	#endif // CACHE
	auto result = form
		| repeat_all<step<BAs...>, BAs...>(step<BAs...>(apply_defs<BAs...>))
		| repeat_all<step<BAs...>, BAs...>(step<BAs...>(elim_for_all<BAs...>))
		| repeat_all<steps<step<BAs...>, BAs...>, BAs...>(steps<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| simplify_wff<BAs...>
			| wff_remove_existential<BAs...>))
		| bf_reduce_canonical<BAs...>()
		| repeat_once<step<BAs...>, BAs...>(elim_eqs<BAs...>)
		| sometimes_always_normalization<BAs...>();
	#ifdef CACHE
	cache[form] = result;
	#endif // CACHE
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
	set vars {1};
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
	while (ref.has_value()
		&& (tau_parser::ref != (ref.value()
			| non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>)))
				ref = ref | only_child_extractor<BAs...>;
	if (!ref.has_value() || tau_parser::ref != (ref.value()
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
	if (r1opt || r2opt) { // one is a ref
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
	for (const auto& offset: select_top(form, is_non_terminal<tau_parser::offsets, BAs...>)) {
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
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin calculate recurrence relation";

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
	ofs.push_back(make_node<tau_sym<BAs...>>(
		tau_parser::instance().literal(tau_parser::offset),
		{ build_num<BAs...>(i) }));
	for (size_t o = 1; o < offset_arity; ++o)
		ofs.push_back(make_node<tau_sym<BAs...>>(
			tau_parser::instance().literal(tau_parser::offset),
			{ 0 }));
	// create enumerated replacement
	auto ref = r | tau_parser::wff_ref | tau_parser::ref
		| optional_value_extractor<sp_tau_node<BAs...>>;
	changes[ref] = make_node<tau_sym<BAs...>>(ref->value, {
		ref->child[0],
		make_node<tau_sym<BAs...>>(tau_parser::instance()
			.literal(tau_parser::offsets), ofs),
		ref->child[1]
	});
	r = replace(r, changes);
	BOOST_LOG_TRIVIAL(debug) << "(F*) " << r;
	//DBG(ptree<BAs...>(std::cout << "enumerated main ", r) << "\n";)
	return build_main_step(r, i);
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
	for (size_t ri = 0; ri != nso_rr.rec_relations.size(); ++ri) {
		const auto& r = nso_rr.rec_relations[ri];
		auto left = get_ref_info(get_ref(r.first).value());
		for (const auto& [ot, _] : left.second)
			if (ot == tau_parser::shift) {
				BOOST_LOG_TRIVIAL(debug) << "(I) -- Recurrence relation " << r.first << " cannot contain an offset shift";
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
			if (ho == bo) graph[left.first].insert(right.first);
			else if (ho.first == tau_parser::num) {
				if (bo.first == tau_parser::capture) {
					BOOST_LOG_TRIVIAL(debug) << "(I) -- Recurrence relation " << r.first << " (having a fixed offset) cannot depend on a relative offset " << r.second;
					return false; // left num right capture
				}
				if (bo.first == tau_parser::num
					&& ho.second < bo.second) {
						BOOST_LOG_TRIVIAL(debug) << "(I) -- Recurrence relation " << r.first << " cannot depend on a future state " << r.second;
						return false; // l num < r num
				}
			}
		}
		visited[left.first]  = false;
		visiting[left.first] = false;
	}
	for (const auto& [left, _] : graph)
		if (!visited[left] && is_cyclic(left)) {
			BOOST_LOG_TRIVIAL(debug) << "(I) -- Recurrence relation is cyclic";
			return false;
		}
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Recurrence relation is well founded";
	return true;
}

template <typename... BAs>
nso<BAs...> find_fixed_point(const rr<nso<BAs...>>& nso_rr,
	size_t offset_arity)
{
	static auto fallback = _F<BAs...>;

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Finding fixed point";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << nso_rr.main;

	std::vector<nso<BAs...>> previous;
	nso<BAs...> current;
	auto eos = "(I) -- End enumeration step";

	std::vector<size_t> loopbacks;
	for (const auto& r : nso_rr.rec_relations)
		loopbacks.push_back(get_max_loopback_in_rr(r.second));
	for (size_t i = 0; ; i++) {
		current = build_enumerated_main_step<BAs...>(
						nso_rr.main, i, offset_arity);
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
		current = normalizer_step(current);
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;

		if (previous.size()
			&& are_nso_equivalent<BAs...>(current, previous.back()))
		{
			BOOST_LOG_TRIVIAL(debug) << eos
				<< " - fixed point found at step: " << i;
			return current;
		}
		else if (previous.size() > 1
			&& is_nso_equivalent_to_any_of(current, previous))
		{
			BOOST_LOG_TRIVIAL(debug) << eos
				<< " - loop (no fixed point) detected at step: "
				<< i;
			return fallback;
		}
		BOOST_LOG_TRIVIAL(debug) << eos
			<< " - no fixed point resolution at step: "
			<< i << " incrementing";
		previous.push_back(current);
	}
}

// REVIEW (HIGH) review overall execution
template <typename... BAs>
nso<BAs...> normalizer(const rr<nso<BAs...>>& nso_rr) {
	// IDEA extract this to an operator| overload

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin normalizer";
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Apply once definitions";

	auto applied_defs = apply_once_definitions(nso_rr);

	if (nso_rr.rec_relations.empty())
		return normalizer_step(applied_defs.main);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Applied once definitions";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << applied_defs;

	//DBG(ptree<BAs...>(std::cout << "main ", applied_defs.main) << "\n";)

	// if rr main is a fixed point search
	// if main has no offsets and there exists a rr with offsets
	auto ref = applied_defs.main | tau_parser::wff_ref | tau_parser::ref;
	if (ref && !(ref | tau_parser::offsets).has_value()) {
		rr_types types;
		get_rr_types(types, applied_defs);
		auto fn = get_ref_name(ref.value());
		auto it = types.find(fn);
		if (it != types.end()) {
			size_t offset_arity = it->second.second;
			if (offset_arity) {
				auto fp = _F<BAs...>;
				if (is_well_founded(applied_defs))
					fp = find_fixed_point<BAs...>(
						applied_defs, offset_arity);
				BOOST_LOG_TRIVIAL(debug)<<"(I) -- End normalizer";
				BOOST_LOG_TRIVIAL(debug)<<"(O) " << fp << "\n";
				return fp;
			}
		}
	}

	auto loopback = get_max_loopback_in_rr(applied_defs.main);

	std::vector<nso<BAs...>> previous;
	nso<BAs...> current;

	for (int i = loopback; ; i++) {
		current = build_main_step(applied_defs.main, i)
			| repeat_all<step<BAs...>, BAs...>(
				step<BAs...>(applied_defs.rec_relations));

		BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin normalizer step";
		BOOST_LOG_TRIVIAL(debug) << "(F) " << current;

		current = normalizer_step(current);
		if (is_nso_equivalent_to_any_of(current, previous)) break;
		else previous.push_back(current);

		BOOST_LOG_TRIVIAL(debug) << "(I) -- End normalizer step";
	}

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End normalizer";
	BOOST_LOG_TRIVIAL(debug) << "(O) " << current;

	return current;
}

template <typename... BAs>
nso<BAs...> normalizer(const nso<BAs...>& form) {
	rr<nso<BAs...>> nso_rr(form);
	return normalizer(nso_rr);
}

} // namespace idni::tau

#endif // __NORMALIZER2_H__
