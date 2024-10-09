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

#ifndef __REPL_EVALUATOR_TMPL_H__
#define __REPL_EVALUATOR_TMPL_H__

#include "repl_evaluator.h"
#include "normalizer.h"
#include "normal_forms.h"
#include "nso_rr.h"
#include "tau_ba.h"
#include "term_colors.h"
#include "solver.h"
#include "satisfiability.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <limits>

namespace idni::tau {

#define TC_STATUS        TC.BG_LIGHT_CYAN()
#define TC_STATUS_OUTPUT TC(color::GREEN, color::BG_LIGHT_CYAN, color::BRIGHT)
#define TC_ERROR         TC(color::RED,   color::BRIGHT)
#define TC_PROMPT        TC(color::WHITE, color::BRIGHT)
#define TC_OUTPUT        TC.GREEN()

template <typename... BAs>
void repl_evaluator<BAs...>::not_implemented_yet() {
	std::cout << "Not implemented yet.\n";
}

template<typename... BAs>
size_t digits(const sp_tau_node<tau_ba<BAs...>, BAs...>& n) {
	return process_digits(n)
		| only_child_extractor<tau_ba<BAs...>, BAs...>
		| size_t_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>;
}

template<typename... BAs>
std::optional<size_t> get_memory_index(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& n, const size_t size,
	bool silent = false)
{
	if (size == 0) {
		if (!silent) cout << "history is empty\n";
		return {};
	}
	auto mem_type = n
		| only_child_extractor<tau_ba<BAs...>, BAs...>
		| non_terminal_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>;
	auto is_relative = (mem_type == tau_parser::rel_memory);
	auto mem_id = n | mem_type | tau_parser::memory_id;
	size_t idx = mem_id ? digits(mem_id.value()) : 0;
	if ((is_relative && idx >= size)
		|| (!is_relative && (idx == 0 || idx > size)))
	{
		if (!silent) {
			cout << "history location " << TC_OUTPUT
				<< (is_relative ? "%-" : "%");
			if (!is_relative || idx) cout << idx;
			cout << TC.CLEAR() << " does not exist\n";
		}
		return {};
	}
	return { is_relative ? size - idx - 1 : idx - 1 };
}


template <typename... BAs>
repl_evaluator<BAs...>::memory_ref
	repl_evaluator<BAs...>::memory_retrieve(
		const sp_tau_node<tau_ba<BAs...>, BAs...>& n, bool silent)
{
	if (auto pos = get_memory_index(n, m.size(), silent); pos.has_value())
		return {{m[pos.value()], pos.value()}};
	cout << "error: history location does not exist\n";
	return {};
}

template<typename... BAs>
void print_memory(const nso<tau_ba<BAs...>, BAs...> mem, const size_t id,
	const size_t size, bool print_relative_index = true)
{
	cout << TC_OUTPUT << "%" << id + 1 << TC.CLEAR();
	if (print_relative_index) {
		cout << "/" << TC_OUTPUT;
		if (size - id == 1) cout << "%";
		else cout << "%-" << size - id - 1;
		cout << TC.CLEAR();
	}
	cout << ": " << mem << "\n";
}

template <typename... BAs>
void repl_evaluator<BAs...>::history_print_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& command)
{
	auto n = command | tau_parser::memory;
	if (!n) return;
	auto idx = get_memory_index(n.value(), m.size());
	if (idx) print_memory(m[idx.value()], idx.value(), m.size());
}

template <typename... BAs>
void repl_evaluator<BAs...>::history_list_cmd() {
	if (m.size() == 0) cout << "history is empty\n";
	else for (size_t i = 0; i < m.size(); i++)
		print_memory(m[i], i, m.size());
}

template <typename... BAs>
void repl_evaluator<BAs...>::memory_store(
	typename repl_evaluator<BAs...>::memory o)
{
	// do not add into memory if the last memory value is the same
	if (!(m.size() && m.back() == o))
		m.push_back(o);
	print_memory(m.back(), m.size() - 1, m.size(), false);
}

template <typename... BAs>
void repl_evaluator<BAs...>::history_store_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& command)
{
	memory_store(command->child[0]);
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::get_bf(
		const nso<tau_ba<BAs...>, BAs...>& n, bool suppress_error)
{
	if (is_non_terminal(tau_parser::bf, n))
		return std::optional(n);
	else if (is_non_terminal(tau_parser::memory, n)) {
		if (auto check = memory_retrieve(n); check) {
			auto [value, _] = check.value();
			if (is_non_terminal(tau_parser::bf, value))
				return std::optional(value);
			else {
				if (!suppress_error) cout
					<< "error: argument has wrong type\n";
				return {};
			}
		}
	}
	if (!suppress_error) cout << "error: argument has wrong type\n";
	return {};
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::get_wff(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (is_non_terminal(tau_parser::wff, n)) return std::optional(n);
	else if (is_non_terminal(tau_parser::memory, n)) {
		if (auto check = memory_retrieve(n); check) {
			auto [value, _] = check.value();
			if (is_non_terminal(tau_parser::wff, value))
				return optional(value);
			else {
				cout << "error: argument has wrong type\n";
				return {};
			}
		}
	}
	cout << "error: argument has wrong type\n";
	return {};
}

template <typename... BAs>
bool repl_evaluator<BAs...>::contains(
	const nso<tau_ba<BAs...>, BAs...>& n, tau_parser::nonterminal nt)
{
	auto pred = [nt](const auto& n) {
		return is_non_terminal<tau_ba<BAs...>, BAs...>(nt, n); };
	return find_top<decltype(pred), nso<tau_ba<BAs...>, BAs...>>(n, pred)
								.has_value();
}

template <typename... BAs>
std::optional<size_t> repl_evaluator<BAs...>::get_type(
	const nso<tau_ba<BAs...>, BAs...>& n)
{
	return n | non_terminal_extractor<tau_ba<BAs...>, BAs...>;
}

template <typename... BAs>
std::optional<std::pair<size_t, nso<tau_ba<BAs...>, BAs...>>>
	repl_evaluator<BAs...>::get_type_and_arg(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto type = n
		| non_terminal_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>;
	switch (type) {
	case tau_parser::memory:
		if (auto check = memory_retrieve(n); check) {
			auto [value, _] = check.value();
			auto mem_type = value
				| non_terminal_extractor<tau_ba<BAs...>, BAs...>
				| optional_value_extractor<size_t>;
			return {{mem_type, value}};
		} else return {};
	default: return {{type, n}};
	}
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::onf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto arg = n->child[1];
	auto var = n->child[2];
	if (auto check = get_type_and_arg(arg); check) {
		auto [_, value] = check.value();
		return onf<tau_ba<BAs...>, BAs...>(var, value);
	}
	return {};
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::dnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto arg = n->child[1];
	if (auto check = get_type_and_arg(arg); check) {
		auto [type, value] = check.value();
		switch (type) {
		case tau_parser::wff:
			return dnf_wff<tau_ba<BAs...>, BAs...>(value);
		case tau_parser::bf:
			return dnf_bf<tau_ba<BAs...>, BAs...>(value);
		default:
			cout << "error: invalid argument\n";
		}
	}
	return {};
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::cnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto arg = n->child[1];
	if (auto check = get_type_and_arg(arg); check) {
		auto [type, value] = check.value();
		switch (type) {
		case tau_parser::wff:
			return cnf_wff<tau_ba<BAs...>, BAs...>(value);
		case tau_parser::bf:
			return cnf_bf<tau_ba<BAs...>, BAs...>(value);
		default:
			cout << "error: invalid argument\n";
		}
	}
	return {};
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::nnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto arg = n->child[1];
	if (auto check = get_type_and_arg(arg); check) {
		auto [type, value] = check.value();
		switch (type) {
		case tau_parser::wff:
			return nnf_wff<tau_ba<BAs...>, BAs...>(value);
		case tau_parser::bf:
			return nnf_bf<tau_ba<BAs...>, BAs...>(value);
		}
	}
	return {};
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::mnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto arg = n->child[1];
	if (auto check = get_type_and_arg(arg); check) {
		auto [type, value] = check.value();
		switch (type) {
		case tau_parser::wff:
			return mnf_wff<tau_ba<BAs...>, BAs...>(value);
		case tau_parser::bf:
			return mnf_bf<tau_ba<BAs...>, BAs...>(value);
		}
	}
	return {};
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::snf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto arg = n->child[1];
	if (auto check = get_type_and_arg(arg); check) {
		auto [type, value] = check.value();
		switch (type) {
		case tau_parser::wff:
			return snf_wff<tau_ba<BAs...>, BAs...>(value);
		case tau_parser::bf:
			return snf_bf<tau_ba<BAs...>, BAs...>(value);
		}
	}
	return {};
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::bf_substitute_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto in = get_bf(n->child[1]);
	auto thiz = get_bf(n->child[2]);
	auto with = get_bf(n->child[3]);
	// Check for correct argument types
	if (!thiz || !in || !with) {
		cout << "error: invalid argument\n"; return {};
	}
	std::map<nso<tau_ba<BAs...>, BAs...>, nso<tau_ba<BAs...>, BAs...>>
		changes = {{thiz.value(), with.value()}};

	return replace(in.value(), changes);
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::substitute_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	// Since the memory command cannot be type-checked we do it here
	// First try to get bf
	auto in = get_bf(n->child[1], true);
	if (in) return bf_substitute_cmd(n);
	// First argument was not a bf so it must be a wff
	in = get_wff(n->child[1]);
	// Now sort out the remaining argument types
	auto thiz = get_bf(n->child[2], true);
	optional<nso<tau_ba<BAs...>, BAs...>> with;
	if (thiz) with = get_bf(n->child[3]);
	else {
		thiz = get_wff(n->child[2]);
		with = get_wff(n->child[3]);
	}
	// Check for correct argument types
	if (!thiz || !in || !with){
		cout << "error: invalid argument\n"; return {};
	}
	std::map<nso<tau_ba<BAs...>, BAs...>, nso<tau_ba<BAs...>, BAs...>>
		changes = {{thiz.value(), with.value()}};

	set free_vars_thiz = get_free_vars_from_nso(thiz.value());
	set free_vars_with = get_free_vars_from_nso(with.value());
	vector<nso<tau_ba<BAs...>, BAs...>> var_stack = {};
	auto var_id = get_new_var_id(in.value());
	set<nso<tau_ba<BAs...>, BAs...>> marked_quants;

	// A variable should only be replaced if it is not quantified
	auto quantified_vars_skipper = [&](auto x) {
		if (is_quantifier<tau_ba<BAs...>, BAs...>(x)) {
			auto var = find_top(x,
				is_var_or_capture<tau_ba<BAs...>, BAs...>);
			if (var && free_vars_thiz.contains(var.value()))
				return false;
		}
		return true;
	};
	// If we encounter a variable in "with" that would be captured by a quantifier
	// the quantified variable needs to be changed
	auto quantified_var_adder = [&](auto x) {
		if (!quantified_vars_skipper(x))
			return false;
		if (is_quantifier<tau_ba<BAs...>, BAs...>(x)) {
			auto var = find_top(x,
				is_var_or_capture<tau_ba<BAs...>, BAs...>);
			if (var && free_vars_with.contains(var.value())) {
				DBG(assert(!(is_non_terminal<
					tau_ba<BAs...>, BAs...>(
						tau_parser::capture,
						var.value())));)
				marked_quants.insert(x);
				bool var_t = is_non_terminal<
					tau_ba<BAs...>, BAs...>(
						tau_parser::variable,
							var.value());
				ostringstream ss; ss << "x" << var_id; ++var_id;
				auto unused_var = var_t
					? build_bf_var<tau_ba<BAs...>, BAs...>(
								ss.str())
					: build_wff_var<tau_ba<BAs...>, BAs...>(
								ss.str());
				// Case where variable is captured by two or more quantifiers
				if (changes.contains(var.value())) {
					var_stack.emplace_back(var.value());
					var_stack.emplace_back(
							changes[var.value()]);
					changes[var.value()] = unused_var;
				} else {
					changes.emplace(var.value(),unused_var);
					var_stack.emplace_back(var.value());
				}
			}
		}
		return true;
	};
	// After a quantifier is encountered on the way up in post_order_traverser
	// it needs to be removed from changes
	auto scoped_replace = [&](auto x, auto& c) {
		nso<tau_ba<BAs...>, BAs...> res;
		if (auto iter = changes.find(x); iter != changes.end())
			res = iter->second;
		else if (x->child == c) res = x;
		else res = make_node(x->value, move(c));

		if (marked_quants.contains(x)) {
			assert(!var_stack.empty());
			if (auto iter = changes.find(var_stack.back());
				iter != changes.end())
			{
				var_stack.pop_back();
				changes.erase(iter);
			} else {
				assert(var_stack.size() >= 2);
				changes[var_stack.end()[-2]] = var_stack.back();
				var_stack.pop_back();
				var_stack.pop_back();
			}
		}
		return res;
	};
	return post_order_recursive_traverser<nso<tau_ba<BAs...>, BAs...>>()(
			in.value(), quantified_var_adder, scoped_replace);
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::instantiate_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto var_type = static_cast<tau_parser::nonterminal>(n->child[2]
		| non_terminal_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>)
			 == tau_parser::variable ? tau_parser::bf
			 			: tau_parser::wff;
	auto nn = make_node(n->value, {	n->child[0], n->child[1],
				wrap(var_type, n->child[2]), n->child[3] });
	return substitute_cmd(nn);
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::normalize_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto arg = n->child[1];
	if (auto check = get_type_and_arg(arg); check) {
		auto [type, value] = check.value();
		bool contains_ref = contains(value, tau_parser::ref);
		rr<nso<tau_ba<BAs...>, BAs...>> rr_ =
			(contains_ref && type == tau_parser::rr)
				? make_nso_rr_from_binded_code<
						tau_ba<BAs...>, BAs...>(value)
				: rr<nso<tau_ba<BAs...>, BAs...>>(value);
		if (contains_ref)
			rr_.rec_relations.insert(rr_.rec_relations.end(),
				definitions.begin(), definitions.end()),
			rr_ = infer_ref_types<tau_ba<BAs...>,BAs...>(rr_);
		if (!is_non_terminal(tau_parser::bf, rr_.main))
			return normalizer<tau_ba<BAs...>, BAs...>(rr_);
		if (contains_ref) return bf_normalizer_with_rec_relation(rr_);
		return bf_normalizer_without_rec_relation(rr_.main);
	}
	cout << "error: invalid argument\n";
	return {};
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::qelim_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto arg = get_wff(n->child[1]);
	if (arg) return eliminate_quantifiers<tau_ba<BAs...>, BAs...>(arg.value())
		| repeat_all<step<tau_ba<BAs...>, BAs...>,
			tau_ba<BAs...>, BAs...>(
				to_dnf_wff<tau_ba<BAs...>, BAs...>
				| simplify_wff<tau_ba<BAs...>, BAs...>
				| simplify_wff<tau_ba<BAs...>, BAs...>)
		| reduce_bf<tau_ba<BAs...>, BAs...>
		| reduce_wff<tau_ba<BAs...>, BAs...>;
	cout << "error: invalid argument\n";
	return {};
}

template <typename... BAs>
void repl_evaluator<BAs...>::run_cmd(
	const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto form = n->child[1];
	if (auto check = form | tau_parser::wff; check) {
		// TODO (HIGH) call executor
	} else if (auto check = form | tau_parser::rr; check) {
		// TODO (HIGH) call executor
	}
	not_implemented_yet();
}

template <typename... BAs>
void repl_evaluator<BAs...>::solve_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n) {
	auto type = is_non_terminal<tau_parser::type, tau_ba<BAs...>, BAs...>(n->child[1])
		?  make_string<tau_node_terminal_extractor_t<tau_ba<BAs...>, BAs...>,
				nso<tau_ba<BAs...>, BAs...>>(
			tau_node_terminal_extractor<tau_ba<BAs...>, BAs...>, n)
		: "bdd"; // only tau makes always sense
	auto nn = is_non_terminal<tau_parser::type, tau_ba<BAs...>, BAs...>(n->child[1])
		? get_wff(n->child[2])
		: get_wff(n->child[1]);
	if (!nn) { cout << "error: invalid argument\n"; return; }
	auto s = solve<tau_ba<BAs...>, BAs...>(nn.value(), type);
	if (!s) { cout << "no solution\n"; return; }
	std::cout << "solution: {" << "\n";
	for (auto& [k, v] : s.value()) std::cout << "\t" << k << " <- " << v << "\n";
	std::cout << "}\n";
}

template<typename... BAs>
void repl_evaluator<BAs...>::is_satisfiable_cmd(
	const nso<tau_ba<BAs...>, BAs...>&)
{
	// TODO (HIGH) call satisfiability
	not_implemented_yet();
}

template<typename... BAs>
void repl_evaluator<BAs...>::is_valid_cmd(
	const nso<tau_ba<BAs...>, BAs...>&)
{
	// TODO (HIGH) call satisfiability
	not_implemented_yet();
}

template<typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>> repl_evaluator<BAs...>::sat_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n) {
	if (find_top(n, is_non_terminal<tau_parser::wff_sometimes, tau_ba<BAs...>, BAs...>)) {
		cout << "Currently only support for always quantification.";
		return {};
	}
	auto arg = n->child[1];
	if (auto check = get_type_and_arg(arg); check) {
		auto [type, value] = check.value();
		bool contains_ref = contains(value, tau_parser::ref);
		rr<nso<tau_ba<BAs...>, BAs...>> rr_ =
			(contains_ref && type == tau_parser::rr)
				? make_nso_rr_from_binded_code<
						tau_ba<BAs...>, BAs...>(value)
				: rr<nso<tau_ba<BAs...>, BAs...>>(value);
		if (contains_ref)
			rr_.rec_relations.insert(rr_.rec_relations.end(),
				definitions.begin(), definitions.end()),
			rr_ = infer_ref_types<tau_ba<BAs...>,BAs...>(rr_);
		if (is_non_terminal(tau_parser::bf, rr_.main)) {
			cout << "error: invalid argument";
			return {};
		}
		auto normalized_fm = normalizer<tau_ba<BAs...>, BAs...>(rr_);
		if (has_no_boolean_combs_of_models(normalized_fm))
			return always_to_unbounded_continuation(normalized_fm);
		// Get each clause if there are several always disjuncts
		auto clauses = get_leaves(normalized_fm, tau_parser::wff_or, tau_parser::wff);
		nso<tau_ba<BAs...>,BAs...> res;
		// Convert each disjunct to unbounded continuation
		for (auto& clause : clauses) {

			if (res) res = build_wff_or(res, build_wff_always(always_to_unbounded_continuation(clause)));
			else res = build_wff_always(always_to_unbounded_continuation(clause));
		}
		return normalizer_step(res);
	}
	cout << "error: invalid argument\n";
	return {};
}

template<typename... BAs>
void repl_evaluator<BAs...>::is_unsatisfiable_cmd(
	const nso<tau_ba<BAs...>, BAs...>&)
{
	// TODO (HIGH) call satisfiability
	not_implemented_yet();
}

template <typename... BAs>
void repl_evaluator<BAs...>::def_rr_cmd(
	const nso<tau_ba<BAs...>, BAs...>& n)
{
	definitions.emplace_back(n->child[0]->child[0], n->child[0]->child[1]);
	cout << "[" << definitions.size() << "] " << definitions.back() << "\n";
}

template <typename... BAs>
void repl_evaluator<BAs...>::def_list_cmd() {
	if (definitions.size() == 0) cout << "definitions are empty\n";
	for (size_t i = 0; i < definitions.size(); i++)
		cout << "    [" << i + 1 << "] " << definitions[i] << "\n";
	if (inputs.size() == 0 && outputs.size() == 0) cout << "i/o variables: empty\n";
	else std::cout << "i/o variables:\n";
	for (auto& [d, f]: inputs)
		cout << "    " << d.second << " " << d.first << " = ifile(\"" << f << "\")\n";
	for (auto& [d, f]: outputs)
		cout << "    " << d.second << " " << d.first << " = ofile(\"" << f << "\")\n";
}

template <typename... BAs>
void repl_evaluator<BAs...>::def_print_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& command)
{
	if (definitions.size() == 0) cout << "rec. relations: empty\n";
	auto num = command | tau_parser::number;
	if (!num) return;
	auto i = digits(num.value());
	if (i && i <= definitions.size()) {
		cout << definitions[i-1] << "\n";
		return;
	}
	cout << "error: definition [" << i << "] does not exist\n";
	return;
}

template <typename... BAs>
void repl_evaluator< BAs...>::def_input_cmd(
		const sp_tau_node<tau_ba<BAs...>, BAs...>& command) {
	filename fn;
	type t = command | tau_parser::type | extract_string<tau_ba<BAs...>, BAs...>;
	if (auto file_name = command | tau_parser::input_stream
			| tau_parser::q_file_name | extract_string<tau_ba<BAs...>, BAs...>; !file_name.empty())
		fn = file_name;
	else fn = "/dev/stdin";
	auto var_name = command | tau_parser::charvar | optional_value_extractor<sp_tau_node<tau_ba<BAs...>, BAs...>>;
	inputs[{var_name, t}] = fn;
}

template <typename... BAs>
void repl_evaluator< BAs...>::def_output_cmd(
		const sp_tau_node<tau_ba<BAs...>, BAs...>& command) {
	filename fn;
	type t = command | tau_parser::type | extract_string<tau_ba<BAs...>, BAs...>;
	if (auto file_name = command | tau_parser::output_stream
			| tau_parser::q_file_name | extract_string<tau_ba<BAs...>, BAs...>; !file_name.empty())
		fn = file_name;
	else fn = "/dev/stdout";
	auto var_name = command | tau_parser::charvar | optional_value_extractor<sp_tau_node<tau_ba<BAs...>, BAs...>>;;
	outputs[{var_name, t}] = fn;
}

// make a nso_rr from the given tau source and binder.
template <typename... BAs>
sp_tau_node<tau_ba<BAs...>, BAs...>
	repl_evaluator<BAs...>::make_cli(
		const std::string& src)
{
	// remove ascii char 22 if exists in the input
	std::string filt = src;
	filt.erase(remove_if(filt.begin(), filt.end(), [](unsigned char c) {
		return c == 22;
	}), filt.end());
	auto cli_src = make_tau_source(filt, {
		.start = tau_parser::cli,
		//.debug = opt.debug_repl
	});
	if (!cli_src) // flush! new line and return null if invalid source
		return (std::cout << std::endl), error = true, nullptr;
	auto cli_code = make_tau_code<tau_ba<BAs...>, BAs...>(cli_src);
	return bind_tau_code_using_factory<tau_ba<BAs...>, BAs...>(cli_code);
}

template <typename... BAs>
void repl_evaluator<BAs...>::get_cmd(
	sp_tau_node<tau_ba<BAs...>, BAs...> n)
{
	static std::string pbool[] = { "off", "on" };
	static std::map<size_t,	std::function<void()>> printers = {
#ifdef DEBUG
	{ tau_parser::debug_repl_opt, [this]() {
		cout << "debug-repl:  " << pbool[opt.debug_repl] << "\n"; } },
#endif
	{ tau_parser::status_opt,   [this]() {
		cout << "status:      " << pbool[opt.status] << "\n"; } },
	{ tau_parser::colors_opt,   [this]() {
		cout << "colors:      " << pbool[opt.colors] << "\n"; } },
	{ tau_parser::severity_opt, [this]() {
		cout << "severity:    " << opt.severity << "\n"; } }};
	auto option = n | tau_parser::bool_option;
	if (option.has_value()) option = n;
	else option = n | tau_parser::option;
	if (!option.has_value()) { for (auto& [_, v] : printers) v(); return; }
	else printers[get_opt(option.value())]();
}

template <typename... BAs>
boost::log::trivial::severity_level
	repl_evaluator<BAs...>::nt2severity(size_t nt) const
{
	switch (nt) {
		case tau_parser::error_sym: return boost::log::trivial::error;
		case tau_parser::debug_sym: return boost::log::trivial::debug;
		case tau_parser::trace_sym: return boost::log::trivial::trace;
		case tau_parser::info_sym:  return boost::log::trivial::info;
		default: std::cerr << "error: invalid severity value\n";
	}
	return boost::log::trivial::error;
}

template <typename... BAs>
size_t get_opt(sp_tau_node<BAs...> n) {
	sp_tau_node<BAs...> value;
	if (auto bool_opt = n | tau_parser::bool_option; bool_opt)
		value =  bool_opt.value();
	else if (auto enum_opt = n | tau_parser::enum_option; enum_opt)
		value =  enum_opt.value();
	return value | only_child_extractor<BAs...>
		| non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
}

template <typename... BAs>
void repl_evaluator<BAs...>::set_cmd(
	sp_tau_node<tau_ba<BAs...>, BAs...> n)
{
	using namespace boost::log;
	auto option = n | tau_parser::option;
	auto v  = n | tau_parser::option_value;
	auto vt = v | only_child_extractor<tau_ba<BAs...>, BAs...>
		| non_terminal_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>;
	auto get_bool_value = [&v, &vt](bool& val) {
		if      (vt == tau_parser::option_value_true) val = true;
		else if (vt == tau_parser::option_value_false) val = false;
		else cout << "error: invalid bool value\n";
		return val;
	};
	static std::map<size_t,	std::function<void()>> setters = {
#ifdef DEBUG
	{ tau_parser::debug_repl_opt, [&]() {
		get_bool_value(opt.debug_repl); } },
#endif
	{ tau_parser::status_opt,   [&]() {
		get_bool_value(opt.status); } },
	{ tau_parser::colors_opt,   [&]() {
		TC.set(get_bool_value(opt.colors)); } },
	{ tau_parser::severity_opt, [&]() {
		auto sev = v | tau_parser::severity;
		if (!sev.has_value()) {
			cout << "error: invalid severity value\n"; return; }
		opt.severity = nt2severity(sev
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| non_terminal_extractor<tau_ba<BAs...>, BAs...>
			| optional_value_extractor<size_t>);
		boost::log::core::get()->set_filter(
			boost::log::trivial::severity >= opt.severity);
		}
	}};
	setters[get_opt(option.value())]();
	get_cmd(n);
}

template <typename... BAs>
void repl_evaluator<BAs...>::toggle_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& n)
{
	auto toggle_type = n | tau_parser::bool_option
		| only_child_extractor<tau_ba<BAs...>, BAs...>
		| non_terminal_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>;
	switch (toggle_type) {
#ifdef DEBUG
	case tau_parser::debug_repl_opt: opt.debug_repl = !opt.debug_repl;break;
#endif
	case tau_parser::colors_opt:
		TC.set(opt.colors = !opt.colors); break;
	case tau_parser::status_opt: opt.status = !opt.status; break;
	default: cout << ": unknown bool option\n"; error = true;break;
	}
	get_cmd(n);
}

template <typename... BAs>
int repl_evaluator<BAs...>::eval_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& n)
{
	auto command = (n
		| only_child_extractor<tau_ba<BAs...>, BAs...>).value();
	auto command_type = command
		| non_terminal_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>;
#ifdef DEBUG
	if (opt.debug_repl) {
		std::cout << "command: " << command << "\n";
		print_sp_tau_node_tree<tau_ba<BAs...>, BAs...>(cout
			<< "tree: ", command) << "\n";
	}
#endif
	std::optional<sp_tau_node<tau_ba<BAs...>, BAs...>> result;
	using p = tau_parser;
	switch (command_type) {
	case p::quit_cmd:           return cout << "Quit.\n", 1;
	case p::clear_cmd:          if (r) r->clear(); break;
	case p::help_cmd:           help_cmd(command); break;
	case p::version_cmd:        version_cmd(); break;
	case p::get_cmd:            get_cmd(command); break;
	case p::set_cmd:            set_cmd(command); break;
	case p::toggle_cmd:         toggle_cmd(command); break;
	case p::history_list_cmd:   history_list_cmd(); break;
	case p::history_print_cmd:  history_print_cmd(command); break;
	case p::history_store_cmd:  history_store_cmd(command); break;
	// normalization
	case p::normalize_cmd:      result = normalize_cmd(command); break;
	// execution
	case p::run_cmd:            run_cmd(command); break;
	case p::solve_cmd:          solve_cmd(command); break;
	// substitution and instantiation
	case p::subst_cmd:          result = substitute_cmd(command); break;
	case p::inst_cmd:           result = instantiate_cmd(command); break;
	// formula checks
	case p::sat_cmd:            result = sat_cmd(command); break;
	case p::valid_cmd:          is_valid_cmd(command); break;
	case p::unsat_cmd:          is_unsatisfiable_cmd(command); break;
	// normal forms
	case p::onf_cmd:            result = onf_cmd(command); break;
	case p::dnf_cmd:            result = dnf_cmd(command); break;
	case p::cnf_cmd:            result = cnf_cmd(command); break;
	case p::anf_cmd:            not_implemented_yet(); break;
	case p::nnf_cmd:            result = nnf_cmd(command); break;
	case p::pnf_cmd:            not_implemented_yet(); break;
	case p::mnf_cmd:            result = mnf_cmd(command); break;
	case p::snf_cmd:            result = snf_cmd(command); break;
	// definition of rec relations to be included during normalization
	case p::def_rr_cmd:         def_rr_cmd(command); break;
	case p::def_list_cmd:       def_list_cmd(); break;
	case p::def_print_cmd:      def_print_cmd(command); break;
	// definitions of i/o streams
	case p::def_input_cmd:      def_input_cmd(command); break;
	case p::def_output_cmd:     def_output_cmd(command); break;
	// qelim
	case p::qelim_cmd:          result = qelim_cmd(command); break;
	case p::comment:            break;
	// error handling
	default: error = true, cout << "\nUnknown command\n"; break;
	}
#ifdef DEBUG
	if (opt.debug_repl && result) ptree<tau_ba<BAs...>, BAs...>(
		std::cout << "result tree: ", result.value()) << "\n";
#endif
	if (result) memory_store(result.value());
	return 0;
}

template <typename... BAs>
repl_evaluator<BAs...>::repl_evaluator(options opt): opt(opt)
{
	TC.set(opt.colors);
	boost::log::core::get()->set_filter(
		boost::log::trivial::severity >= opt.severity);
}

template <typename... BAs>
std::string repl_evaluator<BAs...>::prompt() {
	using namespace boost::log;
	std::stringstream ss;
	if (opt.status) {
		std::stringstream status;
		if (m.size()) status << " " << TC_STATUS_OUTPUT << "%"
			<< m.size() << TC.CLEAR() << TC_STATUS;
		if (opt.severity != trivial::error)
			status << " " << to_string(opt.severity);
		if (status.tellp()) ss << TC_STATUS << "["
			<< status.str() << " ]" << TC.CLEAR() << " ";
	}
	ss << (error ? TC_ERROR : TC_PROMPT) << "tau>" << TC.CLEAR() << " ";
	if (r) r->prompt(ss.str());
	return ss.str();
}

template <typename... BAs>
int repl_evaluator<BAs...>::eval(const std::string& src) {
	error = false;
	auto tau_spec = make_cli(src);
	int quit = 0;
	if (tau_spec) {
		auto commands = tau_spec || tau_parser::cli_command;
		for (const auto& cmd : commands)
			if (quit = eval_cmd(cmd); quit) break;
	}
	std::cout << std::endl;
	if (!quit) prompt();
	return quit;
}

template <typename... BAs>
void repl_evaluator<BAs...>::version_cmd() {
	cout << "Tau version: " << GIT_DESCRIBED << "\n";
}

template <typename... BAs>
void repl_evaluator<BAs...>::help_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& n)
{
	static const std::string bool_options =
#ifdef DEBUG
		"  debug-repl             show REPL commands on/off\n"
#endif
		"  status                 show status        on/off\n"
		"  colors                 use term colors    on/off\n";
	static const std::string all_available_options = std::string{} +
		"Available options:\n" + bool_options +
		"  severity               severity           error/info/debug/trace\n";
	static const std::string bool_available_options = std::string{} +
		"Available options:\n" + bool_options;
	auto arg = n | tau_parser::help_arg
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| non_terminal_extractor<tau_ba<BAs...>, BAs...>;
	size_t nt = arg ? arg.value()
			: static_cast<size_t>(tau_parser::help_sym);
	switch (nt) {
	case tau_parser::help_sym: cout
		<< "Commands:\n"
		<< "  help or h              print this help\n"
		<< "  quit, q, exit or e     exit the repl\n"
		<< "  version or v           print version\n"
		<< "  clear                  clear the screen\n"
		<< "\n"

		<< "Formula commands:\n"
		<< "  normalize or n         normalize formula\n"
		<< "  qelim                  innermost quantifier elimination\n"
		<< "\n"

		<< "Normal form commands:\n"
		<< "  onf                    convert to order normal form\n"
		<< "  dnf                    convert to disjunctive normal form\n"
		<< "  cnf                    convert to conjunctive normal form\n"
		//<< "  anf                    convert to algebraic normal form\n"
		<< "  nnf                    convert to negation normal form\n"
		//<< "  pnf                    convert to prenex normal form\n"
		<< "  mnf                    convert to minterm normal form\n"
		<< "\n"

		<< "Memory commands\n"
		<< "  memory                 manage stored formulas or obtained results\n"
		<< "\n"

		<< "Substitution and instantiation commands:\n"
		<< "  substitute (subst/s)   substitute a formula\n"
		<< "  instantiate (inst/i)   instantiate a variable in a formula\n"
		<< "\n"

		//<< "Validity commands:\n"
		//<< "  sat                   check satisfiability\n"
		//<< "  valid                 check validity\n"
		//<< "  unsat				    check unsatisfiability\n"
		//<< "\n"

		<< "Run command:\n"
		<< "  run                    run a program that meets a tau spec\n"
		<< "\n"

		<< "Solver commands:\n"
		<< "  solve                  solve a formula\n"
		<< "\n"

		<< "Definition commands:\n"
		<< "  def                    manage defined recurrence relations\n"
		<< "\n"

		//<< "Selection commands:\n"
		//<< "  selection or s       selection control\n"
		//<< "\n"

		<< "Settings commands:\n"
		<< "  get                    get options' values\n"
		<< "  set                    set option's value\n"
		<< "  toggle                 toggle option's value\n"
		<< "\n"

		<< "Examples:\n"
		<< "  help or h examples     show examples related to the Tau Language syntax.\n"
		<< "\n";
		break;
	case tau_parser::version_sym: cout
		<< "version or v prints out current Tau commit id\n";
		break;
	case tau_parser::quit_sym: cout
		<< "q, quit, e or exit exits the repl\n";
		break;
	case tau_parser::clear_sym: cout
		<< "clear clears the screen\n";
		break;
	case tau_parser::get_sym: cout
		<< "get if used alone prints all options and their values\n"
		<< "get <option>        prints the value of the given option\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau_parser::set_sym: cout
		<< "set <option> [=] <value> sets option value\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau_parser::toggle_sym: cout
		<< "toggle <option> toggles option value\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau_parser::history_sym: cout
		<< "history command manages stored or previous commands results\n"
		<< "\n"
		<< "  history or hist              lists all stored or output results\n"
		<< "  history or hist <memory_id>  prints the given stored or output result\n"
		<< "  <formula>                    store the given formula in the memory\n"
		<< "  %<memory_id>                 provides relative access to memory\n"
		<< "  &<memory_id>                 provides absolute access to memory\n";
		break;
	case tau_parser::normalize_sym: cout
		<< "normalize or n command normalizes a formula, prints it and\n"
		<< "saves it into memory of previous memoris\n"
		<< "\n"
		<< "usage:\n"
		<< "  normalize <NSORR>      normalizes the given NSO RR\n"
		<< "  normalize <WFF>        normalizes the given WFF formula\n"
		<< "  normalize <BF>         normalizes the given WFF formula\n"
		<< "  normalize <memory>     normalizes the memory with the given id\n";
	//	<< "  normalize <selection>  normalizes the selection\n";
		break;
	case tau_parser::qelim_sym: cout
		<< "qelim command eliminates the innermost quantifier, prints it and\n"
		<< "saves it into memory of previous memories\n"
		<< "\n"
		<< "usage:\n"
		<< "  qelim <WFF>            eliminates innermost quantifier in the given WFF\n";
		break;
	case tau_parser::run_sym: cout
		<< "Command r, run ...\n";
		break;
	case tau_parser::solve_sym: cout
		<< "Command s, solve ...\n";
		break;
	case tau_parser::sat_sym: cout
		<< "Command sat, is_satisfiable ...\n";
		break;
	case tau_parser::valid_sym: cout
		<< "Command valid, is_valid ...\n";
		break;
	case tau_parser::unsat_sym: cout
		<< "Command unsat, is_unsatisfiable ...\n";
		break;
	case tau_parser::dnf_sym: cout
		<< "dnf command converts a boolean formula or a well formed formula to disjunctive normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  dnf <BF>             converts the given BF to DNF\n"
		<< "  dnf <WFF>            converts the given BF to DNF\n"
		<< "  dnf <memory>         converts the memory with the given id to DNF\n";
		break;
	case tau_parser::cnf_sym: cout
		<< "cnf command converts a boolean formula or a well formed formula to conjunctive normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  cnf <BF>             converts the given BF to CNF\n"
		<< "  cnf <WFF>            converts the given BF to CNF\n"
		<< "  cnf <memory>         converts the memory with the given id to CNF\n";
		break;
	//case tau_parser::anf_sym: cout
	//	<< "anf command converts a boolean formula or a well formed formula to algebraic normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  anf <BF>             converts the given BF to ANF\n"
	//	<< "  anf <WFF>            converts the given WFF to ANF\n"
	//	<< "  anf <memory>         converts the memory with the given id to ANF\n";
	//	break;
	case tau_parser::nnf_sym: cout
		<< "nnf command converts a boolean formula or a well formed formula to negation normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  nnf <BF>             converts the given BF to NNF\n"
		<< "  nnf <WFF>            converts the given BF to NNF\n"
		<< "  nnf <memory>         converts the memory with the given id to NNF\n";
		break;
	//case tau_parser::pnf_sym: cout
	//	<< "pnf command converts a boolean formula or a well formed formula to prenex normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  pnf <BF>             converts the given BF to PNF\n"
	//	<< "  pnf <WFF>            converts the given BF to PNF\n"
	//	<< "  pnf <memory>           converts the memory with the given id to PNF\n";
	//	break;
	case tau_parser::mnf_sym: cout
		<< "mnf command converts a boolean formula or a well formed formula to minterm normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  mnf <BF>             converts the given BF to MNF\n"
		<< "  mnf <WFF>            converts the given BF to MNF\n"
		<< "  mnf <memory>         converts the memory with the given id to MNF\n";
		break;
	case tau_parser::onf_sym: cout
		<< "onf command converts a well formed formula to order normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  onf <VAR> <WFF>      converts the given WFF to ONF\n"
		<< "  onf <VAR> <memory>   converts the memory with the given id to ONF\n";
		break;
	case tau_parser::subst_sym: cout
		<< "substitute, subst or s substitutes a formula or a function\n"
		<< "\n"
		<< "usage:\n"
		<< "  substitute <input> [ <match> / <replace>]\n"
		<< "\n"
		<< "where:\n"
		<< "  <input> is BF or WFF or memory reference to BF or WFF\n"
		<< "  <match> is BF or WFF supposedly contained in <input>\n"
		<< "  <replace> is BF or WFF or memory reference to BF or WFF\n"
		<< "\n"
		<< "  if <input> is BF, <match> and <replace> must be BF\n"
		<< "  <match> and <replace> must be of the same type\n"
		<< "\n"
		<< "example:\n"
		<< "  substitute ?x && (?y || ?x) [ ?y || ?x / ?y ] # produces ?x && ?y\n"
		<< "\n";
		break;
	case tau_parser::inst_sym: cout
		<< "instantiate, inst or i instantiates a variable in a formula or in a function\n"
		<< "\n"
		<< "usage:\n"
		<< "  instantiate <input> [ <variable> / <value>]\n"
		<< "\n"
		<< "where:\n"
		<< "  <input> is BF or WFF or memory reference to BF or WFF\n"
		<< "  <variable> is BF or WFF variable contained in <input>\n"
		<< "  <value> is BF or WFF value or memory reference to BF or WFF\n"
		<< "\n"
		<< "  if <input> is BF, <variable> and <value> must be BF\n"
		<< "  <variable> and <value> must be of the same type\n"
		<< "\n"
		<< "example:\n"
		<< "  instantiate x | y & z [ z / 0 ]  # produces x | y & 0\n"
		<< "\n";
		break;
	case tau_parser::def_sym: cout
		<< "defines a rec. relation\n"
		<< "\n"
		<< "usage:\n"
		<< "  <WFF_RR>                     defines a wff rec. relation\n"
		<< "  <BF_RR>                      defines a bf rec. relation\n"
		<< "  defs or definitions          list definitions\n"
		<< "  defs or definitions <rr_id>  print rec relation with given id\n";
		break;
	case tau_parser::examples_sym: cout
		<< "examples\n";
		// TODO (HIGH) show examples
		break;
	}

}

// undef terminal color macros valid only for this file
#undef TC_STATUS
#undef TC_STATUS_OUTPUT
#undef TC_ERROR
#undef TC_PROMPT
#undef TC_OUTPUT

} // idni::tau namespace
#endif // __REPL_EVALUATOR_TMPL_H__
