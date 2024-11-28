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

#ifndef __TEST_INTEGRATION_HELPERS_SBF_H__
#define __TEST_INTEGRATION_HELPERS_SBF_H__

#include "../src/nso_rr.h"
#include "../src/babdd.h"
#include "../src/nso_rr.h"
#include "../src/bdd_handle.h"
#include "../src/sbf_ba.h"
#include "../src/bool_ba.h"
#include "../src/normalizer.h"
#include "dict.h"

using namespace idni::rewriter;
using namespace idni::tau;

rr<nso<sbf_ba>> sbf_make_nso_rr(const std::string& src) {
	auto sample_src = make_tau_source(src);
	return make_nso_rr_using_factory<sbf_ba>(sample_src).value();
}

nso<sbf_ba> sbf_make_nso(const std::string& src) {
	return sbf_make_nso_rr(src).main;
}

std::ostream& print_sp_tau_node(std::ostream &os, sp_tau_node<sbf_ba> n, size_t l = 0) {
	os << "{";
	// for (size_t t = 0; t < l; t++) os << " ";
	std::visit(overloaded{
		[&os](tau_source_sym v) { if (v.nt()) os << v.n(); else os << v.t(); },
		[&os](std::variant<sbf_ba> v) {
			if (auto b = std::get<0>(v); b == true) os << "true";
			else if (auto b = std::get<0>(v); b == false) os << "false";
			else os << "...sbf..."; }
	}, n->value);
	for (auto& d : n->child) print_sp_tau_node(os, d, l + 1);
	os << "}";
	return os;
}

std::ostream& pretty_print_sp_tau_node(std::ostream &os, sp_tau_node<sbf_ba> n, size_t l = 0) {
	// for (size_t t = 0; t < l; t++) os << " ";
	std::visit(overloaded{
		[&os](tau_source_sym v) { if (!v.nt()) os << v.t(); },
		[&os](std::variant<sbf_ba> v) {
			if (auto b = std::get<0>(v); b == true) os << "true";
			else if (auto b = std::get<0>(v); b == false) os << "false";
			else os << "...sbf..."; }
	}, n->value);
	for (auto& d : n->child) pretty_print_sp_tau_node(os, d, l + 1);
	return os;
}

#endif // __TEST_INTEGRATION_HELPERS_SBF_H__