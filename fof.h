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

template<typename B> clause<B>
operator&(const clause<B>& x, const clause<B>& y) {
	//DBG(cout << x << "&&" << y << " = ";)
	clause<B> z;
	z[1] = x[1];
	assert(x[0].size() <= 1);
	assert(y[0].size() <= 1);
	if (!x[0].empty()) {
		if (y[0].empty()) z[0] = x[0];
		else {
			term<bf<Bool>> t1 = *x[0].begin();
			assert(t1.t == term<bf<B>>::ELEM);
			term<bf<Bool>> t2 = *y[0].begin();
			assert(t2.t == term<bf<B>>::ELEM);
			auto t = t1.e | t2.e;
			if (t != bf<Bool>::one()) z[0].insert(t);
		}
	} else z[0] = y[0];
	for (auto& t : y[1])
		if (auto it = z[0].find(t); it != z[0].end())
			return minterm<bf<B>>();
		else z[1].insert(t);
	//DBG(cout << z << endl;)
	return z;
}

template<typename B>
clause<B> ex(const clause<B>& c, const sym_t& v) {
	if (c[0].empty()) {
		assert(!c[1].empty());
		clause<B> r;
		for (const term<bf<B>>& t : c[1]) {
			assert(t.t == term<bf<B>>::ELEM);
			r = r & clause<B>(false, ex(t.e, v));
		}
		return r;
	}
	if (c[1].empty()) {
		assert(c[0].size() == 1);
		assert(c[0].begin()->t == term<bf<B>>::ELEM);
		return clause<B>(true, all(c[0].begin()->e, v));
	}
	clause<B> r;
	assert(c[0].size() == 1);
	bf<B> f0 = c[0].begin()->e;
	//DBG(cout << "f0: " << f0 << endl;)
	r[0] = {all(f0, v)};
	//DBG(cout<< "r0: " << *r[0].begin() << endl;)
	bf<B> f1 = subst(f0, v, bf<B>::one());
	//DBG(cout<< "f1: " << f1 << endl;)
	f0 = subst(f0, v, bf<B>::zero());
	//DBG(cout<< "f0: " << f0 << endl;)
	for (const term<bf<B>>& t : c[1])
		r = r & clause<B>(false, subst(t.e, v, f0) | subst(t.e, v, ~f1));
	return r;
}

template<typename B>
fof<B> ex(const fof<B>& f, const sym_t& v) {
	fof<B> g = fof<B>::zero();
	for (const clause<B>& c : f) g = ex(c, v) | g;
	return g;
}

template<typename B>
fof<B> all(const fof<B>& f, const sym_t& v) { return ~ex(~f, v); }

template<typename B>
term<B> term_trans_vars(const term<B>& t, function<sym_t(sym_t)> g) {
//	cout << "in: " << t << endl;
	if (t.t == term<B>::VAR) return term<B>(g(t.sym));
	if (t.t == term<B>::FUNC) {
		term<B> r;
		r.t = term<B>::FUNC;
		r.name = t.name;
		for (auto& a : t.args)
			if (a.ist)
				r.args.emplace_back(term_trans_vars<B>(a.t, g));
			else r.args.push_back(a);
//		cout << "out: " << r << endl;
		return r;
	}
//	cout << "out: " << t << endl;
	return t;
}

template<typename B>
minterm<B> mt_trans_vars(const minterm<B>& m, function<sym_t(sym_t)> g) {
	minterm<B> b;
	for (size_t j = 0; j != 2; ++j)
		for (const term<B>& t : m[j])
			b = b & minterm<B>(!j,
				term_trans_vars(t, g));
	return b;
}

template<typename B>
clause<B> transform_vars(const clause<B>& c, function<sym_t(sym_t)> g) {
	clause<B> d;
	for (size_t i = 0; i != 2; ++i)
		for (const term<bf<B>>& s : c[i]) {
			assert(s.t == term<bf<B>>::ELEM);
			const bf<B>& h = s.e;
			bf<B> p;
			for (const minterm<B>& m : h)
				p = mt_trans_vars(m, g) | p;
			d = d & clause<B>(!i, p);
		}
	return d;
}

template<typename B>
fof<B> transform_vars(const fof<B>& f, function<sym_t(sym_t)> g) {
	fof<B> r;
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
	size_t n = f.size();
	for (const clause<B>& c : f) os << c << (--n ? "\t||" : "") << endl;
	return os;
}
#endif
