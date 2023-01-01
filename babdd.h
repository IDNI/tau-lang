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
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <variant>
#include <cstdint>
#include <cassert>
#include <memory>
using namespace std;

typedef int32_t int_t;
template<typename T> using sp = shared_ptr<T>;

#define neg_to_odd(x) (((x)<0?(((-(x))<<1)+1):((x)<<1)))
#define hash_pair(x, y) fpairing(neg_to_odd(x), neg_to_odd(y))
#define hash_tri(x, y, z) fpairing(hash_pair(x, y), neg_to_odd(z))
#define hash_upair(x, y) fpairing(x, y)
#define hash_utri(x, y, z) fpairing(hash_upair(x, y), z)

inline size_t fpairing(size_t x, size_t y) {
	const size_t z = x + y;
	return y+((z * (z + 1))>>1);
}

struct bdd_node {
	bdd_node(int_t v, int_t h, int_t l) :
		v(v), h(h), l(l), hash(hash_utri(v, h, l)) {}
	const int_t v, h, l;
	const size_t hash;
	bool operator==(const bdd_node& x) const {
		return hash == x.hash && v == x.v && h == x.h && l == x.l;
	}
};

template<> struct std::hash<bdd_node> {
	size_t operator()(const bdd_node& n) const { return n.hash; }
};

template<typename B> struct bdd : variant<bdd_node, B> {
	typedef variant<bdd_node, B> base;

	bdd(int_t v, int_t h, int_t l) : base(bdd_node(v, h, l)) {}
	bdd(const bdd_node& n) : base(n) {}
	bdd(const B& b) : base(b) {}

	bool leaf() const { return holds_alternative<B>(*this); }

	struct initializer {
		initializer() {
			cout << "init!!" << endl;
			bdd::init();
		}
	};

	static vector<bdd> V;
	static unordered_map<bdd_node, size_t> Mn;
	static map<B, size_t> Mb;
	static int_t T, F;
	inline static initializer I;

	static int_t add(const bdd_node& n) { return add(n.v, n.h, n.l); }

	static int_t add(int_t v, int_t h, int_t l) {
		if (h == l) return h;
		if (l < 0) {
			h = -h, l = -l;
			bdd_node n(v, h, l);
			if (auto it = Mn.find(n); it != Mn.end())
				return -it->second;
			Mn.emplace(n, V.size());
			V.emplace_back(n);
			return -V.size()+1;
		}
		bdd_node n(v, h, l);
		if (auto it = Mn.find(n); it != Mn.end())
			return it->second;
		Mn.emplace(n, V.size());
		V.emplace_back(n);
		return V.size()-1;
	}

	static int_t add(const B& b) {
		if (b == false) return F;
		if (b == true) return T;
		if (auto it = Mb.find(b); it != Mb.end()) return it->second;
		else if ((it = Mb.find(~b)) != Mb.end()) return -it->second;
		Mb.emplace(b, V.size());
		V.emplace_back(b);
		return V.size() - 1;
	}

	static bdd get(int_t n) {
		if (n > 0) return V[n];
		n = -n;
		if (V[n].leaf()) return bdd(~std::get<B>(V[n]));
		const bdd_node& t = std::get<bdd_node>(V[n]);
		return bdd(t.v, -t.h, -t.l);
	}

	static void init();

	static int_t bdd_and(int_t x, const B& b) {
		const bdd& xx = get(x);
		if (xx.leaf()) return add(b & std::get<B>(xx));
		const bdd_node &nx = std::get<bdd_node>(xx);
		return add(nx.v, bdd_and(nx.h, b), bdd_and(nx.l, b));
	}

	static int_t bdd_and(int_t x, int_t y) {
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

	static int_t subst(int_t x, int_t v, int_t y) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node &nx = std::get<bdd_node>(xx);
		if (nx.v < v)
			return add(nx.v, subst(nx.h, v, y), subst(nx.l, v, y));
		if (nx.v > v) return x;
		return -bdd_and(-bdd_and(y, nx.h), -bdd_and(-y, nx.l));
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

	static void dnf(
		int_t x, set<pair<B, vector<int_t>>>& s, vector<int_t>& v) {
		const bdd& xx = get(x);
		if (xx.leaf()) {
			if (!(std::get<B>(xx) == false))
				s.emplace(std::get<B>(xx), v);
			return;
		}
		const bdd_node& n = std::get<bdd_node>(xx);
		v.push_back(n.v);
		dnf(n.h, s, v);
		v.back() = -n.v;
		dnf(n.l, s, v);
		v.pop_back();
	}
};

template<typename B> struct bdd_handle;
template<typename B> using hbdd = sp<bdd_handle<B>>;

template<typename B> struct bdd_handle {
	static unordered_map<bdd_node, std::weak_ptr<bdd_handle>> Mn;
	static map<B, std::weak_ptr<bdd_handle>> Mb;
	static hbdd<B> htrue, hfalse;

	static hbdd<B> get(const bdd_node& x) {
		if (auto it = Mn.find(x); it != Mn.end())
			return it->second.lock();
		hbdd<B> h(new bdd_handle);
		h->b = bdd<B>::add(x);
		Mn.emplace(x, weak_ptr<bdd_handle<B>>(h));
		return h;
	}

	static hbdd<B> get(const B& x) {
		if (auto it = Mb.find(x); it != Mb.end())
			return it->second.lock();
		hbdd<B> h(new bdd_handle);
		h->b = bdd<B>::add(x);
		Mb.emplace(x, weak_ptr<bdd_handle<B>>(h));
		return h;
	}

	static hbdd<B> get(const bdd<B>& x) {
		if (x.leaf()) return get(std::get<B>(x));
		return get(std::get<bdd_node>(x));
	}

	bdd<B> get() const { return bdd<B>::get(b); }

	bool zero() const { return b == bdd<B>::F; }
	bool one() const { return b == bdd<B>::T; }

	static hbdd<B> bit(bool b, int_t v) {
		if (bdd<B>::V.empty()) bdd<B>::init();
		assert(v);
		hbdd<B> r = get(bdd<B>::add(v, bdd<B>::T, bdd<B>::F));
		return b ? r : ~r;
	}

	B get_uelim() const { return bdd<B>::get_uelim(b); }
	B get_eelim() const { return bdd<B>::get_eelim(b); }

	hbdd<B> operator&(const hbdd<B>& x) const {
		const bdd<B> &xx = std::get<B>(x->get());
		const bdd<B> &yy = std::get<B>(get());
		if (xx.leaf()) {
			if (yy.leaf())
				return	bdd_handle<B>::get(
					std::get<B>(xx) & std::get<B>(yy));
			return	get(bdd<B>::get(bdd<B>::bdd_and(
				b, std::get<B>(xx))));
		} else if (yy.leaf())
			return	get(bdd<B>::get(bdd<B>::bdd_and(
				x->b, std::get<B>(yy))));
		else return get(bdd<B>::get(bdd<B>::bdd_and(x->b, b)));
	}

	hbdd<B> operator~() const {
		return get(bdd<B>::get(bdd<B>::bdd_and(bdd<B>::T, -b)));
	}

	hbdd<B> operator|(const hbdd<B>& x) const {
		return ~((~x) & (~*this));
	}

	hbdd<B> subst(size_t v, const hbdd<B>& x) const {
		return get(bdd<B>::subst(b, v, x->b));
	}

	hbdd<B> sub0(size_t v) const { return get(bdd<B>::sub0(b, v)); }
	hbdd<B> sub1(size_t v) const { return get(bdd<B>::sub1(b, v)); }
	
	set<pair<B, vector<int_t>>> dnf() const {
		set<pair<B, vector<int_t>>> r;
		vector<int_t> v;
		return bdd<B>::dnf(b, r, v), r;
	}
private:
	int_t b;
};

template<typename B>
bool operator==(const hbdd<B>& x, bool b) { return b ? x->one() : x->zero(); }
template<typename B>
hbdd<B> operator&(const hbdd<B>& x, const hbdd<B>& y) { return (*x) & y; }
template<typename B>
hbdd<B> operator|(const hbdd<B>& x, const hbdd<B>& y) { return (*x) | y; }
template<typename B> hbdd<B> operator~(const hbdd<B>& x) { return ~*x; }

template<typename B> void bdd<B>::init() {
	bdd<B>::V.emplace_back(B::zero());
	bdd<B>::V.emplace_back(B::one());
	bdd<B>::Mb.emplace(B::one(), 1);
	bdd<B>::F = -(bdd<B>::T = 1);
	bdd_handle<B>::hfalse = bdd_handle<B>::get(-1);
	bdd_handle<B>::htrue = bdd_handle<B>::get(1);
}
#endif
