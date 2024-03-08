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

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace idni::tau {

using namespace idni::rewriter;

void version() { cout << "\nTau version: " << GIT_DESCRIBED << "\n"; }

// TODO (LOW) write proper help messages
void help(size_t nt = tau_parser::help_sym) {
	cout << "\n";
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

int repl_evaluator::eval(const std::string& src) {
	auto cli_src = make_tau_source(src, { .start = tau_parser::cli });
	auto cli_code = make_tau_code<tau_ba<bdd_binding>, bdd_binding>(cli_src);
	auto command = cli_code | tau_parser::cli_command | only_child_extractor<tau_ba<bdd_binding>, bdd_binding>;
	auto command_type = command | non_terminal_extractor<tau_ba<bdd_binding>, bdd_binding> | optional_value_extractor<size_t>;
	switch (command_type) {
	case tau_parser::help: help(); break;
	case tau_parser::version: version(); break;
	case tau_parser::quit: return -1;
	case tau_parser::normalize: {
		auto normalized = normalizer_cmd(command.value());
		m.push_back(normalized); break;
	}
	case tau_parser::toggle_debug: {
	    boost::log::core::get()->set_filter	(
			boost::log::trivial::severity >= boost::log::trivial::debug
		); break;
	}
	case tau_parser::toggle_trace: {
		boost::log::core::get()->set_filter	(
			boost::log::trivial::severity >= boost::log::trivial::trace
		); break;
	}
	case tau_parser::toggle_info: {
		boost::log::core::get()->set_filter	(
			boost::log::trivial::severity >= boost::log::trivial::info
		); break;
	}
	default: cout << "Unknown command\n"; break;
	}
	return 0;
}

} // idni::tau namespace