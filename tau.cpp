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
#include "out.h"
#include "builder.h"
#include <iostream>

template<typename B> vector<bdd<B>> bdd<B>::V;
template<typename B> unordered_map<bdd_node, size_t> bdd<B>::Mn;
template<typename B> map<B, size_t> bdd<B>::Mb;
template<typename B> int_t bdd<B>::T;
template<typename B> int_t bdd<B>::F;
template<typename B>
unordered_map<bdd_node, std::shared_ptr<bdd_handle<B>>> bdd_handle<B>::Mn;
template<typename B>
map<B, std::shared_ptr<bdd_handle<B>>> bdd_handle<B>::Mb;
template<typename B> hbdd<B> bdd_handle<B>::htrue;
template<typename B> hbdd<B> bdd_handle<B>::hfalse;
//template<typename B> bdd<B>::initializer bdd<B>::I;

int main() {
	tau<Bool, hbdd<Bool>> t(true);
	t += "x"s & "y'"s;
	t -= "x'"s & "y"s;
	cout << t;
}

/*#include "fof.h"
//#include "builder.h"
#include "fof.h"
#include "bdd.h"
#include "seq.h"
#include <iostream>
#include <sstream>

using namespace idni;

term<bf<Bool>> var_term(int v) {
	return term<bf<Bool>>(v);
//	stringstream ss;
//	ss << "x[" << v << "]";
//	return term<bf<Bool>>(ss.str());
}

bf<bf<Bool>> var(int v) { return bf<bf<Bool>>(minterm<bf<Bool>>(true, v)); }

term<bf<Bool>> fapp(const string& fname, const vector<sym_t>& args) {
	vector<term<bf<Bool>>::arg> v;
	for (const sym_t& s : args) v.emplace_back(term<bf<Bool>>(s));
	return term<bf<Bool>>(fname, v);
}

term<bf<Bool>> fapp(size_t i, size_t j, size_t v) {
	stringstream ss;
	ss << "f[" << i << ',' << j << ']';
	vector<term<bf<Bool>>::arg> a;
	for (size_t n = 0; n != v; ++n) a.push_back(var_term(n));
	return term<bf<Bool>>(ss.str(), a);
}

template<typename B> bf<B> generic(size_t nv, size_t c = 0) {
	bf<B> r(false);
	for (size_t v = 0; v != (((size_t)1) << nv); ++v) {
		minterm<B> m(true, term<B>(-v-1-c));
		for (size_t k = 0; k != nv; ++k)
			m[(v & (1 << k)) ? 0 : 1].insert(term<B>(k));
		r = m | r;
	}
	return r;
}

template<typename B> fof<bf<B>> generic(size_t nc, size_t csz, size_t nv) {
	fof<bf<B>> f(false);
	for (size_t k = 0; k != nc; ++k) {
		fof<bf<B>> c(clause<bf<B>>() += generic<B>(nv, (1<<nv)*k));
		for (size_t n = 1; n != csz; ++n)
			c = (clause<bf<B>>() -=
				generic<B>(nv, (1<<nv) * (nc * n + k))) & c;
		//cout << "c: " << c << endl;
		f = c | f;
		//cout << "f: " << f << endl;
	}
	return f;
}

void test(const fof<bf<Bool>>& f, const fof<bf<Bool>>& g) {
	if (f == g && (f + g) == fof<bf<Bool>>(false)) {
		cout << "passed" << endl;
		return;
	}
	cout << "failed" << endl;
	cout << "f: " << f << endl;
	cout << "g: " << g << endl;
	cout << "f': " << (~f) << endl;
	cout << "g': " << (~g) << endl;
	cout << "f'|g': " << ((~f)|(~g)) << endl;
	cout << "ff': " << (f&~f) << endl;
	cout << "f+g: " << (f+g) << endl;
	cout << "f&g': " << (f&~g) << endl;
	assert(f == g);
	assert((f + g) == fof<bf<Bool>>(false));
}

void test() {
	fof<bf<Bool>> f, g;
	f = ("x"_v) << 0;
	cout << (f & ~f) << endl;
	test(fof<bf<Bool>>(false), f & ~f);
	test((("x"_v) + ("x'"_v)) <<= 1,
		(("x"_v) | ("x'"_v)) <<= 1);
	cout << ((("a"_v & "x"_v) | ("b"_v & "x'"_v)) <<= 0) << endl;
	g = (("a"_v & "x"_v) | ("b"_v & "x'"_v)) <<= 0;
	f = (("a"_v & "x"_v) + ("b"_v & "x'"_v)) <<= 0;
	test(f, g);		
	// ex(x, ax+bx'=0) <=> ab=0
	test(ex("x", (("a"_v & "x"_v) + ("b"_v & "x'"_v)) <<= 0),
		("a"_v & "b"_v) <<= 0);
	// ex(x, ax+bx'!=0) <=> a!=0 | b!=0
	test(ex("x", (("a"_v & "x"_v) + ("b"_v & "x'"_v)) << 0),
			("a"_v << 0) | ("b"_v << 0));
	// ax+bx'=0 <=> a<=x<=b'
}

int main() {
	bdd::init();
//	test();
//	cout << generic<bf<Bool>>(2, 2, 2) << endl;
	//cout << generic<bf<Bool>>(2) << endl;
	//cout << generic<bf<Bool>>(3) << endl;
//	return 0;
//	bf<bf<Bool>> f(fapp("f", {0,1}));
//	bf<bf<Bool>> g(fapp("g", {1,2}));
//	cout << (~(f & g)) << endl;
//	cout << ex(ex(~(~fof<bf<Bool>>(f) | fof<bf<Bool>>(g)), 0),1) << endl;
//	cout << (generic(2, 3, 2) & generic(1,1,1)) << endl;
//	cout << fapp(0, 0, 1) << endl;
//	bf<B> f = subst(fapp(0, 0, 1), string("x[0]"), bf<B>(term<bf<Bool>>(string("y"))));
//	f = subst(fapp(0, 0, 1), string("x[0]"), bf<B>(fapp(1,1,2)));
//	cout << f << endl;
//	cout << var(1) << endl;
//	cout << (~var(1)) << endl;
//	cout << (var(1) | (~var(1))) << endl;
//	cout << ((var(1) & (~var(2))) | (var(1) & var(2))) << endl;
//	cout << generic<bf<Bool>>(2,2,1) << endl; return 0;
//	auto f = [](sym_t v)->sym_t { return 1+v; };
//	cout << transform_vars(generic<bf<Bool>>(2,2,3), f) << endl;
//	cout << generic<bf<Bool>>(1,1,1) << endl;
//	cout << generic<bf<Bool>>(1,1,1).subst(0,term<bf<Bool>>(1)) << endl;
//	cout << generic<bf<Bool>>(2) << endl;
//	cout << all(generic<bf<Bool>>(2),0) << endl;
	seq<Bool>(generic<Bool>(2,2,2));
	return 0;
}*/
