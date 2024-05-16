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

#include "nso_rr.h"
#include "bdd_handle.h"
#include "babdd.h"
#include "rewriting.h"
#include "tau.h"
#include "dict.h"

#include "../parser/bdd_parser.generated.h"

using namespace idni::rewriter;

// bdd printer taken from out.h
template<typename B, auto o = bdd_options<>::create()>
ostream& operator<<(ostream& os, const hbdd<B, o>& f) {
	if (f == bdd_handle<B, o>::htrue) return os << '1';
	if (f == bdd_handle<B, o>::hfalse) return os << '0';
	set<pair<B, vector<int_t>>> dnf = f->dnf();
	size_t n = dnf.size();
	set<string> ss;
	for (auto& c : dnf) {
		set<string> s;
		assert(!(c.first == false));
		stringstream t;
		if (!(c.first == true)) t << '{' << c.first << '}';
		for (int_t v : c.second)
			if (v < 0) s.insert(string(dict(-v)) + "'"s);
			else s.insert(dict(v));
		bool first = true;
		for (auto& x : s) {
			if (!first) t << " "; else first = false;
			t << x;
		}
		ss.insert(t.str());
	}
	for (auto& s : ss) {
		os << s;
		if (--n) os << " | ";
	}
	return os;
}

namespace idni::tau {

using bdd_binding = hbdd<Bool>;
using sp_bdd_node = sp_tau_node<tau_ba<bdd_binding>, bdd_binding>;

// global static bdd variable cache
inline static std::map<int_t, bdd_binding> var_cache{};

struct bdd_factory {

	using parse_forest = idni::parser<char, char>::pforest;
	using parse_result = idni::parser<char, char>::result;

	// transform a parse forest into a bdd
	bdd_binding transform(const parse_result& r) {
		std::vector<bdd_binding> x; // stack
		auto cb_enter = [&x, &r] (const auto& n) {
			if (!n->first.nt()) return; // skip if terminal
			auto nt = n->first.n(); // get nonterminal id
			static auto T = bdd_handle<Bool>::htrue;
			static auto F = bdd_handle<Bool>::hfalse;
			if      (nt == bdd_parser::T) x.push_back(T);
			else if (nt == bdd_parser::F) x.push_back(F);
			else if (nt == bdd_parser::var) {
				// get var id from var node's terminals
				auto v = dict(r.get_terminals(n));
				// use cached var if exists
				if (auto cn = var_cache.find(v);
					cn != var_cache.end())
						x.push_back(cn->second);
				else {// otherwise create a new var and cache it
					auto ref = bdd<Bool>::bit(v);
					x.push_back(var_cache.emplace(v,
						bdd_handle<Bool>::get(ref))
							.first->second);
				}
			}
		};
		auto cb_exit = [&x] (const auto& n, const auto&) {
			if (!n->first.nt()) return; // skip if terminal
			auto nt = n->first.n(); // get nonterminal id
			// shortcuts for current and previous stack elements
			#define CURR (x.back())
			#define PREV (x[x.size() - 2])
			if (nt == bdd_parser::negation) {
				BOOST_LOG_TRIVIAL(trace) << "CURR: " << CURR << " = " << ~CURR;
				CURR = ~CURR;
			} else if (nt == bdd_parser::conjunction) {
				BOOST_LOG_TRIVIAL(trace) << PREV << " & " << CURR << " = " << (PREV & CURR);
				PREV = PREV & CURR, x.pop_back();
			} else if (nt == bdd_parser::disjunction) {
				BOOST_LOG_TRIVIAL(trace) << PREV << " | " << CURR << " = " << (PREV | CURR);
				PREV = PREV | CURR, x.pop_back();
			} else if (nt == bdd_parser::exclusive_or) {
				BOOST_LOG_TRIVIAL(trace) << PREV << " ^ " << CURR << " = " << (PREV ^ CURR);
				PREV = PREV ^ CURR, x.pop_back();
			}
			#undef CURR
			#undef PREV
		};
		r.get_forest()->traverse(cb_enter, cb_exit); // run traversal
		// return the current element of the stack if any or hfalse
		DBG(assert(x.size() <= 1);)
		return x.size() ? x.back() : bdd_handle<Bool>::hfalse;
	}

	// parses a bdd from a string
	bdd_binding parse(const std::string& src) {
		auto& p = bdd_parser::instance();
		auto r = p.parse(src.c_str(), src.size());
		if (!r.found) {
			BOOST_LOG_TRIVIAL(error) << "# bdd source: `" << src
				<< "`\n" << r.parse_error;
			return bdd_handle<Bool>::hfalse;
		}
		return transform(r); // transform the forest into bdd
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
		std::string var = make_string_with_skip<
			tau_node_terminal_extractor_t<bdd_binding>,
			not_whitespace_predicate_t<bdd_binding>,
			sp_tau_node<bdd_binding>>(
				tau_node_terminal_extractor<bdd_binding>,
				not_whitespace_predicate<bdd_binding>, source);
		if (auto cn = cache.find(var); cn != cache.end()) return cn->second;
		auto ref = bdd<Bool>::bit(index++);
		auto nn =  make_node<tau_sym<bdd_binding>>(bdd_handle<Bool>::get(ref), {});
		return cache.emplace(var, nn).first->second;
	}

	size_t index = 0;
	std::map<std::string, sp_tau_node<bdd_binding>> cache;
};

struct tau_bdd_binding_factory {

	sp_tau_node<tau_ba<bdd_binding>, bdd_binding> build(const std::string type_name, const sp_tau_node<tau_ba<bdd_binding>, bdd_binding>& n) {
		if (type_name != "bdd") return n;
		auto source = n | tau_parser::source_binding | tau_parser::source | optional_value_extractor<sp_tau_node<tau_ba<bdd_binding>, bdd_binding>>;
		std::string var = make_string_with_skip<
			tau_node_terminal_extractor_t<tau_ba<bdd_binding>, bdd_binding>,
			not_whitespace_predicate_t<tau_ba<bdd_binding>, bdd_binding>,
			sp_tau_node<tau_ba<bdd_binding>, bdd_binding>>(
				tau_node_terminal_extractor<tau_ba<bdd_binding>, bdd_binding>,
				not_whitespace_predicate<tau_ba<bdd_binding>, bdd_binding>, source);
		if (auto cn = cache.find(var); cn != cache.end()) return cn->second;
		auto ref = bdd<Bool>::bit(index++);
		auto nn =  make_node<tau_sym<tau_ba<bdd_binding>, bdd_binding>>(bdd_handle<Bool>::get(ref), {});
		return cache.emplace(var, nn).first->second;
	}

	size_t index = 0;
	std::map<std::string, sp_tau_node<tau_ba<bdd_binding>, bdd_binding>> cache;
};

struct tau_bdd_splitter {

	std::variant<tau_ba<bdd_binding>, bdd_binding> operator()(const std::variant<tau_ba<bdd_binding>, bdd_binding>&) {
		// TODO (HIGH) implement
		return nullptr;
	}
};

} // namespace idni::tau


#endif // __BDD_BINDING_H__