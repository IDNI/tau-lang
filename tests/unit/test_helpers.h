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

#include "nso_rr.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

// TODO (LOW) most of this functions could be remove and replace by the standart API

// helper functions
sp_tau_node<Bool> make_statement(const sp_tau_source_node& source) {
	tauify<Bool> tf;
	map_transformer<tauify<Bool>, sp_tau_source_node, sp_tau_node<Bool>> transform(tf);
	return post_order_traverser<
			map_transformer<tauify<Bool>, sp_tau_source_node, sp_tau_node<Bool>>,
			all_t<sp_tau_source_node>,
			sp_node<tau_source_sym>,
			sp_tau_node<Bool>>(
		transform, all<sp_tau_source_node>)(source);
}

sp_tau_node<Bool> make_named_bindings(const sp_tau_node<Bool>& statement, const bindings<Bool>& bs) {
	//true_predicate<sp_tau_node<Bool>> always;
	name_binder<Bool> nb(bs);
	bind_transformer<name_binder<Bool>, Bool> binder(nb);
	return post_order_traverser<
			bind_transformer<name_binder<Bool>, Bool>,
			all_t<sp_tau_node<Bool>>,
			sp_tau_node<Bool>>(
		binder, all<sp_tau_node<Bool>>)(statement);
}

template<typename factory_t>
sp_tau_node<Bool> make_factory_bindings(const sp_tau_node<Bool>& statement, factory_t& factory) {
	//true_predicate<sp_tau_node<Bool>> always;
	factory_binder<factory_t, Bool> fb(factory);
	bind_transformer<factory_binder<factory_t, Bool>, Bool> binder(fb);
	return post_order_traverser<
			bind_transformer<factory_binder<factory_t, Bool>, Bool>,
			all_t<sp_tau_node<Bool>>,
			sp_tau_node<Bool>>(
		binder, all<sp_tau_node<Bool>>)(statement);
}

std::ostream& print_sp_tau_node(std::ostream &os, sp_tau_node<Bool> n, size_t l = 0) {
	os << "{";
	// for (size_t t = 0; t < l; t++) os << " ";
	std::visit(overloaded {
		[&os](tau_source_sym v) { if (v.nt()) os << v.n(); else os << v.t(); },
		[&os](std::variant<Bool> v) { if (auto b = std::get<0>(v); b.b) os << "true"; else os << "false"; },
		[&os](size_t v) { os << v; }
	}, n->value);
	for (auto& d : n->child) print_sp_tau_node(os, d, l + 1);
	os << "}";
	return os;
}

std::ostream& pretty_print_sp_tau_node(std::ostream &os, sp_tau_node<Bool> n, size_t l = 0) {
	// for (size_t t = 0; t < l; t++) os << " ";
	std::visit(overloaded{
		[&os](tau_source_sym v) { if (!v.nt()) os << v.t(); },
		[&os](std::variant<Bool> v) {
			if (auto b = std::get<0>(v); b == true) os << "true";
			else if (auto b = std::get<0>(v); b == false) os << "false";
			else os << "...bdd..."; },
		[&os](size_t v) { os << v; }
	}, n->value);
	for (auto& d : n->child) pretty_print_sp_tau_node(os, d, l + 1);
	return os;
}
#endif // __TEST_HELPERS_H__