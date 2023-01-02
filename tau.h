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

template<typename... BAs> struct tau :
	public set<clauses<tau<BAs...>, BAs...>> {
	typedef set<clauses<tau<BAs...>, BAs...>> base;
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

	bool operator==(bool b) const { return t == (b ? ONE : ZERO); }

	bool operator==(const tau& x) const {
		return t == x.t && (base)(*this) == (base)(x);
	}

	template<typename B> void subst(int_t s, const hbdd<B>& t) {
		tau r = *this;
		this->clear();
		for (const clauses<BAs...>& x : r) *this |= x.subst(s, t);
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
		for (const auto& cs : *this) r |= cs & c;
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

	tau<BAs...>& operator|=(const clauses<tau, BAs...>& cs) {
		if (!cs.empty()) this->insert(cs);
		return t = NONE, *this;
	}

	tau operator|(tau x) const {
		for (auto& t : *this) x |= t;
		return x;
	}

	tau operator&(const tau& x) const {
		if (x == false || *this == false) return zero();
		if (x == true) return *this;
		if (*this == true) return x;
		tau r(false);
		for (auto& c : x)
			for (auto& d : *this)
				r |= (c & d);
		return r;
		return ~((~x) | ~*this); // TODO: better
	}

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

	tau ex(int_t v) const {
		tau r(false);
		for (auto cs : *this) if (cs.ex(v)) r |= cs;
		return r;
	}

private:
	tau() : t(NONE) {}
};
#endif
