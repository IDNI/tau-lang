// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __DEBUG_HELPERS_H__
#define __DEBUG_HELPERS_H__

namespace idni::tau {
#ifdef DEBUG

template <typename node_t>
std::ostream& operator<<(std::ostream &os,
	std::map<rewriter::sp_node<node_t>, rewriter::sp_node<node_t>> m)
{
	os << "{";
	for (auto& [k, v] : m) os << " {" << k << " <- " << v << "} ";
	return os << "}\n";
}

template <typename node_t>
std::ostream& print_sp_tau_source_node_tree(std::ostream &os,
	rewriter::sp_node<node_t> n, size_t l = 0)
{
	auto indent = [&os, &l]() { for (size_t t = 0; t < l; t++) os << "\t";};
	auto& v = n->value;
	indent();
	if (v.nt()) os << tau_parser::instance()
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
		[&os, &indent](tau_source_sym v) {
			indent();
			if (v.nt()) os << tau_parser::instance()
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
std::ostream& ptree(std::ostream &os, rewriter::sp_node<node_t> n, size_t l = 0) {
	return print_sp_tau_source_node_tree<node_t>(os, n, l);
}

template <typename... BAs>
std::ostream& ptree(std::ostream &os, sp_tau_node<BAs...> n, size_t l = 0) {
	return print_sp_tau_node_tree<BAs...>(os, n, l);
}

template <typename node_t>
std::string ptree(rewriter::sp_node<node_t> n, size_t l = 0) {
	std::stringstream ss;
	print_sp_tau_source_node_tree<node_t>(ss, n, l);
	return ss.str();
}

template <typename... BAs>
std::string ptree(sp_tau_node<BAs...> n, size_t l = 0) {
	std::stringstream ss;
	print_sp_tau_node_tree<BAs...>(ss, n, l);
	return ss.str();
}

#endif // DEBUG
} // namespace idni::tau

#endif // __DEBUG_HELPERS_H__