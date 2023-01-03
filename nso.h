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
#include "clauses.h"
#include <utility>
#include <cstddef>

template<typename... BAs> struct nso :
	public set<clauses<nso<BAs...>, BAs...>> {
	typedef set<clauses<nso<BAs...>, BAs...>> base;
	enum type { ZERO, ONE, NONE } t;

	nso(bool b) : t(b ? ONE : ZERO) {}
	template<typename B> nso(const set<clause<B>>& s) : t(NONE) {
		get<set<clause<B>>>(*this) = s;
	}

	static const nso& zero() {
		static nso r(false);
		return r;
	}

	static const nso& one() {
		static nso r(true);
		return r;
	}

	bool operator==(bool b) const { return t == (b ? ONE : ZERO); }

	bool operator==(const nso& x) const {
		return t == x.t && (base)(*this) == (base)(x);
	}

	bool operator<(const nso& x) const {
		return t != x.t ? t < x.t : ((base)(*this) < (base)(x));
	}

	template<typename B> void subst(int_t s, const hbdd<B>& t) {
		nso r = *this;
		this->clear();
		for (const clauses<BAs...>& x : r) *this |= x.subst(s, t);
	}

	template<typename B> nso<BAs...>& operator|=(const clause<B>& c) {
		if (c == false) return *this;
		if (c == true) return *this = one();
		return t = NONE, this->emplace(c), *this;
	}
	
	template<typename B> nso<BAs...>& operator|=(const set<clause<B>>& s) {
		for (const clause<B>& c : s) *this |= c;
		return *this;
	}

	template<typename B> nso<BAs...> operator&(const clause<B>& c) {
		if (t == ZERO) return *this;
		if (t == ONE) return *this |= c;
		nso r(true);
		for (const auto& cs : *this) r |= cs & c;
		return r;
	}

	template<typename B> nso<BAs...> operator|(const clause<B>& c) const {
		nso t = *this;
		return t |= c;
	}

	template<typename B>
	nso<BAs...> operator|(const set<clause<B>>& s) const {
		nso t = *this;
		return t |= s;
	}

	nso<BAs...>& operator|=(const clauses<nso, BAs...>& cs) {
		if (!cs.empty()) this->insert(cs);
		return t = NONE, *this;
	}

	nso operator|(const nso& x) const {
		if (x == true || *this == true) return one();
		if (x == false) return *this;
		if (*this == false) return x;
		nso r = x;
		for (auto& t : *this) r |= t;
		return r;
	}

	nso operator&(const nso& x) const {
		if (x == false || *this == false) return zero();
		if (x == true) return *this;
		if (*this == true) return x;
		nso r(false);
		for (auto& c : x)
			for (auto& d : *this)
				r |= (c & d);
		return r;
		return ~((~x) | ~*this); // TODO: better
	}

	template<typename... Ts>
	static nso to_nso(const tuple<set<clause<Ts>>...>& t) {
		nso r(false);
		auto f = [&r](auto& x) { r |= x; };
		(f(get<set<clause<Ts>>>(t)), ...);
		return r;
	}

	nso operator~() const {
		if (*this == false) return one();
		if (*this == true) return zero();
		nso r(true);
		for (clauses cs : *this) r = r & to_nso(~cs);
		return r;
	}

	template<typename B> nso& operator+=(const hbdd<B>& f) {
		return *this = (*this & (clause<B>() += f));
	}

	template<typename B> nso& operator-=(const hbdd<B>& f) {
		return *this = (*this & (clause<B>() -= f));
	}

	nso ex(int_t v) const {
		nso r(false);
		for (auto cs : *this) if (cs.ex(v)) r |= cs;
		return r;
	}

private:
	nso() : t(NONE) {}
};
#endif
