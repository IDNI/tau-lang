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
		auto nn = n | repeat_all<step<BAs...>, BAs...>(
			to_dnf_wff<BAs...>
			| simplify_wff<BAs...>)
			| bf_positives_upwards<BAs...>
			| wff_squeeze_positives<BAs...>;
		return nn | wff_remove_existential<BAs...>;
	}
};

template<typename... BAs>
nso<BAs...> operator|(const nso<BAs...>& form, const remove_one_wff_existential<BAs...>& r) {
	return r(form);
}

// IDEA (HIGH) rewrite steps as a tuple to optimize the execution
template<typename ... BAs>
nso<BAs...> normalizer_step(const nso<BAs...>& form) {
	#ifndef DEBUG
	static std::map<nso<BAs...>, nso<BAs...>> cache;
	if (auto it = cache.find(form); it != cache.end()) return it->second;
	#endif // DEBUG
	auto result = form
		| repeat_all<step<BAs...>, BAs...>(
			step<BAs...>(apply_defs<BAs...>))
		| repeat_all<step<BAs...>, BAs...>(
			step<BAs...>(elim_for_all<BAs...>))
		| repeat_all<remove_one_wff_existential<BAs...>, BAs...>(
			remove_one_wff_existential<BAs...>())
		| bf_reduce_canonical<BAs...>()
		| repeat_once<step<BAs...>, BAs...>(elim_eqs<BAs...>)
		| sometimes_always_normalization<BAs...>();
	#ifndef DEBUG
	cache[form] = result;
	#endif // DEBUG
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

template <typename... BAs>
bool are_nso_equivalent(nso<BAs...> n1, nso<BAs...> n2) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin are_nso_equivalent";
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n1 " << n1;
	BOOST_LOG_TRIVIAL(trace) << "(I) -- n2 " << n2;

	if (n1 == n2) {
		BOOST_LOG_TRIVIAL(debug) << "(I) -- End are_nso_equivalent: true";
		return true;
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
nso<BAs...> build_num(size_t value) {
	return make_node<tau_sym<BAs...>>(tau_parser::instance()
		.literal(tau_parser::num), {
			make_node<tau_sym<BAs...>>(tau_sym<BAs...>(value), {})
		});
}

template<typename... BAs>
nso<BAs...> build_num_from_num(nso<BAs...> num, size_t value) {
	auto nts = std::get<tau_source_sym>(num->value).nts;
	auto digits = make_node<tau_sym<BAs...>>(tau_sym<BAs...>(value), {});
	return make_node<tau_sym<BAs...>>( tau_sym<BAs...>(tau_source_sym(tau_parser::num, nts)), {digits});
}

template<typename... BAs>
nso<BAs...> build_shift_from_shift(nso<BAs...> shift, size_t step) {
	auto num = shift | tau_parser::num | optional_value_extractor<nso<BAs...>>;
	auto offset = num | only_child_extractor<BAs...> | offset_extractor<BAs...> | optional_value_extractor<size_t>;
	if (step == offset) return shift | tau_parser::capture | optional_value_extractor<nso<BAs...>>;
	std::map<nso<BAs...>, nso<BAs...>> changes{{num, build_num_from_num<BAs...>(num, step - offset)}};
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

	auto bf_unfolded = bf.main | repeat_all<step<BAs...>, BAs...>(step<BAs...>(bf.rec_relations));

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End calculate recurrence relation";

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin Boolean function normalizer";

	auto result = bf_boole_normal_form(bf_unfolded);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- End Boolean function normalizer";

	return result;
}

// enumerates index in main with step i - used for finding a fixed point
template <typename... BAs>
nso<BAs...> build_enumerated_main_step(const nso<BAs...>& form, size_t i, size_t offset_arity) {
	auto r = form;
	std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes;
	std::vector<sp_tau_node<BAs...>> ofs;
	ofs.push_back(make_node<tau_sym<BAs...>>(
		tau_parser::instance().literal(tau_parser::offset),
		{ build_num<BAs...>(i) }));
	for (size_t o = 1; o < offset_arity; ++o)
		ofs.push_back(make_node<tau_sym<BAs...>>(
			tau_parser::instance().literal(tau_parser::offset),
			{ 0 }));

	auto ref = r | tau_parser::wff_ref | tau_parser::ref
		| optional_value_extractor<sp_tau_node<BAs...>>;

	changes[ref] = make_node<tau_sym<BAs...>>(ref->value,
		{
			ref->child[0],
			make_node<tau_sym<BAs...>>(
				tau_parser::instance()
					.literal(tau_parser::offsets),
				ofs
			),
			ref->child[1]
		}

	);
	r = replace(r, changes);
	BOOST_LOG_TRIVIAL(debug) << "(F*) " << r;
	//DBG(ptree<BAs...>(std::cout << "enumerated main ", r) << "\n";)
	return build_main_step(r, i);
}

template <typename... BAs>
nso<BAs...> find_fixed_point(const rr<nso<BAs...>>& nso_rr, size_t offset_arity) {

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Finding fixed point";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << nso_rr.main;

	// TODO check for dependence loop

	std::vector<nso<BAs...>> previous;
	nso<BAs...> current;

	auto eos = "(I) -- End enumeration step";
	for (int i = 0; ; i++) {
		current = build_enumerated_main_step<BAs...>(nso_rr.main, i, offset_arity)
			| repeat_all<step<BAs...>, BAs...>(step<BAs...>(
				nso_rr.rec_relations));

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
			return _F<BAs...>;
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
		auto names = get_rr_types(types, applied_defs);
		for (const auto& r : applied_defs.rec_relations) {
			// #ifdef DEBUG
			// ptree<BAs...>(std::cout << "rrel left: ", r.first) << "\n",
			// ptree<BAs...>(std::cout << "rrel right: ", r.second) << "\n",
			// #endif
			get_rr_types(types, r.first);
			get_rr_types(types, r.second);
		}
		auto fn = get_ref_name(ref.value());
		auto it = types.find(fn);
		if (it != types.end()) {
			size_t offset_arity = it->second.second;
			if (offset_arity > 0) {
				auto fp = find_fixed_point<BAs...>(applied_defs, offset_arity);
				BOOST_LOG_TRIVIAL(debug) << "(I) -- End normalizer";
				BOOST_LOG_TRIVIAL(debug) << "(O) " << fp << "\n";
				return fp;
			}
		}
	}

	auto loopback = get_max_loopback_in_rr(applied_defs.main);

	std::vector<nso<BAs...>> previous;
	nso<BAs...> current;

	for (int i = loopback; ; i++) {
		current = build_main_step(applied_defs.main, i)
			| repeat_all<step<BAs...>, BAs...>(step<BAs...>(applied_defs.rec_relations));

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
