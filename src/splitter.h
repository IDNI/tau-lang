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

#ifndef SPLITTER_H
#define SPLITTER_H

#include "normalizer.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace idni::tau {

enum class split_sym {
	conjunction, disjunction
};

template<typename... BAs>
nso<BAs...> split(const nso<BAs...> &fm, const auto fm_type, const split_sym split_sym, const splitter_type st,
                  vector<nso<BAs...> > &mem, size_t i) {
	assert((fm_type == tau_parser::wff || fm_type == tau_parser::bf));
	const size_t clause_type = split_sym == split_sym::disjunction
		                           ? (fm_type == tau_parser::wff ? tau_parser::wff_or : tau_parser::bf_or)
		                           : (fm_type == tau_parser::wff ? tau_parser::wff_and : tau_parser::bf_and);
	auto is_clause_type = [&](nso<BAs...> n) { return is_non_terminal(clause_type, n); };
	// Return one clause if possible
	auto get_clause = [&clause_type](const nso<BAs...> &d) {
		// More then one DNF clause present
		assert(d->child.size() == 2);
		if (!is_non_terminal(clause_type, trim(d->child[1])))
			return d->child[1];
		else if (!is_non_terminal(clause_type, trim(d->child[0])))
			return d->child[0];
		else return d;
	};
	// Check if a child of "&&", "&", "|" or "||" is a clause
	auto without_clause = [&clause_type](const nso<BAs...> &d) {
		assert(d->child.size() == 2);
		return is_non_terminal(clause_type, trim(d->child[0])) &&
		       is_non_terminal(clause_type, trim(d->child[1]));
	};
	switch (st) {
	case splitter_type::upper: {
		// Remove only one clause
		if (mem.empty()) {
			mem = select_all(fm, is_clause_type);
			// Skip split_syms which are irrelevant
			mem.erase(remove_if(mem.begin(), mem.end(), without_clause), mem.end());
		}
		auto sym = clause_type == tau_parser::wff_or
			           ? _F<BAs...>
			           : (clause_type == tau_parser::wff_and
				              ? _T<BAs...>
				              : (clause_type == tau_parser::bf_or ? _0<BAs...> : _1<BAs...>));
		if (i < mem.size()) {
			map<nso<BAs...>, nso<BAs...> > changes = {{get_clause(mem[i]), sym}};
			auto res = replace(fm, changes);
			if (fm_type == tau_parser::wff)
				return res | repeat_all<step<BAs...>, BAs...>(simplify_wff<BAs...>);
			else return res | repeat_all<step<BAs...>, BAs...>(simplify_bf<BAs...>);
		}
		break;
	}
	case splitter_type::middle: {
		// Remove half of the clause
		if (mem.empty()) {
			mem = select_all(fm, is_clause_type);
			// Skip split_syms which are irrelevant
			mem.erase(remove_if(mem.begin(), mem.end(), without_clause), mem.end());
		}
		if (mem.size() > 1 && mem.size() / 2 + i <= mem.size()) {
			auto res = wrap(fm_type, mem[mem.size() - (mem.size() / 2) - i]);
			if (i > 0) {
				map<nso<BAs...>, nso<BAs...> > changes =
						{{mem[mem.size() - i], trim(get_clause(mem[mem.size() - i]))}};
				return replace(res, changes);
			} else return res;
		}
		if (mem.size() == 1 && i == 0) {
			return get_clause(mem[0]);
		}
		break;
	}
	case splitter_type::lower: {
		// Remove all but one clause
		if (mem.empty()) {
			mem = select_all(fm, is_clause_type);
			// Skip split_syms which are irrelevant
			mem.erase(remove_if(mem.begin(), mem.end(), without_clause), mem.end());
		}
		if (i < mem.size()) return get_clause(mem[i]);
		break;
	}
	}
	// No split_sym is present in fm or i is out of bound
	return fm;
}

// Find a Boolean function which implies f
template<typename... BAs>
nso<BAs...> good_splitter_using_function(const nso<BAs...> &f, splitter_type st, const nso<BAs...> &original_fm) {
	assert(is_non_terminal(tau_parser::bf, f));
	// First check if we have more then one disjunct
	vector<nso<BAs...> > m;
	size_t i = 0;
	do {
		auto s = split(f, tau_parser::bf, split_sym::disjunction, st, m, i);
		if (s != f) {
			map<nso<BAs...>, nso<BAs...> > changes = {{f, s}};
			auto new_fm = replace(original_fm, changes);
			/*if(!are_nso_equivalent(original_fm, new_fm))*/
			return new_fm;
		}
	} while (++i < m.size());

	// TODO: Try to split coefficient

	return original_fm;
}

// Find a Boolean function which is implied by f
template<typename... BAs>
nso<BAs...> good_reverse_splitter_using_function(const nso<BAs...> &f, splitter_type st,
                                                 const nso<BAs...> &original_fm) {
	assert(is_non_terminal(tau_parser::bf, f));
	// Convert Boolean function to CNF
	auto f_cnf = f | repeat_all<step<BAs...>, BAs...>(to_cnf_bf<BAs...>);

	vector<nso<BAs...> > m;
	size_t i = 0;
	do {
		auto s = split(f, tau_parser::bf, split_sym::conjunction, st, m, i);
		if (s != f) {
			map<nso<BAs...>, nso<BAs...> > changes = {{f, s}};
			auto new_fm = replace(original_fm, changes);
			/*if(!are_nso_equivalent(original_fm, new_fm))*/
			return new_fm;
		}
	} while (++i < m.size());

	// TODO: Try to reverse split coefficient

	return original_fm;
}

// Return a splitter for the provided formula
// We assume the formula is fully normalized by normalizer
template<typename... BAs>
nso<BAs...> tau_splitter(nso<BAs...> fm, splitter_type st) {
	// TODO: Add equivalence check once quantifier elimination is reliable
	fm = snf_wff(fm);
	// Collect all occurances of "||" while assuming that fm is in DNF
	auto or_occurances = select_all(fm, is_non_terminal<tau_parser::wff_or, BAs...>);
	vector<nso<BAs...>> clauses;
	// Extract clauses from occurances of "||"
	for (const auto& _or : or_occurances) {
		if (!is_non_terminal(tau_parser::wff_or, trim(_or->child[0])))
			clauses.push_back(_or->child[0]);
		if (!is_non_terminal(tau_parser::wff_or, trim(_or->child[1])))
			clauses.push_back(_or->child[1]);
	}
	// In case no disjunction is present
	if (clauses.empty()) clauses.push_back(fm);
	for (const auto &clause: clauses) {
		// check for equality parts
		auto eqs = select_top(clause, is_non_terminal<tau_parser::bf_eq, BAs...>);
		for (const auto &eq: eqs) {
			assert(is_non_terminal(tau_parser::bf_f, trim(eq->child[1])));
			auto f = eq->child[0];
			if (auto s = good_reverse_splitter_using_function(f, st, clause); s != clause) {
				map<nso<BAs...>,nso<BAs...>> c = {{clause, s}};
				return replace(fm, c);
			}
		}
		// check for inequality parts
		auto neqs = select_top(clause, is_non_terminal<tau_parser::bf_neq, BAs...>);
		for (const auto &neq: neqs) {
			assert(is_non_terminal(tau_parser::bf_f, trim(neq->child[1])));
			auto f = neq->child[0];
			if (auto s = good_splitter_using_function(f, st, clause); s != clause) {
				map<nso<BAs...>,nso<BAs...>> c = {{clause, s}};
				return replace(fm, c);
			}
		}
	}

	// Split disjunction if possible
	vector<nso<BAs...> > m;
	size_t i = 0;
	do {
		auto s = split(fm, tau_parser::wff, split_sym::disjunction, st, m, i);
		if (s != fm) return s;
	} while (++i < m.size());

	//if(!eq) {
	// TODO: Try Lemma 3.1
	//}

	// return bad splitter by conjuncting new uninterpreted constant
	stringstream ss;
	ss << "split" << get_new_uniter_const_id(fm);
	auto new_uniter_const = wrap(tau_parser::wff, build_wff_uniter_const<BAs...>(ss.str()));
	return build_wff_and(fm, new_uniter_const);
}
}
#endif //SPLITTER_H
