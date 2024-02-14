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

#ifndef __DEBUG_HELPERS_H__
#define __DEBUG_HELPERS_H__

#ifdef DEBUG
template <typename...BAs>
std::ostream& print_sp_tau_node(std::ostream &os, sp_tau_node<BAs...> n, size_t l = 0) {
	os << "{";
	// for (size_t t = 0; t < l; t++) os << " ";
	std::visit(overloaded {
			[&os](tau_source_sym v) { if (v.nt()) os << v.n(); else os << v.t(); },
			[&os](std::variant<BAs...>) {
				os << "...BAs..."; },
			[&os](size_t v) { os << v; }},
		n->value);
	for (auto& d : n->child) print_sp_tau_node(os, d, l + 1);
	os << "}";
	return os;
}
#endif // DEBUG

#endif // __DEBUG_HELPERS_H__