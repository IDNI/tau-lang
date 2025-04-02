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

#include "../parser/tau_parser.generated.h"
#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/variant_ba.h"
#include "init_log.h"
#include "splitter_types.h"
#include "parser.h"
#include "utils.h"
#include "rewriting.h"
#include "utility/term_colors.h"
#include "defs.h"

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
using tau_source_node = depreciating::rewriter::node<idni::lit<char, char>>;
using sp_tau_source_node = depreciating::rewriter::sp_node<idni::lit<char, char>>;

// node type for the tau language related programs, libraries and
// specifications trees.
template <typename... BAs>
using tau_sym = std::variant<tau_source_sym, std::variant<BAs...>, size_t>;

template <typename... BAs>
using tau_depreciating = depreciating::rewriter::sp_node<tau_sym<BAs...>>;


template <typename node_t>
using rec_relation = depreciating::rewriter::rule<node_t>;

// IDEA maybe we could define a wrapper for recursive rules and rewriting rules that
// call the appropriate apply method. This would play also nice with the builders
// defined in the normalizer.

template <typename... BAs>
using builder = depreciating::rewriter::rule<tau_depreciating<BAs...>>;

// defines a vector of rules in the tau language, the order is important as it defines
// the order of the rules in the rewriting process of the tau language.
template <typename node_t>
using rules = std::vector<depreciating::rewriter::rule<node_t>>;

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


template <typename... BAs>
tau_depreciating<BAs...> operator<<(const tau_depreciating<BAs...>& n,
	const std::map<tau_depreciating<BAs...>, tau_depreciating<BAs...>>& changes)
{
	return replace(n, changes);
}

template <typename... BAs>
tau_depreciating<BAs...> operator<<(const tau_depreciating<BAs...>& n,
	const std::pair<tau_depreciating<BAs...>, tau_depreciating<BAs...>>& change)
{
	std::map<tau_depreciating<BAs...>, tau_depreciating<BAs...>> changes{change};
	return replace(n, changes);
}

// apply one tau rule to the given expression
// IDEA maybe this could be operator|
template <typename... BAs>
tau_depreciating<BAs...> nso_rr_apply(const depreciating::rewriter::rule<tau_depreciating<BAs...>>& r,
	const tau_depreciating<BAs...>& n)
{
	static const auto is_capture = [](const tau_depreciating<BAs...>& n) {
		return std::holds_alternative<tau_source_sym>(n->value)
			&& get<tau_source_sym>(n->value).nt()
			&& ( get<tau_source_sym>(n->value).n() == tau_parser::capture);
	};

	#ifdef TAU_CACHE
	static std::map<std::pair<depreciating::rewriter::rule<tau_depreciating<BAs...>>, tau_depreciating<BAs...>>, tau_depreciating<BAs...>> cache;
	if (auto it = cache.find({r, n}); it != cache.end()) return it->second;
	#endif // TAU_CACHE

	#ifdef TAU_MEASURE
	measures::increase_rule_counter<tau_depreciating<BAs...>>(r);
	#endif // TAU_MEASURE

	try {
		auto nn = apply_rule<tau_depreciating<BAs...>, decltype(is_capture)>(r, n, is_capture);

		#ifdef TAU_MEASURE
		if (n != nn) measures::increase_rule_hit<tau_depreciating<BAs...>>(r);
		#endif // TAU_MEASURE

		#ifdef TAU_CACHE
		cache[{r, n}] = nn;
		#endif // TAU_CACHE

		return nn;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(warning) << "(Warning) " << e.what();
		return n;
	}
}

// TODO (LOW) move it to a more appropriate place (parser)
template <typename... BAs>
tau_depreciating<BAs...> replace_with(const tau_depreciating<BAs...>& node, const tau_depreciating<BAs...>& with,
	const tau_depreciating<BAs...> inp)
{
	std::map<tau_depreciating<BAs...>, tau_depreciating<BAs...>> changes = {{node, with}};
	return replace<tau_depreciating<BAs...>>(inp, changes);
}

// apply the given rules to the given expression
// IDEA maybe this could be operator|
template <typename... BAs>
tau_depreciating<BAs...> nso_rr_apply(const rules<tau_depreciating<BAs...>>& rs, const tau_depreciating<BAs...>& n)
{
	#ifdef TAU_CACHE
	static std::map<std::pair<rules<tau_depreciating<BAs...>>, tau_depreciating<BAs...>>, tau_depreciating<BAs...>> cache;
	if (auto it = cache.find({rs, n}); it != cache.end()) return it->second;
	#endif // TAU_CACHE

	if (rs.empty()) return n;
	tau_depreciating<BAs...> nn = n;
	for (auto& r : rs) nn = nso_rr_apply<BAs...>(r, nn);

	#ifdef TAU_CACHE
	cache[{rs, n}] = nn;
	#endif // TAU_CACHE
	return nn;
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
	depreciating::rewriter::post_order_tree_traverser<stringify<extractor_t, node_t>,
				depreciating::rewriter::all_t, node_t>(sy, depreciating::rewriter::all)(n);
	return ss.str();
}

/**
 * @brief Structural equality for tau nodes
 * @param l Left-hand side of comparison.
 * @param r Right-hand side of comparison.
 * @return True, if the tree structure for l and r is the same, else false.
 */
template <typename... BAs>
struct struc_equal {
	bool operator() (const tau_depreciating<BAs...>& l, const tau_depreciating<BAs...>& r) const {
		if (r == nullptr && l == nullptr) return true;
		if (r == nullptr || l == nullptr) return false;

		if (std::addressof(*l) == std::addressof(*r)) return true;
		if (l->hash != r->hash) return false;

		if (l->value != r->value) return false;
		if (l->child.size() != r->child.size()) return false;

		//compare children
		for (size_t i = 0; i < l->child.size(); ++i)
			if (!operator()(l->child[i], r->child[i])) return false;
		return true;
	}
};

template <typename T, typename... BAs>
struct pair_struc_equal {
	bool operator() (const std::pair<tau_depreciating<BAs...>, T>& l, const std::pair<tau_depreciating<BAs...>, T>& r) const {
		return struc_equal<BAs...>()(l.first, r.first) && l.second == r.second;
	}
};

template <typename T1, typename T2, typename... BAs>
struct tuple_struc_equal {
	bool operator() (const std::tuple<tau_depreciating<BAs...>, T1, T2>& l, const std::tuple<tau_depreciating<BAs...>, T1, T2>& r) const {
		return struc_equal<BAs...>()(std::get<0>(l), std::get<0>(r)) &&
			std::get<1>(l) == std::get<1>(r) &&
				std::get<2>(l) == std::get<2>(r);
	}
};

template <typename value_t, typename... BAs>
using unordered_tau_map = std::unordered_map<tau_depreciating<BAs...>, value_t,
			std::hash<tau_depreciating<BAs...>>, struc_equal<BAs...>>;

template <typename... BAs>
using unordered_tau_set = std::unordered_set<tau_depreciating<BAs...>, std::hash<tau_depreciating<BAs...>>,
	struc_equal<BAs...>>;



template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const unordered_tau_set<BAs...>& set) {
	os << "{";
	for (auto it = set.begin(); it != set.end(); ++it) {
		if (std::next(it)!=set.end()) os << *it << ",";
		else os << *it;
	}
	os << "}";
	return os;
}

} // namespace tau_lang

namespace idni::depreciating::rewriter  {

template <typename... BAs>
struct make_node_cache_equality<tau_lang::tau_sym<BAs...>> {
	bool operator() (const node<tau_lang::tau_sym<BAs...>>& l,
		const node<tau_lang::tau_sym<BAs...>>& r) const {
		static tau_lang::struc_equal<BAs...> st_eq;
		if (std::addressof(l) == std::addressof(r)) return true;
		if (l.hash != r.hash) return false;

		if (l.value != r.value) return false;
		if (l.child.size() != r.child.size()) return false;

		//compare children
		for (size_t i = 0; i < l.child.size(); ++i)
			if (!st_eq(l.child[i], r.child[i])) return false;
		return true;
	}
};

template <typename... BAs>
struct traverser_cache_equality<tau_lang::tau_depreciating<BAs...>> {
	bool operator() (const tau_lang::tau_depreciating<BAs...>& l,
		const tau_lang::tau_depreciating<BAs...>& r) const {
		static tau_lang::struc_equal<BAs...> st_eq;
		return st_eq(l, r);
	}
};

template <typename... BAs>
struct traverser_pair_cache_equality<tau_lang::tau_depreciating<BAs...>> {
	using p = std::pair<tau_lang::tau_depreciating<BAs...>, size_t>;
	bool operator() (const p& l, const p& r) const {
		static tau_lang::struc_equal<BAs...> st_eq;
		return st_eq(l.first, r.first) && l.second == r.second;
	}
};

} // idni::depreciating

//
// operators << to pretty print the tau language related types
//

template <typename...BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::depreciating::rewriter::rule<idni::tau_lang::tau_depreciating<BAs...>>& r)
{
	return stream << r.first << " := " << r.second << ".";
}

// << for rules
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau_lang::rules<idni::tau_lang::tau_depreciating<BAs...>>& rs)
{
	for (const auto& r : rs) stream << r << " ";
	return stream;
}

// << for tau_source_sym
// std::ostream& operator<<(std::ostream& stream,
// 				const idni::tau_lang::tau_source_sym& rs);

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
	const idni::tau_lang::rr<idni::tau_lang::tau_depreciating<BAs...>>& f)
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

#endif // __NSO_RR_H__
