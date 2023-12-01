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
#ifndef __SEQ_H__
#define __SEQ_H__
#include "nso.h"
#include <iostream>
#define ever ;;

template<typename... BAs> struct seq {
	typedef nso<BAs...> nso_t;
	nso_t pos;
	vector<nso_t> neg;
	typedef map<size_t, nso_t> state;
	map<size_t s, nso_t> cache;

	template<typename T> seq(const nso_t& pos, const T& neg) :
		pos(pos), neg(t.begin(), t.end()) {}

	nso_t negs(size_t s) const {
		if (one_bit(s)) return vec[the_bit(s)];
		if (auto it = cache.find(s) != it->end()) return it->second;
		return cache[s] = (negs(del_lsb(s)) & vec[get_lsb(s)]);
	}

	void iter(fsyms fs, auto q, size_t s,
		const state& prev, state& curr) const {
		nso_t &f = (curr[s] = prev.at(s) & this->first);
		for_each_subset_dec(s, [&f](size_t s) {
				f = f & (prev.at(s) | ~psi(s));
		});
		f = q(f);
	}

	static tau<BAs...> quant(
		const vector<tau_lit<BAs...>>& v, vector<set<int_t>> q) {
		tau<BAs...> f(true);
		for (auto& x : v)
			if (v.neg) f = f & ~v.f;
			else f = f & v.f;
		size_t n = q.size();
		bool ex = true;
		while (n--) {
			for (int_t x : q[n]) f = ex ? f.ex(x) : f.all(x);
			ex = !ex;
		}
		return f;
	}

};
