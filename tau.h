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
#ifndef __TAU_H__
#define __TAU_H__
#include <vector>
#include <set>
#include <utility>
#include <cstddef>
#include "babdd.h"
using namespace std;

struct Bool {
	bool b;
	Bool(bool b) : b(b) {}
	static const Bool& zero() { static Bool b(false); return b; }
	static const Bool& one() { static Bool b(true); return b; }
	Bool operator&(const Bool& x) const;
	Bool operator|(const Bool& x) const;
	Bool operator~() const;
	bool operator<(const Bool& x) const { return b < x.b; }
};

template<> struct zero<Bool> {
	bool operator()(const Bool& x) const { return !x.b; }
};

template<> struct one<Bool> {
	bool operator()(const Bool& x) const { return x.b; }
};

//template<> bool zero(const Bool& b) { return !b.b; }
//template<> bool one(const Bool& b) { return b.b; }

Bool Bool::operator&(const Bool& x) const {
	return ::zero<Bool>()(*this) ? zero() : x;
}
Bool Bool::operator|(const Bool& x) const {
	return ::one<Bool>()(*this) ? one() : x;
}
Bool Bool::operator~() const { return ::zero<Bool>()(*this) ? zero() : one(); }

template<typename B> struct clause : public pair<hbdd<B>, set<hbdd<B>>> {
	clause() {}
	clause(const hbdd<B>& f) { this->first = f; }

	static const clause<B>& zero() {
		static clause<B> r(bdd_handle<B>::htrue);
		return r;
	}

	static const clause<B>& one() {
		static clause<B> r(bdd_handle<B>::hfalse);
		return r;
	}

	clause<B>& operator+=(const hbdd<B>& f) {
		if (!::zero<B>()(f->get_uelim())) return *this = zero();
		this->first = (!this->first ? f : (this->first | f));
		return *this;
	}

	clause<B>& operator-=(const hbdd<B>& f) {
		return	::zero<hbdd<B>>()(f) ? *this = zero()
			: (this->second.insert(f), *this);
	}

	clause<B> operator&(clause<B> x) const {
		if (::zero<clause<B>>()(*this) || ::zero<clause<B>>()(x))
			return zero();
		if (::one<clause<B>>()(x)) return *this;
		if (::one<clause<B>>()(*this)) return x;
		x += this->first;
		for (const hbdd<B>& f : this->second) x -= f;
		return simplify(x), x;
	}

	set<clause<B>> operator~() const {
		set<clause<B>> r;
		clause<B> c;
		if (this->first) r.insert(c -= this->first);
		for (const hbdd<B>& f : this->second) 
			r.insert((c = clause<B>()) += f);
		return r;
	}

	bool ex(int_t v) {
		hbdd<B> f = this->first;
		if (hbdd<B> g = all(v, f); g != hbdd<B>::hfalse) return false;
		else this->first = g;
		set<clause<B>> s;
		for (hbdd<B> g : s) {
			g = (g->subst(v, f->sub0(v)) | g->subst(v, ~(f->sub1(v))));
			if (g == hbdd<B>::hfalse) return false;
			s.insert(g);
		}
		return this->second = s, true;
	}

	clause<B> subst(int_t s, const hbdd<B>& t) const {
		clause r;
		const hbdd<B>& f = this->first.subst(s, t);
		if (f == hbdd<B>::htrue) return zero();
		r += f;
		for (const hbdd<B>& g : this->second)
			if ((f = g.subst(s, t))->zero()) return zero();
			else r -= f;
		r.simplify();
	}

	void simplify() {
		if (!this->first || zero() || one()) return;
		set<hbdd<B>> s;
		for (const hbdd<B>& f : this->second)
			if (const hbdd<B>& g = f & ~this->first; g->zero())
				{ *this = zero(); return; }
			else s.insert(g);
		this->second = s;
	}
};

template<typename B> struct zero<clause<B>> {
	bool operator()(const clause<B>& c) const { return c.first->one(); }
};

template<typename B> struct one<clause<B>> {
	bool operator()(const clause<B>& c) const { return c.first->zero(); }
};

template<typename... BAs> struct tau :
	public tuple<set<clause<tau<BAs...>>>, set<clause<BAs>>...> {
	enum type { ZERO, ONE, NONE } t;
	tau() : t(NONE) {}
	tau(bool b) : t(b ? ONE : ZERO) {}
	template<typename B> tau(const set<clause<B>>& s) {
		get<set<clause<B>>>(*this) = s;
	}

	static const tau& zero() {
		static tau r(false);
		return r;
	}

	static const tau& one() {
		static tau r(true);
		return r;
	}

	template<typename B> tau subst(int_t s, const hbdd<B>& t) const {
		tau r;
		auto f = [&s, &t, &r](const auto& cs) {
			for (auto& c : cs) r |= c.subst(s, t);
		};
		f(get<0>(*this));
		(f(get<set<clause<BAs>>>(*this)), ...);
		return r;
	}

	template<typename B> tau<BAs...>& operator|=(const clause<B>& c) {
		if (::zero<clause<B>>()(c)) return *this;
		if (::one<clause<B>>()(c)) return *this = one();
		return t = NONE, get<set<clause<B>>>(*this).insert(c), *this;
	}
	
	template<typename B> tau<BAs...>& operator|=(const set<clause<B>>& s) {
		for (const clause<B>& c : s) *this |= c;
		return *this;
	}

	template<typename B> tau<BAs...>& operator&=(const clause<B>& c) {
		return (get<B>(*this) = (get<B>(*this) & c)), *this;
	}

	template<typename B> tau<BAs...>& operator|(const clause<B>& c) {
		tau t = *this;
		return t |= c;
	}

	template<typename B> tau<BAs...>& operator|(const set<clause<B>>& s) {
		tau t = *this;
		return t |= s;
	}

	tau operator|(tau x) const {
		auto f = [&x](auto& y) { x |= y; };
		f(get<0>(*this));
		(f(get<set<clause<BAs>>>(*this)), ...);
		return x;
	}

	tau operator&(const tau& x) const {
		return ~((~x) | ~*this); // TODO: better
//		auto f = [&x](auto& y) { x &= y; };
//		f(get<tau<BAs...>>(*this));
//		(f(get<BAs>(*this)), ...);
//		return x;
	}

	tau operator~() const {
		tau r(true);
		auto f = [&r](auto &x) { for (auto& y : x) r = r & tau(~y); };
		f(get<0>(*this));
		(f(get<set<clause<BAs>>>(*this)), ...);
		return r;
	}
};

template<typename... BAs> struct zero<tau<BAs...>> {
	bool operator()(const tau<BAs...>& t) const {
		return t.t == tau<BAs...>::ZERO;
	}
};

template<typename... BAs> struct one<tau<BAs...>> {
	bool operator()(const tau<BAs...>& t) const {
		return t.t == tau<BAs...>::ONE;
	}
};
#endif
