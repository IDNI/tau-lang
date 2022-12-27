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
//#define BREAK_BF
#include "defs.h"
#include <vector>
#include <map>
#include <set>
#include <ostream>
#include <array>
#include <string>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <functional>
#include <climits>
using namespace std;

#define pfst(x) (*(x).begin())
//typedef string sym_t;

template<typename T, typename V> bool has(const T& t, const V& v) {
	return t.find(v) != t.end();
}

template<typename T, typename V> bool hasv(const T& t, const V& v) {
	return std::find(t.begin(), t.end(), v) != t.end();
}

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
		arg(const term& t);
		arg(const bf<B>& f);
		bool operator==(const arg&) const;
		bool operator<(const arg&) const;
		arg subst(const sym_t& s, const bf<B>& f) const {
			return ist ? arg(t.subst(s, f)) : *this;
		}
		arg subst(const term<B>& s, const bf<B>& f) const {
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
	//term() {}
	term(const B& e) :
		t(ELEM), sym(0), name(), e(e),
		f(bf<B>::zero()), args() {}
//	term(type t) : t(t) {}
	term(const sym_t& sym) :
		t(VAR), sym(sym), name(), e(),
		f(bf<B>::zero()), args() {}
	//term(const sym_t& sym, const vector<arg>& a) :
	//	t(FUNC), sym(sym), args(a) {}
	term(const string& name, const vector<arg>& a) :
		t(FUNC), sym(0), name(name), e(B::zero()),
		f(bf<B>::zero()), args(a) {}

	static term<B> mk(const bf<B>& f) {
		if (f.v == bf<B>::ZERO) return term<B>(B::zero());
		if (f.v == bf<B>::ONE) return term<B>(B::one());
		DBG(assert(!f.empty()));
		if (f.size() == 1) {
			auto& c = *f.begin();
			if (c[1].empty() && c[0].size() == 1)
				return term<B>(*c[0].begin());
		}
		return term<B>(f);
	}
	bool operator==(const term& x) const;
	bool operator<(const term& x) const;
	term subst(const term<B>&, const bf<B>&) const;
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
	bool hasterm(const term<B>& v) const;
private:
	term(const bf<B>& f) :
		t(BF), sym(0), name(), e(B::zero()),
		f(f), args() {}
};

template<typename B> struct minterm : public array<set<term<B>>, 2> {
	typedef array<set<term<B>>, 2> base;

	minterm() : base() {}
	minterm(const set<term<B>>& x, const set<term<B>>& y) :
		base({x, y}) {}
	minterm(const minterm &m) { (*this)[0] = m[0]; (*this)[1] = m[1]; }
	minterm(minterm &&m) { (*this)[0] = move(m[0]); (*this)[1] = move(m[1]);}
	minterm(bool pos, const term<B>& t) : base() { add(pos, term<B>(t)); }
	void add(bool, const term<B>&);
	bf<B> subst(sym_t, const bf<B>& f) const;
	bf<B> subst(const term<B>& s, const bf<B>& f) const;
	bool operator<(const minterm& m) const {
		if ((*this)[0].size() != m[0].size())
			return (*this)[0].size() < m[0].size();
		if ((*this)[1].size() != m[1].size())
			return (*this)[1].size() < m[1].size();
		for (	auto it = (*this)[0].begin(), jt = m[0].begin();
			it != (*this)[0].end(); ++it, ++jt)
			if (*it != *jt) return *it < *jt;
		for (	auto it = (*this)[1].begin(), jt = m[1].begin();
			it != (*this)[1].end(); ++it, ++jt)
			if (*it != *jt) return *it < *jt;
		return false;
		return (base)*this < (base)m;
	}
	bool operator==(const minterm& m) const {
		return (*this)[0] == m[0] && (*this)[1] == m[1];
	}
	minterm& operator=(const minterm& m) {
		(*this)[0] = m[0];
		(*this)[1] = m[1];
		return *this;
	}
};

template<typename B> struct bf : public set<minterm<B>> {
	typedef set<minterm<B>> base;
	enum { ZERO, ONE, NONE } v;

	bf() : base(), v(NONE) {}
	bf(const term<B>& t) : bf() {
		if (t.t != term<B>::ELEM) this->emplace(true, t);
		else if (t.e == B::zero()) v = ZERO;
		else if (t.e == B::one()) v = ONE;
		else this->emplace(true, t);
	}
	bf(const minterm<B>& t) : base({t}), v(NONE) {}
	bf(bool b) { v = (b ? ONE : ZERO); }

	bool operator==(const bf<B>& f) const;
	bool operator<(const bf<B>& f) const;
	static const bf<B>& zero();
	static const bf<B>& one();
	bf subst(sym_t s, const bf<B>& y) const;
	bf subst(const term<B>& s, const bf<B>& y) const;
	bool hasterm(const term<B>& v) const;
};

template<typename B> term<B>::arg::arg(const term<B>& t) :
	ist(true), t(t), f(false) {}

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
	DBG(assert(!(t.t == term<B>::ELEM && t.e == B::zero()));)
	(*this)[pos ? 0 : 1].insert(t);
}

template<typename B> bool bf<B>::operator==(const bf<B>& f) const {
	if (f.v != v) return false;
	return (base)(*this) == (base)f;
}

template<typename B> bool bf<B>::operator<(const bf<B>& f) const {
	if (f.v != v) return v < f.v;
	if (base::size() != f.size()) return base::size() < f.size();
	for (auto it=this->begin(), jt=f.begin(); it != this->end(); ++it, ++jt)
		if ((*it)[0].size() != (*jt)[0].size())
			return (*it)[0].size() < (*jt)[0].size();
		else if ((*it)[1].size() != (*jt)[1].size())
			return (*it)[1].size() < (*jt)[1].size();
	return (base)(*this) < (base)f;
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
	bf<B> f(false);
	for (const term<B>& t : x[0]) f = minterm(false, t) | f;
	for (const term<B>& t : x[1]) f = minterm(true, t) | f;
	return f;
}

template<typename B> bf<B> operator&(
		const minterm<B>& x, const minterm<B>& y) {
//	typedef array<minterm<B>, 2> item;
	struct item {
		const minterm<B> x, y;
		item(const minterm<B>& x, const minterm<B>& y) : x(x), y(y) {}
		bool operator<(const item& a) const {
			if (x[0].size() != a.x[0].size())
				return x[0].size() < a.x[0].size();
			if (x[1].size() != a.x[1].size())
				return x[1].size() < a.x[1].size();
			if (y[0].size() != a.y[0].size())
				return y[0].size() < a.y[0].size();
			if (y[1].size() != a.y[1].size())
				return y[1].size() < a.y[1].size();
			return (x != a.x) ? x < a.x : y < a.y;

		}
	};
	if (x[0].empty() && x[1].empty()) return y;
	if (y[0].empty() && y[1].empty()) return x;
	//DBG(cout << x << "&&" << y << " = ";)
//	static map<item, bf<B>> M;
//	static size_t hits = 0, misses = 0;
//	item a = (x < y) ? item(x, y) : item(y, x);
//	if (auto it = M.find(a); it != M.end()) return ++hits, it->second;
//	++misses;
//	if ((hits + misses)%100000 == 0)
//		cout << "(M) hits: " << hits << " misses: " << misses << endl;
//	if ((hits + misses)>1000000) M.clear();
#ifdef DEBUG
	for (const term<B>& t : x[0]) assert(!t.zero() && !t.one());
	for (const term<B>& t : x[1]) assert(!t.zero() && !t.one());
	for (const term<B>& t : y[0]) assert(!t.zero() && !t.one());
	for (const term<B>& t : y[1]) assert(!t.zero() && !t.one());
#endif
	//cout << "(" << x << ") & (" << y << ") = " << endl;
	for (const term<B>& t : y[0])
		if (has(x[1], t)) return bf<B>(false);
	for (const term<B>& t : y[1])
		if (has(x[0], t)) return bf<B>(false);
	minterm<B> z = x;
	z[0].insert(y[0].begin(), y[0].end());
	z[1].insert(y[1].begin(), y[1].end());
	bf<B> r(move(z));
	//DBG(cout << z << endl;)
	//M.insert({a, r});
	return r;
}

template<typename B>
bool operator<=(const minterm<B>& x, const minterm<B>& y) { return (x&y) == x; }

bool operator<=(const minterm<Bool>& x, const minterm<Bool>& y) {
	for (auto& t : y[0]) if (x[0].find(t) == x[0].end()) return false;
	for (auto& t : y[1]) if (x[1].find(t) == x[1].end()) return false;
	return true;
}

template<typename B>
bool operator<=(const minterm<B>& t, const bf<B>& f) {
	if (f == bf<B>::one()) return true;
	if (f == bf<B>::zero()) return false;
	for (const minterm<B>& x : f) if (t <= x) return true;
//	cout << t << " is not leq " << f << endl;
	return false;
}

template<typename B>
bool complementary(const minterm<B>& x, minterm<B>& y) {
	size_t n0 = x[0].size(), n1 = x[1].size();
	size_t k0 = y[0].size(), k1 = y[1].size();
	const term<B> *t1 = 0;
	if (n0 == k0 + 1 && n1 + 1 == k1) {
		bool b = false;
		for (auto& t : x[0])
			if (has(y[0], t)) continue;
			else if (b) return false;
			else t1 = &t, b = true;
		b = false;
		for (auto& t : y[1])
			if (has(x[1], t)) continue;
			else if (b || t != *t1) return false;
			else b = true;
		return b ? y[1] = x[1], true : false;
	}
	if (n0 + 1 != k0 || n1 != k1 + 1) return false;
	bool b = false;
	for (auto& t : y[0])
		if (has(x[0], t)) continue;
		else if (b) return false;
		else t1 = &t, b = true;
	b = false;
	for (auto& t : x[1])
		if (has(y[1], t)) continue;
		else if (b || t != *t1) return false;
		else b = true;
	return b ? y[0] = x[0], true : false;
}

template<typename B> bf<B> disj_fmt(minterm<B> t, const bf<B>& f) {
	if (f == bf<B>::one()) return f;
	if (f == bf<B>::zero()) return bf<B>(t);
	//if (t <= f) return f;
	DBG(assert(!t[0].empty() || !t[1].empty());)
	DBG(assert(!f.empty());)
	for (const term<B>& x : t[0])
		if (x.t == term<B>::ELEM && x.e == B::zero()) return f;
		DBG(else assert(x.t != term<B>::BF);)
	for (const term<B>& x : t[1])
		if (x.t == term<B>::ELEM && x.e == B::one()) return f;
		DBG(else assert(x.t != term<B>::BF);)
	bf<B> g, h;
	for (auto& x : f)
		if (t <= x) return f;
		else if (!(x <= t)) g.insert(x);
	while (!g.empty()) {
		auto x = *g.begin();
		g.erase(g.begin());
		if (!complementary(x, t)) h.insert(x);
		else if (t[0].empty() && t[1].empty()) return bf<B>(true);
	}
	return h.insert(t), h;
} 

template<typename B>
bf<B> operator|(const minterm<B>& t, const bf<B>& f) {
	return disj_fmt<B>(t, f);
}

template<typename B> bf<B> operator~(const bf<B>& f) {
	if (f == bf<B>::one()) return bf<B>::zero();
	if (f == bf<B>::zero()) return bf<B>::one();
	bf g = bf<B>::one();
	for (const minterm<B>& t : f) g = g & ~t;
	return g;
}

template<typename B> bf<B> operator&(const minterm<B>& x, const bf<B>& y) {
	if (y == bf<B>::zero()) return y;
	if (y == bf<B>::one()) return bf<B>(x);
	bf<B> z(false);
	for (const minterm<B>& t : y) z = (x & t) | z;
	return z.empty() ? bf<B>(false) : z;
}

template<typename B> bf<B> operator&(const bf<B>& x, const bf<B>& y) {
	if (x == bf<B>::zero() || y == bf<B>::zero()) return bf<B>::zero();
	if (x == bf<B>::one()) return y;
	if (y == bf<B>::one()) return x;
	bf<B> z = bf<B>::zero();
	for (const minterm<B>& s : x)
		for (const minterm<B>& t : y)
			z = (s & t) | z;
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

template<typename B> bf<B> operator+(const bf<B>& x, const bf<B>& y) {
	return (x&~y)|(y&~x);
}

template<typename B> bool operator<=(const bf<B>& x, const bf<B>& y) {
	if (y == bf<B>::zero()) return x == bf<B>::zero();
	if (x == bf<B>::zero()) return true;
	if (x == bf<B>::one()) return y == bf<B>::one();
	if (y == bf<B>::one()) return true;
	for (const minterm<B>& t : x) if (!(t <= y)) return false;
	return true;
}

template<typename B> bool isvar(const bf<B>& f) {
	if (f.size() != 1) return false;
	auto& m = *f.begin();
	if (m[0].size() == 1 && m[1].empty())
		return m[0].begin()->t == term<B>::VAR;
	return false;
}

template<typename B>
term<B> term<B>::subst(const term<B>& s, const bf<B>& g) const {
	if (*this == s) return term<B>(g);
	if (t == term<B>::BF) return f.subst(s, g);
	DBG(assert(t == term<B>::FUNC);)
	vector<typename term<B>::arg> v;
	for (auto& a : args) v.push_back(a.subst(s, g));
	return term<B>(name, v);
}

template<typename B> bool bf<B>::hasterm(const term<B>& v) const {
	for (auto& x : *this) {
		for (auto& y : x[0]) if (y.hasterm(v)) return true;
		for (auto& y : x[1]) if (y.hasterm(v)) return true;
	}
	return false;
}

template<typename B> bool term<B>::hasterm(const term<B>& v) const {
	switch (t) {
		case VAR: return v == *this;
		case ELEM: return false;
		case FUNC:
			for (auto& x : args)
				if 	((x.ist && x.t.hasterm(v)) ||
					(!x.ist && x.f.hasterm(v)))
				return true;
			return false;
		case BF: return f.hasterm(v);
		default: throw 0;
	}
}

template<typename B> bf<B> minterm<B>::subst(sym_t s, const bf<B>& f) const {
	return subst(term<B>(s), f);
}

template<typename B>
bf<B> minterm<B>::subst(const term<B>& s, const bf<B>& f) const {
	struct item {
		item(const minterm<B>& m, const term<B>& t, const bf<B>& f) :
			m(m), t(t), f(f) {}
		minterm<B> m;
		term<B> t;
		bf<B> f;
		bool operator<(const item& x) const {
			if (t != x.t) return t < x.t;
			if (m != x.m) return m < x.m;
			return f < x.f;
		}
	};
	static map<item, bf<B>> M;
	static size_t hits = 0, misses = 0;
	item a(*this, s, f);
	if (auto it = M.find(a); it != M.end()) {
		/*assert(a.m == *this);
		assert(a.t == s);
		assert(a.f == f);
		assert(it->first.m == *this);
		assert(it->first.t == s);
		assert(it->first.f == f);*/
		return ++hits, it->second;
	}
	++misses;
	if ((hits + misses)%100000 == 0)
		cout << "(S) hits: " << hits << " misses: " << misses << endl;
	if ((hits + misses)>1000000) M.clear();
	bf<B> r(true);
	for (auto& t : (*this)[0])
		if (t.t == term<B>::BF)
			r = (t.f.hasterm(s) ? t.f.subst(s, f) : bf<B>(t)) & r;
		else if (t.t == term<B>::FUNC)
			r = (t.hasterm(s) ? bf<B>(t.subst(s,f)) : bf<B>(t)) & r;
		else if (t.t == term<B>::VAR) {
			if (s.t == term<B>::VAR && t.sym == s.sym) r = r & f;
			else r = bf<B>(t) & r;
		} else if (t.t == term<B>::ELEM) r = bf<B>(t) & r;
		else throw 0;
	for (auto& t : (*this)[1])
		if (t.t == term<B>::BF)
			r = (t.f.hasterm(s) ? ~t.f.subst(s, f) : ~bf<B>(t)) & r;
		else if (t.t == term<B>::FUNC)
			r = (t.hasterm(s) ? ~bf<B>(t.subst(s,f)) : ~bf<B>(t))&r;
		else if (t.t == term<B>::VAR) {
			if (s.t == term<B>::VAR && t.sym == s.sym) r = r & ~f;
			else r = ~bf<B>(t) & r;
		} else if (t.t == term<B>::ELEM) r = ~bf<B>(t) & r;
		else throw 0;
	M.emplace(a, r);
	return r;
}

template<typename B> bf<B> bf<B>::subst(sym_t s, const bf<B>& y) const {
	return subst(term<B>(s), y);
}

template<typename B>
bf<B> bf<B>::subst(const term<B>& s, const bf<B>& y) const {
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

template<typename B>
vector<bf<B>> lgrs(const bf<B>& f, const vector<term<B>>& v) {
	DBG(assert(f != bf<B>::one());)
	size_t n = INT_MAX;
	minterm<B> z;
	for (const minterm<B>& m : f)
		if (m[0].size() < n)
			n = m[0].size, z = m;
	vector<bf<B>> r;
	for (size_t n = 0; n != v.size(); ++n)
		if (has(z[0], v[n])) r[n] = f + bf<B>(v[n]) & ~f;
		else r[n] = bf<B>(v[n]) & ~f;
	return r;
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

template<typename B> ostream& operator<<(ostream& os, const term<B>& t) {
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
