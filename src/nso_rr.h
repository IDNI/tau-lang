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

// TODO (LOW) rename file to msnso_rr.h
// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

#ifndef __NSO_RR_H__
#define __NSO_RR_H__

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
#include <numeric>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#include "bool_ba.h"
#include "variant_ba.h"
#include "splitter_types.h"
#include "parser.h"
#include "utils.h"
#include "../parser/tau_parser.generated.h"
#include "rewriting.h"

#ifdef TAU_MEASURE
#include "measure.h"
#endif // TAU_MEASURE

using namespace idni::rewriter;
using namespace tau_parser_data;

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
using tau_sym = std::variant<tau_source_sym, std::variant<BAs...>, size_t>;

template <typename... BAs>
using sp_tau_node = sp_node<tau_sym<BAs...>>;

template <typename... BAs>
bool is_non_terminal(const size_t, const sp_tau_node<BAs...>&);

// We overload the == operator for sp_tau_node in order to store additional data
// which is not taken into account for the quality check
template<typename... BAs>
bool operator==(const sp_tau_node<BAs...> &l, const sp_tau_node<BAs...>& r) {
	if (r == nullptr && l == nullptr) return true;
	if (r == nullptr || l == nullptr) return false;

	if (std::addressof(*l) == std::addressof(*r)) return true;

	bool l_has_extra = false;
	if (!l->child.empty())
		if (is_non_terminal(tau_parser::extra, l->child.back()))
			l_has_extra = true;
	bool r_has_extra = false;
	if (!r->child.empty())
		if (is_non_terminal(tau_parser::extra, r->child.back()))
			r_has_extra = true;

	if (!l_has_extra && !r_has_extra)
		return (l->value == r->value && l->child == r->child);
	if (l_has_extra && !r_has_extra) {
		if (l->child.size() != r->child.size() + 1) return false;
		for (int i = 0; i < (int_t)r->child.size(); ++i) {
			if (!(l->child[i] == r->child[i]))
				return false;
		}
		return l->value == r->value;
	}
	if (!l_has_extra && r_has_extra) {
		if (l->child.size() + 1 != r->child.size()) return false;
		for (int i = 0; i < (int_t)l->child.size(); ++i) {
			if (!(l->child[i] == r->child[i]))
				return false;
		}
		return l->value == r->value;
	}
	if (l_has_extra && r_has_extra) {
		if (l->child.size() != r->child.size()) return false;
		for (int i = 1; i < ((int_t)l->child.size()); ++i) {
			if (!(l->child[i-1] == r->child[i-1]))
				return false;
		}
		return l->value == r->value;
	}
	// All cases are covered above
	assert(false);
	return false;
}

template<typename... BAs>
bool operator!=(const sp_tau_node<BAs...>& l, const sp_tau_node<BAs...>& r) {
	return !(l == r);
}

template<typename... BAs>
using nso = sp_tau_node<BAs...>;

template <typename node_t>
using rec_relation = rule<node_t>;

// IDEA maybe we could define a wrapper for recursive rules and rewriting rules that
// call the appropriate apply method. This would play also nice with the builders
// defined in the normalizer.

template <typename... BAs>
using builder = rule<nso<BAs...>>;

// defines a vector of rules in the tau language, the order is important as it defines
// the order of the rules in the rewriting process of the tau language.
template <typename node_t>
using rules = std::vector<rule<node_t>>;

// defines a vector of rec. relations in the tau language, the order is important as it defines
// the order of the rec relations in the rewriting process of the tau language.
template <typename node_t>
using rec_relations = std::vector<rec_relation<node_t>>;

// a library is a set of rules to be applied in the rewriting process of the tau
// language, the order of the rules is important.
template <typename node_t>
using library = rules<node_t>;

// bindings map tau_source constants (strings) into elements of the boolean algebras.
template<typename... BAs>
using bindings = std::map<std::string, std::variant<BAs...>>;

// an rr is a set of rules and a main.
template<typename type_t>
struct rr {

	rr(const rules<type_t>& rec_relations, const type_t& main)
				: rec_relations(rec_relations), main(main) {};
	rr(const type_t& main) : main(main) {};

	auto operator<=>(const rr&) const = default;

	rules<type_t> rec_relations;
	type_t main;
};

//
// predicates and functions related to the tau language
//

// check if a node is a non terminal node
template<typename... BAs>
bool is_non_terminal_node(const node<tau_sym<BAs...>>& s) {
	return std::holds_alternative<tau_source_sym>(s.value)
		&& get<tau_source_sym>(s.value).nt();
}

template<typename... BAs>
bool is_non_terminal_node(const sp_tau_node<BAs...>& n) {
	return std::holds_alternative<tau_source_sym>(n->value)
					&& get<tau_source_sym>(n->value).nt();
};

template<typename... BAs>
size_t get_non_terminal_node(const tau_sym<BAs...>& s) {
	return get<tau_source_sym>(s).n();
}

template<typename... BAs>
size_t get_non_terminal_node(const node<tau_sym<BAs...>>& n) {
	return get_non_terminal_node(n.value);
}

template<typename... BAs>
size_t get_non_terminal_node(const sp_tau_node<BAs...>& n) {
	return get_non_terminal_node(*n);
}

// factory method for is_non_terminal_node predicate
template<typename... BAs>
std::function<bool(const sp_tau_node<BAs...>&)> is_non_terminal_node() {
	return [](const sp_tau_node<BAs...>& n) {
		return is_non_terminal_node<BAs...>(n); };
}

// check if the node is the given non terminal
template <typename... BAs>
bool is_non_terminal(const size_t nt, const sp_tau_node<BAs...>& n) {
	return is_non_terminal_node<BAs...>(n)
				&& get<tau_source_sym>(n->value).n() == nt;
}

template <typename... BAs>
bool is_non_terminal_sym(const size_t nt, const tau_sym<BAs...>& s) {
	return is_non_terminal_sym<BAs...>(s)
				&& get<tau_source_sym>(s).n() == nt;
}

// check if the node is the given non terminal (template approach)
template <size_t nt, typename...BAs>
bool is_non_terminal(const sp_tau_node<BAs...>& n) {
	return is_non_terminal<BAs...>(nt, n);
}

template <size_t nt, typename...BAs>
bool is_non_terminal_sym(const tau_sym<BAs...>& s) {
	return is_non_terminal_sym<BAs...>(nt, s);
}

// factory method for is_non_terminal predicate
template<typename... BAs>
std::function<bool(const sp_tau_node<BAs...>&)> is_non_terminal(const size_t nt)
{
	return [nt](const sp_tau_node<BAs...>& n) {
		return is_non_terminal<BAs...>(nt, n); };
}

// check if a node is a terminal
template<typename... BAs>
bool is_terminal_node(const sp_tau_node<BAs...>& n) {
	return std::holds_alternative<tau_source_sym>(n->value)
					&& !get<tau_source_sym>(n->value).nt();
};

// factory method for is_terminal_node predicate
template<typename... BAs>
std::function<bool(const sp_tau_node<BAs...>&)> is_terminal_node() {
	return [](const sp_tau_node<BAs...>& n) {
		return is_terminal_node<BAs...>(n); };
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
	return [c](const sp_tau_node<BAs...>& n) {
		return is_terminal<BAs...>(c, n); };
}

template<typename... BAs>
static const auto is_capture = [](const sp_tau_node<BAs...>& n) {
	return std::holds_alternative<tau_source_sym>(n->value)
		&& get<tau_source_sym>(n->value).nt()
		&& ( get<tau_source_sym>(n->value).n() == tau_parser::capture);
};

template<typename... BAs>
using is_capture_t = decltype(is_capture<BAs...>);

template<typename... BAs>
static const auto is_var_or_capture = [](const nso<BAs...>& n) {
	return std::holds_alternative<tau_source_sym>(n->value)
		&& get<tau_source_sym>(n->value).nt()
		&& ((get<tau_source_sym>(n->value).n() == tau_parser::capture)
			|| (get<tau_source_sym>(n->value).n() ==
							tau_parser::variable));
};

template<typename... BAs>
static const auto is_quantifier = [](const nso<BAs...>& n) {
	if (!std::holds_alternative<tau_source_sym>(n->value)
			|| !get<tau_source_sym>(n->value).nt()) return false;
	auto nt = get<tau_source_sym>(n->value).n();
	return nt == tau_parser::wff_all
		|| nt == tau_parser::wff_ex;
};

template<typename... BAs>
using is_var_or_capture_t = decltype(is_var_or_capture<BAs...>);

template<typename...BAs>
static const auto is_callback = [](const sp_tau_node<BAs...>& n) {
	if (!std::holds_alternative<tau_source_sym>(n->value)
			|| !get<tau_source_sym>(n->value).nt()) return false;
	auto nt = get<tau_source_sym>(n->value).n();
	return nt == tau_parser::bf_normalize_cb
		|| nt == tau_parser::wff_has_clashing_subformulas_cb
		|| nt == tau_parser::bf_has_subformula_cb
		|| nt == tau_parser::wff_has_subformula_cb
		|| nt == tau_parser::wff_remove_existential_cb
		|| nt == tau_parser::bf_remove_funiversal_cb
		|| nt == tau_parser::bf_remove_fexistential_cb;
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
std::optional<sp_tau_node<BAs...>> operator|(const node<tau_sym<BAs...>>& n,
	const size_t nt)
{
	auto v = n.child
		| std::ranges::views::filter(is_non_terminal<BAs...>(nt))
		| std::ranges::views::take(1);
	return v.empty() ? std::optional<sp_tau_node<BAs...>>()
			: std::optional<sp_tau_node<BAs...>>(v.front());
}

template <typename... BAs>
std::optional<sp_tau_node<BAs...>> operator|(const sp_tau_node<BAs...>& n,
	const size_t nt)
{
	return *n | nt;
}

template <typename... BAs>
std::optional<sp_tau_node<BAs...>> operator|(
	const std::optional<sp_tau_node<BAs...>>& n, const size_t nt)
{
// #ifdef DEBUG
// 	static auto last_nt = nt;
// 	auto x = n ? n.value() | nt : n;
// 	if (!n.has_value()) BOOST_LOG_TRIVIAL(warning)
// 		<< "value-less optional tau_parser::"
// 		<< tau_parser::instance().name(last_nt)
// 		<< " | tau_parser::"
// 		<< tau_parser::instance().name(nt);
// 	last_nt = nt;
// 	return x;
// #endif // DEBUG
	return n ? n.value() | nt : n;
}

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator|(
	const std::vector<sp_tau_node<BAs...>>& v, const size_t nt)
{
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
std::vector<sp_tau_node<BAs...>> operator||(const node<tau_sym<BAs...>>& n,
	const tau_parser::nonterminal nt)
{
	// IDEA use ::to to get a vector when gcc and clang implement it in the future
	std::vector<sp_tau_node<BAs...>> nv;
	nv.reserve(n.child.size());
	for (const auto& c: n.child
		| std::ranges::views::filter(is_non_terminal<BAs...>(nt)))
							nv.push_back(c);
	return nv;
}

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(const sp_tau_node<BAs...>& n,
	const tau_parser::nonterminal nt)
{
	return *n || nt;
}

template <typename... BAs>
std::vector<sp_tau_node<BAs...>>  operator||(
	const std::optional<sp_tau_node<BAs...>>& n,
	const tau_parser::nonterminal nt)
{
	// IDEA use ::to to get a vector when gcc and clang implement it in the future
	if (n) return n.value() || nt;
	return {};
}

// TODO (LOW) remove get_nodes if possible and use operator|| instead
template <typename... BAs>
std::vector<sp_tau_node<BAs...>> get_nodes(const tau_parser::nonterminal nt,
	const sp_tau_node<BAs...>& n)
{
	return n || nt;
}

template <size_t nt, typename... BAs>
std::vector<sp_tau_node<BAs...>> get_nodes(const sp_tau_node<BAs...>& n) {
	return n || nt;
}

template <typename... BAs>
auto get_nodes(const tau_parser::nonterminal nt) {
	return [nt](const sp_tau_node<BAs...>& n) {
		return get_nodes<BAs...>(nt, n); };
}

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(
	const std::vector<sp_tau_node<BAs...>>& v,
	const tau_parser::nonterminal nt)
{
	// IDEA use ::to to get a vector when gcc and clang implement it in the future
	std::vector<sp_tau_node<BAs...>> nv; nv.reserve(v.size());
	for (const auto& n : v
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
static const auto value_extractor = [](const sp_tau_node<BAs...>& n)
	-> tau_sym<BAs...> { return n->value; };

template<typename... BAs>
using value_extractor_t = decltype(value_extractor<BAs...>);

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(
	const std::vector<sp_tau_node<BAs...>>& v,
	const value_extractor_t<BAs...> e)
{
	std::vector<std::variant<BAs...>> nv;
	for (const auto& n : v | std::ranges::views::transform(e))
		if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<char> operator|(const std::optional<sp_tau_node<BAs...>>& o,
	const value_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value()) : std::optional<char>();
}

// returns an optional containing the terminal of the node if possible
template<typename... BAs>
static const auto terminal_extractor = [](const sp_tau_node<BAs...>& n)
	-> std::optional<char>
{
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
std::vector<sp_tau_node<BAs...>> operator||(
	const std::vector<sp_tau_node<BAs...>>& v,
	const terminal_extractor_t<BAs...> e)
{
	std::vector<std::variant<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e))
		if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<char> operator|(const std::optional<sp_tau_node<BAs...>>& o,
	const terminal_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value()) : std::optional<char>();
}

// returns an optional containing the non terminal of the node if possible
template<typename... BAs>
static const auto non_terminal_extractor = [](const sp_tau_node<BAs...>& n)
	-> std::optional<size_t>
{
	if (std::holds_alternative<tau_source_sym>(n->value)
			&& get<tau_source_sym>(n->value).nt())
		return std::optional<size_t>(get<tau_source_sym>(n->value).n());
	return std::optional<size_t>();
};

template<typename... BAs>
using non_terminal_extractor_t = decltype(non_terminal_extractor<BAs...>);

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(
	const std::vector<sp_tau_node<BAs...>>& v,
	const non_terminal_extractor_t<BAs...> e)
{
	return v | std::ranges::views::transform(e);
}

template <typename... BAs>
std::optional<size_t> operator|(const std::optional<sp_tau_node<BAs...>>& o,
	const non_terminal_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value()) : std::optional<size_t>();
}

template <typename... BAs>
std::optional<size_t> operator|(const sp_tau_node<BAs...>& o,
	const non_terminal_extractor_t<BAs...> e)
{
	return e(o);
}

// returns an optional containing size_t of the node if possible
template <typename... BAs>
static const auto size_t_extractor = [](const sp_tau_node<BAs...>& n)
	-> std::optional<size_t>
{
	if (std::holds_alternative<size_t>(n->value))
		return std::optional<size_t>(std::get<size_t>(n->value));
	return std::optional<size_t>();
};
template<typename... BAs>
using size_t_extractor_t = decltype(size_t_extractor<BAs...>);

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(
	const std::vector<sp_tau_node<BAs...>>& v,
	const size_t_extractor_t<BAs...> e)
{
	return v | std::ranges::views::transform(e);
}

template <typename... BAs>
std::optional<size_t> operator|(const std::optional<sp_tau_node<BAs...>>& o,
	const size_t_extractor_t<BAs...> e)
{
	return o.has_value() ? e(o.value()) : std::optional<size_t>();
}

template <typename... BAs>
std::optional<size_t> operator|(const sp_tau_node<BAs...>& o,
	const size_t_extractor_t<BAs...> e)
{
	return e(o);
}

// returns an optional containing the offset of the node if possible
template <typename...BAs>
static const auto& offset_extractor = size_t_extractor<BAs...>;
template <typename...BAs>
using offset_extractor_t = size_t_extractor_t<BAs...>;

// returns an optional containing the bas... of the node if possible
template<typename... BAs>
static const auto ba_extractor = [](const sp_tau_node<BAs...>& n)
	-> std::optional<std::variant<BAs...>>
{
	if (std::holds_alternative<std::variant<BAs...>>(n->value))
		return std::optional<std::variant<BAs...>>(
					get<std::variant<BAs...>>(n->value));
	return std::optional<std::variant<BAs...>>();
};

template<typename... BAs>
using ba_extractor_t = decltype(ba_extractor<BAs...>);

template <typename... BAs>
std::vector<std::variant<BAs...>> operator||(
	const std::vector<sp_tau_node<BAs...>>& v,
	const ba_extractor_t<BAs...> e)
{
	std::vector<std::variant<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e))
		if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<std::variant<BAs...>> operator|(
	const std::optional<sp_tau_node<BAs...>>& o,
	const ba_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value())
				: std::optional<std::variant<BAs...>>();
}

template <typename... BAs>
std::optional<std::variant<BAs...>> operator|(const sp_tau_node<BAs...>& o,
	const ba_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return e(o);
}

// returns the only child of a node
template <typename... BAs>
static const auto only_child_extractor = [](const sp_tau_node<BAs...>& n)
	-> std::optional<sp_tau_node<BAs...>>
{
	if (n->child.size() != 1) return std::optional<sp_tau_node<BAs...>>();
	return std::optional<sp_tau_node<BAs...>>(n->child[0]);
};

template<typename... BAs>
using only_child_extractor_t = decltype(only_child_extractor<BAs...>);

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(
	const std::vector<sp_tau_node<BAs...>>& v,
	const only_child_extractor_t<BAs...> e)
{
	std::vector<sp_tau_node<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e))
		if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<sp_tau_node<BAs...>> operator|(
	const std::optional<sp_tau_node<BAs...>>& o,
	const only_child_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value())
		: std::optional<sp_tau_node<BAs...>>();
}

// IDEA maybe unify all the implementations dealing with operator| and operator|| for extractors
template <typename... BAs>
std::optional<sp_tau_node<BAs...>> operator|(const sp_tau_node<BAs...>& o,
	const only_child_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return e(o);
}

// check if the node is the given non terminal
template <typename... BAs>
bool is_child_non_terminal(const size_t nt, const sp_tau_node<BAs...>& n) {
	auto child = n | only_child_extractor<BAs...>;
	return child.has_value() && is_non_terminal<BAs...>(nt, child.value());
}

// check if the node is the given non terminal (template approach)
template <size_t nt, typename...BAs>
bool is_child_non_terminal(const sp_tau_node<BAs...>& n) {
	return is_child_non_terminal<BAs...>(nt, n);
}

// factory method for is_non_terminal predicate
template<typename... BAs>
std::function<bool(const sp_tau_node<BAs...>&)>
	is_child_non_terminal(const size_t nt)
{
	return [nt](const sp_tau_node<BAs...>& n) {
		return is_child_non_terminal<BAs...>(nt, n); };
}

// returns the first child of a node
template <typename... BAs>
static const auto first_child_extractor = [](const sp_tau_node<BAs...>& n)
	-> std::optional<sp_tau_node<BAs...>>
{
	if (n->child.size() == 0) return std::optional<sp_tau_node<BAs...>>();
	return std::optional<sp_tau_node<BAs...>>(n->child[0]);
};

template<typename... BAs>
using first_child_extractor_t = decltype(first_child_extractor<BAs...>);

template <typename... BAs>
std::vector<sp_tau_node<BAs...>> operator||(
	const std::vector<sp_tau_node<BAs...>>& v,
	const first_child_extractor_t<BAs...> e)
{
	std::vector<sp_tau_node<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e))
		if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<sp_tau_node<BAs...>> operator|(
	const std::optional<sp_tau_node<BAs...>>& o,
	const first_child_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value())
		: std::optional<sp_tau_node<BAs...>>();
}

// IDEA maybe unify all the implementations dealing with operator| and operator|| for extractors
template <typename... BAs>
std::optional<sp_tau_node<BAs...>> operator|(const sp_tau_node<BAs...>& o,
	const first_child_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return e(o);
}


template <typename T>
static const auto optional_value_extractor = [](const std::optional<T>& o) -> T{
	if (!o) BOOST_LOG_TRIVIAL(error)
		<< "parse tree traversal: bad optional access";
	return o.value();
};

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
		| std::ranges::views::filter(
				&std::optional<tau_sym<BAs...>>::has_value);
}


// converts a tau_source_sym to a tau_sym, it is used to convert from
// tau_source to tau_node...
template <typename... BAs>
struct tauify {

	tau_sym<BAs...> operator()(const tau_source_sym& n) const {
		tau_sym<BAs...> nn(n);
		return nn;
	}
};

// extracts terminal from sp_tau_node
template <typename... BAs>
auto tau_node_terminal_extractor = [](const sp_tau_node<BAs...>& n)
	-> std::optional<char>
{
	if (n->value.index() == 0
			&& !get<0>(n->value).nt()
			&& !get<0>(n->value).is_null())
		return std::optional<char>(get<0>(n->value).t());
	return std::optional<char>();
};

template <typename... BAs>
using tau_node_terminal_extractor_t =
				decltype(tau_node_terminal_extractor<BAs...>);

// extracts terminal from sp_tau_source_node
extern std::function<std::optional<char>(const sp_tau_source_node& n)>
	tau_source_terminal_extractor;

template <typename... BAs>
using tau_source_terminal_extractor_t = decltype(tau_source_terminal_extractor);

// adds terminal symbols to a given stream, used in conjuction with usual
// traversals (see make_string_* methods).
template <typename extractor_t, typename node_t>
struct stringify {

	stringify(const extractor_t& extractor,
		std::basic_stringstream<char>& stream)
			: stream(stream), extractor(extractor) {}

	node_t operator()(const node_t& n) {
		if (auto str = extractor(n); str) stream << str.value();
		return n;
	}

	std::basic_stringstream<char>& stream;
	const extractor_t& extractor;
};

// converts a sp_tau_node<...> to a string.
template <typename extractor_t, typename node_t>
std::string make_string(const extractor_t& extractor, const node_t& n) {
	std::basic_stringstream<char> ss;
	stringify<extractor_t, node_t> sy(extractor, ss);
	post_order_tree_traverser<stringify<extractor_t, node_t>,
				all_t, node_t>(sy, all)(n);
	return ss.str();
}

template<typename... BAs>
sp_tau_node<BAs...> trim(const sp_tau_node<BAs...>& n) {
	return n->child[0];
}

template<typename... BAs>
sp_tau_node<BAs...> trim2(const sp_tau_node<BAs...>& n) {
	return n->child[0]->child[0];
}

template<typename... BAs>
sp_tau_node<BAs...> wrap(tau_parser::nonterminal nt,
	const std::vector<sp_tau_node<BAs...>>& nn)
{
	return make_node<tau_sym<BAs...>>(
		tau_parser::instance().literal(nt), nn);
}

template<typename... BAs>
sp_tau_node<BAs...> wrap(tau_parser::nonterminal nt,
	const std::initializer_list<sp_tau_node<BAs...>> ch)
{
	return wrap(nt, std::vector<sp_tau_node<BAs...>>(ch));
}

template<typename... BAs>
sp_tau_node<BAs...> wrap(tau_parser::nonterminal nt,
	const sp_tau_node<BAs...>& n)
{
	return wrap(nt, { n });
}

template<typename... BAs>
sp_tau_node<BAs...> wrap(tau_parser::nonterminal nt,
	const sp_tau_node<BAs...>& c1, const sp_tau_node<BAs...>& c2)
{
	return wrap(nt, { c1, c2 });
}

template<typename... BAs>
sp_tau_node<BAs...> wrap(tau_parser::nonterminal nt,
	const std::string& terminals) {
	std::vector<sp_tau_node<BAs...>> children;
	for (const auto& c : terminals)
		children.emplace_back(make_node<tau_sym<BAs...>>(
			tau_source_sym(c), {}));
	return wrap(nt, children);
}

// bind the given, using a binder, the constants of the a given sp_tau_node<...>.
template<typename binder_t, typename... BAs>
struct bind_transformer {

	bind_transformer(binder_t& binder) : binder(binder) {}

	sp_tau_node<BAs...> operator()(const sp_tau_node<BAs...>& n) {
		if (auto it = changes.find(n); it != changes.end())
			return it->second;
		if (is_non_terminal<tau_parser::binding, BAs...>(n))
			return changes.emplace(n, binder.bind(n)).first->second;
		// IDEA maybe we could use the replace transform instead of having the following code
		bool changed = false;
		std::vector<sp_tau_node<BAs...>> child;
		for (auto& c : n->child)
			if (changes.contains(c))
				changed = true, child.push_back(changes[c]);
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
		|| (get<0>(n->value).n() != tau_parser::_ &&
			get<0>(n->value).n() != tau_parser::__);
};

template<typename... BAs>
using not_whitespace_predicate_t = decltype(not_whitespace_predicate<BAs...>);

// binds the constants of a given binding using the label specified
// in the code and according to a map from labels to constants in the BAs...
template<typename... BAs>
struct name_binder {

	name_binder(const bindings<BAs...>& bs) : bs(bs) {}

	sp_tau_node<BAs...> bind(const sp_tau_node<BAs...>& n) const {
		// FIXME (LOW) check if the node is a named binding one
		auto bn = make_string<
				tau_node_terminal_extractor_t<BAs...>,
				sp_tau_node<BAs...>>(
			tau_node_terminal_extractor<BAs...>, n);
		auto s = bs.find(bn);
		if (s != bs.end()) {
			tau_sym<BAs...> ts = s->second;
			return make_node<tau_sym<BAs...>>(ts, {});
		}
		return n;
	}

	const bindings<BAs...>& bs;
};

template<typename...BAs>
struct nso_factory {

	sp_tau_node<BAs...> parse(const std::string&,
			const std::string&) const {
		throw std::runtime_error("not implemented");
	}

	sp_tau_node<BAs...> binding(const sp_tau_node<BAs...>&,
			const std::string&) const {
		throw std::runtime_error("not implemented");
	}
};

// binds the constants of a given binding using the multi-factory for the types
// supported.
template<typename... BAs>
struct factory_binder {

	sp_tau_node<BAs...> bind(const sp_tau_node<BAs...>& n) const {
		static nso_factory<BAs...> factory;
		if(auto type = find_top(n,
			is_non_terminal<tau_parser::type, BAs...>); type)
		{
			// the factory take two arguments, the first is the type and the
			// second is the node representing the constant.
			std::string type_name = make_string<
					tau_node_terminal_extractor_t<BAs...>,
					sp_tau_node<BAs...>>(
				tau_node_terminal_extractor<BAs...>, type.value());
			return factory.binding(n, type_name);
		}
		return n;
	}
};

// TODO (HIGH) improve type resolution adding types to quantifiers.
//
// Type resolution is used to ensure that all the Boolean Algebra operations are
// consistente. Types could be specified at two levels:
//
// - At the boolean algebra expression level: in this case, the type of the
//   expression is the type of one of the constants of the boolean expression,
//   there is no need to provide the type in each of them.
//
// - At the quantifier expression level: in this case, the type of a variable is
//   specified and all the underlying boolean algebra expressions are typed
//   accordingly.
//
// In both cases, if there is a missmatch between the types, an error is raised
// during the type resolution phase, i.e. before executing the program.


// check if the type is unresolved.
template<typename... BAs>
static const auto is_unresolved_predicate = [](const sp_tau_node<BAs...>& n) {
	if (!is_non_terminal<tau_parser::type, BAs...>(n)) return false;
	auto type_name = make_string<
			tau_node_terminal_extractor_t<BAs...>,
			sp_tau_node<BAs...>>(
			tau_node_terminal_extractor<BAs...>, n);
	return type_name.empty();
};

template<typename... BAs>
using is_unresolved_predicate_t = decltype(is_unresolved_predicate<BAs...>);

// check if the type is resolved.
template<typename... BAs>
static const auto is_resolved_predicate = [](const sp_tau_node<BAs...>& n) {
	if (!is_non_terminal<tau_parser::type, BAs...>(n)) return false;
	auto type_name = make_string<
			tau_node_terminal_extractor_t<BAs...>,
			sp_tau_node<BAs...>>(
			tau_node_terminal_extractor<BAs...>, n);
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
	if (auto unresolved = is_unresolved(n); unresolved) {
		// always we have type information or it is not needed at all
		if (auto type = find_bottom(n,
			is_resolved_predicate<BAs...>); type)
		{
			std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>>
				change {{unresolved.value(), type.value()}};
			return replace<sp_tau_node<BAs...>>(n, change);
		}
	}
	return n;
}

// resolve all the unresolved types in the given statement.
template<typename binder_t, typename... BAs>
sp_tau_node<BAs...> resolve_types(const sp_tau_node<BAs...> source) {
	std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes;
	for (const auto& bf: select_top(source,
		is_non_terminal<BAs...>(tau_parser::bf)))
	{
		if (auto rbf = resolve_type(bf); rbf != bf) changes[bf] = rbf;
	}
	return replace<sp_tau_node<BAs...>>(source, changes);
}

// resolve all the unresolved types in the given rules.
template<typename... BAs>
rules<nso<BAs...>> resolve_types(const rules<nso<BAs...>> rs) {
	rules<nso<BAs...>> nrs;
	for (const auto& r : rs) {
		auto nr = resolve_types(r);
		nrs.push_back(nr);
	}
	return nrs;
}

// resolve all the unresolved types in the given library.
template<typename binder_t, typename... BAs>
library<nso<BAs...>> resolve_types(const library<nso<BAs...>> lib) {
	return { resolve_types(lib.system) };
}

// resolve all the unresolved types in the given nso_rr.
template<typename binder_t, typename... BAs>
rr<nso<BAs...>> resolve_types(const rr<nso<BAs...>> f) {
	return { resolve_types(f.rec_relations), resolve_type(f.main) };
}

// creates a specific rule from a generic rule
// TODO (LOW) should depend in node_t instead of BAs...
template<typename... BAs>
rule<nso<BAs...>> make_rule(tau_parser::nonterminal rule_t,
	tau_parser::nonterminal matcher_t, tau_parser::nonterminal body_t,
	const sp_tau_node<BAs...>& rule)
{
	auto matcher = rule | rule_t | matcher_t | only_child_extractor<BAs...>
		| optional_value_extractor<sp_tau_node<BAs...>>;
	auto body = rule | rule_t | body_t | only_child_extractor<BAs...>
		| optional_value_extractor<sp_tau_node<BAs...>>;
	return { matcher, body };
}

// creates a specific rule from a generic rule
// TODO (LOW) should depend in node_t instead of BAs...
template<typename... BAs>
rule<nso<BAs...>> make_rule(const sp_tau_node<BAs...>& rule) {
	auto type = only_child_extractor<BAs...>(rule)
		| non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
	switch (type) {
	case tau_parser::bf_rule: return make_rule<BAs...>(tau_parser::bf_rule,
		tau_parser::bf_matcher, tau_parser::bf_body, rule); break;
	case tau_parser::wff_rule:return make_rule<BAs...>(tau_parser::wff_rule,
		tau_parser::wff_matcher, tau_parser::wff_body, rule); break;
	default: assert(false); return {};
	};
}

// create a set of rules from a given tau source.
// TODO (LOW) should depend in node_t instead of BAs...
template<typename... BAs>
rules<nso<BAs...>> make_rules(sp_tau_node<BAs...>& tau_source) {
	rules<nso<BAs...>> rs;
	// TODO (LOW) change call to select by operator|| and operator|
	for (auto& r: select_top(tau_source,
		is_non_terminal<tau_parser::rule, BAs...>))
			rs.push_back(make_rule<BAs...>(r));
	return rs;
}

// create a set of relations from a given tau source.
// TODO (LOW) should depend in node_t instead of BAs...
template<typename... BAs>
rec_relations<nso<BAs...>> make_rec_relations(
	const sp_tau_node<BAs...>& tau_source)
{
	rec_relations<nso<BAs...>> rs;
	// TODO (LOW) change call to select by operator|| and operator|
	for (auto& r: select_top(tau_source,
		is_non_terminal<tau_parser::rec_relation, BAs...>))
			rs.emplace_back(r->child[0], r->child[1]);
	return rs;
}

// make a tau source from the given source code string.
sp_tau_source_node make_tau_source(const std::string& source,
	idni::parser<>::parse_options options = {});

// make a tau source from the given source code stream.
sp_tau_source_node make_tau_source(std::istream& is,
	idni::parser<>::parse_options options = {});

// make a tau source from the given source code stream.
sp_tau_source_node make_tau_source_from_file(const std::string& filename,
	idni::parser<>::parse_options options = {});

template<typename...BAs>
sp_tau_node<BAs...> process_digits(const sp_tau_node<BAs...>& tau_source) {
	std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes;
	for(auto& n: select_top(tau_source,
		is_non_terminal<tau_parser::digits, BAs...>))
	{
		auto offset = make_string<
				tau_node_terminal_extractor_t<BAs...>,
				sp_tau_node<BAs...>>(
			tau_node_terminal_extractor<BAs...>,  n);
		auto num = std::stoul(offset);
		auto nn = make_node<tau_sym<BAs...>>(tau_sym<BAs...>(num), {});
		changes[n] = nn;
	}
	return replace<sp_tau_node<BAs...>>(tau_source, changes);
}

// } // include for ptree<BAs...>()
// #include "debug_helpers.h"
// namespace idni::tau {

template<typename... BAs>
struct quantifier_vars_transformer {
	using p = tau_parser;
	using node = sp_tau_node<BAs...>;
	static constexpr std::array<p::nonterminal, 2>
					quant_nts = { p::wff_ex, p::wff_all };
	node operator()(const node& n) {
		if (auto it = changes.find(n); it != changes.end())
			return it->second;
		// if the node is any of quant_nts (all, ex, ball, bex)
		for (size_t q = 0; q != 2; ++q)
			if (is_non_terminal<BAs...>(quant_nts[q], n))
		{
			// ptree<BAs...>(std::cout << "QUANT: ", n) << "\n";
			// use nonterminals according to quantifier's q
			const p::nonterminal& quant_nt = quant_nts[q];
			// travers into q_vars node and access all var/capture children
			auto q_vars = n | p::q_vars
				| optional_value_extractor<node>;
			std::vector<node>& vars = q_vars->child;
			//  traverse to quantifier wff expression
			node nn = n | p::wff | only_child_extractor<BAs...>
				| optional_value_extractor<node>;
			if (auto it = changes.find(nn); it != changes.end())
				nn = it->second;
			if (vars.size() == 0) continue; // no variable/capture
			// for each variable
			for (size_t vi = 0; vi != vars.size(); ++vi) {
				// get var (in a reverse order of iteration)
				auto& var = vars[vars.size() - 1 - vi];
				// new expression node
				auto expr = wrap(p::wff, nn);
				// create a new quantifier node with var and new children
				nn = wrap(quant_nt, { var, expr });
			}
			changes[n] = nn;
			return nn;
		}

		bool changed = false;
		std::vector<sp_tau_node<BAs...>> child;
		for (node& c : n->child)
			if (changes.contains(c)) changed = true,
						child.push_back(changes[c]);
			else child.push_back(c);
		node nn = make_node<tau_sym<BAs...>>(n->value, child);
		if (changed) changes[n] = nn;
		return nn;
	}
	std::map<node, node> changes;
};

template<typename...BAs>
sp_tau_node<BAs...> process_quantifier_vars(const sp_tau_node<BAs...>& tau_code)
{
	using node = sp_tau_node<BAs...>;
	quantifier_vars_transformer<BAs...> transformer;
	return post_order_traverser<quantifier_vars_transformer<BAs...>,
		all_t, node>(transformer, all)(tau_code);
}

template<typename...BAs>
sp_tau_node<BAs...> process_offset_variables(
	const sp_tau_node<BAs...>& tau_code)
{
	using p = tau_parser;
	using node = sp_tau_node<BAs...>;
	std::map<node, node> changes;
	for (const auto& offset :
		select_all(tau_code, is_non_terminal<p::offset, BAs...>))
	{
		for (const auto& var : select_all(offset,
			is_non_terminal<p::variable, BAs...>))
				changes[var] = wrap(p::capture, var->child);
	}
	if (changes.size()) return replace(tau_code, changes);
	return tau_code;
}

template<typename...BAs>
sp_tau_node<BAs...> process_defs_input_variables(
	const sp_tau_node<BAs...>& tau_code)
{
	using p = tau_parser;
	using node = sp_tau_node<BAs...>;
	std::map<node, node> changes;
	for (const auto& def :
		select_all(tau_code, is_non_terminal<p::rec_relation, BAs...>))
	{
		for (const auto& ref_arg : select_all(def->child[0],
			is_non_terminal<p::ref_arg, BAs...>))
				for (const auto& var : select_all(ref_arg,
					is_non_terminal<p::variable, BAs...>))
		{
			changes[var] = wrap(p::capture, var->child);
		}
	}
	if (changes.size()) return replace(tau_code, changes);
	return tau_code;
}

// create tau code from tau source
template<typename... BAs>
// TODO (LOW) should depend on node_t instead of BAs...
sp_tau_node<BAs...> make_tau_code(sp_tau_source_node& tau_source) {
	tauify<BAs...> tf;
	map_transformer<tauify<BAs...>, sp_tau_source_node, sp_tau_node<BAs...>>
								transform(tf);
	auto tau_code = post_order_traverser<
				map_transformer<tauify<BAs...>,
				sp_tau_source_node, sp_tau_node<BAs...>>,
			all_t,
			sp_node<tau_source_sym>,
			sp_tau_node<BAs...>>(
		transform, all)(tau_source);
	return process_defs_input_variables(
		process_offset_variables(
		process_quantifier_vars(
		process_digits(tau_code))));
}

// make a library from the given tau source.
// TODO (LOW) should depend on node_t instead of BAs...
template<typename... BAs>
library<nso<BAs...>> make_library(sp_tau_source_node& tau_source) {
	auto lib = make_tau_code<BAs...>(tau_source);
	return make_rules(lib);
}

// make a library from the given tau source string.
// TODO (LOW) should depend on node_t instead of BAs...
template<typename... BAs>
library<nso<BAs...>> make_library(const std::string& source) {
	auto tau_source = make_tau_source(source, {
						.start = tau_parser::library });
	return make_library<BAs...>(tau_source);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
sp_tau_node<BAs...> bind_tau_code_using_binder(const sp_tau_node<BAs...>& code,
	binder_t& binder)
{
	bind_transformer<binder_t, BAs...> bs(binder);
	return post_order_traverser<
			bind_transformer<binder_t, BAs...>,
			all_t,
			sp_tau_node<BAs...>>(bs, all)(code);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
sp_tau_node<BAs...> bind_tau_code_using_bindings(sp_tau_node<BAs...>& code,
	const bindings<BAs...>& bindings)
{
	name_binder<BAs...> nb(bindings);
	return bind_tau_code_using_binder<
			name_binder<BAs...>, BAs...>(code, nb);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
sp_tau_node<BAs...> bind_tau_code_using_factory(const sp_tau_node<BAs...>& code)
{
	factory_binder<BAs...> fb;
	return bind_tau_code_using_binder<factory_binder<BAs...>, BAs...>(code, fb);
}

// make a nso_rr from the given tau code
template<typename... BAs>
rr<nso<BAs...>> make_nso_rr_from_binded_code(sp_tau_node<BAs...>& code) {
	if (is_non_terminal(tau_parser::bf, code)
		|| is_non_terminal(tau_parser::ref, code))
			return { {}, code };

	if (is_non_terminal(tau_parser::rec_relations, code))
		return {make_rec_relations<BAs...>(code), {}};

	auto main = (!is_non_terminal(tau_parser::rr, code)
		? code | tau_parser::rr | tau_parser::main
		: code | tau_parser::main)
			| tau_parser::wff
			| optional_value_extractor<sp_tau_node<BAs...>>;
	auto rules = make_rec_relations<BAs...>(code);
	return infer_ref_types<BAs...>(rr<nso<BAs...>>{ rules, main });
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
rr<nso<BAs...>> make_nso_rr_using_binder(const sp_tau_node<BAs...>& code,
	binder_t& binder)
{
	auto binded = bind_tau_code_using_binder<binder_t, BAs...>(code,
									binder);
	return make_nso_rr_from_binded_code<BAs...>(binded);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
rr<nso<BAs...>> make_nso_rr_using_binder(sp_tau_source_node& source,
	binder_t& binder)
{
	auto code = make_tau_code<BAs...>(source);
	return make_nso_rr_using_binder<binder_t, BAs...>(code, binder);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
rr<nso<BAs...>> make_nso_rr_using_binder(std::string& input, binder_t& binder) {
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	return make_nso_rr_using_binder<binder_t, BAs...>(source, binder);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
rr<nso<BAs...>> make_nso_rr_using_bindings(const sp_tau_node<BAs...>& code,
	const bindings<BAs...>& bindings)
{
	name_binder<BAs...> nb(bindings);
	return make_nso_rr_using_binder<name_binder<BAs...>, BAs...>(code, nb);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
rr<nso<BAs...>> make_nso_rr_using_bindings(sp_tau_source_node& source,
	const bindings<BAs...>& bindings)
{
	auto code = make_tau_code<BAs...>(source);
	return make_nso_rr_using_bindings<BAs...>(code, bindings);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
rr<nso<BAs...>> make_nso_rr_using_bindings(const std::string& input,
	const bindings<BAs...>& bindings)
{
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	return make_nso_rr_using_bindings<BAs...>(source, bindings);
}

template<typename... BAs>
rr<nso<BAs...>> make_nso_rr_using_factory(const sp_tau_node<BAs...>& code)
{
	factory_binder<BAs...> fb;
	return make_nso_rr_using_binder<
			factory_binder<BAs...>, BAs...>(code,fb);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
rr<nso<BAs...>> make_nso_rr_using_factory(sp_tau_source_node& source)
{
	auto code = make_tau_code<BAs...>(source);
	return make_nso_rr_using_factory<BAs...>(code);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
rr<nso<BAs...>> make_nso_rr_using_factory(const std::string& input)
{
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	return make_nso_rr_using_factory<BAs...>(source);
}

//------------------------------------------------------------------------------
// rec relations type inference

struct rr_type {
	tau_parser::nonterminal type;
	size_t offset_arity = 0;
	size_t args_arity = 0;
	bool fp = false;
};

struct rr_types {
	std::unordered_map<std::string, rr_type> types;
	std::set<std::string> errors;
};

static auto type2str = [](const tau_parser::nonterminal& t) {
	return tau_parser::instance().name(t);
};

inline std::ostream& print_rr_type(std::ostream& os, const rr_type& t) {
	os << type2str(t.type) << "/[";
	if (t.fp) os << "*"; else os << t.offset_arity;
	return os << "]" << t.args_arity;
}

inline std::string rr_type2str(const rr_type& t) {
	std::stringstream ss;
	print_rr_type(ss, t);
	return ss.str();
}

template<typename... BAs>
std::string get_ref_name(const sp_tau_node<BAs...>& n) {
	auto ref = n;
	if (auto ref_as_child = n | tau_parser::ref; ref_as_child)
		ref = ref_as_child.value();
	return make_string(tau_node_terminal_extractor<BAs...>,
		(ref | tau_parser::sym).value());
};

template<typename... BAs>
std::pair<std::string, std::pair<size_t, size_t>> get_ref_name_and_arity(
	const sp_tau_node<BAs...>& n)
{
	auto ref = n;
	if (auto ref_as_child = n | tau_parser::ref; ref_as_child)
		ref = ref_as_child.value();
	auto ofs = ref | tau_parser::offsets || tau_parser::offset;
	auto ras = ref | tau_parser::ref_args || tau_parser::ref_arg;
	return { make_string(tau_node_terminal_extractor<BAs...>,
					(ref | tau_parser::sym).value()),
		{ ofs.size(), ras.size() } };
};

template<typename... BAs>
std::string get_ref_type(bool& success, rr_types& ts,
	const sp_tau_node<BAs...>& ref, const tau_parser::nonterminal& t,
	bool possible_fp = false)
{
	auto [fn, arity] = get_ref_name_and_arity(ref);
	rr_type new_type(t, arity.first, arity.second,
					possible_fp && arity.first == 0);
	auto it = ts.types.find(fn);
	if (it != ts.types.end()) {
		auto& rt = it->second;
		std::stringstream err;
		if (rt.type != t) err << "Type mismatch. ";
		if (rt.args_arity != arity.second)
			err << "Argument arity mismatch. ";
		if (rt.fp && arity.first)
			// found a real offset arity for a uncomplete type taken
			// from main with a fp call => update offset arity
			ts.types[fn].offset_arity = arity.first,
			ts.types[fn].fp = false;
		if (!rt.fp && !new_type.fp && rt.offset_arity != arity.first)
			err << "Offset arity mismatch. ";
		if (err.tellp()) return
			err << fn << "() : " << rr_type2str(new_type)
				<< " declared as " << rr_type2str(rt),
			ts.errors.insert(err.str()),
			success = false, "";
	} else {
		ts.types[fn] = new_type;
		BOOST_LOG_TRIVIAL(debug) << "(I) -- Found type of " << fn
					<< "() : " << rr_type2str(ts.types[fn]);
		return fn;
	}
	return "";
};

template<typename... BAs>
std::pair<std::set<std::string>, std::set<std::string>> get_rr_types(
	bool& success, rr_types& ts, const sp_tau_node<BAs...>& n,
	bool possible_fp = false)
{
	std::set<std::string> done_names;
	std::set<std::string> todo_names;
	for (const auto& ref : select_all(n,
			is_non_terminal<tau_parser::ref, BAs...>))
		todo_names.insert(get_ref_name(ref));
	for (const auto& ref : select_all(n,
			is_non_terminal<tau_parser::wff_ref, BAs...>))
	{
		auto fn = get_ref_type(success, ts, ref, tau_parser::wff,
			possible_fp);
		done_names.insert(fn), todo_names.erase(fn);
	}
	for (const auto& ref : select_all(n,
			is_non_terminal<tau_parser::bf_ref, BAs...>))
	{
		auto fn = get_ref_type(success, ts, ref, tau_parser::bf,
			possible_fp);
		done_names.insert(fn), todo_names.erase(fn);
	}
	return { done_names, todo_names };
}

template<typename... BAs>
std::pair<std::set<std::string>, std::set<std::string>> get_rr_types(
	bool& success, rr_types& ts, const rr<nso<BAs...>>& nso_rr)
{
	std::set<std::string> done_names;
	std::set<std::string> todo_names;
	auto add_ref_names = [&done_names, &todo_names](const std::pair<
		std::set<std::string>, std::set<std::string>>& names)
	{
		for (const auto& fn : names.first)
			done_names.insert(fn), todo_names.erase(fn);
		for (const auto& fn : names.second)
	 		if (done_names.find(fn) == done_names.end())
				todo_names.insert(fn);
	};
	// get types from relations if any
	for (const auto& r : nso_rr.rec_relations)
		add_ref_names(get_rr_types(success, ts, r.first)),
		add_ref_names(get_rr_types(success, ts, r.second));
	// get type from main if any
	if (nso_rr.main)
		add_ref_names(get_rr_types(success, ts, nso_rr.main, true));
	return { done_names, todo_names };
}

template<typename... BAs>
rr<nso<BAs...>> infer_ref_types(const rr<nso<BAs...>>& nso_rr) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin type inferrence"; // << ": " << nso_rr;
	// for (auto& r : nso_rr.rec_relations)
	// 	ptree<BAs...>(std::cout << "rule left: ", r.first) << "\n",
	// 	ptree<BAs...>(std::cout << "rule right: ", r.second) << "\n";
	// ptree<BAs...>(std::cout << "main: ", nso_rr.main) << "\n";
	rr<nso<BAs...>> nn = nso_rr;
	rr_types ts;
	std::set<std::string> done_names, todo_names;
	bool success = true;
	static auto get_nt_type = [](const sp_tau_node<BAs...>& r) {
		size_t n = r | non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>;
		return static_cast<tau_parser::nonterminal>(n);
	};
	static auto update_ref = [](sp_tau_node<BAs...>& r,
		const tau_parser::nonterminal& t)
	{
		//ptree<BAs...>(std::cout << "updating ref: ", r) << "\n";
		r = wrap(t, wrap(t == tau_parser::wff
			? tau_parser::wff_ref : tau_parser::bf_ref, r));
		//ptree<BAs...>(std::cout << "updated ref: ", r) << "\n";
	};
	auto add_ref_names = [&done_names, &todo_names](const std::pair<
		std::set<std::string>, std::set<std::string>>& names)
	{
		for (const auto& fn : names.first)
			done_names.insert(fn), todo_names.erase(fn);
		for (const auto& fn : names.second)
	 		if (done_names.find(fn) == done_names.end())
				todo_names.insert(fn);
	};
	auto done = [&done_names, &todo_names](const std::string& fn) {
		todo_names.erase(fn), done_names.insert(fn);
	};
	// get types from relations if any
	for (const auto& r : nn.rec_relations)
		add_ref_names(get_rr_types(success, ts, r.first)),
		add_ref_names(get_rr_types(success, ts, r.second));
	// get type from main if any
	if (nn.main) add_ref_names(get_rr_types(success, ts, nn.main, true));
	// inference loop
	for (;;) {
		bool changed = false;
		for (auto& r : nn.rec_relations) {
			// check type of the right side
			auto t = get_nt_type(r.second);
			BOOST_LOG_TRIVIAL(trace) << "(T) " << r.second << " is " << (type2str(t));
			if (t == tau_parser::ref) {
				// right side is unresolved ref
				auto fn = get_ref_name(r.second);
				auto it = ts.types.find(fn); // if we know type
				if (it != ts.types.end()) {  // update
					BOOST_LOG_TRIVIAL(trace) << "(T) updating right side: " << r.second;
					t = it->second.type;
					update_ref(r.second, t);
					changed = true;
				}
			}
			// update left side if right side is known
			if (t == tau_parser::bf || t == tau_parser::wff) {
				if (get_nt_type(r.first) == tau_parser::ref) {
					// left side is unresolved ref
					BOOST_LOG_TRIVIAL(trace) << "(T) updating left side: " << r.first;
					done(get_ref_type(success, ts, r.first, t));
					update_ref(r.first, t);
					changed = true;
				}
			}
			// infer capture's type from the left side if known
			if (t == tau_parser::capture) {
				auto lt = get_nt_type(r.first);
				// left side is an unresolved ref
				if (lt == tau_parser::ref) {
					auto fn = get_ref_name(r.first);
					auto it = ts.types.find(fn); // if we know
					if (it != ts.types.end()) { // type -> assign
						lt = it->second.type;
						BOOST_LOG_TRIVIAL(trace) << "(T) updating known type of the left side: " << r.first;
						done(get_ref_type(success, ts,
							r.first, lt));
						update_ref(r.first, lt);
						changed = true;
					}
				}
				// left side is bf or wff, update capture
				if (lt == tau_parser::bf
					|| lt == tau_parser::wff)
				{
					BOOST_LOG_TRIVIAL(trace) << "(T) updating capture: " << r.second;
					//ptree<BAs...>(std::cout << "updating ref: ", r) << "\n";
					r.second = wrap(lt, r.second);
					//ptree<BAs...>(std::cout << "updated ref: ", r) << "\n";
				}
			}
		}
		if (!changed) break; // fixed point
	}

	// infer main if unresolved ref
	if (nn.main) {
		//add_ref_names(get_rr_types(success, types, nn.main, true));
		auto t = get_nt_type(nn.main);
		BOOST_LOG_TRIVIAL(trace) << "(T) main " << nn.main << " is " << (type2str(t));
		if (t == tau_parser::ref) {
			// main is an unresolved ref
			auto fn = get_ref_name(nn.main);
			auto it = ts.types.find(fn); // if we know type
			if (it != ts.types.end()) {  // update
				BOOST_LOG_TRIVIAL(trace) << "(T) updating main: " << nn.main;
				t = it->second.type;
				update_ref(nn.main, t);
			}
		}
	}

	for (const auto& err : ts.errors) BOOST_LOG_TRIVIAL(error) << err;

	if (todo_names.size()) {
		std::stringstream ss;
		for (auto& fn : todo_names) ss << " " << fn;
		BOOST_LOG_TRIVIAL(error) << "Unknown recurrence relation ref "
							"type for:" << ss.str();
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End type inferrence"; // << ": " << nn;
	return nn;
}


//------------------------------------------------------------------------------

// creates a specific builder from a sp_tau_node.
template<typename... BAs>
builder<BAs...> make_builder(const sp_tau_node<BAs...>& builder) {
	auto head = builder | tau_parser::builder_head
		| optional_value_extractor<sp_tau_node<BAs...>>;
	auto type_node = builder
		| tau_parser::builder_body | only_child_extractor<BAs...>;
	auto type = type_node | non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
	switch (type) {
	case tau_parser::bf_builder_body:  return { head, type_node
		| tau_parser::bf
		| optional_value_extractor<sp_tau_node<BAs...>>};
	case tau_parser::wff_builder_body: return { head, type_node
		| tau_parser::wff
		| optional_value_extractor<sp_tau_node<BAs...>>};
	default: throw std::runtime_error("unknown builder type");
	};
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
	auto tau_source = make_tau_source(source, {
						.start = tau_parser::builder });
	return make_builder<BAs...>(tau_source);
}

template<typename... BAs>
sp_tau_node<BAs...> tau_apply_builder(const builder<BAs...>& b,
	std::vector<sp_tau_node<BAs...>>& n)
{
	std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes;
	std::vector<sp_tau_node<BAs...>> vars = b.first || tau_parser::capture;
	for (size_t i = 0; i < vars.size(); ++i) changes[vars[i]] = n[i];
	return replace<sp_tau_node<BAs...>>(b.second, changes);
}

// definitions of basic bf and wff
const std::string BLDR_BF_0 = "( $X ) =: 0.";
const std::string BLDR_BF_1 = "( $X ) =: 1.";
const std::string BLDR_WFF_F = "( $X ) =:: F.";
const std::string BLDR_WFF_T = "( $X ) =:: T.";

// definitions of wff builder rules
const std::string BLDR_WFF_EQ = "( $X ) =:: $X = 0.";
const std::string BLDR_WFF_NEQ = "( $X ) =:: $X != 0.";
const std::string BLDR_BF_NOT_LESS_EQUAL = "( $X $Y ) =:: $X !<= $Y.";
const std::string BDLR_BF_INTERVAL = "( $X $Y $Z ) =:: $X <= $Y <= $Z.";
const std::string BDLR_BF_NLEQ_UPPER = "( $X $Y ) =:: $X !<= $Y.";
const std::string BDLR_BF_NLEQ_LOWWER = "( $X $Y ) =:: $Y !<= $X.";
const std::string BLDR_WFF_ALL = "( $X $Y ) =:: all $X $Y.";
const std::string BLDR_WFF_EX = "( $X $Y ) =:: ex $X $Y.";
const std::string BLDR_WFF_SOMETIMES = "( $X ) =:: sometimes $X.";
const std::string BLDR_WFF_ALWAYS = "( $X ) =:: always $X.";

// definitions of bf builder rules
const std::string BLDR_BF_SPLITTER = "( $X ) =: S($X).";


// basic bf and wff builders
template<typename... BAs>
static auto bldr_bf_0 = make_builder<BAs...>(BLDR_BF_0);
template<typename... BAs>
static auto bldr_bf_1 = make_builder<BAs...>(BLDR_BF_1);
template<typename... BAs>
static auto bldr_wff_F = make_builder<BAs...>(BLDR_WFF_F);
template<typename... BAs>
static auto bldr_wff_T = make_builder<BAs...>(BLDR_WFF_T);

// wff builder
template<typename... BAs>
static auto bldr_wff_eq = make_builder<BAs...>(BLDR_WFF_EQ);
template<typename... BAs>
static auto bldr_bf_splitter = make_builder<BAs...>(BLDR_BF_SPLITTER);
template<typename... BAs>
static auto bldr_bf_not_less_equal =
				make_builder<BAs...>(BLDR_BF_NOT_LESS_EQUAL);
template<typename... BAs>
static auto bldr_bf_interval = make_builder<BAs...>(BDLR_BF_INTERVAL);
template<typename... BAs>
static auto bldr_bf_nleq_upper = make_builder<BAs...>(BDLR_BF_NLEQ_UPPER);
template<typename... BAs>
static auto bldr_bf_nleq_lowwer = make_builder<BAs...>(BDLR_BF_NLEQ_LOWWER);

// basic bf and wff constants
template<typename... BAs>
static const sp_tau_node<BAs...> _0 = bldr_bf_0<BAs...>.second;

template<typename... BAs>
static const sp_tau_node<BAs...> _0_trimmed = trim(_0<BAs...>);

template<typename... BAs>
static const sp_tau_node<BAs...> _1 = bldr_bf_1<BAs...>.second;

template<typename... BAs>
static const sp_tau_node<BAs...> _1_trimmed = trim(_1<BAs...>);

template<typename... BAs>
static const sp_tau_node<BAs...> _F = bldr_wff_F<BAs...>.second;

template<typename... BAs>
static const sp_tau_node<BAs...> _F_trimmed = trim(_F<BAs...>);

template<typename... BAs>
static const sp_tau_node<BAs...> _T = bldr_wff_T<BAs...>.second;

template<typename... BAs>
static const sp_tau_node<BAs...> _T_trimmed = trim(_T<BAs...>);

template<typename... BAs>
nso<BAs...> build_extra (const nso<BAs...> n, const std::string &note) {
	assert((!n->child.empty()) && (!is_non_terminal(tau_parser::extra, n->child.back())));
	std::vector<nso<BAs...>> c (n->child);
	c.emplace_back(wrap<BAs...>(tau_parser::extra, note));
	return make_node(n->value, move(c));
}

template<typename... BAs>
nso<BAs...> build_num(size_t value) {
	return wrap(tau_parser::num,
		make_node<tau_sym<BAs...>>(tau_sym<BAs...>(value), {}));
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_constant(const std::variant<BAs...>& v) {
	auto cte = make_node<tau_sym<BAs...>>(tau_sym<BAs...>(v), {});
	return wrap(tau_parser::bf,
		wrap(tau_parser::bf_constant,
		wrap(tau_parser::constant, cte)));
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_and_constant(
	const std::set<std::variant<BAs...>>& ctes)
{
	if (ctes.empty()) return _1<BAs...>;

	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
		[&](const auto& l, const auto& r) { return l & r; });

	return build_bf_constant<BAs...>(cte);
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_or_constant(
	const std::set<std::variant<BAs...>>& ctes)
{
	if (ctes.empty()) return _0<BAs...>;

	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
		[&](const auto& l, const auto& r) { return l | r; });

	return build_bf_constant<BAs...>(cte);
}

template<typename... BAs>
std::optional<sp_tau_node<BAs...>> build_bf_constant(
	const std::optional<std::variant<BAs...>>& o)
{
	return o.has_value() ? build_bf_constant(o.value())
				: std::optional<sp_tau_node<BAs...>>();
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_var(const std::string& name) {
	auto var = make_builder<BAs...>("( $X ) =: " + name + ".").second;
	return trim<BAs...>(var);
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_var(const std::string& name) {
	auto var = make_builder<BAs...>("( $X ) =:: ?" + name + ".").second;
	return trim<BAs...>(var);
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_uniter_const(const std::string& name) {
	auto var = make_builder<BAs...>("( $X ) =:: <:" + name + ">.").second;
	return trim<BAs...>(var);
}

// wff factory method for building wff formulas
template<typename... BAs>
sp_tau_node<BAs...> build_wff_eq(const sp_tau_node<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::bf_eq, l, _0<BAs...>));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_neq(const sp_tau_node<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::bf_neq, l, _0<BAs...>));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_and(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_and, l, r));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_and(const std::set<sp_tau_node<BAs...>>& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _T<BAs...>,
		[](const auto& l, const auto& r) {return build_wff_and(l, r);});
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_or(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_or, l, r));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_or(const std::set<sp_tau_node<BAs...>>& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _F<BAs...>,
		[](const auto& l, const auto& r) { return build_wff_or(l, r);});
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_neg(const sp_tau_node<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::wff_neg, l));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_xor_from_def(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return build_wff_or<BAs...>(build_wff_and(build_wff_neg(l), r),
		build_wff_and(build_wff_neg(r), l));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_xor(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return build_wff_or(
		build_wff_and(build_wff_neg(l), r),
		build_wff_and(build_wff_neg(r), l));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_imply(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return build_wff_or<BAs...>(build_wff_neg<BAs...>(l), r);
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_conditional(const sp_tau_node<BAs...>& x,
	const sp_tau_node<BAs...>& y,
	const sp_tau_node<BAs...>& z)
{
	return build_wff_and<BAs...>(build_wff_imply<BAs...>(x, y),
		build_wff_imply<BAs...>(build_wff_neg<BAs...>(x), z));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_equiv(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return build_wff_and<BAs...>(build_wff_imply<BAs...>(l, r),
		build_wff_imply<BAs...>(r, l));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_all(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_all, l, r));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_ex(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_ex, l, r));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_sometimes(const sp_tau_node<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::wff_sometimes, l));
}

template<typename... BAs>
sp_tau_node<BAs...> build_wff_always(const sp_tau_node<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::wff_always, l));
}

// bf factory method for building bf formulas
template<typename... BAs>
sp_tau_node<BAs...> build_bf_and(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return wrap(tau_parser::bf, wrap(tau_parser::bf_and, l, r));
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_and(const std::set<sp_tau_node<BAs...>>& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _1<BAs...>,
		[](const auto& l, const auto& r) { return build_bf_and(l, r);});
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_or(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return wrap(tau_parser::bf, wrap(tau_parser::bf_or, l, r));
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_or(const std::set<sp_tau_node<BAs...>>& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _0<BAs...>,
		[](const auto& l, const auto& r) { return build_bf_or(l, r); });
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_neg(const sp_tau_node<BAs...>& l) {
	return wrap(tau_parser::bf, wrap(tau_parser::bf_neg, l));
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_xor_from_def(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return build_bf_or<BAs...>(
		build_bf_and(build_bf_neg(l), r),
		build_bf_and(build_bf_neg(r), l));
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_xor(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return build_bf_or<BAs...>(
		build_bf_and<BAs...>(build_bf_neg<BAs...>(l), r),
		build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r)));
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_less(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return build_wff_and<BAs...>(build_wff_eq<BAs...>(
			build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r))),
			build_wff_neq(build_bf_xor<BAs...>(l,
					build_bf_neg<BAs...>(r))));
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_less_equal(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return build_wff_eq<BAs...>(build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r)));
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_nleq(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return build_wff_neq<BAs...>(build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r)));
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_interval(const sp_tau_node<BAs...>& x,
	const sp_tau_node<BAs...>& y, const sp_tau_node<BAs...>& z)
{
	return build_wff_and<BAs...>(build_bf_less_equal<BAs...>(x, y),
		build_bf_less_equal<BAs...>(y, z));
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_nleq_lower(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	std::vector<sp_tau_node<BAs...>> args {trim(l), trim(r)};
	return tau_apply_builder<BAs...>(bldr_bf_nleq_lowwer<BAs...>, args);
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_nleq_upper(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	std::vector<sp_tau_node<BAs...>> args {trim(l), trim(r)};
	return tau_apply_builder<BAs...>(bldr_bf_nleq_upper<BAs...>, args);
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_not_less_equal(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	std::vector<sp_tau_node<BAs...>> args {trim(l), trim(r)};
	return tau_apply_builder<BAs...>(bldr_bf_not_less_equal<BAs...>, args);
}

template<typename... BAs>
sp_tau_node<BAs...> build_bf_greater(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return build_wff_neg(build_bf_less_equal<BAs...>(l, r));
}

template<typename... BAs>
sp_tau_node<BAs...> operator&(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	auto bf_constant_and = [](const auto& l, const auto& r) -> nso<BAs...> {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		auto rc = r
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return build_bf_constant<BAs...>(lc & rc);
	};

	// trivial cases
	if ( l == _0<BAs...> || r == _0<BAs...> ) return _0<BAs...>;
	if ( l == _1<BAs...> ) return r;
	if ( r == _1<BAs...> ) return l;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l)
		&& is_child_non_terminal<tau_parser::bf_constant, BAs...>(r))
			return bf_constant_and(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_non_terminal<tau_parser::bf, BAs...>(r))
			return build_bf_and<BAs...>(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_eq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<sp_tau_node<BAs...>>;
		return build_wff_eq<BAs...>(l & rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r)) {
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<sp_tau_node<BAs...>>;
		return build_wff_neq<BAs...>(l & rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
			&& is_non_terminal<tau_parser::wff, BAs...>(r))
		return build_wff_and<BAs...>(l, r);
	throw std::logic_error("wrong types");
}

template<typename... BAs>
sp_tau_node<BAs...> operator|(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_non_terminal<tau_parser::bf, BAs...>(r))
			return build_bf_or<BAs...>(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_eq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<sp_tau_node<BAs...>>;
		return build_wff_eq<BAs...>(l | rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<sp_tau_node<BAs...>>;
		return build_wff_neq<BAs...>(l | rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
		&& is_non_terminal<tau_parser::wff, BAs...>(r))
			return build_wff_or<BAs...>(l, r);
	throw std::logic_error("wrong types");
}

template<typename... BAs>
sp_tau_node<BAs...> operator~(const sp_tau_node<BAs...>& l) {
	auto bf_constant_neg = [](const auto& l) -> nso<BAs...> {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return build_bf_constant<BAs...>(~lc);
	};

	// trivial cases
	if (l == _0<BAs...>) return _1<BAs...>;
	if (l == _1<BAs...>) return _0<BAs...>;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l))
		return bf_constant_neg(l);
	if (is_non_terminal<tau_parser::bf>(l))
		return build_bf_neg<BAs...>(l);
	if (is_child_non_terminal<tau_parser::bf_eq, BAs...>(l)) {
		auto ll = l
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<sp_tau_node<BAs...>>;
		return build_wff_eq<BAs...>(~ll);
	}
	if (is_child_non_terminal<tau_parser::bf_neq, BAs...>(l)) {
		auto ll = l
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<sp_tau_node<BAs...>>;
		return build_wff_neq<BAs...>(~ll);
	}
	if (is_non_terminal<tau_parser::wff>(l))
		return build_wff_neg<BAs...>(l);
	throw std::logic_error("wrong types");
}

template<typename... BAs>
sp_tau_node<BAs...> operator^(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	auto bf_constant_xor = [](const auto& l, const auto& r) -> nso<BAs...> {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		auto rc = r
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return build_bf_constant<BAs...>(lc ^ rc);
	};

	// trivial cases
	if (l == _0<BAs...>) return r;
	if (r == _0<BAs...>) return l;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l)
		&& is_child_non_terminal<tau_parser::bf_constant, BAs...>(r))
			return bf_constant_xor(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_non_terminal<tau_parser::bf, BAs...>(r))
			return build_bf_xor<BAs...>(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_eq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<sp_tau_node<BAs...>>;
		return build_wff_eq<BAs...>(l ^ rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<sp_tau_node<BAs...>>;
		return build_wff_neq<BAs...>(l ^ rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
		&& is_non_terminal<tau_parser::wff, BAs...>(r))
			return build_wff_xor<BAs...>(l, r);
	throw std::logic_error("wrong types");
}

template<typename... BAs>
sp_tau_node<BAs...> operator+(const sp_tau_node<BAs...>& l,
	const sp_tau_node<BAs...>& r)
{
	return l ^ r;
}

// This function traverses n and normalizes coefficients in a BF
template<typename... BAs>
sp_tau_node<BAs...> normalize_ba(const sp_tau_node<BAs...>& fm) {
	assert(is_non_terminal(tau_parser::bf, fm));
	auto norm_ba = [](const auto& n, const auto& c) {
		if (!is_child_non_terminal(tau_parser::bf_constant, n))
			return n->child == c ? n : make_node(n->value, c);
		auto ba_elem = n
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		auto res = normalize_ba(ba_elem);
		using p = tau_parser;
		return wrap(p::bf, wrap(p::bf_constant, wrap(p::constant,
			make_node<tau_sym<BAs...>>(tau_sym<BAs...>(res), {}))));
	};
	return post_order_recursive_traverser<sp_tau_node<BAs...>>()(fm, all, norm_ba);
}

template<typename... BAs>
bool is_zero(const sp_tau_node<BAs...>& l) {
	auto bf_constant_is_zero = [](const auto& l) -> bool {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return is_zero(lc);
	};

	// trivial cases
	if (l == _0<BAs...>) return true;
	if (l == _1<BAs...>) return false;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l))
		return bf_constant_is_zero(l);
	if (is_non_terminal<tau_parser::bf>(l))
		return l == _0<BAs...>;
	if (is_non_terminal<tau_parser::wff>(l))
		return l == _F<BAs...>;
	throw std::logic_error("wrong types");
}

template<typename... BAs>
bool is_one(const sp_tau_node<BAs...>& l) {
	auto bf_constant_is_one = [](const auto& l) -> bool {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return is_one(lc);
	};

	// trivial cases
	if (l == _0<BAs...>) return false;
	if (l == _1<BAs...>) return true;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l))
		return bf_constant_is_one(l);
	if (is_non_terminal<tau_parser::bf>(l))
		return l == _1<BAs...>;
	if (is_non_terminal<tau_parser::wff>(l))
		return l == _T<BAs...>;
	throw std::logic_error("wrong types");
}

template<typename... BAs>
bool operator==(const sp_tau_node<BAs...>& l, const bool& r) {
	return r ? is_one(l) : is_zero(l);
}

template<typename... BAs>
bool operator==(const bool l, const sp_tau_node<BAs...>& r) {
	return r == l;
}

template<typename... BAs>
sp_tau_node<BAs...> operator<<(const sp_tau_node<BAs...>& n,
	const std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>>& changes)
{
	return replace(n, changes);
}

template<typename... BAs>
sp_tau_node<BAs...> operator<<(const sp_tau_node<BAs...>& n,
	const std::pair<sp_tau_node<BAs...>, sp_tau_node<BAs...>>& change)
{
	std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> changes{change};
	return replace(n, changes);
}

// Splitter function for a nso tau_parser::bf_constant node holding a BA constant
template<typename... BAs>
sp_tau_node<BAs...> splitter(const sp_tau_node<BAs...>& n,
	splitter_type st = splitter_type::upper)
{
	// Lambda for calling splitter on n
	auto _splitter = [&st](const auto& n) -> std::variant<BAs...> {
		return splitter(n, st);
	};

	assert(std::holds_alternative<std::variant<BAs...>>(trim2(n)->value));
	auto ba_constant = get<std::variant<BAs...>>(trim2(n)->value);
	std::variant<BAs...> v = std::visit(_splitter, ba_constant);
	return build_bf_constant<BAs...>(v);
}

// TODO (MEDIUM) unify this code with get_gssotc_clause and get_gssotc_literals
template<typename ...BAs>
void get_leaves(const sp_tau_node<BAs...>& n, tau_parser::nonterminal branch,
	tau_parser::nonterminal skip, std::vector<sp_tau_node<BAs...>>& leaves)
{
	if (auto check = n | branch; check) for (auto& c : check || skip)
		get_leaves(c, branch, skip, leaves);
	else {
		leaves.push_back(n);
		BOOST_LOG_TRIVIAL(trace) << "(I) get_leaves: found clause: " << n;
	}
}

template<typename ...BAs>
std::vector<sp_tau_node<BAs...>> get_leaves(const sp_tau_node<BAs...>& n,
	tau_parser::nonterminal branch, tau_parser::nonterminal skip)
{
	std::vector<sp_tau_node<BAs...>> leaves;
	get_leaves(n, branch, skip, leaves);
	return leaves;
}

// IDEA convert to a const static applier and change all the code accordingly
//
// however, the logic is quite complez a lot of private functions doesn't make
// sense outside the actual structure.
template <typename... BAs>
struct callback_applier {

	sp_tau_node<BAs...> operator()(const sp_tau_node<BAs...>& n) {
		if (!is_callback<BAs...>(n)) return n;
		auto nt = get<tau_source_sym>(n->value).n();
		switch (nt) {
		case tau_parser::bf_normalize_cb:
			return apply_unary_operation(_normalize, n);
		case tau_parser::bf_has_subformula_cb:
			return apply_has_subformula_check(n,tau_parser::bf_cb_arg);
		case tau_parser::wff_has_clashing_subformulas_cb:
			return apply_wff_clashing_subformulas_check(n);
		case tau_parser::wff_has_subformula_cb:
			return apply_has_subformula_check(n,tau_parser::wff_cb_arg);
		case tau_parser::wff_remove_existential_cb:
			return apply_wff_remove_existential(n);
		case tau_parser::bf_remove_funiversal_cb:
			return apply_bf_remove_funiversal(n);
		case tau_parser::bf_remove_fexistential_cb:
			return apply_bf_remove_fexistential(n);
		default: return n;
		}
	}

private:
	// TODO (MEDIUM) simplify following methods using the new node and ba_variant operators

	// unary operation
	static constexpr auto _normalize = [](const auto& n) -> std::variant<BAs...>{
		return normalize(n);
	};

	// ternary operators
	static constexpr auto _eq =
		[](const auto& l) -> bool { return l == false; };
	static constexpr auto _neq =
		[](const auto& l) -> bool { return !(l == false); };

	std::pair<sp_tau_node<BAs...>, sp_tau_node<BAs...>>
		get_quantifier_remove_constituents(
			const tau_parser::nonterminal type,
			const sp_tau_node<BAs...>& n)
	{
		auto args = n || type || only_child_extractor<BAs...>;
		std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> left_changes;
		left_changes[args[0] /* var */] = args[2] /* T */;
		auto left = replace<sp_tau_node<BAs...>>(
			args[1] /* formula */, left_changes);
		std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>>right_changes;
		right_changes[args[0] /* var */] = args[3] /* F */;
		auto right = replace<sp_tau_node<BAs...>>(
			args[1] /* formula */, right_changes);
		return {left, right};
	}

	sp_tau_node<BAs...> apply_bf_remove_funiversal(
		const sp_tau_node<BAs...>& n)
	{
		auto [left, right] = get_quantifier_remove_constituents(
						tau_parser::bf_cb_arg, n);
		return build_bf_and<BAs...>(left, right);
	}

	sp_tau_node<BAs...> apply_bf_remove_fexistential(
		const sp_tau_node<BAs...>& n)
	{
		auto [left, right] = get_quantifier_remove_constituents(
						tau_parser::bf_cb_arg, n);
		return build_bf_or<BAs...>(left, right);
	}

	std::optional<sp_tau_node<BAs...>> squeeze_positives(
		const sp_tau_node<BAs...> n)
	{
		if (auto positives = select_top(n,
			is_non_terminal<tau_parser::bf_eq, BAs...>);
							positives.size() > 0)
		{
			std::set<sp_tau_node<BAs...>> bfs;
			for (auto& p: positives) bfs.insert(p | tau_parser::bf
				| optional_value_extractor<sp_tau_node<BAs...>>);
			return build_bf_or<BAs...>(bfs);
		}
		return {};
	}

	sp_tau_node<BAs...> apply_wff_remove_existential(
		const sp_tau_node<BAs...>& n)
	{
		// Following Corollary 2.3 from Taba book from Ohad
		auto args = n || tau_parser::wff_cb_arg
				|| only_child_extractor<BAs...>;
		auto var = args[0] | only_child_extractor<BAs...>
				| optional_value_extractor<sp_tau_node<BAs...>>;
		auto wff = args[1];
		auto is_var = [&var](const auto& node){return node == var;};
		// if var does not appear in the formula, we can return the formula as is
		// if (!find_top(wff, is_var)) return wff;
		std::map<nso<BAs...>, nso<BAs...>> changes;
		for (const auto& l :
			get_leaves(wff, tau_parser::wff_or, tau_parser::wff))
		{
			// if var does not appear in the clause, we can skip it
			if (!find_top(l, is_var)) continue;
			// Get each conjunct in clause
			nso<BAs...> nl = _T<BAs...>;
			bool is_quant_removable_in_clause = true;
			for (const auto& conj : get_leaves(l,
				tau_parser::wff_and, tau_parser::wff))
			{
				// Check if conjunct is of form = 0 or != 0
				if ((conj | tau_parser::bf_eq)
					|| (conj | tau_parser::bf_neq))
						continue;
				// If the conjunct contains the quantified variable at this point
				// we cannot resolve the quantifier in this clause
				if (find_top(conj, is_var)) {
					is_quant_removable_in_clause = false;
					break;
				}
				// conjunct does not depend on var
				nl = build_wff_and(nl, conj);
			}
			if (!is_quant_removable_in_clause) {
				// Since we cannot remove the quantifier in this
				// clause it needs to be maintained
				changes[l] = build_wff_ex(var, l);
				continue;
			}

			auto f = squeeze_positives(l);
			std::map<nso<BAs...>, nso<BAs...>>
				changes_0 = {{var, _0_trimmed<BAs...>}},
				changes_1 = {{var, _1_trimmed<BAs...>}};
			auto f_0 = f ? replace(f.value(), changes_0)
					: _0<BAs...>;
			auto f_1 = f ? replace(f.value(), changes_1)
					: _0<BAs...>;

			if (auto neqs = select_all(l,
				is_non_terminal<tau_parser::bf_neq, BAs...>);
								neqs.size() > 0)
			{
				auto nneqs = _T<BAs...>;
				for (auto& neq: neqs) {
					auto g = neq | tau_parser::bf
						| optional_value_extractor<sp_tau_node<BAs...>>;
					auto g_0 = replace(g, changes_0);
					auto g_1 = replace(g, changes_1);
					nneqs = build_wff_and(nneqs,
						build_wff_neq(build_bf_or(
						build_bf_and(build_bf_neg(f_1),
									g_1),
						build_bf_and(build_bf_neg(f_0),
									g_0))));
				}
				nl = build_wff_and(nl, build_wff_and(
					build_wff_eq(build_bf_and(f_0, f_1)),
									nneqs));
			} else if (f) {
				nl = build_wff_and(nl, build_wff_eq(
						build_bf_and(f_0, f_1)));
			}
			changes[l] = nl;
		}
		return replace<sp_tau_node<BAs...>>(wff, changes);
	}

	sp_tau_node<BAs...> apply_has_subformula_check(sp_tau_node<BAs...> n,
		const tau_parser::nonterminal& cb_arg_t)
	{
		auto args = n || cb_arg_t || only_child_extractor<BAs...>;
		for (auto& subformula :
			select_all(args[0], all))
				if (subformula == args[1]) return args[2];
		return args[0];
	}

	// TODO (LOW) make a cleaner implementation
	// we should merge the two following methods and split the logic in
	// meaningful methods
	sp_tau_node<BAs...> apply_wff_clashing_subformulas_check(
		const sp_tau_node<BAs...>& n)
	{
		auto args = n || tau_parser::wff_cb_arg
				|| only_child_extractor<BAs...>;
		std::vector<sp_tau_node<BAs...>> positives, negatives;
		for (auto& op: select_all(args[0], all))
			if (is_non_terminal<tau_parser::wff_and>(op))
				for (auto& c: op->child)
		{
			if (auto check = c | tau_parser::wff_and;
				!check.has_value()
					&& is_non_terminal<tau_parser::wff>(c))
			{
				auto cc = c | only_child_extractor<BAs...>
					| optional_value_extractor<
							sp_tau_node<BAs...>>;
				(is_non_terminal<tau_parser::wff_neg>(cc)
					? negatives : positives).push_back(cc);
			}
		}
		for (auto& negation: negatives) {
			auto negated = negation | tau_parser::wff
				| only_child_extractor<BAs...>
				| optional_value_extractor<sp_tau_node<BAs...>>;
			for (auto& positive: positives)
				if (positive == negated) return args[1];
		}
		return args[0];
	}

	sp_tau_node<BAs...> apply_binary_operation(const auto& op,
		const sp_tau_node<BAs...>& n)
	{
		auto ba_elements = n || tau_parser::bf_cb_arg
			|| tau_parser::bf || only_child_extractor<BAs...>
			|| ba_extractor<BAs...>;
		sp_tau_node<BAs...> nn(
			std::visit(op, ba_elements[0], ba_elements[1]));
		return build_bf_constant<BAs...>(nn);
	}

	sp_tau_node<BAs...> apply_unary_operation(const auto& op,
		const sp_tau_node<BAs...>& n)
	{
		auto ba_elements = n || tau_parser::bf_cb_arg || tau_parser::bf
			|| only_child_extractor<BAs...> || ba_extractor<BAs...>;
		std::variant<BAs...> v = std::visit(op, ba_elements[0]);
		return build_bf_constant<BAs...>(v);
	}

	sp_tau_node<BAs...> apply_equality_relation(const auto& op,
		const sp_tau_node<BAs...>& n)
	{
		auto bf_arg = n | tau_parser::bf_cb_arg | tau_parser::bf
				| only_child_extractor<BAs...>;
		auto wff_args = n || tau_parser::wff_cb_arg
				|| only_child_extractor<BAs...>;
		auto ba_element = bf_arg | ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return std::visit(op, ba_element) ? wff_args[0] : wff_args[1];
	}

	sp_tau_node<BAs...> apply_subs(const sp_tau_node<BAs...>& n) {
		auto args = n || tau_parser::bf_cb_arg
					|| only_child_extractor<BAs...>;
		std::map<sp_tau_node<BAs...>, sp_tau_node<BAs...>> m;
		m[args[0]] = args[1];
		auto tmp = replace<sp_tau_node<BAs...>>(args[2], m)
			| only_child_extractor<BAs...>
			| optional_value_extractor<sp_tau_node<BAs...>>;
		return tmp;
	}
};

// apply one tau rule to the given expression
// IDEA maybe this could be operator|
template<typename... BAs>
nso<BAs...> nso_rr_apply(const rule<nso<BAs...>>& r, const nso<BAs...>& n) {

	#ifdef TAU_CACHE
	static std::map<std::pair<rule<nso<BAs...>>, nso<BAs...>>, nso<BAs...>> cache;
	if (auto it = cache.find({r, n}); it != cache.end()) return it->second;
	#endif // TAU_CACHE

	#ifdef TAU_MEASURE
	measures::increase_rule_counter<nso<BAs...>>(r);
	#endif // TAU_MEASURE

	// IDEA maybe we could traverse only once
	auto nn = apply_rule<nso<BAs...>, is_capture_t<BAs...>>(r, n, is_capture<BAs...>);
	#ifdef TAU_MEASURE
	if (n != nn) measures::increase_rule_hit<nso<BAs...>>(r);
	#endif // TAU_MEASURE

	std::map<nso<BAs...>, nso<BAs...>> changes;

	// compute changes from callbacks
	if (auto cbs = select_all(nn, is_callback<BAs...>); !cbs.empty()) {
		callback_applier<BAs...> cb_applier;
		for (auto& cb : cbs) {
			auto nnn = cb_applier(cb);
			changes[cb] = nnn;
		}
	}

	// apply numerical simplifications
	auto pred = is_non_terminal<BAs...>(tau_parser::shift);
	if (auto shifts = select_all(nn, pred); !shifts.empty())
		for (auto& shift : shifts)
	{
		auto args = shift || tau_parser::num;
		if (args.size() == 2) {
			auto left  = args[0] | only_child_extractor<BAs...>
				| offset_extractor<BAs...>
				| optional_value_extractor<size_t>;
			auto right = args[1] | only_child_extractor<BAs...>
				| offset_extractor<BAs...>
				| optional_value_extractor<size_t>;
			if (left < right) {
				BOOST_LOG_TRIVIAL(debug) << "(T) " << n;
				return n;
			}
			changes[shift] = build_num<BAs...>(left-right);
		}
	}

	// apply the changes and print info
	if (!changes.empty()) {
		auto cnn = replace<nso<BAs...>>(nn, changes);
		BOOST_LOG_TRIVIAL(debug) << "(C) " << cnn;
		#ifdef TAU_CACHE
		cache[{r, n}] = cnn;
		#endif // TAU_CACHE
		return cnn;
	}

	#ifdef TAU_CACHE
	cache[{r, n}] = nn;
	#endif // TAU_CACHE
	return nn;
}

// TODO (LOW) move it to a more appropriate place (parser)
template<typename... BAs>
nso<BAs...> replace_with(const nso<BAs...>& node, const nso<BAs...>& with,
	const nso<BAs...> in)
{
	std::map<nso<BAs...>, nso<BAs...>> changes = {{node, with}};
	return replace<nso<BAs...>>(in, changes);
}

// apply the given rules to the given expression
// IDEA maybe this could be operator|
template<typename... BAs>
nso<BAs...> nso_rr_apply(const rules<nso<BAs...>>& rs, const nso<BAs...>& n)
{
	#ifdef TAU_CACHE
	static std::map<std::pair<rules<nso<BAs...>>, nso<BAs...>>, nso<BAs...>> cache;
	if (auto it = cache.find({rs, n}); it != cache.end()) return it->second;
	#endif // TAU_CACHE

	if (rs.empty()) return n;
	nso<BAs...> nn = n;
	for (auto& r : rs) nn = nso_rr_apply<BAs...>(r, nn);

	#ifdef TAU_CACHE
	cache[{rs, n}] = nn;
	#endif // TAU_CACHE
	return nn;
}

//
// sp_tau_node factory methods
//
template<typename...BAs>
sp_tau_node<BAs...> first_argument_formula(const node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[0];
}

template<typename...BAs>
sp_tau_node<BAs...> first_argument_expression(const node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[0]->child[0];
}

template<typename...BAs>
sp_tau_node<BAs...> second_argument_formula(const node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[1];
}

template<typename...BAs>
sp_tau_node<BAs...> second_argument_expression(const node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[1]->child[0];
}

template<typename...BAs>
sp_tau_node<BAs...> third_argument_formula(const node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[2];
}

template<typename...BAs>
sp_tau_node<BAs...> third_argument_expression(const node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[2]->child[0];
}

template<typename...BAs>
sp_tau_node<BAs...> logic_operator(const node<tau_sym<BAs...>>& n) {
	return n.child[0];
}

template<typename...BAs>
sp_tau_node<BAs...> quantifier(const node<tau_sym<BAs...>>& n) {
	return n.child[0];
}

template<typename...BAs>
sp_tau_node<BAs...> quantified_formula(const node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[0];
}

template<typename...BAs>
sp_tau_node<BAs...> make_node_hook_cte_or(const node<tau_sym<BAs...>>& n) {
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto r = second_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	return l && r ? build_bf_constant<BAs...>(l.value() | r.value())
		: std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename...BAs>
sp_tau_node<BAs...> make_node_hook_bf_or(const node<tau_sym<BAs...>>& n) {
	//RULE(BF_SIMPLIFY_ONE_0, "1 | $X := 1.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_SIMPLIFY_ONE_1, "$X | 1 := 1.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_SIMPLIFY_ZERO_2, "0 | $X := $X.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_SIMPLIFY_ZERO_3, "$X | 0 := $X.")
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_SIMPLIFY_SELF_1, "$X | $X := $X.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return first_argument_formula(n);
	//RULE(BF_CALLBACK_OR, "{ $X } | { $Y } := bf_or_cb $X $Y.")
	if (is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
		&& is_non_terminal<tau_parser::bf_constant>(second_argument_expression(n)))
		return make_node_hook_cte_or(n);
	//RULE(BF_SIMPLIFY_SELF_3, "$X | $X' := 1.")
	if (auto negated = second_argument_formula(n) | tau_parser::bf_neg |tau_parser::bf;
			negated && negated.value() == first_argument_formula(n))
		return _1<BAs...>;
	//RULE(BF_SIMPLIFY_SELF_5, "$X' | $X := 1.")
	if (auto negated = first_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == second_argument_formula(n))
		return _1<BAs...>;
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename...BAs>
sp_tau_node<BAs...> make_node_hook_cte_and(const node<tau_sym<BAs...>>& n) {
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto r = second_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	return l && r ? build_bf_constant<BAs...>(l.value() & r.value())
		: std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_bf_and(const node<tau_sym<BAs...>>& n) {
	//RULE(BF_SIMPLIFY_ONE_2, "1 & $X := $X.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_SIMPLIFY_ONE_3, "$X & 1 := $X.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_SIMPLIFY_ZERO_0, "0 & $X := 0.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_SIMPLIFY_ZERO_1, "$X & 0 := 0.")
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_SIMPLIFY_SELF_0, "$X & $X := $X.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return first_argument_formula(n);
	//RULE(BF_CALLBACK_AND, "{ $X } & { $Y } := bf_and_cb $X $Y.")
	if (is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
		&& is_non_terminal<tau_parser::bf_constant>(second_argument_expression(n)))
		return make_node_hook_cte_and(n);
	//RULE(BF_SIMPLIFY_SELF_2, "$X & $X' := 0.")
	if (auto negated = second_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == first_argument_formula(n))
		return _0<BAs...>;
	//RULE(BF_SIMPLIFY_SELF_4, "$X' & $X := 0.")
	if (auto negated = first_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == second_argument_formula(n))
		return _0<BAs...>;
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename...BAs>
sp_tau_node<BAs...> make_node_hook_cte_neg(const node<tau_sym<BAs...>>& n) {
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	return l ? build_bf_constant<BAs...>(~l.value())
		: std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_bf_neg(const node<tau_sym<BAs...>>& n) {
	//RULE(BF_SIMPLIFY_ONE_4, "1' := 0.")
	if (auto neg_one = logic_operator(n) | tau_parser::bf | tau_parser::bf_t;
			neg_one && is_non_terminal<tau_parser::bf_neg>(logic_operator(n)))
		return _0<BAs...>;
	//RULE(BF_SIMPLIFY_ZERO_4, "0' := 1.")
	if (auto neg_zero = logic_operator(n) | tau_parser::bf | tau_parser::bf_f;
			neg_zero && is_non_terminal<tau_parser::bf_neg>(logic_operator(n)))
		return _1<BAs...>;
	//RULE(BF_ELIM_DOUBLE_NEGATION_0, "$X'' :=  $X.")
	if (auto double_neg = logic_operator(n) | tau_parser::bf | tau_parser::bf_neg; double_neg
			&& is_non_terminal<tau_parser::bf_neg>(logic_operator(n)))
		return double_neg.value()->child[0];
	//RULE(BF_CALLBACK_NEG, "{ $X }' := bf_neg_cb $X.")
	if (is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n)))
		return make_node_hook_cte_neg(n);
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename...BAs>
sp_tau_node<BAs...> make_node_hook_cte_xor(const node<tau_sym<BAs...>>& n) {
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto r = second_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	return l && r ? build_bf_constant<BAs...>(l.value() ^ r.value())
		: std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_bf_xor(const node<tau_sym<BAs...>>& n) {
	//RULE(BF_SIMPLIFY_ONE_N, "1 ^ $X := $X'.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n)))
		return build_bf_neg(second_argument_formula(n));
	//RULE(BF_SIMPLIFY_ONE_N, "$X ^ 1 := $X'.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_neg(first_argument_formula(n));
	//RULE(BF_SIMPLIFY_ZERO_N, "0 ^ $X := $X.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_SIMPLIFY_ZERO_N, "$X ^ 0 := $X.")
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_SIMPLIFY_SELF_N, "$X ^ $X := 0.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return wrap<BAs...>(tau_parser::bf, wrap<BAs...>(tau_parser::bf_f, {}));
	//RULE(BF_SIMPLIFY_SELF_N, "$X ^ $X := 0.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return _0<BAs...>;
	//RULE(BF_SIMPLIFY_SELF_2, "$X ^ $X' := 1.")
	if (auto negated = second_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == first_argument_formula(n))
		return _1<BAs...>;
	//RULE(BF_SIMPLIFY_SELF_2, "$X' ^ $X := 1.")
	if (auto negated = first_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == second_argument_formula(n))
		return _1<BAs...>;
	//RULE(BF_CALLBACK_XOR, "{ $X } ^ { $Y } := bf_xor_cb $X $Y.")
	if (is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
		&& is_non_terminal<tau_parser::bf_constant>(second_argument_expression(n)))
		return make_node_hook_cte_xor(n);
	return  build_bf_xor<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template<typename...BAs>
sp_tau_node<BAs...> make_node_hook_cte(const node<tau_sym<BAs...>>& n) {
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	//RULE(BF_CALLBACK_IS_ZERO, "{ $X } := bf_is_zero_cb { $X } 1.")
	//RULE(BF_CALLBACK_IS_ONE, "{ $X } := bf_is_one_cb { $X } 1.")
	if (l.has_value()) {
		if (l.value() == false) return _1<BAs...>;
		else if (l.value() == true) return _0<BAs...>;
	}
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_bf(const node<tau_sym<BAs...>>& n) {
	// if n is ref, capture, 0 or 1, we can return accordingly
	if (n.child.size() != 1) return std::make_shared<node<tau_sym<BAs...>>>(n);
	// otherwise we need to check the children
	if (is_non_terminal_node<BAs...>(n.child[0]))
		switch (std::get<tau_source_sym>(n.child[0]->value).n()) {
			case tau_parser::bf_or:
				return make_node_hook_bf_or<BAs...>(n);
			case tau_parser::bf_and:
				return make_node_hook_bf_and<BAs...>(n);
			case tau_parser::bf_neg:
				return make_node_hook_bf_neg<BAs...>(n);
			case tau_parser::bf_xor:
				return make_node_hook_bf_xor<BAs...>(n);
			case tau_parser::bf_constant:
				return make_node_hook_cte<BAs...>(n);
			default: return std::make_shared<node<tau_sym<BAs...>>>(n);
		}
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_and(const node<tau_sym<BAs...>>& n) {
	//RULE(WFF_SIMPLIFY_ONE_2, "T && $X ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ONE_3, "$X && T ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ZERO_0, "F && $X ::= F.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ZERO_1, "$X && F ::= F.")
	if (is_non_terminal<tau_parser::wff_f>(second_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_SIMPLIFY_SELF_0, "$X && $X ::= $X.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_SELF_2, "$X && ! $X ::= F.")
	if (auto negated = second_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == first_argument_formula(n))
		return _F<BAs...>;
	//RULE(WFF_SIMPLIFY_SELF_4, "!$X && $X ::= F.")
	if (auto negated = first_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == second_argument_formula(n))
		return _F<BAs...>;
	//RULE(BF_EQ_AND_SIMPLIFY_0, "$X != 0 && $X = 0 ::= F.")
	//RULE(BF_EQ_AND_SIMPLIFY_1, "$X = 0 && $X != 0 ::= F.")
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_or(const node<tau_sym<BAs...>>& n) {
	//RULE(WFF_SIMPLIFY_ONE_0, "T || $X ::= T.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ONE_1, "$X || T ::= T.")
	if (is_non_terminal<tau_parser::wff_t>(second_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ZERO_2, "F || $X ::= $X.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ZERO_3, "$X || F ::= $X.")
	if (is_non_terminal<tau_parser::wff_f>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_SELF_1, "$X || $X ::= $X.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_SELF_3, "$X || ! $X ::= T.")
	if (auto negated = second_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == first_argument_formula(n))
		return _T<BAs...>;
	//RULE(WFF_SIMPLIFY_SELF_5, "!$X || $X ::= T.")
	if (auto negated = first_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == second_argument_formula(n))
		return _T<BAs...>;
	//RULE(BF_EQ_OR_SIMPLIFY_0, "$X != 0 || $X = 0 ::= T.")
	//RULE(BF_EQ_OR_SIMPLIFY_1, "$X = 0 || $X != 0 ::= T.")
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_neg(const node<tau_sym<BAs...>>& n) {
	//RULE(WFF_SIMPLIFY_ONE_4, " ! T ::= F.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return _F<BAs...>;
	//RULE(WFF_SIMPLIFY_ZERO_4, "! F ::= T.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_ELIM_DOUBLE_NEGATION_0, "! ! $X ::=  $X.")
	if (auto double_neg = first_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff; double_neg)
		return double_neg.value();
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename...BAs>
sp_tau_node<BAs...> make_node_hook_wff_eq_cte(const node<tau_sym<BAs...>>& n) {
	auto l = n
		| tau_parser::bf_eq
		| tau_parser::bf
		| tau_parser::bf_constant
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	if (l.has_value() && l.value() == false) return _T<BAs...>;
	else if (l.has_value()) return _F<BAs...>;
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_eq(const node<tau_sym<BAs...>>& n) {
	//RULE(BF_EQ_SIMPLIFY_0, "1 = 0 ::=  F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _F<BAs...>;
	//RULE(BF_EQ_SIMPLIFY_1, "0 = 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(BF_DEF_SIMPLIFY_N, "$X = 1 ::= $X' = 0.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_eq<BAs...>(build_bf_neg(first_argument_formula(n)));
	if(is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return make_node_hook_wff_eq_cte(n);
	//RULE(BF_DEF_EQ, "$X = $Y ::= $X & $Y' | $X' & $Y = 0.")
	if (!is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_eq<BAs...>(
			build_bf_xor(first_argument_formula(n), second_argument_formula(n)));
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename...BAs>
sp_tau_node<BAs...> make_node_hook_wff_neq_cte(const node<tau_sym<BAs...>>& n) {
	auto l = n
		| tau_parser::bf_neq
		| tau_parser::bf
		| tau_parser::bf_constant
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	if (l.has_value() && l.value() == false) return _F<BAs...>;
	else if (l.has_value()) return _T<BAs...>;
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_neq(const node<tau_sym<BAs...>>& n) {
	//RULE(BF_NEQ_SIMPLIFY_0, "0 != 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _F<BAs...>;
	//RULE(BF_NEQ_SIMPLIFY_1, "1 != 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _T<BAs...>;
	// TODO (HIGH) fix this simplification rule, it gives problems during creation
	// of libraries
	//RULE(BF_DEF_SIMPLIFY_N, "$X != 1 ::= $X' != 0.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_neq<BAs...>(build_bf_neg(first_argument_formula(n)));
	if(is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return make_node_hook_wff_neq_cte(n);
	//RULE(BF_DEF_NEQ, "$X != $Y ::= $X & $Y' | $X' & $Y != 0.")
	if (!is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_neq<BAs...>(
			build_bf_xor(first_argument_formula(n), second_argument_formula(n)));
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_sometimes(const node<tau_sym<BAs...>>& n) {
	//RULE(WFF_SIMPLIFY_ONE_6, " sometimes T ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_SIMPLIFY_ZERO_6, "sometimes F ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return _F<BAs...>;
	//RULE(WFF_SIMPLIFY_SOMETIMES_1,  "sometimes sometimes $X ::= sometimes $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_sometimes; double_quantifier)
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_SOMETIMES_2,  "sometimes always $X ::= always $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_always;	double_quantifier)
		return first_argument_formula(n);
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_always(const node<tau_sym<BAs...>>& n) {
	//RULE(WFF_SIMPLIFY_ONE_5, " always T ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_SIMPLIFY_ZERO_5, "always F ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return _F<BAs...>;
	//RULE(WFF_SIMPLIFY_ALWAYS_1,     "always always $X ::= always $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_always;
			double_quantifier && is_non_terminal<tau_parser::wff_always>(quantifier(n)))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ALWAYS_2,     "always sometimes $X ::= sometimes $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_sometimes;
			double_quantifier && is_non_terminal<tau_parser::wff_always>(quantifier(n)))
		return first_argument_formula(n);
	return std::make_shared<node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_less(const node<tau_sym<BAs...>>& n) {
	//RULE(BF_LESS_SIMPLIFY_0, "$X < 0 ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _F<BAs...>;
	//RULE(BF_LESS_SIMPLIFY_2, "0 < 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(BF_DEF_SIMPLIFY_N, "$X < 1 ::= $X' != 0.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_neq<BAs...>(build_bf_neg(first_argument_formula(n)));

	return build_bf_less<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_less_equal(const node<tau_sym<BAs...>>& n) {
	//RULE(BF_LESS_EQUAL_SIMPLIFY_0, "$X <= 1 ::= T.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(BF_LESS_EQUAL_SIMPLIFY_2, "0 <= 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _T<BAs...>;
	return build_bf_less_equal<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_greater(const node<tau_sym<BAs...>>& n) {
	//RULE(BF_GREATER_SIMPLIFY_0, "$X > 1 ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _F<BAs...>;
	//RULE(BF_GREATER_SIMPLIFY_2, "1 > 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(BF_GREATER_SIMPLIFY_3, "0 > $X ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return _F<BAs...>;
	return build_bf_greater<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_nleq(const node<tau_sym<BAs...>>& n) {
	//RULE(BF_NLEQ_SIMPLIFY_0, "$X !<= 1 ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _F<BAs...>;
	//RULE(BF_NLEQ_SIMPLIFY_2, "1 !<= 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _T<BAs...>;
	return build_bf_nleq<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_interval(const node<tau_sym<BAs...>>& n) {
	return build_bf_interval<BAs...>(first_argument_formula(n), second_argument_formula(n), third_argument_formula(n));
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_xor(const node<tau_sym<BAs...>>& n) {
	//RULE(BF_XOR_SIMPLIFY_0, "$X ^ 0 ::= $X.")
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_XOR_SIMPLIFY_1, "0 ^ $X ::= $X.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_XOR_SIMPLIFY_2, "$X ^ $X ::= 0.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return _0<BAs...>;
	//RULE(BF_XOR_SIMPLIFY_3, "$X ^ ! $X ::= 1.")
	if (auto negated = second_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == first_argument_formula(n))
		return _1<BAs...>;
	//RULE(BF_XOR_SIMPLIFY_4, "! $X ^ $X ::= 1.")
	if (auto negated = first_argument_formula(n) | tau_parser::bf_neg | tau_parser::bf;
			negated && negated.value() == second_argument_formula(n))
		return _1<BAs...>;
	//RULE(BF_XOR_SIMPLIFY_5, "$X ^ 1 ::= ! $X.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_neg<BAs...>(first_argument_formula(n));
	//RULE(BF_XOR_SIMPLIFY_6, "1 ^ $X ::= ! $X.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n)))
		return build_wff_neg<BAs...>(second_argument_formula(n));
	return build_wff_xor<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_conditional(const node<tau_sym<BAs...>>& n) {
	//RULE(WFF_CONDITIONAL_SIMPLIFY_0, "0 ? $X : $Y ::= $Y.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return third_argument_formula(n);
	//RULE(WFF_CONDITIONAL_SIMPLIFY_1, "1 ? $X : $Y ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_CONDITIONAL_SIMPLIFY_4, "$X ? $Y : $Y ::= $Y.")
	if (second_argument_formula(n) == third_argument_formula(n))
		return third_argument_formula(n);
	return build_wff_conditional<BAs...>(first_argument_formula(n), second_argument_formula(n), third_argument_formula(n));
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_imply(const node<tau_sym<BAs...>>& n) {
	//RULE(WFF_IMPLY_SIMPLIFY_0, "F -> $X ::= T.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_IMPLY_SIMPLIFY_1, "T -> $X ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_IMPLY_SIMPLIFY_2, "$X -> F ::= ! $X.")
	if (is_non_terminal<tau_parser::wff_f>(second_argument_expression(n)))
		return build_wff_neg<BAs...>(first_argument_formula(n));
	//RULE(WFF_IMPLY_SIMPLIFY_3, "$X -> T ::= T.")
	if (is_non_terminal<tau_parser::wff_t>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_IMPLY_SIMPLIFY_4, "$X -> $X ::= T.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return _T<BAs...>;
	return build_wff_imply<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff_equiv(const node<tau_sym<BAs...>>& n) {
	//RULE(WFF_EQUIV_SIMPLIFY_0, "F <-> $X ::= ! $X.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return build_wff_neg<BAs...>(second_argument_formula(n));
	//RULE(WFF_EQUIV_SIMPLIFY_1, "T <-> $X ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_EQUIV_SIMPLIFY_2, "$X <-> F ::= ! $X.")
	if (is_non_terminal<tau_parser::wff_f>(second_argument_expression(n)))
		return build_wff_neg<BAs...>(first_argument_formula(n));
	//RULE(WFF_EQUIV_SIMPLIFY_3, "$X <-> T ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(WFF_EQUIV_SIMPLIFY_4, "$X <-> $X ::= T.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return _T<BAs...>;
	//RULE(WFF_EQUIV_SIMPLIFY_5, "$X <-> ! $X ::= F.")
	if (auto negated = second_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == first_argument_formula(n))
		return _F<BAs...>;
	//RULE(WFF_EQUIV_SIMPLIFY_6, "! $X <-> $X ::= F.")
	if (auto negated = first_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == second_argument_formula(n))
		return _F<BAs...>;
	return build_wff_equiv<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template<typename... BAs>
sp_tau_node<BAs...> make_node_hook_wff(const node<tau_sym<BAs...>>& n) {
	switch (get_non_terminal_node(logic_operator(n))) {
		case tau_parser::wff_and:
			return make_node_hook_wff_and<BAs...>(n);
		case tau_parser::wff_or:
			return make_node_hook_wff_or<BAs...>(n);
		case tau_parser::wff_neg:
			return make_node_hook_wff_neg<BAs...>(n);
		case tau_parser::bf_eq:
			return make_node_hook_wff_eq<BAs...>(n);
		case tau_parser::bf_neq:
			return make_node_hook_wff_neq<BAs...>(n);
		case tau_parser::wff_sometimes:
			return make_node_hook_wff_sometimes<BAs...>(n);
		case tau_parser::wff_always:
			return make_node_hook_wff_always<BAs...>(n);

		case tau_parser::wff_xor:
			return make_node_hook_wff_xor<BAs...>(n);
		case tau_parser::wff_conditional:
			return make_node_hook_wff_conditional<BAs...>(n);
		case tau_parser::wff_imply:
			return make_node_hook_wff_imply<BAs...>(n);
		case tau_parser::wff_equiv:
			return make_node_hook_wff_equiv<BAs...>(n);

		case tau_parser::bf_less:
			return make_node_hook_wff_less<BAs...>(n);
		case tau_parser::bf_less_equal:
			return make_node_hook_wff_less_equal<BAs...>(n);
		case tau_parser::bf_greater:
			return make_node_hook_wff_greater<BAs...>(n);
		case tau_parser::bf_nleq:
			return make_node_hook_wff_nleq<BAs...>(n);
		case tau_parser::bf_interval:
			return make_node_hook_wff_interval<BAs...>(n);

		default: return std::make_shared<node<tau_sym<BAs...>>>(n);
	}
}

template <typename...BAs>
struct make_tau_node {
	std::optional<sp_tau_node<BAs...>> operator()(const node<tau_sym<BAs...>>& n) {
		if (is_non_terminal_node<BAs...>(n)) {
			switch (get_non_terminal_node(n)) {
				case tau_parser::bf: {
					return make_node_hook_bf<BAs...>(n);
				}
				case tau_parser::wff: {
					return make_node_hook_wff<BAs...>(n);
				}
				default: return std::optional<sp_tau_node<BAs...>>();
			}
		}
		return std::optional<sp_tau_node<BAs...>>();
	}
};

} // namespace idni::tau

namespace idni::rewriter {

template <typename...BAs>
struct make_node_hook<idni::tau::tau_sym<BAs...>> {
	std::optional<idni::tau::sp_tau_node<BAs...>> operator()(const node<idni::tau::tau_sym<BAs...>>& n) {
		static idni::tau::make_tau_node<BAs...> hook;
		return hook(n);
	}
};

} // namespace idni::rewriter

//
// operators << to pretty print the tau language related types
//

template <typename...BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::rewriter::rule<idni::tau::sp_tau_node<BAs...>>& r)
{
	return stream << r.first << " := " << r.second << ".";
}

// << for rules
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau::rules<idni::tau::nso<BAs...>>& rs)
{
	for (const auto& r : rs) stream << r << " ";
	return stream;
}

// << for tau_source_sym
std::ostream& operator<<(std::ostream& stream,
				const idni::tau::tau_source_sym& rs);

// << for BAs... variant
template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const std::variant<BAs...>& rs) {
	std::visit(overloaded {
		[&os](const auto& a) { os << a; }
	}, rs);
	return os;
}

// << for tau_sym
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau::tau_sym<BAs...>& rs)
{
	// using tau_sym = std::variant<tau_source_sym, std::variant<BAs...>, size_t>;
	std::visit(overloaded {
		[&stream](const idni::tau::tau_source_sym& t) {
			if (!t.nt() && !t.is_null()) stream << t.t();
		},
		[&stream](const auto& a) { stream << a; }
	}, rs);
	return stream;
}

// << for formulas
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau::rr<idni::tau::nso<BAs...>>& f)
{
	stream << f.rec_relations;
	if (f.main) stream << f.main << '.';
	return stream;
}

// << for bindings
// TODO (HIGH) << for bindings depends on << for variant<BAs...>
// TODO (HIGH) << for bindings needs to follow tau lang syntax
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau::bindings<BAs...>& bs)
{
	for (const auto& b : bs) stream << b.first << ": " << b.second << "\n";
	return stream;
}

// outputs a sp_tau_node<...> to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// IDEA maybe it should be move to out.h
template <typename... BAs>
std::ostream& pp(std::ostream& stream, const idni::tau::sp_tau_node<BAs...>& n,
	size_t parent = tau_parser::start, bool passthrough = false);

template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau::sp_tau_node<BAs...>& n) { return pp(stream, n); }

// old operator<< renamed to print_terminals and replaced by
// pp pretty priniter
template <typename... BAs>
std::ostream& print_terminals(std::ostream& stream,
	const idni::tau::sp_tau_node<BAs...>& n)
{
	stream << n->value;
	for (const auto& c : n->child) print_terminals<BAs...>(stream, c);
	return stream;
}

// outputs a sp_tau_source_node to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// IDEA maybe it should be move to out.h
std::ostream& operator<<(std::ostream& stream,
			const idni::tau::sp_tau_source_node& n);

// << tau_source_node (make it shared to make use of the previous operator)
std::ostream& operator<<(std::ostream& stream,
					const idni::tau::tau_source_node& n);

template <typename... BAs>
std::ostream& pp(std::ostream& stream, const idni::tau::sp_tau_node<BAs...>& n,
	size_t parent, bool passthrough)
{
// #define DEBUG_PP
// #ifdef DEBUG_PP
// auto& p = tau_parser::instance();
// 	auto dbg = [&stream, &p](const auto& c) {
// 		if (std::holds_alternative<idni::tau::tau_source_sym>(c->value)) {
// 			auto tss = std::get<idni::tau::tau_source_sym>(c->value);
// 			if (tss.nt()) stream << " NT:" << p.name(tss.n()) << " ";
// 			else if (tss.is_null()) stream << " <NULL> ";
// 			else stream << " T:'" << tss.t() << "' ";
// 		} else stream << " NONLIT:`" <<c->value << "` ";
// 	};
// 	stream << "\n";
// 	dbg(n);
// 	stream << "    child.size: " << n->child.size() << "\n";
// 	for (const auto& c : n->child)
// 		stream << "\t", dbg(c), stream << "\n";
// #endif // DEBUG_PP
	static auto is_to_wrap = [](const idni::tau::sp_tau_node<BAs...>& n,
		size_t parent)
	{
		static const std::set<size_t> no_wrap_for = {
			tau_parser::bf_splitter,
			tau_parser::bf_ref,
			tau_parser::bf_neg,
			tau_parser::bf_constant,
			tau_parser::bf_t,
			tau_parser::bf_f,
			tau_parser::wff_ref,
			tau_parser::wff_neg,
			tau_parser::wff_t,
			tau_parser::wff_f,
			tau_parser::capture,
			tau_parser::variable,
			tau_parser::uninterpreted_constant,
			tau_parser::ref_args,
			tau_parser::start
		};
		// lower number = higher priority
		static const std::map<size_t, size_t> prio = {
			{ tau_parser::start,                             0 },
			// cli commands
			{ tau_parser::help_cmd,                         50 },
			{ tau_parser::file_cmd,                         50 },
			{ tau_parser::normalize_cmd,                    50 },
			{ tau_parser::execute_cmd,                      50 },
			{ tau_parser::solve_cmd,                        50 },
			{ tau_parser::dnf_cmd,                          50 },
			{ tau_parser::cnf_cmd,                          50 },
			{ tau_parser::anf_cmd,                          50 },
			{ tau_parser::nnf_cmd,                          50 },
			{ tau_parser::pnf_cmd,                          50 },
			{ tau_parser::mnf_cmd,                          50 },
			{ tau_parser::onf_cmd,                          50 },
			{ tau_parser::inst_cmd,                         50 },
			{ tau_parser::subst_cmd,                        50 },
			{ tau_parser::def_rr_cmd,                       50 },
			{ tau_parser::def_list_cmd,                     50 },
			{ tau_parser::history_list_cmd,                 50 },
			{ tau_parser::history_print_cmd,                50 },
			{ tau_parser::history_store_cmd,                50 },
			{ tau_parser::sat_cmd,                        50 },
			{ tau_parser::main,                             60 },
			{ tau_parser::ref,                              80 },
			{ tau_parser::wff,                              90 },
			// wff
			{ tau_parser::wff_has_clashing_subformulas_cb, 320 },
			{ tau_parser::wff_has_subformula_cb,           330 },
			{ tau_parser::wff_remove_existential_cb,       340 },

			{ tau_parser::wff_sometimes,                   380 },
			{ tau_parser::wff_always,                      390 },
			{ tau_parser::wff_conditional,                 400 },
			{ tau_parser::wff_all,                         430 },
			{ tau_parser::wff_ex,                          440 },
			{ tau_parser::wff_imply,                       450 },
			{ tau_parser::wff_equiv,                       460 },
			{ tau_parser::wff_or,                          470 },
			{ tau_parser::wff_and,                         480 },
			{ tau_parser::wff_xor,                         490 },
			{ tau_parser::wff_neg,                         500 },
			{ tau_parser::bf_interval,                     510 },
			{ tau_parser::bf_neq,                          520 },
			{ tau_parser::bf_eq,                           530 },
			{ tau_parser::bf_nleq,                         540 },
			{ tau_parser::bf_greater,                      550 },
			{ tau_parser::bf_less_equal,                   560 },
			{ tau_parser::bf_less,                         570 },
			{ tau_parser::wff,                             580 },
			// bf
			{ tau_parser::bf_has_subformula_cb,            620 },
			{ tau_parser::bf_remove_funiversal_cb,         630 },
			{ tau_parser::bf_remove_fexistential_cb,       640 },

			{ tau_parser::bf_or,                           720 },
			{ tau_parser::bf_and,                          730 },
			{ tau_parser::bf_xor,                          740 },
			{ tau_parser::flag,                            745 },
			{ tau_parser::bf_neg,                          750 },
			{ tau_parser::bf,                              790 },

			{ tau_parser::rec_relation,                    800 },
			{ tau_parser::ref_args,                        800 }
		};
		static const std::set<size_t> wrap_child_for = {
			};
		if (std::holds_alternative<idni::tau::tau_source_sym>(n->value)) {
			auto tss = std::get<idni::tau::tau_source_sym>(n->value);
			if (!tss.nt() || no_wrap_for.find(tss.n())
						!= no_wrap_for.end())
								return false;
			if (prio.find(tss.n()) == prio.end()) {
				std::cerr << "No priority for " << tss.n() << "\n";
				return false;
			}
			if (prio.find(parent) == prio.end()) {
				std::cerr << "No priority for " << parent << "\n";
				return false;
			}
			// tau_parser& p = tau_parser::instance();
			// std::cerr
			// 	<< p.name(parent) << " vs " << p.name(tss.n())
			// 	//<< "(" << parent << ")
			// 	<< " " << prio.at(parent)
			// 	<< (prio.at(parent) > prio.at(tss.n())
			// 		? " > " : " <= ")
			// 	<< prio.at(tss.n())
			// 	// << " (" << tss.n() << ")"
			// 	<< "\n";
			return prio.at(parent) > prio.at(tss.n()) ? true
					: wrap_child_for.contains(parent);
		}
		return false;
	};

	if (passthrough) { // passthrough
		//auto ch = get_children(n->child);
		for (const auto& c : n->child) pp(stream, c, parent);
		return stream;
	}


	if (std::holds_alternative<idni::tau::tau_source_sym>(n->value)) {
		auto& ch = n->child;
		auto tss = std::get<idni::tau::tau_source_sym>(n->value);
		auto sep = [&](const std::string& separator) {
			for (size_t i = 0; i < ch.size(); ++i) {
				if (i > 0) stream << separator << " ";
				pp(stream, ch[i], tss.n());
			}
		};
		auto pass = [&]() {
			sep("");
		};
		//auto pass_nows = [&]() {
		//	for (const auto& c : ch) pp(stream, c, tss.n());
		//};
		auto infix_nows = [&](const std::string& op) {
			pp(stream, ch[0], tss.n());
			stream << op;
			pp(stream, ch[1], tss.n());
		};
		auto infix = [&](const std::string& op) {
			infix_nows(std::string(" ") + op + " ");
		};
		auto infix2 = [&](const std::string& op1,
			const std::string& op2)
		{
			infix(op1);
			stream << " " << op2 << " ";
			pp(stream, ch[2], tss.n());
		};
		auto postfix_nows = [&](const std::string& postf) {
			pass(), stream << postf;
		};
		auto prefix_nows = [&](const std::string& pref) {
			stream << pref, pass();
		};
		auto prefix = [&](const std::string& pref) {
			prefix_nows(pref + " ");
		};
		auto wrap = [&](const std::string& pref,
			const std::string& postf)
		{
			stream << pref, pass(), stream << postf;
		};
		auto quant = [&]() {
			using namespace idni::tau;
			size_t quant_nt = tss.n();
			auto qch = ch;
			//std::cout << "(quant_nt=" << quant_nt << ")";
			switch (quant_nt) {
			case tau_parser::wff_all:  stream << "all";   break;
			case tau_parser::wff_ex:   stream << "ex";    break;
			}
			sp_tau_node<BAs...> expr;
			size_t expr_nt;
			size_t counter = 0;
			do {
				pp(stream << " ", qch[0], quant_nt);
				expr = qch[1]->child[0];
				expr_nt = expr | non_terminal_extractor<BAs...>
					| optional_value_extractor<size_t>;
				//std::cout << "(expr_nt=" << expr_nt << ")";
				if (expr_nt == quant_nt) {
					stream << ",", qch = expr->child;
				} else {
					pp(stream << " ", expr, tss.n());
					break;
				}
				if (++counter > 10) break;
			} while (true);
		};
		if (tss.nt()) { //stream /*<< "*" << tss.nts << "-"*/ << tau_parser::instance().name(tss.n()) << ":";
			switch (tss.n()) {
			case tau_parser::main:
			case tau_parser::builder:
				postfix_nows("."); break;
			case tau_parser::rec_relation:
				infix(":=");  stream << "."; break;
			case tau_parser::wff_rule:
				infix("::="); stream << "."; break;
			case tau_parser::bf_rule:
				infix(":=");  stream << "."; break;
			case tau_parser::bf_builder_body:  prefix("=:");  break;
			case tau_parser::wff_builder_body: prefix("=::"); break;
			case tau_parser::inputs:           prefix("<");   break;
			case tau_parser::input:
				infix(": {"); stream << " }"; break;
			case tau_parser::in:
			case tau_parser::out:
				infix_nows("["); stream << "]"; break;
			// wrappable by parenthesis
			case tau_parser::bf:
			case tau_parser::wff:
			{
				//assert(ch.size() <= 1);
				//if (ch.size() > 1) wrap("(", ")");
				//else {
				bool wrap = is_to_wrap(ch[0], parent);
				if (wrap) stream << "(";
				pp(stream, ch[0], parent);
				if (wrap) stream << ")";
				//}
			} break;
			case tau_parser::shift:
				if (ch.size() == 1) pass();
				else infix_nows("-");
				break;
			case tau_parser::source_binding: infix(":"); break;
			// nodes to wrap
			case tau_parser::bf_splitter: wrap("S(", ")"); break;
			case tau_parser::bf_constant:
				wrap("{ ", " }"); break;
			case tau_parser::builder_head:
				wrap("(" , ")"); break;
			case tau_parser::offsets:
				stream << "[", sep(","), stream << "]"; break;
			case tau_parser::ref_args:
				stream << "(", sep(","), stream << ")"; break;
			// unary operators
			case tau_parser::wff_neg:        prefix_nows("!"); break;
			case tau_parser::bf_neg:         postfix_nows("'"); break;
			case tau_parser::wff_sometimes:  prefix("sometimes"); break;
			case tau_parser::wff_always:     prefix("always"); break;
			//
			// binary operators
			case tau_parser::bf_and:         infix("&"); break;
			case tau_parser::bf_or:          infix("|"); break;
			case tau_parser::bf_xor:         infix("+"); break;
			case tau_parser::bf_eq:          infix("="); break;
			case tau_parser::bf_neq:         infix("!="); break;
			case tau_parser::bf_less:        infix("<"); break;
			case tau_parser::bf_less_equal:  infix("<="); break;
			case tau_parser::bf_nleq:        infix("!<="); break;
			case tau_parser::bf_greater:     infix(">"); break;
			case tau_parser::flag_neq:           infix("!="); break;
			case tau_parser::flag_eq:            infix("=");  break;
			case tau_parser::flag_greater_equal: infix(">="); break;
			case tau_parser::flag_greater:       infix(">");  break;
			case tau_parser::flag_less_equal:    infix("<="); break;
			case tau_parser::flag_less:          infix("<");  break;
			case tau_parser::wff_and:        infix("&&"); break;
			case tau_parser::wff_or:         infix("||"); break;
			case tau_parser::wff_xor:        infix("^"); break;
			case tau_parser::wff_imply:      infix("->"); break;
			case tau_parser::wff_equiv:      infix("<->"); break;
			// ternary operators
			case tau_parser::bf_interval:    infix2("<=", "<="); break;
			case tau_parser::wff_conditional:infix2("?", ":"); break;
			// quantifiers
			case tau_parser::wff_all:
			case tau_parser::wff_ex:         quant(); break;
			// callbacks
			case tau_parser::bf_normalize_cb:prefix("bf_normalize_cb"); break;
			case tau_parser::bf_remove_funiversal_cb:
				prefix("bf_remove_funiversal_cb"); break;
			case tau_parser::bf_remove_fexistential_cb:
				prefix("bf_remove_fexistential_cb"); break;
			case tau_parser::wff_remove_existential_cb:
				prefix("wff_remove_existential_cb"); break;
			case tau_parser::wff_has_clashing_subformulas_cb:
				prefix("wff_has_clashing_subformulas_cb"); break;
			case tau_parser::bf_has_subformula_cb:
				prefix("bf_has_subformula_cb"); break;
			case tau_parser::wff_has_subformula_cb:
				prefix("wff_has_subformula_cb"); break;
			// cli commands
			case tau_parser::cli:           sep(". "); break;
			case tau_parser::rel_memory:    prefix_nows("%-"); break;
			case tau_parser::abs_memory:    prefix_nows("%"); break;
			case tau_parser::quit_cmd:      stream << "quit"; break;
			case tau_parser::version_cmd:   stream << "version"; break;
			case tau_parser::clear_cmd:     stream << "clear"; break;
			case tau_parser::help_cmd:      prefix("help"); break;
			case tau_parser::file_cmd:      prefix("file"); break;
			case tau_parser::valid_cmd:     prefix("valid"); break;
			case tau_parser::sat_cmd:       prefix("sat"); break;
			case tau_parser::unsat_cmd:     prefix("unsat"); break;
			case tau_parser::solve_cmd:     prefix("solve"); break;
			case tau_parser::execute_cmd:   prefix("execute"); break;
			case tau_parser::normalize_cmd: prefix("normalize"); break;
			case tau_parser::inst_cmd:
			case tau_parser::subst_cmd:
				stream << (tss.n() == tau_parser::inst_cmd
						? "instantiate" : "substitute");
				pp(stream << " ",   ch[1], tss.n());
				pp(stream << " [",  ch[2], tss.n());
				pp(stream << " / ", ch[3], tss.n());
				stream << "]";
				break;
			case tau_parser::dnf_cmd:       prefix("dnf"); break;
			case tau_parser::cnf_cmd:       prefix("cnf"); break;
			case tau_parser::anf_cmd:       prefix("anf"); break;
			case tau_parser::nnf_cmd:       prefix("nnf"); break;
			case tau_parser::pnf_cmd:       prefix("pnf"); break;
			case tau_parser::mnf_cmd:       prefix("mnf"); break;
			case tau_parser::snf_cmd:       prefix("snf"); break;
			case tau_parser::onf_cmd:       prefix("onf"); break;
			case tau_parser::def_print_cmd:
			case tau_parser::def_rr_cmd:    prefix("def"); break;
			case tau_parser::def_list_cmd:  stream << "def"; break;
			case tau_parser::history_list_cmd:
						stream << "history"; break;
			case tau_parser::history_print_cmd:
			case tau_parser::history_store_cmd:
							prefix("history"); break;
			case tau_parser::get_cmd:       prefix("get"); break;
			case tau_parser::set_cmd:       prefix("set"); break;
			case tau_parser::toggle_cmd:    prefix("toggle"); break;
			// just print terminals for these
			case tau_parser::in_var_name:
			case tau_parser::out_var_name:
			case tau_parser::chars:
			case tau_parser::capture:
			case tau_parser::sym:
			case tau_parser::num:
			case tau_parser::type:
			case tau_parser::source:
			case tau_parser::named_binding:
			case tau_parser::uninter_const_name:
				print_terminals(stream, n);
				break;
			// constants
			case tau_parser::uninterpreted_constant:
							wrap("<", ">"); break;
			// simple symbols
			case tau_parser::bf_f:  stream << '0'; break;
			case tau_parser::bf_t:  stream << '1'; break;
			case tau_parser::wff_f: stream << 'F'; break;
			case tau_parser::wff_t: stream << 'T'; break;
			case tau_parser::severity_opt: stream << "severity"; break;
			case tau_parser::status_opt: stream << "status"; break;
			case tau_parser::colors_opt: stream << "colors"; break;
			case tau_parser::debug_repl_opt: stream << "debug"; break;
			case tau_parser::option_value_true: stream << "true"; break;
			case tau_parser::option_value_false: stream << "false"; break;
			case tau_parser::error_sym: stream << "error"; break;
			case tau_parser::debug_sym: stream << "debug"; break;
			case tau_parser::trace_sym: stream << "trace"; break;
			case tau_parser::info_sym:  stream << "info"; break;
			case tau_parser::extra: break; // We do not output this
			// for the rest skip value and just passthrough to child
			default: for (const auto& c : n->child)
					pp(stream, c, parent);
				break;
			}
		}
		else if (!tss.is_null()) stream << tss.t();
	} else stream << n->value;
	return stream;
}

#endif // __NSO_RR_H__