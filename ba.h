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

using namespace std;

struct Bool {
	bool b;
	Bool() {}
	Bool(bool b) : b(b) {}
	static const Bool& zero() { static Bool b(false); return b; }
	static const Bool& one() { static Bool b(true); return b; }
	const Bool& operator&(const Bool& t) const {return b&t.b?one():zero();}
	const Bool& operator|(const Bool& t) const {return b|t.b?one():zero();}
	const Bool& operator~() const { return b ? zero() : one(); }
	bool operator<(const Bool& t) const { return b < t.b; }
	bool operator==(const Bool& t) const { return b == t.b; }
};

template<typename elem> struct bf;
template<typename elem> struct term {
	struct arg {
		bool ist;
		term t;
		bf<elem> f;
		arg() {}
		arg(const term& t);
		arg(const bf<elem>& f);
		bool operator==(const arg&) const;
		bool operator<(const arg&) const;
		arg subst(const string& s, const bf<elem>& f);
	};
	enum type { ELEM, VAR, BF, FUNC } t;
	//int sym = 0;
	string sym;
	elem e;
	bf<elem> f;
	vector<arg> args;
	term() {}
	term(const elem& e) : t(ELEM), e(e) {}
	term(type t) : t(t) {}
	term(const string& sym) : t(VAR), sym(sym) {}
	term(const string& sym, const vector<arg>& a) :
		t(FUNC), sym(sym), args(a) {}
	term(const bf<elem>& f) : t(BF), f(f) {}
	bool operator==(const term& x) const;
	bool operator<(const term& x) const;
};

template<typename elem> struct minterm : public array<set<term<elem>>, 2> {
	typedef array<set<term<elem>>, 2> base;

	minterm() : base() {}
	minterm(bool pos, const term<elem>& t);
};

template<typename elem> struct bf : public set<minterm<elem>> {
	enum { ZERO, ONE, NONE } v;

	bf() : set<minterm<elem>>(), v(NONE) {}
	bf(const term<elem>& t) : set<minterm<elem>>({minterm<elem>(true, t)}),
		v(NONE) {}
	bf(const minterm<elem>& t) : set<minterm<elem>>({t}), v(NONE) {}
	bf(bool b) { v = b ? ONE : ZERO; }

	bool operator==(const bf<elem>& f) const;
	bool operator<(const bf<elem>& f) const;
	static const bf<elem>& zero();
	static const bf<elem>& one();
};

template<typename elem> bf<elem> operator~(const minterm<elem>&);
template<typename elem>
minterm<elem> operator&(const minterm<elem>&, const minterm<elem>&);
template<typename elem>
bool operator<=(const minterm<elem>&, const minterm<elem>&);
template<typename elem>
bool operator<=(const minterm<elem>&, const bf<elem>&);
template<typename elem>
bf<elem> operator|(const minterm<elem>&, const bf<elem>&);
template<typename elem> bf<elem> operator~(const bf<elem>&);
template<typename elem> bf<elem> operator&(const bf<elem>&, const bf<elem>&);
template<typename elem> bf<elem> operator|(const bf<elem>&, const bf<elem>&);
template<typename elem> bool operator<=(const bf<elem>&, const bf<elem>&);
template<typename elem>
term<elem> subst(const term<elem>&, const string&, const bf<elem>&);
template<typename elem>
bf<elem> subst(const minterm<elem>&, const string&, const bf<elem>&);
template<typename elem>
bf<elem> subst(const bf<elem>&, const string&, const bf<elem>&);
template<typename elem> bf<elem> ex(const bf<elem>&, int);
template<typename elem> bf<elem> all(const bf<elem>&, int);
template<typename elem> bf<elem> ex(const bf<elem>&, const string&);
template<typename elem> bf<elem> all(const bf<elem>&, const string&);
template<typename elem>
ostream& operator<<(ostream& os, const typename term<elem>::arg& a);
template<typename elem>
ostream& operator<<(ostream& os, const term<elem>& t);
template<typename elem>
ostream& operator<<(ostream& os, const minterm<elem>& x);
template<typename elem>
ostream& operator<<(ostream& os, const bf<elem>& f);

template<typename elem> term<elem>::arg::arg(const term<elem>& t) :
	ist(true), t(t) {}

template<typename elem> term<elem>::arg::arg(const bf<elem>& f) :
	ist(false), f(bf<elem>(f)) {}

template<typename elem> bool term<elem>::arg::operator==(
		const term<elem>::arg& a) const {
	return ist != a.ist ? false : ist ? t == a.t : f == a.f;
}

template<typename elem> bool term<elem>::arg::operator<(
		const term<elem>::arg& a) const {
	return a.ist != ist ? ist : ist ? a.t < t : a.f < f;
}

template<typename elem> bool term<elem>::operator==(const term& x) const {
	if (t != x.t) return false;
	switch (t) {
		case ELEM: return e == x.e;
		case VAR: return sym == x.sym;
		case BF: return f == x.f;
		case FUNC: return sym == x.sym && args == x.args;
		default: assert(0);
	}
}

template<typename elem> bool term<elem>::operator<(const term& x) const {
	if (t != x.t) return t < x.t;
	switch (t) {
		case ELEM: return e < x.e;
		case VAR: return sym < x.sym;
		case BF: return f < x.f;
		case FUNC: return sym == x.sym ? args < x.args : sym < x.sym;
		default: assert(0);
	}
}

template<typename elem> minterm<elem>::minterm(bool pos, const term<elem>& t) :
	base() {
	(*this)[pos ? 0 : 1].insert(t);
}

template<typename elem> bool bf<elem>::operator==(const bf<elem>& f) const {
	if (f.v != v) return false;
	return (set<minterm<elem>>)(*this) == (set<minterm<elem>>)f;
}

template<typename elem> bool bf<elem>::operator<(const bf<elem>& f) const {
	if (f.v != v) return v < f.v;
	return (set<minterm<elem>>)(*this) < (set<minterm<elem>>)f;
}

template<typename elem> const bf<elem>& bf<elem>::zero() {
	static bf<elem> z(false);
	return z;
}

template<typename elem> const bf<elem>& bf<elem>::one() {
	static bf<elem> z(true);
	return z;
}

template<typename elem> bf<elem> operator~(const minterm<elem>& x) {
	bf<elem> f = bf<elem>::zero();
	for (const term<elem>& t : x[0])
		f = f | bf<elem>(minterm(false, t));
	for (const term<elem>& t : x[1])
		f = f | bf<elem>(minterm(true, t));
	return f;
}

template<typename elem> minterm<elem> operator&(
		const minterm<elem>& x, const minterm<elem>& y) {
	//DBG(cout << x << "&&" << y << " = ";)
	minterm<elem> z = x;
	for (const term<elem>& t : y[0])
		if (auto it = z[1].find(t); it != z[1].end())
			return minterm<elem>();
		else z[0].insert(t);
	for (const term<elem>& t : y[1])
		if (auto it = z[0].find(t); it != z[0].end())
			return minterm<elem>();
		else z[1].insert(t);
	//DBG(cout << z << endl;)
	return z;
}

template<typename elem>
bool operator<=(const minterm<elem>& x, const minterm<elem>& y) {
	return (x & y) == x;
}

template<typename elem>
bf<elem>& operator+=(bf<elem>& f, const minterm<elem>& t) {
	if (t == minterm<elem>::zero()) return f;
	if (t == minterm<elem>::one()) { f.clear(); f.insert(t); }
	return f.insert(t), f;
}

template<typename elem>
bool operator<=(const minterm<elem>& t, const bf<elem>& f) {
	if (f == bf<elem>::one()) return true;
	for (const minterm<elem>& x : f) if (t <= x) return true;
	return false;
}

template<typename elem>
bf<elem> operator|(const minterm<elem>& t, const bf<elem>& f) {
	if (t[0].empty() && t[1].empty()) return f;
	if (f == bf<elem>::one()) return f;
	if (f == bf<elem>::zero()) return bf<elem>(t);
	//DBG(cout << t << "||" << f << " = ";)
	for (const minterm<elem>& x : f) if (t <= x) return f;
	bf g = {t};
	for (const minterm<elem>& x : f)
		//if (!(x <= t)) TODO //g += x;
		g.insert(x);
	//DBG(cout << g << endl;)
	return g;
}

template<typename elem> bf<elem> operator~(const bf<elem>& f) {
	bf g = bf<elem>::one();
	for (const minterm<elem>& t : f) g = g & ~t;
	return g;
}

template<typename elem>
bf<elem> operator&(const minterm<elem>& x, const bf<elem>& y) {
	if (y == bf<elem>::zero()) return y;
	if (y == bf<elem>::one()) return bf<elem>(x);
	bf<elem> z;
	for (const minterm<elem>& t : y) z = (x & t) | z;
	return z;
}

template<typename elem>
bf<elem> operator&(const bf<elem>& x, const bf<elem>& y) {
	if (x == bf<elem>::zero()) return x;
	if (y == bf<elem>::zero()) return y;
	if (x == bf<elem>::one()) return y;
	if (y == bf<elem>::one()) return x;
	bf<elem> z;
	for (const minterm<elem>& s : x)
		for (const minterm<elem>& t : y)
			z = (s & t) | z;
	return z;
}

template<typename elem>
bf<elem> operator|(const bf<elem>& x, const bf<elem>& y) {
	bf<elem> z = x;
	for (const minterm<elem>& t : y) z = t | z;
	return z;
}

template<typename elem>
bool operator<=(const bf<elem>& x, const bf<elem>& y) {
	for (const minterm<elem>& t : x) if (!(t <= y)) return false;
	return true;
}

template<typename elem>
term<elem>::arg term<elem>::arg::subst(const string& s, const bf<elem>& f) {
	return ist ? arg(::subst(t, s, f)) : *this;
}

/*template<typename elem>
term<elem>::arg term<elem>::subst(
	const term<elem>::arg& a, const string& s, const bf<elem>& f) {
	return a.ist ? term<elem>::arg(subst(a.t, s, f)) : a;
}*/

template<typename elem>
term<elem> subst(const term<elem>& t, const string& s, const bf<elem>& f) {
	if (t.t == term<elem>::VAR) return s == t.sym ? term<elem>(f) : t;
	if (t.t == term<elem>::BF) return subst(t.f, s, f);
	assert(t.t == term<elem>::FUNC);
	term r = t;
	for (size_t n = 0; n != r.args.size(); ++n)
		r.args[n] = r.args[n].subst(s, f);
	return r;
}

template<typename elem>
bf<elem> subst(const minterm<elem>& t, const string& s, const bf<elem>& f) {
	bf<elem> r = bf<elem>::one();
	for (const term<elem>& x : t[0])
		r = minterm<elem>(true, subst(x, s, f)) & r;
	for (const term<elem>& x : t[1])
		r = minterm<elem>(false, subst(x, s, f)) & r;
	return r;
}

template<typename elem>
bf<elem> subst(const bf<elem>& x, const string& s, const bf<elem>& y) {
	bf<elem> z = bf<elem>::zero();
	for (const minterm<elem>& t : x) z = subst(t, s, y) | z;
	return z;
}

template<typename elem> bf<elem> ex(const bf<elem>& f, const string& v) {
	return subst(f, v, bf<elem>::zero()) | subst(f, v, bf<elem>::one());
}

template<typename elem> bf<elem> all(const bf<elem>& f, const string& v) {
	return subst(f, v, bf<elem>::zero()) & subst(f, v, bf<elem>::one());
}

ostream& operator<<(ostream& os, const Bool& b) { return os << (b.b?"T":"F"); }

template<typename elem>
ostream& operator<<(ostream& os, const typename term<elem>::arg& a) {
	return a.ist ? os << a.t : os << a.f;
}

template<typename elem>
void out(ostream& os, const typename term<elem>::arg& a) {
	if (a.ist) os << a.t; else os << a.f;
}

template<typename elem>
ostream& operator<<(ostream& os, const term<elem>& t) {
	if (t.t == term<elem>::ELEM) return os << t.e;
	if (t.t == term<elem>::VAR) return os << t.sym;// "x[" << -t.sym << "]";
	if (t.t == term<elem>::BF) return os << t.f;
	if (t.t == term<elem>::FUNC) os << t.sym << "(";//"f[" << t.sym << "](";
	for (size_t n = 0; n != t.args.size(); ++n) {
		out<elem>(os, t.args[n]);
		//os << t.args[n]; -- compiler error somehow
		os << (n == t.args.size() - 1 ? "" : ",");
	}
	return os << ")";
}

template<typename elem>
ostream& operator<<(ostream& os, const minterm<elem>& x) {
	for (const term<elem>& t : x[0]) os << t;
	for (const term<elem>& t : x[1]) os << t << "'";
	return os;
}

template<typename elem> ostream& operator<<(ostream& os, const bf<elem>& f) {
	if (f.v == bf<elem>::ONE) return os << '1';
	if (f.v == bf<elem>::ZERO) return os << '0';
	size_t n = f.size();
	for (auto& t : f) os << t << (--n ? " | " : "");
	return os;
}

