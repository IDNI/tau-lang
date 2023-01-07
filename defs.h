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
#ifndef __DEF_H__
#define __DEF_H__
typedef int sym_t;
#define DEBUG

#ifdef DEBUG
#define DBG(x) x
#include <iostream>
#include <cxxabi.h>
#else
#include <iostream>
#define DBG(x)
#endif

#define pfst(x) (*(x).begin())
using namespace std;

template<typename B> struct zero {
	bool operator()(const B&) const;
};

template<typename B> struct one {
	bool operator()(const B&) const;
};

template<typename T, typename V> bool has(const T& t, const V& v) {
	return t.find(v) != t.end();
}

template<typename T, typename V> bool hasv(const T& t, const V& v) {
	return std::find(t.begin(), t.end(), v) != t.end();
}
#endif
