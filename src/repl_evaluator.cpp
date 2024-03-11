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

#include "defs.h" // for GIT_DESCRIBED
#include "repl_evaluator.h"
#include "parser_instance.h"
#include "normalizer2.h"
#include "nso_rr.h"
#include "bdd_binding.h"
#include "term_colors.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

using namespace idni::rewriter;
using namespace idni::term;
using namespace boost::log;

namespace idni::tau {

term::colors TC;
bool error = false;

#define TC_STATUS        TC.BG_CYAN()
#define TC_STATUS_OUTPUT TC(color::GREEN, color::BG_CYAN, color::BRIGHT)
#define TC_PROMPT        TC(color::WHITE, color::BRIGHT)
#define TC_OUTPUT        TC.GREEN()

void reprompt(repl_evaluator& re) {
	std::stringstream ss;
	if (re.opt.status) {
		ss << TC_STATUS << "[";
		ss << " " << TC_STATUS_OUTPUT
			<< "o:" << re.m.size() << TC.CLEAR() << TC_STATUS;
		if (re.opt.severity != trivial::error)
			ss << " " << to_string(re.opt.severity);
		if (error) ss << " error";
		ss << " ]" << TC.CLEAR() << " ";
	}
	ss << TC_PROMPT << "tau>" << TC.CLEAR() << " ";
	if (re.r) re.r->prompt(ss.str());
}

size_t digits(sp_tau_node<tau_ba<bdd_binding>, bdd_binding> n) {
	return process_digits(n)
		| only_child_extractor<tau_ba<bdd_binding>, bdd_binding>
		| size_t_extractor<tau_ba<bdd_binding>, bdd_binding>
		| optional_value_extractor<size_t>;
}

void version() { cout << "Tau version: " << GIT_DESCRIBED << "\n"; }

// TODO (LOW) write proper help messages
void help(size_t nt = tau_parser::help_sym) {
	switch (nt) {
	case tau_parser::help_sym: cout
		<< "Commands:\n"
		<< "  h, help.        print this help\n"
		<< "  v, version.     print version\n"
		<< "  q, quit, exit   exit the repl\n";
		break;
	case tau_parser::version_sym: cout
		<< "Command v, version prints out current Tau commit id.\n";
		break;
	case tau_parser::quit_sym: cout
		<< "Command q, quit, exit exits the repl.\n";
		break;
	case tau_parser::output_sym: cout
		<< "Command o, output ...\n";
		break;
	case tau_parser::selection_sym: cout
		<< "Command s, selection ...\n";
		break;
	case tau_parser::instantiate_sym: cout
		<< "Command i, instantiate ...\n";
		break;
	case tau_parser::substitute_sym: cout
		<< "Command s, substitute ...\n";
		break;
	case tau_parser::normalize_sym: cout
		<< "Command n, normalize ...\n";
		break;
	case tau_parser::file_sym: cout
		<< "Command r, read ...\n";
		break;
	}
}

void print_output(size_t i, const typename repl_evaluator::outputs& m) {
	cout << TC_OUTPUT << i << TC.CLEAR();
	if (i < m.size()) cout << ": " << m[i];
	else cout << " does not exist";
	cout << "\n";
}

void list_outputs(typename repl_evaluator::outputs& m) {
	if (!m.size()) cout << "no outputs\n";
	else for (size_t i = 0; i < m.size(); i++) print_output(i, m);
}

void clear_outputs(typename repl_evaluator::outputs& m) {
	m.clear();
	cout << "outputs cleared\n";
}

void store_output(typename repl_evaluator::output o,
	typename repl_evaluator::outputs& m)
{
	// do not add into memory if the last memory value is the same ?
	if (m.size() && m.back() == o) return;
	m.push_back(o);
}

// make wff

// make rr_wff

nso<tau_ba<bdd_binding>, bdd_binding> normalizer_cmd(const nso<tau_ba<bdd_binding>, bdd_binding>& n) {
	/*auto type = n
		| only_child_extractor<tau_ba<bdd_binding>, bdd_binding>
		| non_terminal_extractor<tau_ba<bdd_binding>, bdd_binding>
		| optional_value_extractor<size_t>;
	switch(type) {
	case tau_parser::q_wff: {*/
		auto wff = n | tau_parser::q_wff | tau_parser::wff;
		if (wff.has_value()) {
			auto result = normalizer<tau_ba<bdd_binding>, bdd_binding>(wff.value());
			std::cout << "normalized: " << result << "\n";
			return result;
		}
		std::cout << "Unsupported type to normalize.\n"; return n;
	/*}
	case tau_parser::q_nso_rr: {
		auto nso_rr = n | tau_parser::q_nso_rr | optional_value_extractor<nso<tau_ba<bdd_binding>, bdd_binding>>;
		auto result = normalizer<tau_ba<bdd_binding>, bdd_binding>(nso_rr);
		std::cout << "normalized: " << result << "\n";
	return result;
	}
	default: {
		std::cout << "Unsupported type to normalize.\n";
		return n;
	}}*/
}

// make a nso_rr from the given tau source and binder.
sp_tau_node<tau_ba<bdd_binding>, bdd_binding> make_cli(const std::string src) {
	auto cli_src = make_tau_source(src, { .start = tau_parser::cli });
	bdd_binding_factory bf;
	tau_factory<bdd_binding_factory, bdd_binding> tbf(bf);
	factory_binder<tau_factory<bdd_binding_factory, bdd_binding>, tau_ba<bdd_binding>, bdd_binding> fb(tbf);
	return make_tau_code<tau_ba<bdd_binding>, bdd_binding>(cli_src);
}

size_t get_opt(sp_tau_node<tau_ba<bdd_binding>, bdd_binding> n) {
	auto bool_opt = n | tau_parser::bool_option;
	if (bool_opt.has_value()) n = bool_opt.value();
	return n| only_child_extractor<tau_ba<bdd_binding>, bdd_binding>
		| non_terminal_extractor<tau_ba<bdd_binding>, bdd_binding>
		| optional_value_extractor<size_t>;
}

void get_cmd(sp_tau_node<tau_ba<bdd_binding>, bdd_binding> n, repl_evaluator& re) {
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

void set_cmd(sp_tau_node<tau_ba<bdd_binding>, bdd_binding> n, repl_evaluator& re) {
	auto option = n | tau_parser::option;
	auto v  = n | tau_parser::option_value;
	auto vt = v | only_child_extractor<tau_ba<bdd_binding>, bdd_binding>
		| non_terminal_extractor<tau_ba<bdd_binding>, bdd_binding>
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
		TC.set(get_bool_value(re.opt.colors)); } },
	{ tau_parser::severity_opt, [&re, &v, &vt]() {
		auto sev = v | tau_parser::severity;
		if (!sev.has_value()) {
			cout << "error: invalid severity value\n"; return; }
		auto sev_type = sev
			| only_child_extractor<tau_ba<bdd_binding>, bdd_binding>
			| non_terminal_extractor<tau_ba<bdd_binding>, bdd_binding>
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

void toggle_cmd(sp_tau_node<tau_ba<bdd_binding>, bdd_binding> n, repl_evaluator& re) {
	auto toggle_type = n | tau_parser::bool_option
		| only_child_extractor<tau_ba<bdd_binding>, bdd_binding>
		| non_terminal_extractor<tau_ba<bdd_binding>, bdd_binding>
		| optional_value_extractor<size_t>;
	switch (toggle_type) {
	case tau_parser::colors_opt: TC.set(re.opt.colors = !re.opt.colors); break;
	case tau_parser::status_opt: re.opt.status = !re.opt.status; break;
	default: cout << ": unknown bool option\n"; error = true; break;
	}
}

int eval_cmd(sp_tau_node<tau_ba<bdd_binding>, bdd_binding> n, repl_evaluator& re) {
	auto command = n | only_child_extractor<tau_ba<bdd_binding>, bdd_binding>;
	auto command_type = command
		| non_terminal_extractor<tau_ba<bdd_binding>, bdd_binding>
		| optional_value_extractor<size_t>;
	//std::cout << "command.value(): " << command.value() << "\n";
	//DBG(print_sp_tau_node_tree<tau_ba<bdd_binding>, bdd_binding>(cout << "command.value(): ", command.value()) << "\n";)
	switch (command_type) {
	case tau_parser::quit: return cout << "Quit.\n", 1;
	case tau_parser::help: {
		auto optarg = command | tau_parser::cli_cmd_sym
			| only_child_extractor<tau_ba<bdd_binding>, bdd_binding>
			| non_terminal_extractor<tau_ba<bdd_binding>, bdd_binding>;
		if (optarg.has_value())
			//cout << "help: " << optarg.value() << "\n",
			help(optarg.value());
		else help();
		break;
	}
	case tau_parser::version:       version(); break;
	case tau_parser::get:           get_cmd(command.value(), re); break;
	case tau_parser::set:           set_cmd(command.value(), re); break;
	case tau_parser::toggle:        toggle_cmd(command.value(), re); break;
	case tau_parser::list_outputs:  list_outputs(re.m); break;
	case tau_parser::clear_outputs: clear_outputs(re.m); break;
	case tau_parser::print_output: {
		auto out_id = command
			| tau_parser::output
			| tau_parser::output_id
			| optional_value_extractor<
				sp_tau_node<tau_ba<bdd_binding>, bdd_binding>>;
		print_output(digits(out_id), re.m), cout << "\n";
		break;
	}
	case tau_parser::normalize: {
		auto normalized = normalizer_cmd(command.value());
		store_output(normalized, re.m);
		break;
	}
	default:
		cout << "Unknown command\n";
		error = true;
		break;
	}
	return 0;
}

repl_evaluator::repl_evaluator(options opt) : opt(opt) {
	TC.set(opt.colors);
}

void repl_evaluator::set_repl(repl<repl_evaluator>& r_) {
	r = &r_;
	reprompt(*this);
}

int repl_evaluator::eval(const std::string& src) {
	auto tau_spec = make_cli(src);
	auto commands = tau_spec || tau_parser::cli_command;
	int quit = 0;
	error = false;
	for (const auto& cmd : commands)
		if (quit = eval_cmd(cmd, *this); quit) break;
	if (!quit) reprompt(*this);
	return quit;
}

// undef terminal color macros valid only for this file
#undef TC_OUTPUT
#undef TC_STATUS
#undef TC_STATUS_OUTPUT
#undef TC_PROMPT

} // idni::tau namespace