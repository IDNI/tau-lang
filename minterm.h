#include "term.h"
#include "bool.h"
#include <set>
#include <map>
#include <array>

template<typename B> struct minterm : public array<set<term<B>>, 2> {
	typedef array<set<term<B>>, 2> base;

	minterm() : base() {}
	minterm(const set<term<B>>& x, const set<term<B>>& y) :
		base({x, y}) {}
	minterm(const minterm &m) { (*this)[0] = m[0]; (*this)[1] = m[1]; }
	minterm(minterm &&m) { (*this)[0] = move(m[0]); (*this)[1] = move(m[1]);}
	minterm(bool pos, const term<B>& t) : base() { add(pos, term<B>(t)); }
	void add(bool, const term<B>&);
	bf<B> subst(sym_t, const bf<B>& f) const;
	bf<B> subst(const term<B>& s, const bf<B>& f) const;
	bool operator<(const minterm& m) const {
		if ((*this)[0].size() != m[0].size())
			return (*this)[0].size() < m[0].size();
		if ((*this)[1].size() != m[1].size())
			return (*this)[1].size() < m[1].size();
		for (	auto it = (*this)[0].begin(), jt = m[0].begin();
			it != (*this)[0].end(); ++it, ++jt)
			if (*it != *jt) return *it < *jt;
		for (	auto it = (*this)[1].begin(), jt = m[1].begin();
			it != (*this)[1].end(); ++it, ++jt)
			if (*it != *jt) return *it < *jt;
		return false;
		return (base)*this < (base)m;
	}
	bool operator==(const minterm& m) const {
		return (*this)[0] == m[0] && (*this)[1] == m[1];
	}
	minterm& operator=(const minterm& m) {
		(*this)[0] = m[0];
		(*this)[1] = m[1];
		return *this;
	}
};

template<typename B> void minterm<B>::add(bool pos, const term<B>& t) {
	DBG(assert(!(t.t == term<B>::ELEM && t.e == B::zero()));)
	(*this)[pos ? 0 : 1].insert(t);
}

template<typename B> bf<B> minterm<B>::subst(sym_t s, const bf<B>& f) const {
	return subst(term<B>(s), f);
}

template<typename B>
bf<B> minterm<B>::subst(const term<B>& s, const bf<B>& f) const {
	struct item {
		item(const minterm<B>& m, const term<B>& t, const bf<B>& f) :
			m(m), t(t), f(f) {}
		minterm<B> m;
		term<B> t;
		bf<B> f;
		bool operator<(const item& x) const {
			if (t != x.t) return t < x.t;
			if (m != x.m) return m < x.m;
			return f < x.f;
		}
	};
	static map<item, bf<B>> M;
	static size_t hits = 0, misses = 0;
	item a(*this, s, f);
	if (auto it = M.find(a); it != M.end()) return ++hits, it->second;
	++misses;
	if ((hits + misses)%100000 == 0)
		cout << "(S) hits: " << hits << " misses: " << misses << endl;
	if ((hits + misses)>1000000) M.clear();
	bf<B> r(true);
	for (auto& t : (*this)[0])
		if (t.t == term<B>::BF)
			r = (t.f.hasterm(s) ? t.f.subst(s, f) : bf<B>(t)) & r;
		else if (t.t == term<B>::FUNC)
			r = (t.hasterm(s) ? bf<B>(t.subst(s,f)) : bf<B>(t)) & r;
		else if (t.t == term<B>::VAR) {
			if (s.t == term<B>::VAR && t.sym == s.sym) r = r & f;
			else r = bf<B>(t) & r;
		} else if (t.t == term<B>::ELEM) r = bf<B>(t) & r;
		else throw 0;
	for (auto& t : (*this)[1])
		if (t.t == term<B>::BF)
			r = (t.f.hasterm(s) ? ~t.f.subst(s, f) : ~bf<B>(t)) & r;
		else if (t.t == term<B>::FUNC)
			r = (t.hasterm(s) ? ~bf<B>(t.subst(s,f)) : ~bf<B>(t))&r;
		else if (t.t == term<B>::VAR) {
			if (s.t == term<B>::VAR && t.sym == s.sym) r = r & ~f;
			else r = ~bf<B>(t) & r;
		} else if (t.t == term<B>::ELEM) r = ~bf<B>(t) & r;
		else throw 0;
	M.emplace(a, r);
	return r;
}

template<typename B> bf<B> operator&(
		const minterm<B>& x, const minterm<B>& y) {
//	typedef array<minterm<B>, 2> item;
	struct item {
		const minterm<B> x, y;
		item(const minterm<B>& x, const minterm<B>& y) : x(x), y(y) {}
		bool operator<(const item& a) const {
			if (x[0].size() != a.x[0].size())
				return x[0].size() < a.x[0].size();
			if (x[1].size() != a.x[1].size())
				return x[1].size() < a.x[1].size();
			if (y[0].size() != a.y[0].size())
				return y[0].size() < a.y[0].size();
			if (y[1].size() != a.y[1].size())
				return y[1].size() < a.y[1].size();
			return (x != a.x) ? x < a.x : y < a.y;

		}
	};
	if (x[0].empty() && x[1].empty()) return y;
	if (y[0].empty() && y[1].empty()) return x;
	//DBG(cout << x << "&&" << y << " = ";)
//	static map<item, bf<B>> M;
//	static size_t hits = 0, misses = 0;
//	item a = (x < y) ? item(x, y) : item(y, x);
//	if (auto it = M.find(a); it != M.end()) return ++hits, it->second;
//	++misses;
//	if ((hits + misses)%100000 == 0)
//		cout << "(M) hits: " << hits << " misses: " << misses << endl;
//	if ((hits + misses)>1000000) M.clear();
#ifdef DEBUG
	for (const term<B>& t : x[0]) assert(!t.zero() && !t.one());
	for (const term<B>& t : x[1]) assert(!t.zero() && !t.one());
	for (const term<B>& t : y[0]) assert(!t.zero() && !t.one());
	for (const term<B>& t : y[1]) assert(!t.zero() && !t.one());
#endif
	//cout << "(" << x << ") & (" << y << ") = " << endl;
	for (const term<B>& t : y[0])
		if (has(x[1], t)) return bf<B>(false);
	for (const term<B>& t : y[1])
		if (has(x[0], t)) return bf<B>(false);
	minterm<B> z = x;
	z[0].insert(y[0].begin(), y[0].end());
	z[1].insert(y[1].begin(), y[1].end());
	bf<B> r(move(z));
	//DBG(cout << z << endl;)
	//M.insert({a, r});
	return r;
}

template<typename B>
bool operator<=(const minterm<B>& x, const minterm<B>& y) { return (x&y) == x; }

template<typename B>
bool operator<=(const minterm<B>& t, const bf<B>& f) {
	if (f == bf<B>::one()) return true;
	if (f == bf<B>::zero()) return false;
	for (const minterm<B>& x : f) if (t <= x) return true;
//	cout << t << " is not leq " << f << endl;
	return false;
}

template<typename B>
bool complementary(const minterm<B>& x, minterm<B>& y) {
	size_t n0 = x[0].size(), n1 = x[1].size();
	size_t k0 = y[0].size(), k1 = y[1].size();
	const term<B> *t1 = 0;
	if (n0 == k0 + 1 && n1 + 1 == k1) {
		bool b = false;
		for (auto& t : x[0])
			if (has(y[0], t)) continue;
			else if (b) return false;
			else t1 = &t, b = true;
		b = false;
		for (auto& t : y[1])
			if (has(x[1], t)) continue;
			else if (b || t != *t1) return false;
			else b = true;
		return b ? y[1] = x[1], true : false;
	}
	if (n0 + 1 != k0 || n1 != k1 + 1) return false;
	bool b = false;
	for (auto& t : y[0])
		if (has(x[0], t)) continue;
		else if (b) return false;
		else t1 = &t, b = true;
	b = false;
	for (auto& t : x[1])
		if (has(y[1], t)) continue;
		else if (b || t != *t1) return false;
		else b = true;
	return b ? y[0] = x[0], true : false;
}

template<typename B> ostream& operator<<(ostream& os, const minterm<B>& x) {
	for (const term<B>& t : x[0]) os << t;
	for (const term<B>& t : x[1]) os << t << "'";
	return os;
}
