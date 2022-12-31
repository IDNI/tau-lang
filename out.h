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
#ifndef __OUT_H__
#define __OUT_H__
#include "tau.h"
#include <ostream>

ostream& operator<<(ostream& os, const Bool& b) { return os << (b.b ? 1 : 0); }

template<typename B> ostream& operator<<(ostream& os, const hbdd<B>& f) {
	set<pair<B, vector<int_t>>> dnf = f->dnf();
	size_t n = dnf.size();
	for (auto& c : dnf) {
		assert(!zero<B>()(c.first));
		if (!one<B>()(c.first)) os << '{' << c.first << '}';
		for (int_t v : c.second)
			if (v < 0) os << "x[" << -v << "]'";
			else os << "x[" << v << ']';
		if (--n) os << " | ";
	}
	return os;
}

template<typename B> ostream& operator<<(ostream& os, const clause<B>& c) {
	if (c.first) os << c.first << " = 0" << endl;
	for (const hbdd<B>& f : c.second) os << f << " != 0" << endl;
	return os;
}

template<typename B> ostream& operator<<(ostream& os, const set<clause<B>>& s) {
	for (auto& c : s) os << c;
	return os;
}

template<typename... Ts>
ostream& operator<<(ostream& os, const tuple<Ts...>& t) {
	auto f = [&os](auto x) { os << x; };
	(f(get<Ts>(t)), ...);
	return os;
}
#endif
