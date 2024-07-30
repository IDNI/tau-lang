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
#include "tau.h"
#include "dict.h"

#include "../parser/bdd_parser.generated.h"

using namespace idni::rewriter;

namespace idni::tau {

using bdd_binding = hbdd<Bool>;
using sp_bdd_node = sp_tau_node<tau_ba<bdd_binding>, bdd_binding>;

// global static bdd variable cache
inline static std::map<int_t, bdd_binding> var_cache{};

struct bdd_factory {

	using parse_forest = idni::parser<char, char>::pforest;
	using parse_result = idni::parser<char, char>::result;
	using traverser_t  = traverser<tau_sym<bdd_binding>, bdd_parser>;
	static constexpr const auto& get_only_child =
			traverser_t::get_only_child_extractor();
	static constexpr const auto& get_terminals =
			traverser_t::get_terminal_extractor();
	static constexpr const auto& get_nonterminal =
			traverser_t::get_nonterminal_extractor();

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
	// parses a bdd from a string
	bdd_binding parse(const std::string& src) {
		auto& p = bdd_parser::instance();
		auto r = p.parse(src.c_str(), src.size());
		if (!r.found) {
			BOOST_LOG_TRIVIAL(error) << "# bdd binding: `"
				<< src << "`\n" << r.parse_error;
			return bdd_handle<Bool>::hfalse;
		}
		char dummy = 0;
		auto root = make_node_from_tree<bdd_parser, char,
			tau_sym<bdd_binding>>(dummy, r.get_shaped_tree());
		auto t = traverser_t(root) | bdd_parser::bdd;
		if (t.has_value()) return eval_node(t);
		return bdd_handle<Bool>::hfalse;
	}

	// builds a bdd bounded node parsed from terminals of a source binding
	sp_bdd_node build(const std::string type_name, const sp_bdd_node& sn) {
		if (type_name != "bdd") return sn;
		auto n = sn | tau_parser::source_binding | tau_parser::source
			| optional_value_extractor<sp_bdd_node>;
		std::string src = make_string<
			tau_node_terminal_extractor_t<
				tau_ba<bdd_binding>, bdd_binding>,
			sp_bdd_node>(
				tau_node_terminal_extractor<
					tau_ba<bdd_binding>, bdd_binding>,
				n);
		// check source cache
		if (auto cn = cache.find(src); cn != cache.end()) return
			cn->second;
		auto x = parse(src); // parse bdd from source
		auto nn = make_node<tau_sym<tau_ba<bdd_binding>, bdd_binding>>(
			tau_sym<tau_ba<bdd_binding>, bdd_binding>(x), {});
		return cache.emplace(src, nn).first->second; // cache and return
	}

	size_t index = 0;
	std::map<std::string, sp_bdd_node> cache;
};

struct bdd_binding_factory {

	sp_tau_node<bdd_binding> build(const std::string type_name, const sp_tau_node<bdd_binding>& n) {
		if (type_name != "bdd") return n;
		auto source = n | tau_parser::source_binding | tau_parser::source | optional_value_extractor<sp_tau_node<bdd_binding>>;
		std::string var = make_string<
			tau_node_terminal_extractor_t<bdd_binding>,sp_tau_node<bdd_binding>>(
				tau_node_terminal_extractor<bdd_binding>, source);
		if (auto cn = cache.find(var); cn != cache.end()) return cn->second;
		bdd_init<Bool>();
		// Trim whitespaces from var
		auto is_not_space = [](char c) {return !isspace(c);};
		auto var_trim = var | ranges::views::filter(is_not_space);
		var = {var_trim.begin(), var_trim.end()};
		// Make sure that variable name is saved in dict.h for printing
		int v = dict(var);
		auto ref = bdd_handle<Bool>::bit(true, v);
		auto nn =  make_node<tau_sym<bdd_binding>>(ref, {});
		return cache.emplace(var, nn).first->second;
	}

	std::map<std::string, sp_tau_node<bdd_binding>> cache;
};

struct tau_bdd_binding_factory {

	sp_tau_node<tau_ba<bdd_binding>, bdd_binding> build(const std::string type_name, const sp_tau_node<tau_ba<bdd_binding>, bdd_binding>& n) {
		if (type_name != "bdd") return n;
		auto source = n | tau_parser::source_binding | tau_parser::source | optional_value_extractor<sp_tau_node<tau_ba<bdd_binding>, bdd_binding>>;
		std::string var = make_string<
			tau_node_terminal_extractor_t<tau_ba<bdd_binding>, bdd_binding>,
			sp_tau_node<tau_ba<bdd_binding>, bdd_binding>>(
				tau_node_terminal_extractor<tau_ba<bdd_binding>, bdd_binding>, source);
		if (auto cn = cache.find(var); cn != cache.end()) return cn->second;
		bdd_init<Bool>();
		// Trim whitespaces from var
		auto is_not_space = [](char c) {return !isspace(c);};
		auto var_trim = var | ranges::views::filter(is_not_space);
		var = {var_trim.begin(), var_trim.end()};
		// Make sure that variable name is saved in dict.h for printing
		int v = dict(var);
		auto ref = bdd_handle<Bool>::bit(true, v);
		auto nn =  make_node<tau_sym<tau_ba<bdd_binding>, bdd_binding>>(ref, {});
		return cache.emplace(var, nn).first->second;
	}

	std::map<std::string, sp_tau_node<tau_ba<bdd_binding>, bdd_binding>> cache;
};

} // namespace idni::tau


#endif // __BDD_BINDING_H__