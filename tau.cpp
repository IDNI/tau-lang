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
#include "anf.h"
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
	typedef hbdd<Bool> sbf;
	typedef tau<Bool, sbf> fof;
	sbf f = ("a"s & "x"s) | ("b"s & "x'"s);
	sbf g = ("c"s & "x"s) | ("d"s & "x'"s);
	cout << f << endl << g << endl << g->subst(dict("x"), f) << endl;
	g = g->subst(dict("x"), f);
	cout << g->sub0(dict("x")) << endl << g->sub1(dict("x")) << endl;
	cout << (g = (g->sub0(dict("x")) | g->sub1(dict("x")))) << endl;
	cout << anf(g) << endl;
	anf(g).verify();
	return 0;
	cout << (fof(true) += f).ex(dict("x")) << endl;
	cout << ((fof(true) += f) -= g).ex(dict("x")) << endl;
	return 0;
/*	tau<Bool, hbdd<Bool>> t(true);
	hbdd<Bool> f = "x"s & "y"s;
//	cout << f << endl;
//	cout << (f | "y'"s )<< endl;
	t += f;
//	cout << t;
	t -= "x"s & "y'"s;
	cout << t;
	cout << t.ex(dict("x"));*/
}
