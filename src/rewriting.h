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

#ifndef __REWRITING_H__
#define __REWRITING_H__

#include <memory>
#include <utility>
#include <functional>
#include <map>

#include "forest.h"
#include "parser.h"

namespace idni {

template <typename C = char, typename T = C>
struct rewriter {

	// literals from the parser
	using symbol = lit<C, T>;
	using fnode = forest<symbol>::node;

	// TODO untill proper integration with Earley parser, we use the following
	// struct to pass the options to the rewriting constructor.
	struct options {
		size_t variable;
		size_t ignore;
	};

	// Node of a rewritable graph. THe hash field is used to compare nodes and
	// contains the hash of the node symbol and the hash of the symbols of the
	// children (ala Merkle, see https://en.wikipedia.org/wiki/Merkle_tree).
	struct node {
		symbol sym;
		size_t hash;

		bool operator==(const node& that) const = default;
		bool operator!=(const node& that) const = default;
		auto operator<=>(const node& that) const { return *this.hash <=> that.hash; }
	};

	using pattern = node;
	using instantiation = node;
	using rule = std::pair<pattern, instantiation>;

	// to be move to cpp file
	using capture = node;
	using variable = node;
	using unification = std::map<variable, capture>;

	struct graph : forest<node>::graph {

		graph(const forest<fnode>::graph& g, const forest<fnode>::node& n);
		graph() = default;

		node add(const symbol& l, const forest<node>::nodes& ns);
	};

	bool apply(const rule& r);
	void traverse(const std::function<void(const node&)>& cb) { g.traverse(cb); }
	// void find(const function<bool(const node&)>& cb);

	rewriter(const options& o) : opts(o) { }
	rewriter(const forest<symbol>::graph& g, const options& o) : opts(o), g(g, g.root) { }
	rewriter(const forest<symbol>::graph& g, const forest<symbol>::node& n, const options& o) : opts(o), g(g, n) { }

	// main helper methods
	bool unify(const pattern& p, const node& n, unification& u);
	node apply(const unification& u, const instantiation& i);

	// helper methods
	bool is_nonterminal(const typename rewriter<C, T>::node& n) { return n.symbol->first.nt(); }
	bool is_terminal(const typename rewriter<C, T>::node& n) { return n.symbol->first.t(); }
	bool is_var(const node& n) { return is_nonterminal(n) && n.symbol->first.n() == opts.variable; }
	bool is_ignore(const node& n) { return is_nonterminal(n) && n.symbol->first.n() == opts.ignore; }
	auto get_symbol(const node& n) { return n.symbol->first; }
	auto children(const node& n) { return g[0][n]; }

	options opts;
	graph g;
};

} // namespace idni
#endif // __IDNI__REWRITING_H__
