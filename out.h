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
#include "barr.h"
#include "dict.h"
#include "bool.h"
#include "anf.h"
#include <ostream>
#include <sstream>

ostream& operator<<(ostream& os, const Bool& b) { return os << (b.b ? 1 : 0); }

template<typename B> ostream& operator<<(ostream& os, const hbdd<B>& f) {
	if (f == bdd_handle<B>::htrue) return os << '1';
	if (f == bdd_handle<B>::hfalse) return os << '0';
	set<pair<B, vector<int_t>>> dnf = f->dnf();
	size_t n = dnf.size();
	set<string> ss;
	for (auto& c : dnf) {
		set<string> s;
		assert(!(c.first == false));
		stringstream t;
		if (!(c.first == true)) t << '{' << c.first << '}';
		for (int_t v : c.second)
			if (v < 0) s.insert(string(dict(-v)) + "'"s);
			else s.insert(dict(v));
		for (auto& x : s) t << x;
		ss.insert(t.str());
	}
	for (auto& s : ss) {
		os << s;
		if (--n) os << " | ";
	}
	return os;
}

// converting this to operator<< gives weird compile error
template<typename... BDDs, typename... aux>
ostream& out(ostream& os, const msba<tuple<BDDs...>, aux...>& m) {
	typedef msba<tuple<BDDs...>, aux...> msba_t;
	if (m.b == false) return os << 'F' << endl;
	if (m.b == true) return os << 'T' << endl;
	set<pair<Bool, vector<int_t>>> dnf = m.b->dnf();
	size_t n = dnf.size();
	set<string> ss;
	stringstream t;
	auto f = [&t](const auto& x) { t << x; };
	for (const auto& c : dnf) {
		for (int_t v : c.second)
			visit(f, msba_t::V[abs(v)]),
			t << (v > 0 ? " = 0" : " != 0") << endl;
		ss.insert(t.str());
		t = stringstream();
	}
	for (auto& s : ss) {
		os << s;
		if (--n) os << endl << " || " << endl;
	}
	return os;
}

/*template<typename... BAs>
ostream& operator<<(ostream& os, const tau<BAs...>& t) {
	if (t == false) return os << 'F';
	if (t == true) return os << 'T';
	DBG(assert(!t.empty());)
	size_t n = t.size();
	for (auto& c : t) if (os << c; --n) os << " || " << endl;
	return os;
}*/

ostream& operator<<(ostream& os, const anf& a) {
	size_t n = a.size();
	set<string> ss;
	for (auto& x : a) {
		set<string> s;
		for (auto y : x) s.insert(dict(y));
		string t;
		for (const string& y : s) t += y;
		ss.insert(t);
	}
	for (const string& s : ss) {
		os << s;
		if (--n) os << " + ";
	}
	return os;
}
#endif
