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

template struct bf<Bool>;
typedef bf<Bool> sbf;
template struct minterm<sbf>;
typedef minterm<sbf> clause;
template struct bf<sbf>;
typedef bf<sbf> fof;

fof operator~(const clause&);
fof operator~(const fof&);
fof operator&(const fof&, const fof&);
fof operator|(const fof&, const clause&);
fof operator|(const fof&, const fof&);
bool operator<=(const clause&, const clause&);
bool operator<=(const clause&, const clause&);
bool operator<=(const clause&, const fof&);
fof operator~(const clause&);
fof operator~(const fof&);
fof operator|(const fof&, const fof& y);
fof operator&(const fof&, const fof&);
clause subst(const clause&, const string&, const term<Bool>&);
fof subst(const fof&, const string&, const term<Bool>&);
clause ex(const clause&, const string&);
fof ex(const fof&, const string&);
fof all(const fof&, const string&);

bool operator<=(const clause& c, const clause& d) {return operator<=<sbf>(c,d);}
bool operator<=(const clause& c, const fof& f) { return operator<=<sbf>(c, f); }
fof operator~(const clause& x) { return operator~<sbf>(x); }
fof operator~(const fof& x) { return operator~<sbf>(x); }
fof operator|(const fof& x, const fof& y) { return operator|<sbf>(x, y); }
fof operator&(const fof& x, const fof& y) { return operator&<sbf>(x, y); }
fof all(const fof& f, const string& v) { return ~ex(~f, v); }

clause operator&(const clause& x, const clause& y) {
	//DBG(cout << x << "&&" << y << " = ";)
	clause z;
	z[1] = x[1];
	assert(x[0].size() <= 1);
	assert(y[0].size() <= 1);
	if (!x[0].empty()) {
		if (y[0].empty()) z[0] = x[0];
		else {
			term<bf<Bool>> t1 = *x[0].begin();
			assert(t1.t == term<sbf>::ELEM);
			term<bf<Bool>> t2 = *y[0].begin();
			assert(t2.t == term<sbf>::ELEM);
			auto t = t1.e | t2.e;
			if (t != bf<Bool>::one()) z[0].insert(t);
		}
	} else z[0] = y[0];
	for (auto& t : y[1])
		if (auto it = z[0].find(t); it != z[0].end())
			return minterm<sbf>();
		else z[1].insert(t);
	//DBG(cout << z << endl;)
	return z;
}

clause ex(const clause& c, const string& v) {
	if (c[0].empty()) {
		assert(!c[1].empty());
		clause r;
		for (const term<sbf>& t : c[1]) {
			assert(t.t == term<sbf>::ELEM);
			r = r & clause(false, ex(t.e, v));
		}
		return r;
	}
	if (c[1].empty()) {
		assert(c[0].size() == 1);
		assert(c[0].begin()->t == term<sbf>::ELEM);
		return clause(true, all(c[0].begin()->e, v));
	}
	clause r;
	assert(c[0].size() == 1);
	sbf f0 = c[0].begin()->e;
	r[0] = {all(f0, v)};
	sbf f1 = subst(f0, v, sbf::one());
	f0 = subst(f0, v, sbf::zero());
	for (const term<sbf>& t : c[1])
		r = r & clause(false, subst(t.e, v, f0) | subst(t.e, v, ~f1));
	return r;
}

fof ex(const fof& f, const string& v) {
	fof g = fof::zero();
	for (const clause& c : f) g = ex(c, v) | g;
	return g;
}

ostream& operator<<(ostream& os, const clause& c) {
	for (auto& t : c[0]) os << t << " = 0" << endl;
	for (auto& t : c[1]) os << t << " != 0" << endl;
	return os;
}

ostream& operator<<(ostream& os, const fof& f) {
	size_t n = f.size();
	for (const clause& c : f) os << c << (--n ? "\t||" : "") << endl;
	return os;
}
