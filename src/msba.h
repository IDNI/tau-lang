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

#ifndef __MSBA_H__ // many sorted boolean algebra
#define __MSBA_H__
#include "bdd_handle.h"
#include "bool.h"
#include <variant>

typedef hbdd<Bool> sbf;
#define sbf_T bdd_handle<Bool>::htrue
#define sbf_F bdd_handle<Bool>::hfalse

sbf leq_bdd(int_t x, int_t y) {
	return bdd_handle<Bool>::bit(false, x) | bdd_handle<Bool>::bit(true, y);
}

#ifdef DEBUG
template<typename T, typename S> T operator&(T, S) {
	int s;
	cout << "wrong call to operator& with types " <<
		abi::__cxa_demangle(typeid(T).name(), 0, 0, &s) <<
		" and " <<
		abi::__cxa_demangle(typeid(S).name(), 0, 0, &s) << endl;
	throw 0;
}
#endif

template<typename... BAs>
variant<BAs...> operator&(const variant<BAs...>& x, const variant<BAs...>& y) {
	DBG(assert(x.index() == y.index());)
	auto f = [](const auto& x, const auto& y) -> variant<BAs...> {
#ifdef DEBUG
		if constexpr(!is_same_v<decltype(x), decltype(y)>) assert(0);
#endif
		return x & y;
	};
	return visit(f, x, y);
}

template<typename... BAs> variant<BAs...> operator~(const variant<BAs...>& x) {
	auto f = [](const auto& t) -> variant<BAs...> { return ~t; };
	return visit(f, x);
}

// dirty hack to allow two param packs
template<typename BDDs, typename... aux> struct msba {
	// this &&false is also a dirty hack
	static_assert(sizeof(BDDs) && false);
};

template<typename BDDs, typename... aux> struct normalizer {
	static_assert(sizeof(BDDs) && false);
};

template<typename T, typename... Ts> struct get_first_type { typedef T type; };

template<typename... BDDs, typename... aux>
struct msba<tuple<BDDs...>, aux...> {
	typedef msba<tuple<BDDs...>, aux...> msba_t;
	typedef variant<BDDs..., aux...> elem;
	typedef typename get_first_type<BDDs...>::type first_type;
	sbf b;
	inline static map<elem, int_t> M;
	inline static vector<elem> V;
	inline static set<array<int_t, 2>> leq;

	msba(const msba& t) : b(t.b) {}
	msba(bool p) : b(p ? sbf_T : sbf_F) {}
	msba(bool p, const auto& x) {
		// weirdly, but because this is actually "something = F"
		if (x == true) b = (p ? sbf_F : sbf_T);
		else if (x == false) b = (p ? sbf_T : sbf_F);
		else b = bdd_handle<Bool>::bit(p, get(elem(x)));
	}

	static bool dummy; // nonworking hack to call init
	static void init() {
		if (V.empty()) V.emplace_back(get_one<first_type>());
	}

	bool operator==(bool t) const { return b == t; }

//	static int_t get(const auto& x) { // the bdd var associated with x
//		return get(elem(x));
//	}
	static int_t get(const elem& e) { // the bdd var associated with e
		if (auto it = M.find(e); it != M.end()) return it->second;
		return M.emplace(e, V.size()), V.push_back(e), V.size() - 1;
	}
	msba operator&(const msba& x) const { return b & x.b; }
	msba operator|(const msba& x) const { return b | x.b; }
	msba operator~() const { return ~b; }
	bool operator==(const msba& x) const { return b == x.b; }
	msba& operator=(const msba& x) { b = x.b; return *this; }
	bool operator<(const msba& x) const { return b < x.b; }

	typedef normalizer<tuple<BDDs...>, aux...> norm_t;
	template<typename T> using set_t = set<remove_cvref_t<T>>;

	// naturally expected to be called as dnf(this->b->dnf())
	static set<tuple<set<BDDs>..., set<aux>...>>
	dnf(const set<pair<Bool, vector<int_t>>>& s) {
		set<tuple<set<BDDs>..., set<aux>...>> r;
		for (const pair<Bool, vector<int_t>>& c : s) {
			tuple<set<BDDs>..., set<aux>...> t;
			for (int_t i : c.second)
				visit([i, &t](auto& x) {
					std::get<set_t<decltype(x)>>(t)
						.insert(i > 0 ? x : ~x);
				}, V[abs(i)]);
			r.insert(t);
		}
		return r;
	}

	void normalize(const set<pair<Bool, vector<int_t>>>& s) {
		msba r(false);
		vector<int_t> pos, neg;
		for (auto& p : s) {
			pos.clear(), neg.clear();
			for (int_t i : p.second)
				(i > 0 ? pos : neg).push_back(abs(i));
			r = (r | norm_t::normalize(norm_t::to_tuple(pos, neg)));
		}
		r.apply_leq(), *this = r;
	}
	void normalize() { normalize(b->dnf()); }

//	void normalize() {
//		msba r(false);
//		auto f = [&r](const auto& p) {
//			vector<int_t> pos, neg;
//			for (int_t i : p.second)
//				(i > 0 ? pos : neg).push_back(abs(i));
//			r = (r | norm_t::normalize(norm_t::to_tuple(pos, neg)));
//			return true;
//		};
//		b->dnf(f), r.apply_leq(), *this = r;
//	}

	void apply_leq() {
		set<int_t> v = b->get_vars();
		set<array<int_t, 2>> s;
		int_t n, k;
		auto f = [&s, &n, &k, this](const auto& a, const auto& b) {
#ifdef DEBUG
			if constexpr(!is_same_v<decltype(a), decltype(a)>)
				assert(0);
#endif
			if ((a & b) == a) s.insert({n, k});
		};
		for (int_t x : v)
			for (int_t y : v)
				if (V[x].index() != V[y].index()) continue;
				else if (x < y && leq.find({x, y}) == leq.end())
					visit(f, V[n = x], V[k = y]);
		leq.insert(s.begin(), s.end());
		for (auto x : s) b = (b & leq_bdd(x[0], x[1]));
	}
private:
	msba(sbf b) : b(b) {}
	friend norm_t;
};
#endif
