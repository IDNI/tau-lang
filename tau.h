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
	template<typename T> Bool(const T&) {
		static_assert(is_same<T, bool>::value);
	}
	static const Bool& zero() { static Bool b(false); return b; }
	static const Bool& one() { static Bool b(true); return b; }
	Bool operator&(const Bool& x) const;
	Bool operator|(const Bool& x) const;
	Bool operator~() const;
	bool operator<(const Bool& x) const { return b < x.b; }
	bool operator==(bool t) const { return b == t; }
};

//template<> bool zero(const Bool& b) { return !b.b; }
//template<> bool one(const Bool& b) { return b.b; }

Bool Bool::operator&(const Bool& x) const {
	return (*this == false) ? zero() : x;
}
Bool Bool::operator|(const Bool& x) const {
	return (*this == true) ? one() : x;
}

Bool Bool::operator~() const { return *this == true ? zero() : one(); }

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

	void to_zero() { *this = zero(); }
	void to_one() { *this = one(); }

	bool operator==(bool b) const {
		return this->first && !(this->first == b);
	}

	clause<B>& operator+=(const hbdd<B>& f) {
		if (!(f->get_uelim() == false)) return *this = zero();
		this->first = (!this->first ? f : (this->first | f));
		return *this;
	}

	clause<B>& operator-=(const hbdd<B>& f) {
		return	f == false ? *this = zero()
			: (this->second.insert(f), *this);
	}

	clause<B> operator&(clause<B> x) const {
		if (*this == false || x == false)
			return zero();
		if (x == true) return *this;
		if (*this == true) return x;
		x += this->first;
		for (const hbdd<B>& f : this->second) x -= f;
		return x.simplify(), x;
	}

	set<clause<B>> operator~() const {
		set<clause<B>> r;
		clause<B> c;
		if (this->first) r.insert(c -= this->first);
		for (const hbdd<B>& f : this->second) 
			r.insert(clause<B>() += f);
			//r.insert((c = clause<B>()) += f);
		return r;
	}

	bool ex(int_t v) {
		hbdd<B> f = this->first;
		if (hbdd<B> g = all(v, f); g != hbdd<B>::hfalse) return false;
		else this->first = g;
		set<clause<B>> s;
		for (hbdd<B> g : s) {
			g =	(g->subst(v, f->sub0(v)) |
				g->subst(v, ~(f->sub1(v))));
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
		if (!this->first || *this == true || *this == false) return;
		set<hbdd<B>> s;
		for (const hbdd<B>& f : this->second)
			if (const hbdd<B>& g = f & ~this->first; g->zero())
				{ *this = zero(); return; }
			else s.insert(g);
		this->second = s;
	}
};

template<typename... BAs> struct clauses : public tuple<clause<BAs>...> {
	clauses() {}
	clauses(bool b) {
		auto f = [&b](auto& x) { b ? x.to_one() : x.to_zero(); };
		(f(get<BAs>(*this)), ...);
	}
	template<typename B> clauses(const clause<B>& c) {
		get<clause<B>>(*this) = c;
	}

	template<typename B> void put(const clause<B>& c) {
		if (c == false) clear_all();
		get<clause<B>>(*this) = (c == true) ? clause<B>() : c;
	}

	template<typename B> clauses& clear() { put(clause<B>()); return *this; }
	void clear_all() { (clear<BAs>(), ...); }

	template<typename B> clauses& subst(int_t v, const hbdd<B>& t) {
		return put(get<clause<B>>(*this).subst(v, t)), *this;
	}

	template<typename B> clauses& operator&=(const clause<B>& c) {
		return put(get<clause<B>>(*this) & c), *this;
	}

	clauses operator&(clauses cs) const {
		auto f = [&cs](auto x) { cs &= x; };
		(f(get<clause<BAs>>(*this)), ...);
		return cs;
	}

	template<typename B>
	static void put(tuple<set<clause<BAs>>...>& t,
			const set<clause<B>>& s) {
		get<set<clause<B>>>(t) = s;
	}

	tuple<set<clause<BAs>>...> operator~() const {
		tuple<set<clause<BAs>>...> r;
		auto f = [&r](auto& x) { put(r, ~x); };
		(f(get<clause<BAs>>(*this)), ...);
		return r;
	}
};

template<typename... BAs> struct tau :
	public set<clauses<tau<BAs...>, BAs...>> {
	//typedef tuple<clause<tau<BAs...>>, clause<BAs>...> ct; // clauses tuple
	enum type { ZERO, ONE, NONE } t;

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

//	template<typename B> void remove() {
//		tau r = *this;
//		this->clear();
//		for (ct t : r) *this |= t.put(clause<B>());
//	}

	bool operator==(bool b) const { return t == (b ? ONE : ZERO); }

	template<typename B> void subst(int_t s, const hbdd<B>& t) {
		tau r = *this;
		this->clear();
		for (const clauses<BAs...>& x : r) *this |= x.subst(s, t);
//		set<clause<B>> cs;
//		for (const clauses& x : *this) cs.insert(get<clause<B>>(x));
//		remove<B>;
//		for (const clause<B>& c : cs) *this |= c.subst(s, t);
	}

	template<typename B> tau<BAs...>& operator|=(const clause<B>& c) {
		if (c == false) return *this;
		if (c == true) return *this = one();
		return t = NONE, this->emplace(c), *this;
	}
	
	template<typename B> tau<BAs...>& operator|=(const set<clause<B>>& s) {
		for (const clause<B>& c : s) *this |= c;
		return *this;
	}

	template<typename B> tau<BAs...> operator&(const clause<B>& c) {
		if (t == ZERO) return *this;
		if (t == ONE) return *this |= c;
		tau r(true);
		for (const auto& cs : *this) r.insert(cs & c);
		return r;
	}

	template<typename B> tau<BAs...> operator|(const clause<B>& c) const {
		tau t = *this;
		return t |= c;
	}

	template<typename B>
	tau<BAs...> operator|(const set<clause<B>>& s) const {
		tau t = *this;
		return t |= s;
	}

	tau<BAs...>& operator|=(const clauses<tau, BAs...>& t) {
		auto f = [this](auto& x) { *this |= x; };
		*this |= get<0>(t);
		(f(get<clause<BAs>>(t)), ...);
		return *this;
	}

	tau operator|(tau x) const {
		for (auto& t : *this) x |= t;
		return x;
	}

	tau operator&(const tau& x) const {
		return ~((~x) | ~*this); // TODO: better
	}

//	static tau neg(const ct& t) {
//		tau r;
//		auto f = [&r](auto c) { r |= ~c; };
//		(f(get<clause<BAs>>(t)), ...);
//		return r;
//	}

	template<typename... Ts>
	static tau to_tau(const tuple<set<clause<Ts>>...>& t) {
		tau r(false);
		auto f = [&r](auto& x) { r |= x; };
		(f(get<set<clause<Ts>>>(t)), ...);
		return r;
	}

	tau operator~() const {
		tau r(true);
		for (clauses cs : *this) r = r & to_tau(~cs);
		return r;
	}

	template<typename B> tau& operator+=(const hbdd<B>& f) {
		return *this = (*this & (clause<B>() += f));
	}

	template<typename B> tau& operator-=(const hbdd<B>& f) {
		return *this = (*this & (clause<B>() -= f));
	}
private:
	tau() : t(NONE) {}
};
#endif
