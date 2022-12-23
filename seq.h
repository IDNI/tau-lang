#ifndef __SEQ_H__
#define __SEQ_H__
#include "fof.h"
#include <iostream>

template<typename B>
term<B> tmpvar(int n) {
/*	stringstream ss;
	ss << "_t[" << n << ']';
	return ss.str();*/
	return n;
}

template<typename B>
vector<term<B>> to_vars(const vector<sym_t>& v) {
	vector<term<B>> r;
	for (size_t n = 0; n != v.size(); ++n) r.emplace_back(v[n]);
	return r;
}

template<typename B>
void rename(fof<B>& f, const vector<term<B>>& x, const vector<term<B>>& y) {
	for (size_t n = 0; n != x.size(); ++n)
		f = f.subst(x[n].sym, sbf(term<B>(y[n])));
}

template<typename B>
vector<term<B>> tmpvars(size_t n) {
	vector<term<B>> r;
	while (n--) r.push_back(tmpvar<B>(n));
	return r;
}

template<typename B>
fof<B> shr(fof<B> f, const vector<term<B>>& v) {
	vector<term<B>> t(tmpvars(v.size() - 1));
	for (size_t n = 0; n != v.size() - 1; ++n) f = f.subst(v[n].sym, t[n]);
	for (size_t n = 0; n != v.size() - 1; ++n) f = f.subst(t[n].sym, v[n+1]);
	return f;
}

template<typename B>
fof<B> shl(fof<B> f, const vector<term<B>>& v) {
	vector<term<B>> t(tmpvars(v.size() - 1));
	for (size_t n = 0; n != v.size() - 1; ++n) f = subst(f, v[n].sym, t[n]);
	for (size_t n = 1; n != v.size(); ++n) f = subst(f, t[n+1].sym, v[n-1]);
	return f;
}

template<typename B>
fof<B> iter(const fof<B>& f, const vector<term<B>>& v) {
	fof g = shr(f, v);
	return shl(ex(f & g, v[0].sym), v);
}

// remember to add additional var!!
template<typename B>
void seq(fof<B> f, const vector<sym_t>& vs) {
	set<fof<B>> s({f});
	auto v = to_vars<B>(vs);
	size_t n = 0;
	cout << f << endl;
	do {
		f = iter(f, v);
		cout << ++n << '\t' << f << endl;
		if (f == fof<B>::zero()) { cout << "unsat" << endl; break; }
	} while (s.insert(f).second);
}
#endif
