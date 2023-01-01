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
#include "dict.h"
#include "bool.h"
#include <ostream>

ostream& operator<<(ostream& os, const Bool& b) { return os << (b.b ? 1 : 0); }

template<typename B> ostream& operator<<(ostream& os, const hbdd<B>& f) {
	set<pair<B, vector<int_t>>> dnf = f->dnf();
	size_t n = dnf.size();
	for (auto& c : dnf) {
		assert(!(c.first == false));
		if (!(c.first == true)) os << '{' << c.first << '}';
		for (int_t v : c.second)
			if (v < 0) os << dict(-v) << '\'';
			else os << dict(v);
		if (--n) os << " | ";
	}
	return os;
}

template<typename B> ostream& operator<<(ostream& os, const clause<B>& c) {
	if (c.first) os << c.first << " = 0" << endl;
	for (const hbdd<B>& f : c.second) os << f << " != 0" << endl;
	return os;
}

template<typename... Ts>
ostream& operator<<(ostream& os, const tuple<Ts...>& t) {
	auto f = [&os](auto x) { os << x; };
	(f(get<Ts>(t)), ...);
	return os;
}

template<typename... BAs>
ostream& operator<<(ostream& os, const tau<BAs...>& t) {
	size_t n = t.size();
	for (auto& c : t) {
		os << c;
		if (--n) os << " || " << endl;
	}
	return os;
}
#endif
