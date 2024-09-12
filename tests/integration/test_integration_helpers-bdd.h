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

#ifndef __TEST_INTEGRATION_HELPERS_BDD_H__
#define __TEST_INTEGRATION_HELPERS_BDD_H__

#include "../src/nso_rr.h"
#include "../src/babdd.h"
#include "../src/nso_rr.h"
#include "../src/bdd_handle.h"
#include "../src/bool_ba.h"
#include "../src/normalizer.h"
#include "dict.h"


using namespace idni::rewriter;
using namespace idni::tau;

using bdd_test = hbdd<Bool>;

std::ostream& print_sp_tau_node(std::ostream &os, sp_tau_node<bdd_test> n, size_t l = 0) {
	os << "{";
	// for (size_t t = 0; t < l; t++) os << " ";
	std::visit(overloaded{
		[&os](tau_source_sym v) { if (v.nt()) os << v.n(); else os << v.t(); },
		[&os](std::variant<bdd_test> v) {
			if (auto b = std::get<0>(v); b == true) os << "true";
			else if (auto b = std::get<0>(v); b == false) os << "false";
			else os << "...bdd..."; }
	}, n->value);
	for (auto& d : n->child) print_sp_tau_node(os, d, l + 1);
	os << "}";
	return os;
}

std::ostream& pretty_print_sp_tau_node(std::ostream &os, sp_tau_node<bdd_test> n, size_t l = 0) {
	// for (size_t t = 0; t < l; t++) os << " ";
	std::visit(overloaded{
		[&os](tau_source_sym v) { if (!v.nt()) os << v.t(); },
		[&os](std::variant<bdd_test> v) {
			if (auto b = std::get<0>(v); b == true) os << "true";
			else if (auto b = std::get<0>(v); b == false) os << "false";
			else os << "...bdd..."; }
	}, n->value);
	for (auto& d : n->child) pretty_print_sp_tau_node(os, d, l + 1);
	return os;
}

#endif // __TEST_INTEGRATION_HELPERS_BDD_H__