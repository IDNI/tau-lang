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
#include "dict.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
using namespace std;

vector<string> v({"dummy"});
map<string, size_t> m;

sym_t dict(const char* s) {
	if (auto it = m.find(s); it != m.end()) return it->second;
	return m.emplace(s, v.size()), v.push_back(s), v.size() - 1;
}

const char* dict(sym_t n) {
	if ((size_t)n >= v.size()) {
		static string tmp;
		stringstream ss;
		ss << "x[" << n << "]";
		return (tmp = ss.str()).c_str();
	}
	return v[n].c_str();
}

sym_t dict(const string& s) { return dict(s.c_str()); }
