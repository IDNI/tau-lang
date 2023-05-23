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

#ifndef __NORMALIZER_H__
#define __NORMALIZER_H__

#include "msba.h"
#include "bdd_handle.h"

template<typename... BDDs, typename... aux>
struct normalizer<tuple<BDDs...>, aux...> {
	typedef msba<tuple<BDDs...>, aux...> msba_t;
	typedef tuple<array<set<BDDs>, 2>..., array<set<aux>, 2>...> splitted;

	template<typename T> using arr_t = array<set<remove_cvref_t<T>>, 2>;

	static splitted to_tuple(
		const vector<int_t>& pos, const vector<int_t>& neg) {
		splitted r;
		auto f = [&r](const auto& x) {
			std::get<arr_t<decltype(x)>>(r)[0].insert(x); };
		auto g = [&r](const auto& x) {
			std::get<arr_t<decltype(x)>>(r)[1].insert(x); };
		for (int_t i : pos) visit(f, msba_t::V[i]);
		for (int_t i : neg) visit(g, msba_t::V[i]);
		return r;
	}

	template<typename B>
	static msba_t normalize(const set<B>& pos, const set<B>& neg) {
//		for (const B& x : pos)
//			for (const B& y : neg)
//				if ((x & y) == y)
//					return msba_t(false);
		msba_t r(true);
		for (const auto& x : pos) r = (r & msba_t(true, x));
		for (const auto& x : neg) r = (r & msba_t(false, x));
		return r;
	}

	template<typename B, auto o = bdd_options<>::create()>
	static msba_t normalize( const set<hbdd<B, o>>& pos,
		const set<hbdd<B, o>>& neg) {
		for (auto x : pos)
			for (auto y : neg)
				if ((x & y) == y)
					return msba_t(false);
		hbdd<B, o> p = ~get_one<hbdd<B, o>>();
		for (const auto& x : pos) p = (p | x);
		msba_t r(true, p);
		hbdd<B, o> np = ~p;
		if (!(p->get_uelim() == false)) return msba_t(false); 
		auto t = p->lgrs();
		for (const hbdd<B, o>& x : neg) {
			hbdd<B, o> z = x->compose(t) & np;
			//hbdd<B> z = (x & np);
			if (!(z->get_uelim() == false)) continue;
			else if ((r = (r & msba_t(false, z))) == false) return r;
		}
		return r;
	}

	static msba_t normalize(const splitted& t) {
		msba_t r(true);
		auto f = [&r](const auto& x) {
			if (auto t = normalize(x[0], x[1]); t == false) throw 0;
			else r = (r & t);
		};
		try {	(f(std::get<array<set<BDDs>, 2>>(t)), ...);
			return r;
		} catch (...) { return sbf_F; }
	}
};
#endif
