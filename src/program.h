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
#include "rewriting.h"
#include "../parser/tau_parser.generated.h"

using namespace idni::rewriter;

namespace idni::tau {

// tau_source_node is the type of nodes we use to represent get from parsing tau language
using tau_source_sym = idni::lit<char, char>;
using tau_source_node = node<idni::lit<char, char>>;
using sp_tau_source_node = sp_node<idni::lit<char, char>>;
using tau_source_tree = tree<node<idni::lit<char, char>>>;

// node type for the tau language related programs, libraries and 
// specifications trees.
template <typename... BAs>
using tau_sym = std::variant<tau_source_sym, std::variant<BAs...>>;
template <typename... BAs>
using tau_node = node<tau_sym<BAs...>>;
template <typename... BAs>
using sp_tau_node = sp_node<tau_sym<BAs...>>;;
template <typename... BAs>
using tau_tree = tree<tau_node<BAs...>>;
template <typename... BAs>
using tau_rule = rule<sp_node<tau_sym<BAs...>>>;


// defines a vector of rules in the tau language, the order is important as it defines
// the order of the rules in the rewriting process of the tau language.
template <typename... BAs>
using rules = std::vector<tau_rule<BAs...>>;
// defines the main statement of a tau program.
template <typename... BAs>
using statement = sp_tau_node<BAs...>;

// a tau source is a set of rules and a (possibly) main rule, it is an intermediate
// representation of a program/library, it is not a program itself. A real program
// is a tau source with all the boolean algebra constants instantiated. Neither is
// a library, which is a tau source transformed into a proper library, i.e. a set
// of rules to be applied in the rewriting process of the tau language.
using tau_source = tree<node<tau_source_sym>>;

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

// a program is a set of rules and a main, the boolean algebra constants 
// (unless '0' or '1') are uninstantiated.
template<typename... BAs>
struct program {

	program(rules<BAs...>& rec_relations, statement<BAs...>& main): rec_relations(rec_relations), main(main) {};

	rules<BAs...> rec_relations;
	statement<BAs...> main;
};

// a program is a set of rules and a main, the boolean algebra constants 
// (unless '0' or '1') are uninstantiated.
template<typename... BAs>
struct tau_spec {

	// logical operators on tau specs, dummy implementation for now.
	tau_spec operator&(tau_spec const& that) const { 
		// TODO: implement in the future
		return tau_spec(); 
	}
	tau_spec operator|(tau_spec const& that) const {
		// TODO: implement in the future
		return tau_spec(); 
	}
	tau_spec operator^(tau_spec const& that) const {
		// TODO: implement in the future
		return tau_spec(); 
	}
	tau_spec operator~() const {
		// TODO: implement in the future
		return tau_spec(); 
	}
};

// TODO implementations details to be moved to a separate file
template<typename... BAs>
struct is_non_terminal {

	size_t operator()(const sp_tau_node<BAs...>& n) {
		return n->value.index() == 0 // std::holds_alternative<tau_sym>(*n) 
			&& get<0>(*n).nt();
	}
};

template <size_t nt_t>
struct is {

	bool operator()(const sp_tau_source_node& n) {
		return n->value.nt() && n->value.n() == nt_t;
	}

	template <typename... BAs>
	bool operator()(const sp_tau_node<BAs...>& n) {
		return n->value.index() == 0 // std::holds_alternative<tau_sym>(*n) 
			&& get<0>(n->value).nt() 
			&& get<0>(n->value).n() == nt_t;
	}
};

template<typename... BAs>
struct is_capture_predicate {

	bool operator()(const sp_tau_node<BAs...>& n) {
		return n->value.index() == 0 // std::holds_alternative<tau_sym>(*n) 
			&& get<0>(n->value).n() == ::tau_parser::capture;
	}
};

template<typename... BAs>
struct is_ignore_predicate {

	bool operator()(const sp_tau_node<BAs...>& n) {
		if(!std::holds_alternative<tau_sym>(n->value)) return false;
		auto sn = get<0>(n->value);
		return sn->nt() && n->n() == ::tau_parser::ignore;
	}
};

template <size_t nt_t, typename... BAs>
std::vector<sp_tau_node<BAs...>> get(const sp_tau_node<BAs...>& n) {
	return select_top(n, [](const sp_tau_node<BAs...>& n) {
		return get<0>(*n)->value.nt() == nt_t;
	});
}

template <typename... BAs>
struct builtin_applier {

	sp_tau_node<BAs...> operator()(const sp_tau_node<BAs...>& n) {
		if (!is_builtin(n)) return n;
		switch (n->get().nt()) {
			case ::tau_parser::bf_and_bltin: return apply_and<BAs...>(n);
			case ::tau_parser::bf_or_bltin: return apply_or<BAs...>(n);
			case ::tau_parser::bf_neg_bltin: return apply_neg<BAs...>(n);
			case ::tau_parser::bf_xor_bltin: return apply_xor<BAs...>(n);
			case ::tau_parser::bf_subs_bltin: return apply_subs<BAs...>(n);
			default: return n;
		}
	}

private:
	sp_tau_node<BAs...> apply_and(const sp_tau_node<BAs...>& n) {
		auto os = get<tau_parser::bltin_arg>(n);
		return make_shared<tau_node<BAs...>>(*os[0] & *os[1]);
	}

	sp_tau_node<BAs...> apply_or(const sp_tau_node<BAs...>& n) {
		auto os = get<tau_parser::bltin_arg>(n);
		return make_shared<tau_node<BAs...>>(*os[0] | *os[1]);
	}

	sp_tau_node<BAs...> apply_neg(const sp_tau_node<BAs...>& n) {
		auto os = get<tau_parser::bltin_arg>(n);
		return make_shared<tau_node<BAs...>>(~*os[0]);
	}

	sp_tau_node<BAs...> apply_xor(const sp_tau_node<BAs...>& n) {
		auto os = get<tau_parser::bltin_arg>(n);
		return make_shared<tau_node<BAs...>>(*os[0] ^ *os[1]);
	}
	
	sp_tau_node<BAs...> apply_subs(const sp_tau_node<BAs...>& n) {
		auto os = get<tau_parser::bltin_arg>(n);
		std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> m;
		m[os[0]] = os[1];
		replace_transformer<sp_tau_node<BAs...>> replace{m};
		true_predicate<sp_tau_node<BAs...>> always;
		return post_order_traverser<decltype(replace), decltype(always), sp_tau_node<BAs...>>(replace , always)(os[2]);
	}
	
};


template <typename... BAs>
struct tauify {

	tau_sym<BAs...> operator()(const tau_source_sym& n) const {
		tau_sym<BAs...> nn(n);
		return nn;
	}
};

template<typename... BAs>
struct bind_transformer {

	bind_transformer(const bindings<BAs...>& bs) : bs(bs) {}

	sp_tau_node<BAs...> operator()(const sp_tau_node<BAs...>& n) {
		if (auto it = changes.find(n); it != changes.end()) return it->second;
		if (auto b = bind(n); b != n) 
			return changes.emplace(n, b).first->second;
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
	const bindings<BAs...>& bs;
private:
	sp_tau_node<BAs...> bind(const sp_tau_node<BAs...>& n) const {
		if (n->value.index() != 0) return n;
		if (get<0>(n->value).nt() && get<0>(n->value).n() == ::tau_parser::binding) {
			auto bn = make_string(n);
			auto s = bs.find(bn);
			if (s != bs.end()) {
				tau_sym<BAs...> ts = s->second;
				return make_node<tau_sym<BAs...>>(ts, {});
			}
		}
		return n;
	}

	std::string make_string(const sp_tau_node<BAs...>& n) const {
		std::basic_stringstream<char> ss;
		stringify sy(ss);
		// true_predicate<sp_tau_node<BAs...>> always;
		auto not_ws = [](const sp_tau_node<BAs...>& n) { return !(n->value.index() == 0 && get<0>(n->value).nt() && get<0>(n->value).n() == tau_parser::ws); };	
		post_order_tree_traverser<decltype(sy), decltype(not_ws), sp_tau_node<BAs...>>(sy, not_ws)(n);
		return ss.str();
	}

	struct stringify {

		stringify(std::basic_stringstream<char>& ss) : ss(ss) {}

		sp_tau_node<BAs...> operator()(const sp_tau_node<BAs...>& n) {
			if (n->value.index() != 0) return n;
			if (n->value.index() == 0 
					&& !get<0>(n->value).nt()
					&& !get<0>(n->value).is_null())
				ss << get<0>(n->value).t(); 
			return n;
		}

		std::basic_stringstream<char>& ss;
	};
};

template<typename... BAs>
tau_rule<BAs...> make_rule(sp_tau_node<BAs...>& n) {
	auto p = n->child[0]->child[0]->child[0];
	auto s = n->child[0]->child[0]->child[1];
	return { p, s }; 
}

template<typename... BAs>
library<BAs...> make_library(tau_source& tau_source) {
	tauify<BAs...> tf;
	auto lib = map_transformer<decltype(tf), sp_tau_source_node, sp_tau_node<BAs...>>(tf)(tau_source.root);
	rules<BAs...> rs;
	auto is_rule = is<::tau_parser::rule>();
	for (auto& r: select_top(lib, is_rule)) rs.push_back(make_rule<BAs...>(r));
	return { rs };
}

template<typename... BAs>
program<BAs...> make_program(tau_source& tau_source, const bindings<BAs...>& bindings) {
	tauify<BAs...> tf;
	auto src = map_transformer<decltype(tf), sp_tau_source_node, sp_tau_node<BAs...>>(tf)(tau_source.root);
	auto is_main = is<::tau_parser::main>();
	auto m = find_top(src, is_main).value();
	bind_transformer<BAs...> bs(bindings); 
	true_predicate<sp_tau_node<BAs...>> always;
	auto statement = post_order_traverser<decltype(bs), decltype(always), sp_tau_node<BAs...>>(bs, always)(m);
	rules<BAs...> rs;
	is<::tau_parser::rule> is_rule;
	for (auto& r: select_top(src, is_rule)) rs.push_back(make_rule<BAs...>(r));
	return { rs, statement };
}

// apply one tau rule to the tree given by a tau node
template<typename... BAs>
sp_tau_node<BAs...> tau_apply(const rule<tau_sym<BAs...>>& r, const sp_tau_node<BAs...>& n) {
	// TODO we could also apply only once
	return post_order_traverser(map_transformer(builtin_applier<BAs...>()))(apply(r,n));
}

// execute one step of the program
template<typename... BAs>
sp_tau_node<BAs...> tau_apply(const rules<BAs...>& rs, const sp_tau_node<BAs...>& n) {
	sp_tau_node<BAs...> nn;
	for (auto& r : rs) nn = tau_apply(r, nn, is<::tau_parser::ignore>(), is<::tau_parser::capture>());
	return nn;
}

template<typename... BAs>
tau_spec<BAs...> make_tau_spec() {
	// TODO give a proper implementation in the future
	return tau_spec<BAs...>();
}

using parse_symbol = idni::lit<char, char>;
using parse_location = std::array<size_t, 2UL>;
using parse_node = std::pair<parse_symbol, parse_location>;
using parse_forest = idni::forest<parse_node>;
using parse_tree = typename parse_forest::tree;
using sp_parse_tree = typename parse_forest::sptree;
using parse_node = typename parse_forest::node;
using parse_graph = typename parse_forest::graph;

struct drop_location {

	tau_source_sym operator()(const parse_node& p) {
		return p.first;
	}	
};

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
	true_predicate<sp_parse_tree> always;
	return post_order_traverser<decltype(transform), decltype(always),
		sp_parse_tree, sp_node<tau_source_sym>>(transform, always)(t);
}

} // namespace idni::tau
#endif // __PROGRAM_H__