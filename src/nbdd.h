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
#ifndef __NBDD_H__
#define __NBDD_H__
#include "bdd_handle.h"
​
template<typename T, typename B> struct nbdd { // bdd with names
	typedef T name_type;
	typedef B leaf_type;
	hbdd<B> b;
	nbdd(bool t) : b(t ? bdd_handle<B>::htrue : bdd_handle<B>::hfalse) {}
	typedef B b_type;
	inline static map<T, int_t> M;
	inline static vector<T> V;
	inline static const T& get(int_t n) { return V[n]; }
	inline static int_t get(const T& t) {
		if (auto it = M.find(t); it != M.end())
			return it->second;
		return M.emplace(t, V.size()), V.push_back(t), (V.size() - 1);
	}
	inline static nbdd bit(bool b, const T& t) {
		return hbdd<B>::bit(b, get(t));
	}
	inline static void init() { bdd_init<B>(); }
	nbdd operator&(const nbdd& x) const { return b & x.b; }
	nbdd operator|(const nbdd& x) const { return b | x.b; }
	nbdd operator~() const { return ~b; }
	bool operator==(bool t) const { return (b == t); }
	static nbdd ite(const nbdd& x, const nbdd& y, const nbdd& z);
	static nbdd ite(const nbdd& x, const B& y, const nbdd& z);
	static nbdd ite(const nbdd& x, const nbdd& y, const B& z);
	static nbdd ite(const nbdd& x, const B& y, const B& z);
​
	typedef vector<const T*> pnames;
	typedef function<bool(const B&, const pnames&, const pnames&)>
		cb_dnf; // leaf, pos, neg
​
​
	void dnf(cb_dnf f) const {
		b->dnf([f](const B& b, const vector<int_t>& v) {
			pnames pos, neg;
			for (int_t i : v)
				(i > 0 ? pos : neg).push_back(&V[abs(i)]);
			f(b, pos, neg);
		});
	}
private:
	nbdd(const hbdd<B>& b) : b(b) {}
};
#endif