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
		if (x == T) return add(b);
		if (x == F) return F;
		const bdd& xx = get(x);
		if (xx.leaf()) {
#ifdef DEBUG
			if constexpr(is_same<B, struct Bool>::value)
				if (b == true && !((b & std::get<B>(xx)) == xx))
					assert((b & std::get<B>(xx)) == xx);
#endif
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

	static int_t ex(int_t x, int_t v) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node &nx = std::get<bdd_node>(xx);
		if (nx.v < v) return add(nx.v, ex(nx.h, v), ex(nx.l, v));
		if (nx.v > v) return x;
		return -bdd_and(-nx.h, -nx.l);
	}

	static int_t all(int_t x, int_t v) { return -ex(-x, v); }

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

//	static int_t subst(int_t x, int_t v, int_t y) {
//		const bdd &xx = get(x);
//		if (xx.leaf()) return x;
//		const bdd_node &nx = std::get<bdd_node>(xx);
//		if (nx.v < v)
//			return add(nx.v, subst(nx.h, v, y), subst(nx.l, v, y));
//		if (nx.v > v) return x;
//		return -bdd_and(-bdd_and(y, nx.h), -bdd_and(-y, nx.l));
//	}

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
#endif
