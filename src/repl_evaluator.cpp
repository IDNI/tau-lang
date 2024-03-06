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


struct bdd_binding_factory {

	sp_tau_node<bdd_binding> build(const std::string type_name, const sp_tau_node<bdd_binding>& n) {
		if (type_name != "bdd") return n;
		auto source = n | tau_parser::source_binding | tau_parser::source | optional_value_extractor<sp_tau_node<bdd_binding>>;
		std::string var = make_string_with_skip<
			tau_node_terminal_extractor_t<bdd_binding>,
			not_whitespace_predicate_t<bdd_binding>,
			sp_tau_node<bdd_binding>>(
				tau_node_terminal_extractor<bdd_binding>,
				not_whitespace_predicate<bdd_binding>, source);
		if (auto cn = cache.find(var); cn != cache.end()) return cn->second;
		auto ref = bdd<Bool>::bit(index++);
		auto nn =  make_node<tau_sym<bdd_binding>>(bdd_handle<Bool>::get(ref), {});
		return cache.emplace(var, nn).first->second;
	}

	size_t index = 0;
	std::map<std::string, sp_tau_node<bdd_binding>> cache;
};

int repl_evaluator::eval(const std::string& src) {
	auto tau_src = make_tau_source(src);
	bdd_binding_factory bf;
	factory_binder<bdd_binding_factory, bdd_binding> fb(bf);
	auto tau_spec = make_nso_rr_using_factory<factory_binder<bdd_binding_factory, bdd_binding>, bdd_binding>(tau_src, fb);
	return 0;
}

} // idni::tau namespace