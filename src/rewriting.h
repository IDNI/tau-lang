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

// IDEA this is very similar to idni::forest<...>::tree, but it
// also defines equality operators and ordering (important during hashing).
// Both notions could be unified if we keep those operators defined.

// IDEA make make_node a friend function and the constructor private. Right now
// it is public to easy the construction of the tree during testing, but it is 
// not really needed.

// node of a tree. 
template <typename symbol_t>
struct node {
	// equality operators and ordering
	bool operator==(const node& that) const = default;
	bool operator!=(const node& that) const = default;
	auto operator <=> (const node& that) const noexcept {
		if (auto cmp = value <=> that.value; cmp != 0) { return cmp; }
		return std::lexicographical_compare_three_way(
			child.begin(), child.end(),
			that.child.begin(), that.child.end());
	}	

	// the value of the node and pointers to the children, we follow the same 
	// notation as in forest<...>::tree to be able to reuse the code with
	// forest<...>::tree.
	symbol_t value;
	std::vector<std::shared_ptr<node>> child;
};

// pointer to a node
template <typename symbol_t>
using sp_node = std::shared_ptr<node<symbol_t>>;

// node factory method
template <typename symbol_t>
sp_node<symbol_t> make_node(const symbol_t& s, 
		const std::vector<sp_node<symbol_t>>& ns) {
	static std::map<node<symbol_t>, sp_node<symbol_t>> cache;
	node<symbol_t> key{s, ns}; 
	if (auto it = cache.find(key); it != cache.end()) {
		return it->second;
	}
	return cache.emplace(key, std::make_shared<node<symbol_t>>(s, ns)).first->second;
}

// simple function objects to be used as default values for the traversers.
template <typename node_t>
static const auto all = [](const node_t&) { return true; };

template <typename node_t>
using all_t = decltype(all<node_t>);

template <typename node_t>
static const auto none = [](const node_t&) { return false; };

template <typename node_t>
using none_t = decltype(none<node_t>);

template <typename node_t>
static const auto identity = [](const node_t& n) { return n; };

template <typename node_t>
using identity_t = decltype(identity<node_t>);

// visitor that traverse the tree in post-order (avoiding visited nodes).
template <typename wrapped_t, typename predicate_t, typename input_node_t, 
	typename output_node_t = input_node_t>
struct post_order_traverser {

	post_order_traverser(wrapped_t& wrapped, predicate_t& query) : 
		wrapped(wrapped), query(query) {}

	output_node_t operator()(const input_node_t& n) {
		// we kept track of the visited nodes to avoid visiting the same node
		// twice. However, we do not need to keep track of the root node, since
		// it is the one we start from and we will always be visited.
		std::set<input_node_t> visited;
		// if the root node matches the query predicate, we traverse it, otherwise
		// we return the result of apply the wrapped transform to the node.
		return query(n) ? traverse(n, visited) : wrapped(n);
	}

	wrapped_t& wrapped;
	predicate_t& query;

private:
	output_node_t traverse(const input_node_t& n, std::set<input_node_t>& visited) {
		// we traverse the children of the node in post-order, i.e. we visit
		// the children first and then the node itself.
		for (const auto& c : n->child) 
			// we skip already visited nodes and nodes that do not match the
			// query predicate if it is present.
			if (!visited.contains(c) && query(c)) {
				traverse(c, visited);
				// we assume we have no cycles, i.e. there is no way we could
				// visit the same node again down the tree. 
				// thus we can safely add the node to the visited set after
				// visiting it.
				visited.insert(c);
			}
		// finally we apply the wrapped visitor to the node if it is present.
		return wrapped(n);
	}
};

// TODO (MEDIUM) add a post_order_traverser that does not have a wrapped transformer so
// it is faster when dealing with only predicate operations (searches,...) and
// change all the related code.

// visitor that traverse the tree in post-order (repeating visited nodes if necessary).
template <typename wrapped_t, typename predicate_t, typename input_node_t, 
	typename output_node_t = input_node_t>
struct post_order_tree_traverser {

	post_order_tree_traverser(wrapped_t& wrapped, predicate_t& query) : 
		wrapped(wrapped), query(query) {}

	output_node_t operator()(const input_node_t& n) {
		// if the root node matches the query predicate, we traverse it, otherwise
		// we return the result of apply the wrapped transform to the node.
		return query(n) ? traverse(n) : wrapped(n);
	}

	wrapped_t& wrapped;
	predicate_t& query;

private:
	output_node_t traverse(const input_node_t& n) {
		// we traverse the children of the node in post-order, i.e. we visit
		// the children first and then the node itself.
		for (const auto& c : n->child) 
			// we skip already visited nodes and nodes that do not match the
			// query predicate if it is present.
			if (query(c)) traverse(c);
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
		std::vector<output_node_t> child;
		for (const auto& c : n->child) 
			if (auto it = changes.find(c); it != changes.end()) 
				child.push_back(it->second);
		return changes[n] = make_node(wrapped(n->value), child);
	}

	std::map<input_node_t, output_node_t> changes;
	wrapped_t& wrapped;
};

// visitor that produces nodes transformed accordingly to the 
// given transformer. It only works with post order traversals.
template <typename wrapped_t, typename input_node_t, 
	typename output_node_t = input_node_t>
struct map_node_transformer {

	// REVIEW (MEDIUM) check the implementation of this transformer, it seems buggy
	map_node_transformer(wrapped_t& wrapped) : wrapped(wrapped) {}

	output_node_t operator()(const input_node_t& n) {
		auto nn = wrapped(n);
		if (nn != n) { changes[n] == nn; return nn; };
		std::vector<output_node_t> child;
		for (const auto& c : n->child) 
			if (auto it = changes.find(c); it != changes.end()) 
				child.push_back(it->second);
			else child.push_back(c);
		auto nn2 = make_node(nn->value, child);
		return changes[n] = nn2;
	}

	std::map<input_node_t, output_node_t> changes;
	wrapped_t& wrapped;
};

// visitor that produces nodes transformed accordingly to the 
// given transformer. It only works with post order traversals.
//
// TODO (MEDIUM) merge replace and replace_node transformers into one.
template <typename wrapped_t, typename input_node_t, 
	typename output_node_t = input_node_t>
struct replace_node_transformer {

	replace_node_transformer(wrapped_t& wrapped) : wrapped(wrapped) {}

	output_node_t operator()(const input_node_t& n) {
		std::vector<output_node_t> child;
		for (const auto& c : n->child) 
			if (auto it = changes.find(c); it != changes.end()) 
				child.push_back(it->second);
		return changes[n] = wrapped(n->value, child);
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
		auto it = changes.find(n);
		return it != changes.end() ? it->second : replace(n);
	}

	std::map<node_t, node_t>& changes;
private:
	node_t replace(const node_t& n) {
		std::vector<node_t> child;
		for (const auto& c : n->child) 
			if (auto it = changes.find(c); it != changes.end())
				child.push_back(it->second);
			else child.push_back(c);
		return changes[n] = make_node(n->value, child);
	}
};

// visitor that selects top nodes that satisfy a predicate and stores them in the 
// supplied vector. It only works with post order traversals and never produces
// duplicates.
template <typename predicate_t, typename node_t>
struct select_top_predicate {
	select_top_predicate(predicate_t& query, std::vector<node_t>& selected) : 
		query(query), selected(selected) {}

	bool operator()(const node_t& n) {
		if (!query(n)) return true;
		// we return false to avoid visiting the children of the node
		// since we are only interested in the top nodes.
		if (std::find(selected.begin(), selected.end(), n) == selected.end())
			selected.push_back(n);
		return false; 
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

// always true predicate
//
// TODO define a const version of the predicate, move it to rewriter and use
// it in all the code. Review the connection with all predicate and use only 
// one of them.
template<typename node_t>
struct true_predicate {
	
	bool operator()(const node_t& n) const {
		return true;
	}
};

template <typename node_t>
using true_predicate_t = true_predicate<node_t>;

// always false predicate
//
// TODO define a const version of the predicate, move it to rewriter and use
// it in all the code.
template<typename node_t>
struct false_predicate {
	
	bool operator()(const node_t& n) const {
		return false;
	}
};

template <typename node_t>
using false_predicate_t = false_predicate<node_t>;

// disjuction of the wrapped predicates.
//
// IDEA we use combinators to build logical predicates. This could be simplified
// by overloading the operators &&, ||, !, etc.
template <typename l_predicate_t, typename r_predicate_t>
struct and_predicate {

	and_predicate(l_predicate_t& p1, r_predicate_t& p2) : p1(p1), p2(p2) {}

	template<typename node_t>
	bool operator()(const node_t& n) const {
		return p1(n) && p2(n);
	}

	l_predicate_t& p1;
	r_predicate_t& p2;
};

template <typename l_predicate_t, typename r_predicate_t>
using and_predicate_t = and_predicate<l_predicate_t, r_predicate_t>;

// disjuction of the wrapped predicates.
//
// IDEA we use combinators to build logical predicates. This could be simplified
// by overloading the operators &&, ||, !, etc.
template <typename l_predicate_t, typename r_predicate_t>
struct or_predicate {

	or_predicate(l_predicate_t& p1, r_predicate_t& p2) : p1(p1), p2(p2) {}

	template<typename node_t>
	bool operator()(const node_t& n) const {
		return p1(n) || p2(n);
	}

	l_predicate_t& p1;
	r_predicate_t& p2;
};

template <typename l_predicate_t, typename r_predicate_t>
using or_predicate_t = or_predicate<l_predicate_t, r_predicate_t>;

// negation of the wrapped predicate.
//
// IDEA we use combinators to build logical predicates. This could be simplified
// by overloading the operators &&, ||, !, etc.
template <typename predicate_t>
struct neg_predicate {

	neg_predicate(predicate_t& p) : p(p) {}

	template<typename node_t>
	bool operator()(const node_t& n) const {
		return !p(n);
	}

	predicate_t& p;
};

template <typename predicate_t>
using neg_predicate_t = neg_predicate<predicate_t>;

// delete all top nodes that satisfy a predicate.
template <typename predicate_t, typename symbol_t, typename node_t = sp_node<symbol_t>>
node_t trim_top(const node_t& input, predicate_t& query) {
	neg_predicate<predicate_t> neg(query);
	map_transformer<identity_t<symbol_t>, node_t> map(identity<symbol_t>);
	return post_order_traverser<
			map_transformer<identity_t<symbol_t>, node_t>, 
			neg_predicate_t<predicate_t>, 
			node_t>(
		map, neg)(input);
}

// select all top nodes that satisfy a predicate and return them.
template <typename predicate_t, typename node_t>
std::vector<node_t> select_top(const node_t& input, predicate_t& query) {
	std::vector<node_t> selected;
	select_top_predicate<predicate_t, node_t> select(query, selected);
	post_order_traverser<
			identity_t<node_t>, 
			select_top_predicate<predicate_t, node_t>, 
			node_t>(
		identity<node_t>, select)(input);
	return selected;
}

// select all top nodes that satisfy a predicate and return them.
template <typename predicate_t, typename node_t>
std::vector<node_t> select_all(const node_t& input, predicate_t& query) {
	std::vector<node_t> selected;
	select_all_predicate<predicate_t, node_t> select(query, selected);
	post_order_traverser<
			identity_t<node_t>, 
			select_all_predicate<predicate_t, node_t>, 
			node_t>(
		identity<node_t>, select)(input);
	return selected;
}

// find the first node that satisfy a predicate and return it.
template <typename predicate_t, typename node_t>
std::optional<node_t> find_top(const node_t& input, predicate_t& query) {
	std::optional<node_t> found;
	find_top_predicate<predicate_t, node_t> find_top(query, found);
	post_order_traverser<
			identity_t<node_t>, 
			find_top_predicate<predicate_t, node_t>, 
			node_t>(
		identity<node_t>, find_top)(input);
	return found;
}

// true while found is not set (found), it aborts the traversal once found has
// been set.
template <typename node_t>
struct while_not_found_predicate {

	while_not_found_predicate(std::optional<node_t>& found) : found(found) {}

	bool operator()(const node_t& n) const {
		return !found;		
	}

	std::optional<node_t>& found;
};

// to be used in conjunction with while_not_found_predicate. It sets found when
// the predicate is satisfied by a node (set in found).
template <typename predicate_t, typename node_t>
struct find_visitor {
	
	find_visitor(predicate_t& query, std::optional<node_t>& found) : query(query), found(found) {}

	node_t operator()(const node_t& n) const {
		if (!found && query(n)) found = n;
		return n;
	}

	predicate_t& query;
	std::optional<node_t>& found;
};

// find the first node that satisfy a predicate and return it.
template <typename predicate_t, typename node_t>
std::optional<node_t> find_bottom(const node_t& input, predicate_t& query) {
	std::optional<node_t> node;
	while_not_found_predicate<node_t> not_found(node);
	find_visitor<predicate_t, node_t> find(query, node);
	post_order_traverser<
			find_visitor<predicate_t, node_t>, 
			while_not_found_predicate<node_t>, 
			node_t>(
		find, not_found)(input);
	return node;
}

// a environment is a map from captures to tree nodes, it is used
// to keep track of the captures that have been unified and their
// corresponding tree nodes.
template<typename node_t>
using environment = std::map<node_t, node_t>;

// a rule is a pair of a pattern and a substitution. It is used to
// rewrite a tree.
template<typename node_t>
using rule = std::pair<node_t, node_t>;

// this predicate matches when there exists a environment that makes the
// pattern match the node.
//
// TODO create and env in operator() and pass it as a parameter to match, if
// a  match occurs, copy the data from the temp env to the env passed as
// parameter.
//
// IDEA use also a skip predicate to skip subtrees that are not needed in the match.
// It should allow to detects matches in the middle of a tree.
template <typename node_t, typename is_ignore_t, typename is_capture_t> 
struct pattern_matcher {
	using pattern_t = node_t;

	pattern_matcher(pattern_t& pattern, environment<node_t>& env, 
		is_ignore_t& is_ignore, is_capture_t& is_capture): pattern(pattern), 
		env(env), is_ignore(is_ignore), is_capture(is_capture) {}

	bool operator()(const node_t& n) {
		// if we have matched the pattern, we never try again to unify
		if (matched) return false;
		// we clear previous environment attempts
		env.clear();
		// then we try to match the pattern against the node and if the match
		// was successful, we save the node that matched.
		if (match(pattern, n)) matched = { n };
		else env.clear();
		// we continue visiting until we found a match.
		return !matched;
	}

	std::optional<node_t> matched = std::nullopt;
	pattern_t& pattern;
	environment<node_t>& env;
	is_ignore_t& is_ignore;
	is_capture_t& is_capture;

private:
	bool match(const pattern_t& p, const node_t& n) {
		// if we already have captured a node associated to the current capture
		// we check if it is the same as the current node, if it is not, we
		// return false...
		if (is_capture(p)) 
			if (auto it = env.find(p); it != env.end() && it->second != n) 
				return false;
			// ...otherwise we save the current node as the one associated to the
			// current capture and return true.
			else return env.emplace(p, n), true;
		// if the current node is an ignore, we return true.
		else if (is_ignore(p)) return true;
		// otherwise, we check the symbol of the current node and if it is the
		// same as the one of the current pattern, we check if the children
		// match recursively.
		else if (p->value == n->value) {
			if (p->child.size() != n->child.size()) return false;
			for (size_t i = 0; i < p->child.size(); ++i) {
				if (p->child[i] == n->child[i]) continue;
				else if (match(p->child[i], n->child[i])) continue;
				else return false;
			}
			return true;
		}
		return false;
	}
};

// this predicate matches when there exists a environment that makes the
// pattern match the node ignoring the nodes detected as skippable.
//
// TODO create and env in operator() and pass it as a parameter to match, if
// a  match occurs, copy the data from the temp env to the env passed as
// parameter.
template <typename node_t, typename is_ignore_t, typename is_capture_t, typename is_skip_t> 
struct pattern_matcher_with_skip {
	using pattern_t = node_t;

	pattern_matcher_with_skip(pattern_t& pattern, environment<node_t>& env, 
		is_ignore_t& is_ignore, is_capture_t& is_capture, is_skip_t is_skip): 
		pattern(pattern), env(env), is_ignore(is_ignore), 
		is_capture(is_capture), is_skip(is_skip) {}

	bool operator()(const node_t& n) {
		// if we have matched the pattern, we never try again to unify
		if (matched) return false;
		// we clear previous environment attempts
		env.clear();
		// then we try to match the pattern against the node and if the match
		// was successful, we save the node that matched.
		if (match(pattern, n)) matched = { n };
		else env.clear();
		// we continue visiting until we found a match.
		return !matched;
	}

	std::optional<node_t> matched = std::nullopt;
	pattern_t& pattern;
	environment<node_t>& env;
	is_ignore_t& is_ignore;
	is_capture_t& is_capture;
	is_skip_t& is_skip;

private:
	bool match(const pattern_t& p, const node_t& n) {
		// if we already have captured a node associated to the current capture
		// we check if it is the same as the current node, if it is not, we
		// return false...
		if (is_capture(p)) 
			if (auto it = env.find(p); it != env.end()  && it->second != n) 
				return false;
			// ...otherwise we save the current node as the one associated to the
			// current capture and return true.
			else return env.emplace(p, n), true;
		// if the current node is an ignore, we return true.
		else if (is_ignore(p)) return true;
		// otherwise, we check the symbol of the current node and if it is the
		// same as the one of the current pattern, we check if the children
		// match recursively.
		else if (p->value == n->value) {
			auto p_it = p->child.begin();
			auto n_it = n->child.begin();
			while (p_it != p->child.end() && n_it != n->child.end()) {
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
template <typename node_t, typename is_ignore_t, typename is_capture_t> 
node_t apply(rule<node_t>& r, node_t& n, is_ignore_t& i, is_capture_t& c) {
	auto [p , s] = r;
	environment<node_t> u;
	pattern_matcher<node_t, is_ignore_t, is_capture_t> matcher {p, u, i, c};
	return apply(s, n, matcher);
}

// apply a rule to a tree using the predicate to pattern_matcher and skipping
// unnecessary subtrees
template <typename node_t, typename is_ignore_t, typename is_capture_t, 
	typename is_skip_t> 
node_t apply_with_skip(rule<node_t>& r, node_t& n, is_ignore_t& i, is_capture_t& c, is_skip_t& sk) {
	auto [p , s] = r;
	environment<node_t> u;
	pattern_matcher_with_skip<node_t, is_ignore_t, is_capture_t, is_skip_t> 
		matcher {p, u, i, c, sk};
	#ifndef OUTPUT_APPLY_RULES
	return apply(s, n, matcher);
	#else // OUTPUT_APPLY_RULES
	auto nn = apply(s, n, matcher);
	std::cout << "applying rule: " << p << " = " << s << std::endl;
	std::cout << "\tinitial node: " << n << std::endl;
	std::cout << "\tfinal node: " << nn << std::endl;
	return nn;
	#endif // OUTPUT_APPLY_RULES
}

// apply a substitution to a rule according to a given matcher, this method is 
// use internaly by apply and apply with skip.
template <typename node_t, typename matcher_t> 
node_t apply(node_t& s, node_t& n, matcher_t& matcher) {
	// TODO check if this could be improved using a composed transformer
	// that deals with the matcher and the substitution.
	post_order_traverser<identity_t<node_t>, matcher_t, node_t>(identity<node_t>, matcher)(n);
	if (matcher.matched) {
		replace_transformer<node_t> replace {matcher.env};
		auto nn = post_order_traverser<replace_transformer<node_t>,all_t<node_t>, node_t>(replace, all<node_t>)(s);
		environment<node_t> nu { {matcher.matched.value(), nn} };
		replace_transformer<node_t> nreplace {nu};
		return post_order_traverser<replace_transformer<node_t>, all_t<node_t>, node_t>(nreplace, all<node_t>)(n);
	}
	return n;
}

// drop unnecessary information from the parse tree nodes
template <typename parse_symbol_t, typename symbol_t>
auto drop_location = [](const parse_symbol_t& n) -> symbol_t { return n.first; };
template <typename parse_symbol_t, typename symbol_t>
using drop_location_t = decltype(drop_location<parse_symbol_t, symbol_t>);

// make a tree from the given source code string.
template<typename parser_t, typename transformer_t, typename parse_symbol_t, typename symbol_t>
sp_node<symbol_t> make_node_from_string(const transformer_t& transformer, const std::string source) {
	using parse_forest = idni::forest<parse_symbol_t>;
	using parse_tree = typename parse_forest::tree;
	using sp_parse_tree = typename parse_forest::sptree;
	using parse_graph = typename parse_forest::graph;

	sp_parse_tree t;
	static parser_t parser;
	auto f = parser.parse(source.c_str(), source.size());
	// MARK output the error if the parser failed
	// avoiding doctest issues, uncomment for errors
	//if (!f || !parser.found()) {
	//	std::cerr << parser.get_error().to_str(); 
	//}

	auto get_tree = [&f, &t] (auto& g ){
			f->remove_recursive_nodes(g);
			f->remove_binarization(g);
			t = g.extract_trees();
			// MARK output the parsed tree
			// #ifdef OUTPUT_PARSED_TREES
			//t->to_print(std::cout);
			// #endif // OUTPUT_PARSED_TREES
			return false;
		};
	f->extract_graphs(f->root(), get_tree);

	map_transformer<drop_location_t<parse_symbol_t, symbol_t>, 
		sp_parse_tree, sp_node<symbol_t>> transform(drop_location<parse_symbol_t, symbol_t>);
	return post_order_traverser<
			// REVIEW maybe should be transformer instead of drop_location
			map_transformer<drop_location_t<parse_symbol_t, symbol_t>, sp_parse_tree, sp_node<symbol_t>>, 
			all_t<sp_parse_tree>,
			sp_parse_tree, sp_node<symbol_t>>
		(transform, all<sp_parse_tree>)(t);
	}
} // namespace idni::rewriter
#endif // __IDNI__REWRITING_H__
