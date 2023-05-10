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

#ifndef __COMBINATORS_H__ // combinators for parsing
#define __COMBINATORS_H__

#include "parser.h"

namespace idni::tree::matcher {

	/* This code defines a set of functions that can be used to filter
	the nodes in a forest. The current function always returns true. 
	The child function returns true if the node has a child at the
	specified index and the function passed to the constructor returns
	true for that child. The _and function returns true if both functions
	passed to the constructor return true for the node. The _or function
	returns true if either function passed to the constructor returns true
	for the node. The _not function returns true if the function passed 
	to the constructor returns false for the node. The _xor function 
	returns true if exactly one of the functions passed to the constructor
	returns true for the node. */

	template <typename NodeT>
	struct current: std::function<bool(forest<NodeT>::tree)> {

		current() {}

		bool operator()(forest<NodeT>::tree tree) {
			return true;
		}
	};

	template <typename NodeT>
	struct child: std::function<bool(forest<NodeT>::tree)> {
		size_t idx;
		std::function<bool(forest<NodeT>::tree)> f;

		child(size_t idx, std::function<bool(forest<NodeT>::tree)> f): idx(idx), f(f) {}

		bool operator()(forest<NodeT>::tree tree) {
			return tree.children.size() > idx && f(tree.children[idx]);
		}
	};

	template <typename NodeT>
	struct childs: std::function<bool(forest<NodeT>::tree)> {
		std::vector<std::function<bool(forest<NodeT>::tree)>> cf;

		childs(std::function<bool(forest<NodeT>::tree)>...cf): cf(cf) {}

		bool operator()(forest<NodeT>::tree tree) {
			for (auto f : cf) {
				if (!f(tree)) return false;
			}
			return true;
		}
	};

	template <typename NodeT>
	struct _and: std::function<bool(forest<NodeT>::tree)> {
		std::function<bool(forest<NodeT>::tree)> f2;
		std::function<bool(forest<NodeT>::tree)> f1;

		_and(std::function<bool(forest<NodeT>::tree)> f1, std::function<bool(forest<NodeT>::tree)> f2): f1(f1), f2(f2) {}

		bool operator()(forest<NodeT>::tree tree) {
			return f1(tree) && f2(tree);
		}
	};

	template <typename NodeT>
	struct _or: std::function<bool(forest<NodeT>::tree)> {
		std::function<bool(forest<NodeT>::tree)> f2;
		std::function<bool(forest<NodeT>::tree)> f1;

		_or(std::function<bool(forest<NodeT>::tree)> f1, std::function<bool(forest<NodeT>::tree)> f2): f1(f1), f2(f2) {}

		bool operator()(forest<NodeT>::tree tree) {
			return f1(tree) || f2(tree);
		}
	};

	template <typename NodeT>
	struct _not: std::function<bool(forest<NodeT>::tree)> {
		std::function<bool(forest<NodeT>::tree)> f;

		_not(std::function<bool(forest<NodeT>::tree)> f): f(f) {}

		bool operator()(forest<NodeT>::tree tree) {
			return !f(tree);
		}
	};

	template <typename NodeT>
	struct _xor: std::function<bool(forest<NodeT>::tree)> {
		std::function<bool(forest<NodeT>::tree)> f2;
		std::function<bool(forest<NodeT>::tree)> f1;

		_xor(std::function<bool(forest<NodeT>::tree)> f1, std::function<bool(forest<NodeT>::tree)> f2): f1(f1), f2(f2) {}

		bool operator()(forest<NodeT>::tree tree) {
			return f1(tree) ^ f2(tree);
		}
	};



} // namespace idni::tree::matcher

namespace idni::tree::action {

} // namespace idni::tree::action
#endif // __COMBINATORS_H__
