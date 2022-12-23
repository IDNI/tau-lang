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
#ifndef __BA_H__
#define __BA_H__
#define DEBUG

#ifdef DEBUG
#define DBG(x) x
#include <iostream>
#else
#define DBG(x)
#endif

#include <vector>
#include <map>
#include <set>
#include <ostream>
#include <array>
#include <string>
#include <sstream>
#include <cassert>
#include <functional>

using namespace std;

//typedef string sym_t;
typedef int sym_t;

struct Bool {
	bool b;
	Bool() {}
	Bool(bool b) : b(b) {}
	static const Bool& zero() { static Bool b(false); return b; }
	static const Bool& one() { static Bool b(true); return b; }
	const Bool& operator&(const Bool& t) const {return(b&t.b)?one():zero();}
	const Bool& operator|(const Bool& t) const {return(b|t.b)?one():zero();}
	const Bool& operator~() const { return b ? zero() : one(); }
	bool operator<(const Bool& t) const { return b < t.b; }
	bool operator==(const Bool& t) const { return b == t.b; }
};

template<typename B> struct term;
template<typename B> struct bf;

template<typename B> struct term {
	struct arg {
		bool ist;
		term<B> t;
		bf<B> f;
		arg() {}
		arg(const term& t);
		arg(const bf<B>& f);
		bool operator==(const arg&) const;
		bool operator<(const arg&) const;
		arg subst(const sym_t& s, const bf<B>& f) {
			return ist ? arg(t.subst(s, f)) : *this;
		}
	};
	enum type { ELEM, VAR, BF, FUNC } t;
	//int sym = 0;
	sym_t sym;
	string name;
	B e;
	bf<B> f;
	vector<arg> args;
	term() {}
	term(const B& e) : t(ELEM), e(e) {}
	term(type t) : t(t) {}
	term(const sym_t& sym) : t(VAR), sym(sym) {}
	//term(const sym_t& sym, const vector<arg>& a) :
	//	t(FUNC), sym(sym), args(a) {}
	term(const string& name, const vector<arg>& a) :
		t(FUNC), name(name), args(a) {}
	term(const bf<B>& g) {
		if (g.v == bf<B>::ZERO) { t = ELEM; e = B::zero(); return; }
		if (g.v == bf<B>::ONE) { t = ELEM; e = B::one(); return; }
		assert(!g.empty());
		if (g.size() == 1) {
			auto& c = *g.begin();
			if (c[1].empty() && c[0].size() == 1)
				*this = *c[0].begin();
			return;
		}
		t = BF;
		f = g;
	}
	bool operator==(const term& x) const;
	bool operator<(const term& x) const;
	term subst(const sym_t&, const bf<B>&) const;
	bool zero() const {
		switch (t) {
			case ELEM: return e == B::zero();
			case BF: return f.v == bf<B>::ZERO;
			default: return false;
		}
	}
	bool one() const {
		switch (t) {
			case ELEM: return e == B::one();
			case BF: return f.v == bf<B>::ONE;
			default: return false;
		}
	}
};

//template<typename B> struct minterm : public array<set<term<B>>, 2> {
template<typename B> struct minterm : public array<set<term<B>>, 2> {
	typedef array<set<term<B>>, 2> base;

	minterm() : base() {}
	minterm(bool pos, const term<B>& t) : base() { add(pos, term<B>(t)); }
	void add(bool, const term<B>&);
	bf<B> subst(const sym_t& s, const bf<B>& f) const;
};

//template<typename B> struct bf : public set<minterm<B>> {
template<typename B> struct bf : public set<minterm<B>> {
	enum { ZERO, ONE, NONE } v;

	bf() : set<minterm<B>>(), v(NONE) {}
	bf(const term<B>& t) : set<minterm<B>>({minterm<B>(true, t)}),
		v(NONE) {}
	bf(const minterm<B>& t) : set<minterm<B>>({t}), v(NONE) {}
	bf(bool b) { v = b ? ONE : ZERO; }

	bool operator==(const bf<B>& f) const;
	bool operator<(const bf<B>& f) const;
	static const bf<B>& zero();
	static const bf<B>& one();
	bf subst(const sym_t& s, const bf<B>& y) const;
};

template<typename B> term<B>::arg::arg(const term<B>& t) :
	ist(true), t(t) {}

template<typename B> term<B>::arg::arg(const bf<B>& f) :
	ist(false), f(bf<B>(f)) {}

template<typename B> bool term<B>::arg::operator==(
		const term<B>::arg& a) const {
	return ist != a.ist ? false : ist ? t == a.t : f == a.f;
}

template<typename B> bool term<B>::arg::operator<(
		const term<B>::arg& a) const {
	return a.ist != ist ? ist : ist ? a.t < t : a.f < f;
}

template<typename B> bool term<B>::operator==(const term& x) const {
	if (t != x.t) return false;
	switch (t) {
		case ELEM: return e == x.e;
		case VAR: return sym == x.sym;
		case BF: return f == x.f;
		case FUNC: return name == x.name && args == x.args;
		default: assert(0);
	}
}

template<typename B> bool term<B>::operator<(const term& x) const {
	if (t != x.t) return t < x.t;
	switch (t) {
		case ELEM: return e < x.e;
		case VAR: return sym < x.sym;
		case BF: return f < x.f;
		case FUNC: return name == x.name ? args < x.args : name < x.name;
		default: assert(0);
	}
}

template<typename B> void minterm<B>::add(bool pos, const term<B>& t) {
	assert(!(t.t == term<B>::ELEM && t.e == B::zero()));
	(*this)[pos ? 0 : 1].insert(t);
}

template<typename B> bool bf<B>::operator==(const bf<B>& f) const {
	if (f.v != v) return false;
	return (set<minterm<B>>)(*this) == (set<minterm<B>>)f;
}

template<typename B> bool bf<B>::operator<(const bf<B>& f) const {
	if (f.v != v) return v < f.v;
	return (set<minterm<B>>)(*this) < (set<minterm<B>>)f;
}

template<typename B> const bf<B>& bf<B>::zero() {
	static bf<B> z(false);
	return z;
}

template<typename B> const bf<B>& bf<B>::one() {
	static bf<B> z(true);
	return z;
}

template<typename B> bf<B> operator~(const minterm<B>& x) {
	bf<B> f = bf<B>::zero();
	for (const term<B>& t : x[0])
		f = f | bf<B>(minterm(false, t));
	for (const term<B>& t : x[1])
		f = f | bf<B>(minterm(true, t));
	return f;
}

template<typename B> minterm<B> operator&(
		const minterm<B>& x, const minterm<B>& y) {
	//DBG(cout << x << "&&" << y << " = ";)
#ifdef DEBUG
	for (const term<B>& t : x[0]) assert(!t.zero() && !t.one());
	for (const term<B>& t : x[1]) assert(!t.zero() && !t.one());
	for (const term<B>& t : y[0]) assert(!t.zero() && !t.one());
	for (const term<B>& t : y[1]) assert(!t.zero() && !t.one());
#endif
	//cout << "(" << x << ") & (" << y << ") = " << endl;
	minterm<B> z = x;
	for (const term<B>& t : y[0])
		if (auto it = x[1].find(t); it != x[1].end())
			return minterm<B>();
		else z[0].insert(t);
	for (const term<B>& t : y[1])
		if (auto it = x[0].find(t); it != x[0].end())
			return minterm<B>();
		else z[1].insert(t);
	//DBG(cout << z << endl;)
	return z;
}

template<typename B>
bool operator<=(const minterm<B>& x, const minterm<B>& y) {
	return (x & y) == x;
}

template<typename B>
bool operator<=(const minterm<B>& t, const bf<B>& f) {
	if (f == bf<B>::one()) return true;
	for (const minterm<B>& x : f) if (t <= x) return true;
	return false;
}

template<typename T>
T symdiff(const T& x, const T& y) {
	T r;
	for (auto& t : x) if (y.find(t) == y.end()) r.insert(t);
	for (auto& t : y) if (x.find(t) == x.end()) r.insert(t);
	return r;
}

template<typename B>
bool complementary(const minterm<B>& x, minterm<B>& y) {
	set<term<B>> d0 = symdiff(x[0], y[0]);
	if (d0.size() != 1) return false;
	set<term<B>> d1 = symdiff(x[1], y[1]);
	if (d1.size() != 1) return false;
	if (*d0.begin() != *d1.begin()) return false;
	if (auto it = y[0].find(*d0.begin()); it != y[0].end())
		y[0].erase(it);
	else y[1].erase(*d0.begin());
	return true;
}

template<typename B>
bf<B> disj_fmt(const minterm<B>& t, const bf<B>& f) {
	if (t[0].empty() && t[1].empty()) return bf<B>(true);// f;
	if (f == bf<B>::one()) return f;
	if (f == bf<B>::zero() || f.empty()) return bf<B>(t);
	for (const term<B>& x : t[0])
		if (x.t == term<B>::ELEM && x.e == B::zero()) return f;
		else if (x.t == term<B>::BF) {
			minterm<B> s = t;
			return s[0].erase(x), s & (f & x.f);
		} //else assert(x.t != term<B>::ELEM);
	for (const term<B>& x : t[1])
		if (x.t == term<B>::ELEM && x.e == B::one()) return f;
		else if (x.t == term<B>::BF) {
			minterm<B> s = t;
			return s[1].erase(x), s & (f & ~x.f);
		} //else assert(x.t != term<B>::ELEM);
	for (const minterm<B>& x : f) if (t <= x) return f;
	if (t[0].size() == 1 && t[1].empty())
		for (const minterm<B>& m : f)
			if (	m[1].size() == 1 && m[0].empty() &&
				*m[1].begin() == *t[0].begin())
				return bf<B>(true);
	if (t[1].size() == 1 && t[0].empty())
		for (const minterm<B>& m : f)
			if (	m[0].size() == 1 && m[1].empty() &&
				*m[0].begin() == *t[1].begin())
				return bf<B>(true);
	bf g = f;
	auto s = t;
	auto it = g.begin();
	while (it != g.end())
		if (complementary<B>(*it, s)) break;
		else ++it;
	if (it != g.end()) return g.erase(it), s | g;
	return g.insert(s), g;
}

template<typename B>
bf<B> operator|(const minterm<B>& t, const bf<B>& f) {
	return disj_fmt<B>(t, f);
}

template<typename B> bf<B> operator~(const bf<B>& f) {
	bf g = bf<B>::one();
	for (const minterm<B>& t : f) g = g & ~t;
	return g;
}

template<typename B> bf<B> operator&(const minterm<B>& x, const bf<B>& y) {
	if (y == bf<B>::zero()) return y;
	if (y == bf<B>::one()) return bf<B>(x);
	//cout << "(" << x << ") & (" << y << ") = ";
	bf<B> z;
	for (const minterm<B>& t : y)
//		if (minterm<B> m = (x & t); m.empty()) return bf<B>::zero();
		if (minterm<B> m = (x & t); !m.empty()) 
	//	else 
			z = m | z;
	//cout << z << endl;
	return z.empty() ? bf<B>(false) : z;
}

template<typename B> bf<B> operator&(const bf<B>& x, const bf<B>& y) {
	if (x == bf<B>::zero() || y == bf<B>::zero()) return bf<B>::zero();
	if (x == bf<B>::one()) return y;
	if (y == bf<B>::one()) return x;
	bf<B> z = bf<B>::zero();
	for (const minterm<B>& s : x)
		for (const minterm<B>& t : y) {
			minterm<B> m = s & t;
			if (!m[0].empty() || !m[1].empty()) z = (m | z);
		}
	return z;
}

template<typename B> bf<B> operator|(const bf<B>& x, const bf<B>& y) {
	if (x == bf<B>::zero()) return y;
	if (y == bf<B>::zero()) return x;
	if (x == bf<B>::one() || y == bf<B>::one()) return bf<B>::one();
	bf<B> z = x;
	for (const minterm<B>& t : y) z = t | z;
	return z;
}

template<typename B> bool operator<=(const bf<B>& x, const bf<B>& y) {
	for (const minterm<B>& t : x) if (!(t <= y)) return false;
	return true;
}

template<typename B>
term<B> term<B>::subst(const sym_t& s, const bf<B>& g) const {
	if (t == term<B>::VAR) return s == sym ? term<B>(g) : *this;
	if (t == term<B>::BF) return f.subst(s, g);
	assert(t == term<B>::FUNC);
	term r = *this;
	for (size_t n = 0; n != r.args.size(); ++n)
		r.args[n] = r.args[n].subst(s, g);
	return r;
}

template<typename B>
bf<B> minterm<B>::subst(const sym_t& s, const bf<B>& f) const {
	bf<B> r(true);
	for (const term<B>& x : (*this)[0])
		if (term<B> t = x.subst(s, f); t.zero()) return bf<B>(false);
		else if (!t.one()) r = minterm<B>(true, t) & r;
	for (const term<B>& x : (*this)[1])
		if (term <B> t = x.subst(s, f); t.one()) return bf<B>(false);
		else if (!t.zero()) r = minterm<B>(false, t) & r;
	return r;
}

template<typename B>
bf<B> bf<B>::subst(const sym_t& s, const bf<B>& y) const {
	bf<B> z(false);
	for (const minterm<B>& t : *this) z = t.subst(s, y) | z;
	return z;
}

/*template<typename B>
term<B> subst(const term<B>& t,
	function<bf<B>(const bf<B>&)> &f, function<term<B>(const term<B>)>& g) {
	switch (t.t) {
		
	}
	enum type { ELEM, VAR, BF, FUNC } t;
}

template<typename B>
bf<B> subst(const bf<B>& x,
	function<bf<B>(const bf<B>&)> &f, function<term<B>(const term<B>)>& g) {
	bf<B> r;
	for (const minterm<B>& y : x) {
		minterm<B> m;
		for (const term<B>& t : x[0])
			m[0] = m[0] & minterm<B>(subst(t, f, g));
		for (const term<B>& t : x[1])
			m[1] = m[1] & minterm<B>(subst(t, f, g));
		r = m | r;
	}
	return r;
}*/

template<typename B> bf<B> ex(const bf<B>& f, const sym_t& v) {
	return f.subst(v, bf<B>::zero()) | f.subst(v, bf<B>::one());
}

template<typename B> bf<B> all(const bf<B>& f, const sym_t& v) {
	//cout << "in all" << endl;
	//auto f0 = f.subst(v, bf<B>::zero()), f1 = f.subst(v, bf<B>::one());
	//cout << "subst 0 for " << v << " in " << f << " = " << f0 << endl;
	//cout << "subst 1 for " << v << " in " << f << " = " << f1 << endl;
	//cout << "their conj: " << (f0 & f1) << endl;
	return f.subst(v, bf<B>::zero()) & f.subst(v, bf<B>::one());
}

ostream& operator<<(ostream& os, const Bool& b) { return os << (b.b?"T":"F"); }

template<typename B>
ostream& operator<<(ostream& os, const typename term<B>::arg& a) {
	return a.ist ? os << a.t : os << a.f;
}

template<typename B>
void out(ostream& os, const typename term<B>::arg& a) {
	if (a.ist) os << a.t; else os << a.f;
}

template<typename B>
ostream& operator<<(ostream& os, const term<B>& t) {
	if (t.t == term<B>::ELEM) return os << t.e;
	if (t.t == term<B>::VAR) return os << "x[" << t.sym << "]";
	if (t.t == term<B>::BF) return os << t.f;
	if (t.t == term<B>::FUNC) os << t.name << "(";//"f[" << t.sym << "](";
	for (size_t n = 0; n != t.args.size(); ++n) {
		out<B>(os, t.args[n]);
		//os << t.args[n]; -- compiler error somehow
		os << (n == t.args.size() - 1 ? "" : ",");
	}
	return os << ")";
}

template<typename B>
ostream& operator<<(ostream& os, const minterm<B>& x) {
	for (const term<B>& t : x[0]) os << t;
	for (const term<B>& t : x[1]) os << t << "'";
	return os;
}

template<typename B> ostream& operator<<(ostream& os, const bf<B>& f) {
	if (f.v == bf<B>::ONE) return os << '1';
	if (f.v == bf<B>::ZERO) return os << '0';
	size_t n = f.size();
	for (auto& t : f) os << t << (--n ? " | " : "");
	return os;
}
#endif
