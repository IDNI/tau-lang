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
#ifndef __BDD_HANDLE_H__
#define __BDD_HANDLE_H__
#include "babdd.h"

template<typename B, auto params> struct bdd_handle;
template<typename B, auto params = INV_IN | INV_OUT | VARSHIFT>
using hbdd = sp<bdd_handle<B, params>>;

template<typename B, auto params = INV_IN | INV_OUT | VARSHIFT>
bool operator==(const hbdd<B, params>& x, bool b) {
	return b ? x->is_one() : x->is_zero();
}

template<typename B, auto params = INV_IN | INV_OUT | VARSHIFT>
hbdd<B, params> operator&(const hbdd<B, params> &x, const hbdd<B, params> &y) {
	return (*x) & y;
}

template<typename B, auto params = INV_IN | INV_OUT | VARSHIFT>
hbdd<B, params> operator|(const hbdd<B, params> &x, const hbdd<B, params> &y) {
	return (*x) | y;
}

template<typename B, auto params = INV_IN | INV_OUT | VARSHIFT>
hbdd<B, params> operator+(const hbdd<B, params> &x, const hbdd<B, params> &y) {
	return (y & ~x) | (x & ~y);
}

template<typename B, auto params = INV_IN | INV_OUT | VARSHIFT>
hbdd<B, params> operator~(const hbdd<B, params>& x) { return ~(*x); }

#ifdef DEBUG
template<typename B, auto params = INV_IN | INV_OUT | VARSHIFT>
bool operator==(const hbdd<B, params>& x, const hbdd<B, params>& y) {
	assert((&*x == &*y) == (x->b == y->b));
	return x->b == y->b;
}
#endif

template<typename B, auto params = INV_IN | INV_OUT | VARSHIFT>
struct bdd_handle {
	typedef unordered_map<bdd_node, shared_ptr<bdd_handle>> mn_type;
	typedef map<B, std::shared_ptr<bdd_handle>> mb_type;
	inline static unordered_map<bdd_node, shared_ptr<bdd_handle>> Mn;
	inline static map<B, std::shared_ptr<bdd_handle>> Mb;
	inline static hbdd<B, params> htrue, hfalse;

	// nonworking hack to call init
	template<typename T, T> struct dummy_type {};
	typedef dummy_type<mn_type&, Mn> dummy_mn_type;
	typedef dummy_type<mb_type&, Mb> dummy_mb_type;
	static bool dummy;

//	bdd_handle();

	static hbdd<B, params> get(const bdd_node& x) {
		if (auto it = Mn.find(x); it != Mn.end())
			return it->second;//.lock();
		hbdd<B, params> h = make_shared<bdd_handle<B, params>>(); //(new bdd_handle);
		return h->b = bdd<B, params>::add(x), Mn.emplace(x, h), h;
	}

	static hbdd<B, params> get(const B& x) {
		if (auto it = Mb.find(x); it != Mb.end())
			return it->second;//.lock();
		hbdd<B, params> h = make_shared<bdd_handle<B, params>>();//(new bdd_handle);
		return h->b = bdd<B, params>::add(x), Mb.emplace(x, h), h;
	}

	static hbdd<B, params>
	        get(const bdd<B, params>& x) {
		return	x.leaf() ? get(std::get<B>(x))
			: get(std::get<bdd_node>(x));
	}

	static hbdd<B, params> get(int_t t) {
		return get(bdd<B, params>::get(t));
	}

	bdd<B, params> get() const {
		return bdd<B, params>::get(b);
	}

	bool is_zero() const { return b == bdd<B, params>::F; }
	bool is_one() const { return b == bdd<B, params>::T; }

	static hbdd<B, params> one() {
		return get(bdd<B, params>::T);
	}

	static hbdd<B, params> zero() {
		return get(bdd<B, params>::F);
	}

	static hbdd<B, params> bit(bool b, int_t v) {
		DBG(assert(v > 0);)
		hbdd<B, params> r = get(bdd<B, params>::bit(b ? v : -v));
		//hbdd<B, params> r = get(bdd_node(v, bdd<B, params>::T, bdd<B, params>::F));
		DBG(assert(r);)
		return r;
	}

	B get_uelim() const { return bdd<B, params>::get_uelim(b); }
	B get_eelim() const { return bdd<B, params>::get_eelim(b); }

	hbdd<B, params> operator&(
		const hbdd<B, params>& x) const {
		const bdd<B, params> &xx = x->get();
		const bdd<B, params> &yy = get();
		if (xx.leaf()) {
#ifndef DEBUG
			if (std::get<B>(xx) == true) return get(*this);
			if (std::get<B>(xx) == false) return hfalse;
#endif
			if (yy.leaf())
				return	bdd_handle<B, params>::get(
					std::get<B>(xx) & std::get<B>(yy));
			return get(bdd<B, params>::bdd_and(b, std::get<B>(xx)));
		} else if (yy.leaf()) {
#ifndef DEBUG
			if (std::get<B, params>(yy) == true) return x;
			if (std::get<B, params>(yy) == false) return hfalse;
#endif
			return get(bdd<B, params>::bdd_and(x->b, std::get<B>(yy)));
		}
		return get(bdd<B, params>::bdd_and(x->b, b));
	}

	hbdd<B, params> operator~() const {
		return get( bdd<B, params>::bdd_and(
			bdd<B, params>::T,
			bdd<B, params>::bdd_not(b)));
	}

	hbdd<B, params>
	operator|(const hbdd<B, params>& x) const {
		return ~((~x) & (~*this));
	}

	hbdd<B, params> ex(int_t v) const {
		return get(bdd<B, params>::ex(b, v));
	}

	hbdd<B, params> all(int_t v) const {
		return get(bdd<B, params>::all(b, v));
	}

	hbdd<B, params>
	subst(size_t v, const hbdd<B, params>& x) const {
#ifdef DEBUG
//		assert( get(bdd<B, params>::subst(b, v, x->b)) ==
//			((sub0(v) & ~x) | (sub1(v) & x)));
#endif
		return (sub0(v) & ~x) | (sub1(v) & x);
//		return get(bdd<B, params>::subst(b, v, x->b));
	}

	hbdd<B, params> sub0(size_t v) const {
		return get(bdd<B, params>::sub0(b, v));
	}

	hbdd<B, params> sub1(size_t v) const {
		return get(bdd<B, params>::sub1(b, v));
	}

	hbdd<B, params>
	condition(size_t v, const hbdd<B, params>& f) const {
		return subst(v, f->sub0(v)) | subst(v, ~(f->sub1(v)));
	}

	void dnf(function<bool(const pair<B, vector<int_t>>&)> f) const {
		vector<int_t> v;
		bdd<B, params>::dnf(b, v, [f](const pair<B, vector<int_t>>& v) {
			return f(v);
		});
	}
	
	set<pair<B, vector<int_t>>> dnf() const {
		set<pair<B, vector<int_t>>> r;
		dnf([&r](auto& x) { r.insert(x); return true; });
		return r;
	}

	set<int_t> get_vars() const {
		set<int_t> r;
		return bdd<B, params>::get_vars(b, r), r;
	}

	map<int_t, B> get_one_zero() const {
		map<int_t, B> m;
		bdd<B, params>::get_one_zero(b, m);
//#ifdef DEBUG
//		auto d = dnf();
//		bool t = false;
//		for (auto x : d) t |= (r == x.second);
//		assert(!t);
//#endif
		return m;
	}

	hbdd<B, params>
	compose(const map<int_t, hbdd<B, params>>& m) const {
		map<int_t, int_t> p;
		for (auto& x : m) p.emplace(x.first, x.second->b);
		return get(bdd<B, params>::compose(b, p));
	}

	B eval(map<int_t, B>& m) const { return bdd<B, params>::eval(b, m); }

	map<int_t, hbdd<B, params>> lgrs() const {
		map<int_t, hbdd<B, params>> r;
		if (b == bdd<B, params>::F) return r;
		DBG(assert((b != bdd<B, params>::T));)
		for (const auto& z : get_one_zero())
			r.emplace(z.first,	((*this) & get(z.second)) |
						(bit(true, z.first) & ~*this));
		return r;
	}
#ifndef DEBUG
private:
#endif
	int_t b;
};

template<typename T> constexpr bool is_sp{};
template<typename T> constexpr bool is_sp<sp<T>>{true};

template<typename B> B get_one() requires is_sp<B> {
	return B::element_type::one();
}

/*template<typename B> void bdd_init() requires is_sp<B, params> {
	if (!bdd<B, params>::V.empty()) return;
#ifdef DEBUG
	int s;
	cout << "bdd_init" << '<' <<
		abi::__cxa_demangle(typeid(bdd<B, params>).name(), 0, 0, &s) <<
		'>' << endl;
#endif
	auto one = get_one<B, params>();
	bdd<B, params>::V.emplace_back(one);
	bdd<B, params>::V.emplace_back(one);
	bdd<B, params>::Mb.emplace(one, 1);
	bdd<B, params>::F = -(bdd<B, params>::T = 1);
	bdd_handle<B, params>::hfalse = bdd_handle<B, params>::get(bdd<B, params>::get(-1));
	bdd_handle<B, params>::htrue = bdd_handle<B, params>::get(bdd<B, params>::get(1));
}*/

template<typename B, auto params = INV_IN | INV_OUT | VARSHIFT> void bdd_init() {
	if (!bdd<B, params>::V.empty()) return;
#ifdef DEBUG
	int s;
	cout << "bdd_init" << '<' <<
	     abi::__cxa_demangle(typeid(bdd<B, params>).name(), 0, 0, &s) <<
	     '>' << endl;
#endif
	auto one = get_one<B>();
	if constexpr (!(params & INV_OUT)) {
		auto zero = ~one;
		bdd<B, params>::V.emplace_back(zero);
		bdd<B, params>::Mb.emplace(zero, 0);
	} else bdd<B, params>::V.emplace_back(one);
	bdd<B, params>::V.emplace_back(one);
	bdd<B, params>::Mb.emplace(one, 1);
	//bdd<B, params>::Mb.emplace(B::one(), 1);
	if constexpr (params & INV_OUT)
		bdd<B, params>::F = -(bdd<B, params>::T = 1);
	else {
		bdd<B, params>::F = 0;
		bdd<B, params>::T = 1;
	}
	bdd_handle<B, params>::hfalse = bdd_handle<B, params>::get(
		bdd<B, params>::get(bdd<B, params>::bdd_not(1)));
	bdd_handle<B, params>::htrue = bdd_handle<B, params>::get(
		bdd<B, params>::get(1));
}

template<typename B, auto params>
bdd<B, params>::initializer::initializer() {
	bdd_init<B, params>();
}
#endif
