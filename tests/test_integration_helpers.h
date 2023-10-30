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

#ifndef __TEST_INTEGRATION_HELPERS_H__
#define __TEST_INTEGRATION_HELPERS_H__

#include "../src/formula.h"

using namespace idni::rewriter;
using namespace idni::tau;

struct bdd_factory {
	
	sp_tau_node<hbdd<Bool>> build(const sp_tau_node<hbdd<Bool>>& type, const sp_tau_node<hbdd<Bool>> n) {
		auto string_type = make_string_with_skip<
			decltype(tau_node_terminal_extractor<hbdd<Bool>>),
			decltype(not_whitespace_predicate<hbdd<Bool>>), 
			sp_tau_node<hbdd<Bool>>>(tau_node_terminal_extractor<hbdd<Bool>>, 
			not_whitespace_predicate<hbdd<Bool>>, type);
		if (string_type != "bdd") return n;
		auto var = make_string_with_skip<
			decltype(tau_node_terminal_extractor<hbdd<Bool>>),
			decltype(not_whitespace_predicate<hbdd<Bool>>), 
			sp_tau_node<hbdd<Bool>>>(tau_node_terminal_extractor<hbdd<Bool>>, 
			not_whitespace_predicate<hbdd<Bool>>, type);
		if (auto cn = cache.find(var); cn != cache.end()) return cn->second;
		auto nn =  make_node<tau_sym<hbdd<Bool>>>(bdd_handle<Bool>::bit(true, index++), {});
		return cache.emplace(var, nn).first->second;
	}

	static size_t index;
	static std::map<std::string, sp_tau_node<hbdd<Bool>>> cache;
};

#endif // __TEST_INTEGRATION_HELPERS_H__