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
#include "tau.h"
#include <sstream>
#include <iostream>
#include <cassert>

#define DEBUG
#ifdef DEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif

template<typename elem> term<elem>::arg::arg(const term<elem>& t) :
	ist(true), t(t) {}

template<typename elem> term<elem>::arg::arg(const bf<elem>& f) :
	ist(false), f(bf<elem>(f)) {}

template<typename elem> bool term<elem>::arg::operator==(
		const term<elem>::arg& a) const {
	if (ist != a.ist) return false;
	//if (t) return *t == *a.t; return *f == *a.f;
	return ist ? t == a.t : f == a.f;
}

template<typename elem> bool term<elem>::arg::operator<(
		const term<elem>::arg& a) const {
	if (a.ist != ist) return ist; 
	//if (t) return a.t < t; return a.f < f;
	return ist ? a.t < t : a.f < f;
}

template<typename elem> bool term<elem>::operator==(const term& x) const {
	return t == x.t && sym == x.sym && args == x.args;
}

template<typename elem> bool term<elem>::operator<(const term& x) const {
	if (t != x.t) return t < x.t;
	if (t == ELEM) return e < x.e;
	if (t == VAR) return sym < x.sym;
	if (t != FUNC) return false;
	if (sym != x.sym) return sym < x.sym;
	return args < x.args;
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

clause operator&(const clause& x, const clause& y) {
	//DBG(cout << x << "&&" << y << " = ";)
	clause z;
	z[1] = x[1];
	assert(x[0].size() <= 1);
	assert(y[0].size() <= 1);
	if (!x[0].empty()) {
		if (y[0].empty()) z[0] = x[0];
		else {
			term<bf<Bool>> t1 = *x[0].begin();
			assert(t1.t == term<sbf>::ELEM);
			term<bf<Bool>> t2 = *y[0].begin();
			assert(t2.t == term<sbf>::ELEM);
			auto t = t1.e | t2.e;
			if (t != bf<Bool>::one()) z[0].insert(t);
		}
	} else z[0] = y[0];
	for (auto& t : y[1])
		if (auto it = z[0].find(t); it != z[0].end())
			return minterm<sbf>();
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

bool operator<=(const clause& c, const clause& d) {
	return operator<=<sbf>(c, d);
}

bool operator<=(const clause& c, const fof& f) {
	return operator<=<sbf>(c, f);
}

fof operator&(const fof& x, const fof& y) {
	return operator&<sbf>(x, y);
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
term<elem>::arg subst(
	const typename term<elem>::arg& a, int s, const bf<elem>& f) {
	return a.t ? term<elem>::arg(subst(*a.t, s, f)) : a;
}

template<typename elem>
term<elem> subst(const term<elem>& t, int s, const bf<elem>& f) {
	if (t.t == term<elem>::VAR) {
		if (t.sym != s) return t;
		return term({f});
	}
	if (t.t == term<elem>::FUNC) {
		term r = t;
		for (size_t n = 0; n != r.args.size(); ++n)
			r.args[n] = subst(r.args[n], s, f);
		return r;
	}
	return t;
}

template<typename elem>
bf<elem> subst(const minterm<elem>& t, int s, const bf<elem>& f) {
	bf<elem> r(minterm<elem>::one());
	for (const term<elem>& x : t[0])
		r = minterm<elem>(true, subst(x, s, f)) && r;
	for (const term<elem>& x : t[1])
		r = minterm<elem>(false, subst(x, s, f)) && r;
	return r;
}

template<typename elem>
bf<elem> subst(const bf<elem>& x, int s, const bf<elem>& y) {
	bf<elem> z;
	for (const minterm<elem>& t : x) z = z | subst(t, s, y);
	return z;
}

fof all(const fof&, int);
fof ex(const fof&, int);

ostream& operator<<(ostream& os, const Bool& b) {
	return os << (b.b ? "T" : "F");
}

template<typename elem>
ostream& operator<<(ostream& os, const typename term<elem>::arg& a) {
	if (a.t) return os << *a.t;
	return os << *a.f;
}

template<typename elem>
void out(ostream& os, const typename term<elem>::arg& a) {
	//if (a.t) os << *a.t; else os << *a.f;
	if (a.ist) os << a.t; else os << a.f;
}

template<typename elem>
ostream& operator<<(ostream& os, const term<elem>& t) {
	if (t.t == term<elem>::ELEM) return os << t.e;
	if (t.t == term<elem>::VAR) return os << t.sym;// "x[" << -t.sym << "]";
	if (t.t == term<elem>::FUNC) os << t.sym << "(";//"f[" << t.sym << "](";
	for (size_t n = 0; n != t.args.size(); ++n) {
		out<elem>(os, t.args[n]);
		//os << t.args[n];
		os << (n == t.args.size() - 1 ? "" : ",");
	}
	if (t.t == term<elem>::FUNC) os << ")";
	return os;
}

template<typename elem>
ostream& operator<<(ostream& os, const minterm<elem>& x) {
	for (const term<elem>& t : x[0]) os << t;
	for (const term<elem>& t : x[1]) os << t << "'";
	return os;
}

template<typename elem>
ostream& operator<<(ostream& os, const bf<elem>& f) {
	size_t n = f.size();
	for (auto& t : f) os << t << (--n ? " | " : "");
	return os;
}

ostream& operator<<(ostream& os, const clause& c) {
	for (auto& t : c[0]) os << t << " = 0" << endl;
	for (auto& t : c[1]) os << t << " != 0" << endl;
	return os;
}

term<Bool> var_term(int v) {
	stringstream ss;
	ss << "x[" << v << "]";
	return term<Bool>(ss.str());
}

bf<Bool> var(int v) { return bf<Bool>(minterm(true, var_term(v))); }

term<Bool> fapp(const string& fname, const vector<string>& args) {
	vector<term<Bool>::arg> v;
	for (const string& s : args) v.emplace_back(term<Bool>(s));
	return term<Bool>(fname, v);
}

term<Bool> fapp(size_t i, size_t j, size_t v) {
	stringstream ss;
	ss << "f[" << i << ',' << j << ']';
	vector<term<Bool>::arg> a;
	for (size_t n = 0; n != v; ++n) a.push_back(var_term(n));
	return term<Bool>(ss.str(), a);
}

fof generic(size_t nc, size_t csz, size_t nv) {
	fof f = fof::zero();
	for (size_t k = 0; k != nc; ++k)
		for (size_t n = 0; n != csz; ++n)
			f = clause(!n, term<sbf>(sbf(fapp(k, n, nv)))) | f;
	return f;
}

int main() {
	cout << generic(2, 2, 2) << endl;
	return 0;
	//auto f = ~(var(-1) & var(-2) & var(-3) & var(-4));
	auto f = ~(var(1) & var(2));
//	cout << f << endl;
	auto g = var(3) & var(4);
	//auto g = ~(var(-1) & var(-2)) | (var(-3) & var(-4));
//	cout << g << endl;
	clause c1(true, f);
	//clause c2(false, g);
	clause c2(true, g);
	fof f1(c1);
	fof f2(c2);
	cout << c1 << endl;
	cout << c2 << endl;
	cout << (f1 & f2) << endl;
//	bf<Bool> f;
//	cout << f << endl;
}
