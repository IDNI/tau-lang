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

// basic macro for conditional execution of code
#ifdef DEBUG
#define DBG(x) x
#include <cxxabi.h>
#else
#define DBG(x)
#endif

#include <algorithm>
#include <iostream>

#define pfst(x) (*(x).begin())
#define hasbc(x, y, f) std::binary_search(x.begin(), x.end(), y, f)

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

//-----------------------------------------------------------------------------
// GIT_* macros are populated at compile time by -D or they're set to "n/a"
#ifndef GIT_DESCRIBED
#define GIT_DESCRIBED   "n/a"
#endif
#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "n/a"
#endif
#ifndef GIT_BRANCH
#define GIT_BRANCH      "n/a"
#endif

//-----------------------------------------------------------------------------
// use: os << COLOR BLUE << "blue text" << COLOR CLEAR;
// or:  os << COLOR BRIGHT AND WHITE << "bright and white text" << COLOR CLEAR;
#define COLOR      "\033["
#define BRIGHT     "1"
#define UNDERLINED "4"
#define FLASHING   "5"
#define AND        ";"
#define CLEAR      "0m"
#define BLACK      "30m"
#define RED        "31m"
#define GREEN      "32m"
#define YELLOW     "33m"
#define BROWN      "33m"
#define BLUE       "34m"
#define PURPLE     "35m"
#define CYAN       "36m"
#define WHITE      "37m"
#define GRAY       "37m"

#endif // __DEF_H__
