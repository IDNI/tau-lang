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
#ifndef __FOF_H__
#define __FOF_H__
#include "bf.h"
#include <functional>

template<typename B> using clause = minterm<bf<B>>;
template<typename B> using fof = bf<bf<B>>;

template<typename B> clause<B> simplify(const clause<B>& c) {
	if (c[0].empty()) return c;
//	cout << "simplifying " << c;
	clause<B> d;
	d[0] = c[0];
	set<term<B>> s;
	for (const minterm<B>& x : c[0].begin()->e)
		if (x[0].empty() && x[1].size() == 1) s.insert(*x[1].begin());
	for (const term<bf<B>>& t : c[1]) {
		const bf<B>& f = t.e;
		bf<B> g;
		for (const minterm<B>& x : f) {
			bool b = true;
			for (const minterm<B>& y : c[0].begin()->e)
				if (!(b &= !(x <= y))) break;
			if (b) g.insert(x);
		}
		for (const term<B>& t : s) g = g.subst(t, bf<Bool>(true));
		d[1].insert(g);
	}
//	cout << " gives " << d << endl;
	return d;
}

template<typename B> fof<B> operator&(const clause<B>& x, const clause<B>& y) {
//	static map<array<clause<B>, 2>, clause<B>> M;
//	static size_t hits = 0, misses = 0;
//	typedef array<clause<B>, 2> item;
//	item a = (x < y) ? item{x, y} : item{y, x};
//	if (auto it = M.find(a); it != M.end()) return ++hits, it->second;
//	++misses;
//	if ((hits + misses)%1000 == 0) cout << "hits: " << hits << " misses: "
//	       	<< misses << endl;
	//DBG(cout << x << "&&" << y << " = ";)
	clause<B> z(set<term<bf<B>>>(), x[1]);
	DBG(assert(x[0].size() <= 1 && y[0].size() <= 1);)
	if (x[0].empty()) z[0] = y[0];
	else if (y[0].empty()) z[0] = x[0];
	else if (bf<B> t = pfst(x[0]).e | pfst(y[0]).e; t == bf<Bool>::one())
		return fof<B>(false);
	else z[0].insert(t);
	if (!z[0].empty())
		for (auto& t : z[1])
			if (t.e <= pfst(z[0]).e) return fof<B>(false);
	for (auto& t : y[1])
		if (!z[0].empty() && t.e <= pfst(z[0]).e) return fof<B>(false);
		else if (t.e == bf<Bool>::one()) continue;
		else {
			DBG(assert(t.e != bf<Bool>::zero());)
			bool b = true;
			for (auto& s : z[1]) if (!(b &= !(s.e <= t.e))) break;
			if (b) z[1].insert(t);
		}
	z = simplify(z);
//	M.insert({a, z});
	return z;
}

template<typename B> fof<B> c2fof(const clause<B>& c) {
	fof<B> r(true);
/*	for (const auto& t : c[0])
		for (const auto& m : t.f)
			r = clause<B>(true, bf(m)) & r;*/
	for (const auto& t : c[1]) {
		fof<B> g(false);
		for (const auto& m : t.f)
			g = clause<B>(false, bf(m)) | g;
		r = r & g;
	}
	return r;
}

template<typename B> fof<B> operator|(clause<B> c, const fof<B>& f) {
	DBG(assert(!f.empty() || f == fof<B>::zero() || f == fof<B>::one());)
	DBG(assert(!c[0].empty() || !c[1].empty());)
	if (c[0].empty() && c[1].size() == 1) {
		clause<B> x;
		for (const clause<B>& d : f)
			if (d[0].empty() && d[1].size() == 1)
				{ x = d; break; }
		if (!x[1].empty()) {
#ifdef DEBUG
			for (const clause<B>& d : f)
				if (d[0].empty() && d[1].size() == 1)
					assert(x == d);
#endif
			fof<B> g = ~fof<B>(c[1].begin()->e | x[1].begin()->e);
			for (const clause<B>& d : f) if (d != x) g.insert(d);
			return g;
		}
	}
#ifdef BREAK_BF
	bool b = !c[1].empty();
	if (b) for (const auto& t : c[1])
		if (!(b &= (t.f.size() == 1)))
			break;
	if (b) return (f | c2fof(c));
#endif
//	cout << "disj with " << f.size() << " clauses." << endl;
	if (!c[0].empty()) {
		if (const bf<B>& g = c[0].begin()->e; g == bf<B>::zero())
			c[0].clear();
		else if (g == bf<B>::one()) return f;
	}
	for (auto it = c[1].begin(); it != c[1].end(); ++it)
		if (const bf<B>& g = it->e; g == bf<B>::one()) c[1].erase(it);
		else if (g == bf<B>::zero()) return f;
//	if (fof(c) <= f) return f;
	return disj_fmt(c, f);
}

template<typename B> bf<B> elim(const bf<B>& f, const bf<B>& g, const sym_t& v) {
	return 	g.subst(v, f.subst(v, bf<B>::zero())) |
		g.subst(v, ~f.subst(v, bf<B>::one()));
}

template<typename B> fof<B> ex(const clause<B>& c, const sym_t& v) {
	if (c[0].empty()) {
		DBG(assert(!c[1].empty());)
		fof<B> r(true);
		for (const term<bf<B>>& t : c[1])
			if (bf<B> f = ex(t.e, v); f == bf<B>::one())
				return fof<B>::zero();
			else r = clause<B>(false, f) & r;
		return r;
	}
	if (c[1].empty()) {
		DBG(assert(c[0].size() == 1);)
		DBG(assert(c[0].begin()->t == term<bf<B>>::ELEM);)
		const bf<B> f = all(c[0].begin()->e, v);
		return f.empty() ? fof<B>(true) : clause<B>(true, f); 
	}
	DBG(assert(c[0].size() == 1);)
	const bf<B>& f = c[0].begin()->e;
	fof<B> r(all(f, v));
	if (r != fof<B>::zero())
		for (const term<bf<B>>& t : c[1])
			r = clause<B>(false, elim(f, t.e, v)) & r;
	return r;
}

template<typename B>
array<clause<B>, 2> split(const clause<B>& c, const sym_t& v) {
	clause<B> x, y;
	term<B> s(v);
	for (auto& t : c[0]) (t.e.hasterm(s) ? x : y)[0].insert(t);
	for (auto& t : c[1]) (t.e.hasterm(s) ? x : y)[1].insert(t);
	return {x, y};
}

template<typename B> // f=0->g=0
fof<B> horn(const bf<B>& f, const bf<B>& g, const sym_t& v) {
	static size_t n = 0;
	cout << "horn " << ++n << endl;
	return (fof<B>(all(g, v)) | ~fof<B>(all(f, v))) & fof<B>(elim(f, g, v));
}

template<typename B> fof<B> ex(const fof<B>& f, const sym_t& v) {
#ifdef DEBUG
	static bool nohorn = false;
	if (!nohorn)
#endif
	for (auto& c : f)
		for (auto& d : f)
			if (c[0].empty() && d[1].empty()) {
				DBG(assert(!d[0].empty());)
				const bf<B> g =d[0].begin()->e;
				bf<B> h(false);
				for (auto& t : c[1]) h = h | t.e;
				if (f.size() == 2) {
#ifdef DEBUG
					fof<B> t = d | fof<B>(c);
					nohorn = true;
					assert(ex(t, v) == horn(g, h, v));
					nohorn = false;
#endif
					return horn(h, g, v);
				}
				fof<B> r = f;
				r.erase(c), r.erase(d);
				return horn(g, h, v) | ex(r, v);
			}
	fof<B> g(false);
	array<clause<B>, 2> a;
	for (const clause<B>& c : f) {
		if (a = split(c, v); a[0][0].empty() && a[0][1].empty())
			g = c | g;
		else g = ex(a[0], v) | g;
		if (!a[1][0].empty() || !a[1][1].empty()) g = a[1] & g;
		//g = ex(c, v) | g;
	}
	return g;
}

template<typename B>
fof<B> all(const fof<B>& f, const sym_t& v) { return ~ex<B>(~f, v); }

template<typename B>
term<B> term_trans_vars(const term<B>& t, function<sym_t(sym_t)> g) {
//	cout << "in: " << t << endl;
	if (t.t == term<B>::VAR) return term<B>(g(t.sym));
	if (t.t == term<B>::FUNC) {
		vector<typename term<B>::arg> v;
		for (auto& a : t.args)
			if (a.ist) v.emplace_back(term_trans_vars<B>(a.t, g));
			else v.push_back(a);
//		cout << "out: " << r << endl;
		return term<B>(t.name, v);
	}
//	cout << "out: " << t << endl;
	return t;
}

template<typename B>
bf<B> mt_trans_vars(const minterm<B>& m, function<sym_t(sym_t)> g) {
	bf<B> b(true);
	for (size_t j = 0; j != 2; ++j)
		for (const term<B>& t : m[j])
			b = minterm<B>(!j, term_trans_vars(t, g)) & b;
	return b;
}

template<typename B>
fof<B> transform_vars(const clause<B>& c, function<sym_t(sym_t)> g) {
	fof<B> d(true);
	for (size_t i = 0; i != 2; ++i)
		for (const term<bf<B>>& s : c[i]) {
			DBG(assert(s.t == term<bf<B>>::ELEM);)
			bf<B> p(false);
			for (const minterm<B>& m : s.e)
				p = mt_trans_vars(m, g) | p;
			if (p == bf<B>::zero()) {
				if (i) return fof<B>::zero();
				continue;
			}
			d = clause<B>(!i, p) & d;
		}
	return d;
}

template<typename B>
fof<B> transform_vars(const fof<B>& f, function<sym_t(sym_t)> g) {
	fof<B> r(false);
	for (const clause<B>& c : f) r = transform_vars<B>(c, g) | r;
	return r;
}

template<typename B> fof<B> shr(const fof<B>& f) {
	static auto g = [](sym_t v)->sym_t { return v >= 0 ? v + 1 : v; };
	return transform_vars(f, g);
}

template<typename B> fof<B> shl(const fof<B>& f) {
	static auto g = [](sym_t v)->sym_t { return v >= 0 ? v - 1 : v; };
	return transform_vars(f, g);
}

template<typename B>
ostream& operator<<(ostream& os, const clause<B>& c) {
	for (auto& t : c[0]) os << t << " = 0" << endl;
	for (auto& t : c[1]) os << t << " != 0" << endl;
	return os;
}

template<typename B>
ostream& operator<<(ostream& os, const fof<B>& f) {
	if (f == fof<B>::zero()) os << "F";
	if (f == fof<B>::one()) os << "T";
	size_t n = f.size();
	for (const clause<B>& c : f) os << c << (--n ? "\t||" : "") << endl;
	return os;
}
#endif
