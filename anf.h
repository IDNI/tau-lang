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
#ifndef __ANF_H__
#define __ANF_H__
#include "bdd_handle.h"
#include "bool.h"
#include <set>
using namespace std;

struct anf : set<set<int_t>> {
	bool neg = false;
	anf() {}
	anf(int_t t, bool pos = true) : neg(!pos) { insert({t}); }
	anf(const hbdd<Bool>& f) {
		for (auto& c : f->dnf()) {
			assert(c.first == true);
			anf t;
			t.neg = true;
			for (int_t i : c.second)
				t = t * (i > 0 ? anf(i, true) : anf(-i, false));
			*this = *this + t;
		}
	}

	anf operator~() const {
		anf r = *this;
		return r.neg = !r.neg, r;
	}
	anf operator+(const anf& x) const {
		anf r;
		for (auto& v : *this) if (x.find(v) == x.end()) r.insert(v);
		for (auto& v : x) if (find(v) == end()) r.insert(v);
		r.neg = !(neg == x.neg);
		return r;
	}

	static set<int_t> union_(const set<int_t>& x, const set<int_t>& y) {
		set<int_t> r = x;
		for (int_t t : y) r.insert(t);
		return r;
	}

	anf operator*(const set<int_t>& v) const {
		anf r;
		for (auto& t : *this) r.insert(union_(t, v));
		if (neg) {
			if (auto it = r.find(v); it == r.end()) r.insert(v);
			else r.erase(it);
		}
		return r;
	}

	anf operator*(anf x) const {
		anf r;
		for (auto& t : *this) r = r + (x * t);
		if (neg) r = r + x;
		return r;
	}

	void verify() const {
		auto f = [](auto& t) {
			hbdd<Bool> f = bdd_handle<Bool>::htrue;
			for (auto x : t) f = f & bdd_handle<Bool>::bit(true, x);
			return f;
		};
		hbdd<Bool> g = bdd_handle<Bool>::hfalse;
		for (auto& t : *this) g = g + f(t);
		if (neg) g = ~g;
		/*anf h;
		for (auto c : g->dnf()) {
			assert(c.first == true);
			anf t;
			t.neg = true;
			for (int_t i : c) t = t * anf(i, i > 0);
			h = h + t;
		}*/
		assert(anf(g) == *this);
	}
};
#endif
