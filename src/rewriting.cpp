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

#include "rewriting.h"

using namespace std;

namespace idni {

template <typename C, typename T>
rewriter<C, T>::graph::graph(const forest<fnode>::graph& g, const forest<fnode>::node& n) {
	map<fnode, node> m;
	auto cb_exit = [&m, this] (const auto& n, const auto&) {
		vector<symbol> ns;
		for (const auto& c : children(n)) ns.emplace_back(m[c]);
		m[n] = add(n.symbol, ns);
	};
	g.traverse(n, cb_exit);
}

template <typename C, typename T>
rewriter<C, T>::node rewriter<C, T>::graph::add(const symbol& l, const forest<node>::nodes& ns) {
	size_t h = hash(l);
	for (const auto& n : ns) h ^= n.hash;
	node key{l, h}; 
	auto it = this->find(key);
	while (it != this->end()) {
		if (it->second.sym == l) return it->second;
		++key.hash;
		it = this->find(key);
	} 
	return (*this)[key] = node{l, h};
}

// unify the pattern against the node and return the unification if unify, 
// or empty unification if not
template <typename C, typename T>
bool rewriter<C, T>::unify(const pattern& p, const node& n, unification& u) {
	if (is_var(p)) return u.emplace(p,n).second;
	else if (is_ignore(p)) return true;
	else if (get_symbol(p) == get_symbol(n)) {
		// check if the children unify
		if (children(p).size() != children(n).size()) return false;
		for (size_t i = 0; i < children(p).size(); ++i)
			if (unify(children(p)[i], children(n)[i], u)) continue;
			else return false;
		return true;
	}
	return false;
}

// apply the unification to the instantiation return the result
template <typename C, typename T>
rewriter<C, T>::node rewriter<C, T>::apply(const unification& u, const instantiation& i) {
	typename forest<node>::nodes ns;
	for (auto& c : children(i)) {
		auto a = apply(u, c);
		ns.push_back(a);
	}
	return add(get_symbol(i), ns);
}

// apply a rule to the directed acyclic graph
template <typename C, typename T>
bool rewriter<C, T>::apply(const rewriter<C, T>::rule& r) {
	map<node, node> m;
	node last;
	auto cb_exit = [&m, &last, &r, this] (const auto& n, const auto&) {
		vector<symbol> ns;
		unification u;
		if (unify(r.first, n, u)) {
			last = apply(u, r.second);
			g.replace(n, last);
		}
	};
	g.traverse(g.root, cb_exit);
	g.root = last;
	return false;
}

} // namespace idni