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
clause<B> ex(const clause<B>& c, const string& v) {
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
fof<B> ex(const fof<B>& f, const string& v) {
	fof<B> g = fof<B>::zero();
	for (const clause<B>& c : f) g = ex(c, v) | g;
	return g;
}

template<typename B>
fof<B> all(const fof<B>& f, const string& v) { return ~ex(~f, v); }

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
