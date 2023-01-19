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

#ifdef DEBUG
ostream& operator<<(ostream& os, const struct anf& a);
#endif

struct anf : set<set<int_t>> {
	bool neg = false;
	anf() {}
	anf(bool pos) : neg(pos) {} // surprisingly enough
	anf(int_t t, bool pos = true) : neg(!pos) { insert({t}); }
	anf(const set<int_t>& s) { insert(s); }

	anf(const hbdd<Bool, true, true, false>& f) {
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
	
	anf operator|(const anf& x) const { return x + *this + (*this * x); }

	static set<int_t> union_(const set<int_t>& x, const set<int_t>& y) {
		set<int_t> r = x;
		return r.insert(y.begin(), y.end()), r;
	}

	anf operator*(const set<int_t>& v) const {
		anf r;
		for (auto& t : *this) r = r + union_(t, v);
		if (neg) {
			if (auto it = r.find(v); it == r.end()) r.insert(v);
			else r.erase(it);
		}
		return r;
	}

	anf operator*(const anf& x) const {
		anf r(false);
		for (auto& t : *this) r = r + (x * t);
		if (neg) r = r + x;
		return r;
	}

	hbdd<Bool, true, true, false> to_bdd() const {
		auto f = [](auto& t) {
			hbdd<Bool, true, true, false> f = bdd_handle<Bool, true, true, false>::htrue;
			for (auto x : t) f = f & bdd_handle<Bool, true, true, false>::bit(true, x);
			return f;
		};
		hbdd<Bool, true, true, false> g = bdd_handle<Bool, true, true, false>::hfalse;
		for (auto& t : *this) g = g + f(t);
		if (neg) g = ~g;
		return g;
	}

	void verify() const { assert(anf(to_bdd()) == *this); }

	anf subst(int_t v, const anf& f) const {
		auto g = [v, &f](auto& t) {
			anf r;
			r.neg = true;
			for (int_t i : t)
				if (i == v) r = r * f;
				else r = r * anf(i);
			return r;
		};
		anf r;
		for (auto& t : *this) r = r + g(t);
#ifdef DEBUG
		verify();
		r.verify();
		assert(r == anf(to_bdd()->subst(v, f.to_bdd())));
		if (!f.empty())
			assert(r == ((f * sub1(v)) | ((~f) * sub0(v)))),
			assert(r == ((f * sub1(v)) + ((~f) * sub0(v))));
#endif
		return r;
	}

	anf sub0(int_t v) const {
		anf r;
		for (auto& c : *this)
			if (c.find(v) == c.end())
				r.insert(c);
		DBG(assert(r == subst(v, anf(false)));)
		DBG(r.verify();)
		return r;
	}

	anf sub1(int_t v) const {
		anf r;
		for (auto c : *this) {
			if (auto it = c.find(v); it != c.end()) c.erase(it);
			if (!c.empty()) r = r + c;
		}
		DBG(assert(r == subst(v, anf(true)));)
		DBG(r.verify();)
		return r;
	}
};
#endif
