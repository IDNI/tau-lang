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
#include "parser_instance.h"
#include "normalizer2.h"
#include "normal_forms.h"
#include "nso_rr.h"
#include "bdd_binding.h"
#include "term_colors.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace idni::tau {

// internal namespace for globals scoped to this file
// to access them within your local function/method scope, use:
//     using namespace _repl_evaluator;
namespace _repl_evaluator {
	static term::colors TC;
	static bool error = false;
}

#define TC_STATUS        TC.BG_CYAN()
#define TC_STATUS_OUTPUT TC(color::GREEN, color::BG_CYAN, color::BRIGHT)
#define TC_ERROR         TC(color::RED,   color::BRIGHT)
#define TC_PROMPT        TC(color::WHITE, color::BRIGHT)
#define TC_OUTPUT        TC.GREEN()


template <typename... BAs>
void reprompt(repl_evaluator<BAs...>& re) {
	using namespace _repl_evaluator;
	using namespace boost::log;
	std::stringstream ss;
	if (re.opt.status) {
		std::stringstream status;
		if (re.m.size()) status << " " << TC_STATUS_OUTPUT << "&"
			<< re.m.size()-1 << TC.CLEAR() << TC_STATUS;
		if (re.opt.severity != trivial::error)
			status << " " << to_string(re.opt.severity);
		if (status.tellp()) ss << TC_STATUS << "["
			<< status.str() << " ]" << TC.CLEAR() << " ";
	}
	if (_repl_evaluator::error) ss << TC_ERROR << "error" << TC.CLEAR() << " ";
	ss << TC_PROMPT << "tau>" << TC.CLEAR() << " ";
	if (re.r) re.r->prompt(ss.str());
}

template <typename... BAs>
size_t digits(sp_tau_node<BAs...> n) {
	return process_digits(n)
		| only_child_extractor<BAs...>
		| size_t_extractor<BAs...>
		| optional_value_extractor<size_t>;
}

template <typename... BAs>
void print_output(size_t id, const typename repl_evaluator<BAs...>::outputs& m) {
	using namespace _repl_evaluator;
	cout << TC_OUTPUT << "&" << id << TC.CLEAR() << " / "
		<< TC_OUTPUT << "%" << (m.size() - id - 1) << TC.CLEAR()
		<< ": " << m[id] << "\n";
}

struct output_ref {
	size_t id = 0;
	size_t abs_id = 0;
	size_t rel_id = 0;
	bool relative = false;
	bool out_of_range = false;
};

template <typename... BAs>
output_ref get_output_ref(sp_tau_node<BAs...> n,
	const typename repl_evaluator<BAs...>::outputs& m, bool silent = false)
{
	using namespace _repl_evaluator;
	auto out_type = n
		| only_child_extractor<BAs...>
		| non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
	auto out_id = n | out_type | tau_parser::output_id
		| optional_value_extractor<
			sp_tau_node<BAs...>>;
	output_ref o;
	o.relative = out_type == tau_parser::relative_output;
	o.id = digits<BAs...>(out_id);
	o.abs_id = o.relative ? m.size() - o.id - 1 : o.id;
	o.rel_id = o.relative ? o.id : m.size() - o.id - 1;
	o.out_of_range = o.abs_id >= m.size();
	if (!silent && o.out_of_range)
		cout << "output " << TC_OUTPUT << (o.relative ? "%" : "&")
			<< o.id << TC.CLEAR() << " does not exist\n";
	return o;
}

template <typename... BAs>
void print_output_cmd(sp_tau_node<BAs...> command,
	const typename repl_evaluator<BAs...>::outputs& m)
{
	auto o = get_output_ref<BAs...>(command, m);
	if (o.out_of_range) return;
	print_output<BAs...>(o.abs_id, m);
}

template <typename... BAs>
void list_outputs(typename repl_evaluator<BAs...>::outputs& m) {
	if (!m.size()) cout << "no outputs\n";
	else for (size_t i = 0; i < m.size(); i++) print_output(i, m);
}

template <typename... BAs>
void clear_outputs(typename repl_evaluator<BAs...>::outputs& m) {
	m.clear();
	cout << "outputs cleared\n";
}

template <typename... BAs>
void store_output(typename repl_evaluator<BAs...>::output o,
	typename repl_evaluator<BAs...>::outputs& m)
{
	// do not add into memory if the last memory value is the same ?
	// if (m.size() && m.back() == o) return;
	m.push_back(o);
}

// make wff

// make rr_wff

template <typename... BAs>
nso<BAs...> normalizer_cmd(
	const nso<BAs...>& n,
	typename repl_evaluator<BAs...>::outputs& m)
{
	auto normal = [&m] (auto& n) {
		auto result = normalizer<BAs...>(n);
		std::cout << "normalized: " << result << "\n";
		store_output(result, m);
		return result;
	};
	auto type = n | tau_parser::form_arg
		| only_child_extractor<BAs...>
		| non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
	auto arg = (n | tau_parser::form_arg | type).value();
	switch(type) {
	case tau_parser::q_wff: {
		auto wff = arg | tau_parser::wff;
		if (wff.has_value()) return normal(wff.value());
		std::cout << "Unsupported type to normalize.\n"; return n;
	}
	// case tau_parser::q_nso_rr: {
	// 	auto nso_rr = n | tau_parser::q_nso_rr | optional_value_extractor<nso<BAs...>>;
	// 	return normal(nso_rr);
	// }
	case tau_parser::output: {
		auto o = get_output_ref(arg, m);
		if (o.out_of_range) return n;
		auto& ov = m[o.abs_id];
		if (std::holds_alternative<rr<nso<BAs...>>>(ov))
			return normal(std::get<rr<nso<BAs...>>>(ov));
		return normal(std::get<nso<BAs...>>(ov));
	}
	default: {
		std::cout << "Unsupported type to normalize.\n";
		return n;
	}}
}

// make a nso_rr from the given tau source and binder.
template <typename... BAs>
sp_tau_node<BAs...> make_cli(const std::string src) {
	auto cli_src = make_tau_source(src, { .start = tau_parser::cli });
	bdd_binding_factory bf;
	tau_factory<bdd_binding_factory, bdd_binding> tbf(bf);
	factory_binder<tau_factory<bdd_binding_factory, bdd_binding>, BAs...> fb(tbf);
	return make_tau_code<BAs...>(cli_src);
}

template <typename... BAs>
size_t get_opt(sp_tau_node<BAs...> n) {
	auto bool_opt = n | tau_parser::bool_option;
	if (bool_opt.has_value()) n = bool_opt.value();
	return n| only_child_extractor<BAs...>
		| non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
}

template <typename... BAs>
void get_cmd(sp_tau_node<BAs...> n, repl_evaluator<BAs...>& re) {
	static auto pbool = [] (bool b) { return b ? "on" : "off"; };
	static std::map<size_t,	std::function<void()>> printers = {
	{ tau_parser::status_opt,   [&re]() {
		cout << "show status: " << pbool(re.opt.status) << "\n"; } },
	{ tau_parser::colors_opt,   [&re]() {
		cout << "colors:      " << pbool(re.opt.colors) << "\n"; } },
	{ tau_parser::severity_opt, [&re]() {
		cout << "severity:    " << re.opt.severity << "\n"; } }};
	auto option = n | tau_parser::option;
	if (!option.has_value()) { for (auto& [_, v] : printers) v(); return; }
	printers[get_opt(option.value())]();
}

template <typename... BAs>
void set_cmd(sp_tau_node<BAs...> n, repl_evaluator<BAs...>& re) {
	using namespace boost::log;
	auto option = n | tau_parser::option;
	auto v  = n | tau_parser::option_value;
	auto vt = v | only_child_extractor<BAs...>
		| non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
	auto get_bool_value = [&v, &vt](bool& val) {
		if      (vt == tau_parser::option_value_true) val = true;
		else if (vt == tau_parser::option_value_false) val = false;
		else cout << "error: invalid bool value\n";
		return val;
	};
	static std::map<size_t,	std::function<void()>> setters = {
	{ tau_parser::status_opt,   [&re, &get_bool_value]() {
		get_bool_value(re.opt.status); } },
	{ tau_parser::colors_opt,   [&re, &get_bool_value]() {
		_repl_evaluator::TC.set(get_bool_value(re.opt.colors)); } },
	{ tau_parser::severity_opt, [&re, &v, &vt]() {
		auto sev = v | tau_parser::severity;
		if (!sev.has_value()) {
			cout << "error: invalid severity value\n"; return; }
		auto sev_type = sev
			| only_child_extractor<BAs...>
			| non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>;
		switch (sev_type) {
		case tau_parser::error_sym: re.opt.severity=trivial::error;break;
		case tau_parser::debug_sym:re.opt.severity=trivial::debug;break;
		case tau_parser::trace_sym:re.opt.severity=trivial::trace;break;
		case tau_parser::info_sym: re.opt.severity=trivial::info; break;
		default: cout << "error: invalid severity value\n"; return;
		}
		boost::log::core::get()->set_filter(
			boost::log::trivial::severity >= re.opt.severity);
		}
	}};
	setters[get_opt(option.value())]();
	get_cmd(n, re);
}

template <typename... BAs>
void toggle_cmd(sp_tau_node<BAs...> n, repl_evaluator<BAs...>& re) {
	auto toggle_type = n | tau_parser::bool_option
		| only_child_extractor<BAs...>
		| non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
	switch (toggle_type) {
	case tau_parser::colors_opt: _repl_evaluator::TC.set(re.opt.colors = !re.opt.colors); break;
	case tau_parser::status_opt: re.opt.status = !re.opt.status; break;
	default: cout << ": unknown bool option\n"; _repl_evaluator::error = true; break;
	}
}

void version();
void help(size_t nt = tau_parser::help_sym);

template <typename... BAs>
int eval_cmd(sp_tau_node<BAs...> n, repl_evaluator<BAs...>& re) {
	auto command = n | only_child_extractor<BAs...>;
	auto command_type = command
		| non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
	//std::cout << "eval_cmd: " << command.value() << "\n";
	//print_sp_tau_node_tree<BAs...>(cout << "cmd tree: ", command.value()) << "\n";
	switch (command_type) {
	case tau_parser::quit: return cout << "Quit.\n", 1;
	case tau_parser::help: {
		auto optarg = command | tau_parser::cli_cmd_sym
			| only_child_extractor<BAs...>
			| non_terminal_extractor<BAs...>;
		if (optarg.has_value()) help(optarg.value());
		else help();
		break;
	}
	case tau_parser::version:       version(); break;
	case tau_parser::get:           get_cmd(command.value(), re); break;
	case tau_parser::set:           set_cmd(command.value(), re); break;
	case tau_parser::toggle:        toggle_cmd(command.value(), re); break;
	case tau_parser::list_outputs:  list_outputs(re.m); break;
	case tau_parser::clear_outputs: clear_outputs(re.m); break;
	case tau_parser::print_output:  print_output_cmd(command.value(), re.m); break;
	case tau_parser::normalize:     normalizer_cmd(command.value(), re.m); break;
	case tau_parser::onf:		    onf(command.value()); break; //TODO (HIGH) include var
	case tau_parser::dnf:		  	dnf(command.value()); break;
	case tau_parser::cnf:		  	cnf(command.value()); break;
	case tau_parser::anf:		  	anf(command.value()); break;
	case tau_parser::nnf:		  	nnf(command.value()); break;
	case tau_parser::pnf:		  	pnf(command.value()); break;
	case tau_parser::minterm:	  	minterm(command.value()); break;
	default:
		cout << "Unknown command\n";
		_repl_evaluator::error = true;
		break;
	}
	return 0;
}

template <typename... BAs>
repl_evaluator<BAs...>::repl_evaluator(options opt) : opt(opt) {
	_repl_evaluator::TC.set(opt.colors);
}

template <typename... BAs>
void repl_evaluator<BAs...>::set_repl(repl<repl_evaluator<BAs...>>& r_) {
	r = &r_;
	reprompt(*this);
}

template <typename... BAs>
int repl_evaluator<BAs...>::eval(const std::string& src) {
	auto tau_spec = make_cli<BAs...>(src);
	auto commands = tau_spec || tau_parser::cli_command;
	int quit = 0;
	_repl_evaluator::error = false;
	for (const auto& cmd : commands)
		if (quit = eval_cmd<BAs...>(cmd, *this); quit) break;
	if (!quit) reprompt<BAs...>(*this);
	return quit;
}

// undef terminal color macros valid only for this file
#undef TC_STATUS
#undef TC_STATUS_OUTPUT
#undef TC_ERROR
#undef TC_PROMPT
#undef TC_OUTPUT

} // idni::tau namespace
#endif // __REPL_EVALUATOR_TMPL_H__
