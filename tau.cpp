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
#include "fof.h"
#include "seq.h"
#include <iostream>

term<Bool> var_term(int v) {
	return term<Bool>(v);
//	stringstream ss;
//	ss << "x[" << v << "]";
//	return term<Bool>(ss.str());
}

bf<Bool> var(int v) { return bf<Bool>(minterm(true, var_term(v))); }

term<Bool> fapp(const string& fname, const vector<sym_t>& args) {
	vector<term<Bool>::arg> v;
	for (const sym_t& s : args) v.emplace_back(term<Bool>(s));
	return term<Bool>(fname, v);
}

term<Bool> fapp(size_t i, size_t j, size_t v) {
	stringstream ss;
	ss << "f[" << i << ',' << j << ']';
	vector<term<Bool>::arg> a;
	for (size_t n = 0; n != v; ++n) a.push_back(var_term(n));
	return term<Bool>(ss.str(), a);
}

template<typename B>
fof<B> generic(size_t nc, size_t csz, size_t nv) {
	fof<B> f = fof<B>::zero();
	for (size_t k = 0; k != nc; ++k) {
		fof<B> g = fof<B>::one();
		for (size_t n = 0; n != csz; ++n)
			g = clause<B>(!n, term<bf<B>>(bf<B>(fapp(k,n,nv)))) & g;
		f = f | g;
	}
	return f;
}

int main() {
//	bf<B> f(fapp("f", {"x", "y"}));
//	bf<B> g(fapp("g", {"y", "x"}));
//	cout << (~(f & g)) << endl;
//	cout << (generic(2, 3, 2) & generic(1,1,1)) << endl;
//	cout << fapp(0, 0, 1) << endl;
//	bf<B> f = subst(fapp(0, 0, 1), string("x[0]"), bf<B>(term<Bool>(string("y"))));
//	f = subst(fapp(0, 0, 1), string("x[0]"), bf<B>(fapp(1,1,2)));
//	cout << f << endl;
//	cout << var(1) << endl;
//	cout << (~var(1)) << endl;
//	cout << (var(1) | (~var(1))) << endl;
//	cout << ((var(1) & (~var(2))) | (var(1) & var(2))) << endl;
	cout << generic<Bool>(5,5,1) << endl;
	auto f = [](sym_t v)->sym_t { return 1+v; };
	cout << transform_vars(generic<Bool>(2,2,3), f) << endl;
	seq(generic<Bool>(5,15,2));
//	cout << ex(generic<Bool>(2,2,1), 0) << endl;
//	cout << ex(ex(generic<Bool>(2,2,2), 0), 1) << endl;
	return 0;
/*	cout << generic(2, 2, 2) << endl;
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
//	cout << f << endl;*/
}
