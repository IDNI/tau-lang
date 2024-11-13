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

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <limits>

#include "repl_evaluator.h"
#include "normalizer.h"
#include "normal_forms.h"
#include "nso_rr.h"
#include "tau_ba.h"
#include "term_colors.h"
#include "solver.h"
#include "satisfiability.h"
#include "interpreter.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

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
		if (!silent) std::cout << "history is empty\n";
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
			std::cout << "history location " << TC_OUTPUT
				<< (is_relative ? "%-" : "%");
			if (!is_relative || idx) std::cout << idx;
			std::cout << TC.CLEAR() << " does not exist\n";
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
	std::cout << "error: history location does not exist\n";
	return {};
}

template<typename... BAs>
void print_memory(const nso<tau_ba<BAs...>, BAs...> mem, const size_t id,
	const size_t size, bool print_relative_index = true)
{
	std::cout << TC_OUTPUT << "%" << id + 1 << TC.CLEAR();
	if (print_relative_index) {
		std::cout << "/" << TC_OUTPUT;
		if (size - id == 1) std::cout << "%";
		else std::cout << "%-" << size - id - 1;
		std::cout << TC.CLEAR();
	}
	std::cout << ": " << mem << "\n";
}

template<typename... BAs>
gssotc<BAs...> repl_evaluator<BAs...>::apply_rr_to_rr_gssotc(const size_t type, const nso<tau_ba<BAs...>, BAs...>& program) {
	bool contains_ref = contains(program, tau_parser::ref);
	rr<nso<tau_ba<BAs...>, BAs...>> rr_ =
		(contains_ref && type == tau_parser::rr)
			? make_nso_rr_from_binded_code<tau_ba<BAs...>, BAs...>(program)
			: rr<nso<tau_ba<BAs...>, BAs...>>(program);
	if (contains_ref)
		rr_.rec_relations.insert(rr_.rec_relations.end(),
			definitions.begin(), definitions.end()),
		rr_ = infer_ref_types<tau_ba<BAs...>, BAs...>(rr_);
	rr_.main = rr_.main | repeat_all<step<tau_ba<BAs...>, BAs...>,
		tau_ba<BAs...>, BAs...>(step<tau_ba<BAs...>, BAs...>(rr_.rec_relations));
	return rr_.main;
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
	if (m.size() == 0) std::cout << "history is empty\n";
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
				if (!suppress_error) std::cout
					<< "error: argument has wrong type\n";
				return {};
			}
		}
	}
	if (!suppress_error) std::cout << "error: argument has wrong type\n";
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
				return std::optional(value);
			else {
				std::cout << "error: argument has wrong type\n";
				return {};
			}
		}
	}
	std::cout << "error: argument has wrong type\n";
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
		auto [type, program] = check.value();
		auto applied = apply_rr_to_rr_gssotc(type, program);
		return onf<tau_ba<BAs...>, BAs...>(var, applied);
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
		auto [type, program] = check.value();
		auto applied = apply_rr_to_rr_gssotc(type, program);
		switch (type) {
		case tau_parser::wff:
			return reduce2(to_dnf2(program), tau_parser::wff);
		case tau_parser::bf:
			return reduce2(to_dnf2(program, false), tau_parser::bf);
		default:
			std::cout << "error: invalid argument\n";
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
		auto [type, program] = check.value();
		auto applied = apply_rr_to_rr_gssotc(type, program);
		switch (type) {
		case tau_parser::wff:
			return reduce2(to_cnf2(applied), tau_parser::wff, true);
		case tau_parser::bf:
			return reduce2(to_cnf2(applied, false), tau_parser::bf, true);
		default:
			std::cout << "error: invalid argument\n";
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
		auto [type, program] = check.value();
		auto applied = apply_rr_to_rr_gssotc(type, program);
		switch (type) {
		case tau_parser::wff:
			return nnf_wff<tau_ba<BAs...>, BAs...>(applied);
		case tau_parser::bf:
			return nnf_bf<tau_ba<BAs...>, BAs...>(applied);
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
		auto [type, program] = check.value();
		auto applied = apply_rr_to_rr_gssotc(type, program);
		switch (type) {
		case tau_parser::wff:
			return mnf_wff<tau_ba<BAs...>, BAs...>(applied);
		case tau_parser::bf:
			return mnf_bf<tau_ba<BAs...>, BAs...>(applied);
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
		auto [type, program] = check.value();
		auto applied = apply_rr_to_rr_gssotc(type, program);
		switch (type) {
		case tau_parser::wff:
			return snf_wff<tau_ba<BAs...>, BAs...>(applied);
		case tau_parser::bf:
			return snf_bf<tau_ba<BAs...>, BAs...>(applied);
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
		std::cout << "error: invalid argument\n"; return {};
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
	std::optional<nso<tau_ba<BAs...>, BAs...>> with;
	if (thiz) with = get_bf(n->child[3]);
	else {
		thiz = get_wff(n->child[2]);
		with = get_wff(n->child[3]);
	}
	// Check for correct argument types
	if (!thiz || !in || !with){
		std::cout << "error: invalid argument\n"; return {};
	}
	std::map<nso<tau_ba<BAs...>, BAs...>, nso<tau_ba<BAs...>, BAs...>>
		changes = {{thiz.value(), with.value()}};

	std::set free_vars_thiz = get_free_vars_from_nso(thiz.value());
	std::set free_vars_with = get_free_vars_from_nso(with.value());
	std::vector<nso<tau_ba<BAs...>, BAs...>> var_stack = {};
	auto var_id = get_new_var_id(in.value());
	std::set<nso<tau_ba<BAs...>, BAs...>> marked_quants;

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
				std::ostringstream ss; ss << "x" << var_id; ++var_id;
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
		else res = make_node(x->value, c);

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
	return rewriter::post_order_recursive_traverser<
		nso<tau_ba<BAs...>, BAs...>>()(
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
	auto nn = rewriter::make_node(n->value, { n->child[0], n->child[1],
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
	std::cout << "error: invalid argument\n";
	return {};
}

template <typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<BAs...>::qelim_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto check = get_type_and_arg(n->child[1]); check) {
		auto [type, program] = check.value();
		auto applied = apply_rr_to_rr_gssotc(type, program);
		return eliminate_quantifiers<tau_ba<BAs...>, BAs...>(applied)
			| repeat_all<step<tau_ba<BAs...>, BAs...>,
				tau_ba<BAs...>, BAs...>(
					to_dnf_wff<tau_ba<BAs...>, BAs...>
					| simplify_wff<tau_ba<BAs...>, BAs...>)
			| reduce_bf<tau_ba<BAs...>, BAs...>
			| reduce_wff<tau_ba<BAs...>, BAs...>;
	}
	std::cout << "error: invalid argument\n";
	return {};
}

template <typename... BAs>
void repl_evaluator<BAs...>::run_cmd(
	const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto arg = n->child[1];
	if (auto check = find_top(arg, is_regular_or_temporal_quantifier<tau_ba<BAs...>, BAs...>); check) {
		std::cout << "not enabled yet\n";
		return;
	}

	// running the program
	if (auto check = get_type_and_arg(arg); check) {
		// TODO (HIGH) remove this step once we plug the computation of phi/chi infinity
		// as we would get a formula in dnf already. However, we would need to
		// kept the application of definitionsand call the computation of phi/chi infinity
		auto [t, program] = check.value();
		auto applied = apply_rr_to_rr_gssotc(t, program);

		#ifdef DEBUG
		std::cout << "applied: " << applied << "\n";
		#endif // DEBUG

		auto dnf = applied
			| repeat_each<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(
				to_dnf_wff<tau_ba<BAs...>, BAs...>
				| simplify_wff<tau_ba<BAs...>, BAs...>);

		// TODO (HIGH) only consider inputs/outputs present in the formula
		// select current input variables
		auto in_vars = select_all(dnf,
			is_non_terminal<tau_parser::in_var_name, tau_ba<BAs...>, BAs...>);
		if (!in_vars.empty() && inputs.empty()) {
			std::cout << "error: no input variables defined\n";
			return;
		}

		std::map<nso<tau_ba<BAs...>, BAs...>, std::pair<type, filename>> current_inputs;
		for (auto& var: in_vars) {
			if (auto it = inputs.find(var); it != inputs.end()) {
				current_inputs[var] = it->second;
			} else {
				std::cout << "error: input variable " << var << " not defined\n";
				return;
			}
		}

		auto ins = finputs<tau_ba<BAs...>, BAs...>(current_inputs);

		// select current output variables
		auto out_vars = select_all(dnf,
			is_non_terminal<tau_parser::out_var_name, tau_ba<BAs...>, BAs...>);
		if (out_vars.empty()) {
			std::cout << "error: no output variables, nothing to compute\n";
			return;
		}
		if (outputs.empty()) {
			std::cout << "error: no output variables defined\n";
			return;
		}

		// do we need to bound the computation?
		auto max_iter = std::numeric_limits<size_t>::max();
		if (in_vars.empty() && !out_vars.empty()) {
			// TODO (MEDIUM) maybe we should use an specialized method to get the number of steps
			std::cout << "no inputs vars defined, how many steps do you want to perform? ";
			term::enable_getline_mode();
			// read input
			std::string line;
			std::getline(std::cin, line);
			term::disable_getline_mode();
			try {
				max_iter = std::stoul(line);
				std::cout << line << "\n";
			} catch (std::exception& e) {
				std::cout << "error: invalid input\n";
				return;
			}
		}

		std::map<nso<tau_ba<BAs...>, BAs...>, std::pair<type, filename>> current_outputs;
		for (auto& var: out_vars) {
			if (auto it = outputs.find(var); it != outputs.end()) {
				current_outputs[var] = it->second;
			} else {
				std::cout << "error: output variable " << var << " not defined\n";
				return;
			}
		}

		auto outs = foutputs<tau_ba<BAs...>, BAs...>(current_outputs);
		run<finputs<tau_ba<BAs...>, BAs...>,
				foutputs<tau_ba<BAs...>, BAs...>,
				tau_ba<BAs...>, BAs...>(
			dnf, ins, outs, max_iter);
		return;
	}

	std::cout << "error: invalid argument\n";
}

template <typename... BAs>
void repl_evaluator<BAs...>::solve_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n) {
	auto type = is_non_terminal<tau_parser::type, tau_ba<BAs...>, BAs...>(n->child[1])
		?  make_string<tau_node_terminal_extractor_t<tau_ba<BAs...>, BAs...>,
				nso<tau_ba<BAs...>, BAs...>>(
			tau_node_terminal_extractor<tau_ba<BAs...>, BAs...>, n)
		: "sbf"; // only tau makes always sense
	if (auto nn = is_non_terminal<tau_parser::type, tau_ba<BAs...>, BAs...>(n->child[1])
			? get_type_and_arg(n->child[2]) : get_type_and_arg(n->child[1]); nn) {
		auto [t, program] = nn.value();
		auto applied = apply_rr_to_rr_gssotc(t, program);
		if (!nn) { std::cout << "error: invalid argument\n"; return; }
		auto s = solve<tau_ba<BAs...>, BAs...>(applied, type);
		if (!s) { std::cout << "no solution\n"; return; }
		std::cout << "solution: {" << "\n";
		for (auto& [k, v] : s.value()) std::cout << "\t" << k << " <- " << v << "\n";
		std::cout << "}\n";
	}
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
			std::cout << "error: invalid argument";
			return {};
		}
		auto normalized_fm = normalizer<tau_ba<BAs...>, BAs...>(rr_);
		if (has_no_boolean_combs_of_models(normalized_fm))
			return build_wff_always(
				always_to_unbounded_continuation( normalized_fm));
		// Get each clause if there are several always disjuncts
		auto clauses = get_leaves(normalized_fm, tau_parser::wff_or, tau_parser::wff);
		nso<tau_ba<BAs...>,BAs...> res;
		// Convert each disjunct to unbounded continuation
		for (auto& clause : clauses) {
			if (res) res = build_wff_or(res, build_wff_always(
				transform_to_execution(clause)));
			else res = build_wff_always(
				transform_to_execution(clause));
		}
		return res;
	}
	std::cout << "error: invalid argument\n";
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
	std::cout << "[" << definitions.size() << "] " << definitions.back() << "\n";
}

template <typename... BAs>
void repl_evaluator<BAs...>::def_list_cmd() {
	if (definitions.size() == 0) std::cout << "definitions are empty\n";
	for (size_t i = 0; i < definitions.size(); i++)
		std::cout << "    [" << i + 1 << "] " << definitions[i] << "\n";
	if (inputs.size() == 0 && outputs.size() == 0) std::cout << "i/o variables: empty\n";
	else std::cout << "i/o variables:\n";
	for (auto& [var, desc]: inputs) {
		auto file = desc.second.empty() ? "console" : "ifile(\"" + desc.second + "\")";
		std::cout << "\t" << desc.first << " " << var << " = " << file << "\n";
	}
	for (auto& [var, desc]: outputs) {
		auto file = desc.second.empty() ? "console" : "ofile(\"" + desc.second + "\")";
		std::cout << "\t" << desc.first << " " << var << " = " << file << "\n";
	}
}

template <typename... BAs>
void repl_evaluator<BAs...>::def_print_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& command)
{
	if (definitions.size() == 0) std::cout << "rec. relations: empty\n";
	auto num = command | tau_parser::number;
	if (!num) return;
	auto i = digits(num.value());
	if (i && i <= definitions.size()) {
		std::cout << definitions[i-1] << "\n";
		return;
	}
	std::cout << "error: definition [" << i << "] does not exist\n";
	return;
}

template <typename... BAs>
void repl_evaluator< BAs...>::def_input_cmd(
		const sp_tau_node<tau_ba<BAs...>, BAs...>& command) {
	filename fn;
	type type = command
		| tau_parser::type
		| extract_string<tau_ba<BAs...>, BAs...>;
	if (auto file_name = command
			| tau_parser::input_stream
			| tau_parser::q_file_name
			| extract_string<tau_ba<BAs...>, BAs...>; !file_name.empty())
		fn = file_name;
	else fn = ""; // default input (std::cin)

	for (auto& t: nso_factory<tau_ba<BAs...>, BAs...>::instance().types()) {
		if (type == t) {
			auto var_name = command
				| tau_parser::in_var_name
				| optional_value_extractor<sp_tau_node<tau_ba<BAs...>, BAs...>>;
			inputs[var_name] = {type, fn};
			return;
		}
	}
	std::cout << "error: invalid type " << type << "\n";
}

template <typename... BAs>
void repl_evaluator< BAs...>::def_output_cmd(
		const sp_tau_node<tau_ba<BAs...>, BAs...>& command) {
	filename fn;
	type type = command
		| tau_parser::type
		| extract_string<tau_ba<BAs...>, BAs...>;
	if (auto file_name = command
			| tau_parser::output_stream
			| tau_parser::q_file_name
			| extract_string<tau_ba<BAs...>, BAs...>; !file_name.empty())
		fn = file_name;
	else fn = ""; // default output (std::cout)

	for (auto& t: nso_factory<tau_ba<BAs...>, BAs...>::instance().types()) {
		if (type == t) {
			auto var_name = command
				| tau_parser::out_var_name
				| optional_value_extractor<sp_tau_node<tau_ba<BAs...>, BAs...>>;
			outputs[var_name] = {type, fn};
			return;
		}
	}
	std::cout << "error: invalid type " << type << "\n";
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
		std::cout << "debug-repl:  " << pbool[opt.debug_repl] << "\n"; } },
#endif
	{ tau_parser::status_opt,     [this]() {
		std::cout << "status:            " << pbool[opt.status] << "\n"; } },
	{ tau_parser::colors_opt,     [this]() {
		std::cout << "colors:            " << pbool[opt.colors] << "\n"; } },
	{ tau_parser::charvar_opt,     [this]() {
		std::cout << "charvar:           " << pbool[opt.charvar] << "\n"; } },
	{ tau_parser::hilighting_opt, [this]() {
		std::cout << "syntax hilighting: " << pbool[pretty_printer_hilighting] << "\n"; } },
	{ tau_parser::indenting_opt,  [this]() {
		std::cout << "indenting:         " << pbool[pretty_printer_indenting] << "\n"; } },
	{ tau_parser::severity_opt,   [this]() {
		std::cout << "severity:          " << opt.severity << "\n"; } }};
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
	return boost::log::trivial::info;
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
		else std::cout << "error: invalid bool value\n";
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
	{ tau_parser::charvar_opt,   [&]() {
		update_charvar(get_bool_value(opt.charvar)); } },
	{ tau_parser::hilighting_opt,   [&]() {
		get_bool_value(pretty_printer_hilighting); } },
	{ tau_parser::indenting_opt,   [&]() {
		get_bool_value(pretty_printer_indenting); } },
	{ tau_parser::severity_opt, [&]() {
		if (vt == tau_parser::option_value_true)
			opt.severity = boost::log::trivial::trace;
		else {
			auto sev = v | tau_parser::severity;
			if (!sev.has_value()) {	std::cout
				<< "error: invalid severity value\n"; return; }
			opt.severity = nt2severity(sev
				| only_child_extractor<tau_ba<BAs...>, BAs...>
				| non_terminal_extractor<tau_ba<BAs...>, BAs...>
				| optional_value_extractor<size_t>);
		}
		boost::log::core::get()->set_filter(
			boost::log::trivial::severity >= opt.severity);
	} } };
	setters[get_opt(option.value())]();
	get_cmd(n);
}

template <typename... BAs>
void repl_evaluator<BAs...>::update_bool_opt_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& n,
	const std::function<bool(bool&)>& update_fn)
{
	auto option_type = n | tau_parser::bool_option
		| only_child_extractor<tau_ba<BAs...>, BAs...>
		| non_terminal_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>;
	switch (option_type) {
#ifdef DEBUG
	case tau_parser::debug_repl_opt: update_fn(opt.debug_repl); break;
#endif
	case tau_parser::colors_opt: TC.set(update_fn(opt.colors)); break;
	case tau_parser::charvar_opt:
		update_charvar(update_fn(opt.charvar)); break;
	case tau_parser::hilighting_opt:
		update_fn(pretty_printer_hilighting); break;
	case tau_parser::indenting_opt:
		update_fn(pretty_printer_indenting); break;
	case tau_parser::status_opt: update_fn(opt.status); break;
	default: std::cout << ": unknown bool option\n"; error = true;break;
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
		print_sp_tau_node_tree<tau_ba<BAs...>, BAs...>(std::cout
			<< "tree: ", command) << "\n";
	}
#endif
	std::optional<sp_tau_node<tau_ba<BAs...>, BAs...>> result;
	using p = tau_parser;
	switch (command_type) {
	case p::quit_cmd:           return std::cout << "Quit.\n", 1;
	case p::clear_cmd:          if (r) r->clear(); break;
	case p::help_cmd:           help_cmd(command); break;
	case p::version_cmd:        version_cmd(); break;
	case p::get_cmd:            get_cmd(command); break;
	case p::set_cmd:            set_cmd(command); break;
	case p::enable_cmd:         update_bool_opt_cmd(command,
					[](bool& b){ return b = true; }); break;
	case p::disable_cmd:        update_bool_opt_cmd(command,
					[](bool& b){ return b = false; });break;
	case p::toggle_cmd:         update_bool_opt_cmd(command,
					[](bool& b){ return b = !b; }); break;
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
	default: error = true, std::cout << "\nUnknown command\n"; break;
	}
#ifdef DEBUG
	if (opt.debug_repl && result) ptree<tau_ba<BAs...>, BAs...>(
		std::cout << "result tree: ", result.value()) << "\n";
#endif
	if (result) memory_store(result.value());
	return 0;
}

template<typename... BAs>
bool repl_evaluator<BAs...>::update_charvar(bool value) {
	std::set<std::string> guards{
		(opt.charvar = value) ? "charvar" : "var" };
	tau_parser::instance().get_grammar().set_enabled_productions(guards);
	bdd_parser::instance().get_grammar().set_enabled_productions(guards);
	return value;
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
	using namespace idni::term;
	std::stringstream ss;
	if (opt.status) {
		std::stringstream status;
		if (m.size()) status << " " << TC_STATUS_OUTPUT << "%"
			<< m.size() << TC.CLEAR() << TC_STATUS;
		if (opt.severity != trivial::info)
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
	std::cout << "Tau version: " << GIT_DESCRIBED << "\n";
}

template <typename... BAs>
void repl_evaluator<BAs...>::help_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& n)
{
	static const std::string bool_options =
#ifdef DEBUG
		"  debug-repl             show REPL commands             on/off\n"
#endif
		"  status                 show status                    on/off\n"
		"  colors                 use term colors                on/off\n"
		"  hilighting             syntax hilighting of formulas  on/off\n"
		"  indenting              indenting of formulas          on/off\n";
	static const std::string all_available_options = std::string{} +
		"Available options:\n" + bool_options +
		"  severity               severity                       error/info/debug/trace\n";
	static const std::string bool_available_options = std::string{} +
		"Available options:\n" + bool_options;
	auto arg = n | tau_parser::help_arg
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| non_terminal_extractor<tau_ba<BAs...>, BAs...>;
	size_t nt = arg ? arg.value()
			: static_cast<size_t>(tau_parser::help_sym);
	switch (nt) {
	case tau_parser::help_sym: std::cout
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
		<< "  run                    run a program that meets the tau spec\n"
		<< "\n"

		<< "Solver commands:\n"
		<< "  solve                  solve a formula\n"
		<< "\n"

		<< "Definition commands:\n"
		<< "  def                    show recurrence relations and input/output variables\n"
		<< "\n"

		//<< "Selection commands:\n"
		//<< "  selection or s       selection control\n"
		//<< "\n"

		<< "Settings commands:\n"
		<< "  get                    get options' values\n"
		<< "  set                    set option's value\n"
		<< "  enable                 enable option's value\n"
		<< "  disable                disable option's value\n"
		<< "  toggle                 toggle option's value\n"
		<< "\n"

		<< "Examples:\n"
		<< "  help or h examples     show examples related to the Tau Language syntax.\n"
		<< "\n";
		break;
	case tau_parser::version_sym: std::cout
		<< "version or v prints out current Tau commit id\n";
		break;
	case tau_parser::quit_sym: std::cout
		<< "q, quit, e or exit exits the repl\n";
		break;
	case tau_parser::clear_sym: std::cout
		<< "clear clears the screen\n";
		break;
	case tau_parser::get_sym: std::cout
		<< "get if used alone prints all options and their values\n"
		<< "get <option>        prints the value of the given option\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau_parser::set_sym: std::cout
		<< "set <option> [=] <value> sets option value\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau_parser::enable_sym: std::cout
		<< "enable <option> enables option\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau_parser::disable_sym: std::cout
		<< "disable <option> disables option\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau_parser::toggle_sym: std::cout
		<< "toggle <option> toggles option value\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau_parser::history_sym: std::cout
		<< "history command manages stored or previous commands results\n"
		<< "\n"
		<< "  history or hist              lists all stored or output results\n"
		<< "  history or hist <memory_id>  prints the given stored or output result\n"
		<< "  <formula>                    store the given formula in the memory\n"
		<< "  %<memory_id>                 provides relative access to memory\n"
		<< "  &<memory_id>                 provides absolute access to memory\n";
		break;
	case tau_parser::normalize_sym: std::cout
		<< "normalize or n command normalizes a formula, prints it and\n"
		<< "saves it into memory of previous memoris\n"
		<< "\n"
		<< "usage:\n"
		<< "  normalize <NSORR>      normalizes the given NSO RR\n"
		<< "  normalize <tau>        normalizes the given tau formula\n"
		<< "  normalize <term>         normalizes the given term formula\n"
		<< "  normalize <memory>     normalizes the memory with the given id\n";
	//	<< "  normalize <selection>  normalizes the selection\n";
		break;
	case tau_parser::qelim_sym: std::cout
		<< "qelim command eliminates the innermost quantifier, prints it and\n"
		<< "saves it into memory of previous memories\n"
		<< "\n"
		<< "usage:\n"
		<< "  qelim <tau>            eliminates innermost quantifier in the given tau\n";
		break;
	case tau_parser::run_sym: std::cout
		<< "Command r, run <tau>\n";
		break;
	case tau_parser::solve_sym: std::cout
		<< "Command s, solve ...\n";
		break;
	case tau_parser::sat_sym: std::cout
		<< "Command sat, is_satisfiable ...\n";
		break;
	case tau_parser::valid_sym: std::cout
		<< "Command valid, is_valid ...\n";
		break;
	case tau_parser::unsat_sym: std::cout
		<< "Command unsat, is_unsatisfiable ...\n";
		break;
	case tau_parser::dnf_sym: std::cout
		<< "dnf command converts a boolean formula or a well formed formula to disjunctive normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  dnf <term>             converts the given term to DNF\n"
		<< "  dnf <tau>            converts the given term to DNF\n"
		<< "  dnf <memory>         converts the memory with the given id to DNF\n";
		break;
	case tau_parser::cnf_sym: std::cout
		<< "cnf command converts a boolean formula or a well formed formula to conjunctive normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  cnf <term>             converts the given term to CNF\n"
		<< "  cnf <tau>            converts the given term to CNF\n"
		<< "  cnf <memory>         converts the memory with the given id to CNF\n";
		break;
	//case tau_parser::anf_sym: std::cout
	//	<< "anf command converts a boolean formula or a well formed formula to algebraic normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  anf <term>             converts the given term to ANF\n"
	//	<< "  anf <tau>            converts the given tau to ANF\n"
	//	<< "  anf <memory>         converts the memory with the given id to ANF\n";
	//	break;
	case tau_parser::nnf_sym: std::cout
		<< "nnf command converts a boolean formula or a well formed formula to negation normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  nnf <term>             converts the given term to NNF\n"
		<< "  nnf <tau>            converts the given term to NNF\n"
		<< "  nnf <memory>         converts the memory with the given id to NNF\n";
		break;
	//case tau_parser::pnf_sym: std::cout
	//	<< "pnf command converts a boolean formula or a well formed formula to prenex normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  pnf <term>             converts the given term to PNF\n"
	//	<< "  pnf <tau>            converts the given term to PNF\n"
	//	<< "  pnf <memory>           converts the memory with the given id to PNF\n";
	//	break;
	case tau_parser::mnf_sym: std::cout
		<< "mnf command converts a boolean formula or a well formed formula to minterm normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  mnf <term>             converts the given term to MNF\n"
		<< "  mnf <tau>            converts the given tau to MNF\n"
		<< "  mnf <memory>         converts the memory with the given id to MNF\n";
		break;
	case tau_parser::onf_sym: std::cout
		<< "onf command converts a well formed formula to order normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  onf <VAR> <tau>      converts the given tau to ONF\n"
		<< "  onf <VAR> <memory>   converts the memory with the given id to ONF\n";
		break;
	case tau_parser::subst_sym: std::cout
		<< "substitute, subst or s substitutes a formula or a function\n"
		<< "\n"
		<< "usage:\n"
		<< "  substitute <input> [ <match> / <replace>]\n"
		<< "\n"
		<< "where:\n"
		<< "  <input> is term or tau or memory reference to term or tau\n"
		<< "  <match> is term or tau supposedly contained in <input>\n"
		<< "  <replace> is term or tau or memory reference to term or tau\n"
		<< "\n"
		<< "  if <input> is term, <match> and <replace> must be term\n"
		<< "  <match> and <replace> must be of the same type\n"
		<< "\n"
		<< "example:\n"
		<< "  substitute ?x && (?y || ?x) [ ?y || ?x / ?y ] # produces ?x && ?y\n"
		<< "\n";
		break;
	case tau_parser::inst_sym: std::cout
		<< "instantiate, inst or i instantiates a variable in a formula or in a function\n"
		<< "\n"
		<< "usage:\n"
		<< "  instantiate <input> [ <variable> / <value>]\n"
		<< "\n"
		<< "where:\n"
		<< "  <input> is term or tau or memory reference to term or tau\n"
		<< "  <variable> is term or tau variable contained in <input>\n"
		<< "  <value> is term or tau value or memory reference to term or tau\n"
		<< "\n"
		<< "  if <input> is term, <variable> and <value> must be term\n"
		<< "  <variable> and <value> must be of the same type\n"
		<< "\n"
		<< "example:\n"
		<< "  instantiate x | y & z [ z / 0 ]  # produces x | y & 0\n"
		<< "\n";
		break;
	case tau_parser::def_sym: std::cout
		<< "defines a rec. relation\n"
		<< "\n"
		<< "usage:\n"
		<< "  <tau_rr>                     defines a wff rec. relation\n"
		<< "  <term_rr>                      defines a term rec. relation\n"
		<< "  defs or definitions          list definitions\n"
		<< "  defs or definitions <rr_id>  print rec relation with given id\n";
		break;
	case tau_parser::examples_sym: std::cout
		<< "examples\n"
		<< "\n"
		<< "  # defining an input variables\n"
		<< "  sbf i1 = console\n"
		<< "  tau i2 = ifile(\"inputs.in\")\n"
		<< "\n"
		<< "  # defining an output variables\n"
		<< "  sbf o1 = console\n"
		<< "  tau o2 = ofile(\"outputs.out\")\n"
		<< "\n"
		<< "  # defining a rec. relation\n"
		<< "  rr1 := (x & y) | z\n"
		<< "  rr2 := ( x = 0 || y = 0) && z = 0\n"
		<< "\n";
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
