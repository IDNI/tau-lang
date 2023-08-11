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

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <map>
#include <variant>
#include <string>
#include <vector>
#include <memory>

//#include "tree.h"
#include "bool.h"
#include "rewriting.h"
#include "../parser/tau_parser.generated.h"

using namespace idni::rewriter;

namespace idni::tau {

// tau_source_node is the type of nodes we use to represent get from parsing tau language
using tau_source_sym = idni::lit<char, char>;
using tau_source_node = node<idni::lit<char, char>>;
using sp_tau_source_node = sp_node<idni::lit<char, char>>;

// node type for the tau language related programs, libraries and 
// specifications trees.
template <typename... BAs>
using tau_sym = std::variant<tau_source_sym, std::variant<BAs...>>;
template <typename... BAs>
using tau_node = node<tau_sym<BAs...>>;
template <typename... BAs>
using sp_tau_node = sp_node<tau_sym<BAs...>>;;
template <typename... BAs>
using tau_rule = rule<sp_node<tau_sym<BAs...>>>;

// defines a vector of rules in the tau language, the order is important as it defines
// the order of the rules in the rewriting process of the tau language.
template <typename... BAs>
using rules = std::vector<tau_rule<BAs...>>;
// defines the main statement of a tau formula.
template <typename... BAs>
using statement = sp_tau_node<BAs...>;

// a library is a set of rules to be applied in the rewriting process of the tau
// language, the order of the rules is important.
template <typename... BAs>
struct library {

	library(rules<BAs...>& system): system(system) {};

	rules<BAs...> system;
};

// bindings map tau_source constants (strings) into elements of the boolean algebras.
template<typename... BAs>
using bindings = std::map<std::string, std::variant<BAs...>>;

// a formula is a set of rules and a main, the boolean algebra constants 
// (unless '0' or '1') are uninstantiated.
template<typename... BAs>
struct formula {

	formula(rules<BAs...>& rec_relations, statement<BAs...>& main): rec_relations(rec_relations), main(main) {};

	rules<BAs...> rec_relations;
	statement<BAs...> main;
};

// a formula is a set of rules and a main, the boolean algebra constants 
// (unless '0' or '1') are uninstantiated.
template<typename... BAs>
struct tau {

	// logical operators on tau specs, dummy implementation for now.
	tau operator&(tau const& that) const { 
		// TODO: implement in the future
		return tau(); 
	}
	tau operator|(tau const& that) const {
		// TODO: implement in the future
		return tau(); 
	}
	tau operator^(tau const& that) const {
		// TODO: implement in the future
		return tau(); 
	}
	tau operator~() const {
		// TODO: implement in the future
		return tau(); 
	}
};

// TODO:LOW implementations details to be moved to a separate file
// TODO:LOW rename to is_non_terminal_predicate
template<typename... BAs>
struct is_non_terminal {

	size_t operator()(const sp_tau_node<BAs...>& n) {
		return n->value.index() == 0 // std::holds_alternative<tau_sym>(*n) 
			&& get<0>(*n).nt();
	}
};

// check if the node is the given non-terminal
template <size_t nt, typename...BAs>
auto is_tau_node = [](const sp_tau_node<BAs...> n) { return n->value.index() == 0 // std::holds_alternative<tau_sym>(*n) 
			&& get<0>(n->value).nt() 
			&& get<0>(n->value).n() == nt;
 };

// check if the node is the given non-terminal
template <size_t nt>
auto is_tau_source = [](const sp_tau_source_node& n) { return n->value() && n->value.n() == nt; };

// gets the top nodes of the given non terminal type
template <size_t nt, typename... BAs>
std::vector<sp_tau_node<BAs...>> get(const sp_tau_node<BAs...>& n) {
	return select_top(n, is_tau_node<nt, BAs...>);
}

// gets the children of the top nodes of the given non terminal type
template <size_t nt, typename... BAs>
std::vector<sp_tau_node<BAs...>> get_children(const sp_tau_node<BAs...>& n) {
	std::vector<sp_tau_node<BAs...>> result;
	for (auto& c : select_top(n, is_tau_node<nt, BAs...>)) result.push_back(c->child[0]);
	return result;
}

// gets the only child of the top nodes of the given non terminal type
template <size_t nt, typename... BAs>
std::optional<sp_tau_node<BAs...>> get_child(const sp_tau_node<BAs...>& n) {
	auto children = get_children<nt, BAs...>(n);
	return (children.size() == 1) ? std::optional(std::move(children[0])) : std::nullopt;
}

// gets a pair of children of the top nodes of the corresponding non terminal type.
// Its mainly used to get the left and right children of a binary operator,
// equality, implication, etc.
template<size_t nt_l, size_t nt_r, typename... BAs>
std::optional<std::pair<sp_tau_node<BAs...>, sp_tau_node<BAs...>>> get_pair(const sp_tau_node<BAs...>& n) {
	auto l = get_child<nt_l, BAs...>(n);
	auto r = get_child<nt_r, BAs...>(n);
	return (l && r) ? std::optional(std::move(std::make_pair(*l, *r))) : std::nullopt;
}

// apply the given callback if the value of the node is a callback
//
// TODO convert to a const static predicate and change all the code accordingly
template <typename... BAs>
struct callback_applier {

	sp_tau_node<BAs...> operator()(const sp_tau_node<BAs...>& n) {
		if (!is_callback(n)) return n;
		auto os = get<tau_parser::bf_cb_arg>(n);
		switch (n->get().nt()) {
			case ::tau_parser::bf_and_cb: return make_shared<tau_node<BAs...>>(*os[0] & *os[1]);
			case ::tau_parser::bf_or_cb: return make_shared<tau_node<BAs...>>(*os[0] | *os[1]);
			case ::tau_parser::bf_neg_cb: return make_shared<tau_node<BAs...>>(~*os[0]);
			case ::tau_parser::bf_xor_cb: return make_shared<tau_node<BAs...>>(*os[0] ^ *os[1]);
			case ::tau_parser::bf_subs_cb: return apply_subs<BAs...>(n);
			default: return n;
		}
	}

private:
	bool is_callback(const sp_tau_node<BAs...>& n) {
		return n->value.index() == 0 // std::holds_alternative<tau_sym>(*n) 
			&& get<0>(n->value).nt() >= ::tau_parser::bf_and_cb
			&& get<0>(n->value).nt() <= ::tau_parser::bf_subs_cb;
	}

	sp_tau_node<BAs...> apply_subs(const sp_tau_node<BAs...>& n) {
		auto os = get<tau_parser::bf_cb_arg>(n);
		std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> m;
		m[os[0]] = os[1];
		replace_transformer<sp_tau_node<BAs...>> replace{m};
		return post_order_traverser<decltype(replace), 
			decltype(all<sp_tau_node<BAs...>>), 
			sp_tau_node<BAs...>>(replace , all<sp_tau_node<BAs...>>)(os[2]);
	}	
};

// converts a tau_source_sym to a tau_sym, it is used to convert from
// tau_source to tau_node...
template <typename... BAs>
struct tauify {

	tau_sym<BAs...> operator()(const tau_source_sym& n) const {
		tau_sym<BAs...> nn(n);
		return nn;
	}
};

// extracts terminal from sp_tau_source_node
template <typename... BAs>
auto tau_node_terminal_extractor = [](const sp_tau_node<BAs...>& n) -> std::optional<char> {
	if (n->value.index() == 0 
			&& !get<0>(n->value).nt()
			&& !get<0>(n->value).is_null())
		return std::optional<char>(get<0>(n->value).t());
	return std::optional<char>();
};

// extracts terminal from sp_tau_source_node
auto tau_source_terminal_extractor = [](const sp_tau_source_node& n) -> std::optional<char> {
	if (n->value.nt()&& !(n->value).is_null())
		return std::optional<char>(n->value.t());
	return std::optional<char>();
};

// adds terminal symbols to a given stream, used in conjuction with usual
// traversals (see make_string_* methods).
template <typename extractor_t, typename node_t>
struct stringify {

	stringify(const extractor_t& extractor, std::basic_stringstream<char>& stream) 
		: stream(stream), extractor(extractor) {}

	node_t operator()(const node_t& n) {
		if (auto str = extractor(n); str)
			stream << str.value(); 
		return n;
	}

	const extractor_t& extractor;
	std::basic_stringstream<char>& stream;
};

// converts a sp_tau_node<...> to a string skipping the nodes that satisfy the 
// given predicate.
template <typename extractor_t, typename predicate_t, typename node_t>
std::string make_string_with_skip(const extractor_t& extractor, predicate_t& skip, const node_t& n) {
	std::basic_stringstream<char> ss;
	stringify<extractor_t, node_t> sy(extractor, ss);
	post_order_tree_traverser<decltype(sy), predicate_t, node_t>(sy, skip)(n);
	return ss.str();
}

// converts a sp_tau_node<...> to a string.
template <typename extractor_t, typename node_t>
std::string make_string(const extractor_t& extractor, const node_t& n) {
	return make_string_with_skip<extractor_t, decltype(all<node_t>), node_t>(extractor, all<node_t>, n);
}

// bind the given, using a binder, the constants of the a given sp_tau_node<...>.
template<typename binder_t, typename... BAs>
struct bind_transformer {

	bind_transformer(const binder_t& binder) : binder(binder) {}

	sp_tau_node<BAs...> operator()(const sp_tau_node<BAs...>& n) {
		if (auto it = changes.find(n); it != changes.end()) return it->second;
		if (is_tau_node<tau_parser::binding, BAs...>(n)) 
			return changes.emplace(n, binder.bind(n)).first->second;
		bool changed = false;
		std::vector<sp_tau_node<BAs...>> child;
		for (auto& c : n->child) 
			if (changes.contains(c)) changed = true, child.push_back(changes[c]);
			else child.push_back(c);
		auto nn = make_node<tau_sym<BAs...>>(n->value, child);
		if (changed) 
			changes[n] = nn;
		return nn;
	}

	std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes;
	const binder_t binder;
};

// is not a whitespace predicate
template<typename... BAs>
static const auto not_whitespace_predicate = [](const sp_tau_node<BAs...>& n) { 
	return n->value.index() != 0 
		|| !get<0>(n->value).nt() 
		|| get<0>(n->value).n() != tau_parser::ws); 
};

// binds the constants of a given binding using the label specified
// in the code and according to a map from labels to constants in the BAs...
template<typename... BAs>
struct name_binder {
	
	name_binder(const bindings<BAs...>& bs) : bs(bs) {}

	sp_tau_node<BAs...> bind(const sp_tau_node<BAs...>& n) const {
		auto bn = make_string_with_skip<
			decltype(tau_node_terminal_extractor<BAs...>),
			decltype(not_whitespace_predicate<BAs...>), 
			sp_tau_node<BAs...>>(tau_node_terminal_extractor<BAs...>, 
				not_whitespace_predicate<BAs...>, n);
		auto s = bs.find(bn);
		if (s != bs.end()) {
			tau_sym<BAs...> ts = s->second;
			return make_node<tau_sym<BAs...>>(ts, {});
		}
		return n;
	}

	const bindings<BAs...>& bs;
};

// binds the constants of a given binding using the multi-factory for the types
// supported. 
template<typename factory_t, typename... BAs>
struct factory_binder {

	factory_binder(const factory_t& factory) : factory(factory) {}

	sp_tau_node<BAs...> bind(const sp_tau_node<BAs...>& n) const {
		if(auto type = find_top(n, is_tau_node<tau_parser::type, BAs...>); type)
			// the factory take two arguments, the first is the type and the 
			// second is the node representing the constant.
			return factory.build(type, n);
		else return n;
	}

	const factory_t& factory;
};

// check if the type is unresolved.
//
// TODO add static definition for is_unresolved and update code
template<typename... BAs>
struct is_unresolved_predicate {
	
	bool operator()(const sp_tau_node<BAs...>& n) {
		return is_tau_node<tau_parser::type, BAs...>(n) && make_string(n).empty();
	}
};

// check if the type is resolved.
//
// TODO add static definition for is_resolved and update code
template<typename... BAs>
struct is_resolved_predicate {
	
	bool operator()(const sp_tau_node<BAs...>& n) {
		return is_tau_node<tau_parser::type, BAs...>(n) || !make_string(n).empty();
	}
};

// TODO add static definition for is_unresolved_predicate and update code
// TODO add static definition for is_resolved_predicate and update code

// check if the given expression is unresolved, i.e. contains a type which
// is not resolved.
//
// TODO rename to find_unresolved
template<typename... BAs>
std::optional<sp_tau_node<BAs...>> is_unresolved(const sp_tau_node<BAs...>& n) {
	return find_top(n, is_unresolved_predicate<BAs...>());
}

// resolve all the unresolved types in the given expression. the types are
// resolved to the first bottom type resolved in the expression.
template<typename... BAs>
sp_tau_node<BAs...> resolve_type(const sp_tau_node<BAs...>& n) {
	// should not be call with other that bfs.
	// if (!is_tau_node<tau_parser::bf, BAs...>(n)) return n;
	if (auto unresolved = is_unresolved(n); unresolved) {
		// always we have type information or it is not needed at all
		auto type = find_bottom(n, is_resolved_predicate<BAs...>()).value();
		// TODO this should be extracted to a function in rewriting as it is 
		// a common pattern.
		std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> m;
		m[unresolved.value()] = type;
		replace_transformer<sp_tau_node<BAs...>> replace{m};
		return post_order_traverser<decltype(replace), 
			decltype(all<sp_tau_node<BAs...>>), sp_tau_node<BAs...>>(replace , all<sp_tau_node<BAs...>>)(n);
	}
}

// resolve all the unresolved types in the given statement.
template<typename binder_t, typename... BAs>
sp_tau_node<BAs...> resolve_types(const sp_tau_node<BAs...> source) {
	std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes;
	for (const auto& bf: select_top(source, is_tau_node<tau_parser::bf, BAs...>)) {
		if (auto rbf = resolve_type(bf); rbf != bf) changes[bf] = rbf;
	}
	replace_transformer<sp_tau_node<BAs...>> rt(changes);
	return post_order_traverser<decltype(rt), 
		decltype(all<sp_tau_node<BAs...>>), sp_tau_node<BAs...>>(rt, all<sp_tau_node<BAs...>>)(source);
}

// resolve all the unresolved types in the given rules.
template<typename... BAs>
rules<BAs...> resolve_types(const rules<BAs...> rs) {
	rules<BAs...> nrs;
	for (const auto& r : rs) {
		auto nr = resolve_types(r);
		nrs.push_back(nr);
	}
	return nrs;
}

// resolve all the unresolved types in the given library.
template<typename binder_t, typename... BAs>
library<BAs...> resolve_types(const library<BAs...> lib) {
	return { resolve_types(lib.system) };
}

// resolve all the unresolved types in the given formula.
template<typename binder_t, typename... BAs>
formula<BAs...> resolve_types(const formula<BAs...> f) {
	return { resolve_types(f.rec_relations), resolve_type(f.main) };
}

// creates a specific rule from a generic rule.
template<typename... BAs>
tau_rule<BAs...> make_rule(sp_tau_node<BAs...>& rule) {
	// TODO this tree structure should be checked in test_tau_parser.cpp
	auto rule_type = get<0>(rule->child[0]->value).n();
	switch(rule_type) {
		case ::tau_parser::wff_def:
			return *get_pair<::tau_parser::wff_head, ::tau_parser::wff, BAs...>(rule);
		case ::tau_parser::cbf_def:
			return *get_pair<::tau_parser::cbf_head, ::tau_parser::cbf, BAs...>(rule);
		case ::tau_parser::bf_rule:
			return *get_pair<::tau_parser::bf, ::tau_parser::bf, BAs...>(rule);
		default:
			assert(false); // error in grammar or parser
	} 
}

// make a library from the given tau source.
template<typename... BAs>
library<BAs...> make_library(sp_tau_source_node& tau_source) {
	tauify<BAs...> tf;
	auto lib = map_transformer<decltype(tf), sp_tau_source_node, sp_tau_node<BAs...>>(tf)(tau_source);
	rules<BAs...> rs;
	for (auto& r: select_top(lib, is_tau_node<tau_parser::rule, BAs...>)) rs.push_back(make_rule<BAs...>(r));
	return { rs };
}

// make a formula from the given tau source and bindings.
//
// TODO rename to make_formula
template<typename... BAs>
formula<BAs...> make_formula(sp_tau_source_node& tau_source, const bindings<BAs...>& bindings) {
	tauify<BAs...> tf;
	auto src = map_transformer<decltype(tf), sp_tau_source_node, sp_tau_node<BAs...>>(tf)(tau_source);
	auto m = find_top(src, is_tau_node<tau_parser::main, BAs...>).value();
	name_binder<BAs...> nb(bindings);
	bind_transformer<name_binder<BAs...>, BAs...> bs(nb); 
	auto statement = post_order_traverser<decltype(bs), decltype(all<sp_tau_node<BAs...>>), sp_tau_node<BAs...>>(bs, all<sp_tau_node<BAs...>>)(m);
	rules<BAs...> rs;
	for (auto& r: select_top(src, is_tau_node<tau_parser::rule, BAs...>)) rs.push_back(make_rule<BAs...>(r));
	return { rs, statement };
}

// TODO:HIGH add a make_formula function dealing with factory_binder

// apply one tau rule to the given expression
template<typename... BAs>
sp_tau_node<BAs...> tau_apply(const rule<tau_sym<BAs...>>& r, const sp_tau_node<BAs...>& n) {
	// TODO we could also apply only once
	return post_order_traverser(map_transformer(callback_applier<BAs...>()))(apply(r,n));
}

// apply the given rules to the given expression
template<typename... BAs>
sp_tau_node<BAs...> tau_apply(const rules<BAs...>& rs, const sp_tau_node<BAs...>& n) {
	sp_tau_node<BAs...> nn;
	for (auto& r : rs) nn = tau_apply(r, nn, is_tau_node<::tau_parser::ignore, BAs...>, is_tau_node<::tau_parser::capture, BAs...>);
	return nn;
}

template<typename... BAs>
tau<BAs...> make_tau() {
	// TODO give a proper implementation in the future
	return tau<BAs...>();
}

using parse_symbol = idni::lit<char, char>;
using parse_location = std::array<size_t, 2UL>;
using parse_node = std::pair<parse_symbol, parse_location>;
using parse_forest = idni::forest<parse_node>;
using parse_tree = typename parse_forest::tree;
using sp_parse_tree = typename parse_forest::sptree;
using parse_node = typename parse_forest::node;
using parse_graph = typename parse_forest::graph;

// drop unnecessary information from the parse tree nodes
//
// TODO make an static const version of this and change all the code to use it
struct drop_location {

	tau_source_sym operator()(const parse_node& p) { return p.first; }	
};

// make a tau source from the given source code string.
sp_tau_source_node make_tau_source(const std::string source) {
	sp_parse_tree t;
	tau_parser p;
	auto f = p.parse(source.c_str(), source.size());
	if (!f || !p.found()) {
		std::cerr << p.get_error().to_str(); 
	}

	auto get_tree = [&f, &t] (auto& g ){
			f->remove_recursive_nodes(g);
			t = g.extract_trees();
			t->to_print(std::cout);
			return false;
		};
	f->extract_graphs(f->root(), get_tree);

	drop_location dl;
	map_transformer<drop_location, sp_parse_tree, sp_tau_source_node> transform(dl);
	return post_order_traverser<decltype(transform), decltype(all<sp_parse_tree>),
		sp_parse_tree, sp_node<tau_source_sym>>(transform, all<sp_parse_tree>)(t);
}

} // namespace idni::tau

// outputs a sp_tau_node<...> to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// TODO maybe it should be move to out.h
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream, const idni::tau::sp_tau_node<BAs...>& n){
	return stream << idni::tau::make_string(idni::tau::tau_node_terminal_extractor<BAs...>, n);
}

// outputs a sp_tau_source_node to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// TODO maybe it should be move to out.h
std::ostream& operator<<(std::ostream& stream, const idni::tau::sp_tau_source_node& n){
	return stream << idni::tau::make_string(idni::tau::tau_source_terminal_extractor, n);
}

#endif // __PROGRAM_H__