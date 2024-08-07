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

#ifndef __TEST_INTEGRATION_HELPERS_TAU_H__
#define __TEST_INTEGRATION_HELPERS_TAU_H__

#include "../src/nso_rr.h"
#include "../src/babdd.h"
#include "../src/bdd_handle.h"
#include "../src/bool_ba.h"
#include "../src/normalizer.h"
#include "../src/tau.h"
#include "dict.h"


using namespace idni::rewriter;
using namespace idni::tau;

using bdd_test = hbdd<Bool>;

struct bdd_test_factory {

	sp_tau_node<tau_ba<bdd_test>, bdd_test> build(const std::string type_name, const sp_tau_node<tau_ba<bdd_test>, bdd_test>& n) {
		if (type_name != "bdd") return n;
		std::string var = make_string<
			tau_node_terminal_extractor_t<tau_ba<bdd_test>, bdd_test>,
			sp_tau_node<tau_ba<bdd_test>, bdd_test>>(
				tau_node_terminal_extractor<tau_ba<bdd_test>, bdd_test>, n);
		if (auto cn = cache.find(var); cn != cache.end()) return cn->second;
		// Trim whitespaces from var
		auto is_not_space = [](char c) {return !isspace(c);};
		auto var_trim = var | ranges::views::filter(is_not_space);
		var = {var_trim.begin(), var_trim.end()};
		// Make sure that variable name is saved in dict.h for printing
		int v = dict(var);
		auto ref = bdd_handle<Bool>::bit(true, v);
		tau_sym<tau_ba<bdd_test>, bdd_test> ts(ref);
		auto nn =  make_node<tau_sym<tau_ba<bdd_test>, bdd_test>>(ts, {});
		return cache.emplace(var, nn).first->second;
	}

	std::map<std::string, sp_tau_node<tau_ba<bdd_test>, bdd_test>> cache;
};

nso<tau_ba<bdd_test>, bdd_test> normalize_test_tau(const char* src) {
	bdd_test_factory bf;
	tau_factory<bdd_test_factory, bdd_test> fb(bf);
	rr<nso<tau_ba<bdd_test>, bdd_test>> nso_rr = make_nso_rr_using_factory<
			tau_factory<bdd_test_factory, bdd_test>, tau_ba<bdd_test>, bdd_test>(src, fb);
	return normalizer<tau_ba<bdd_test>, bdd_test>(nso_rr);
}

tau_ba<bdd_test> get_tau_ba(const char* src) {
	bdd_test_factory bf;
	tau_factory<bdd_test_factory, bdd_test> tf(bf);
	rr<nso<tau_ba<bdd_test>, bdd_test>> nso_rr = make_nso_rr_using_factory<
			tau_factory<bdd_test_factory, bdd_test>,
			tau_ba<bdd_test>, bdd_test>(src, tf);
	auto value = nso_rr.main
		| tau_parser::bf_eq
		| tau_parser::bf
		| tau_parser::bf_constant
		| tau_parser::constant
		| only_child_extractor<tau_ba<bdd_test>, bdd_test>;
	return get<tau_ba<bdd_test>>(get<variant<
			tau_ba<bdd_test>, bdd_test>>(value.value()->value));
}

#endif // __TEST_INTEGRATION_HELPERS_TAU_H__