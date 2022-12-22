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
#include <vector>
#include <map>
#include <set>
#include <ostream>
#include <array>
#include <string>

using namespace std;

struct Bool {
	bool b;
	Bool() {}
	Bool(bool b) : b(b) {}
	static const Bool& zero() { static Bool b(false); return b; }
	static const Bool& one() { static Bool b(true); return b; }
	const Bool& operator&(const Bool& t) const {return b&t.b?one():zero();}
	const Bool& operator|(const Bool& t) const {return b|t.b?one():zero();}
	const Bool& operator~() const { return b ? zero() : one(); }
	bool operator<(const Bool& t) const { return b < t.b; }
};

template<typename elem> struct bf;
template<typename elem> struct term {
	struct arg {
		bool ist;
		term t;
		bf<elem> f;
		arg() {}
		arg(const term& t);
		arg(const bf<elem>& f);
		bool operator==(const arg&) const;
		bool operator<(const arg&) const;
	};
	enum type { ELEM, VAR, FUNC } t;
	//int sym = 0;
	string sym;
	elem e;
	vector<arg> args;
	term() {}
	term(const elem& e) : t(ELEM), e(e) {}
	term(type t) : t(t) {}
	term(const string& sym) : t(VAR), sym(sym) {}
	term(const string& sym, const vector<arg>& a) :
		t(FUNC), sym(sym), args(a) {}
	bool operator==(const term& x) const;
	bool operator<(const term& x) const;
};

template<typename elem> struct minterm : public array<set<term<elem>>, 2> {
	typedef array<set<term<elem>>, 2> base;

	minterm() : base() {}
	minterm(bool pos, const term<elem>& t);
};

template<typename elem> struct bf : public set<minterm<elem>> {
	enum { ZERO, ONE, NONE } v;

	bf() : set<minterm<elem>>(), v(NONE) {}
	bf(const term<elem>& t) : set<minterm<elem>>({minterm<elem>(true, t)}),
		v(NONE) {}
	bf(const minterm<elem>& t) : set<minterm<elem>>({t}), v(NONE) {}
	bf(bool b) { v = b ? ONE : ZERO; }

	bool operator==(const bf<elem>& f) const;
	bool operator<(const bf<elem>& f) const;
	static const bf<elem>& zero();
	static const bf<elem>& one();
};

typedef bf<Bool> sbf;
typedef minterm<sbf> clause;
typedef bf<sbf> fof;

template<typename elem> bf<elem> operator~(const minterm<elem>&);
template<typename elem>
minterm<elem> operator&(const minterm<elem>&, const minterm<elem>&);
template<typename elem>
bool operator<=(const minterm<elem>&, const minterm<elem>&);
template<typename elem>
bool operator<=(const minterm<elem>&, const bf<elem>&);
template<typename elem>
bf<elem> operator|(const minterm<elem>&, const bf<elem>&);
template<typename elem> bf<elem> operator~(const bf<elem>&);
template<typename elem> bf<elem> operator&(const bf<elem>&, const bf<elem>&);
template<typename elem> bf<elem> operator|(const bf<elem>&, const bf<elem>&);
template<typename elem> bool operator<=(const bf<elem>&, const bf<elem>&);
template<typename elem>
term<elem>::arg subst(const typename term<elem>::arg&, int, const bf<elem>&);
template<typename elem> term<elem> subst(const term<elem>&, int, const bf<elem>&);
template<typename elem> bf<elem> subst(const minterm<elem>&, int, const bf<elem>&);
template<typename elem> bf<elem> subst(const bf<elem>&, int, const bf<elem>&);
fof operator~(const clause&);
fof operator~(const fof&);
fof operator&(const fof&, const fof&);
fof operator|(const fof&, const clause&);
fof operator|(const fof&, const fof&);
bool operator<=(const clause&, const clause&);
bool operator<=(const clause& c, const clause& d) {return operator<=<sbf>(c,d);}
bool operator<=(const clause& c, const fof& f) { return operator<=<sbf>(c, f); }
fof operator|(const fof& x, const fof& y) { return operator|<sbf>(x, y); }
fof operator&(const fof& x, const fof& y) { return operator&<sbf>(x, y); }
clause subst(const clause&, int, const term<Bool>&);
fof subst(const fof&, int, const term<Bool>&);
fof all(const fof&, int);
fof ex(const fof&, int);

template<typename elem>
ostream& operator<<(ostream& os, const typename term<elem>::arg& a);
template<typename elem>
ostream& operator<<(ostream& os, const term<elem>& t);
template<typename elem>
ostream& operator<<(ostream& os, const minterm<elem>& x);
template<typename elem>
ostream& operator<<(ostream& os, const bf<elem>& f);
