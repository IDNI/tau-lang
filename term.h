#ifndef __TERM_H__
#define __TERM_H__
#include "defs.h"
#include <vector>
#include <string>
#include <cassert>

template<typename B> struct term;
template<typename B> struct bf;

template<typename B> struct term {
	struct arg {
		bool ist;
		term<B> t;
		bf<B> f;
		arg(const term& t);
		arg(const bf<B>& f);
		bool operator==(const arg&) const;
		bool operator<(const arg&) const;
		arg subst(const sym_t& s, const bf<B>& f) const {
			return ist ? arg(t.subst(s, f)) : *this;
		}
		arg subst(const term<B>& s, const bf<B>& f) const {
			return ist ? arg(t.subst(s, f)) : *this;
		}
	};
	enum type { ELEM, VAR, BF, FUNC } t;
	//int sym = 0;
	sym_t sym;
	string name;
	B e;
	bf<B> f;
	vector<arg> args;
	//term() {}
	term(const B& e) :
		t(ELEM), sym(0), name(), e(e),
		f(bf<B>::zero()), args() {}
//	term(type t) : t(t) {}
	term(const sym_t& sym) :
		t(VAR), sym(sym), name(), e(),
		f(bf<B>::zero()), args() {}
	//term(const sym_t& sym, const vector<arg>& a) :
	//	t(FUNC), sym(sym), args(a) {}
	term(const string& name, const vector<arg>& a) :
		t(FUNC), sym(0), name(name), e(B::zero()),
		f(bf<B>::zero()), args(a) {}

	static term<B> mk(const bf<B>& f) {
		if (f.v == bf<B>::ZERO) return term<B>(B::zero());
		if (f.v == bf<B>::ONE) return term<B>(B::one());
		DBG(assert(!f.empty()));
		if (f.size() == 1) {
			auto& c = *f.begin();
			if (c[1].empty() && c[0].size() == 1)
				return term<B>(*c[0].begin());
		}
		return term<B>(f);
	}
	bool operator==(const term& x) const;
	bool operator<(const term& x) const;
	term subst(const term<B>&, const bf<B>&) const;
	bool zero() const {
		switch (t) {
			case ELEM: return e == B::zero();
			case BF: return f.v == bf<B>::ZERO;
			default: return false;
		}
	}
	bool one() const {
		switch (t) {
			case ELEM: return e == B::one();
			case BF: return f.v == bf<B>::ONE;
			default: return false;
		}
	}
	bool hasterm(const term<B>& v) const;
private:
	term(const bf<B>& f) :
		t(BF), sym(0), name(), e(B::zero()),
		f(f), args() {}
};

template<typename B> term<B>::arg::arg(const term<B>& t) :
	ist(true), t(t), f(false) {}

template<typename B> term<B>::arg::arg(const bf<B>& f) :
	ist(false), f(bf<B>(f)) {}

template<typename B> bool term<B>::arg::operator==(
		const term<B>::arg& a) const {
	return ist != a.ist ? false : ist ? t == a.t : f == a.f;
}

template<typename B> bool term<B>::arg::operator<(
		const term<B>::arg& a) const {
	return a.ist != ist ? ist : ist ? a.t < t : a.f < f;
}

template<typename B> bool term<B>::operator==(const term& x) const {
	if (t != x.t) return false;
	switch (t) {
		case ELEM: return e == x.e;
		case VAR: return sym == x.sym;
		case BF: return f == x.f;
		case FUNC: return name == x.name && args == x.args;
		default: assert(0);
	}
}

template<typename B> bool term<B>::operator<(const term& x) const {
	if (t != x.t) return t < x.t;
	switch (t) {
		case ELEM: return e < x.e;
		case VAR: return sym < x.sym;
		case BF: return f < x.f;
		case FUNC: return name == x.name ? args < x.args : name < x.name;
		default: assert(0);
	}
}

template<typename B> bool term<B>::hasterm(const term<B>& v) const {
	switch (t) {
		case VAR: return v == *this;
		case ELEM: return false;
		case FUNC:
			for (auto& x : args)
				if 	((x.ist && x.t.hasterm(v)) ||
					(!x.ist && x.f.hasterm(v)))
				return true;
			return false;
		case BF: return f.hasterm(v);
		default: throw 0;
	}
}

template<typename B> ostream& operator<<(ostream& os, const term<B>& t) {
	if (t.t == term<B>::ELEM) return os << t.e;
	if (t.t == term<B>::VAR) return os << "x[" << t.sym << "]";
	if (t.t == term<B>::BF) return os << t.f;
	if (t.t == term<B>::FUNC) os << t.name << "(";//"f[" << t.sym << "](";
	for (size_t n = 0; n != t.args.size(); ++n) {
		out<B>(os, t.args[n]);
		//os << t.args[n]; -- compiler error somehow
		os << (n == t.args.size() - 1 ? "" : ",");
	}
	return os << ")";
}

template<typename B>
ostream& operator<<(ostream& os, const typename term<B>::arg& a) {
	return a.ist ? os << a.t : os << a.f;
}

template<typename B> void out(ostream& os, const typename term<B>::arg& a) {
	if (a.ist) os << a.t; else os << a.f;
}

template<typename B>
term<B> term<B>::subst(const term<B>& s, const bf<B>& g) const {
	if (*this == s) return term<B>(g);
	if (t == term<B>::BF) return f.subst(s, g);
	DBG(assert(t == term<B>::FUNC);)
	vector<typename term<B>::arg> v;
	for (auto& a : args) v.push_back(a.subst(s, g));
	return term<B>(name, v);
}
#endif
