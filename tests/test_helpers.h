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

#ifndef __TEST_HELPERS_H__
#define __TEST_HELPERS_H__

#include "../src/formula.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

// helper functions
sp_tau_node<Bool> make_statement(const sp_tau_source_node& source) {
	tauify<Bool> tf;
	map_transformer<decltype(tf), sp_tau_source_node, sp_tau_node<Bool>> transform(tf);
	return post_order_traverser<decltype(transform), decltype(all<sp_tau_source_node>),
		sp_node<tau_source_sym>, sp_tau_node<Bool>>(transform, all<sp_tau_source_node>)(source);
}

sp_tau_node<Bool> make_binding(const sp_tau_node<Bool>& statement, const bindings<Bool>& bs) {
	true_predicate<sp_tau_node<Bool>> always;
	name_binder<Bool> nb(bs);
	bind_transformer<decltype(nb), Bool> binder(nb); 
	return post_order_traverser<decltype(binder), decltype(all<sp_tau_node<Bool>>), sp_tau_node<Bool>>(binder, all<sp_tau_node<Bool>>)(statement);
}

#endif // __TEST_HELPERS_H__