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

namespace idni::tau {
#ifdef DEBUG

template <typename node_t>
std::ostream& print_sp_tau_source_node_tree(std::ostream &os, sp_node<node_t> n,
	size_t l = 0)
{
	auto indent = [&os, &l]() { for (size_t t = 0; t < l; t++) os << "\t";};
	auto& v = n->value;
	indent();
	if (v.nt()) os << parser_instance<tau_parser>()
		.name(v.n()) << "(" << v.n() << ")";
	else if (v.is_null()) os << "null";
	else os << v.t();
	if (n->child.size()) os << " {\n";
	for (auto& c : n->child) print_sp_tau_source_node_tree<node_t>(os, c, l + 1);
	if (n->child.size()) indent(), os << "}";
	return os << "\n";
}

// print the tree of tau nodes for general debugging
template <typename... BAs>
std::ostream& print_sp_tau_node_tree(std::ostream &os, sp_tau_node<BAs...> n,
	size_t l = 0)
{
	auto indent = [&os, &l]() { for (size_t t = 0; t < l; t++) os << "\t";};
	std::visit(overloaded{
		[&os, &ws, &enter, &indent](tau_source_sym v) {
			indent();
			if (v.nt()) os << parser_instance<tau_parser>()
				.name(v.n()) << "(" << v.n() << ")";
			else if (v.is_null()) os << "null";
			else os << v.t();
		},
		[&os, &indent](const auto& v) { indent(), os << v; }
	}, n->value);
	if (n->child.size()) os << " {\n";
	for (auto& c : n->child) print_sp_tau_node_tree<BAs...>(os, c, l + 1);
	if (n->child.size()) indent(), os << "}";
	return os << "\n";
}

template <typename node_t>
std::ostream& ptree(std::ostream &os, sp_node<node_t> n, size_t l = 0) {
	return print_sp_tau_source_node_tree<node_t>(os, n, l);
}

template <typename... BAs>
std::ostream& ptree(std::ostream &os, sp_tau_node<BAs...> n, size_t l = 0) {
	return print_sp_tau_node_tree<BAs...>(os, n, l);
}


#endif // DEBUG
} // namespace idni::tau

#endif // __DEBUG_HELPERS_H__