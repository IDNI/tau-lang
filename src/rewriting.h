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
#include <algorithm>
#include <functional>
#include <vector>
#include <map>
#include <set>
#include <array>
#include <optional>
#include <variant>
#include <compare>

#include "forest.h"
#include "parser.h"
//#include "tree.h"

namespace idni::rewriter {

// node of a tree.
template <typename symbol_t>
struct node {
	// equality operators and ordering
	bool operator==(const node& that) const = default;
	bool operator!=(const node& that) const = default;
	auto operator <=> (const node& that) const noexcept {
		if (auto cmp = sym <=> that.sym; cmp != 0) { return cmp; }
		return std::lexicographical_compare_three_way(
			childs.begin(), childs.end(),
			that.childs.begin(), that.childs.end());
	}	

	// the symbol of the node and pointers to the children
	symbol_t sym;
	std::vector<std::shared_ptr<node>> childs;
};

// pointer to a node
template <typename symbol_t>
using sp_node = std::shared_ptr<node<symbol_t>>;

// it defines a tree of symbols, where each node has a symbol and a list of
// children. The tree is immutable, and it is implemented as a shared pointer
// to the root node.
template <typename node_t> 
struct tree {

	// constructors and assignment operators
	// TODO add initializer list constructor or remove all of them
	// tree(const sp_node& root) : root(root) {}
	// tree(const tree& that) : root(that.root) {}
	// tree(tree&& that) : root(std::move(that.root)) {}
	// tree& operator=(const tree& that) { root = that.root; return *this; }
	// tree& operator=(tree&& that) { root = std::move(that.root); return *this; }

	// ordering operator
	bool operator==(const tree& that) const = default;
	bool operator!=(const tree& that) const = default;
	auto operator <=> (const tree& that) const noexcept { return root <=> that.root; }

	// the root of the tree
	std::shared_ptr<node_t> root;
};

// node factory method
template <typename symbol_t>
::std::pair<sp_node<symbol_t>, bool> make_node(const symbol_t& s, 
		const std::vector<sp_node<symbol_t>>& ns) {
	static std::map<node<symbol_t>, sp_node<symbol_t>> cache;
	node<symbol_t> key{s, ns}; 
	if (auto it = cache.find(key); it != cache.end()) {
		return std::pair(it->second, false);
	}
	cache[key] = std::make_shared<node<symbol_t>>(s, ns);
	return std::pair(cache[key], true);
}

template <typename node_t>
struct identity_transformer {
	node_t operator()(const node_t& n) { return n; }
};

// visitor that traverse the tree in post-order.
template <typename wrapped_t, typename predicate_t>
struct post_order_traverser {

	post_order_traverser(wrapped_t& wrapped, predicate_t& query) : 
		wrapped(wrapped), 
		query(query) {}

	template <typename node_t>
	node_t operator()(const node_t& n) {
		// we kept track of the visited nodes to avoid visiting the same node
		// twice. However, we do not need to keep track of the root node, since
		// it is the one we start from and we will never visit it again (as we
		// always work with trees compacted into a DAG).
		std::set<node_t> v;
		// if the root node matches the query predicate, we traverse it, otherwise

		return query(n) ? traverse(n, v) : n;
	}

	wrapped_t& wrapped;
	predicate_t& query;

private:
	template<typename node_t>
	node_t traverse(const node_t& n, std::set<node_t>& v) {
		// we traverse the children of the node in post-order, i.e. we visit
		// the children first and then the node itself.
		for (const auto& c : n->childs) 
			// we skip already visited nodes and nodes that do not match the
			// query predicate if it is present.
			if (!v.contains(c) && query(c)) {
				traverse(c, v);
				// we assume we have no cycles, i.e. there is no way we could
				// visit the same node again down the tree. 
				// thus we can safely add the node to the visited set after
				// visiting it.
				v.insert(c);
			}
		// finally we apply the wrapped visitor to the node if it is present.
		return wrapped(n);
	}
};

// visitor that produces nodes transformed accordingly to the 
// given transformer. It only works with post order traversals.
template <typename wrapped_t, typename input_node_t, 
	typename output_node_t = input_node_t>
struct map_transformer {

	map_transformer(wrapped_t& wrapped) : wrapped(wrapped) {}

	output_node_t operator()(const input_node_t& n) {
		std::vector<output_node_t> childs;
		for (const auto& c : n->childs) 
			if (changes.contains(c)) childs.push_back(changes[c]);
		auto nn = make_node(wrapped(n->sym), childs).first;
		changes[n] = nn;
		return nn;
	}

	std::map<input_node_t, output_node_t> changes;
	wrapped_t& wrapped;
};

// visitor that produces nodes transformed accordingly to the 
// given transformer. It only works with post order traversals.
template <typename node_t>
struct replace_transformer {
	replace_transformer(std::map<node_t, node_t>& changes) : changes(changes) {}

	node_t operator()(const node_t& n) {
		if (changes.contains(n)) return changes[n];
		return replace(n);
	}

	std::map<node_t, node_t>& changes;
private:
	node_t replace(const node_t& n) {
		std::vector<node_t> childs;
		for (const auto& c : n->childs) 
			if (changes.contains(c)) childs.push_back(changes[c]);
			else childs.push_back(c);
		auto nn = make_node(n->sym, childs).first;
		changes[n] = nn;
		return nn;
	}
};

// visitor that outputs the tree corresponding to a post order traversal.
template <typename wrapped_t, typename symbol_t>
struct to_visitor {

	to_visitor(tree<symbol_t>& output, wrapped_t& wrapped) : output(output), 
		wrapped(wrapped) {}

	template <typename node_t>
	sp_node<symbol_t> operator()(const node_t& n) {
		output.root = wrapped(n);
		return output.root;
	}

	wrapped_t& wrapped;
	tree<symbol_t>& output;
};

// visitor that selects top nodes that satisfy a predicate and stores them in the 
// supplied vector. It only works with post order traversals and never produces
// duplicates.
template <typename predicate_t, typename node_t>
struct select_top_predicate {
	select_top_predicate(predicate_t& query, std::vector<node_t>& selected) : 
		query(query), selected(selected) {}

	bool operator()(const node_t& n) {
		if (query(n)) { 
			// we return false to avoid visiting the children of the node
			// since we are only interested in the top nodes.
			if (std::find(selected.begin(), selected.end(), n) == selected.end())
				selected.push_back(n);
			return false; 
		}
		return true;
	}

	predicate_t& query;
	std::vector<node_t>& selected;
};

// visitor that selects nodes that satisfy a predicate and stores them in the 
// supplied vector.
template <typename predicate_t, typename node_t>
struct select_all_predicate {
	select_all_predicate(predicate_t& query, std::vector<node_t>& selected) : 
		query(query), selected(selected) {}

	bool operator()(const node_t& n) {
		if (query(n)) selected.push_back(n);
		// we always return true to visit all the nodes.
		return true;		
	}

	predicate_t& query;
	std::vector<node_t>& selected;
};

// visitor that selects nodes that satisfy a predicate and stores them in the 
// supplied vector.
template <typename predicate_t, typename node_t>
struct find_top_predicate {
	find_top_predicate(predicate_t& query, std::optional<node_t>& found) : 
	query(query), found(found) {}

	bool operator()(const node_t& n) {
		if (!found && query(n)) found = n;
		return !found;		
	}

	predicate_t& query;
	std::optional<node_t>& found;
};

template<typename node_t>
struct true_predicate {
	
	bool operator()(const node_t& n) {
		return true;
	}
};

template<typename node_t>
struct false_predicate {
	
	bool operator()(const node_t& n) {
		return false;
	}
};

// CHECK we use combinators to build logical predicates. This could be simplified
// by overloading the operators &&, ||, !, etc.
template <typename l_predicate_t, typename r_predicate_t>
struct and_predicate {

	and_predicate(l_predicate_t& p1, r_predicate_t& p2) : p1(p1), p2(p2) {}

	template<typename node_t>
	bool operator()(const node_t& n) {
		return p1(n) && p2(n);
	}

	l_predicate_t& p1;
	r_predicate_t& p2;
};

// TODO we use combinators to build logical predicates. This could be simplified
// by overloading the operators &&, ||, !, etc.
template <typename l_predicate_t, typename r_predicate_t>
struct or_predicate {

	or_predicate(l_predicate_t& p1, r_predicate_t& p2) : p1(p1), p2(p2) {}

	template<typename node_t>
	bool operator()(const node_t& n) {
		return p1(n) || p2(n);
	}

	l_predicate_t& p1;
	r_predicate_t& p2;
};

template <typename predicate_t>
struct neg_predicate {

	neg_predicate(predicate_t& p) : p(p) {}

	template<typename node_t>
	bool operator()(const node_t& n) {
		return !p(n);
	}

	predicate_t& p;
};

// delete all top nodes that satisfy a predicate.
template <typename predicate_t, typename symbol_t>
sp_node<symbol_t> trim_top(const sp_node<symbol_t>& input, predicate_t& query) {
	// TODO try a functional object instead of a lambda to easy the type deduction,
	// or try to reduce the source code and/or the type deduction aids somehow.
	identity_transformer<symbol_t> identity;
	auto neg = neg_predicate(query);
	auto map = map_transformer<decltype(identity), sp_node<symbol_t>>(identity);
	return post_order_traverser(map, neg)(input);
}

template <typename predicate_t, typename symbol_t>
tree<symbol_t> trim_top(const tree<symbol_t>& input, predicate_t& query) { 
	return { trim_top(input.root, query) };
}

// select all top nodes that satisfy a predicate and return them.
template <typename predicate_t, typename symbol_t>
std::vector<sp_node<symbol_t>> select_top(const sp_node<symbol_t>& input, predicate_t& query) {
	// TODO try a functional object instead of a lambda to easy the type deduction,
	// or try to reduce the source code and/or the type deduction aids somehow.
	std::vector<sp_node<symbol_t>> selected;
	identity_transformer<sp_node<symbol_t>> identity;
	auto select = select_top_predicate<predicate_t, sp_node<symbol_t>>(query, selected);
	post_order_traverser(identity, select)(input);
	return selected;
}

// select all top nodes of a tree that satisfy a predicate and return them.
template <typename predicate_t, typename symbol_t, 
	typename tree_t = tree<symbol_t>, typename node_t = tree<symbol_t>::sp_node>
std::vector<node_t> select_top(const tree_t& input, predicate_t& query) {
	return select_top(input.root, query);
}

// select all top nodes that satisfy a predicate and return them.
template <typename predicate_t, typename symbol_t>
std::vector<sp_node<symbol_t>> select_all(const sp_node<symbol_t>& input, predicate_t& query) {
	// TODO try a functional object instead of a lambda to easy the type deduction,
	// or try to reduce the source code and/or the type deduction aids somehow.
	std::vector<sp_node<symbol_t>> selected;
	identity_transformer<sp_node<symbol_t>> identity;
	auto select = select_all_predicate<predicate_t, sp_node<symbol_t>>(query, selected);
	post_order_traverser(identity, select)(input);
	return selected;
}

// select all top nodes of a tree that satisfy a predicate and return them.
template <typename predicate_t, typename symbol_t, 
	typename tree_t = tree<symbol_t>, typename node_t = tree<symbol_t>::sp_node>
std::vector<node_t> select_all(const tree_t& input, predicate_t& query) {
	return select_all(input.root, query);
}

// find the first node that satisfy a predicate and return it.
template <typename predicate_t, typename symbol_t>
std::optional<sp_node<symbol_t>> find_top(const sp_node<symbol_t>& input, 
		predicate_t& query) {
	std::optional<sp_node<symbol_t>> found;
	identity_transformer<sp_node<symbol_t>> identity;
	auto find_top = find_top_predicate<predicate_t, sp_node<symbol_t>>(query, found);
	post_order_traverser(identity, find_top)(input);
	return found;
}

// find the top nodes of a tree that satisfy a predicate and return them.
template <typename predicate_t, typename symbol_t, 
	typename tree_t = tree<symbol_t>, typename node_t = tree<symbol_t>::sp_node>
std::optional<node_t> find_top(const tree_t& input, predicate_t& query) {
	return find_top(input.root, query);
}

// a unification is a map from captures to tree nodes, it is used
// to keep track of the captures that have been unified and their
// corresponding tree nodes.
template<typename node_t>
using unification = std::map<node_t, node_t>;

// a rule is a pair of a pattern and a substitution. It is used to
// rewrite a tree.
template<typename node_t>
using rule = std::pair<node_t, node_t>;

template<typename node_t>
struct non_terminal {

	std::optional<size_t> operator()(const node_t& s) const override {
		return std::nullopt;
	}
};

// this predicate matches when there exists a unification that makes the
// pattern match the node.
template <typename node_t, typename is_ignore_t, typename is_capture_t> 
struct pattern_matcher {
	using pattern_t = node_t;

	pattern_matcher(pattern_t& pattern, unification<node_t>& substitutions, 
		is_ignore_t& is_ignore, is_capture_t& is_capture): pattern(pattern), 
		substitutions(substitutions), is_ignore(is_ignore), is_capture(is_capture) {}

	bool operator()(const node_t& n) {
		// if we have matched the pattern, we never try again to unify
		if (matched) return false;
		// we clear previous unification attempts
		substitutions.clear();
		// then we try to match the pattern against the node and if the match
		// was successful, we save the node that matched.
		if (match(pattern, n)) matched = { n };
		else substitutions.clear();
		// we continue visiting until we found a match.
		return !matched;
	}

	std::optional<node_t> matched = std::nullopt;
	pattern_t& pattern;
	unification<node_t>& substitutions;
	is_ignore_t& is_ignore;
	is_capture_t& is_capture;

private:
	bool match(const pattern_t& p, const node_t& n) {
		// if we already have captured a node associated to the current capture
		// we check if it is the same as the current node, if it is not, we
		// return false...
		if (is_capture(p) && substitutions.contains(p)  && substitutions[p] != n) return false;
		// ...otherwise we save the current node as the one associated to the
		// current capture and return true.
		else if (is_capture(p)) return substitutions.emplace(p, n), true;
		// if the current node is an ignore, we return true.
		else if (is_ignore(p)) return true;
		// otherwise, we check the symbol of the current node and if it is the
		// same as the one of the current pattern, we check if the children
		// match recursively.
		else if (p->sym == n->sym) {
			if (p->childs.size() != n->childs.size()) return false;
			for (size_t i = 0; i < p->childs.size(); ++i) {
				if (p->childs[i] == n->childs[i]) continue;
				else if (match(p->childs[i], n->childs[i])) continue;
				else return false;
			}
			return true;
		}
		return false;
	}
};

// this predicate matches when there exists a unification that makes the
// pattern match the node ignoring the nodes detected as skippable.
template <typename node_t, typename is_ignore_t, typename is_capture_t, typename is_skip_t> 
struct pattern_matcher_with_skip {
	using pattern_t = node_t;

	pattern_matcher_with_skip(pattern_t& pattern, unification<node_t>& substitutions, 
		is_ignore_t& is_ignore, is_capture_t& is_capture, is_skip_t is_skip): 
		pattern(pattern), substitutions(substitutions), is_ignore(is_ignore), 
		is_capture(is_capture), is_skip(is_skip) {}

	bool operator()(const node_t& n) {
		// if we have matched the pattern, we never try again to unify
		if (matched) return false;
		// we clear previous unification attempts
		substitutions.clear();
		// then we try to match the pattern against the node and if the match
		// was successful, we save the node that matched.
		if (match(pattern, n)) matched = { n };
		else substitutions.clear();
		// we continue visiting until we found a match.
		return !matched;
	}

	std::optional<node_t> matched = std::nullopt;
	pattern_t& pattern;
	unification<node_t>& substitutions;
	is_ignore_t& is_ignore;
	is_capture_t& is_capture;
	is_skip_t& is_skip;

private:
	bool match(const pattern_t& p, const node_t& n) {
		// if we already have captured a node associated to the current capture
		// we check if it is the same as the current node, if it is not, we
		// return false...
		if (is_capture(p) && substitutions.contains(p)  && substitutions[p] != n) return false;
		// ...otherwise we save the current node as the one associated to the
		// current capture and return true.
		else if (is_capture(p)) return substitutions.emplace(p, n), true;
		// if the current node is an ignore, we return true.
		else if (is_ignore(p)) return true;
		// otherwise, we check the symbol of the current node and if it is the
		// same as the one of the current pattern, we check if the children
		// match recursively.
		else if (p->sym == n->sym) {
			auto p_it = p->childs.begin();
			auto n_it = n->childs.begin();
			while (p_it != p->childs.end() && n_it != n->childs.end()) {
				if (is_skip(*p_it)) { ++p_it; continue; }
				if (is_skip(*n_it)) { ++n_it; continue; }
				if (*p_it == *n_it) { ++p_it; ++n_it; continue; }
				if (match(*p_it, *n_it)) { ++p_it; ++n_it; continue; }
				return false;
			}
			return true;
		}
		return false;
	}
};

// apply a rule to a tree using the predicate to pattern_matcher.
template <typename symbol_t, typename is_ignore_t, typename is_capture_t> 
sp_node<symbol_t> apply(rule<sp_node<symbol_t>>& r, sp_node<symbol_t>& n, 
		is_ignore_t& i, is_capture_t& c) {
	auto [p , s] = r;
	unification<sp_node<symbol_t>> u;
	pattern_matcher<sp_node<symbol_t>, is_ignore_t, is_capture_t> matcher {p, u, i, c};
	return apply(s, n, matcher);
}

// apply a rule to a tree using the predicate to pattern_matcher and skipping
// unnecessary trees
template <typename symbol_t, typename is_ignore_t, typename is_capture_t, 
	typename is_skip_t> 
sp_node<symbol_t> apply_with_skip(rule<sp_node<symbol_t>>& r, sp_node<symbol_t>& n, 
		is_ignore_t& i, is_capture_t& c, is_skip_t& sk) {
	auto [p , s] = r;
	unification<sp_node<symbol_t>> u;
	pattern_matcher_with_skip<sp_node<symbol_t>, is_ignore_t, is_capture_t, is_skip_t> 
		matcher {p, u, i, c, sk};
	return apply(s, n, matcher);
}

// apply a substitution to a rule according to a given matcher
template <typename symbol_t, typename matcher_t> 
sp_node<symbol_t> apply(sp_node<symbol_t>& s, sp_node<symbol_t>& n, matcher_t& matcher) {
	identity_transformer<sp_node<symbol_t>> identity;
	true_predicate<sp_node<symbol_t>> always;
	post_order_traverser(identity, matcher)(n);
	if (matcher.matched) {
		replace_transformer<sp_node<symbol_t>> replace {matcher.substitutions};
		auto nn = post_order_traverser(replace, always)(s);
		unification<sp_node<symbol_t>> nu { {matcher.matched.value(), nn} };
		replace_transformer<sp_node<symbol_t>> nreplace {nu};
		return post_order_traverser(nreplace, always)(n);
	}
	return n;
}

// apply a substitution to a rule according to a given matcher on a tree
template <typename node_t, typename is_ignore_t, typename is_capture_t>
tree<node_t> apply(const rule<node_t>& r, const tree<node_t>& n, 
		is_ignore_t& is_ignore, is_capture_t& is_capture) {
	return { apply(r, n.root, is_ignore, is_capture) };
}

// apply a substitution to a rule according to a given matcher on a tree
// taking into account the skip predicate
template <typename node_t, typename is_ignore_t, typename is_capture_t, typename is_skip_t>
tree<node_t> apply_with_skip(const rule<node_t>& r, const tree<node_t>& n, 
		is_ignore_t& is_ignore, is_capture_t& is_capture, is_skip_t& is_skip) {
	return { apply(r, n.root, is_ignore, is_capture, is_skip) };
}

// transform a parse tree into a tree of symbols according to the given
// transformer.
//
// The usual type of output_t is idni::lit<char, char> and the usual type of
// input is std::pair<idni::lit<char, char>, std::array<size_t, 2UL>>
template <typename input_t, typename output_t, typename transformer_t>
sp_node<output_t> transform_parse_tree(const typename idni::forest<input_t>::sptree& t, 
		transformer_t& transformer) {
	// traverse recursively the parse tree t and build the new tree applying 
	// the transformation to each node
	std::vector<sp_node<output_t>> childs;
	for (const auto& c : t->child) childs.push_back(transform_parse_tree(c, transformer));
	return make_node<output_t>(transformer(t->value), childs);
}

} // namespace idni::rewriter
#endif // __IDNI__REWRITING_H__
