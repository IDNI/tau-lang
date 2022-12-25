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
#include "ba.h"

template<typename B> using clause = minterm<bf<B>>;
template<typename B> using fof = bf<bf<B>>;

template<typename B> clause<B> simplify(const clause<B>& c) {
	if (c[0].empty()) return c;
	clause<B> d;
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
	return d;
}

template<typename B>
clause<B> operator&(const clause<B>& x, const clause<B>& y) {
	static map<array<clause<B>, 2>, clause<B>> M;
	static size_t hits = 0, misses = 0;
	if (y < x) return y & x;
	if (auto it = M.find({x, y}); it != M.end()) return ++hits, it->second;
	++misses;
	if ((hits + misses)%1000 == 0) cout << "hits: " << hits << " misses: "
	       	<< misses << endl;
	//DBG(cout << x << "&&" << y << " = ";)
	clause<B> z(set<term<bf<B>>>(), x[1]);
//	z[1] = x[1];
	assert(x[0].size() <= 1 && y[0].size() <= 1);
	if (!x[0].empty()) {
		if (y[0].empty()) z[0].insert(*x[0].begin());
		else {
			term<bf<Bool>> t1 = *x[0].begin(), t2 = *y[0].begin();
			assert(t1.t==term<bf<B>>::ELEM&&t2.t==term<bf<B>>::ELEM);
			auto t = t1.e | t2.e;
			if (t == bf<Bool>::one()) return clause<B>();
			else z[0].insert(t);
		}
	} else z[0] = y[0];//.push_back(*y[0].begin());
	for (auto& t : y[1])
		if (!z[0].empty() && t.e <= z[0].begin()->e) return clause<B>();
		else if (t.e == bf<Bool>::one()) continue;
		else if (t.e == bf<Bool>::zero()) throw 0;
		else {
			bool b = true;
			for (auto& s : z[1]) if (!(b &= !(s.e <= t.e))) break;
			if (b) z[1].insert(t);
		}
	//DBG(cout << z << endl;)
	M.insert({array<clause<B>, 2>{x, y}, z = simplify(z)});
	return z;
}

template<typename B>
fof<B> c2fof(const clause<B>& c) {
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
	assert(!c[0].empty() || !c[1].empty());
#ifdef BREAK_BF
	bool b = !c[1].empty();
/*	for (const auto& t : c[0])
		if (!(b &= (t.f.size() == 1)))
			break;*/
	if (b) for (const auto& t : c[1])
		if (!(b &= (t.f.size() == 1)))
			break;
	if (b) return (f | c2fof(c));
#endif
//	cout << "disj with " << f.size() << " clauses." << endl;
	if (!c[0].empty()) {
		const bf<B>& g = c[0].begin()->e;
		if (g == bf<B>::zero()) c[0].clear();
		else if (g == bf<B>::one()) return f;
	}
	for (auto it = c[1].begin(); it != c[1].end(); ++it) {
		const bf<B>& g = it->e;
		if (g == bf<B>::one()) c[1].erase(it);
		else if (g == bf<B>::zero()) return f;
	}
//	if (fof(c) <= f) return f;
	return disj_fmt(c, f);
}

template<typename B>
fof<B> ex(const clause<B>& c, const sym_t& v) {
	if (c[0].empty()) {
		DBG(assert(!c[1].empty());)
		clause<B> r;
		for (const term<bf<B>>& t : c[1]) {
			assert(t.t == term<bf<B>>::ELEM);
			bf<B> f = ex(t.e, v);
			if (f == bf<B>::one()) return fof<B>::zero();
			r = r & clause<B>(false, f);
		}
		return r;
	}
	if (c[1].empty()) {
		DBG(assert(c[0].size() == 1);)
		DBG(assert(c[0].begin()->t == term<bf<B>>::ELEM);)
		bf<B> f = all(c[0].begin()->e, v);
		return f.empty() ? fof<B>(true) : clause<B>(true, f); 
	}
	assert(c[0].size() == 1);
	bf<B> f0 = c[0].begin()->e;
	clause<B> r(true, all(f0, v));
	bf<B> f1 = f0.subst(v, bf<B>::one());
	f0 = f0.subst(v, bf<B>::zero());
	for (const term<bf<B>>& t : c[1])
		r = r & clause<B>(false, t.e.subst(v, f0) | t.e.subst(v, ~f1));
	return r;
}

template<typename B> fof<B> ex(const fof<B>& f, const sym_t& v) {
	fof<B> g(false);
//	size_t n = 0;
	for (const clause<B>& c : f) {
//		clause<B> d = ex(c, v);
//		cout << ++n << ' '<< d << endl;
		g = ex(c, v) | g;
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
			if (a.ist)
				v.emplace_back(term_trans_vars<B>(a.t, g));
			else v.push_back(a);
//		cout << "out: " << r << endl;
		return term<B>(t.name, v);
	}
//	cout << "out: " << t << endl;
	return t;
}

template<typename B>
minterm<B> mt_trans_vars(const minterm<B>& m, function<sym_t(sym_t)> g) {
	minterm<B> b;
	for (size_t j = 0; j != 2; ++j)
		for (const term<B>& t : m[j])
			b = (b & minterm<B>(!j, term_trans_vars(t, g)));
	return b;
}

template<typename B>
clause<B> transform_vars(const clause<B>& c, function<sym_t(sym_t)> g) {
	clause<B> d;
	for (size_t i = 0; i != 2; ++i)
		for (const term<bf<B>>& s : c[i]) {
			assert(s.t == term<bf<B>>::ELEM);
			bf<B> p(false);
			for (const minterm<B>& m : s.e)
				p = mt_trans_vars(m, g) | p;
			d = d & clause<B>(!i, p);
		}
	return d;
}

template<typename B>
fof<B> transform_vars(const fof<B>& f, function<sym_t(sym_t)> g) {
	fof<B> r(false);
	for (const clause<B>& c : f) r = transform_vars<B>(c, g) | r;
	return r;
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
