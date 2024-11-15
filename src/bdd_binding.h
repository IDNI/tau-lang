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

#ifndef __BDD_BINDING_H__
#define __BDD_BINDING_H__

#include <boost/log/trivial.hpp>

#include "nso_rr.h"
#include "bdd_handle.h"
#include "babdd.h"
#include "rewriting.h"
#include "tau_ba.h"
#include "dict.h"

#include "../parser/bdd_parser.generated.h"

namespace idni::tau {

using bdd_binding = hbdd<Bool>;
using sp_bdd_node = sp_tau_node<tau_ba<bdd_binding>, bdd_binding>;

// global static bdd variable cache
inline static std::map<int_t, bdd_binding> var_cache{};

template<typename...BAs>
struct bdd_factory {

	using parse_forest = idni::parser<char, char>::pforest;
	using parse_result = idni::parser<char, char>::result;
	using traverser_t  = traverser<tau_sym<BAs...>, bdd_parser>;
	static constexpr const auto& get_only_child =
			traverser_t::get_only_child_extractor();
	static constexpr const auto& get_terminals =
			traverser_t::get_terminal_extractor();
	static constexpr const auto& get_nonterminal =
			traverser_t::get_nonterminal_extractor();
	inline static std::map<std::string, nso<BAs...>> cache;

	// parses a bdd from a string
	std::optional<nso<BAs...>> parse(const std::string& src) {
		// check source cache
		if (auto cn = cache.find(src); cn != cache.end())
			return cn->second;
		auto& p = bdd_parser::instance();
		auto r = p.parse(src.c_str(), src.size());
		if (!r.found) return std::optional<nso<BAs...>>{};
		char dummy = 0;
		auto root = rewriter::make_node_from_tree<bdd_parser, char,
			tau_sym<BAs...>>(dummy, r.get_shaped_tree());
		auto t = traverser_t(root) | bdd_parser::bdd;
		return std::optional<nso<BAs...>>{ build_node(t.has_value()
			? eval_node(t) : bdd_handle<Bool>::hfalse) };
	}

	// builds a bdd bounded node parsed from terminals of a source binding
	nso<BAs...> binding(const nso<BAs...>& sn) {
		auto source = sn
			| tau_parser::source
			| optional_value_extractor<nso<BAs...>>;
		std::string src = make_string(
			tau_node_terminal_extractor<BAs...>, source);
		if (auto parsed = parse(src); parsed.has_value())
			return parsed.value();
		return sn;
	}

	std::variant<BAs...> splitter_one () const {
		return std::variant<BAs...>(bdd_splitter_one<Bool>());
	}

private:

	nso<BAs...> build_node(const bdd_binding& b) {
		std::variant<BAs...> vp{b};
		return rewriter::make_node<tau_sym<BAs...>>(vp, {});
	}

	// evaluates a parsed bdd terminal node recursively
	bdd_binding eval_node(const traverser_t& t) {
		//BOOST_LOG_TRIVIAL(debug) << "eval_node";
		auto n  = t | get_only_child;
		auto nt = n | get_nonterminal;
		switch (nt) {
		case bdd_parser::zero: return bdd_handle<Bool>::hfalse;
		case bdd_parser::one:  return bdd_handle<Bool>::htrue;
		case bdd_parser::negation: {
			auto e = eval_node(n | get_only_child);
			BOOST_LOG_TRIVIAL(trace) << e << "' = " << ~e;
			return ~e;
		}
		case bdd_parser::variable: {
			// get var id from var node's terminals
			auto var_name = n | get_terminals;
			auto v = dict(var_name);
			// use cached var if exists
			if (auto cn = var_cache.find(v);
				cn != var_cache.end())
					return cn->second;
			// otherwise create a new var and cache it
			auto ref = bdd<Bool>::bit(v);
			return var_cache.emplace(v, bdd_handle<Bool>::get(ref))
				.first->second;
		}
		default:
			auto o = (n || bdd_parser::bdd)();
			auto l = eval_node(o[0]), r = eval_node(o[1]);
			switch (nt) {
			case bdd_parser::disjunction:
				BOOST_LOG_TRIVIAL(trace)
					<< l << " | " << r << " -> " << (l | r);
				return l | r;
			case bdd_parser::exclusive_disjunction:
				BOOST_LOG_TRIVIAL(trace)
					<< l << " ^ " << r << " -> " << (l ^ r);
				return l ^ r;
			case bdd_parser::conjunction:
				BOOST_LOG_TRIVIAL(trace)
					<< l << " & " << r << " -> " << (l & r);
				return l & r;
			default: return bdd_handle<Bool>::hfalse;
			}
		}
	}

};

// using during testing
template<>
struct nso_factory<bdd_binding> {
	inline static bdd_factory<bdd_binding> bf;

	std::optional<nso<bdd_binding>> parse(const std::string& src,
		const std::string& = "")
	{
		return bf.parse(src);
	}

	nso<bdd_binding> binding(const nso<bdd_binding>& n,
		const std::string& = "")
	{
		return bf.binding(n);
	}

	std::vector<std::string> types() const {
		return { "sbf" };
	}

	nso<bdd_binding> splitter_one() const {
		return build_bf_constant(bf.splitter_one());
	}

	std::string default_type() const {
		return "sbf";
	}

	static nso_factory<bdd_binding>& instance() {
		static nso_factory<bdd_binding> factory;
		return factory;
	}

private:

	nso_factory() {};
};

// using in repl
template<>
struct nso_factory<tau_ba<bdd_binding>, bdd_binding> {

	inline static bdd_factory<tau_ba<bdd_binding>, bdd_binding> bf;
	inline static tau_ba_factory<bdd_binding> tf;

	std::optional<gssotc<bdd_binding>> parse(const std::string src,
		const std::string type_name)
	{
		if (type_name == "sbf")	return bf.parse(src);
		return tf.parse(src);
	}

	gssotc<bdd_binding> binding(
		const sp_tau_node<tau_ba<bdd_binding>, bdd_binding>& n,
		const std::string type_name)
	{
		if (type_name == "sbf") return bf.binding(n);
		return tf.binding(n);
	}

	std::vector<std::string> types() const {
		return { "sbf", "tau" };
	}

	std::string default_type() const {
		return "tau";
	}

	gssotc<bdd_binding> splitter_one (const std::string& type_name) const {
		if (type_name == "sbf")
			return build_bf_constant(bf.splitter_one());
		else return build_bf_constant(tf.splitter_one());
	}

	static nso_factory<tau_ba<bdd_binding>, bdd_binding>& instance() {
		static nso_factory<tau_ba<bdd_binding>, bdd_binding> factory;
		return factory;
	}

private:

	nso_factory() {};

};

} // namespace idni::tau


#endif // __BDD_BINDING_H__