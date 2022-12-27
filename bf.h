#ifndef __BF_H__
#define __BF_H__
#include "minterm.h"
#include <climits>

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

template<typename B> bool bf<B>::operator==(const bf<B>& f) const {
	if (f.v != v) return false;
	return (base)(*this) == (base)f;
}

bool operator<=(const minterm<Bool>& x, const minterm<Bool>& y) {
	for (auto& t : y[0]) if (x[0].find(t) == x[0].end()) return false;
	for (auto& t : y[1]) if (x[1].find(t) == x[1].end()) return false;
	return true;
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

template<typename B>
bf<B> bf<B>::subst(const term<B>& s, const bf<B>& y) const {
	bf<B> z(false);
	for (const minterm<B>& t : *this) z = t.subst(s, y) | z;
	return z;
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

template<typename B> bf<B> operator|(const minterm<B>& t, const bf<B>& f) {
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

template<typename B> bool bf<B>::hasterm(const term<B>& v) const {
	for (auto& x : *this) {
		for (auto& y : x[0]) if (y.hasterm(v)) return true;
		for (auto& y : x[1]) if (y.hasterm(v)) return true;
	}
	return false;
}

template<typename B>
bf<B> mt_trans_vars(const minterm<B>& m, function<sym_t(sym_t)> g) {
	bf<B> b(true);
	for (size_t j = 0; j != 2; ++j)
		for (const term<B>& t : m[j])
			b = minterm<B>(!j, term_trans_vars(t, g)) & b;
	return b;
}

// condition g!=0 given f=0
template<typename B> bf<B> condition(const bf<B>& f, const bf<B>& g) {
	set<term<B>> s;
	// find negative units
	for (const minterm<B>& x : f)
		if (x[0].empty() && x[1].size() == 1)
			s.insert(*x[1].begin());
	bf<B> r;
	for (const minterm<B>& x : g) {
		bool b = true;
		for (const minterm<B>& y : f)
			// elim terms dominated by zero
			if (!(b &= !(x <= y))) break;
		if (b) r.insert(x);
	}
	// elim the units
	for (const term<B>& t : s) r = r.subst(t, bf<Bool>(true));
	return r;
}

template<typename B> bf<B> bf<B>::subst(sym_t s, const bf<B>& y) const {
	return subst(term<B>(s), y);
}

template<typename B> bf<B> ex(const bf<B>& f, const sym_t& v) {
	return f.subst(v, bf<B>::zero()) | f.subst(v, bf<B>::one());
}

template<typename B> bf<B> all(const bf<B>& f, const sym_t& v) {
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

template<typename B> ostream& operator<<(ostream& os, const bf<B>& f) {
	if (f.v == bf<B>::ONE) return os << '1';
	if (f.v == bf<B>::ZERO) return os << '0';
	size_t n = f.size();
	for (auto& t : f) os << t << (--n ? " | " : "");
	return os;
}
#endif
