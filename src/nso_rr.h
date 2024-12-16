// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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

#include "init_log.h"
#include "bool_ba.h"
#include "variant_ba.h"
#include "splitter_types.h"
#include "parser.h"
#include "utils.h"
#include "../parser/tau_parser.generated.h"
#include "rewriting.h"
#include "term_colors.h"

#ifdef TAU_MEASURE
#include "measure.h"
#endif // TAU_MEASURE

namespace idni::tau_lang {

extern bool pretty_printer_highlighting;
extern bool pretty_printer_indenting;

//
// types related to the tau language
//

// tau_source_node is the type of nodes we use to represent get from parsing tau language
using tau_source_sym = idni::lit<char, char>;
using tau_source_node = rewriter::node<idni::lit<char, char>>;
using sp_tau_source_node = rewriter::sp_node<idni::lit<char, char>>;

// node type for the tau language related programs, libraries and
// specifications trees.
template <typename... BAs>
using tau_sym = std::variant<tau_source_sym, std::variant<BAs...>, size_t>;

template <typename... BAs>
using tau = rewriter::sp_node<tau_sym<BAs...>>;

template <typename... BAs>
bool is_non_terminal(const size_t, const tau<BAs...>&);
template <typename... BAs>
bool is_child_non_terminal(const size_t nt, const tau<BAs...>& n);
template <size_t nt, typename...BAs>
bool is_child_non_terminal(const tau<BAs...>& n);

// We overload the == operator for tau in order to store additional data
// which is not taken into account for the quality check
template <typename... BAs>
bool operator==(const tau<BAs...> &l, const tau<BAs...>& r) {
	if (r == nullptr && l == nullptr) return true;
	if (r == nullptr || l == nullptr) return false;

	if (std::addressof(*l) == std::addressof(*r)) return true;

	// check if typed bf_f or bf_t
	if (is_non_terminal(tau_parser::bf_f, l)
			&& is_non_terminal(tau_parser::bf_f, r)) {
		return (!l->child.empty() && !r->child.empty())
			? (l->child[0] == r->child[0])
			: true;
	}

	if (is_non_terminal(tau_parser::bf_t, l)
			&& is_non_terminal(tau_parser::bf_t, r)) {
		return (!l->child.empty() && !r->child.empty())
			? (l->child[0] == r->child[0])
			: true;
	}

	// check if the nodes have extra data
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
		for (size_t i = 0; i < r->child.size(); ++i) {
			if (!(l->child[i] == r->child[i]))
				return false;
		}
		return l->value == r->value;
	}
	if (!l_has_extra && r_has_extra) {
		if (l->child.size() + 1 != r->child.size()) return false;
		for (size_t i = 0; i < l->child.size(); ++i) {
			if (!(l->child[i] == r->child[i]))
				return false;
		}
		return l->value == r->value;
	}
	if (l_has_extra && r_has_extra) {
		if (l->child.size() != r->child.size()) return false;
		for (size_t i = 1; i < l->child.size(); ++i) {
			if (!(l->child[i-1] == r->child[i-1]))
				return false;
		}
		return l->value == r->value;
	}
	// All cases are covered above
	assert(false);
	return false;
}

template <typename... BAs>
bool operator!=(const tau<BAs...>& l, const tau<BAs...>& r) {
	return !(l == r);
}

template <typename node_t>
using rec_relation = rewriter::rule<node_t>;

// IDEA maybe we could define a wrapper for recursive rules and rewriting rules that
// call the appropriate apply method. This would play also nice with the builders
// defined in the normalizer.

template <typename... BAs>
using builder = rewriter::rule<tau<BAs...>>;

// defines a vector of rules in the tau language, the order is important as it defines
// the order of the rules in the rewriting process of the tau language.
template <typename node_t>
using rules = std::vector<rewriter::rule<node_t>>;

// defines a vector of rec. relations in the tau language, the order is important as it defines
// the order of the rec relations in the rewriting process of the tau language.
template <typename node_t>
using rec_relations = std::vector<rec_relation<node_t>>;

// a library is a set of rules to be applied in the rewriting process of the tau
// language, the order of the rules is important.
template <typename node_t>
using library = rules<node_t>;

// bindings map tau_source constants (strings) into elements of the boolean algebras.
template <typename... BAs>
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
template <typename... BAs>
bool is_non_terminal_node(const rewriter::node<tau_sym<BAs...>>& s) {
	return std::holds_alternative<tau_source_sym>(s.value)
		&& get<tau_source_sym>(s.value).nt();
}

template <typename... BAs>
bool is_non_terminal_node(const tau<BAs...>& n) {
	return std::holds_alternative<tau_source_sym>(n->value)
					&& get<tau_source_sym>(n->value).nt();
};

template <typename... BAs>
size_t get_non_terminal_node(const tau_sym<BAs...>& s) {
	return get<tau_source_sym>(s).n();
}

template <typename... BAs>
size_t get_non_terminal_node(const rewriter::node<tau_sym<BAs...>>& n) {
	return get_non_terminal_node(n.value);
}

template <typename... BAs>
size_t get_non_terminal_node(const tau<BAs...>& n) {
	return get_non_terminal_node(*n);
}

// factory method for is_non_terminal_node predicate
template <typename... BAs>
std::function<bool(const tau<BAs...>&)> is_non_terminal_node() {
	return [](const tau<BAs...>& n) {
		return is_non_terminal_node<BAs...>(n); };
}

// check if the node is the given non terminal
template <typename... BAs>
bool is_non_terminal(const size_t nt, const tau<BAs...>& n) {
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
bool is_non_terminal(const tau<BAs...>& n) {
	return is_non_terminal<BAs...>(nt, n);
}

template <size_t nt, typename...BAs>
bool is_non_terminal_sym(const tau_sym<BAs...>& s) {
	return is_non_terminal_sym<BAs...>(nt, s);
}

// factory method for is_non_terminal predicate
template <typename... BAs>
std::function<bool(const tau<BAs...>&)> is_non_terminal(const size_t nt)
{
	return [nt](const tau<BAs...>& n) {
		return is_non_terminal<BAs...>(nt, n); };
}

// check if a node is a terminal
template <typename... BAs>
bool is_terminal_node(const tau<BAs...>& n) {
	return std::holds_alternative<tau_source_sym>(n->value)
					&& !get<tau_source_sym>(n->value).nt();
};

// factory method for is_terminal_node predicate
template <typename... BAs>
std::function<bool(const tau<BAs...>&)> is_terminal_node() {
	return [](const tau<BAs...>& n) {
		return is_terminal_node<BAs...>(n); };
}

// check if the node is the given terminal (functional approach)
template <typename...BAs>
bool is_terminal(const char c, const tau<BAs...>& n) {
	return is_terminal<BAs...>(n) && get<tau_source_sym>(n->value).n() == c;
};

// check if the node is the given terminal (template approach)
template <char c, typename...BAs>
bool is_terminal(const tau<BAs...>& n) {
	return is_terminal<BAs...>(c, n);
};

// factory method for is_terminal predicate
template <typename... BAs>
std::function<bool(const tau<BAs...>&)> is_terminal(char c) {
	return [c](const tau<BAs...>& n) {
		return is_terminal<BAs...>(c, n); };
}

template <typename... BAs>
static const auto is_capture = [](const tau<BAs...>& n) {
	return std::holds_alternative<tau_source_sym>(n->value)
		&& get<tau_source_sym>(n->value).nt()
		&& ( get<tau_source_sym>(n->value).n() == tau_parser::capture);
};

template <typename... BAs>
using is_capture_t = decltype(is_capture<BAs...>);

template <typename... BAs>
static const auto is_var_or_capture = [](const tau<BAs...>& n) {
	return std::holds_alternative<tau_source_sym>(n->value)
		&& get<tau_source_sym>(n->value).nt()
		&& ((get<tau_source_sym>(n->value).n() == tau_parser::capture)
			|| (get<tau_source_sym>(n->value).n() ==
							tau_parser::variable));
};

template <typename... BAs>
static const auto is_quantifier = [](const tau<BAs...>& n) {
	if (!std::holds_alternative<tau_source_sym>(n->value)
			|| !get<tau_source_sym>(n->value).nt()) return false;
	auto nt = get<tau_source_sym>(n->value).n();
	return nt == tau_parser::wff_all
		|| nt == tau_parser::wff_ex;
};

template <typename... BAs>
static const auto is_temporal_quantifier = [](const tau<BAs...>& n) {
	if (!std::holds_alternative<tau_source_sym>(n->value)
			|| !get<tau_source_sym>(n->value).nt()) return false;
	auto nt = get<tau_source_sym>(n->value).n();
	return nt == tau_parser::wff_sometimes
		|| nt == tau_parser::wff_always;
};

template <typename... BAs>
static const auto is_regular_or_temporal_quantifier = [](const tau<BAs...>& n) {
	return is_quantifier<BAs...>(n) || is_temporal_quantifier<BAs...>(n);
};



template <typename... BAs>
using is_var_or_capture_t = decltype(is_var_or_capture<BAs...>);

//
// functions to traverse the tree according to the specified non terminals
// and collect the corresponding nodes
//
//

// traverse the tree, depth first, according to the specified non
// terminals and return, if possible, the required non terminal node
template <typename... BAs>
std::optional<tau<BAs...>> operator|(
	const rewriter::node<tau_sym<BAs...>>& n, const size_t nt)
{
	auto v = n.child
		| std::ranges::views::filter(is_non_terminal<BAs...>(nt))
		| std::ranges::views::take(1);
	return v.empty() ? std::optional<tau<BAs...>>()
			: std::optional<tau<BAs...>>(v.front());
}

template <typename... BAs>
std::optional<tau<BAs...>> operator|(const tau<BAs...>& n, const size_t nt) {
	return *n | nt;
}

template <typename... BAs>
std::optional<tau<BAs...>> operator|(const std::optional<tau<BAs...>>& n,
	const size_t nt)
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
std::vector<tau<BAs...>> operator|(const std::vector<tau<BAs...>>& v,
	const size_t nt)
{
	// IDEA use ::to to get a vector when gcc and clang implement it in the future
	std::vector<tau<BAs...>> nv;
	for (const auto& n: v
			| std::ranges::views::transform(get_node<BAs...>(nt))
			| std::ranges::views::join)
		nv.emplace(std::move(n));
	return nv;
}

// traverse the tree, top down, and return all the nodes accessible according
// to the specified non terminals and return them
template <typename... BAs>
std::vector<tau<BAs...>> operator||(
	const rewriter::node<tau_sym<BAs...>>& n,
	const tau_parser::nonterminal nt)
{
	// IDEA use ::to to get a vector when gcc and clang implement it in the future
	std::vector<tau<BAs...>> nv;
	nv.reserve(n.child.size());
	for (const auto& c: n.child
		| std::ranges::views::filter(is_non_terminal<BAs...>(nt)))
							nv.push_back(c);
	return nv;
}

template <typename... BAs>
std::vector<tau<BAs...>> operator||(const tau<BAs...>& n,
	const tau_parser::nonterminal nt)
{
	return *n || nt;
}

template <typename... BAs>
std::vector<tau<BAs...>>  operator||(const std::optional<tau<BAs...>>& n,
	const tau_parser::nonterminal nt)
{
	// IDEA use ::to to get a vector when gcc and clang implement it in the future
	if (n) return n.value() || nt;
	return {};
}

// TODO (LOW) remove get_nodes if possible and use operator|| instead
template <typename... BAs>
std::vector<tau<BAs...>> get_nodes(const tau_parser::nonterminal nt,
	const tau<BAs...>& n)
{
	return n || nt;
}

template <size_t nt, typename... BAs>
std::vector<tau<BAs...>> get_nodes(const tau<BAs...>& n) {
	return n || nt;
}

template <typename... BAs>
auto get_nodes(const tau_parser::nonterminal nt) {
	return [nt](const tau<BAs...>& n) {
		return get_nodes<BAs...>(nt, n); };
}

template <typename... BAs>
std::vector<tau<BAs...>> operator||(const std::vector<tau<BAs...>>& v,
	const tau_parser::nonterminal nt)
{
	// IDEA use ::to to get a vector when gcc and clang implement it in the future
	std::vector<tau<BAs...>> nv; nv.reserve(v.size());
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
template <typename... BAs>
static const auto value_extractor = [](const tau<BAs...>& n)
	-> tau_sym<BAs...> { return n->value; };

template <typename... BAs>
using value_extractor_t = decltype(value_extractor<BAs...>);

template <typename... BAs>
std::vector<tau<BAs...>> operator||(const std::vector<tau<BAs...>>& v,
	const value_extractor_t<BAs...> e)
{
	std::vector<std::variant<BAs...>> nv;
	for (const auto& n : v | std::ranges::views::transform(e))
		if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<char> operator|(const std::optional<tau<BAs...>>& o,
	const value_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value()) : std::optional<char>();
}

// returns an optional containing the terminal of the node if possible
template <typename... BAs>
static const auto terminal_extractor = [](const tau<BAs...>& n)
	-> std::optional<char>
{
	auto value = n->value;
	if (!std::holds_alternative<tau_source_sym>(value)
			|| get<tau_source_sym>(value).nt()
			|| get<tau_source_sym>(value).is_null())
		return std::optional<char>();
	return std::optional<char>(get<tau_source_sym>(value).t());
};

template <typename... BAs>
using terminal_extractor_t = decltype(terminal_extractor<BAs...>);

template <typename... BAs>
std::vector<tau<BAs...>> operator||(const std::vector<tau<BAs...>>& v,
	const terminal_extractor_t<BAs...> e)
{
	std::vector<std::variant<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e))
		if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<char> operator|(const std::optional<tau<BAs...>>& o,
	const terminal_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value()) : std::optional<char>();
}

// returns an optional containing the non terminal of the node if possible
template <typename... BAs>
static const auto non_terminal_extractor = [](const tau<BAs...>& n)
	-> std::optional<size_t>
{
	if (std::holds_alternative<tau_source_sym>(n->value)
			&& get<tau_source_sym>(n->value).nt())
		return std::optional<size_t>(get<tau_source_sym>(n->value).n());
	return std::optional<size_t>();
};

template <typename... BAs>
using non_terminal_extractor_t = decltype(non_terminal_extractor<BAs...>);

template <typename... BAs>
std::vector<tau<BAs...>> operator||(const std::vector<tau<BAs...>>& v,
	const non_terminal_extractor_t<BAs...> e)
{
	return v | std::ranges::views::transform(e);
}

template <typename... BAs>
std::optional<size_t> operator|(const std::optional<tau<BAs...>>& o,
	const non_terminal_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value()) : std::optional<size_t>();
}

template <typename... BAs>
std::optional<size_t> operator|(const tau<BAs...>& o,
	const non_terminal_extractor_t<BAs...> e)
{
	return e(o);
}

// returns an optional containing size_t of the node if possible
template <typename... BAs>
static const auto size_t_extractor = [](const tau<BAs...>& n)
	-> std::optional<size_t>
{
	if (std::holds_alternative<size_t>(n->value))
		return std::optional<size_t>(std::get<size_t>(n->value));
	return std::optional<size_t>();
};
template <typename... BAs>
using size_t_extractor_t = decltype(size_t_extractor<BAs...>);

template <typename... BAs>
std::vector<tau<BAs...>> operator||(const std::vector<tau<BAs...>>& v,
	const size_t_extractor_t<BAs...> e)
{
	return v | std::ranges::views::transform(e);
}

template <typename... BAs>
std::optional<size_t> operator|(const std::optional<tau<BAs...>>& o,
	const size_t_extractor_t<BAs...> e)
{
	return o.has_value() ? e(o.value()) : std::optional<size_t>();
}

template <typename... BAs>
std::optional<size_t> operator|(const tau<BAs...>& o,
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
template <typename... BAs>
static const auto ba_extractor = [](const tau<BAs...>& n)
	-> std::optional<std::variant<BAs...>>
{
	if (std::holds_alternative<std::variant<BAs...>>(n->value))
		return std::optional<std::variant<BAs...>>(
					get<std::variant<BAs...>>(n->value));
	return std::optional<std::variant<BAs...>>();
};

template <typename... BAs>
using ba_extractor_t = decltype(ba_extractor<BAs...>);

template <typename... BAs>
std::vector<std::variant<BAs...>> operator||(const std::vector<tau<BAs...>>& v,
	const ba_extractor_t<BAs...> e)
{
	std::vector<std::variant<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e))
		if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<std::variant<BAs...>> operator|(
	const std::optional<tau<BAs...>>& o,
	const ba_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value())
				: std::optional<std::variant<BAs...>>();
}

template <typename... BAs>
std::optional<std::variant<BAs...>> operator|(const tau<BAs...>& o,
	const ba_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return e(o);
}

// returns the only child of a node
template <typename... BAs>
static const auto only_child_extractor = [](const tau<BAs...>& n)
	-> std::optional<tau<BAs...>>
{
	if (n->child.size() != 1) return std::optional<tau<BAs...>>();
	return std::optional<tau<BAs...>>(n->child[0]);
};

template <typename... BAs>
using only_child_extractor_t = decltype(only_child_extractor<BAs...>);

template <typename... BAs>
std::vector<tau<BAs...>> operator||(const std::vector<tau<BAs...>>& v,
	const only_child_extractor_t<BAs...> e)
{
	std::vector<tau<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e))
		if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<tau<BAs...>> operator|(const std::optional<tau<BAs...>>& o,
	const only_child_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value())
		: std::optional<tau<BAs...>>();
}

// IDEA maybe unify all the implementations dealing with operator| and operator|| for extractors
template <typename... BAs>
std::optional<tau<BAs...>> operator|(const tau<BAs...>& o,
	const only_child_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return e(o);
}

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

// converts a tau<...> to a string.
template <typename extractor_t, typename node_t>
std::string make_string(const extractor_t& extractor, const node_t& n) {
	std::basic_stringstream<char> ss;
	stringify<extractor_t, node_t> sy(extractor, ss);
	rewriter::post_order_tree_traverser<stringify<extractor_t, node_t>,
				rewriter::all_t, node_t>(sy, rewriter::all)(n);
	return ss.str();
}

// extracts terminal from tau node
template <typename... BAs>
auto tau_node_terminal_extractor = [](const tau<BAs...>& n)
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

template <typename... BAs>
auto extract_string = [](const tau<BAs...>& n) {
	return idni::tau_lang::make_string(
		idni::tau_lang::tau_node_terminal_extractor<BAs...>, n);
};

template <typename... BAs>
using extract_string_t = decltype(extract_string<BAs...>);

// Simple helper method to convert a tau tree to string
template <typename... BAs>
std::string tau_to_str (const tau<BAs...>& n) {
	std::stringstream ss;
	ss << n;
	return ss.str();
}

template <typename... BAs>
std::string operator|(const tau<BAs...>& n, const extract_string_t<BAs...> e) {
	return e(n);
}

template <typename... BAs>
std::string operator|(const std::optional<tau<BAs...>>& n,
	const extract_string_t<BAs...> e)
{
	return n.has_value() ? e(n.value()) : "";
}

// check if the node is the given non terminal
template <typename... BAs>
bool is_child_non_terminal(const size_t nt, const tau<BAs...>& n) {
	auto child = n | only_child_extractor<BAs...>;
	return child.has_value() && is_non_terminal<BAs...>(nt, child.value());
}

// check if the node is the given non terminal (template approach)
template <size_t nt, typename...BAs>
bool is_child_non_terminal(const tau<BAs...>& n) {
	return is_child_non_terminal<BAs...>(nt, n);
}

// factory method for is_non_terminal predicate
template <typename... BAs>
std::function<bool(const tau<BAs...>&)> is_child_non_terminal(const size_t nt) {
	return [nt](const tau<BAs...>& n) {
		return is_child_non_terminal<BAs...>(nt, n); };
}

// returns the first child of a node
template <typename... BAs>
static const auto first_child_extractor = [](const tau<BAs...>& n)
	-> std::optional<tau<BAs...>>
{
	if (n->child.size() == 0) return std::optional<tau<BAs...>>();
	return std::optional<tau<BAs...>>(n->child[0]);
};

template <typename... BAs>
using first_child_extractor_t = decltype(first_child_extractor<BAs...>);

template <typename... BAs>
std::vector<tau<BAs...>> operator||(
	const std::vector<tau<BAs...>>& v,
	const first_child_extractor_t<BAs...> e)
{
	std::vector<tau<BAs...>> nv;
	for (const auto& n: v | std::ranges::views::transform(e))
		if (n.has_value()) nv.push_back(n.value());
	return nv;
}

template <typename... BAs>
std::optional<tau<BAs...>> operator|(const std::optional<tau<BAs...>>& o,
	const first_child_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return o.has_value() ? e(o.value())
		: std::optional<tau<BAs...>>();
}

// IDEA maybe unify all the implementations dealing with operator| and operator|| for extractors
template <typename... BAs>
std::optional<tau<BAs...>> operator|(const tau<BAs...>& o,
	const first_child_extractor_t<BAs...> e)
{
	// IDEA use o.transform(e) from C++23 when implemented in the future by gcc/clang
	return e(o);
}


template <typename T>
static const auto optional_value_extractor = [](const std::optional<T>& o) -> T {
	if (!o) BOOST_LOG_TRIVIAL(error)
		<< "(Error) parse tree traversal: bad optional access";
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
auto get_optional(const extractor_t& extractor, const tau<BAs...>& n) {
	return get_node<nt..., BAs...>(n).and_then(extractor);
}

// returns the extracted components/informations of the specified nodes, the
// component/information is extracted using the given extractor.
template <size_t... nt, typename extractor_t, typename... BAs>
auto get_optionals(const extractor_t& extractor, const tau<BAs...>& n) {
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
		// TODO this is simple retype of bf_and_nosep_1st_op to bf
		// this is temporary until parser features retyping
		if (n.nt() && n.n() == tau_parser::bf_and_nosep_1st_op)
			return tau_sym<BAs...>(tau_parser::instance()
						.literal(tau_parser::bf));
		return tau_sym<BAs...>(n);
	}
};

// extracts terminal from sp_tau_source_node
extern std::function<std::optional<char>(const sp_tau_source_node& n)>
	tau_source_terminal_extractor;

template <typename... BAs>
using tau_source_terminal_extractor_t = decltype(tau_source_terminal_extractor);

template <typename... BAs>
tau<BAs...> trim(const tau<BAs...>& n) {
	return n->child[0];
}

template <typename... BAs>
tau<BAs...> trim2(const tau<BAs...>& n) {
	return n->child[0]->child[0];
}

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const std::vector<tau<BAs...>>& nn)
{
	return make_node<tau_sym<BAs...>>(
		tau_parser::instance().literal(nt), nn);
}

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt,
	const std::initializer_list<tau<BAs...>> ch)
{
	return wrap(nt, std::vector<tau<BAs...>>(ch));
}

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const tau<BAs...>& n) {
	return wrap(nt, { n });
}

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const tau<BAs...>& c1,
	const tau<BAs...>& c2)
{
	return wrap(nt, { c1, c2 });
}

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const std::string& terminals) {
	std::vector<tau<BAs...>> children;
	for (const auto& c : terminals)
		children.emplace_back(rewriter::make_node<tau_sym<BAs...>>(
			tau_source_sym(c), {}));
	return wrap(nt, children);
}

// bind the given, using a binder, the constants of the a given tau<...>.
template<typename binder_t, typename... BAs>
struct bind_transformer {

	bind_transformer(binder_t& binder) : binder(binder) {}

	tau<BAs...> operator()(const tau<BAs...>& n) {
		if (auto it = changes.find(n); it != changes.end())
			return it->second;
		if (is_non_terminal<tau_parser::bf_constant, BAs...>(n))
			if (auto nb = binder.bind(n); nb != n)
				return changes[n] = nb;
		// IDEA maybe we could use the replace transform instead of having the following code
		bool changed = false;
		std::vector<tau<BAs...>> child;
		for (auto& c : n->child)
			if (changes.contains(c))
				changed = true, child.push_back(changes[c]);
			else child.push_back(c);
		auto nn = make_node<tau_sym<BAs...>>(n->value, child);
		if (changed) changes[n] = nn;
		return nn;
	}

	std::map<tau<BAs...>, tau<BAs...>> changes;
	binder_t binder;
};

// is not a whitespace predicate
template <typename... BAs>
static const auto not_whitespace_predicate = [](const tau<BAs...>& n) {
	return n->value.index() != 0
		|| !get<0>(n->value).nt()
		|| (get<0>(n->value).n() != tau_parser::_ &&
			get<0>(n->value).n() != tau_parser::__);
};

template <typename... BAs>
using not_whitespace_predicate_t = decltype(not_whitespace_predicate<BAs...>);

// binds the constants of a given binding using the label specified
// in the code and according to a map from labels to constants in the BAs...
template <typename... BAs>
struct name_binder {

	name_binder(const bindings<BAs...>& bs) : bs(bs) {}

	tau<BAs...> bind(const tau<BAs...>& n) const {
		auto binding = n | tau_parser::constant | tau_parser::binding;
		if (!binding || (n | tau_parser::type).has_value()) return n;
		auto bn = make_string<
				tau_node_terminal_extractor_t<BAs...>,
				tau<BAs...>>(
			tau_node_terminal_extractor<BAs...>, binding.value());
		auto s = bs.find(bn);
		if (s != bs.end()) {
			tau_sym<BAs...> ts = s->second;
			return wrap(tau_parser::bf_constant,
				wrap(tau_parser::constant,
					rewriter::make_node<tau_sym<BAs...>>(
								ts, {})));
		}
		return n;
	}

	const bindings<BAs...>& bs;
};

template <typename...BAs>
struct nso_factory {

	std::optional<tau<BAs...>> parse(const std::string&,
			const std::string& = "") const {
		throw std::runtime_error("not implemented");
	}

	tau<BAs...> binding(const tau<BAs...>&,
			const std::string& = "") const {
		throw std::runtime_error("not implemented");
	}

	std::vector<std::string> types() const {
		throw std::runtime_error("not implemented");
	}

	std::string default_type() const {
		throw std::runtime_error("not implemented");
	}

	std::string one(const std::string& type_name) const {
		throw std::runtime_error("not implemented");
	}

	std::string zero(const std::string& type_name) const {
		throw std::runtime_error("not implemented");
	}

	tau<BAs...> splitter_one(const std::string& = "") const {
		throw std::runtime_error("not implemented");
	}

	static nso_factory<BAs...>& instance() {
		throw std::runtime_error("not implemented");
	}
};

// binds the constants of a given binding using the multi-factory for the types
// supported.
template <typename... BAs>
struct factory_binder {

	tau<BAs...> bind(const tau<BAs...>& n) const {
		auto binding = n | tau_parser::constant | tau_parser::binding;
		if (!binding) return n; // not a binding (capture?)
		auto type = find_top(n, is_non_terminal<tau_parser::type, BAs...>);
		if (type)
		{
			// the factory take two arguments, the first is the type and the
			// second is the node representing the constant.
			std::string type_name = make_string<
				tau_node_terminal_extractor_t<BAs...>,
				tau<BAs...>>(
					tau_node_terminal_extractor<BAs...>,
					type.value());
			auto nn = nso_factory<BAs...>::instance().binding(binding.value(), type_name);
			if (!nn) return nullptr;
			if (nn != binding.value())
				return wrap(tau_parser::bf_constant,
					wrap(tau_parser::constant, nn), type.value());
			return n;
		}
		auto nn = nso_factory<BAs...>::instance().binding(binding.value(), "");
		if (!nn) return nullptr;
		if (nn != binding.value())
			return wrap(tau_parser::bf_constant,
				wrap(tau_parser::constant, nn));
		return n;
	}
};

// creates a specific rule from a generic rule
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rewriter::rule<tau<BAs...>> make_rule(tau_parser::nonterminal rule_t,
	tau_parser::nonterminal matcher_t, tau_parser::nonterminal body_t,
	const tau<BAs...>& rule)
{
	auto matcher = rule | rule_t | matcher_t | only_child_extractor<BAs...>
		| optional_value_extractor<tau<BAs...>>;
	auto body = rule | rule_t | body_t | only_child_extractor<BAs...>
		| optional_value_extractor<tau<BAs...>>;
	return { matcher, body };
}

// creates a specific rule from a generic rule
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rewriter::rule<tau<BAs...>> make_rule(const tau<BAs...>& rule) {
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
template <typename... BAs>
rules<tau<BAs...>> make_rules(tau<BAs...>& tau_source) {
	rules<tau<BAs...>> rs;
	// TODO (LOW) change call to select by operator|| and operator|
	for (auto& r: select_top(tau_source,
		is_non_terminal<tau_parser::rule, BAs...>))
			rs.push_back(make_rule<BAs...>(r));
	return rs;
}

// create a set of relations from a given tau source.
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rec_relations<tau<BAs...>> make_rec_relations(
	const tau<BAs...>& tau_source)
{
	rec_relations<tau<BAs...>> rs;
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

template <typename...BAs>
tau<BAs...> process_digits(const tau<BAs...>& tau_source) {
	std::map<tau<BAs...>, tau<BAs...>> changes;
	for(auto& n: select_top(tau_source,
		is_non_terminal<tau_parser::digits, BAs...>))
	{
		auto offset = make_string<
				tau_node_terminal_extractor_t<BAs...>,
				tau<BAs...>>(
			tau_node_terminal_extractor<BAs...>,  n);
		auto num = std::stoul(offset);
		auto nn = rewriter::make_node<tau_sym<BAs...>>(tau_sym<BAs...>(num), {});
		changes[n] = nn;
	}
	return replace<tau<BAs...>>(tau_source, changes);
}

// } // include for ptree<BAs...>()
// #include "debug_helpers.h"
// namespace idni::tau_lang {

template <typename... BAs>
struct quantifier_vars_transformer {
	using p = tau_parser;
	using node = tau<BAs...>;
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
		std::vector<tau<BAs...>> child;
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

template <typename...BAs>
tau<BAs...> process_quantifier_vars(const tau<BAs...>& tau_code) {
	using node = tau<BAs...>;
	quantifier_vars_transformer<BAs...> transformer;
	return rewriter::post_order_traverser<quantifier_vars_transformer<BAs...>,
		rewriter::all_t, node>(transformer, rewriter::all)(tau_code);
}

template <typename...BAs>
tau<BAs...> process_offset_variables(const tau<BAs...>& tau_code) {
	using p = tau_parser;
	using node = tau<BAs...>;
	std::map<node, node> changes;
	for (const auto& offsets :
		select_all(tau_code, is_non_terminal<p::offsets, BAs...>))
	{
		for (const auto& var : select_all(offsets,
			is_non_terminal<p::variable, BAs...>))
				changes[var] = wrap(p::capture, var->child);
	}
	if (changes.size()) return replace(tau_code, changes);
	return tau_code;
}

template <typename...BAs>
tau<BAs...> process_defs_input_variables(const tau<BAs...>& tau_code) {
	using p = tau_parser;
	using node = tau<BAs...>;
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

template <typename... BAs>
tau<BAs...> infer_constant_types(const tau<BAs...>& code) {
	BOOST_LOG_TRIVIAL(trace)
		<< "(T) infer constant types: " << code;
	using node = tau<BAs...>;
	std::map<node, node> changes;
	auto extract_type = [](const node& n) {
		return make_string<tau_node_terminal_extractor_t<BAs...>,
			node>(tau_node_terminal_extractor<BAs...>, n);
	};
	auto type_mismatch = [&](const std::string& expected,
		const std::string& got)
	{
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Type mismatch. Constant is expected to be "
			<< expected << " but is " << got;
		return false;
	};
	auto unsupported_type = [&](const std::string& type)
	{
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Unsupported type: " << type << "\n";
		return false;
	};
	auto infer_over_scope = [&](const node& nd) {
		std::string type = "";
		BOOST_LOG_TRIVIAL(trace)
			<< "(T) infer over scope: " << nd;
		// get first appearing type information in an expression
		auto bfcs = select_all(nd,
			is_non_terminal<tau_parser::bf_constant, BAs...>);
		if (bfcs.size() == 0) return true;
		for (auto& bfc : bfcs) {
			auto type_opt = bfc | tau_parser::type;
			if (type_opt) {
				type = extract_type(type_opt.value());
				break;
			}
		}
		BOOST_LOG_TRIVIAL(trace)
			<< "(T) first appearing constant type: " << type;
		if (type.size() == 0) type = nso_factory<BAs...>::instance().default_type(); // default type
		// pass the type to all constants and check for mismatch
		for (auto& c : select_all(nd, is_non_terminal<
				tau_parser::bf_constant, BAs...>))
		{
			auto type_nd = c | tau_parser::type;
			if (type_nd) {
				auto got = extract_type(type_nd.value());
				bool found = false;
				for (auto t: nso_factory<BAs...>::instance().types())
					if (got == t) { found = true; break; }
				if (!found) return unsupported_type(got);
				if (!(type.size() == 0 && got == nso_factory<BAs...>::instance().default_type())
					&& got != type)
				{
					BOOST_LOG_TRIVIAL(trace)
						<< "(T) type mismatch: " << type << " got: " << got << " for: " << c;
					return type_mismatch(got,type);
				}
			} else changes.emplace(c, rewriter::make_node<
				tau_sym<BAs...>>(c->value, {c->child[0],
					wrap<BAs...>(tau_parser::type,
							type)}));
		}
		return true;
	};
	static auto is_constant_type_scope_node = [](const node& n) {
		static std::vector<size_t> scope_nodes{
			tau_parser::bf_interval,
			tau_parser::bf_neq,
			tau_parser::bf_eq,
			tau_parser::bf_nleq,
			tau_parser::bf_greater,
			tau_parser::bf_less_equal,
			tau_parser::bf_less,
			tau_parser::bf
		};
		if (!is_non_terminal_node<BAs...>(n)) return false;
		auto nt = n | non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>;
		return find(scope_nodes.begin(), scope_nodes.end(), nt)
							!= scope_nodes.end();
	};
	for (auto& nd : select_top(code, is_constant_type_scope_node))
		if (!infer_over_scope(nd)) return 0;
	return replace<node>(code, changes);
}

// TODO (LOW) refactor and clean this structure
template <typename... BAs>
struct free_vars_collector {

	free_vars_collector(std::set<tau<BAs...>>& free_vars) : free_vars(free_vars) {}

	tau<BAs...> operator()(const tau<BAs...>& n) {
		if (is_quantifier<BAs...>(n)) {
			// IDEA using quantified_variable => variable | capture would simplify the code
			auto var = find_top(n, is_var_or_capture<BAs...>);
			if (var.has_value()) {
				if (auto it = free_vars.find(var.value()); it != free_vars.end()) {
					free_vars.erase(it);
					BOOST_LOG_TRIVIAL(trace) << "(I) -- removing quantified var: " << var.value();
				}
			}
		}
		if (is_var_or_capture<BAs...>(n)) {
			if (auto check = n
					| tau_parser::io_var | only_child_extractor<BAs...> | tau_parser::offset
					| only_child_extractor<BAs...>;
					check.has_value() && is_var_or_capture<BAs...>(check.value())) {
				auto var = check.value();
				if (auto it = free_vars.find(var); it != free_vars.end()) {
					free_vars.erase(it);
					BOOST_LOG_TRIVIAL(trace) << "(I) -- removing var: " << var;
				}
			}
			free_vars.insert(n);
			BOOST_LOG_TRIVIAL(trace) << "(I) -- inserting var: " << n;
		}
		return n;
	}

	std::set<tau<BAs...>>& free_vars;
};

template <typename... BAs>
auto get_free_vars_from_nso(const tau<BAs...>& n) {
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin get_free_vars_from_nso of " << n;
	std::set<tau<BAs...>> free_vars;
	free_vars_collector<BAs...> collector(free_vars);
	rewriter::post_order_traverser<
			free_vars_collector<BAs...>,
			rewriter::all_t,
			tau<BAs...>>(collector, rewriter::all)(n);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- End get_free_vars_from_nso";
	return free_vars;
}

// TODO (MEDIUM) unify this code with get_tau_nso_clause and get_tau_nso_literals
template<typename ...BAs>
void get_leaves(const tau<BAs...>& n, tau_parser::nonterminal branch,
	tau_parser::nonterminal skip, std::vector<tau<BAs...>>& leaves)
{
    if (is_child_non_terminal(branch, n)) {
	    for (const auto& c : trim(n)->child)
	    	if (is_non_terminal(skip, c))
	    		get_leaves(c, branch, skip, leaves);
    } else {
        leaves.push_back(n);
        BOOST_LOG_TRIVIAL(trace) << "(I) get_leaves: found clause: " << n;
    }
}

template<typename ...BAs>
std::vector<tau<BAs...>> get_leaves(const tau<BAs...>& n,
	tau_parser::nonterminal branch, tau_parser::nonterminal skip)
{
	std::vector<tau<BAs...>> leaves;
	get_leaves(n, branch, skip, leaves);
	return leaves;
}

template<typename ...BAs>
std::vector<tau<BAs...>> get_dnf_wff_clauses(const tau<BAs...>& n) {
	return get_leaves(n, tau_parser::wff_or, tau_parser::wff);
}

template<typename ...BAs>
std::vector<tau<BAs...>> get_dnf_bf_clauses(const tau<BAs...>& n) {
	return get_leaves(n, tau_parser::bf_or, tau_parser::bf);
}

template<typename ...BAs>
std::vector<tau<BAs...>> get_cnf_wff_clauses(const tau<BAs...>& n) {
	return get_leaves(n, tau_parser::wff_and, tau_parser::wff);
}

template<typename ...BAs>
std::vector<tau<BAs...>> get_cnf_bf_clauses(const tau<BAs...>& n) {
	return get_leaves(n, tau_parser::bf_and, tau_parser::bf);
}

// A formula has a temporal variable if either it contains an io_var with a variable or capture
// or it contains a flag
template <typename... BAs>
bool has_temp_var (const tau<BAs...>& fm) {
	auto io_vars = select_top(fm, is_non_terminal<tau_parser::io_var, BAs...>);
	if (io_vars.empty()) return find_top(fm, is_non_terminal<tau_parser::constraint, BAs...>).has_value();
	// any input/output stream is a temporal variable, also constant positions
	else return true;
}

// Check that no non-temporal quantified variable appears nested in the scope of
// temporal quantification
template <typename... BAs>
bool invalid_nesting_of_quants (const tau<BAs...>& fm) {
	auto non_temp_quants = select_all(fm, is_quantifier<BAs...>);
	for (const auto& ntq : non_temp_quants) {
		auto ntq_var = trim(ntq);
		auto temp_quants = select_all(ntq, is_temporal_quantifier<BAs...>);
		for (const auto& tq : temp_quants) {
			// Check that the non-temp quantified variable doesn't appear free
			if (get_free_vars_from_nso(tq).contains(ntq_var)) {
				BOOST_LOG_TRIVIAL(error) << "(Error) Non-temporal quantifier cannot capture variable appearing in scoped temporal subformula: \n"
				<< "Variable \"" << ntq_var << "\" is captured in \"" << tq << "\"";
				return true;
			}
		}
	}
	return false;
}

template <typename... BAs>
bool invalid_nesting_of_temp_quants (const tau<BAs...>& fm) {
	auto temp_statements = select_top(fm, is_temporal_quantifier<BAs...>);
	// Check that in no temp_statement another temporal statement is found
	for (const auto& temp_st : temp_statements) {
		if(auto n = find_top(trim(temp_st), is_temporal_quantifier<BAs...>); n.has_value()) {
			BOOST_LOG_TRIVIAL(error) << "(Error) Nesting of temporal quantifiers is currently not allowed: \n"
			<< "Found \"" << n.value() << "\" in \"" << temp_st << "\"";
			return true;
		}
	}
	return false;
}

template <typename... BAs>
bool has_open_tau_fm_in_constant (const tau<BAs...>& fm) {
	auto _closed = [](const auto& n) -> bool {
		return is_closed(n);
	};
	auto consts = select_top(fm, is_child_non_terminal<tau_parser::bf_constant, BAs...>);
	for (const auto& c : consts) {
		auto ba_const = c
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		if (!std::visit(_closed, ba_const)) {
			BOOST_LOG_TRIVIAL(error) << "(Error) A Tau formula constant must be closed: " << ba_const;
			return true;
		}
	}
	return false;
}

// This function is used to check for semantic errors in formulas since those
// cannot be captured by the grammar
template <typename... BAs>
bool has_semantic_error (const tau<BAs...>& fm) {
	bool error = invalid_nesting_of_quants(fm)
			|| has_open_tau_fm_in_constant(fm)
			|| invalid_nesting_of_temp_quants(fm);
	return error;
}

// create tau code from tau source
template <typename... BAs>
// TODO (LOW) should depend on node_t instead of BAs...
tau<BAs...> make_tau_code(sp_tau_source_node& tau_source) {
	if (!tau_source) return 0;
	tauify<BAs...> tf;
	rewriter::map_transformer<tauify<BAs...>,
		sp_tau_source_node, tau<BAs...>> transform(tf);
	auto tau_code = rewriter::post_order_traverser<
				rewriter::map_transformer<tauify<BAs...>,
				sp_tau_source_node, tau<BAs...>>,
			rewriter::all_t,
			rewriter::sp_node<tau_source_sym>,
			tau<BAs...>>(
		transform, rewriter::all)(tau_source);
	if (!tau_code) return nullptr;
	return infer_constant_types(
		process_defs_input_variables(
		process_offset_variables(
		process_quantifier_vars(
		process_digits(tau_code)))));
}

// make a library from the given tau source.
// TODO (LOW) should depend on node_t instead of BAs...
template <typename... BAs>
library<tau<BAs...>> make_library(sp_tau_source_node& tau_source) {
	auto lib = make_tau_code<BAs...>(tau_source);
	return make_rules(lib);
}

// make a library from the given tau source string.
// TODO (LOW) should depend on node_t instead of BAs...
template <typename... BAs>
library<tau<BAs...>> make_library(const std::string& source) {
	auto tau_source = make_tau_source(source, {
						.start = tau_parser::library });
	return make_library<BAs...>(tau_source);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
tau<BAs...> bind_tau_code_using_binder(const tau<BAs...>& code,
	binder_t& binder)
{
	bind_transformer<binder_t, BAs...> bs(binder);
	auto res = rewriter::post_order_traverser<
			bind_transformer<binder_t, BAs...>,
			rewriter::all_t,
			tau<BAs...>>(bs, rewriter::all)(code);
	// Check for errors which cannot be captured by the grammar
	if (res && has_semantic_error(res)) return {};
	else return res;
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
tau<BAs...> bind_tau_code_using_bindings(tau<BAs...>& code,
	const bindings<BAs...>& bindings)
{
	name_binder<BAs...> nb(bindings);
	return bind_tau_code_using_binder<
			name_binder<BAs...>, BAs...>(code, nb);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
tau<BAs...> bind_tau_code_using_factory(const tau<BAs...>& code)
{
	factory_binder<BAs...> fb;
	return bind_tau_code_using_binder<factory_binder<BAs...>, BAs...>(code, fb);
}

// make a nso_rr from the given tau code
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_from_binded_code(
	const tau<BAs...>& code)
{
	if (is_non_terminal(tau_parser::bf, code)
		|| is_non_terminal(tau_parser::ref, code))
			return { { {}, code } };

	if (is_non_terminal(tau_parser::rec_relations, code))
		return { { make_rec_relations<BAs...>(code), {} } };

	auto main = (is_non_terminal(tau_parser::tau_constant_source, code)
		|| is_non_terminal(tau_parser::rr, code)
			? code | tau_parser::main
			: code | tau_parser::rr | tau_parser::main)
				| tau_parser::wff
				| optional_value_extractor<tau<BAs...>>;
	auto rules = make_rec_relations<BAs...>(code);
	return infer_ref_types<BAs...>(rr<tau<BAs...>>{ rules, main });
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_binder(const tau<BAs...>& code,
	binder_t& binder)
{
	auto binded = bind_tau_code_using_binder<binder_t, BAs...>(code, binder);
	if (!binded) return {};
	return make_nso_rr_from_binded_code<BAs...>(binded);
}

template<typename binder_t, typename... BAs>
tau<BAs...> make_nso_using_binder(const tau<BAs...>& code,
	binder_t& binder)
{
	return bind_tau_code_using_binder<binder_t, BAs...>(code, binder);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_binder(
	sp_tau_source_node& source, binder_t& binder)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return {};
	return make_nso_rr_using_binder<binder_t, BAs...>(code, binder);
}

template<typename binder_t, typename... BAs>
tau<BAs...> make_nso_using_binder(sp_tau_source_node& source,
	binder_t& binder)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return nullptr;
	return make_nso_using_binder<binder_t, BAs...>(code, binder);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_binder(std::string& input,
	binder_t& binder)
{
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	return !source ? std::optional<rr<tau<BAs...>>>{}
		: std::optional<rr<tau<BAs...>>>{
			make_nso_rr_using_binder<binder_t, BAs...>(
				source, binder) };
}

template<typename binder_t, typename... BAs>
std::optional<tau<BAs...>> make_nso_using_binder(std::string& input,
	binder_t& binder,
	idni::parser<>::parse_options options = { .start = tau_parser::wff })
{
	auto source = make_tau_source(input, options);
	return !source ? std::optional<tau<BAs...>>{}
		: std::optional<tau<BAs...>>{
			make_nso_using_binder<binder_t, BAs...>(
				source, binder) };
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(
	const tau<BAs...>& code, const bindings<BAs...>& bindings)
{
	name_binder<BAs...> nb(bindings);
	return make_nso_rr_using_binder<name_binder<BAs...>, BAs...>(code, nb);
}

template <typename... BAs>
tau<BAs...> make_nso_using_bindings(const tau<BAs...>& code,
	const bindings<BAs...>& bindings)
{
	name_binder<BAs...> nb(bindings);
	return make_nso_using_binder<name_binder<BAs...>, BAs...>(code, nb);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(
	sp_tau_source_node& source, const bindings<BAs...>& bindings)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return {};
	return make_nso_rr_using_bindings<BAs...>(code, bindings);
}

template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_bindings(sp_tau_source_node& source,
	const bindings<BAs...>& bindings)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return {};
	return make_nso_using_bindings<BAs...>(code, bindings);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(
	const std::string& input, const bindings<BAs...>& bindings)
{
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	if (!source) return std::optional<rr<tau<BAs...>>>{};
	return make_nso_rr_using_bindings<BAs...>(source, bindings);
}
// make a nso from the given tau source and bindings.
template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_bindings(
	const std::string& input, const bindings<BAs...>& bindings,
	idni::parser<>::parse_options options =  { .start = tau_parser::wff })
{
	auto source = make_tau_source(input, options);
	if (!source) return std::optional<tau<BAs...>>{};
	return make_nso_using_bindings<BAs...>(source, bindings);
}

template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(
	const tau<BAs...>& code)
{
	factory_binder<BAs...> fb;
	return make_nso_rr_using_binder<
			factory_binder<BAs...>, BAs...>(code,fb);
}

template <typename... BAs>
tau<BAs...> make_nso_using_factory(const tau<BAs...>& code) {
	factory_binder<BAs...> fb;
	return make_nso_using_binder<
			factory_binder<BAs...>, BAs...>(code,fb);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(
	sp_tau_source_node& source)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return std::optional<rr<tau<BAs...>>>{};
	return make_nso_rr_using_factory<BAs...>(code);
}

template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_factory(
	sp_tau_source_node& source)
{
	auto code = make_tau_code<BAs...>(source);
	if (!code) return std::optional<tau<BAs...>>{};
	return make_nso_using_factory<BAs...>(code);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(
	const std::string& input)
{
	auto source = make_tau_source(input, { .start = tau_parser::rr });
	if (!source) return std::optional<rr<tau<BAs...>>>{};
	return make_nso_rr_using_factory<BAs...>(source);
}

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_factory(
	const std::string& input,
	idni::parser<>::parse_options options =  { .start = tau_parser::wff })
{
	auto source = make_tau_source(input, options);
	if (!source) return std::optional<tau<BAs...>>{};
	return make_nso_using_factory<BAs...>(source);
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

template <typename... BAs>
std::string get_ref_name(const tau<BAs...>& n) {
	auto ref = n;
	if (auto ref_as_child = n | tau_parser::ref; ref_as_child)
		ref = ref_as_child.value();
	return make_string(tau_node_terminal_extractor<BAs...>,
		(ref | tau_parser::sym).value());
};

template <typename... BAs>
std::pair<std::string, std::pair<size_t, size_t>> get_ref_name_and_arity(
	const tau<BAs...>& n)
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

template <typename... BAs>
std::string get_ref_type(bool& success, rr_types& ts,
	const tau<BAs...>& ref, const tau_parser::nonterminal& t,
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
		if (rt.fp && arity.first) {
			// found a real offset arity for a uncomplete type taken
			// from main with a fp call => update offset arity
			ts.types[fn].offset_arity = arity.first;
			ts.types[fn].fp = false;
			BOOST_LOG_TRIVIAL(trace) << "(I) -- Updated type of "
				<< fn << "() : " << rr_type2str(ts.types[fn]);
		}
		if (!rt.fp && !new_type.fp && rt.offset_arity != arity.first)
			err << "Offset arity mismatch. ";
		if (err.tellp()) return
			err << fn << "() : " << rr_type2str(new_type)
				<< " declared as " << rr_type2str(rt),
			ts.errors.insert(err.str()),
			success = false, "";
	} else {
		ts.types[fn] = new_type;
		BOOST_LOG_TRIVIAL(trace) << "(I) -- Found type of " << fn
					<< "() : " << rr_type2str(ts.types[fn]);
		return fn;
	}
	return "";
};

template <typename... BAs>
std::pair<std::set<std::string>, std::set<std::string>> get_rr_types(
	bool& success, rr_types& ts, const tau<BAs...>& n,
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

template <typename... BAs>
std::pair<std::set<std::string>, std::set<std::string>> get_rr_types(
	bool& success, rr_types& ts, const rr<tau<BAs...>>& nso_rr)
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

template <typename... BAs>
std::optional<rr<tau<BAs...>>> infer_ref_types(const rr<tau<BAs...>>& nso_rr) {
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin type inferrence"; // << ": " << nso_rr;
	// for (auto& r : nso_rr.rec_relations)
	// 	ptree<BAs...>(std::cout << "rule left: ", r.first) << "\n",
	// 	ptree<BAs...>(std::cout << "rule right: ", r.second) << "\n";
	// ptree<BAs...>(std::cout << "main: ", nso_rr.main) << "\n";
	rr<tau<BAs...>> nn = nso_rr;
	rr_types ts;
	std::set<std::string> done_names, todo_names;
	bool success = true;
	static auto get_nt_type = [](const tau<BAs...>& r) {
		size_t n = r | non_terminal_extractor<BAs...>
			| optional_value_extractor<size_t>;
		return static_cast<tau_parser::nonterminal>(n);
	};
	static auto update_ref = [](tau<BAs...>& r,
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

	for (const auto& err : ts.errors)
		BOOST_LOG_TRIVIAL(error) << "(Error) " << err;
	if (ts.errors.size()) return {};

	if (todo_names.size()) {
		std::stringstream ss;
		for (auto& fn : todo_names) ss << " " << fn;
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Unknown type for:" << ss.str();
		return {};
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End type inferrence"; // << ": " << nn;
	return { nn };
}


//------------------------------------------------------------------------------

// creates a specific builder from a tau.
template <typename... BAs>
builder<BAs...> make_builder(const tau<BAs...>& builder) {
	auto head = builder | tau_parser::builder_head
		| optional_value_extractor<tau<BAs...>>;
	auto type_node = builder
		| tau_parser::builder_body | only_child_extractor<BAs...>;
	auto type = type_node | non_terminal_extractor<BAs...>
		| optional_value_extractor<size_t>;
	switch (type) {
	case tau_parser::bf_builder_body:  return { head, type_node
		| tau_parser::bf
		| optional_value_extractor<tau<BAs...>>};
	case tau_parser::wff_builder_body: return { head, type_node
		| tau_parser::wff
		| optional_value_extractor<tau<BAs...>>};
	default: throw std::runtime_error("unknown builder type");
	};
}

// create a builder from a given tau source.
template <typename... BAs>
builder<BAs...> make_builder(sp_tau_source_node& tau_source) {
	auto builder = make_tau_code<BAs...>(tau_source);
	return make_builder(builder);
}

// make a builder from the given tau source string.
template <typename... BAs>
builder<BAs...> make_builder(const std::string& source) {
	auto tau_source = make_tau_source(source, {
						.start = tau_parser::builder });
	return make_builder<BAs...>(tau_source);
}

template <typename... BAs>
tau<BAs...> tau_apply_builder(const builder<BAs...>& b,
	std::vector<tau<BAs...>>& n)
{
	std::map<tau<BAs...>, tau<BAs...>> changes;
	std::vector<tau<BAs...>> vars = b.first || tau_parser::capture;
	for (size_t i = 0; i < vars.size(); ++i) changes[vars[i]] = n[i];
	return replace<tau<BAs...>>(b.second, changes);
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
template <typename... BAs>
static auto bldr_bf_0 = make_builder<BAs...>(BLDR_BF_0);
template <typename... BAs>
static auto bldr_bf_1 = make_builder<BAs...>(BLDR_BF_1);
template <typename... BAs>
static auto bldr_wff_F = make_builder<BAs...>(BLDR_WFF_F);
template <typename... BAs>
static auto bldr_wff_T = make_builder<BAs...>(BLDR_WFF_T);

// wff builder
template <typename... BAs>
static auto bldr_wff_eq = make_builder<BAs...>(BLDR_WFF_EQ);
template <typename... BAs>
static auto bldr_bf_splitter = make_builder<BAs...>(BLDR_BF_SPLITTER);
template <typename... BAs>
static auto bldr_bf_not_less_equal =
				make_builder<BAs...>(BLDR_BF_NOT_LESS_EQUAL);
template <typename... BAs>
static auto bldr_bf_interval = make_builder<BAs...>(BDLR_BF_INTERVAL);
template <typename... BAs>
static auto bldr_bf_nleq_upper = make_builder<BAs...>(BDLR_BF_NLEQ_UPPER);
template <typename... BAs>
static auto bldr_bf_nleq_lowwer = make_builder<BAs...>(BDLR_BF_NLEQ_LOWWER);

// basic bf and wff constants
template <typename... BAs>
static const tau<BAs...> _0 = bldr_bf_0<BAs...>.second;

template <typename... BAs>
static const tau<BAs...> _0_trimmed = trim(_0<BAs...>);

template <typename... BAs>
static const tau<BAs...> _1 = bldr_bf_1<BAs...>.second;

template <typename... BAs>
static const tau<BAs...> _1_trimmed = trim(_1<BAs...>);

template <typename... BAs>
static const tau<BAs...> _F = bldr_wff_F<BAs...>.second;

template <typename... BAs>
static const tau<BAs...> _F_trimmed = trim(_F<BAs...>);

template <typename... BAs>
static const tau<BAs...> _T = bldr_wff_T<BAs...>.second;

template <typename... BAs>
static const tau<BAs...> _T_trimmed = trim(_T<BAs...>);

template <typename... BAs>
tau<BAs...> build_extra (const tau<BAs...> n, const std::string &note) {
	assert((!n->child.empty()) && (!is_non_terminal(tau_parser::extra, n->child.back())));
	std::vector<tau<BAs...>> c (n->child);
	c.emplace_back(wrap<BAs...>(tau_parser::extra, note));
	return make_node(n->value, move(c));
}

template <typename... BAs>
tau<BAs...> build_num(size_t num) {
	return wrap(
		tau_parser::num, rewriter::make_node<tau_sym<BAs...>>(
			tau_sym<BAs...>(num), {}));
}

template <typename... BAs>
tau<BAs...> build_variable(const std::string& name) {
	return wrap<BAs...>(tau_parser::variable, name);
}

template <typename... BAs>
tau<BAs...> build_in_var_name(const size_t& index) {
	std::stringstream var_name;
	var_name << "i" << index;
	return wrap<BAs...>(tau_parser::in_var_name, var_name.str());
}

template<typename... BAs>
tau<BAs...> build_type(const std::string& type) {
	return wrap<BAs...>(tau_parser::type, type);
}

template<typename... BAs>
tau<BAs...> build_bf_t_type(const std::string& type) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_t,
				build_type<BAs...>(type)));
}

template<typename... BAs>
tau<BAs...> build_bf_t_type(const tau<BAs...>& type) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_t,
				type));
}

template<typename... BAs>
tau<BAs...> build_bf_f_type(const std::string& type) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_f,
				build_type<BAs...>(type)));
}

template<typename... BAs>
tau<BAs...> build_bf_f_type(const tau<BAs...>& type) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_f,
				type));
}

template<typename... BAs>
tau<BAs...> build_in_variable_at_n(const tau<BAs...>& in_var_name, const size_t& num) {
	assert(is_non_terminal(tau_parser::in_var_name, in_var_name));

	return wrap(
		tau_parser::bf, wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var,	wrap(
					tau_parser::in,
						in_var_name, wrap(
						tau_parser::offset,
							build_num<BAs...>(num))))));
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_n(const size_t& index, const size_t& num) {
	return build_in_variable_at_(build_in_var_name<BAs...>(index), num);
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_t(const tau<BAs...>& in_var_name) {
	assert(is_non_terminal(tau_parser::in_var_name, in_var_name));

	return wrap(
		tau_parser::bf, wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var,	wrap(
					tau_parser::in,
						in_var_name, wrap(
						tau_parser::offset,
							build_variable<BAs...>('t'))))));
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_t(const size_t& index) {
	return build_in_variable_at_t(build_in_var_name<BAs...>(index));
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_t_minus (const std::string& name, const int_t shift) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::in_var_name, name);
	auto shift_node = wrap<BAs...>(p::shift, {wrap<BAs...>(p::variable, "t"), build_num<BAs...>(shift)});
	auto offset = wrap<BAs...>(p::offset, shift_node);
	return wrap<BAs...>(p::bf, wrap<BAs...>(p::variable, wrap<BAs...>(p::io_var, wrap<BAs...>(p::in, { var_name, offset }))));
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_t_minus(const tau<BAs...>& in_var_name, const size_t& num) {
	assert(is_non_terminal(tau_parser::in_var_name, in_var_name));
	assert(num > 0);

	return wrap(
		tau_parser::bf, wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var,	wrap(
					tau_parser::in,
						in_var_name, wrap(
						tau_parser::offset, wrap(
							tau_parser::shift,
								build_variable<BAs...>('t'),
								build_num<BAs...>(num)))))));
}

template <typename... BAs>
tau<BAs...> build_in_variable_at_t_minus(const size_t& index, const size_t& num) {
	return build_in_variable_at_t_minus(build_in_var_name<BAs...>(index), num);
}

template <typename... BAs>
tau<BAs...> build_out_var_name(const size_t& index) {
	std::stringstream var_name;
	var_name << "o" << index;
	return wrap<BAs...>(tau_parser::out_var_name, var_name.str());
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_t(const tau<BAs...>& out_var_name) {
	assert(is_non_terminal(tau_parser::out_var_name, out_var_name));
	return wrap(
		tau_parser::bf,	wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var, wrap(
					tau_parser::out,
						out_var_name, wrap(
						tau_parser::offset,
							build_variable<BAs...>('t'))))));
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_t(const size_t& index) {
	return build_out_variable_at_t(build_out_var_name<BAs...>(index));
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_n(const tau<BAs...>& out_var_name, const size_t& num) {
	assert(is_non_terminal(tau_parser::out_var_name, out_var_name));

	return wrap(
		tau_parser::bf,	wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var, wrap(
					tau_parser::out,
						out_var_name, wrap(
						tau_parser::offset,
							build_num<BAs...>(num))))));
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_n(const size_t& index, const size_t& num) {
	return build_out_variable_at_n(build_out_var_name<BAs...>(index), num);
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_t_minus (const std::string& name, const int_t shift) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto shift_node = wrap<BAs...>(p::shift, {wrap<BAs...>(p::variable, "t"), build_num<BAs...>(shift)});
	auto offset = wrap<BAs...>(p::offset, shift_node);
	return wrap<BAs...>(p::bf, wrap<BAs...>(p::variable, wrap<BAs...>(p::io_var, wrap<BAs...>(p::out, { var_name, offset }))));
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_t_minus(const tau<BAs...>& out_var_name, const size_t& num) {
	assert(is_non_terminal(tau_parser::out_var_name, out_var_name));
	assert(num > 0);

	return wrap(
		tau_parser::bf,	wrap(
			tau_parser::variable, wrap(
				tau_parser::io_var, wrap(
					tau_parser::out,
						out_var_name, wrap(
						tau_parser::offset, wrap(
							tau_parser::shift,
								build_variable<BAs...>('t'),
								build_num<BAs...>(num)))))));
}

template <typename... BAs>
tau<BAs...> build_out_variable_at_t_minus(const size_t& index, const size_t& num) {
	return build_out_variable_at_t_minus(build_out_var_name<BAs...>(index), num);
}

// ------ Helpers for variables having io_var as child ---------------
template <typename... BAs>
auto is_io_initial (const tau<BAs...>& io_var) {
	return (trim2(io_var)->child[1] | tau_parser::num).has_value();
}

template <typename... BAs>
auto is_io_shift (const tau<BAs...>& io_var) {
	return (trim2(io_var)->child[1] | tau_parser::shift).has_value();
}

template <typename... BAs>
auto get_io_time_point (const tau<BAs...>& io_var) {
	return size_t_extractor<BAs...>(trim2(trim2(io_var)->child[1])).value();
}

template <typename... BAs>
auto get_io_shift (const tau<BAs...>& io_var) {
	return size_t_extractor<BAs...>(trim2(io_var)->child[1]->child[0]->child[1]->child[0]).value();
}

template <typename... BAs>
std::string get_io_name (const tau<BAs...>& io_var) {
	std::stringstream ss; ss << trim(trim2(io_var));
	return ss.str();
}

template <typename... BAs>
int_t get_io_var_shift(const tau<BAs...>& io_var) {
	// If there is a shift
	if (is_io_shift(io_var))
		return get_io_shift(io_var);
	return 0;
}

int_t get_max_shift(const auto& io_vars, bool ignore_temps = false) {
	int_t max_shift = 0;
	for (const auto& v : io_vars) {
		if (ignore_temps && get_io_name(v)[0] == '_')
			continue;
		max_shift = std::max(max_shift, get_io_var_shift(v));
	}
	return max_shift;
}

template <typename... BAs>
int_t get_max_initial(const std::vector<tau<BAs...>>& io_vars) {
	int_t max_init = -1;
	for (const tau<BAs...>& v : io_vars) {
		if (is_io_initial(v)) {
			int_t init = get_io_time_point(v);
			max_init = std::max(max_init, init);
		}
	}
	return max_init;
}

// -------------------------------------------------------------------

template <typename... BAs>
tau<BAs...> build_bf_var(const std::string& name) {
	auto var = make_builder<BAs...>("( $X ) =: " + name + ".").second;
	return trim<BAs...>(var);
}

template <typename... BAs>
tau<BAs...> build_wff_var(const std::string& name) {
	auto var = make_builder<BAs...>("( $X ) =:: ?" + name + ".").second;
	return trim<BAs...>(var);
}

template <typename... BAs>
tau<BAs...> build_bf_constant(const tau<BAs...>& cte) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_constant, wrap(
				tau_parser::constant,
					cte)));
}

template<typename... BAs>
tau<BAs...> build_bf_constant(const tau<BAs...>& cte, const tau<BAs...>& type) {
	return wrap(
		tau_parser::bf, wrap(
			tau_parser::bf_constant, wrap(
				tau_parser::constant,
					cte),
				type));
}

template <typename... BAs>
tau<BAs...> build_bf_constant(const std::variant<BAs...>& v) {
	auto cte = rewriter::make_node<tau_sym<BAs...>>(tau_sym<BAs...>(v), {});
	return build_bf_constant<BAs...>(cte);
}

template<typename... BAs>
tau<BAs...> build_bf_constant(const std::variant<BAs...>& v, const tau<BAs...>& type) {
	auto cte = rewriter::make_node<tau_sym<BAs...>>(tau_sym<BAs...>(v), {});
	return build_bf_constant<BAs...>(cte, type);
}

template<typename... BAs>
tau<BAs...> build_bf_constant(const std::variant<BAs...>& v, const std::optional<tau<BAs...>>& type) {
	auto cte = rewriter::make_node<tau_sym<BAs...>>(tau_sym<BAs...>(v), {});
	return type.has_value() ? build_bf_constant<BAs...>(cte, type.value()) : build_bf_constant<BAs...>(cte);
}

template<typename... BAs>
tau<BAs...> build_bf_and_constant( const auto& ctes)
{
	if (ctes.empty()) return _1<BAs...>;

	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
		[&](const auto& l, const auto& r) { return l & r; });

	return build_bf_constant<BAs...>(cte);
}

template <typename... BAs>
tau<BAs...> build_bf_or_constant( const auto& ctes)
{
	if (ctes.empty()) return _0<BAs...>;

	auto cte = std::accumulate(++ctes.begin(), ctes.end(), *ctes.begin(),
		[&](const auto& l, const auto& r) { return l | r; });

	return build_bf_constant<BAs...>(cte);
}

template <typename... BAs>
std::optional<tau<BAs...>> build_bf_constant(
	const std::optional<std::variant<BAs...>>& o)
{
	return o.has_value() ? build_bf_constant(o.value())
				: std::optional<tau<BAs...>>();
}

template <typename... BAs>
tau<BAs...> build_bf_uniter_const(const std::string& n1, const std::string& n2) {
	auto name = wrap<BAs...>(tau_parser::uninter_const_name, n1 + ":" + n2);
	return wrap(tau_parser::bf,
		wrap(tau_parser::variable,
			wrap(tau_parser::uninterpreted_constant, name)));
}

// wff factory method for building wff formulas
template <typename... BAs>
tau<BAs...> build_wff_eq(const tau<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::bf_eq, l, _0<BAs...>));
}

template <typename... BAs>
tau<BAs...> build_wff_neq(const tau<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::bf_neq, l, _0<BAs...>));
}

template <typename... BAs>
tau<BAs...> build_wff_and(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_and, l, r));
}

template <typename... BAs>
tau<BAs...> build_wff_and(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _T<BAs...>,
		[](const auto& l, const auto& r) {return build_wff_and<BAs...>(l, r);});
}

template <typename... BAs>
tau<BAs...> build_wff_or(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_or, l, r));
}

template <typename... BAs>
tau<BAs...> build_wff_or(const auto& wffs) {
	return std::accumulate(wffs.begin(), wffs.end(), _F<BAs...>,
		[](const auto& l, const auto& r) { return build_wff_or<BAs...>(l, r);});
}

template <typename... BAs>
tau<BAs...> build_wff_neg(const tau<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::wff_neg, l));
}

template <typename... BAs>
tau<BAs...> build_wff_xor_from_def(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_or<BAs...>(build_wff_and(build_wff_neg(l), r),
		build_wff_and(build_wff_neg(r), l));
}

template <typename... BAs>
tau<BAs...> build_wff_xor(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_or(
		build_wff_and(build_wff_neg(l), r),
		build_wff_and(build_wff_neg(r), l));
}

template <typename... BAs>
tau<BAs...> build_wff_imply(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_or<BAs...>(build_wff_neg<BAs...>(l), r);
}

template <typename... BAs>
tau<BAs...> build_wff_conditional(const tau<BAs...>& x,
	const tau<BAs...>& y,
	const tau<BAs...>& z)
{
	return build_wff_and<BAs...>(build_wff_imply<BAs...>(x, y),
		build_wff_imply<BAs...>(build_wff_neg<BAs...>(x), z));
}

template <typename... BAs>
tau<BAs...> build_wff_equiv(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_and<BAs...>(build_wff_imply<BAs...>(l, r),
		build_wff_imply<BAs...>(r, l));
}

template <typename... BAs>
tau<BAs...> build_wff_all(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_all, l, r));
}

template <typename... BAs>
tau<BAs...> build_wff_ex(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::wff, wrap(tau_parser::wff_ex, l, r));
}

template <typename... BAs>
tau<BAs...> build_wff_sometimes(const tau<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::wff_sometimes, l));
}

template <typename... BAs>
tau<BAs...> build_wff_always(const tau<BAs...>& l) {
	return wrap(tau_parser::wff, wrap(tau_parser::wff_always, l));
}

// bf factory method for building bf formulas
template <typename... BAs>
tau<BAs...> build_bf_and(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::bf, wrap(tau_parser::bf_and, l, r));
}

template <typename... BAs>
tau<BAs...> build_bf_and(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _1<BAs...>,
		[](const auto& l, const auto& r) { return build_bf_and<BAs...>(l, r);});
}

template <typename... BAs>
tau<BAs...> build_bf_or(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return wrap(tau_parser::bf, wrap(tau_parser::bf_or, l, r));
}

template <typename... BAs>
tau<BAs...> build_bf_or(const auto& bfs) {
	return std::accumulate(bfs.begin(), bfs.end(), _0<BAs...>,
		[](const auto& l, const auto& r) { return build_bf_or<BAs...>(l, r); });
}

template <typename... BAs>
tau<BAs...> build_bf_neg(const tau<BAs...>& l) {
	return wrap(tau_parser::bf, wrap(tau_parser::bf_neg, l));
}

template <typename... BAs>
tau<BAs...> build_bf_xor_from_def(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_bf_or<BAs...>(
		build_bf_and(build_bf_neg(l), r),
		build_bf_and(build_bf_neg(r), l));
}

template <typename... BAs>
tau<BAs...> build_bf_xor(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_bf_or<BAs...>(
		build_bf_and<BAs...>(build_bf_neg<BAs...>(l), r),
		build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r)));
}

template <typename... BAs>
tau<BAs...> build_bf_less(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_and<BAs...>(
		build_wff_eq<BAs...>(
			build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r))),
		build_wff_neq<BAs...>(build_bf_xor<BAs...>(l, r)));
}

template <typename... BAs>
tau<BAs...> build_bf_nless(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_neg<BAs...>(build_bf_less<BAs...>(l, r));
}

template <typename... BAs>
tau<BAs...> build_bf_less_equal(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_eq<BAs...>(build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r)));
}

template <typename... BAs>
tau<BAs...> build_bf_nleq(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_neq<BAs...>(build_bf_and<BAs...>(l, build_bf_neg<BAs...>(r)));
}

template <typename... BAs>
tau<BAs...> build_bf_interval(const tau<BAs...>& x,
	const tau<BAs...>& y, const tau<BAs...>& z)
{
	return build_wff_and<BAs...>(build_bf_less_equal<BAs...>(x, y),
		build_bf_less_equal<BAs...>(y, z));
}

template <typename... BAs>
tau<BAs...> build_bf_nleq_lower(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	std::vector<tau<BAs...>> args {trim(l), trim(r)};
	return tau_apply_builder<BAs...>(bldr_bf_nleq_lowwer<BAs...>, args);
}

template <typename... BAs>
tau<BAs...> build_bf_nleq_upper(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	std::vector<tau<BAs...>> args {trim(l), trim(r)};
	return tau_apply_builder<BAs...>(bldr_bf_nleq_upper<BAs...>, args);
}

template <typename... BAs>
tau<BAs...> build_bf_greater(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_bf_less<BAs...>(r, l);
}

template <typename... BAs>
tau<BAs...> build_bf_ngreater(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_neg<BAs...>(build_bf_greater<BAs...>(l, r));
}

template <typename... BAs>
tau<BAs...> build_bf_greater_equal(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_bf_less_equal<BAs...>(r, l);
}

template <typename... BAs>
tau<BAs...> build_bf_ngeq(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return build_wff_neg<BAs...>(build_bf_greater_equal<BAs...>(l, r));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_greater_equal(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_greater_equal, {ctnvar, num})));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_greater(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_greater, {ctnvar, num})));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_less_equal(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_less_equal, {ctnvar, num})));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_less(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_less, {ctnvar, num})));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_eq(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_eq, {ctnvar, num})));
}

template <typename... BAs>
tau<BAs...> build_wff_ctn_neq(const tau<BAs...>& ctnvar,
	const tau<BAs...>& num) {
	return wrap(tau_parser::wff,
			wrap(tau_parser::constraint,
				wrap(tau_parser::ctn_neq, {ctnvar, num})));
}

template <typename... BAs>
tau<BAs...> operator&(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	auto bf_constant_and = [](const auto& l, const auto& r) -> tau<BAs...> {
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
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(l & rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r)) {
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(l & rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
			&& is_non_terminal<tau_parser::wff, BAs...>(r))
		return build_wff_and<BAs...>(l, r);
	throw std::logic_error("wrong types");
}

template <typename... BAs>
tau<BAs...> operator|(const tau<BAs...>& l,
	const tau<BAs...>& r)
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
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(l | rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(l | rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
		&& is_non_terminal<tau_parser::wff, BAs...>(r))
			return build_wff_or<BAs...>(l, r);
	throw std::logic_error("wrong types");
}

template <typename... BAs>
tau<BAs...> operator~(const tau<BAs...>& l) {
	auto bf_constant_neg = [](const auto& l) -> tau<BAs...> {
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
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(~ll);
	}
	if (is_child_non_terminal<tau_parser::bf_neq, BAs...>(l)) {
		auto ll = l
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(~ll);
	}
	if (is_non_terminal<tau_parser::wff>(l))
		return build_wff_neg<BAs...>(l);
	throw std::logic_error("wrong types");
}

template <typename... BAs>
tau<BAs...> operator^(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	auto bf_constant_xor = [](const auto& l, const auto& r) -> tau<BAs...> {
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
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(l ^ rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(l ^ rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
		&& is_non_terminal<tau_parser::wff, BAs...>(r))
			return build_wff_xor<BAs...>(l, r);
	throw std::logic_error("wrong types");
}

template <typename... BAs>
tau<BAs...> operator+(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return l ^ r;
}

// This function traverses n and normalizes coefficients in a BF
template <typename... BAs>
tau<BAs...> normalize_ba(const tau<BAs...>& fm) {
#ifdef TAU_CACHE
	static std::map<tau<BAs...>, tau<BAs...>> cache;
	if (auto it = cache.find(fm); it != cache.end())
		return it->second;
#endif
	assert(is_non_terminal(tau_parser::bf, fm));
	auto norm_ba = [&](const auto& n, const auto& c) {

		BOOST_LOG_TRIVIAL(trace) << "normalize_ba/norm_ba/n: " << n;
		BOOST_LOG_TRIVIAL(trace) << "normalize_ba/norm_ba/c: " << c;

		if (!is_child_non_terminal(tau_parser::bf_constant, n)) {
			auto res = n->child == c ? n : make_node(n->value, c);
#ifdef TAU_CACHE
			cache.emplace(res, res);
			return cache.emplace(n, res).first->second;
#endif
			return res;
		}
		auto ba_elem = n
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		auto res = normalize_ba(ba_elem);
		using p = tau_parser;
		auto r = wrap(p::bf, wrap(p::bf_constant, wrap(p::constant,
			rewriter::make_node<tau_sym<BAs...>>(tau_sym<BAs...>(res), {}))));
#ifdef TAU_CACHE
		cache.emplace(r, r);
		return cache.emplace(n, r).first->second;
#endif
		return r;
	};
	return rewriter::post_order_recursive_traverser<tau<BAs...>>()(
						fm, rewriter::all, norm_ba);
}

template <typename... BAs>
bool is_zero(const tau<BAs...>& l) {
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

template <typename... BAs>
bool is_one(const tau<BAs...>& l) {
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

template <typename... BAs>
bool operator==(const tau<BAs...>& l, const bool& r) {
	return r ? is_one(l) : is_zero(l);
}

template <typename... BAs>
bool operator==(const bool l, const tau<BAs...>& r) {
	return r == l;
}

template <typename... BAs>
tau<BAs...> operator<<(const tau<BAs...>& n,
	const std::map<tau<BAs...>, tau<BAs...>>& changes)
{
	return replace(n, changes);
}

template <typename... BAs>
tau<BAs...> operator<<(const tau<BAs...>& n,
	const std::pair<tau<BAs...>, tau<BAs...>>& change)
{
	std::map<tau<BAs...>, tau<BAs...>> changes{change};
	return replace(n, changes);
}

// Splitter function for a nso tau_parser::bf_constant node holding a BA constant
template <typename... BAs>
tau<BAs...> splitter(const tau<BAs...>& n,
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

// apply one tau rule to the given expression
// IDEA maybe this could be operator|
template <typename... BAs>
tau<BAs...> nso_rr_apply(const rewriter::rule<tau<BAs...>>& r,
	const tau<BAs...>& n)
{
	#ifdef TAU_CACHE
	static std::map<std::pair<rewriter::rule<tau<BAs...>>, tau<BAs...>>, tau<BAs...>> cache;
	if (auto it = cache.find({r, n}); it != cache.end()) return it->second;
	#endif // TAU_CACHE

	#ifdef TAU_MEASURE
	measures::increase_rule_counter<tau<BAs...>>(r);
	#endif // TAU_MEASURE

	// IDEA maybe we could traverse only once
	auto nn = apply_rule<tau<BAs...>, is_capture_t<BAs...>>(r, n, is_capture<BAs...>);
	#ifdef TAU_MEASURE
	if (n != nn) measures::increase_rule_hit<tau<BAs...>>(r);
	#endif // TAU_MEASURE

	std::map<tau<BAs...>, tau<BAs...>> changes;

	// apply numerical simplifications
	// TODO (HIGH) check if this is done with hooks
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
		auto cnn = replace<tau<BAs...>>(nn, changes);
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
template <typename... BAs>
tau<BAs...> replace_with(const tau<BAs...>& node, const tau<BAs...>& with,
	const tau<BAs...> in)
{
	std::map<tau<BAs...>, tau<BAs...>> changes = {{node, with}};
	return replace<tau<BAs...>>(in, changes);
}

// apply the given rules to the given expression
// IDEA maybe this could be operator|
template <typename... BAs>
tau<BAs...> nso_rr_apply(const rules<tau<BAs...>>& rs, const tau<BAs...>& n)
{
	#ifdef TAU_CACHE
	static std::map<std::pair<rules<tau<BAs...>>, tau<BAs...>>, tau<BAs...>> cache;
	if (auto it = cache.find({rs, n}); it != cache.end()) return it->second;
	#endif // TAU_CACHE

	if (rs.empty()) return n;
	tau<BAs...> nn = n;
	for (auto& r : rs) nn = nso_rr_apply<BAs...>(r, nn);

	#ifdef TAU_CACHE
	cache[{rs, n}] = nn;
	#endif // TAU_CACHE
	return nn;
}

//
// tau factory methods
//
template <typename...BAs>
tau<BAs...> first_argument_formula(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[0];
}

template <typename...BAs>
tau<BAs...> first_argument_expression(const rewriter::node<tau_sym<BAs...>>& n){
	return n.child[0]->child[0]->child[0];
}

template <typename...BAs>
tau<BAs...> second_argument_formula(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[1];
}

template <typename...BAs>
tau<BAs...> second_argument_expression(const rewriter::node<tau_sym<BAs...>>& n)
{
	return n.child[0]->child[1]->child[0];
}

template <typename...BAs>
tau<BAs...> third_argument_formula(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[2];
}

template <typename...BAs>
tau<BAs...> third_argument_expression(const rewriter::node<tau_sym<BAs...>>& n){
	return n.child[0]->child[2]->child[0];
}

template <typename...BAs>
tau<BAs...> logic_operator(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0];
}

template <typename...BAs>
tau<BAs...> quantifier(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0];
}

template <typename...BAs>
tau<BAs...> quantified_formula(const rewriter::node<tau_sym<BAs...>>& n) {
	return n.child[0]->child[0];
}

/*template<typename...BAs>
std::optional<tau<BAs...>> type_of(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	if (auto check = l | tau_parser::type; check) return check.value();
	if (auto check = r | tau_parser::type; check) return check.value();
	return {};
}*/

template<typename...BAs>
std::optional<tau<BAs...>> type_of(const tau<BAs...>& e)
{
	return e | tau_parser::type;
}

template<typename...BAs>
tau<BAs...> make_node_hook_cte_or(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto r = second_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	auto type = type_l ? type_l : type_r;
	return l && r ? build_bf_constant<BAs...>(l.value() | r.value(), type)
		: std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);

}

template<typename...BAs>
tau<BAs...> build_bf_1(const rewriter::node<tau_sym<BAs...>>& n) {
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	if (type_l == type_r && type_l) return build_bf_t_type<BAs...>(type_l.value());
	if (type_l == type_r) return _1<BAs...>;
	if (type_l && !type_r) return build_bf_t_type<BAs...>(type_l.value());
	if (!type_l && type_r) return build_bf_t_type<BAs...>(type_r.value());
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template<typename...BAs>
tau<BAs...> build_bf_0(const rewriter::node<tau_sym<BAs...>>& n) {
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	if (type_l == type_r && type_l) return build_bf_f_type<BAs...>(type_l.value());
	if (type_l == type_r) return _0<BAs...>;
	if (type_l && !type_r) return build_bf_f_type<BAs...>(type_l.value());
	if (!type_l && type_r) return build_bf_f_type<BAs...>(type_r.value());
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_bf_or(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_SIMPLIFY_ONE_00, "1 | 1 := 1.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_01, "0 |  0:= 0.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_02, "1 | 0 := 1.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_03, "0 | 1 := 1.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
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
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_cte_and(const rewriter::node<tau_sym<BAs...>>& n) {
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto r = second_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	auto type = type_l ? type_l : type_r;
	return l && r ? build_bf_constant<BAs...>(l.value() & r.value(), type)
		: std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_bf_and(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_SIMPLIFY_ONE_00, "1 & 1 := 1.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_01, "0 & 0 := 0.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_02, "1 & 0 := 0.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_03, "0 & 1 := 0.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
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
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_cte_neg(const rewriter::node<tau_sym<BAs...>>& n) {
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto type = type_of(first_argument_expression(n));
	return l ? build_bf_constant<BAs...>(~l.value(), type)
		: std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_bf_neg(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(BF_SIMPLIFY_ONE_4, "1' := 0.")
	if (auto neg_one = logic_operator(n) | tau_parser::bf | tau_parser::bf_t;
			neg_one && is_non_terminal<tau_parser::bf_neg>(logic_operator(n))) {
		auto type = type_of(neg_one.value());
		return type ? build_bf_f_type(type.value()) : _0<BAs...>;
	}
	//RULE(BF_SIMPLIFY_ZERO_4, "0' := 1.")
	if (auto neg_zero = logic_operator(n) | tau_parser::bf | tau_parser::bf_f;
			neg_zero && is_non_terminal<tau_parser::bf_neg>(logic_operator(n))) {
		auto type = type_of(neg_zero.value());
		return type ? build_bf_t_type(type.value()) : _1<BAs...>;
		}
	//RULE(BF_ELIM_DOUBLE_NEGATION_0, "$X'' :=  $X.")
	if (auto double_neg = logic_operator(n) | tau_parser::bf | tau_parser::bf_neg; double_neg
			&& is_non_terminal<tau_parser::bf_neg>(logic_operator(n)))
		return double_neg.value()->child[0];
	//RULE(BF_CALLBACK_NEG, "{ $X }' := bf_neg_cb $X.")
	if (is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n)))
		return make_node_hook_cte_neg(n);
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_cte_xor(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	auto l = first_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto r = second_argument_expression(n)
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	auto type = type_l ? type_l : type_r;
	return l && r ? build_bf_constant<BAs...>(l.value() ^ r.value(), type)
		: std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);

}

template <typename... BAs>
tau<BAs...> make_node_hook_bf_xor(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_SIMPLIFY_ONE_00, "1 ^ 1 := 0.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_01, "0 ^ 0 := 0.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_0<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_02, "1 ^ 0 := 1.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
	//RULE(BF_SIMPLIFY_ONE_03, "0 ^ 1 := 1.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_bf_1<BAs...>(n);
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

template <typename...BAs>
tau<BAs...> make_node_hook_cte(const rewriter::node<tau_sym<BAs...>>& n)
{
	auto l = n
		| tau_parser::bf_constant
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	//RULE(BF_CALLBACK_IS_ZERO, "{ $X } := bf_is_zero_cb { $X } 1.")
	//RULE(BF_CALLBACK_IS_ONE, "{ $X } := bf_is_one_cb { $X } 1.")
	if (l.has_value()) {
		auto typed = n | tau_parser::bf_constant | tau_parser::type;
		if (l.value() == false)
			return typed.has_value() ? build_bf_f_type(typed.value()) : _0<BAs...>;
		else if (l.value() == true)
			return typed.has_value() ? build_bf_t_type(typed.value()) : _1<BAs...>;
	}
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_ctn(const rewriter::node<tau_sym<BAs...>>& n) {
	auto sp_n = make_shared<rewriter::node<tau_sym<BAs...>>>(n);
	auto num = find_top(sp_n, is_non_terminal<tau_parser::num, BAs...>).value();
	auto ctnvar = find_top(sp_n, is_non_terminal<tau_parser::ctnvar, BAs...>).value();
	auto op = non_terminal_extractor<BAs...>(trim2(sp_n)).value();
	switch (op) {
	case tau_parser::ctn_eq:
		return build_wff_and(build_wff_ctn_less_equal(ctnvar, num),
				build_wff_ctn_greater_equal(ctnvar, num));
	case tau_parser::ctn_neq:
		return build_wff_or(build_wff_ctn_less(ctnvar, num),
	 			build_wff_ctn_greater(ctnvar, num));
	}
	return sp_n;
}

template <typename...BAs>
tau<BAs...> make_node_hook_ctn_neg(const tau<BAs...>& n) {
	auto num = find_top(n, is_non_terminal<tau_parser::num, BAs...>).value();
	auto ctnvar = find_top(n, is_non_terminal<tau_parser::ctnvar, BAs...>).value();
	auto op = non_terminal_extractor<BAs...>(trim(n)).value();
	switch (op) {
		//RULE(BF_PUSH_NEGATION_INWARDS_2, "($X != $Y)' := $X = $Y.")
	case tau_parser::ctn_neq:
		return build_wff_ctn_eq<BAs...>(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_3, "($X = $Y)' := $X != $Y.")
	case tau_parser::ctn_eq:
		return build_wff_ctn_neq<BAs...>(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_4, "($X >= $Y)' := $X < $Y.")
	case tau_parser::ctn_greater_equal:
		return build_wff_ctn_less<BAs...>(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_5, "($X > $Y)' := $X <= $Y.")
	case tau_parser::ctn_greater:
		return build_wff_ctn_less_equal<BAs...>(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_6, "($X <= $Y)' := $X > $Y.")
	case tau_parser::ctn_less_equal:
		return build_wff_ctn_greater<BAs...>(ctnvar, num);
		//RULE(BF_PUSH_NEGATION_INWARDS_7, "($X < $Y)' := $X >= $Y.")
	case tau_parser::ctn_less:
		return build_wff_ctn_greater_equal<BAs...>(ctnvar, num);
	}
	// Can never happen - all cases exhausted
	assert(false);
	return n;
}

template <typename... BAs>
tau<BAs...> make_node_hook_bf(const rewriter::node<tau_sym<BAs...>>& n) {
	// if n is ref, capture, 0 or 1, we can return accordingly
	//if (n.child.size() != 1)
	//	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
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
			default: return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
		}
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_and(const rewriter::node<tau_sym<BAs...>>& n) {
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
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_or(const rewriter::node<tau_sym<BAs...>>& n) {
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
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_neg(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(WFF_SIMPLIFY_ONE_4, " ! T ::= F.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return _F<BAs...>;
	//RULE(WFF_SIMPLIFY_ZERO_4, "! F ::= T.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_ELIM_DOUBLE_NEGATION_0, "! ! $X ::=  $X.")
	if (auto double_neg = first_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff; double_neg)
		return double_neg.value();
	if (is_non_terminal<tau_parser::constraint>(first_argument_expression(n))) {
		return make_node_hook_ctn_neg(first_argument_expression(n));
	}
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_wff_eq_cte(const rewriter::node<tau_sym<BAs...>>& n){
	auto l = n
		| tau_parser::bf_eq
		| tau_parser::bf
		| tau_parser::bf_constant
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	if (l.has_value() && l.value() == false) return _T<BAs...>;
	else if (l.has_value()) return _F<BAs...>;
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
tau<BAs...> build_wff_t(const rewriter::node<tau_sym<BAs...>>& n) {
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	if (type_l == type_r && type_l) return _T<BAs...>;
	if (type_l == type_r) return _T<BAs...>;
	if (type_l && !type_r) return _T<BAs...>;
	if (!type_l && type_r) return _T<BAs...>;
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
tau<BAs...> build_wff_f(const rewriter::node<tau_sym<BAs...>>& n) {
	auto type_l = type_of(first_argument_expression(n));
	auto type_r = type_of(second_argument_expression(n));
	if (type_l == type_r && type_l) return _F<BAs...>;
	if (type_l == type_r) return _F<BAs...>;
	if (type_l && !type_r) return _F<BAs...>;
	if (!type_l && type_r) return _F<BAs...>;
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template<typename... BAs>
tau<BAs...> make_node_hook_wff_eq(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_EQ_SIMPLIFY_0, "1 = 0 ::=  F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_EQ_SIMPLIFY_1, "0 = 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_0, "0 = 1")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_1, "1 = 1")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_N, "{c} = 0 ::= ...."): this should never happen
	if(is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return make_node_hook_wff_eq_cte(n);
	//RULE(BF_DEF_EQ, "$X = $Y ::= $X & $Y' | $X' & $Y = 0.")
	if (!is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_eq<BAs...>(
			build_bf_xor(first_argument_formula(n), second_argument_formula(n)));
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename...BAs>
tau<BAs...> make_node_hook_wff_neq_cte(const rewriter::node<tau_sym<BAs...>>& n)
{
	auto l = n
		| tau_parser::bf_neq
		| tau_parser::bf
		| tau_parser::bf_constant
		| tau_parser::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	if (l.has_value() && l.value() == false) return _F<BAs...>;
	else if (l.has_value()) return _T<BAs...>;
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_neq(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_NEQ_SIMPLIFY_0, "0 != 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NEQ_SIMPLIFY_1, "1 != 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NEQ_SIMPLIFY_2, "0 != 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NEQ_SIMPLIFY_3, "1 != 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_N, "{c} = 0 ::= ....")
	if(is_non_terminal<tau_parser::bf_constant>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return make_node_hook_wff_neq_cte(n);
	//RULE(BF_DEF_NEQ, "$X != $Y ::= $X & $Y' | $X' & $Y != 0.")
	if (!is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_neq<BAs...>(
			build_bf_xor(first_argument_formula(n), second_argument_formula(n)));
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_sometimes(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(WFF_SIMPLIFY_ONE_6, " sometimes T ::= T.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_SIMPLIFY_ZERO_6, "sometimes F ::= F.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return _F<BAs...>;
	//RULE(WFF_SIMPLIFY_SOMETIMES_1,  "sometimes sometimes $X ::= sometimes $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_sometimes; double_quantifier)
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_SOMETIMES_2,  "sometimes always $X ::= always $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_always;	double_quantifier)
		return first_argument_formula(n);
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_always(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(WFF_SIMPLIFY_ONE_5, " always T ::= T.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return _T<BAs...>;
	//RULE(WFF_SIMPLIFY_ZERO_5, "always F ::= F.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return _F<BAs...>;
	//RULE(WFF_SIMPLIFY_ALWAYS_1,     "always always $X ::= always $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_always;
			double_quantifier && is_non_terminal<tau_parser::wff_always>(quantifier(n)))
		return first_argument_formula(n);
	//RULE(WFF_SIMPLIFY_ALWAYS_2,     "always sometimes $X ::= sometimes $X.")
	if (auto double_quantifier = first_argument_formula(n) | tau_parser::wff_sometimes;
			double_quantifier && is_non_terminal<tau_parser::wff_always>(quantifier(n)))
		return first_argument_formula(n);
	return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_less(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_LESS_SIMPLIFY_20, "0 < 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_LESS_SIMPLIFY_21, "1 < 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_LESS_SIMPLIFY_0, "0 < 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_N, "1 < 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_LESS_SIMPLIFY_0, "$X < 0 ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _F<BAs...>;
	//RULE(BF_DEF_SIMPLIFY_N, "$X < 1 ::= $X' != 0.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_neq<BAs...>(build_bf_neg(first_argument_formula(n)));

	return build_bf_less<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_nless(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_NLESS_SIMPLIFY_20, "0 !< 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NLESS_SIMPLIFY_21, "1 !< 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NLESS_SIMPLIFY_0, "0 !< 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_N, "1 !< 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NLESS_SIMPLIFY_0, "$X !< 0 ::= T.") @CP
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(BF_DEF_SIMPLIFY_N, "$X !< 1 ::= $X' = 0.")
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_eq<BAs...>(build_bf_neg(first_argument_formula(n)));
	return build_bf_nless<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_less_equal(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(BF_LESS_EQUAL_SIMPLIFY_2, "0 <= 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_LESS_EQUAL_SIMPLIFY_0, "0 <= 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_LESS_EQUAL_SIMPLIFY_1, "1 <= 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_DEF_SIMPLIFY_N, "1 <= 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_LESS_EQUAL_SIMPLIFY_0, "$X <= 1 ::= T.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _T<BAs...>;
	return build_bf_less_equal<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_nleq(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_NLEQ_SIMPLIFY_2, "0 !<= 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NLEQ_SIMPLIFY_0, "0 !<= 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NLEQ_SIMPLIFY_1, "1 !<= 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NLEQ_SIMPLIFY_0, "1 !<= 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NLEQ_SIMPLIFY_0, "$X !<= 1 ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _F<BAs...>;
	return build_bf_nleq<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_greater(const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(BF_GREATER_SIMPLIFY_2, "1 > 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_GREATER_SIMPLIFY_0, "0 > 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_GREATER_SIMPLIFY_1, "0 > 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_GREATER_SIMPLIFY_3, "1 > 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_GREATER_SIMPLIFY_0, "$X > 1 ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _F<BAs...>;
	//RULE(BF_GREATER_SIMPLIFY_3, "0 > $X ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return _F<BAs...>;
	return build_bf_greater<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_ngreater(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(BF_NGREATER_SIMPLIFY_2, "1 !> 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NGREATER_SIMPLIFY_1, "0 !> 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NGREATER_SIMPLIFY_0, "0 !> 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NGREATER_SIMPLIFY_3, "1 !> 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NGREATER_SIMPLIFY_0, "$X !> 1 ::= T.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(BF_NGREATER_SIMPLIFY_3, "0 !> $X ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n)))
		return _T<BAs...>;
	return build_bf_ngreater<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
inline tau<BAs...> make_node_hook_wff_greater_equal(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_2, "1 >= 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_1, "0 >= 0 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_0, "0 >= 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_3, "1 >= 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_0, "$X >= 0 ::= T.") @CP
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _T<BAs...>;
	//RULE(BF_GREATER_EQUAL_SIMPLIFY_2, "1 >= X ::= T.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n)))
		return _T<BAs...>;
	return build_bf_greater_equal<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_ngeq(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_NGEQ_SIMPLIFY_2, "1 !>= 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NGEQ_SIMPLIFY_1, "0 !>= 0 ::= F.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NGEQ_SIMPLIFY_0, "0 !>= 1 ::= T.")
	if (is_non_terminal<tau_parser::bf_f>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_t<BAs...>(n);
	//RULE(BF_NGEQ_SIMPLIFY_3, "1 !>= 1 ::= F.")
	if (is_non_terminal<tau_parser::bf_t>(first_argument_expression(n))
			&& is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return build_wff_f<BAs...>(n);
	//RULE(BF_NGEQ_SIMPLIFY_0, "$X !>= 0 ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_f>(second_argument_expression(n)))
		return _F<BAs...>;
	//RULE(BF_NGEQ_SIMPLIFY_1, "1 !>= $X ::= F.") @CP
	if (is_non_terminal<tau_parser::bf_t>(second_argument_expression(n)))
		return _F<BAs...>;
	return build_bf_ngeq<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_interval(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	return build_bf_interval<BAs...>(first_argument_formula(n), second_argument_formula(n), third_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_xor(const rewriter::node<tau_sym<BAs...>>& n) {
	//RULE(BF_XOR_SIMPLIFY_0, "$X ^ F ::= $X.")
	if (is_non_terminal<tau_parser::wff_f>(second_argument_expression(n)))
		return first_argument_formula(n);
	//RULE(BF_XOR_SIMPLIFY_1, "F ^ $X ::= $X.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(BF_XOR_SIMPLIFY_2, "$X ^ $X ::= F.")
	if (first_argument_formula(n) == second_argument_formula(n))
		return _F<BAs...>;
	//RULE(BF_XOR_SIMPLIFY_3, "$X ^ ! $X ::= T.")
	if (auto negated = second_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == first_argument_formula(n))
		return _T<BAs...>;
	//RULE(BF_XOR_SIMPLIFY_4, "! $X ^ $X ::= T.")
	if (auto negated = first_argument_formula(n) | tau_parser::wff_neg | tau_parser::wff;
			negated && negated.value() == second_argument_formula(n))
		return _T<BAs...>;
	//RULE(BF_XOR_SIMPLIFY_5, "$X ^ T ::= ! $X.")
	if (is_non_terminal<tau_parser::wff_t>(second_argument_expression(n)))
		return build_wff_neg<BAs...>(first_argument_formula(n));
	//RULE(BF_XOR_SIMPLIFY_6, "T ^ $X ::= ! $X.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return build_wff_neg<BAs...>(second_argument_formula(n));
	return build_wff_xor<BAs...>(first_argument_formula(n), second_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_conditional(
	const rewriter::node<tau_sym<BAs...>>& n)
{
	//RULE(WFF_CONDITIONAL_SIMPLIFY_0, "F ? $X : $Y ::= $Y.")
	if (is_non_terminal<tau_parser::wff_f>(first_argument_expression(n)))
		return third_argument_formula(n);
	//RULE(WFF_CONDITIONAL_SIMPLIFY_1, "T ? $X : $Y ::= $X.")
	if (is_non_terminal<tau_parser::wff_t>(first_argument_expression(n)))
		return second_argument_formula(n);
	//RULE(WFF_CONDITIONAL_SIMPLIFY_4, "$X ? $Y : $Y ::= $Y.")
	if (second_argument_formula(n) == third_argument_formula(n))
		return third_argument_formula(n);
	return build_wff_conditional<BAs...>(first_argument_formula(n), second_argument_formula(n), third_argument_formula(n));
}

template <typename... BAs>
tau<BAs...> make_node_hook_wff_imply(const rewriter::node<tau_sym<BAs...>>& n) {
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

template <typename... BAs>
tau<BAs...> make_node_hook_wff_equiv(const rewriter::node<tau_sym<BAs...>>& n) {
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

template <typename... BAs>
tau<BAs...> make_node_hook_wff(const rewriter::node<tau_sym<BAs...>>& n) {
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
		case tau_parser::constraint:
			return make_node_hook_wff_ctn<BAs...>(n);
		case tau_parser::bf_less:
			return make_node_hook_wff_less<BAs...>(n);
		case tau_parser::bf_nless:
			return make_node_hook_wff_nless<BAs...>(n);
		case tau_parser::bf_less_equal:
			return make_node_hook_wff_less_equal<BAs...>(n);
		case tau_parser::bf_nleq:
			return make_node_hook_wff_nleq<BAs...>(n);
		case tau_parser::bf_greater:
			return make_node_hook_wff_greater<BAs...>(n);
		case tau_parser::bf_ngreater:
			return make_node_hook_wff_ngreater<BAs...>(n);
		case tau_parser::bf_greater_equal:
			return make_node_hook_wff_greater_equal<BAs...>(n);
		case tau_parser::bf_ngeq:
			return make_node_hook_wff_ngeq<BAs...>(n);
		case tau_parser::bf_interval:
			return make_node_hook_wff_interval<BAs...>(n);
		default: return std::make_shared<rewriter::node<tau_sym<BAs...>>>(n);
	}
}

template <typename...BAs>
struct make_tau_node {
	std::optional<tau<BAs...>> operator()(
		const rewriter::node<tau_sym<BAs...>>& n)
	{
		if (is_non_terminal_node<BAs...>(n)) {
			switch (get_non_terminal_node(n)) {
				case tau_parser::bf: {
					return make_node_hook_bf<BAs...>(n);
				}
				case tau_parser::wff: {
					return make_node_hook_wff<BAs...>(n);
				}
				default: return std::optional<tau<BAs...>>();
			}
		}
		return std::optional<tau<BAs...>>();
	}
};

} // namespace idni::tau_lang

namespace idni::rewriter {

template <typename...BAs>
struct make_node_hook<idni::tau_lang::tau_sym<BAs...>> {
	std::optional<idni::tau_lang::tau<BAs...>> operator()(
		const rewriter::node<idni::tau_lang::tau_sym<BAs...>>& n)
	{
		static idni::tau_lang::make_tau_node<BAs...> hook;
		return hook(n);
	}
};

} // namespace idni::rewriter

//
// operators << to pretty print the tau language related types
//

template <typename...BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::rewriter::rule<idni::tau_lang::tau<BAs...>>& r)
{
	return stream << r.first << " := " << r.second << ".";
}

// << for rules
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau_lang::rules<idni::tau_lang::tau<BAs...>>& rs)
{
	for (const auto& r : rs) stream << r << " ";
	return stream;
}

// << for tau_source_sym
std::ostream& operator<<(std::ostream& stream,
				const idni::tau_lang::tau_source_sym& rs);

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
	const idni::tau_lang::tau_sym<BAs...>& rs)
{
	// using tau_sym = std::variant<tau_source_sym, std::variant<BAs...>, size_t>;
	std::visit(overloaded {
		[&stream](const idni::tau_lang::tau_source_sym& t) {
			if (!t.nt() && !t.is_null()) stream << t.t();
		},
		[&stream](const auto& a) { stream << a; }
	}, rs);
	return stream;
}

// << for formulas
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau_lang::rr<idni::tau_lang::tau<BAs...>>& f)
{
	stream << f.rec_relations;
	if (f.main) stream << f.main;
	return stream;
}

// << for bindings
// TODO (HIGH) << for bindings depends on << for variant<BAs...>
// TODO (HIGH) << for bindings needs to follow tau lang syntax
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau_lang::bindings<BAs...>& bs)
{
	for (const auto& b : bs) stream << b.first << ": " << b.second << "\n";
	return stream;
}

// outputs a tau<...> to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
//
// IDEA maybe it should be move to out.h
template <typename... BAs>
std::ostream& pp(std::ostream& stream, const idni::tau_lang::tau<BAs...>& n,
	std::vector<size_t>& hl_path, size_t& depth,
	size_t parent = tau_parser::start, bool passthrough = false);

template <typename... BAs>
std::ostream& pp(std::ostream& stream, const idni::tau_lang::tau<BAs...>& n,
	size_t parent = tau_parser::start, bool passthrough = false)
{
	std::vector<size_t> hl_path;
	size_t depth = 0;
	return pp(stream, n, hl_path, depth, parent, passthrough);
}

template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau_lang::tau<BAs...>& n) { return pp(stream, n); }

// << for node<tau_sym>
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::rewriter::node<idni::tau_lang::tau_sym<BAs...>>& n)
{
	return stream << std::make_shared<
			idni::rewriter::node<idni::tau_lang::tau_sym<BAs...>>>(n);
}

// old operator<< renamed to print_terminals and replaced by
// pp pretty priniter
template <typename... BAs>
std::ostream& print_terminals(std::ostream& stream,
	const idni::tau_lang::tau<BAs...>& n)
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
			const idni::tau_lang::sp_tau_source_node& n);

// << tau_source_node (make it shared to make use of the previous operator)
std::ostream& operator<<(std::ostream& stream,
					const idni::tau_lang::tau_source_node& n);

inline static const std::map<size_t, std::string> hl_colors = {
	{ tau_parser::bf,            idni::TC.LIGHT_GREEN() },
	{ tau_parser::variable,      idni::TC.WHITE() },
	{ tau_parser::capture,       idni::TC.BLUE() },
	{ tau_parser::wff_all,       idni::TC.MAGENTA() },
	{ tau_parser::wff_ex,        idni::TC.LIGHT_MAGENTA() },

	{ tau_parser::rec_relation,  idni::TC.YELLOW() },
	{ tau_parser::constraint,    idni::TC.LIGHT_MAGENTA() },
	{ tau_parser::io_var,        idni::TC.WHITE() },
	{ tau_parser::constant,      idni::TC.LIGHT_CYAN() }

	// { tau_parser::rule,          idni::TC.BG_YELLOW() },
	// { tau_parser::builder,       idni::TC.BG_LIGHT_YELLOW() }
};

inline static const std::vector<size_t> breaks = {
	tau_parser::wff_and, tau_parser::wff_or, tau_parser::wff_xor,
	tau_parser::wff_imply, tau_parser::wff_equiv,
	tau_parser::wff_all, tau_parser::wff_ex
};

inline static const std::vector<size_t> indents = {
	tau_parser::wff_sometimes, tau_parser::wff_always,
	tau_parser::wff_conditional,
	tau_parser::wff_all, tau_parser::wff_ex,
	tau_parser::wff_imply, tau_parser::wff_equiv
};

template <typename... BAs>
std::ostream& pp(std::ostream& stream, const idni::tau_lang::tau<BAs...>& n,
	std::vector<size_t>& hl_path, size_t& depth, size_t parent,
	bool passthrough)
{
	using namespace idni;
// #define DEBUG_PP
// #ifdef DEBUG_PP
// auto& p = tau_parser::instance();
// 	auto dbg = [&stream, &p](const auto& c) {
// 		if (std::holds_alternative<idni::tau_lang::tau_source_sym>(c->value)) {
// 			auto tss = std::get<idni::tau_lang::tau_source_sym>(c->value);
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
	static auto is_to_wrap = [](const idni::tau_lang::tau<BAs...>& n,
		size_t parent)
	{
		static const std::set<size_t> no_wrap_for = {
			tau_parser::constraint,
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
			{ tau_parser::run_cmd,                          50 },
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
			{ tau_parser::bf_rule,                          60 },
			{ tau_parser::wff_rule,                         60 },
			{ tau_parser::ref,                              80 },
			{ tau_parser::wff,                              90 },
			// wff
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
			{ tau_parser::bf_interval,                     501 },
			{ tau_parser::bf_eq,                           502 },
			{ tau_parser::bf_neq,                          503 },
			{ tau_parser::bf_less_equal,                   504 },
			{ tau_parser::bf_nleq,                         505 },
			{ tau_parser::bf_greater,                      506 },
			{ tau_parser::bf_ngreater,                     507 },
			{ tau_parser::bf_greater_equal,                508 },
			{ tau_parser::bf_ngeq,                         509 },
			{ tau_parser::bf_less,                         510 },
			{ tau_parser::bf_nless,                        511 },
			{ tau_parser::wff,                             580 },
			// bf
			{ tau_parser::bf_or,                           720 },
			{ tau_parser::bf_and,                          730 },
			{ tau_parser::bf_xor,                          740 },
			{ tau_parser::bf_neg,                          750 },
			{ tau_parser::bf,                              790 },

			{ tau_parser::rec_relation,                    800 },
			{ tau_parser::ref_args,                        800 },
			{ tau_parser::bf_rule,                         800 },
			{ tau_parser::wff_rule,                        800 },
			{ tau_parser::binding,                         800 },
		};
		static const std::set<size_t> wrap_child_for = {
			};
		if (std::holds_alternative<idni::tau_lang::tau_source_sym>(n->value)) {
			auto tss = std::get<idni::tau_lang::tau_source_sym>(n->value);
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
		for (const auto& c : n->child)
			pp(stream, c, hl_path, depth, parent);
		return stream;
	}

	if (std::holds_alternative<idni::tau_lang::tau_source_sym>(n->value)) {
		auto& ch = n->child;
		auto tss = std::get<idni::tau_lang::tau_source_sym>(n->value);
		auto ppch = [&](size_t i) -> std::ostream& {
			return pp(stream, ch[i], hl_path, depth, tss.n());
		};
		auto indent = [&depth, &stream]() {
			if (!idni::tau_lang::pretty_printer_indenting) return;
			for (size_t i = 0; i < depth; ++i) stream << "\t";
		};
		auto break_line = [&]() {
			if (!idni::tau_lang::pretty_printer_indenting) return;
			stream << "\n", indent();
		};
		auto break_if_needed = [&]() -> bool {
			if (!idni::tau_lang::pretty_printer_indenting) return false;
			if (find(breaks.begin(), breaks.end(), tss.n())
				!= breaks.end()) return break_line(), true;
			return false;
		};
		auto sep = [&](const std::string& separator) {
			for (size_t i = 0; i < ch.size(); ++i) {
				if (i > 0) stream << separator << " ";
				ppch(i);
			}
		};
		auto pass = [&]() {
			sep("");
		};
		// auto pass_nows = [&]() {
		// 	for (const auto& c : ch)
		// 		pp(stream, c, hl_path, depth, tss.n());
		// };
		auto infix_nows = [&](const std::string& op) {
			ppch(0);
			if (ch.size() == 1) return;
			stream << op, break_if_needed();
			ppch(1);
		};
		auto infix = [&](const std::string& op) {
			infix_nows(std::string(" ") + op + " ");
		};
		auto infix2 = [&](const std::string& op1,
			const std::string& op2)
		{
			infix(op1);
			stream << " " << op2 << " ";
			ppch(2);
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
			using namespace idni::tau_lang;
			size_t quant_nt = tss.n();
			auto qch = ch;
			switch (quant_nt) {
			case tau_parser::wff_all:  stream << "all";   break;
			case tau_parser::wff_ex:   stream << "ex";    break;
			}
			tau<BAs...> expr;
			size_t expr_nt;
			do {
				pp(stream << " ", qch[0], hl_path, depth, quant_nt);
				expr = qch[1]->child[0];
				expr_nt = expr | non_terminal_extractor<BAs...>
					| optional_value_extractor<size_t>;
				if (expr_nt == quant_nt) {
					stream << ",", qch = expr->child;
				} else {
					if (!break_if_needed()) stream << " ";
					pp(stream, expr, hl_path, depth, tss.n());
					break;
				}
			} while (true);
		};
		auto print_bf_and = [&]() {
			std::stringstream ss;
			bool is_hilight = idni::tau_lang::pretty_printer_highlighting;
			if (is_hilight)
				idni::tau_lang::pretty_printer_highlighting = false;
			pp(ss, ch[0], hl_path, depth, tss.n());
			if (is_hilight)
				idni::tau_lang::pretty_printer_highlighting = true;
			auto str = ss.str();
			if (is_hilight)
				pp(stream, ch[0], hl_path, depth, tss.n());
			else stream << str;
			char lc = str[str.size()-1];
			if (isdigit(lc) // || lc == '}'
				|| idni::tau_lang::is_child_non_terminal(
					tau_parser::bf_constant, ch[0]))
						stream << " ";
			pp(stream, ch[1], hl_path, depth, tss.n());
		};
		if (tss.nt()) { //stream /*<< "*" << tss.nts << "-"*/ << tau_parser::instance().name(tss.n()) << ":";
			// indenting and breaklines
			bool indented = false;
			if (idni::tau_lang::pretty_printer_indenting)
				if (find(indents.begin(), indents.end(),
					tss.n()) != indents.end())
						indented = true, depth++;
			// syntax highlighting color start
			bool hl_pop = false;
			if (idni::tau_lang::pretty_printer_highlighting)
				if (auto it = hl_colors.find(tss.n());
					it != hl_colors.end())
						hl_path.push_back(tss.n()),
						hl_pop = true,
						stream << it->second;
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
			case tau_parser::in:
			case tau_parser::out:
				infix_nows("["); stream << "]"; break;
			// wrappable by parenthesis
			case tau_parser::bf:
			case tau_parser::wff:
			{
				bool wrap = is_to_wrap(ch[0], parent);
				if (wrap) {
					stream << "(";
					if (tss.n() == tau_parser::wff)
						depth++, break_line();
				}
				pp(stream, ch[0], hl_path, depth, parent);
				if (wrap) {
					if (tss.n() == tau_parser::wff)
						depth--, break_line();
					stream << ")";
				}
			} break;
			case tau_parser::shift:
				if (ch.size() == 1) pass();
				else infix_nows("-");
				break;
			case tau_parser::bf_constant:
				stream << "{ ", ppch(0) << " }";
				if (ch.size() > 1) stream << " : ", ppch(1);
				break;
			// nodes to wrap
			case tau_parser::constraint: wrap("[", "]"); break;
			case tau_parser::bf_splitter: wrap("S(", ")"); break;
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
			case tau_parser::bf_and:         print_bf_and(); break;
			case tau_parser::bf_or:          infix_nows("|"); break;
			case tau_parser::bf_xor:         infix_nows("+"); break;
			case tau_parser::bf_eq:             infix("="); break;
			case tau_parser::bf_neq:            infix("!="); break;
			case tau_parser::bf_less_equal:     infix("<="); break;
			case tau_parser::bf_nleq:           infix("!<="); break;
			case tau_parser::bf_greater:        infix(">"); break;
			case tau_parser::bf_ngreater:       infix("!>"); break;
			case tau_parser::bf_greater_equal:  infix(">="); break;
			case tau_parser::bf_ngeq:           infix("!>="); break;
			case tau_parser::bf_less:           infix("<"); break;
			case tau_parser::bf_nless:          infix("!<"); break;
			case tau_parser::ctn_neq:           infix("!="); break;
			case tau_parser::ctn_eq:            infix("=");  break;
			case tau_parser::ctn_greater_equal: infix(">="); break;
			case tau_parser::ctn_greater:       infix(">");  break;
			case tau_parser::ctn_less_equal:    infix("<="); break;
			case tau_parser::ctn_less:          infix("<");  break;
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
			case tau_parser::run_cmd:       prefix("run"); break;
			case tau_parser::normalize_cmd: prefix("normalize"); break;
			case tau_parser::inst_cmd:
			case tau_parser::subst_cmd:
				stream << (tss.n() == tau_parser::inst_cmd
						? "instantiate" : "substitute")
					<< " ", ppch(1) << " [", ppch(2)
					<< " / ", ppch(3) << "]";
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
			case tau_parser::ctnvar:
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
			// fixed point calculation fallback
			case tau_parser::fp_fallback: prefix(" fallback"); break;
			// simple symbols
			case tau_parser::first_sym: stream << "first"; break;
			case tau_parser::last_sym: stream << "last"; break;
			case tau_parser::bf_f:  stream << '0'; break;
				if (ch.size() > 0) stream << " : ", ppch(0);
				break;
			case tau_parser::bf_t:  stream << '1'; break;
				if (ch.size() > 0) stream << " : ", ppch(0);
				break;
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
					pp(stream, c, hl_path, depth, parent);
				break;
			}
			// indenting and breaklines
			if (idni::tau_lang::pretty_printer_indenting && indented)
				depth--;
			// syntax highlighting color end
			if (idni::tau_lang::pretty_printer_highlighting && hl_pop) {
				hl_path.pop_back();
				stream << TC.CLEAR();
				if (hl_path.size()) // restore the prev color
					stream << hl_colors.at(hl_path.back());
			}
		}
		else if (!tss.is_null()) stream << tss.t();
	} else stream << n->value;
	return stream;
}

#endif // __NSO_RR_H__
