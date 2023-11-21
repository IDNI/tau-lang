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
#include <variant>
#include <cassert>

//#include "tree.h"
#include "bool.h"
#include "rewriting.h"
#include "../parser/tau_parser.generated.h"

using namespace idni::rewriter;

namespace idni::tau {

// TODO (LOW) reorganize methods so related ones are closed

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
// TODO (LOW) remove tau_node
template <typename... BAs>
using tau_node = node<tau_sym<BAs...>>;
template <typename... BAs>
using sp_tau_node = sp_node<tau_sym<BAs...>>;
template <typename... BAs>
using tau_rule = rule<sp_node<tau_sym<BAs...>>>;

// IDEA maybe we could define a wrapper for recursive rules and rewriting rules that
// call the appropriate apply method. This would play also nice with the builders
// defined in the normalizer.

template <typename... BAs>
using builder = tau_rule<BAs...>;

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
using library = rules<BAs...>;

// bindings map tau_source constants (strings) into elements of the boolean algebras.
template<typename... BAs>
using bindings = std::map<std::string, std::variant<BAs...>>;

// a formula is a set of rules and a main, the boolean algebra constants
// (unless '0' or '1') are uninstantiated.

// TODO (LOW) replace formula with a pair of rules and main
template<typename... BAs>
struct formula {

	formula(rules<BAs...>& rec_relations, statement<BAs...>& main) : rec_relations(rec_relations), main(main) {};
//	formula(statement<BAs...>& main) : main(main) {};

	rules<BAs...> rec_relations;
	statement<BAs...> main;
};

// a formula is a set of rules and a main, the boolean algebra constants
// (unless '0' or '1') are uninstantiated.
template<typename... BAs>
struct tau {

	// logical operators on tau specs, dummy implementation for now.
	tau operator&(tau const& that) const {

		// TODO (IMPORTANT) implement in the future

		return tau();
	}
	tau operator|(tau const& that) const {

		// TODO (IMPORTANT) implement in the future

		return tau();
	}
	tau operator^(tau const& that) const {

		// TODO (IMPORTANT) implement in the future

		return tau();
	}
	tau operator~() const {

		// TODO (IMPORTANT) implement in the future

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

template<typename... BAs>
static const auto is_capture = [](const sp_tau_node<BAs...>& n) {
	return std::holds_alternative<tau_source_sym>(n->value)
		&& get<tau_source_sym>(n->value).nt()
		&& get<tau_source_sym>(n->value).n() == tau_parser::capture;
};

template<typename... BAs>
using is_capture_t = decltype(is_capture<BAs...>);

auto is_non_essential_terminal = [] (const size_t n) {
	return n == tau_parser::nul
		|| n == tau_parser::eof
		|| n == tau_parser::space
		|| n == tau_parser::digit
		|| n == tau_parser::xdigit
		|| n == tau_parser::alpha
		|| n == tau_parser::alnum
		|| n == tau_parser::punct
		|| n == tau_parser::printable
		|| n == tau_parser::eol
		|| n == tau_parser::ws_required
		|| n == tau_parser::ws_comment
		|| n == tau_parser::ws
		|| n == tau_parser::esc
		|| n == tau_parser::open_parenthesis
		|| n == tau_parser::close_parenthesis
		|| n == tau_parser::open_bracket
		|| n == tau_parser::close_bracket
		|| n == tau_parser::open_brace
		|| n == tau_parser::close_brace;
};

auto is_non_essential_sym = [] (const tau_source_sym& n) {
	if (!n.nt()) return false;
	return  is_non_essential_terminal(n.n());
};

auto is_non_essential_source = [] (const sp_tau_source_node& n) {
	if (!n->value.nt()) return false;
	return  is_non_essential_terminal(n->value.n());
};

using is_non_essential_source_t = decltype(is_non_essential_source);

template<typename...BAs>
auto is_non_essential = [] (const sp_tau_node<BAs...>& n) {
	if (!std::holds_alternative<tau_source_sym>(n->value)) return false;
	return is_non_essential_sym(std::get<tau_source_sym>(n->value));
};

template<typename...BAs>
using is_non_essential_t = decltype(is_non_essential<BAs...>);

template<typename...BAs>
static const auto is_callback = [](const sp_tau_node<BAs...>& n) {
	if (!std::holds_alternative<tau_source_sym>(n->value) || !get<tau_source_sym>(n->value).nt()) return false;
	auto nt = get<tau_source_sym>(n->value).n();
	return nt == tau_parser::bf_and_cb
		|| nt == tau_parser::bf_or_cb
		|| nt == tau_parser::bf_xor_cb
		|| nt == tau_parser::bf_neg_cb
		|| nt == tau_parser::bf_less_cb
		|| nt == tau_parser::bf_less_equal_cb
		|| nt == tau_parser::bf_greater_cb
		|| nt == tau_parser::bf_subs_cb
		|| nt == tau_parser::bf_eq_cb
		|| nt == tau_parser::bf_neq_cb
		|| nt == tau_parser::bf_is_one_cb
		|| nt == tau_parser::bf_is_zero_cb
		|| nt == tau_parser::bf_has_clashing_subformulas_cb
		|| nt == tau_parser::wff_has_clashing_subformulas_cb
		|| nt == tau_parser::bf_has_subformula_cb
		|| nt == tau_parser::wff_has_subformula_cb;
};

template<typename...BAs>
using is_callback_t = decltype(is_callback<BAs...>);


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
std::vector<sp_tau_node<BAs...>> operator|(const std::vector<sp_tau_node<BAs...>>& v, const size_t nt) {
	// IDEA use ::to to get a vector when gcc and clang implement it in the future
	std::vector<sp_tau_node<BAs...>> nv;
	for (const auto& n: v
			| std::ranges::views::transform(get_node<BAs...>(nt))
			| std::ranges::views::join)
		nv.emplace(std::move(n));
	return nv;
}

// traverse the tree, top down, and return all the nodes accessible according
// to the specified non terminals and return them
template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(const sp_tau_node<BAs...>& n, const tau_parser::nonterminal nt) {
	// IDEA use ::to to get a vector when gcc and clang implement it in the future
	std::vector<sp_tau_node<BAs...>> nv;
	nv.reserve(n->child.size());
	for (const auto& c: n->child
			| std::ranges::views::filter(is_non_terminal<BAs...>(nt)))
		nv.push_back(c);
	return nv;
}

template <typename... BAs>
std::vector<sp_tau_node<BAs...>>  operator||(const std::optional<sp_tau_node<BAs...>>& n, const tau_parser::nonterminal nt) {
	// IDEA use ::to to get a vector when gcc and clang implement it in the future
	if (n) return n.value() || nt;
	return {};
}

// TODO (LOW) remove get_nodes if possible and use operator|| instead
template <typename... BAs>
std::vector<sp_tau_node<BAs...>> get_nodes(const tau_parser::nonterminal nt, const sp_tau_node<BAs...>& n) {
	return n || nt;
}

template <size_t nt, typename... BAs>
std::vector<sp_tau_node<BAs...>> get_nodes(const sp_tau_node<BAs...>& n) {
	return n || nt;
}

template <typename... BAs>
auto get_nodes(const tau_parser::nonterminal nt) {
	return [nt](const sp_tau_node<BAs...>& n) { return get_nodes<BAs...>(nt, n); };
}

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(const std::vector<sp_tau_node<BAs...>>& v, const tau_parser::nonterminal nt) {
	// IDEA use ::to to get a vector when gcc and clang implement it in the future
	std::vector<sp_tau_node<BAs...>> nv; nv.reserve(v.size());
	for (const auto& n: v
			| std::ranges::views::transform(get_nodes<BAs...>(nt))
			| std::ranges::views::join)
		nv.push_back(n);
	return nv;
}

//
// functions to extract components/informatino from nodes
//
//

// extract the value of the node
template<typename... BAs>
static const auto value_extractor = [](const sp_tau_node<BAs...>& n) -> tau_sym<BAs...> {
	return n->value;
};

template<typename... BAs>
using value_extractor_t = decltype(value_extractor<BAs...>);

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(const std::vector<sp_tau_node<BAs...>>& v, const value_extractor_t<BAs...> e) {
	std::vector<std::variant<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e)) if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<char> operator|(const std::optional<sp_tau_node<BAs...>>& o, const value_extractor_t<BAs...> e) {
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value()) : std::optional<char>();
}

// returns an optional containing the terminal of the node if possible
template<typename... BAs>
static const auto terminal_extractor = [](const sp_tau_node<BAs...>& n) -> std::optional<char> {
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
std::vector<sp_tau_node<BAs...>> operator||(const std::vector<sp_tau_node<BAs...>>& v, const terminal_extractor_t<BAs...> e) {
	std::vector<std::variant<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e)) if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<char> operator|(const std::optional<sp_tau_node<BAs...>>& o, const terminal_extractor_t<BAs...> e) {
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value()) : std::optional<char>();
}

// returns an optional containing the non terminal of the node if possible
template<typename... BAs>
static const auto non_terminal_extractor = [](const sp_tau_node<BAs...>& n) -> std::optional<size_t> {
	if (std::holds_alternative<tau_source_sym>(n->value)
			&& get<tau_source_sym>(n->value).nt())
		return std::optional<size_t>(get<tau_source_sym>(n->value).n());
	return std::optional<size_t>();
};

template<typename... BAs>
using non_terminal_extractor_t = decltype(non_terminal_extractor<BAs...>);

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(const std::vector<sp_tau_node<BAs...>>& v, const non_terminal_extractor_t<BAs...> e) {
	return v | std::ranges::views::transform(e);
}

template <typename... BAs>
std::optional<size_t> operator|(const std::optional<sp_tau_node<BAs...>>& o, const non_terminal_extractor_t<BAs...> e) {
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value()) : std::optional<size_t>();
}

// returns an optional containing the bas... of the node if possible
template<typename... BAs>
static const auto ba_extractor = [](const sp_tau_node<BAs...>& n) -> std::optional<std::variant<BAs...>> {
	if (std::holds_alternative<std::variant<BAs...>>(n->value))
		return std::optional<std::variant<BAs...>>(get<std::variant<BAs...>>(n->value));
	return std::optional<std::variant<BAs...>>();
};

template<typename... BAs>
using ba_extractor_t = decltype(ba_extractor<BAs...>);

template <typename... BAs>
std::vector<std::variant<BAs...>> operator||(const std::vector<sp_tau_node<BAs...>>& v, const ba_extractor_t<BAs...> e) {
	std::vector<std::variant<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e)) if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<std::variant<BAs...>> operator|(const std::optional<sp_tau_node<BAs...>>& o, const ba_extractor_t<BAs...> e) {
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value()) : std::optional<std::variant<BAs...>>();
}

template <typename... BAs>
std::optional<std::variant<BAs...>> operator|(const sp_tau_node<BAs...>& o, const ba_extractor_t<BAs...> e) {
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return e(o);
}

// returns the only child of a node
template <typename... BAs>
static const auto only_child_extractor = [](const sp_tau_node<BAs...>& n) -> std::optional<sp_tau_node<BAs...>> {
	if (n->child.size() != 1) return std::optional<sp_tau_node<BAs...>>();
	return std::optional<sp_tau_node<BAs...>>(n->child[0]);
};

template<typename... BAs>
using only_child_extractor_t = decltype(only_child_extractor<BAs...>);

// TODO (HIGH) merge both implementations using a new template parameter
template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(const std::vector<sp_tau_node<BAs...>>& v, const only_child_extractor_t<BAs...> e) {
	std::vector<sp_tau_node<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e)) if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(const std::set<sp_tau_node<BAs...>>& v, const only_child_extractor_t<BAs...> e) {
	std::vector<sp_tau_node<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e)) if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<sp_tau_node<BAs...>> operator|(const std::optional<sp_tau_node<BAs...>>& o, const only_child_extractor_t<BAs...> e) {
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value()) : std::optional<sp_tau_node<BAs...>>();
}

// IDEA maybe unify all the implementations dealing with operator| and operator|| for extractors
template <typename... BAs>
std::optional<sp_tau_node<BAs...>> operator|(const sp_tau_node<BAs...>& o, const only_child_extractor_t<BAs...> e) {
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return e(o);
}

template <typename T>
static const auto optional_value_extractor = [](const std::optional<T>& o) -> T {return o.value();} ;

template <typename T>
using optional_value_extractor_t = decltype(optional_value_extractor<T>);

template <typename T>
T operator|(const std::optional<T>& o, const optional_value_extractor_t<T> e) {
	return e(o);
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

// apply the given  T and F are  if the value of the node is a callback
//
// TODO (HIGH) convert to a const static applier and change all the code accordingly
template <typename... BAs>
struct callback_applier {

	sp_tau_node<BAs...> operator()(const sp_tau_node<BAs...>& n) {
		// TODO (IMPORTANT) deal with errors once we have a clear strategy
		if (!is_callback<BAs...>(n)) return n;
		auto nt = get<tau_source_sym>(n->value).n();
		switch (nt) {
			case tau_parser::bf_and_cb: return apply_binary_operation(_and, n);
			case tau_parser::bf_or_cb: return apply_binary_operation(_or, n);
			case tau_parser::bf_xor_cb: return apply_binary_operation(_xor, n);
			case tau_parser::bf_neg_cb: return apply_unary_operation(_neg, n);
			case tau_parser::bf_eq_cb: return apply_equality_relation(_eq, n);
			case tau_parser::bf_neq_cb: return apply_equality_relation(_neq, n);
			case tau_parser::bf_is_one_cb: return apply_constant_check(_is_one, n);
			case tau_parser::bf_is_zero_cb: return apply_constant_check(_is_zero, n);
			case tau_parser::bf_less_equal_cb: return apply_order_relation(_less_equal, n);
			case tau_parser::bf_less_cb: return apply_order_relation(_less, n);
			case tau_parser::bf_greater_cb: return apply_order_relation(_greater, n);
			case tau_parser::bf_subs_cb: return apply_subs(n);
			case tau_parser::bf_has_clashing_subformulas_cb: return apply_bf_clashing_subformulas_check(n);
			case tau_parser::bf_has_subformula_cb: return apply_has_subformula_check(n, tau_parser::bf_cb_arg);
			case tau_parser::wff_has_clashing_subformulas_cb: return apply_wff_clashing_subformulas_check(n);
			case tau_parser::wff_has_subformula_cb: return apply_has_subformula_check(n, tau_parser::wff_cb_arg);
			default: return n;
		}
	}

private:
	// speed up callbacks

	// binary operations
	static constexpr auto _and = [](const auto& l, const auto& r) { return l & r; };
	static constexpr auto _or = [](const auto& l, const auto& r) { return l | r; };
	static constexpr auto _xor = [](const auto& l, const auto& r) { return l ^ r; };
	static constexpr auto _imply = [](const auto& l, const auto& r) { return ~l | r; };
	static constexpr auto _equiv = [](const auto& l, const auto& r) { return _imply(l, r) & _imply(r, l); };
	// unary operation
	static constexpr auto _neg = [](const auto& l) { return ~l; };
	// order operations
	static constexpr auto _less_equal = [](const auto& l, const auto& r) -> bool { return (l & (~r)) == false; };
	static constexpr auto _greater = [](const auto& l, const auto& r) -> bool { return !_less_equal(l, r); };
	static constexpr auto _less = [](const auto& l, const auto& r) -> bool { return _less_equal(l,r) && ((l ^ r) != false); };
	// ternary operators
	static constexpr auto _eq = [](const auto& l) -> bool { return l == false; };
	static constexpr auto _neq = [](const auto& l) -> bool { return !(l == false); };
	static constexpr auto _is_one = [](const auto& l) -> bool { return l == true; };
	static constexpr auto _is_zero = [](const auto& l) -> bool { return l == false; };

	// TODO (LOW) this should be promoted to a predicate if needed elsewhere
	bool are_equal(sp_tau_node<BAs...>& p, sp_tau_node<BAs...>& n) {
		if (p->value == n->value) {
			auto p_it = p->child.begin();
			auto n_it = n->child.begin();
			while (p_it != p->child.end() && n_it != n->child.end()) {
				if (is_non_essential<BAs...>(*p_it)) { ++p_it; continue; }
				if (is_non_essential<BAs...>(*n_it)) { ++n_it; continue; }
				if (*p_it == *n_it) { ++p_it; ++n_it; continue; }
				if (are_equal(*p_it, *n_it)) { ++p_it; ++n_it; continue; }
				return false;
			}
			return true;
		}
		return false;
	}

	sp_tau_node<BAs...> apply_has_subformula_check(sp_tau_node<BAs...> n, const tau_parser::nonterminal& cb_arg_t) {
		auto args = n || cb_arg_t || only_child_extractor<BAs...>;
		for (auto& subformula: select_all(args[0], all<sp_tau_node<BAs...>>))
			if (subformula == args[1]) return args[2];
		return args[0];
	}

	// TODO (HIGH) make a cleaner implementation
	// TODO (HIGH) merge with next method
	sp_tau_node<BAs...> apply_wff_clashing_subformulas_check(const sp_tau_node<BAs...>& n) {
		auto args = n || tau_parser::wff_cb_arg || only_child_extractor<BAs...>;
		//std::set<sp_tau_node<BAs...>> positives, negatives;
		std::vector<sp_tau_node<BAs...>> positives, negatives;
		for (auto& op: select_all(args[0], all<sp_tau_node<BAs...>>))
			if (is_non_terminal<tau_parser::wff_and>(op))
				for (auto& c: op->child)
					if (auto check = c | tau_parser::wff_and; !check.has_value() && is_non_terminal<tau_parser::wff>(c)) {
						auto cc = c | only_child_extractor<BAs...> | optional_value_extractor<sp_tau_node<BAs...>>;
						//(is_non_terminal<tau_parser::wff_neg>(cc) ? negatives : positives).insert(cc);
						(is_non_terminal<tau_parser::wff_neg>(cc) ? negatives : positives).push_back(cc);
					}
		for (auto& negation: negatives) {
			auto negated = negation | tau_parser::wff | only_child_extractor<BAs...> | optional_value_extractor<sp_tau_node<BAs...>>;
			//if(std::find(positives.begin(), positives.end(), negated) != positives.end()) return args[1];
			//if (positives.contains(negated)) return args[1];
			for (auto& positive: positives)
				if (are_equal(positive, negated))
					return args[1];
		}
		return args[0];
	}

	sp_tau_node<BAs...> apply_bf_clashing_subformulas_check(const sp_tau_node<BAs...>& n) {
		auto args = n || tau_parser::bf_cb_arg || only_child_extractor<BAs...>;
		//std::set<sp_tau_node<BAs...>> positives, negatives;
		std::vector<sp_tau_node<BAs...>> positives, negatives;
		for (auto& op: select_all(args[0], all<sp_tau_node<BAs...>>))
			if (is_non_terminal<tau_parser::bf_and>(op))
				for (auto& c: op->child)
					if (auto check = c | tau_parser::bf_and; !check.has_value() && is_non_terminal<tau_parser::bf>(c)) {
						auto cc = c | only_child_extractor<BAs...> | optional_value_extractor<sp_tau_node<BAs...>>;
						//(is_non_terminal<tau_parser::wff_neg>(cc) ? negatives : positives).insert(cc);
						(is_non_terminal<tau_parser::bf_neg>(cc) ? negatives : positives).push_back(cc);
					}
		for (auto& negation: negatives) {
			auto negated = negation | tau_parser::bf | only_child_extractor<BAs...> | optional_value_extractor<sp_tau_node<BAs...>>;
			//if(std::find(positives.begin(), positives.end(), negated) != positives.end()) return args[1];
			//if (positives.contains(negated)) return args[1];
			for (auto& positive: positives)
				if (are_equal(positive, negated))
					return args[1];
		}
		return args[0];
	}

	sp_tau_node<BAs...> apply_binary_operation(const auto& op, const sp_tau_node<BAs...>& n) {
		auto ba_elements = n || tau_parser::bf_cb_arg || tau_parser::bf || only_child_extractor<BAs...> || ba_extractor<BAs...>;
		return make_node<tau_sym<BAs...>>(std::visit(op, ba_elements[0], ba_elements[1]), {});
	}

	sp_tau_node<BAs...> apply_unary_operation(const auto& op, const sp_tau_node<BAs...>& n) {
		auto ba_elements = n || tau_parser::bf_cb_arg || tau_parser::bf || only_child_extractor<BAs...> || ba_extractor<BAs...>;
		return make_node<tau_sym<BAs...>>(std::visit(op, ba_elements[0]), {});
	}

	sp_tau_node<BAs...> apply_equality_relation(const auto& op, const sp_tau_node<BAs...>& n) {
		auto bf_arg = n | tau_parser::bf_cb_arg | tau_parser::bf | only_child_extractor<BAs...>;
		auto wff_args = n || tau_parser::wff_cb_arg || only_child_extractor<BAs...>;
		auto ba_element = bf_arg | ba_extractor<BAs...> | optional_value_extractor<std::variant<BAs...>>;
		return std::visit(op, ba_element) ? wff_args[0] : wff_args[1];
	}

	sp_tau_node<BAs...> apply_constant_check(const auto& op, const sp_tau_node<BAs...>& n) {
		auto args = n || tau_parser::bf_cb_arg || only_child_extractor<BAs...>;
		auto ba_element = args[0] | tau_parser::bf_constant | tau_parser::constant
			| only_child_extractor<BAs...> |  ba_extractor<BAs...> | optional_value_extractor<std::variant<BAs...>>;
		return std::visit(op, ba_element) ? args[1] : args[0];
	}

	sp_tau_node<BAs...> apply_order_relation(const auto& op, const sp_tau_node<BAs...>& n) {
		auto args = n || tau_parser::bf_cb_arg || tau_parser::bf || only_child_extractor<BAs...>;
		auto ba_first_element = args[0] | ba_extractor<BAs...> | optional_value_extractor<std::variant<BAs...>>;;
		auto ba_second_element = args[1] | ba_extractor<BAs...> | optional_value_extractor<std::variant<BAs...>>;;
		return std::visit(op, ba_first_element, ba_second_element) ? args[2] : args[3];
	}

	sp_tau_node<BAs...> apply_subs(const sp_tau_node<BAs...>& n) {
		auto params = n || tau_parser::bf_cb_arg || only_child_extractor<BAs...>;
		std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> m;
		m[params[0]] = params[1] | only_child_extractor<BAs...> | optional_value_extractor<sp_tau_node<BAs...>>;
		// TODO (HIGH) extract this pattern to a method call replace in rewriting.h
		replace_transformer<sp_tau_node<BAs...>> replace{m};
		return post_order_traverser<
				replace_transformer<sp_tau_node<BAs...>>,
				all_t<sp_tau_node<BAs...>>,
				sp_tau_node<BAs...>>
			(replace , all<sp_tau_node<BAs...>>)(params[2]);
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

// TODO (LOW) remove this extractor and use a previous one
// extracts terminal from sp_tau_source_node
template <typename... BAs>
auto tau_node_terminal_extractor = [](const sp_tau_node<BAs...>& n) -> std::optional<char> {
	if (n->value.index() == 0
			&& !get<0>(n->value).nt()
			&& !get<0>(n->value).is_null())
		return std::optional<char>(get<0>(n->value).t());
	return std::optional<char>();
};

template <typename... BAs>
using tau_node_terminal_extractor_t = decltype(tau_node_terminal_extractor<BAs...>);

// TODO (LOW) remove this extractor and use a previous one
// extracts terminal from sp_tau_source_node
auto tau_source_terminal_extractor = [](const sp_tau_source_node& n) -> std::optional<char> {
	if (n->value.nt()&& !(n->value).is_null())
		return std::optional<char>(n->value.t());
	return std::optional<char>();
};

template <typename... BAs>
using tau_source_terminal_extractor_t = decltype(tau_source_terminal_extractor);

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

	std::basic_stringstream<char>& stream;
	const extractor_t& extractor;
};

// converts a sp_tau_node<...> to a string skipping the nodes that satisfy the
// given predicate.
template <typename extractor_t, typename predicate_t, typename node_t>
std::string make_string_with_skip(const extractor_t& extractor, predicate_t& skip, const node_t& n) {
	std::basic_stringstream<char> ss;
	stringify<extractor_t, node_t> sy(extractor, ss);
	post_order_tree_traverser<stringify<extractor_t, node_t>, predicate_t, node_t>(sy, skip)(n);
	return ss.str();
}

// converts a sp_tau_node<...> to a string.
template <typename extractor_t, typename node_t>
std::string make_string(const extractor_t& extractor, const node_t& n) {
	return make_string_with_skip<extractor_t, all_t<node_t>, node_t>(extractor, all<node_t>, n);
}

// bind the given, using a binder, the constants of the a given sp_tau_node<...>.
template<typename binder_t, typename... BAs>
struct bind_transformer {

	bind_transformer(binder_t& binder) : binder(binder) {}

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
	binder_t binder;
};

// is not a whitespace predicate
template<typename... BAs>
static const auto not_whitespace_predicate = [](const sp_tau_node<BAs...>& n) {
	return n->value.index() != 0
		|| !get<0>(n->value).nt()
		|| get<0>(n->value).n() != tau_parser::ws;
};

template<typename... BAs>
using not_whitespace_predicate_t = decltype(not_whitespace_predicate<BAs...>);

// binds the constants of a given binding using the label specified
// in the code and according to a map from labels to constants in the BAs...
template<typename... BAs>
struct name_binder {

	name_binder(const bindings<BAs...>& bs) : bs(bs) {}

	sp_tau_node<BAs...> bind(const sp_tau_node<BAs...>& n) const {
		// FIXME check if the node is a named binding one
		auto bn = make_string_with_skip<
				tau_node_terminal_extractor_t<BAs...>,
				not_whitespace_predicate_t<BAs...>,
				sp_tau_node<BAs...>>(
			tau_node_terminal_extractor<BAs...>,
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

	factory_binder(factory_t& factory) : factory(factory) {}

	sp_tau_node<BAs...> bind(const sp_tau_node<BAs...>& n) const {
		// FIXME check that the node is a factory binding one
		if(auto type = find_top(n, is_non_terminal<tau_parser::type, BAs...>); type) {
			// the factory take two arguments, the first is the type and the
			// second is the node representing the constant.
			auto type_name = make_string_with_skip<
					tau_node_terminal_extractor_t<BAs...>,
					not_whitespace_predicate_t<BAs...>,
					sp_tau_node<BAs...>>(
				tau_node_terminal_extractor<BAs...>,
				not_whitespace_predicate<BAs...>, type.value());
			return factory.build(type_name, n);
		}
		return n;
	}

	factory_t& factory;
};

// check if the type is unresolved.
template<typename... BAs>
static const auto is_unresolved_predicate = [](const sp_tau_node<BAs...>& n) {
	if (!is_non_terminal<tau_parser::type, BAs...>(n)) return false;
	auto type_name = make_string_with_skip<
			tau_node_terminal_extractor_t<BAs...>,
			not_whitespace_predicate_t<BAs...>,
			sp_tau_node<BAs...>>(
			tau_node_terminal_extractor<BAs...>,
		not_whitespace_predicate<BAs...>, n);
	return type_name.empty();
};

template<typename... BAs>
using is_unresolved_predicate_t = decltype(is_unresolved_predicate<BAs...>);

// check if the type is resolved.
template<typename... BAs>
static const auto is_resolved_predicate = [](const sp_tau_node<BAs...>& n) {
	if (!is_non_terminal<tau_parser::type, BAs...>(n)) return false;
	auto type_name = make_string_with_skip<
			tau_node_terminal_extractor_t<BAs...>,
			not_whitespace_predicate_t<BAs...>,
			sp_tau_node<BAs...>>(
			tau_node_terminal_extractor<BAs...>,
		not_whitespace_predicate<BAs...>, n);
	return !type_name.empty();
};

template<typename... BAs>
using is_resolved_predicate_t = decltype(is_resolved_predicate<BAs...>);

// check if the given expression is unresolved, i.e. contains a type which
// is not resolved.
template<typename... BAs>
std::optional<sp_tau_node<BAs...>> is_unresolved(const sp_tau_node<BAs...>& n) {
	return find_top(n, is_unresolved_predicate<BAs...>);
}

// resolve all the unresolved types in the given expression. the types are
// resolved to the first bottom type resolved in the expression.
template<typename... BAs>
sp_tau_node<BAs...> resolve_type(const sp_tau_node<BAs...>& n) {
	// REVIEW (MEDIUM) should this be called only with bfs?
	// if (!is_non_terminal<tau_parser::bf, BAs...>(n)) return n;
	if (auto unresolved = is_unresolved(n); unresolved) {
		// always we have type information or it is not needed at all
		if (auto type = find_bottom(n, is_resolved_predicate<BAs...>); type) {

			// TODO (HIGH) this should be extracted to a function in rewriting
			//
			// there are few places where we deal with a similar pattern, this
			// should be extracted to a function in rewriting.h named replace
			// which takes as arguments the node and thhe changes.
			std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> change;
			change[unresolved.value()] = type.value();
			replace_transformer<sp_tau_node<BAs...>> replace{change};
			return post_order_traverser<
					replace_transformer<sp_tau_node<BAs...>>,
					all_t<sp_tau_node<BAs...>>,
					sp_tau_node<BAs...>>(
				replace , all<sp_tau_node<BAs...>>)(n);
		}
	}
	return n;
}

// resolve all the unresolved types in the given statement.
template<typename binder_t, typename... BAs>
sp_tau_node<BAs...> resolve_types(const sp_tau_node<BAs...> source) {
	std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes;
	for (const auto& bf: select_top(source, is_non_terminal<BAs...>(tau_parser::bf))) {
		if (auto rbf = resolve_type(bf); rbf != bf) changes[bf] = rbf;
	}
	replace_transformer<sp_tau_node<BAs...>> rt(changes);
	return post_order_traverser<
			replace_transformer<sp_tau_node<BAs...>>,
			all_t<sp_tau_node<BAs...>>,
			sp_tau_node<BAs...>>(
		rt, all<sp_tau_node<BAs...>>)(source);
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
template<size_t rule_t, size_t matcher_t, size_t body_t, typename... BAs>
tau_rule<BAs...> make_rule(sp_tau_node<BAs...>& rule) {
	auto matcher = rule | rule_t | matcher_t| only_child_extractor<BAs...> | optional_value_extractor<sp_tau_node<BAs...>>;
	auto body = rule | rule_t | body_t | only_child_extractor<BAs...> | optional_value_extractor<sp_tau_node<BAs...>>;
	return { matcher, body };
}


// creates a specific rule from a generic rule.
template<typename... BAs>
tau_rule<BAs...> make_rule(sp_tau_node<BAs...>& rule) {
	// TODO (IMPORTANT) check that the rule is well formed an raise an error otherwise
	auto type = only_child_extractor<BAs...>(rule) | non_terminal_extractor<BAs...> | optional_value_extractor<size_t>;
	switch (type) {
	case tau_parser::bf_rule: return make_rule<tau_parser::bf_rule, tau_parser::bf_matcher, tau_parser::bf_body, BAs...>(rule);
	case tau_parser::wff_rule: return make_rule<tau_parser::wff_rule, tau_parser::wff_matcher, tau_parser::wff_body, BAs...>(rule);
	case tau_parser::cbf_rule: return make_rule<tau_parser::cbf_rule, tau_parser::cbf_matcher, tau_parser::cbf_body, BAs...>(rule);
	default: assert(false); return {};
	};
}

// create a set of rules from a given tau source.
template<typename... BAs>
rules<BAs...> make_rules(sp_tau_node<BAs...>& tau_source) {
	rules<BAs...> rs;
	// TODO (HIGH) change call to select by operator|| and operator|
	for (auto& r: select_top(tau_source, is_non_terminal<tau_parser::rule, BAs...>))
		rs.push_back(make_rule<BAs...>(r));
	return rs;
}

sp_tau_source_node clean_tau_source(const sp_tau_source_node& tau_source) {
	// return tau_source;
	// FIXME (LOW) fix the trim implementation
	return trim_top<
			is_non_essential_source_t,
			tau_source_sym,
			sp_tau_source_node>(
		tau_source, is_non_essential_source);
}

// make a tau source from the given source code string.
sp_tau_source_node make_tau_source(const std::string& source) {
	using parse_lit = idni::lit<char, char>;
	using parse_location = std::array<size_t, 2UL>;
	using parse_symbol = std::pair<parse_lit, parse_location>;
	return make_node_from_string<
			tau_parser,
			drop_location_t<parse_symbol, tau_source_sym>,
			parse_symbol,
			tau_source_sym>(
		drop_location<parse_symbol, tau_source_sym>, source);
}

// create tau code from tau source
template<typename... BAs>
sp_tau_node<BAs...> make_tau_code(sp_tau_source_node& tau_source) {
	tauify<BAs...> tf;
	map_transformer<tauify<BAs...>, sp_tau_source_node, sp_tau_node<BAs...>> transform(tf);
	return post_order_traverser<
			map_transformer<tauify<BAs...>, sp_tau_source_node, sp_tau_node<BAs...>>,
			all_t<sp_tau_source_node>,
			sp_node<tau_source_sym>,
			sp_tau_node<BAs...>>(
		transform, all<sp_tau_source_node>)(tau_source);
}

// make a library from the given tau source.
template<typename... BAs>
library<BAs...> make_library(sp_tau_source_node& tau_source) {
	auto lib = make_tau_code<BAs...>(tau_source);
	return make_rules(lib);
}

// make a formula from the given tau source and binder.
template<typename binder_t, typename... BAs>
formula<BAs...> make_formula_using_binder(sp_tau_source_node& tau_source, binder_t& binder) {
	auto src = make_tau_code<BAs...>(tau_source);
	auto unbinded_main = src | tau_parser::formula | tau_parser::main | tau_parser::wff | optional_value_extractor<sp_tau_node<BAs...>>;
	auto binded_main = post_order_traverser<
			binder_t,
			all_t<sp_tau_node<BAs...>>,
			sp_tau_node<BAs...>>(
		binder, all<sp_tau_node<BAs...>>)(unbinded_main);
	auto rules = make_rules<BAs...>(binded_main);
	return { rules, binded_main };
}

// make a formula from the given tau source and bindings.
template<typename... BAs>
formula<BAs...> make_formula_using_bindings(sp_tau_source_node& tau_source, const bindings<BAs...>& bindings) {
	name_binder<BAs...> nb(bindings);
	bind_transformer<name_binder<BAs...>, BAs...> bs(nb);
	return make_formula_using_binder<bind_transformer<name_binder<BAs...>, BAs...>, BAs...>(tau_source, bs);
}

// make a formula from the given tau source and bindings.
template<typename factory_t, typename... BAs>
formula<BAs...> make_formula_using_factory(sp_tau_source_node& tau_source, factory_t& factory) {
	bind_transformer<factory_t, BAs...> bs(factory);
	return make_formula_using_binder<bind_transformer<factory_t, BAs...>, BAs...>(tau_source, bs);
}

// apply one tau rule to the given expression
// IDEA maybe this could be operator|
template<typename... BAs>
sp_tau_node<BAs...> tau_apply(tau_rule<BAs...>& r, sp_tau_node<BAs...>& n) {
	// IDEA maybe we could traverse only once
	auto nn = apply_with_skip<
			sp_tau_node<BAs...>,
			none_t<sp_tau_node<BAs...>>,
			is_capture_t<BAs...>,
			is_non_essential_t<BAs...>>(
		r, n , none<sp_tau_node<BAs...>>, is_capture<BAs...>, is_non_essential<BAs...>);
	if (auto cbs = select_all(nn, is_callback<BAs...>); !cbs.empty()) {
		callback_applier<BAs...> cb_applier;
		std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes;
		for (auto& cb : cbs) {
			auto nnn = cb_applier(cb);
			changes[cb] = nnn;
		}
		replace_transformer<sp_tau_node<BAs...>> replace{changes};
		return post_order_traverser<
				replace_transformer<sp_tau_node<BAs...>>,
				all_t<sp_tau_node<BAs...>>,
				sp_tau_node<BAs...>>(
			replace , all<sp_tau_node<BAs...>>)(nn);
	}
	return nn;
}

// apply the given rules to the given expression
// IDEA maybe this could be operator|
template<typename... BAs>
sp_tau_node<BAs...> tau_apply(rules<BAs...>& rs, sp_tau_node<BAs...>& n) {
	if (rs.empty()) return n;
	sp_tau_node<BAs...> nn = n;
	for (auto& r : rs) nn = tau_apply<BAs...>(r, nn);
	return nn;
}

template<typename... BAs>
tau<BAs...> make_tau() {
	// TODO (IMPORTANT) give a proper implementation in the future
	return tau<BAs...>();
}

// make a library from the given tau source string.
template<typename... BAs>
library<BAs...> make_library(const std::string& source) {
	auto tau_source = make_tau_source(source);
	return make_library<BAs...>(tau_source);
}

// creates a specific builder from a sp_tau_node.
template<typename... BAs>
builder<BAs...> make_builder(const sp_tau_node<BAs...>& builder) {
	return {builder | tau_parser::builder | tau_parser::captures | optional_value_extractor<sp_tau_node<BAs...>>,
			builder | tau_parser::builder | tau_parser::builder_body | only_child_extractor<BAs...> | optional_value_extractor<sp_tau_node<BAs...>>};
}

// create a builder from a given tau source.
template<typename... BAs>
builder<BAs...> make_builder(sp_tau_source_node& tau_source) {
	auto builder = make_tau_code<BAs...>(tau_source);
	return make_builder(builder);
}

// make a builder from the given tau source string.
template<typename... BAs>
builder<BAs...> make_builder(const std::string& source) {
	auto tau_source = make_tau_source(source);
	return make_builder<BAs...>(tau_source);
}

// apply the given builder to the given expression
template<typename... BAs>
sp_tau_node<BAs...> tau_apply_builder(const builder<BAs...>& b, std::vector<sp_tau_node<BAs...>>& n) {
	std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes;
	std::vector<sp_tau_node<BAs...>> vars = b.first || tau_parser::capture;
	for (size_t i = 0; i < vars.size(); ++i) changes[vars[i]] = n[i];
	replace_transformer<sp_tau_node<BAs...>> replace{changes};
	return post_order_traverser<
			replace_transformer<sp_tau_node<BAs...>>,
			all_t<sp_tau_node<BAs...>>,
			sp_tau_node<BAs...>>(
		replace , all<sp_tau_node<BAs...>>)(b.second);
}


// make a formula from the given tau source and bindings.
template<typename factory_t, typename... BAs>
formula<BAs...> make_formula_using_factory(const std::string& source, factory_t& factory) {
	auto tau_source = make_tau_source(source);
	return make_formula_using_factory<factory_t, BAs...>(tau_source, factory);
}

// make a formula from the given tau source and bindings.
template<typename... BAs>
formula<BAs...> make_formula_using_bindings(const std::string& source, const bindings<BAs...>& bindings) {
	auto tau_source = make_tau_source(source);
	name_binder<BAs...> nb(bindings);
	bind_transformer<name_binder<BAs...>, BAs...> bs(nb);
	return make_formula_using_bindings<
			bind_transformer<name_binder<BAs...>, BAs...>,
			BAs...>(
		tau_source, bs);
}


} // namespace idni::tau

//
// operators << to pretty print the tau language related types
//

// TODO (HIGH) << for tau_source_sym
// TODO (HIGH) << for tau_source_node
// TODO (HIGH) << for tau_source_node
// TODO (HIGH) << for sp_tau_source_node
// TODO (HIGH) << for tau_sym
// TODO (HIGH) << for sp_tau_node
// TODO (HIGH) << for tau_rule
// TODO (HIGH) << for rule
// TODO (HIGH) << for rules
// TODO (HIGH) << for statement
// TODO (HIGH) << for library
// TODO (HIGH) << for bindings
// TODO (HIGH) << for formulas



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