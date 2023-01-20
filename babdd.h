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
#ifndef __BABDD_H__
#define __BABDD_H__
#include "defs.h"
#include "bool.h"
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <variant>
#include <cstdint>
#include <cassert>
#include <memory>
#include <functional>
using namespace std;

typedef int32_t int_t;
template<typename T> using sp = shared_ptr<T>;

#define neg_to_odd(x) (((x)<0?(((-(x))<<1)+1):((x)<<1)))
#define hash_pair(x, y) fpairing(neg_to_odd(x), neg_to_odd(y))
#define hash_tri(x, y, z) fpairing(hash_pair(x, y), neg_to_odd(z))
#define hash_upair(x, y) fpairing(x, y)
#define hash_utri(x, y, z) fpairing(hash_upair(x, y), z)
// Define the default settings for the bdd class
#define bdd_param_defaults bool inv_in = true, bool inv_out = true, bool varshift = false
#define bdd_param_types bool inv_in, bool inv_out, bool varshift
#define bdd_param_names inv_in, inv_out, varshift

inline size_t fpairing(size_t x, size_t y) {
	const size_t z = x + y;
	return y+((z * (z + 1))>>1);
}

struct bdd_node {
	bdd_node(int_t v, int_t h, int_t l) :
		v(v), h(h), l(l), hash(hash_utri(v, h, l)) {}
//	const int_t v, h, l;
//	const size_t hash; // problems with operator=...
	int_t v, h, l;
	size_t hash;
	bool operator==(const bdd_node& x) const {
		return hash == x.hash && v == x.v && h == x.h && l == x.l;
	}
};

template<> struct std::hash<bdd_node> {
	size_t operator()(const bdd_node& n) const { return n.hash; }
};

template<typename B> B get_zero() { return B::zero(); }
template<typename B> B get_one() { return B::one(); }

template<typename B, bdd_param_defaults>
struct bdd : variant<bdd_node, B> {
	typedef variant<bdd_node, B> base;

	bdd(int_t v, int_t h, int_t l) : base(bdd_node(v, h, l)) {}
	bdd(const bdd_node& n) : base(n) {}
	bdd(const B& b) : base(b) {}

	bool leaf() const { return holds_alternative<B>(*this); }
	static bool leaf(int_t n) { return V[abs(n)].leaf(); }

	struct initializer { initializer(); };

	inline static vector<bdd> V;
	inline static unordered_map<bdd_node, size_t> Mn;
	inline static map<B, size_t> Mb;
	inline static int_t T, F;
	inline static initializer I;

	static int_t add(const bdd_node& n) { return add(n.v, n.h, n.l); }
	
	static int_t add(int_t v, int_t h, int_t l) {
		if (h == l) return h;
#ifdef DEBUG
		auto p = V[abs(l)], q = V[abs(h)];
		if (!p.leaf() && !q.leaf()) {
			const auto x = std::get<bdd_node>(p);
			const auto y = std::get<bdd_node>(q);
			if(!inv_in) assert((x.v > v) && (y.v > v));
		}
#endif
        	if (inv_in && abs(h) < abs(l)) {
			swap(h, l), v = -v;
		}
		if (inv_out && l < 0) {
			h = -h, l = -l;
			bdd_node n(v, h, l);
			if (auto it = Mn.find(n); it != Mn.end())
				return -it->second;
			Mn.emplace(n, V.size());
			V.emplace_back(n);
			return -V.size()+1;
		}
		bdd_node n(v, h, l);
		if (auto it = Mn.find(n); it != Mn.end()) return it->second;
		Mn.emplace(n, V.size());
		V.emplace_back(n);
		return V.size()-1;
	}

	static int_t bit(int_t v) {
		return v > 0 ? add(v, T, F) : add(-v, F, T);
	}

	static int_t add(const B& b) {
		if (b == false) return F;
		if (b == true) return T;
		if (auto it = Mb.find(b); it != Mb.end()) return it->second;
		else if (inv_out && (it = Mb.find(~b)) != Mb.end()) return -it->second;
		Mb.emplace(b, V.size());
		V.emplace_back(b);
		return V.size() - 1;
	}

	static bdd get(int_t n) {
#ifdef DEBUG
		if(!inv_out) assert(n >= 0);
#endif
		if(!inv_out && n == 0) return V[0];
		if (n > 0) {
			if(!inv_in) return V[n];
			if(V[n].leaf()) return V[n];
			const bdd_node &t = std::get<bdd_node>(V[n]);
			return t.v > 0 ? V[n] : bdd(-t.v, t.l, t.h);
		}
		n = -n;
		if (V[n].leaf()) return bdd(~std::get<B>(V[n]));
		const bdd_node& t = std::get<bdd_node>(V[n]);
		return t.v > 0 ? bdd(t.v, -t.h, -t.l) : bdd(-t.v, -t.l, -t.h);
	}

	static B get_elem(int_t x) { return std::get<B>(get(x)); }
	static bdd_node get_node(int_t x) { return std::get<bdd_node>(get(x)); }

	static int_t bdd_not(int_t x) {
		if(inv_out) return -x;
		if (x == T) return F;
		if (x == F) return T;
		const bdd& xx = get(x);
		if(xx.leaf()) {
			return add(~std::get<B>(xx));
		}
		const bdd_node &nx = std::get<bdd_node>(xx);
		return add(nx.v, bdd_not(nx.h), bdd_not(nx.l));
	}

	static int_t bdd_and(int_t x, const B& b) {
		if (x == T) return add(b);
		if (x == F) return F;
		const bdd& xx = get(x);
		if (xx.leaf()) {
//#ifdef DEBUG
//			if constexpr(is_same<B, struct Bool>::value)
//				if (b == true && !((b & std::get<B>(xx)) == xx))
//					assert((b & std::get<B>(xx)) == xx);
//#endif
			return add(b & std::get<B>(xx));
		}
		const bdd_node &nx = std::get<bdd_node>(xx);
		int_t y = add(nx.v, bdd_and(nx.h, b), bdd_and(nx.l, b));
#ifdef DEBUG
		if (b == true) assert(y == x);
#endif
		return y;
	}

	static int_t bdd_and(int_t x, int_t y) {
		if (x == F || y == F) return F;
		if (x == T) return y;
		if (y == T) return x;
		const bdd &xx = get(x), &yy = get(y);
		if (xx.leaf()) return bdd_and(y, std::get<B>(xx));
		if (yy.leaf()) return bdd_and(x, std::get<B>(yy));
		const bdd_node &nx = std::get<bdd_node>(xx);
		const bdd_node &ny = std::get<bdd_node>(yy);
		if (nx.v == ny.v)
			return	add(nx.v,
				bdd_and(nx.h, ny.h),
				bdd_and(nx.l, ny.l));
		if (nx.v < ny.v)
			return add(nx.v, bdd_and(nx.h, y), bdd_and(nx.l, y));
		return add(ny.v, bdd_and(ny.h, x), bdd_and(ny.l, x));
	}

	static int_t bdd_or(int_t x, const B& b) {
		if (x == T) return T;
		if (x == F) return add(b);
		const bdd& xx = get(x);
		if (xx.leaf()) {
			return add(b | std::get<B>(xx));
		}
		const bdd_node &nx = std::get<bdd_node>(xx);
		int_t y = add(nx.v, bdd_or(nx.h, b), bdd_or(nx.l, b));
#ifdef DEBUG
		if (b == true) assert(y == T);
#endif
		return y;
	}

	static int_t bdd_or(int_t x, int_t y){
		if (x == T || y == T) return T;
		if (x == F) return y;
		if (y == F) return x;
		const bdd &xx = get(x), &yy = get(y);
		if (xx.leaf()) return bdd_or(y, std::get<B>(xx));
		if (yy.leaf()) return bdd_or(x, std::get<B>(yy));
		const bdd_node &nx = std::get<bdd_node>(xx);
		const bdd_node &ny = std::get<bdd_node>(yy);
		if (nx.v == ny.v)
			return	add(nx.v,
					  bdd_or(nx.h, ny.h),
					  bdd_or(nx.l, ny.l));
		if (nx.v < ny.v)
			return add(nx.v, bdd_or(nx.h, y), bdd_or(nx.l, y));
		return add(ny.v, bdd_or(ny.h, x), bdd_or(ny.l, x));
	}

	static int_t ex(int_t x, int_t v) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node &nx = std::get<bdd_node>(xx);
		if (nx.v < v) return add(nx.v, ex(nx.h, v), ex(nx.l, v));
		if (nx.v > v) return x;
		return bdd_or(nx.h, nx.l);
	}

	static int_t all(int_t x, int_t v) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node &nx = std::get<bdd_node>(xx);
		if (nx.v < v) return add(nx.v, all(nx.h, v), all(nx.l, v));
		if (nx.v > v) return x;
		return bdd_and(nx.h, nx.l);
	}

	static B get_uelim(int x) {
		const bdd &xx = get(x);
		if (xx.leaf()) return std::get<B>(xx);
		const bdd_node &nx = std::get<bdd_node>(xx);
		if (B r = get_uelim(nx.h); r == false) return r;
		else return r & get_uelim(nx.l);
	}

	static B get_eelim(int x) {
		const bdd &xx = get(x);
		if (xx.leaf()) return std::get<B>(xx);
		const bdd_node &nx = std::get<bdd_node>(xx);
		if (B r = get_eelim(nx.h); r == true) return r;
		else return r | get_eelim(nx.l);
	}

	static int_t sub0(int_t x, int_t v) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node &nx = std::get<bdd_node>(xx);
		if (nx.v < v) return add(nx.v, sub0(nx.h, v), sub0(nx.l, v));
		if (nx.v > v) return x;
		return nx.l;
	
	}

	static int_t sub1(int_t x, int_t v) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node &nx = std::get<bdd_node>(xx);
		if (nx.v < v) return add(nx.v, sub1(nx.h, v), sub1(nx.l, v));
		if (nx.v > v) return x;
		return nx.h;
	}

	static bool dnf(int_t x, vector<int_t>& v,
		function<bool(const pair<B, vector<int_t>>&)> f) {
		const bdd& xx = get(x);
		if (xx.leaf()) {
			if (!(std::get<B>(xx) == false))
				return f({std::get<B>(xx), v});
			return true;
		}
		const bdd_node& n = std::get<bdd_node>(xx);
		v.push_back(n.v);
		if (!dnf(n.h, v, f)) return false;
		v.back() = -n.v;
		if (!dnf(n.l, v, f)) return false;
		v.pop_back();
		return true;
	}

	static void get_vars(int_t x, set<int_t>& s) {
		const bdd& xx = get(x);
		if (xx.leaf()) return;
		const bdd_node& n = std::get<bdd_node>(xx);
		if (s.find(n.v) != s.end()) return;
		s.insert(n.v), get_vars(n.h, s), get_vars(n.l, s);
	}

	static int_t ite(int_t x, int_t y, int_t z) {
		return bdd_or(bdd_and(x, y), bdd_and(bdd_not(x),z));
	}
	static void get_one_zero(int_t, map<int_t, B>&);

	static int_t compose(int_t x, const map<int_t, int_t>& m) {
		if (leaf(x)) return x;
		const bdd_node& n = get_node(x);
		int_t a = compose(n.h, m), b = compose(n.l, m);
		if (auto it = m.find(n.v); it == m.end())
			return ite(add(n.v, T, F), a, b);
		else return ite(it->second, a, b);
	}

	// m must include all vars in x, otherwise use compose()
	static B eval(int_t x, const map<int_t, B>& m) {
		if (leaf(x)) return get_elem(x);
		const bdd_node& n = get_node(x);
		B a = get_elem(eval(n.h, m)), b = get_elem(eval(n.l, m));
		if (auto it = m.find(n.v); it == m.end()) assert(0);
		else return ite(it->second, a, b);
	}

	static int_t from_clause(const pair<B, vector<int_t>>& v) {
		int_t r = bdd_and(T, v.first);
		for (int_t t : v.second) r = bdd_and(r, bit(t));
		return r;
	}

	static int_t from_dnf(const set<pair<B, vector<int_t>>>& s) {
		int_t r = F;
		for (auto& x : s) r = bdd_or(r, from_clause(x));
		return r;
	}

	// treat x as a *disjoint* union of elements of s
	static int_t split(int_t x, const B& e, const set<B>& s) {
		set<pair<B, vector<int_t>>> r;
		B p = get_one<B>();
		for (const B& y : s)
			if ((p = (p & ~y)) == false)
				break;
		dnf(x, [&r, &e, &s, &p](auto x) {
			if ((x.first & e) == false) r.insert(x);
			else {
				r.emplace(x.first & p, x.second);
				for (const B& y : x.second)
					r.emplace(x.first & y, x.second);
			}
		});
		return from_dnf(r);
	}
};

template<typename B, bdd_param_types>
void bdd<B, bdd_param_names>::get_one_zero(int_t x, map<int_t, B>& m) {
	assert(!leaf(x));
	const bdd_node& n = get_node(x);
	if (n.l == F) m.clear(), m.emplace(n.v, B::zero());
	else if (n.h == F) m.clear(), m.emplace(n.v, B::one());
	else if (!leaf(n.l))
		get_one_zero(bdd_and(n.l, n.h), m),
		m.empalce(n.v, get_elem(eval(n.l, m)));
	else if (leaf(n.h)) m.emplace(n.v, get_elem(n.l));
	else	get_one_zero(bdd_and(n.h, get_elem(n.l)), m),
		m.emplace(n.v, compose(bdd_not(n.h), m));
	DBG(assert(compose(x, m) == false);)
}

template<> void bdd<Bool>::get_one_zero(int_t x, map<int_t, Bool>& m) {
	DBG(assert(x != T);)
	m.clear();
	while (!leaf(x)) {
		const bdd_node& n = get_node(x);
		if (n.h == F) { m.emplace(n.v, true); return; }
		m.emplace(n.v, false);
		if ((x = n.l) == F) return;
	}
	throw 0;
}
#endif
