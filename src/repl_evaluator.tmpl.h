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
#include "tau.h"
#include "term_colors.h"
#include "satisfiability.h"
#include "solver.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace idni::tau {

#define TC_STATUS        TC.BG_LIGHT_CYAN()
#define TC_STATUS_OUTPUT TC(color::GREEN, color::BG_LIGHT_CYAN, color::BRIGHT)
#define TC_ERROR         TC(color::RED,   color::BRIGHT)
#define TC_PROMPT        TC(color::WHITE, color::BRIGHT)
#define TC_OUTPUT        TC.GREEN()

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::not_implemented_yet() {
	std::cout << "Not implemented yet.\n";
}

template<typename... BAs>
size_t  digits(const sp_tau_node<tau_ba<BAs...>, BAs...>& n) {
	return process_digits(n)
		| only_child_extractor<tau_ba<BAs...>, BAs...>
		| size_t_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>;
}

template<typename... BAs>
std::optional<size_t> get_memory_index(const sp_tau_node<tau_ba<BAs...>, BAs...>& n, const size_t size, bool silent = false) {
	auto mem_type = n
		| only_child_extractor<tau_ba<BAs...>, BAs...>
		| non_terminal_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>;
	auto mem_id = n | mem_type | tau_parser::memory_id
		| optional_value_extractor<sp_tau_node<tau_ba<BAs...>, BAs...>>;
	auto idx = digits(mem_id);
	auto is_relative = (mem_type == tau_parser::relative_memory);
	if (idx >= size) {
		if (!silent) cout << "memory " << TC_OUTPUT
			<< (is_relative ? "%" : "&")
			<< idx << TC.CLEAR() << " does not exist\n";
		return {};
	}
	auto pos = is_relative ? size - 1 - idx : idx;
	return pos;
}


template <typename factory_t, typename... BAs>
repl_evaluator<factory_t, BAs...>::memory_ref
	repl_evaluator<factory_t, BAs...>::memory_retrieve(
		const sp_tau_node<tau_ba<BAs...>, BAs...>& n, bool silent)
{
	if (auto pos = get_memory_index(n, m.size()); pos.has_value(), silent)
		return {{m[pos.value()], pos.value()}};
	return {};
}

template<typename... BAs>
void print_memory(const nso<tau_ba<BAs...>, BAs...> mem, const size_t id, const size_t size) {
	cout << TC_OUTPUT << "&" << id << TC.CLEAR() << "/"
		<< TC_OUTPUT << "%" << (size - id - 1) << TC.CLEAR()
		<< ": " << mem << "\n";
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::memory_print_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& command)
{
	auto n = command | tau_parser::memory;
	if (!n) return;
	auto idx = get_memory_index(n.value(), m.size());
	if (!idx) return;
	print_memory(m[idx.value()], idx.value(), m.size());
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::memory_list_cmd() {
	if (!m.size()) cout << "memory is empty\n";
	else for (size_t i = 0; i < m.size(); i++)
		print_memory(m[i], i, m.size());
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::memory_clear_cmd() {
	m.clear();
	cout << "memorys cleared\n";
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::memory_store(
	typename repl_evaluator<factory_t, BAs...>::memory o)
{
	// do not add into memory if the last memory value is the same
	if (m.size() && m.back() == o) return;
	m.push_back(o);
	print_memory(m.back(), m.size() - 1, m.size());
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::memory_store_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& command) {
	auto form = command
		| tau_parser::memory_store_cmd_arg
		| only_child_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<nso<tau_ba<BAs...>, BAs...>>;
	memory_store(form);
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::memory_del_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& command) {
	auto n = command | tau_parser::memory;
	if (!n) return;
	auto idx = get_memory_index(n.value(), m.size());
	if (!idx) return;
	m.erase(m.begin() + idx.value());
	cout << "deleted index " << idx.value() << " from memory\n";
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::get_bf(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto bf = n | tau_parser::bf; bf) return bf.value();
	else if (auto memory = n | tau_parser::memory; memory) {
		auto [value, _] = memory_retrieve(memory.value()).value();
		return value;
	}
	return {};
}
template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::get_wff(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto wff = n | tau_parser::wff; wff) return wff.value();
	else if (auto memory = n | tau_parser::memory; memory) {
		auto [value, _] = memory_retrieve(memory.value()).value();
		return value;
	}
	return {};
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::wff_onf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto var = n | tau_parser::variable
		| optional_value_extractor<nso<tau_ba<BAs...>, BAs...>>;
	if (auto form = get_wff(n); form)
		return onf<tau_ba<BAs...>, BAs...>(form.value(), var);
	return {};
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::wff_dnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto form = get_wff(n); form) return
		dnf<tau_parser::wff, tau_ba<BAs...>, BAs...>(form.value());
	return {};
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::wff_cnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto form = get_wff(n); form) return
		cnf<tau_parser::wff, tau_ba<BAs...>, BAs...>(form.value());
	return {};
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::wff_nnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto form = get_wff(n); form) return
		nnf<tau_parser::wff, tau_ba<BAs...>, BAs...>(form.value());
	return {};
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::wff_mnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto form = get_wff(n); form) return
		mnf<tau_parser::wff, tau_ba<BAs...>, BAs...>(form.value());
	return {};
}
template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::bf_dnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto form = get_bf(n); form) return
		dnf<tau_parser::bf, tau_ba<BAs...>, BAs...>(form.value());
	return {};
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::bf_cnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto form = get_bf(n); form) return
		cnf<tau_parser::bf, tau_ba<BAs...>, BAs...>(form.value());
	return {};
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::bf_nnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto form = get_bf(n); form) return
		nnf<tau_parser::bf, tau_ba<BAs...>, BAs...>(form.value());
	return {};
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::bf_mnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto form = get_bf(n); form) return
		mnf<tau_parser::bf, tau_ba<BAs...>, BAs...>(form.value());
	return {};
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::bf_substitute_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto thiz = get_bf(n->child[1]).value();
	auto in = get_bf(n->child[2]).value();
	auto with = get_bf(n->child[3]).value();
	std::map<nso<tau_ba<BAs...>, BAs...>, nso<tau_ba<BAs...>, BAs...>>
		changes = {{thiz, with}};
	return replace(in, changes);
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::wff_substitute_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto thiz = get_wff(n->child[1]).value();
	auto in = get_wff(n->child[2]).value();
	auto with = get_wff(n->child[3]).value();
	std::map<nso<tau_ba<BAs...>, BAs...>, nso<tau_ba<BAs...>, BAs...>>
		changes = {{thiz, with}};
	return replace(in, changes);
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::bf_instantiate_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto var = wrap(tau_parser::bf, n | tau_parser::variable
		| optional_value_extractor<nso<tau_ba<BAs...>, BAs...>>);
	auto in = get_bf(n->child[2]).value();
	auto with = trim(get_bf(n->child[3]).value());
	std::map<nso<tau_ba<BAs...>, BAs...>, nso<tau_ba<BAs...>, BAs...>>
		changes = {{var, with}};
	return replace(in, changes);
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::wff_instantiate_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto var = wrap(tau_parser::wff, n | tau_parser::variable
		| optional_value_extractor<nso<tau_ba<BAs...>, BAs...>>);
	auto in = get_wff(n->child[2]).value();
	auto with = trim(get_wff(n->child[3]).value());
	std::map<nso<tau_ba<BAs...>, BAs...>, nso<tau_ba<BAs...>, BAs...>>
		changes = {{var, with}};
	return replace(in, changes);
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::normalizer_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	auto arg = n | tau_parser::normalize_cmd_arg;
	if (auto wff = arg | tau_parser::wff; wff) {
		rr<gssotc<BAs...>> rr_wff = { definitions, wff.value() };
		auto result = normalizer<tau_ba<BAs...>, BAs...>(rr_wff);
		return result;
	} else if (auto nso_rr = arg | tau_parser::nso_rr; nso_rr) {
		auto n_nso_rr = make_nso_rr_using_factory<
			factory_t, tau_ba<BAs...>, BAs...>(
				arg.value(), factory);
		rec_relations<nso<tau_ba<BAs...>, BAs...>> rrs;
		rrs.insert(rrs.end(), n_nso_rr.rec_relations.begin(), n_nso_rr.rec_relations.end());
		rrs.insert(rrs.end(), definitions.begin(), definitions.end());
		rr<nso<tau_ba<BAs...>, BAs...>> rr_nso = { rrs, n_nso_rr.main };
		auto result = normalizer<tau_ba<BAs...>, BAs...>(rr_nso);
		return result;
	} else if (auto memory = arg | tau_parser::memory; memory) {
		auto ref = memory_retrieve(memory.value());
		if (ref) {
			auto [value, _] = ref.value();
			rr<gssotc<BAs...>> rr_memory = { definitions, value };
			auto result = normalizer<tau_ba<BAs...>, BAs...>(rr_memory);
			return result;
		}
		return {};
	}
	return arg;
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::execute_cmd(const nso<tau_ba<BAs...>, BAs...>& n) {
	auto form = n | tau_parser::execute_cmd_arg;
	if (auto check = form | tau_parser::tau; check) {
		// TODO (HIGH) call executor
	} else if (auto check = form | tau_parser::gssotc_rr; check) {
		// TODO (HIGH) call executor
	}
	not_implemented_yet();
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>> repl_evaluator<factory_t, BAs...>::solve_cmd(const nso<tau_ba<BAs...>, BAs...>& n) {
	auto form = n | tau_parser::solve_cmd_arg;
	// TODO (VERY_HIGH) call solver
	if (auto check = form | tau_parser::wff; check) {
	} else {
	}
	return {};
}

template<typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::is_satisfiable_cmd(const nso<tau_ba<BAs...>, BAs...>&) {
	// TODO (HIGH) call satisfiability
	not_implemented_yet();
}

template<typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::is_valid_cmd(const nso<tau_ba<BAs...>, BAs...>&) {
	// TODO (HIGH) call satisfiability
	not_implemented_yet();
}

template<typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::is_unsatisfiable_cmd(const nso<tau_ba<BAs...>, BAs...>&) {
	// TODO (HIGH) call satisfiability
	not_implemented_yet();
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::def_rule_cmd(const nso<tau_ba<BAs...>, BAs...>& n) {
	auto r = n | tau_parser::def_rule_cmd_arg | optional_value_extractor<nso<tau_ba<BAs...>, BAs...>>;
	auto rule = make_rec_relation<tau_ba<BAs...>, BAs...>(r);
	definitions.emplace_back(rule);
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::def_list_cmd() {
	for (size_t i = 0; i < definitions.size(); i++)	cout << "[" << i << "]" << definitions[i] << "\n";
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::def_clear_cmd() {
	definitions.clear();
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::def_del_cmd(const nso<tau_ba<BAs...>, BAs...>& n) {
	auto idx = digits(n | tau_parser::digits | optional_value_extractor<nso<tau_ba<BAs...>, BAs...>>);
	definitions.erase(definitions.begin() + idx);
}

// make a nso_rr from the given tau source and binder.
template <typename factory_t, typename... BAs>
sp_tau_node<tau_ba<BAs...>, BAs...>
	repl_evaluator<factory_t, BAs...>::make_cli(
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
	tau_factory<factory_t, BAs...> tf(factory);
	return bind_tau_code_using_factory<tau_factory<factory_t, BAs...>,
		tau_ba<BAs...>, BAs...>(cli_code, tf);
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::get_cmd(
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
	auto option = n | tau_parser::option;
	if (!option.has_value()) { for (auto& [_, v] : printers) v(); return; }
	printers[get_opt(option.value())]();

}

template <typename factory_t, typename... BAs>
boost::log::trivial::severity_level
	repl_evaluator<factory_t, BAs...>::nt2severity(size_t nt) const
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
	auto bool_opt = n | tau_parser::bool_option;
	if (bool_opt.has_value()) n = bool_opt.value();
	return n| only_child_extractor<BAs...>
		| non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::set_cmd(
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

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::toggle_cmd(
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
}

template <typename factory_t, typename... BAs>
int repl_evaluator<factory_t, BAs...>::eval_cmd(
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
	case p::memory_list_cmd:    memory_list_cmd(); break;
	case p::memory_clear_cmd:   memory_clear_cmd(); break;
	case p::memory_print_cmd:   memory_print_cmd(command); break;
	case p::memory_store_cmd:   memory_store_cmd(command); break;
	case p::memory_del_cmd:     memory_del_cmd(command); break;
	// normalization
	case p::normalize_cmd:      result = normalizer_cmd(command); break;
	// execution
	case p::execute_cmd:        execute_cmd(command); break;
	case p::solve_cmd:          solve_cmd(command); break;
	// substitution and instantiation
	case p::bf_substitute_cmd:  result = bf_substitute_cmd(command); break;
	case p::bf_instantiate_cmd: result = bf_instantiate_cmd(command); break;
	case p::wff_substitute_cmd: result = wff_substitute_cmd(command); break;
	case p::wff_instantiate_cmd:result = wff_instantiate_cmd(command); break;
	// formula checks
	case p::is_satisfiable_cmd:   is_satisfiable_cmd(command); break;
	case p::is_valid_cmd:         is_valid_cmd(command); break;
	case p::is_unsatisfiable_cmd: is_unsatisfiable_cmd(command); break;
	// wff normal forms
	case p::wff_onf_cmd:        result = wff_onf_cmd(command); break;
	case p::wff_dnf_cmd:        result = wff_dnf_cmd(command); break;
	case p::wff_cnf_cmd:        result = wff_cnf_cmd(command); break;
	case p::wff_anf_cmd:        not_implemented_yet(); break;
	case p::wff_nnf_cmd:        result = wff_cnf_cmd(command); break;
	case p::wff_pnf_cmd:        not_implemented_yet(); break;
	case p::wff_mnf_cmd:        result = wff_mnf_cmd(command); break;
	// bf normal forms
	case p::bf_dnf_cmd:         result = bf_dnf_cmd(command); break;
	case p::bf_cnf_cmd:         result = bf_cnf_cmd(command); break;
	case p::bf_anf_cmd:         not_implemented_yet(); break;
	case p::bf_nnf_cmd:         result = bf_nnf_cmd(command); break;
	case p::bf_pnf_cmd:         not_implemented_yet(); break;
	case p::bf_mnf_cmd:         result = bf_mnf_cmd(command); break;
	// store the given formula as memory for future references
	case p::bf:                 result = command; break;
	case p::wff:                result = command; break;
	case p::nso_rr:	            result = command; break;
	// definition of rec relations to be included during normalization
	case p::def_rule_cmd:       def_rule_cmd(command); break;
	case p::def_list_cmd:       def_list_cmd(); break;
	case p::def_clear_cmd:      def_clear_cmd(); break;
	case p::def_del_cmd:        def_del_cmd(command); break;
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

template <typename factory_t, typename... BAs>
repl_evaluator<factory_t, BAs...>::repl_evaluator(factory_t& factory,
	options opt) : factory(factory), opt(opt)
{
	TC.set(opt.colors);
	boost::log::core::get()->set_filter(
		boost::log::trivial::severity >= opt.severity);
}

template <typename factory_t, typename... BAs>
std::string repl_evaluator<factory_t, BAs...>::prompt() {
	using namespace boost::log;
	std::stringstream ss;
	if (opt.status) {
		std::stringstream status;
		if (m.size()) status << " " << TC_STATUS_OUTPUT << "&"
			<< m.size()-1 << TC.CLEAR() << TC_STATUS;
		if (opt.severity != trivial::error)
			status << " " << to_string(opt.severity);
		if (status.tellp()) ss << TC_STATUS << "["
			<< status.str() << " ]" << TC.CLEAR() << " ";
	}
	ss << (error ? TC_ERROR : TC_PROMPT) << "tau>" << TC.CLEAR() << " ";
	if (r) r->prompt(ss.str());
	return ss.str();
}

template <typename factory_t, typename... BAs>
int repl_evaluator<factory_t, BAs...>::eval(const std::string& src) {
	error = false;
	auto tau_spec = make_cli(src);
	int quit = 0;
	if (tau_spec) {
		auto commands = tau_spec || tau_parser::cli_command;
		for (const auto& cmd : commands)
			if (quit = eval_cmd(cmd); quit) break;
	}
	if (!quit) prompt();
	return quit;
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::version_cmd() {
	cout << "Tau version: " << GIT_DESCRIBED << "\n";
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::help_cmd(
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
	auto arg = n | tau_parser::cli_cmd_sym
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| non_terminal_extractor<tau_ba<BAs...>, BAs...>;
	size_t nt = arg ? arg.value()
			: static_cast<size_t>(tau_parser::help_cmd_sym);
	switch (nt) {
	case tau_parser::help_cmd_sym: cout
		<< "Commands:\n"
		<< "  help or h              print this help\n"
		<< "  quit, q, exit or e     exit the repl\n"
		<< "  version or v           print version\n"
		<< "  clear                  clear the screen\n"
		<< "\n"

		<< "Formula commands:\n"
		<< "  normalize or n         normalize formula\n"
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

		//<< "Substitution and instantiation commands:\n"
		//<< "  substitute             substitute a formula\n"
		//<< "  instantiate            instantiate a formula\n"
		//<< "\n"

		<< "Validity commands:\n"
		<< "  sat                   check satisfiability\n"
		<< "  valid                 check validity\n"
		<< "  unsat				    check unsatisfiability\n"
		<< "\n"

		<< "Execute commands:\n"
		<< "  execute                execute a tau code\n"
		<< "\n"

		<< "Solver commands:\n"
		<< "  solve                  solve a formula\n"
		<< "\n"

		<< "Definition commands:\n"
		<< "  def                   define a rec. relation\n"
		<< "  def list              list all rec. relations\n"
		<< "  def clear             clear all rec. relations\n"
		<< "  def del <id>          delete rec. relation\n"

		//<< "Selection commands:\n"
		//<< "  selection or s         selection control\n"

		<< "Settings commands:\n"
		<< "  get                    get options' values\n"
		<< "  set                    set option's value\n"
		<< "  toggle                 toggle option's value\n"

		<< "Examples:\n"
		<< "  help or h examples     show examples related to the Tau Language syntax.\n";
		break;
	case tau_parser::version_cmd_sym: cout
		<< "version or v prints out current Tau commit id\n";
		break;
	case tau_parser::quit_cmd_sym: cout
		<< "q, quit, e or exit exits the repl\n";
		break;
	case tau_parser::clear_cmd_sym: cout
		<< "clear clears the screen\n";
		break;
	case tau_parser::get_cmd_sym: cout
		<< "get if used alone prints all options and their values\n"
		<< "get <option>        prints the value of the given option\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau_parser::set_cmd_sym: cout
		<< "set <option> [=] <value> sets option value\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau_parser::toggle_cmd_sym: cout
		<< "toggle <option> toggles option value\n"
		<< "\n"
		<< bool_available_options;
		break;
	case tau_parser::memory_cmd_sym: cout
		<< "memory command manages stored or previous commands results\n"
		<< "\n"
		<< "  memory                 lists all stored or output results\n"
		<< "  memory <memory_id>     prints the given stored or output result\n"
		<< "  memory clear           clears memory\n"
		<< "  memory <formula>       store the given formula in the memory\n"
		<< "  memory del <memory_id> deletes the given stored or output result\n"
		<< "  %<memory_id>           provides relative access to memory\n"
		<< "  &<memory_id>           provides absolute access to memory\n";
		break;

	//case tau_parser::selection_sym: cout
	//	<< "Command s, selection ...\n";
	//	break;

	//case tau_parser::file_cmd_sym: cout
	//	<< "Command r, read ...\n";
	//	break;

	case tau_parser::normalize_cmd_sym: cout
		<< "normalize or n command normalizes a formula, prints it and\n"
		<< "saves it into memory of previous memorys\n"
		<< "\n"
		<< "usage:\n"
		<< "  normalize '<NSORR>'    normalizes the given NSO RR\n"
		<< "  normalize '<WFF>'      normalizes the given WFF formula\n";
	//	<< "  normalize <memory>     normalizes the memory with the given id\n"
	//	<< "  normalize <selection>  normalizes the selection\n";
		break;

	case tau_parser::execute_cmd_sym: cout
		<< "Command e, execute ...\n";
		break;

	case tau_parser::solve_cmd_sym: cout
		<< "Command s, solve ...\n";
		break;

	case tau_parser::is_satisfiable_cmd_sym: cout
		<< "Command sat, is_satisfiable ...\n";
		break;
	case tau_parser::is_valid_cmd_sym: cout
		<< "Command valid, is_valid ...\n";
		break;
	case tau_parser::is_unsatisfiable_cmd_sym: cout
		<< "Command unsat, is_unsatisfiable ...\n";
		break;

	case tau_parser::dnf_cmd_sym: cout
		<< "dnf command converts a boolean formula or a well formed formula to disjunctive normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  dnf '<BF>'             converts the given BF to DNF\n"
		<< "  dnf '<WFF>'            converts the given BF to DNF\n";
	//	<< "  dnf <memory>           converts the memory with the given id to DNF\n";
		break;
	case tau_parser::cnf_cmd_sym: cout
		<< "cnf command converts a boolean formula or a well formed formula to conjunctive normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  cnf '<BF>'             converts the given BF to CNF\n"
		<< "  cnf '<WFF>'            converts the given BF to CNF\n";
	//	<< "  cnf <memory>           converts the memory with the given id to CNF\n";
		break;
	//case tau_parser::anf_cmd_sym: cout
	//	<< "cnf command converts a boolean formula or a well formed formula to algebraic normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  anf '<BF>'             converts the given BF to ANF\n"
	//	<< "  anf '<WFF>'            converts the given BF to ANF\n"
	//	<< "  anf <memory>           converts the memory with the given id to ANF\n";
	//	break;
	case tau_parser::nnf_cmd_sym: cout
		<< "nnf command converts a boolean formula or a well formed formula to negation normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  nnf '<BF>'             converts the given BF to NNF\n"
		<< "  nnf '<WFF>'            converts the given BF to NNF\n";
	//	<< "  nnf <memory>           converts the memory with the given id to NNF\n";
		break;
	//case tau_parser::pnf_cmd_sym: cout
	//	<< "cnf command converts a boolean formula or a well formed formula to prenex normal form\n"
	//	<< "\n"
	//	<< "usage:\n"
	//	<< "  pnf '<BF>'             converts the given BF to PNF\n"
	//	<< "  pnf '<WFF>'            converts the given BF to PNF\n"
	//	<< "  pnf <memory>           converts the memory with the given id to PNF\n";
	//	break;
	case tau_parser::mnf_cmd_sym: cout
		<< "mnf command converts a boolean formula or a well formed formula to minterm normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  mnf '<BF>'             converts the given BF to MNF\n"
		<< "  mnf '<WFF>'            converts the given BF to MNF\n";
	//	<< "  mnf <memory>           converts the memory with the given id to MNF\n";
		break;
	case tau_parser::onf_cmd_sym: cout
		<< "onf command converts a well formed formula to order normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  onf '<VAR>' '<WFF>'    converts the given WFF to ONF\n";
	//	<< "  onf '<VAR>' <memory>   converts the memory with the given id to ONF\n";
		break;

	//case tau_parser::substitute_cmd_sym: cout
	//	<< "Command s, substitute ...\n";
	//	break;
	//case tau_parser::instantiate_cmd_sym: cout
	//	<< "Command i, instantiate ...\n";
	//	break;

	case tau_parser::def_cmd_sym: cout
		<< "defines a rec. relation\n"
		<< "\n"
		<< "usage:\n"
		<< "  def '<TAU>'      defines a tau rec. relation\n"
		<< "  def '<WFF>'      defines a wff rec. relation\n"
		<< "  def '<BF>'       defines a bf rec. relation\n"
		<< "  def list         list definitions\n"
		<< "  def clear        clear all definitions\n"
		<< "  def del <id>     delete <id> definition\n";
		break;

	case tau_parser::examples_sym: cout
		<< "examples\n";
		// TODO (VERY_HIGH) show examples
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
