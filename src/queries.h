// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __QUERIES_H__
#define __QUERIES_H__

#include "nso_rr.h"
namespace idni::tau_lang {

template <typename... BAs>
bool is_non_terminal(const size_t, const tau<BAs...>&);
template <typename... BAs>
bool is_child_non_terminal(const size_t nt, const tau<BAs...>& n);
template <size_t nt, typename...BAs>
bool is_child_non_terminal(const tau<BAs...>& n);

template <typename... BAs>
bool is_non_terminal_node(const tau_sym<BAs...>& s) {
	return std::holds_alternative<tau_source_sym>(s)
		&& get<tau_source_sym>(s).nt();
}

template <typename... BAs>
bool is_z3_node(const tau_sym<BAs...>& s) {
	return std::holds_alternative<z3::expr>(s);
}

template <typename... BAs>
bool is_z3_node(const rewriter::node<tau_sym<BAs...>>& s) {
	return is_z3_node(s.value);
}

template <typename... BAs>
bool is_z3_node(const tau<BAs...>& n) {
	return is_z3_node<BAs...>(n->value);
}

template <typename... BAs>
auto is_z3_constant = [](const tau<BAs...>& n) {
	return std::holds_alternative<z3::expr>(n->child[0]->value);
};

template <typename... BAs>
auto is_z3_literal = [](const tau<BAs...>& n) {
	return is_child_non_terminal<tau_parser::z3_eq, BAs...>(n)
		|| is_child_non_terminal<tau_parser::z3_neq, BAs...>(n)
		|| is_child_non_terminal<tau_parser::z3_less_equal, BAs...>(n)
		|| is_child_non_terminal<tau_parser::z3_nleq, BAs...>(n)
		|| is_child_non_terminal<tau_parser::z3_greater, BAs...>(n)
		|| is_child_non_terminal<tau_parser::z3_ngreater, BAs...>(n)
		|| is_child_non_terminal<tau_parser::z3_greater_equal, BAs...>(n)
		|| is_child_non_terminal<tau_parser::z3_ngeq, BAs...>(n)
		|| is_child_non_terminal<tau_parser::z3_less, BAs...>(n);
};

template <typename... BAs>
auto is_tau_literal = [](const tau<BAs...>& n) {
	return is_child_non_terminal<tau_parser::bf_eq, BAs...>(n)
		|| is_child_non_terminal<tau_parser::bf_neq, BAs...>(n)
		|| is_child_non_terminal<tau_parser::bf_less_equal, BAs...>(n)
		|| is_child_non_terminal<tau_parser::bf_nleq, BAs...>(n)
		|| is_child_non_terminal<tau_parser::bf_greater, BAs...>(n)
		|| is_child_non_terminal<tau_parser::bf_ngreater, BAs...>(n)
		|| is_child_non_terminal<tau_parser::bf_greater_equal, BAs...>(n)
		|| is_child_non_terminal<tau_parser::bf_ngeq, BAs...>(n)
		|| is_child_non_terminal<tau_parser::bf_less, BAs...>(n);
};

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
	return is_non_terminal_node<BAs...>(s)
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

// --- Traversal restrictions for Tau tree

template <typename... BAs>
auto visit_wff = [](const tau<BAs...>& n) static {
	using p = tau_parser;
	if (is_non_terminal(p::bf, n))
		return false;
	return true;
};

template <typename... BAs>
auto visit_io_vars = [] (const tau<BAs...>& n) static {
	using p = tau_parser;
	if (is_non_terminal(p::bf_constant, n))
		return false;
	if (is_non_terminal(p::uninterpreted_constant, n))
		return false;
	return true;
};

// ---

//TODO: think about cache
template <typename... BAs>
bool contains (const tau<BAs...>& fm, const tau<BAs...>& sub_fm) {
	bool is_contained = false;
	auto has_sub_fm = [&sub_fm, &is_contained](const auto& n) {
		if (n == sub_fm) return is_contained = true, false;
		return true;
	};
	pre_order(fm).search_unique(has_sub_fm);
	return is_contained;
}

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
const auto is_child_quantifier = [](const tau<BAs...>& n) {
	using p = tau_parser;
	return is_child_non_terminal(p::wff_all, n)
		|| is_child_non_terminal(p::wff_ex, n);
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

template<typename...BAs>
inline const auto int_extractor = [](const tau<BAs...>& n) {
	assert(is_non_terminal(tau_parser::integer, n));
	const auto& c = n->child;
	if (c.size() == 1) {
		return (int_t)std::get<size_t>(c[0]->value);
	} else {
		return -((int_t)std::get<size_t>(c[1]->value));
	}
};

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
using tau_node_terminal_extractor_t = decltype(tau_node_terminal_extractor<BAs...>);

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

} // namespace idni::tau_lang

// Specialization of tau node
namespace idni::rewriter {

// Specialization of tau node in order to hash typed and non-typed
// Tau constants the same
template <typename... BAs>
struct node<idni::tau_lang::tau_sym<BAs...>> {
	using child_type = std::vector<std::shared_ptr<node>>;
	node (const idni::tau_lang::tau_sym<BAs...> v, const child_type& c) :
		value(v), child(c), hash(calc_hash(v, c)) {}
	node (const idni::tau_lang::tau_sym<BAs...> v, child_type&& c) :
		value(v), child(std::move(c)), hash(calc_hash(v,child)) {}
	// This is not default because the hash value is not considered
	auto operator<=>(const node& that) const {
		if (value != that.value) return value <=> that.value;
		return child <=> that.child;
	}
	// We list all ordering operators explicitly
	bool operator<(const node& that) {
		return (*this <=> that) < 0;
	}
	bool operator<=(const node& that) {
		return (*this <=> that) <= 0;
	}
	bool operator>(const node& that) {
		return (*this <=> that) > 0;
	}
	bool operator>=(const node& that) {
		return (*this <=> that) >= 0;
	}
	auto operator==(const node& that) const {
		return value == that.value && child == that.child;
	}
	auto operator!=(const node& that) const {
		return !(*this == that);
	}

	// the value of the node and pointers to the children, we follow the same
	// notation as in forest<...>::tree to be able to reuse the code with
	// forest<...>::tree.
	const idni::tau_lang::tau_sym<BAs...> value;
	const child_type child;
	// Hash of the node
	const size_t hash;
private:
	static size_t calc_hash (const idni::tau_lang::tau_sym<BAs...> v, const child_type& c) {
		size_t seed = 0;
		// Hash bf_t and bf_f same for all types
		// This is done in order to use hash inequality in equality operator
		if (idni::tau_lang::is_non_terminal_sym(tau_parser::bf_t, v)) {
			return hash_combine(seed, v), seed;
		}
		if (idni::tau_lang::is_non_terminal_sym(tau_parser::bf_f, v)) {
			return hash_combine(seed, v), seed;
		};
		hash_combine(seed, v);
		for (const std::shared_ptr<node>& _c : c) hash_combine(seed, *_c);
		return seed;
	}
};

} // namespace idni::rewriter

// The hash function for tau as specialisation of std::hash
template<typename... BAs>
struct std::hash<idni::tau_lang::tau<BAs...>> {
	size_t operator()(const idni::tau_lang::tau<BAs...>& n) const noexcept {
		using namespace idni::tau_lang;
		using namespace idni::rewriter;
		// Let the hash respect the typing rules for bf_t and bf_f
		return hash<node<tau_sym<BAs...>>>{}(*n);
	}
};

// Hash for rr using specialization to std::hash
template <typename type_t>
struct std::hash<idni::tau_lang::rr<type_t>> {
	size_t operator()(const idni::tau_lang::rr<type_t>& rr) const noexcept {
		size_t seed = 0;
		hash_combine(seed, rr.main);
		hash_combine(seed, rr.rec_relations);
		return seed;
	}
};

#endif // __QUERIES_H__