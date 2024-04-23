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
#include "normalizer2.h"
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

template <typename factory_t, typename... BAs>
size_t  repl_evaluator<factory_t, BAs...>::digits(
	sp_tau_node<tau_ba<BAs...>, BAs...> n)
{
	return process_digits(n)
		| only_child_extractor<tau_ba<BAs...>, BAs...>
		| size_t_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>;
}

template <typename factory_t, typename... BAs>
repl_evaluator<factory_t, BAs...>::output_ref
	repl_evaluator<factory_t, BAs...>::get_output_ref(
		const sp_tau_node<tau_ba<BAs...>, BAs...>& n, bool silent)
{
	auto out_type = n
		| only_child_extractor<tau_ba<BAs...>, BAs...>
		| non_terminal_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<size_t>;
	auto out_id = n | out_type | tau_parser::output_id
		| optional_value_extractor<sp_tau_node<tau_ba<BAs...>, BAs...>>;
	auto idx = digits(out_id);
	auto is_relative = (out_type == tau_parser::relative_output);
	if (idx >= m.size()) {
		if (!silent) cout << "output " << TC_OUTPUT
			<< (is_relative ? "%" : "&")
			<< idx << TC.CLEAR() << " does not exist\n";
		return {};
	}
	auto pos = is_relative ? m.size() - 1 - idx : idx;
	if (auto check = m[pos];
		std::holds_alternative<nso<tau_ba<BAs...>, BAs...>>(check))
	{
		return {{std::get<nso<tau_ba<BAs...>, BAs...>>(m[pos]), pos}};
	}
	return {};
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::print_output(size_t id) {
	cout << TC_OUTPUT << "&" << id << TC.CLEAR() << "/"
		<< TC_OUTPUT << "%" << (m.size() - id - 1) << TC.CLEAR()
		<< ": " << m[id] << "\n";
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::print_output_cmd(
	const sp_tau_node<tau_ba<BAs...>, BAs...>& command)
{
	auto n = command | tau_parser::output;
	if (!n) return;
	auto o = get_output_ref(n.value());
	if (!o) return;
	print_output(o.value().second);
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::list_outputs_cmd() {
	if (!m.size()) cout << "no outputs\n";
	else for (size_t i = 0; i < m.size(); i++) print_output(i);
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::clear_outputs_cmd() {
	m.clear();
	cout << "outputs cleared\n";
}

template <typename factory_t, typename... BAs>
void repl_evaluator<factory_t, BAs...>::store_output(
	typename repl_evaluator<factory_t, BAs...>::output o)
{
	// do not add into memory if the last memory value is the same ?
	// if (m.size() && m.back() == o) return;
	m.push_back(o);
	print_output(m.size() - 1);
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>>
	repl_evaluator<factory_t, BAs...>::get_bf(
		const nso<tau_ba<BAs...>, BAs...>& n)
{
	if (auto bf = n | tau_parser::bf; bf) return bf.value();
	else if (auto output = n | tau_parser::output; output) {
		auto [value, _] = get_output_ref(output.value()).value();
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
	else if (auto output = n | tau_parser::output; output) {
		auto [value, _] = get_output_ref(output.value()).value();
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
		//std::cout << "normalized: " << result << "\n";
		return result;
	} else if (auto output = arg | tau_parser::output; output) {
		auto ref = get_output_ref(output.value());
		if (ref) {
			auto [value, _] = ref.value();
			rr<gssotc<BAs...>> rr_output = { definitions, value };
			auto result = normalizer<tau_ba<BAs...>, BAs...>(rr_output);
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
		auto n_gssotc = make_tau_spec_using_factory<factory_t, BAs...>(form.value(), factory);
		// TODO (HIGH) call executor
	} else if (auto check = form | tau_parser::gssotc_rr; check) {
		auto n_gssotc_rr = make_tau_spec_using_factory<factory_t, BAs...>(form.value(), factory);
		rec_relations<gssotc<BAs...>> rrs;
		rrs.insert(rrs.end(), n_gssotc_rr.rec_relations.begin(), n_gssotc_rr.rec_relations.end());
		rrs.insert(rrs.end(), definitions.begin(), definitions.end());
		tau_spec<BAs...> rr_gssotc = { rrs, n_gssotc_rr.main };
		// TODO (HIGH) call executor
	}
	not_implemented_yet();
}

template <typename factory_t, typename... BAs>
std::optional<nso<tau_ba<BAs...>, BAs...>> repl_evaluator<factory_t, BAs...>::solve_cmd(const nso<tau_ba<BAs...>, BAs...>& n) {
	auto form = n | tau_parser::solve_cmd_arg;
	if (auto check = form | tau_parser::wff; check) {
		std::cout << "solving wff\n";
		// TODO (HIGH) call solver
	} else {
		std::cout << "solving unimplemented type\n";
		// TODO (HIGH) call solver
	}
	return {};
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
	case p::list_outputs_cmd:   list_outputs_cmd(); break;
	case p::clear_outputs_cmd:  clear_outputs_cmd(); break;
	case p::print_output_cmd:   print_output_cmd(command); break;
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
	// store the given formula as output for future references
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
	if (result) store_output(result.value());
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
		<< "Output commands:\n"
		<< "  output or o            output control\n"
		<< "  &                      output_ref. or short alias of output command\n"
		<< "  %                      output ref. or relative output control\n"
		<< "\n"
		<< "Settings commands:\n"
		<< "  get                    get options' values\n"
		<< "  set                    set option's value\n"
		<< "  toggle                 toggle option's value\n";
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
	case tau_parser::output_sym: cout
		<< "output or out command manages outputs of previous commands\n"
		<< "\n"
		<< "  output                 lists all outputs\n"
		<< "  output <output_id>     prints output\n"
		<< "  output clear           clears output memory\n";
		break;
	case tau_parser::absolute_output_sym: cout
		<< "& is a short alias of output command\n"
		<< "\n"
		<< "  &                      lists all outputs\n"
		<< "  &clear                 clears output memory\n"
		<< "  &<output_id>           is a reference to a previous output\n"
		<< "                         if used as a command it prints the output\n";
		break;
	case tau_parser::relative_output_sym: cout
		<< "% same as & but output_id is relative to the latest\n"
		<< "\n"
		<< "  %                      lists all outputs\n"
		<< "  %clear                 clears output memory\n"
		<< "  %<relative_output_id>  is a reference to a previous output\n"
		<< "                         if used as a command it prints the output\n";
		break;
	case tau_parser::selection_sym: cout
		<< "Command s, selection ...\n";
		break;
	case tau_parser::instantiate_cmd_sym: cout
		<< "Command i, instantiate ...\n";
		break;
	case tau_parser::substitute_cmd_sym: cout
		<< "Command s, substitute ...\n";
		break;
	case tau_parser::normalize_cmd_sym: cout
		<< "normalize or n command normalizes a formula, prints it and\n"
		<< "saves it into memory of previous outputs\n"
		<< "\n"
		<< "usage:\n"
		<< "  normalize '<NSORR>'    normalizes the given NSO RR\n"
		<< "  normalize '<WFF>'      normalizes the given WFF formula\n"
		<< "  normalize <output>     normalizes the output with the given id\n";
		break;
	case tau_parser::file_cmd_sym: cout
		<< "Command r, read ...\n";
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
	case tau_parser::dnf_cmd_sym: cout
		<< "dnf command converts a boolean formula or a well formed formula to disjunctive normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  dnf '<BF>'             converts the given BF to DNF\n"
		<< "  dnf '<WFF>'            converts the given BF to DNF\n"
		<< "  dnf <output>           converts the output with the given id to DNF\n";
		break;
	case tau_parser::cnf_cmd_sym: cout
		<< "cnf command converts a boolean formula or a well formed formula to conjunctive normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  cnf '<BF>'             converts the given BF to CNF\n"
		<< "  cnf '<WFF>'            converts the given BF to CNF\n"
		<< "  cnf <output>           converts the output with the given id to CNF\n";
		break;
	case tau_parser::nnf_cmd_sym: cout
		<< "nnf command converts a boolean formula or a well formed formula to negation normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  nnf '<BF>'             converts the given BF to NNF\n"
		<< "  nnf '<WFF>'            converts the given BF to NNF\n"
		<< "  nnf <output>           converts the output with the given id to NNF\n";
		break;
	case tau_parser::mnf_cmd_sym: cout
		<< "mnf command converts a boolean formula or a well formed formula to minterm normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  mnf '<BF>'             converts the given BF to MNF\n"
		<< "  mnf '<WFF>'            converts the given BF to MNF\n"
		<< "  mnf <output>           converts the output with the given id to MNF\n";
		break;
	case tau_parser::onf_cmd_sym: cout
		<< "onf command converts a well formed formula to order normal form\n"
		<< "\n"
		<< "usage:\n"
		<< "  onf '<VAR>' '<WFF>'    converts the given WFF to ONF\n"
		<< "  onf '<VAR>' <output>   converts the output with the given id to ONF\n";
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
