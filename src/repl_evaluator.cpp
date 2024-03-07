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

gssotc<bdd_binding> normalizer_cmd(const gssotc<bdd_binding>& n) {
	auto wff = n | tau_parser::q_wff | tau_parser::wff;
	if (wff.has_value()) {
		auto result = normalizer<tau_ba<bdd_binding>, bdd_binding>(wff.value());
		std::cout << "normalized: " << result << "\n";
		return result;
	}
	return n;
}



// make a nso_rr from the given tau source and binder.
sp_tau_node<tau_ba<bdd_binding>, bdd_binding> make_cli(const std::string src) {
	auto cli_src = make_tau_source(src, { .start = tau_parser::cli });
	bdd_binding_factory bf;
	tau_factory<bdd_binding_factory, bdd_binding> tbf(bf);
	factory_binder<tau_factory<bdd_binding_factory, bdd_binding>, tau_ba<bdd_binding>, bdd_binding> fb(tbf);
	return make_tau_code<tau_ba<bdd_binding>, bdd_binding>(cli_src);
}

int repl_evaluator::eval(const std::string& src) {
	auto tau_spec = make_cli(src);
	auto command = tau_spec | tau_parser::cli_command | only_child_extractor<tau_ba<bdd_binding>, bdd_binding>;
	auto command_type = command | non_terminal_extractor<tau_ba<bdd_binding>, bdd_binding> | optional_value_extractor<size_t>;
	switch (command_type) {
	case tau_parser::help: help(); break;
	case tau_parser::version: version(); break;
	case tau_parser::quit: return -1;
	case tau_parser::normalize: {
		auto normalized = normalizer_cmd(command.value());
		m.push_back(normalized); break;
	}
	default: cout << "Unknown command\n"; break;
	}
	return 0;
}

} // idni::tau namespace