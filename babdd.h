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
typedef uint32_t uint_t;
template<typename T> using sp = shared_ptr<T>;

#define neg_to_odd(x) (((x)<0?(((-(x))<<1)+1):((x)<<1)))
#define hash_pair(x, y) fpairing(neg_to_odd(x), neg_to_odd(y))
#define hash_tri(x, y, z), N fpairing(hash_pair(x, y), neg_to_odd(z))
#define hash_upair(x, y) fpairing(x, y)
#define hash_utri(x, y, z) fpairing(hash_upair(x, y), z)

inline size_t fpairing(size_t x, size_t y) {
	const size_t z = x + y;
	return y+((z * (z + 1))>>1);
}

/* Functionality options for bdd library
 * - use of input inverters
 * - use of output inverters
 * - use of variable shifters
 */
enum bdd_params { INV_IN = (1u << 0), INV_OUT = (1u << 1), VARSHIFT = (1u << 2) };

/* Options for bdd instantiation. The class handles dependencies and restrictions
 * between parameters
 */
template<uint8_t params = INV_IN | INV_OUT | VARSHIFT>
struct bdd_options {
	uint8_t ID_WIDTH = 18;
	uint8_t SHIFT_WIDTH = 12;

	constexpr bdd_options() {
		if constexpr (!(params & VARSHIFT)) {
			SHIFT_WIDTH = 0;
			ID_WIDTH = 30;
		}
	}
	constexpr bdd_options(auto ID_WIDTH_) {
		if constexpr (!(params & VARSHIFT)) {
			DBG(assert(ID_WIDTH_ <= 62));
			SHIFT_WIDTH = 0;
			ID_WIDTH = ID_WIDTH_;
		} else {
			DBG(assert(ID_WIDTH_ < 30));
			SHIFT_WIDTH = 30 - ID_WIDTH_;
			ID_WIDTH = ID_WIDTH_;
		}
	}
	constexpr bdd_options(auto ID_WIDTH_, auto SHIFT_WIDTH_) {
		DBG(assert(ID_WIDTH_ <= 62));
		if constexpr (!(params & VARSHIFT)) {
			SHIFT_WIDTH = 0;
			ID_WIDTH = ID_WIDTH_;
		} else {
			SHIFT_WIDTH = SHIFT_WIDTH_;
			ID_WIDTH = ID_WIDTH_;
		}
	}
	constexpr bool has_varshift() const {
		return params & static_cast<uint8_t>(VARSHIFT);
	}
	constexpr bool has_inv_in() const {
		return params & static_cast<uint8_t>(INV_IN);
	}
	constexpr bool has_inv_out() const {
		return params & static_cast<uint8_t>(INV_OUT);
	}
};

// Defines the reference type to reference a bdd_node in the bdd universe
template<bool SHIFTED, int_t ID_WIDTH, int_t SHIFT_WIDTH>
struct bdd_reference {
	typedef std::conditional<ID_WIDTH + SHIFT_WIDTH <= 30,
					uint32_t, uint64_t>::type ref_type;
	ref_type IN: 1 = 0;
	ref_type OUT: 1 = 0;
	ref_type SHIFT: SHIFT_WIDTH = 0;
	ref_type ID: ID_WIDTH = 0;

	bdd_reference() = default;
	bdd_reference(auto in, auto out, auto id) : IN(in), OUT(out), ID(id) {}
	bdd_reference(auto in, auto out, auto shift, auto id) : IN(in), OUT(out),
							  SHIFT(shift),
							  ID(id) {}

	bool operator==(const bdd_reference x) const {
		return IN == x.IN && OUT == x.OUT && SHIFT == x.SHIFT &&
		       ID == x.ID;
	}

	static bdd_reference flip_in(const bdd_reference x) {
		return x.IN == 1 ? bdd_reference(0, x.OUT, x.SHIFT, x.ID) :
		       bdd_reference(1, x.OUT, x.SHIFT, x.ID);
	}

	static bdd_reference flip_out(const bdd_reference x) {
		return x.OUT == 1 ? bdd_reference(x.IN, 0, x.SHIFT, x.ID) :
		       bdd_reference(x.IN, 1, x.SHIFT, x.ID);
	}

	static bdd_reference to_shift_node(const bdd_reference x, uint_t v) {
		if(x.SHIFT == 0) return x;
		else return bdd_reference(x.IN, x.OUT, x.SHIFT - v, x.ID);
	}

	static bdd_reference to_bdd_node(const bdd_reference x, uint_t v) {
		if(x.SHIFT == 0) return x;
		else return bdd_reference(x.IN, x.OUT, x.SHIFT + v, x.ID);
	}

	static size_t hash(const bdd_reference x) {
		return (x.ID + x.IN) ^ (x.SHIFT + x.OUT);
	}
};

template<int_t ID_WIDTH, int_t SHIFT_WIDTH>
struct bdd_reference<false, ID_WIDTH, SHIFT_WIDTH> {
	typedef std::conditional<ID_WIDTH <= 30,
					uint32_t, uint64_t>::type ref_type;
	ref_type IN: 1 = 0;
	ref_type OUT: 1 = 0;
	ref_type ID: ID_WIDTH = 0;

	bdd_reference() = default;
	bdd_reference(auto in, auto out, auto id) : IN(in), OUT(out), ID(id) {}

	bool operator==(const bdd_reference x) const {
		return IN == x.IN && OUT == x.OUT && ID == x.ID;
	}

	static bdd_reference flip_in(const bdd_reference x) {
		return x.IN == 1 ? bdd_ref(0, x.OUT, x.ID) :
		       bdd_ref(1, x.OUT, x.ID);
	}

	static bdd_reference flip_out(const bdd_reference x) {
		return x.OUT == 1 ? bdd_reference(x.IN, 0, x.ID) :
		       bdd_reference(x.IN, 1, x.ID);
	}

	static size_t hash(const bdd_reference x) {
		return (x.ID + x.IN) ^ x.OUT;
	}
};

template<typename R>
struct bdd_node {
	bdd_node(uint_t v, R h, R l) :
		v(v), h(h), l(l),
		hash(hash_utri(v, R::hash(h), R::hash(l))) {}
	uint_t v;
	R h, l;
	size_t hash;
	bool operator==(const auto& x) const {
		return hash == x.hash && v == x.v && h == x.h && l == x.l;
	}
};

/* This class represents a bdd_node in case variable shifters are used
 * R is the reference type for the high and low pointer
 */
template<typename R>
struct node_skeleton {
	node_skeleton(R h, R l) :
		h(h), l(l), hash(hash_upair(R::hash(h), R::hash(l))) {}
	R h, l;
	size_t hash;
	bool operator==(const auto& x) const {
		return hash == x.hash && h == x.h && l == x.l;
	}
};

template<typename R>
struct std::hash<bdd_node<R>> {
	size_t operator()(auto& n) const { return n.hash; }
};

template<typename R>
struct std::hash<node_skeleton<R>> {
	size_t operator()(auto& n) const { return n.hash; }
};

template<typename B> B get_zero() { return B::zero(); }
template<typename B> B get_one() { return B::one(); }

template<typename B, auto o = bdd_options()>
struct bdd : variant<bdd_node<bdd_reference<o.has_varshift(), o.ID_WIDTH, o.SHIFT_WIDTH>>, B> {
	using bdd_ref = bdd_reference<o.has_varshift(), o.ID_WIDTH, o.SHIFT_WIDTH>;
	typedef bdd_node<bdd_ref> bdd_node_t;
	typedef variant<bdd_node_t, B> base;

	// This class is placed into the bdd universe if variable shifters are used
	struct bdd_skeleton : variant<node_skeleton<bdd_ref>, B> {
		using base = variant<node_skeleton<bdd_ref>, B>;
		bdd_skeleton(bdd_ref h, bdd_ref l) : base(node_skeleton<bdd_ref>(h, l)) {}
		explicit bdd_skeleton(const auto& n) : base(n) {}

		bool leaf() const { return holds_alternative<B>(*this); }
	};

	bdd(uint_t v, bdd_ref h, bdd_ref l) : base(bdd_node_t(v, h, l)) {}
	explicit bdd(const auto& n) : base(n) {}
	//bdd(const B& b) : base(b) {}

	bool leaf() const { return holds_alternative<B>(*this); }
	static bool leaf(bdd_ref n) { return holds_alternative<B>(V[n.ID]); }

	struct initializer { initializer(); };

	inline static std::conditional<o.has_varshift(),
				vector<bdd_skeleton>, vector<bdd>>::type V;
	inline static std::conditional<o.has_varshift(),
			unordered_map<node_skeleton<bdd_ref>, size_t>,
			        unordered_map<bdd_node_t, size_t>>::type Mn;
	inline static map<B, size_t> Mb;
	inline static bdd_ref T, F;
	inline static initializer I;

	static bdd_ref add(const bdd_node_t& n) { return add(n.v, n.h, n.l); }

	static bdd_ref add(uint_t v, bdd_ref h, bdd_ref l) {
		if (h == l) return h;
#ifdef DEBUG
		auto p = get(l), q = get(h);
		if (!p.leaf() && !q.leaf()) {
			const auto x = std::get<bdd_node_t>(p);
			const auto y = std::get<bdd_node_t>(q);
			if constexpr (!o.has_inv_in()) assert((x.v > v) && (y.v > v));
		}
#endif
		bool in = false, out = false;
        	if constexpr (o.has_inv_in())
			if(h.ID < l.ID)
				swap(h, l), in = true;
		if constexpr (o.has_inv_out())
			if (l.OUT) {
				h = bdd_ref::flip_out(h);
				l = bdd_ref::flip_out(l);
				out = true;
			}
		if constexpr (o.has_varshift()){
			h = bdd_ref::to_shift_node(h, v);
			l = bdd_ref::to_shift_node(l, v);
			node_skeleton<bdd_ref> n(h, l);
			if (auto it = Mn.find(n); it != Mn.end())
				return bdd_ref(in, out, v, it->second);
			Mn.emplace(n, V.size());
			V.emplace_back(n);
			return bdd_ref(in, out, v, V.size()-1);
		} else {
			bdd_node_t n(v, h, l);
			if (auto it = Mn.find(n); it != Mn.end())
				return bdd_ref(in, out, it->second);
			Mn.emplace(n, V.size());
			V.emplace_back(n);
			return bdd_ref(in, out, V.size() - 1);
		}
	}

	static bdd_ref bit(int_t v) {
		return v > 0 ? add(v, T, F) : add(-v, F, T);
	}

	static bdd_ref add(const B& b) {
		if (b == false) return F;
		if (b == true) return T;
		if (auto it = Mb.find(b); it != Mb.end()) return bdd_ref(0,0,it->second);
		else if constexpr (o.has_inv_out())
			if ((it = Mb.find(~b)) != Mb.end()) return bdd_ref(0,1,it->second);
		Mb.emplace(b, V.size());
		V.emplace_back(b);
		return bdd_ref(0, 0, V.size()-1);
	}

	static bdd get(bdd_ref n) {
		constexpr auto get_bdd_node = [](const bdd_ref n) {
			if constexpr (o.has_varshift()) {
				const bdd_skeleton& s = V[n.ID];
				if(s.leaf()) return bdd(std::get<B>(s));
				const auto& x = std::get<node_skeleton<bdd_ref>>(s);
				return bdd(n.SHIFT, bdd_ref::to_bdd_node(x.h, n.SHIFT),
					   	bdd_ref::to_bdd_node(x.l, n.SHIFT));
			} else {
				return V[n.ID];
			}
		};
#ifdef DEBUG
		if constexpr (!o.has_inv_out()) assert(!n.OUT);
		if constexpr (!o.has_inv_in()) assert(!n.IN);
#endif
		const auto& b = get_bdd_node(n);
		if constexpr (!o.has_inv_out())
			if (n.ID == 0) return b;
		if (!n.OUT) {
			if constexpr (!o.has_inv_in()) return b;
			if(leaf(n)) return b;
			const auto &t = std::get<bdd_node_t>(b);
			return !n.IN ? b : bdd(t.v, t.l, t.h);
		}
		if (leaf(n)) return bdd(~std::get<B>(b));
		const auto& t = std::get<bdd_node_t>(b);
		return !n.IN ? bdd(t.v, bdd_ref::flip_out(t.h), bdd_ref::flip_out(t.l)) :
				bdd(t.v, bdd_ref::flip_out(t.l), bdd_ref::flip_out(t.h));
	}

	static B get_elem(bdd_ref x) { return std::get<B>(get(x)); }
	static bdd_node_t get_node(bdd_ref x) { return std::get<bdd_node_t>(get(x)); }

	static bdd_ref bdd_not(bdd_ref x) {
		if constexpr (o.has_inv_out()) return bdd_ref::flip_out(x);
		if (x == T) return F;
		if (x == F) return T;
		const bdd& xx = get(x);
		if(xx.leaf()) {
			return add(~std::get<B>(xx));
		}
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		return add(nx.v, bdd_not(nx.h), bdd_not(nx.l));
	}

	static bdd_ref bdd_and(bdd_ref x, const B& b) {
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
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		bdd_ref y = add(nx.v, bdd_and(nx.h, b), bdd_and(nx.l, b));
#ifdef DEBUG
		if (b == true) assert(y == x);
#endif
		return y;
	}

	static bdd_ref bdd_and(bdd_ref x, bdd_ref y) {
		if (x == F || y == F) return F;
		if (x == T) return y;
		if (y == T) return x;
		const bdd &xx = get(x), &yy = get(y);
		if (xx.leaf()) return bdd_and(y, std::get<B>(xx));
		if (yy.leaf()) return bdd_and(x, std::get<B>(yy));
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		const bdd_node_t &ny = std::get<bdd_node_t>(yy);
		if (nx.v == ny.v)
			return	add(nx.v,
				bdd_and(nx.h, ny.h),
				bdd_and(nx.l, ny.l));
		if (nx.v < ny.v)
			return add(nx.v, bdd_and(nx.h, y), bdd_and(nx.l, y));
		return add(ny.v, bdd_and(ny.h, x), bdd_and(ny.l, x));
	}

	static bdd_ref bdd_or(bdd_ref x, const B& b) {
		if (x == T) return T;
		if (x == F) return add(b);
		const bdd& xx = get(x);
		if (xx.leaf()) {
			return add(b | std::get<B>(xx));
		}
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		bdd_ref y = add(nx.v, bdd_or(nx.h, b), bdd_or(nx.l, b));
#ifdef DEBUG
		if (b == true) assert(y == T);
#endif
		return y;
	}

	static bdd_ref bdd_or(bdd_ref x, bdd_ref y){
		if (x == T || y == T) return T;
		if (x == F) return y;
		if (y == F) return x;
		const bdd &xx = get(x), &yy = get(y);
		if (xx.leaf()) return bdd_or(y, std::get<B>(xx));
		if (yy.leaf()) return bdd_or(x, std::get<B>(yy));
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		const bdd_node_t &ny = std::get<bdd_node_t>(yy);
		if (nx.v == ny.v)
			return	add(nx.v,
					  bdd_or(nx.h, ny.h),
					  bdd_or(nx.l, ny.l));
		if (nx.v < ny.v)
			return add(nx.v, bdd_or(nx.h, y), bdd_or(nx.l, y));
		return add(ny.v, bdd_or(ny.h, x), bdd_or(ny.l, x));
	}

	static bdd_ref ex(bdd_ref x, uint_t v) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		if (nx.v < v) return add(nx.v, ex(nx.h, v), ex(nx.l, v));
		if (nx.v > v) return x;
		return bdd_or(nx.h, nx.l);
	}

	static bdd_ref all(bdd_ref x, uint_t v) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		if (nx.v < v) return add(nx.v, all(nx.h, v), all(nx.l, v));
		if (nx.v > v) return x;
		return bdd_and(nx.h, nx.l);
	}

	static B get_uelim(bdd_ref x) {
		const bdd &xx = get(x);
		if (xx.leaf()) return std::get<B>(xx);
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		if (B r = get_uelim(nx.h); r == false) return r;
		else return r & get_uelim(nx.l);
	}

	static B get_eelim(bdd_ref x) {
		const bdd &xx = get(x);
		if (xx.leaf()) return std::get<B>(xx);
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		if (B r = get_eelim(nx.h); r == true) return r;
		else return r | get_eelim(nx.l);
	}

	static bdd_ref sub0(bdd_ref x, uint_t v) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		if (nx.v < v) return add(nx.v, sub0(nx.h, v), sub0(nx.l, v));
		if (nx.v > v) return x;
		return nx.l;
	
	}

	static bdd_ref sub1(bdd_ref x, uint_t v) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		if (nx.v < v) return add(nx.v, sub1(nx.h, v), sub1(nx.l, v));
		if (nx.v > v) return x;
		return nx.h;
	}

	static bool dnf(bdd_ref x, vector<int_t>& v,
		function<bool(const pair<B, vector<int_t>>&)> f) {
		const bdd& xx = get(x);
		if (xx.leaf()) {
			if (!(std::get<B>(xx) == false))
				return f({std::get<B>(xx), v});
			return true;
		}
		const bdd_node_t& n = std::get<bdd_node_t>(xx);
		v.push_back(n.v);
		if (!dnf(n.h, v, f)) return false;
		v.back() = -n.v;
		if (!dnf(n.l, v, f)) return false;
		v.pop_back();
		return true;
	}

	static void get_vars(bdd_ref x, set<int_t>& s) {
		const bdd& xx = get(x);
		if (xx.leaf()) return;
		const bdd_node_t& n = std::get<bdd_node_t>(xx);
		if (s.find(n.v) != s.end()) return;
		s.insert(n.v), get_vars(n.h, s), get_vars(n.l, s);
	}

	static bdd_ref ite(bdd_ref x, bdd_ref y, bdd_ref z) {
		return bdd_or(bdd_and(x, y), bdd_and(bdd_not(x),z));
	}

	static void get_one_zero(bdd_ref, map<int_t, B>&);

	static bdd_ref compose(bdd_ref x, const map<int_t, bdd_ref>& m) {
		if (leaf(x)) return x;
		const bdd_node_t& n = get_node(x);
		bdd_ref a = compose(n.h, m), b = compose(n.l, m);
		if (auto it = m.find(n.v); it == m.end())
			return ite(add(n.v, T, F), a, b);
		else return ite(it->second, a, b);
	}

	// m must include all vars in x, otherwise use compose()
	static B eval(bdd_ref x, const map<int_t, B>& m) {
		if (leaf(x)) return get_elem(x);
		const bdd_node_t& n = get_node(x);
		B a = get_elem(eval(n.h, m)), b = get_elem(eval(n.l, m));
		if (auto it = m.find(n.v); it == m.end()) assert(0);
		else return ite(it->second, a, b);
	}

	static bdd_ref from_clause(const pair<B, vector<int_t>>& v) {
		bdd_ref r = bdd_and(T, v.first);
		for (int_t t : v.second) r = bdd_and(r, bit(t));
		return r;
	}

	static bdd_ref from_dnf(const set<pair<B, vector<int_t>>>& s) {
		bdd_ref r = F;
		for (auto& x : s) r = bdd_or(r, from_clause(x));
		return r;
	}

	// treat x as a *disjoint* union of elements of s
	static bdd_ref split(bdd_ref x, const B& e, const set<B>& s) {
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

template<typename B, auto o>
void bdd<B, o>::get_one_zero(bdd_ref x, map<int_t, B>& m) {
	assert(!leaf(x));
	const bdd_node_t& n = get_node(x);
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

template<> void bdd<Bool>::get_one_zero(bdd_ref x, map<int_t, Bool>& m) {
	DBG(assert(x != T);)
	m.clear();
	while (!leaf(x)) {
		const bdd_node_t& n = get_node(x);
		if (n.h == F) { m.emplace(n.v, true); return; }
		m.emplace(n.v, false);
		if ((x = n.l) == F) return;
	}
	throw 0;
}
#endif
