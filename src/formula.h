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
#include <optional>
#include <algorithm>
#include <functional>
#include <ranges>

//#include "tree.h"
#include "bool.h"
#include "rewriting.h"
#include "../parser/tau_parser.generated.h"

using namespace idni::rewriter;

namespace idni::tau {

// 
// types related to the tau language
// 

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
using sp_tau_node = sp_node<tau_sym<BAs...>>;
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
		// TODO implement in the future
		return tau(); 
	}
	tau operator|(tau const& that) const {
		// TODO implement in the future
		return tau(); 
	}
	tau operator^(tau const& that) const {
		// TODO implement in the future
		return tau(); 
	}
	tau operator~() const {
		// TODO implement in the future
		return tau(); 
	}
};

//
// predicates and functions related to the tau language
//

// check if a node is a non terminal node
template<typename... BAs>
bool is_non_terminal_node(const sp_tau_node<BAs...>& n) {
	return std::holds_alternative<tau_source_sym>(n->value) && get<tau_source_sym>(n->value).nt();
};

// factory method for is_non_terminal_node predicate
template<typename... BAs>
std::function<bool(const sp_tau_node<BAs...>&)> is_non_terminal_node() {
	return [](const sp_tau_node<BAs...>& n) { return is_non_terminal_node<BAs...>(n); };
}

// check if the node is the given non terminal
template <typename...BAs>
bool is_non_terminal(const size_t nt, const sp_tau_node<BAs...>& n) { 
	return is_non_terminal_node<BAs...>(n) && get<tau_source_sym>(n->value).n() == nt;
}

// check if the node is the given non terminal (template approach)
template <size_t nt, typename...BAs>
bool is_non_terminal(const sp_tau_node<BAs...>& n) { 
	return is_non_terminal<BAs...>(nt, n);
}
// factory method for is_non_terminal predicate
template<typename... BAs>
std::function<bool(const sp_tau_node<BAs...>&)> is_non_terminal(const size_t nt) {
	return [nt](const sp_tau_node<BAs...>& n) { return is_non_terminal<BAs...>(nt, n); };
}

// check if a node is a terminal
template<typename... BAs>
bool is_terminal_node(const sp_tau_node<BAs...>& n) {
	return std::holds_alternative<tau_source_sym>(n->value) && !get<tau_source_sym>(n->value).nt();
};

// factory method for is_terminal_node predicate
template<typename... BAs>
std::function<bool(const sp_tau_node<BAs...>&)> is_terminal_node() {
	return [](const sp_tau_node<BAs...>& n) { return is_terminal_node<BAs...>(n); };
}

// check if the node is the given terminal (functional approach)
template <typename...BAs>
bool is_terminal(const char c, const sp_tau_node<BAs...>& n) { 
	return is_terminal<BAs...>(n) && get<tau_source_sym>(n->value).n() == c;
};

// check if the node is the given terminal (template approach)
template <char c, typename...BAs>
bool is_terminal(const sp_tau_node<BAs...>& n) { 
	return is_terminal<BAs...>(c, n);
};

// factory method for is_terminal predicate
template<typename... BAs>
std::function<bool(const sp_tau_node<BAs...>&)> is_terminal(char c) {
	return [c](const sp_tau_node<BAs...>& n) { return is_terminal<BAs...>(c, n); };
}

//
// functions to traverse the tree according to the specified non terminals
// and collect the corresponding nodes
//
//

// traverse the tree, depth first, according to the specified non 
// terminals and return, if possible, the required non terminal node
template <typename... BAs>
std::optional<sp_tau_node<BAs...>> operator|(const sp_tau_node<BAs...>& n, const size_t nt) {
	auto v = n->child 
		| std::ranges::views::filter(is_non_terminal<BAs...>(nt))
		| std::ranges::views::take(1);
	return v.empty() ? std::optional<sp_tau_node<BAs...>>() : std::optional<sp_tau_node<BAs...>>(v.front());
}

template <typename... BAs>
std::optional<sp_tau_node<BAs...>> operator|(const std::optional<sp_tau_node<BAs...>>& n, const size_t nt) {
	return n ? n.value() | nt : n; 
}

template <typename... BAs>
std::optional<sp_tau_node<BAs...>> get_node(const size_t nt, const sp_tau_node<BAs...>& n) {
	return n | nt;
}

template <size_t nt, typename... BAs>
std::optional<sp_tau_node<BAs...>> get_node(const sp_tau_node<BAs...>& n) {
	return n | nt;
}

template <typename... BAs>
std::optional<sp_tau_node<BAs...>> get_node(const size_t nt) {
	return [nt](const sp_tau_node<BAs...>& n) { return get_node<BAs...>(nt, n); };
}

template <typename... BAs>
std::ranges::view auto operator|(const std::ranges::view auto& v, const size_t nt) {
	return v 
		| std::ranges::views::transform(get_node<BAs...>(nt)) 
		| std::ranges::views::join; 
}

// traverse the tree, top down, and return all the nodes accessible according
// to the specified non terminals and return them
template <typename... BAs>
std::ranges::view auto operator||(const sp_tau_node<BAs...>& n, const size_t nt) {
	return n->child 
		| std::ranges::views::filter(is_non_terminal<BAs...>(nt));
}

template <typename... BAs>
std::ranges::view auto get_nodes(const size_t nt, const sp_tau_node<BAs...>& n) {
	return n || nt;
}

template <size_t nt, typename... BAs>
std::ranges::view auto get_nodes(const sp_tau_node<BAs...>& n) {
	return n || nt;
}

template <typename... BAs>
auto get_nodes(const size_t nt) {
	return [nt](const sp_tau_node<BAs...>& n) { return get_nodes<BAs...>(nt, n); };
}

template <typename... BAs>
std::ranges::view auto operator||(const std::ranges::view auto& v, const size_t nt) {
	return v 
		| std::ranges::views::transform(get_nodes<BAs...>(nt)) 
		| std::ranges::views::join; 
}

//
// functions to extract components/informatino from nodes
//
//

// extract the value of the node
template<typename... BAs>
auto value_extractor = [](const sp_tau_node<BAs...>& n) -> tau_sym<BAs...> {
	return n->value;
};

template<typename... BAs>
using value_extractor_t = decltype(value_extractor<BAs...>);

// returns an optional containing the terminal of the node if possible
template<typename... BAs>
auto terminal_extractor = [](const sp_tau_node<BAs...>& n) -> std::optional<char> {
	auto value = n->value;
	if (!std::holds_alternative<tau_source_sym>(value)
			|| get<tau_source_sym>(value).nt() 
			|| get<tau_source_sym>(value).is_null()) 
		return std::optional<char>();
	return std::optional<char>(get<tau_source_sym>(value).t());
};

template<typename... BAs>
using terminal_extractor_t = decltype(terminal_extractor<BAs...>);

template <typename... BAs>
std::ranges::view auto operator||(const std::ranges::view auto& v, const terminal_extractor_t<BAs...> e) {
	return v | std::ranges::views::transform(e); 
}

template <typename... BAs>
std::optional<char> operator|(const std::optional<sp_tau_node<BAs...>>& o, const terminal_extractor_t<BAs...> e) {
	return o.transform(e); 
}

// returns an optional containing the non terminal of the node if possible
template<typename... BAs>
auto non_terminal_extractor = [](const sp_tau_node<BAs...>& n) -> std::optional<size_t> {
	if (std::holds_alternative<tau_source_sym>(n->value) 
			&& get<tau_source_sym>(n->value).nt())
		return std::optional<size_t>(get<tau_source_sym>(n->value).n());
	return std::optional<size_t>();
};

template<typename... BAs>
using non_terminal_extractor_t = decltype(non_terminal_extractor<BAs...>);

template <typename... BAs>
std::ranges::view auto operator||(const std::ranges::view auto& v, const non_terminal_extractor_t<BAs...> e) {
	return v | std::ranges::views::transform(e); 
}

template <typename... BAs>
std::optional<size_t> operator|(const std::optional<sp_tau_node<BAs...>>& o, const non_terminal_extractor_t<BAs...> e) {
	return o.transform(e); 
}

// returns an optional containing the bas... of the node if possible
template<typename... BAs>
auto ba_extractor = [](const sp_tau_node<BAs...>& n) -> std::optional<std::variant<BAs...>> {
	if (std::holds_alternative<std::variant<BAs...>>(n->value) ) 
		return std::optional<std::variant<BAs...>>(get<std::variant<BAs...>>(n->value));
	return std::optional<std::variant<BAs...>>();
};

template<typename... BAs>
using ba_extractor_t = decltype(ba_extractor<BAs...>);

template <typename... BAs>
std::ranges::view auto operator||(const std::ranges::view auto& v, const ba_extractor_t<BAs...> e) {
	return v | std::ranges::views::transform(e); 
}

template <typename... BAs>
std::optional<std::variant<BAs...>> operator|(const std::optional<sp_tau_node<BAs...>>& o, const ba_extractor_t<BAs...> e) {
	return o.transform(e); 
}

//
// functions traversing the nodes according to the specified non terminals and
// extracting the required information from them 
//
//

// returns the extracted component/information of the specified node if possible,  
// the component/information is extracted using the given extractor. 
template <size_t... nt, typename extractor_t, typename... BAs>
auto get_optional(const extractor_t& extractor, const sp_tau_node<BAs...>& n) {
	return get_node<nt..., BAs...>(n).and_then(extractor);
}

// returns the extracted components/informations of the specified nodes, the 
// component/information is extracted using the given extractor. 
template <size_t... nt, typename extractor_t, typename... BAs>
auto get_optionals(const extractor_t& extractor, const sp_tau_node<BAs...>& n) {
	return get_nodes<nt..., BAs...>(n) 
		| std::ranges::views::transform(extractor)
		| std::ranges::views::filter(&std::optional<tau_sym<BAs...>>::has_value);
}

// returns the terminal of the specified node if possible
template <size_t... nts, typename... BAs>
auto get_terminal(const sp_tau_node<BAs...>& n) {
	return get_optional<nts..., terminal_extractor_t<BAs...>, BAs...>(terminal_extractor<BAs...>, n);
}

// returns the terminals of the specified nodes
template <size_t... nts, typename... BAs>
auto get_terminals(const sp_tau_node<BAs...>& n) {
	return get_optionals<nts..., terminal_extractor_t<BAs...>, BAs...>(terminal_extractor<BAs...>, n);
}

// returns the non terminal of the specified node if possible
template <size_t... nts, typename... BAs>
std::optional<char> get_non_terminal(const sp_tau_node<BAs...>& n) {
	return get_optional<nts..., non_terminal_extractor_t<BAs...>, BAs...>(non_terminal_extractor<BAs...>, n);
}

// returns the non terminals of the specified nodes
template <size_t... nts, typename... BAs>
auto get_non_terminals(const sp_tau_node<BAs...>& n) {
	return get_optionals<nts..., non_terminal_extractor_t<BAs...>, BAs...>(non_terminal_extractor<BAs...>, n);
}

// returns the bas... of the specified node if possible
template <size_t... nts, typename... BAs>
std::optional<char> get_ba(const sp_tau_node<BAs...>& n) {
	return get_optional<nts..., ba_extractor_t<BAs...>, BAs...>(ba_extractor<BAs...>, n);
}

// returns the bas... of the specified nodes
template <size_t... nts, typename... BAs>
auto get_bas(const sp_tau_node<BAs...>& n) {
	return get_optionals<nts..., ba_extractor_t<BAs...>, BAs...>(ba_extractor<BAs...>, n);
}

// apply the given callback if the value of the node is a callback
//
// TODO convert to a const static applier and change all the code accordingly
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

// TODO remove this extractor and use a previous one
// extracts terminal from sp_tau_source_node
template <typename... BAs>
auto tau_node_terminal_extractor = [](const sp_tau_node<BAs...>& n) -> std::optional<char> {
	if (n->value.index() == 0 
			&& !get<0>(n->value).nt()
			&& !get<0>(n->value).is_null())
		return std::optional<char>(get<0>(n->value).t());
	return std::optional<char>();
};

// TODO remove this extractor and use a previous one
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
		if (is_non_terminal<tau_parser::binding, BAs...>(n)) 
			return changes.emplace(n, binder.bind(n)).first->second;
		// IDEA maybe we could use the replace transform instead of having the following code
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
		|| get<0>(n->value).n() != tau_parser::ws; 
};

// binds the constants of a given binding using the label specified
// in the code and according to a map from labels to constants in the BAs...
template<typename... BAs>
struct name_binder {
	
	name_binder(const bindings<BAs...>& bs) : bs(bs) {}

	sp_tau_node<BAs...> bind(const sp_tau_node<BAs...>& n) const {
		// FIXME check if the node is a named binding one
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
		// FIXME check that the node is a factory binding one
		if(auto type = find_top(n, is_non_terminal<tau_parser::type, BAs...>); type)
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
		return is_non_terminal<tau_parser::type, BAs...>(n) && make_string(n).empty();
	}
};

// check if the type is resolved.
//
// TODO add static definition for is_resolved and update code
template<typename... BAs>
struct is_resolved_predicate {
	
	bool operator()(const sp_tau_node<BAs...>& n) {
		return is_non_terminal<tau_parser::type, BAs...>(n) || !make_string(n).empty();
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
	// if (!is_non_terminal<tau_parser::bf, BAs...>(n)) return n;
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
	for (const auto& bf: select_top(source, is_non_terminal<tau_parser::bf, BAs...>)) {
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
	auto type = non_terminal_extractor<BAs...>(rule->child[0]).value();
	switch(type) {
		case ::tau_parser::wff_def:
			return { (rule | ::tau_parser::wff_head).value() , (rule | ::tau_parser::wff).value() };
		case ::tau_parser::cbf_def:
			return { (rule | ::tau_parser::cbf_head).value(), (rule | ::tau_parser::cbf).value() };
		case ::tau_parser::bf_rule:
			return { (rule | ::tau_parser::bf).value(), (rule | ::tau_parser::bf).value() };
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
	for (auto& r: select_top(lib, is_non_terminal<tau_parser::rule, BAs...>)) rs.push_back(make_rule<BAs...>(r));
	return { rs };
}

// make a formula from the given tau source and bindings.
template<typename... BAs>
formula<BAs...> make_formula_using_bindings(sp_tau_source_node& tau_source, const bindings<BAs...>& bindings) {
	name_binder<BAs...> nb(bindings);
	bind_transformer<name_binder<BAs...>, BAs...> bs(nb); 
	return make_formula_using_binder<decltype(bs), BAs...>(tau_source, bs);
}

// make a formula from the given tau source and bindings.
template<typename factory_t, typename... BAs>
formula<BAs...> make_formula_using_factory(sp_tau_source_node& tau_source, const factory_t& factory) {
	bind_transformer<factory_t, BAs...> bs(factory);
	return make_formula_using_binder<decltype(bs), BAs...>(tau_source, bs);
}

// make a formula from the given tau source and binder.
template<typename binder_t, typename... BAs>
formula<BAs...> make_formula_using_binder(sp_tau_source_node& tau_source, const binder_t& binder) {
	tauify<BAs...> tf;
	auto src = map_transformer<decltype(tf), sp_tau_source_node, sp_tau_node<BAs...>>(tf)(tau_source);
	auto m = find_top(src, is_non_terminal<tau_parser::main, BAs...>).value();
	auto statement = post_order_traverser<binder_t, decltype(all<sp_tau_node<BAs...>>), sp_tau_node<BAs...>>(binder, all<sp_tau_node<BAs...>>)(m);
	rules<BAs...> rs;
	for (auto& r: select_top(src, is_non_terminal<tau_parser::rule, BAs...>)) rs.push_back(make_rule<BAs...>(r));
	return { rs, statement };
}

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
	for (auto& r : rs) nn = tau_apply(r, nn, is_non_terminal<::tau_parser::ignore, BAs...>, is_non_terminal<::tau_parser::capture, BAs...>);
	return nn;
}

template<typename... BAs>
tau<BAs...> make_tau() {
	// TODO give a proper implementation in the future
	return tau<BAs...>();
}

// make a tau source from the given source code string.
sp_tau_source_node make_tau_source(const std::string source) {
	using parse_lit = idni::lit<char, char>;
	using parse_location = std::array<size_t, 2UL>;
	using parse_symbol = std::pair<parse_lit, parse_location>;

	return make_node_from_string<tau_parser, decltype(drop_location<parse_symbol, tau_source_sym>),
			parse_symbol, tau_source_sym>(drop_location<parse_symbol, tau_source_sym>, 
			source);
}

} // namespace idni::tau

// outputs a sp_tau_node<...> to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// IDEA maybe it should be move to out.h
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream, const idni::tau::sp_tau_node<BAs...>& n){
	return stream << idni::tau::make_string(idni::tau::tau_node_terminal_extractor<BAs...>, n);
}

// outputs a sp_tau_source_node to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// IDEA maybe it should be move to out.h
std::ostream& operator<<(std::ostream& stream, const idni::tau::sp_tau_source_node& n){
	return stream << idni::tau::make_string(idni::tau::tau_source_terminal_extractor, n);
}

#endif // __PROGRAM_H__