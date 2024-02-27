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

#ifndef __EXECUTOR_H__
#define __EXECUTOR_H__

#include "tau.h"
#include "satisfiability.h"

template<typename...BAs>
using inputs = std::map<gssotc<BAs...>, gssotc<BAs...>>;

template<typename...BAs>
using outputs = std::map<gssotc<BAs...>, gssotc<BAs...>>;


template <typename...BAs>
struct compiled_gssotc_clause {

	gssotc<BAs...> clause;
	gssotc<BAs...> compiled;
};

template <typename...BAs>
struct compiled_gssotc {

	std::vector<compiled_gssotc_clause<BAs...>> compiled_clauses;
};

template <typename...BAs>
struct executor {

	gssotc<BAs...> execute(const gssotc<BAs...>& input) {
			// TODO (HIGH) implement
			return input;
	}

	compiled_gssotc<BAs...> compiled;
	inputs<BAs...> ins;
	outputs<BAs...> outs;
};

template <typename...BAs>
compiled_gssotc_clause<BAs...> compile_gssotc_clause(const gssotc<BAs...>& clause) {
	// TODO (HIGH) implement
	auto collapsed = clause |
		repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(
			simplify_tau<tau_ba<BAs...>, BAs...>
			| collapse_positives_tau<tau_ba<BAs...>, BAs...>);

	return compiled_gssotc_clause<BAs...>{};
}

template <typename...BAs>
compiled_gssotc<BAs...> compile(const gssotc<BAs...>& form) {
	auto dnf = form
		| repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(
			to_dnf_tau<tau_ba<BAs...>, BAs...>
			| simplify_tau<tau_ba<BAs...>, BAs...>);
	auto clauses = get_gssotc_clauses(dnf);
	for (auto& clause: clauses) {
		compile_clause(clause);
	}

	return compiled_gssotc<BAs...>{};
}

# endif //__EXECUTOR_H__