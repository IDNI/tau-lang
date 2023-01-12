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

template<typename B> struct bdd_handle;
template<typename B> using hbdd = sp<bdd_handle<B>>;

template<typename B>
bool operator==(const hbdd<B>& x, bool b) {
	return b ? x->is_one() : x->is_zero();
}

template<typename B>
hbdd<B> operator&(const hbdd<B>& x, const hbdd<B>& y) { return (*x) & y; }

template<typename B>
hbdd<B> operator|(const hbdd<B>& x, const hbdd<B>& y) { return (*x) | y; }

template<typename B>
hbdd<B> operator+(const hbdd<B>& x, const hbdd<B>& y) { return (y&~x)|(x&~y); }

template<typename B> hbdd<B> operator~(const hbdd<B>& x) { return ~*x; }

#ifdef DEBUG
template<typename B> bool operator==(const hbdd<B>& x, const hbdd<B>& y) {
	assert((&*x == &*y) == (x->b == y->b));
	return x->b == y->b;
}
#endif

template<typename B> struct bdd_handle {
	typedef unordered_map<bdd_node, shared_ptr<bdd_handle>> mn_type;
	typedef map<B, std::shared_ptr<bdd_handle>> mb_type;
	inline static unordered_map<bdd_node, shared_ptr<bdd_handle>> Mn;
	inline static map<B, std::shared_ptr<bdd_handle>> Mb;
	inline static hbdd<B> htrue, hfalse;

	// nonworking hack to call init
	template<typename T, T> struct dummy_type {};
	typedef dummy_type<mn_type&, Mn> dummy_mn_type;
	typedef dummy_type<mb_type&, Mb> dummy_mb_type;
	static bool dummy;

//	bdd_handle();

	static hbdd<B> get(const bdd_node& x) {
		if (auto it = Mn.find(x); it != Mn.end())
			return it->second;//.lock();
		hbdd<B> h = make_shared<bdd_handle<B>>();//(new bdd_handle);
		return h->b = bdd<B>::add(x), Mn.emplace(x, h), h;
	}

	static hbdd<B> get(const B& x) {
		if (auto it = Mb.find(x); it != Mb.end())
			return it->second;//.lock();
		hbdd<B> h = make_shared<bdd_handle<B>>();//(new bdd_handle);
		return h->b = bdd<B>::add(x), Mb.emplace(x, h), h;
	}

	static hbdd<B> get(const bdd<B>& x) {
		return	x.leaf() ? get(std::get<B>(x))
			: get(std::get<bdd_node>(x));
	}

	static hbdd<B> get(int_t t) { return get(bdd<B>::get(t)); }

	bdd<B> get() const { return bdd<B>::get(b); }

	bool is_zero() const { return b == bdd<B>::F; }
	bool is_one() const { return b == bdd<B>::T; }

	static hbdd<B> one() { return get(bdd<B>::T); }
	static hbdd<B> zero() { return get(bdd<B>::F); }

	static hbdd<B> bit(bool b, int_t v) {
		DBG(assert(v);)
		hbdd<B> r = get(bdd_node(v, bdd<B>::T, bdd<B>::F));
		DBG(assert(r);)
		return b ? r : ~r;
	}

	B get_uelim() const { return bdd<B>::get_uelim(b); }
	B get_eelim() const { return bdd<B>::get_eelim(b); }

	hbdd<B> operator&(const hbdd<B>& x) const {
		const bdd<B> &xx = x->get();
		const bdd<B> &yy = get();
		if (xx.leaf()) {
#ifndef DEBUG
			if (std::get<B>(xx) == true) return get(*this);
			if (std::get<B>(xx) == false) return hfalse;
#endif
			if (yy.leaf())
				return	bdd_handle<B>::get(
					std::get<B>(xx) & std::get<B>(yy));
			return get(bdd<B>::bdd_and(b, std::get<B>(xx)));
		} else if (yy.leaf()) {
#ifndef DEBUG
			if (std::get<B>(yy) == true) return x;
			if (std::get<B>(yy) == false) return hfalse;
#endif
			return get(bdd<B>::bdd_and(x->b, std::get<B>(yy)));
		}
		return get(bdd<B>::bdd_and(x->b, b));
	}

	hbdd<B> operator~() const { return get(bdd<B>::bdd_and(bdd<B>::T, -b)); }
	hbdd<B> operator|(const hbdd<B>& x) const { return ~((~x) & (~*this)); }

	hbdd<B> ex(int_t v) const { return get(bdd<B>::ex(b, v)); }
	hbdd<B> all(int_t v) const { return get(bdd<B>::all(b, v)); }

	hbdd<B> subst(size_t v, const hbdd<B>& x) const {
#ifdef DEBUG
//		assert( get(bdd<B>::subst(b, v, x->b)) ==
//			((sub0(v) & ~x) | (sub1(v) & x)));
#endif
		return (sub0(v) & ~x) | (sub1(v) & x);
//		return get(bdd<B>::subst(b, v, x->b));
	}

	hbdd<B> sub0(size_t v) const { return get(bdd<B>::sub0(b, v)); }
	hbdd<B> sub1(size_t v) const { return get(bdd<B>::sub1(b, v)); }

	hbdd<B> condition(size_t v, const hbdd<B>& f) const {
		return subst(v, f->sub0(v)) | subst(v, ~(f->sub1(v)));
	}

	void dnf(function<bool(const pair<B, vector<int_t>>&)> f) const {
		vector<int_t> v;
		bdd<B>::dnf(b, v, [f](const pair<B, vector<int_t>>& v) {
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
		return bdd<B>::get_vars(b, r), r;
	}

	map<int_t, B> get_one_zero() const {
		map<int_t, B> m;
		bdd<B>::get_one_zero(b, m);
//#ifdef DEBUG
//		auto d = dnf();
//		bool t = false;
//		for (auto x : d) t |= (r == x.second);
//		assert(!t);
//#endif
		return m;
	}

	hbdd<B> compose(const map<int_t, hbdd<B>>& m) const {
		map<int_t, int_t> p;
		for (auto& x : m) p.emplace(x.first, x.second->b);
		return get(bdd<B>::compose(b, p));
	}

	B eval(map<int_t, B>& m) const { return bdd<B>::eval(b, m); }

	map<int_t, hbdd<B>> lgrs() const {
		map<int_t, hbdd<B>> r;
		if (b == bdd<B>::F) return r;
		DBG(assert(b != bdd<B>::T);)
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
template<typename B> B get_one() { return B::one(); }

/*template<typename B> void bdd_init() requires is_sp<B> {
	if (!bdd<B>::V.empty()) return;
#ifdef DEBUG
	int s;
	cout << "bdd_init" << '<' <<
		abi::__cxa_demangle(typeid(bdd<B>).name(), 0, 0, &s) <<
		'>' << endl;
#endif
	auto one = get_one<B>();
	bdd<B>::V.emplace_back(one);
	bdd<B>::V.emplace_back(one);
	bdd<B>::Mb.emplace(one, 1);
	bdd<B>::F = -(bdd<B>::T = 1);
	bdd_handle<B>::hfalse = bdd_handle<B>::get(bdd<B>::get(-1));
	bdd_handle<B>::htrue = bdd_handle<B>::get(bdd<B>::get(1));
}*/

template<typename B> void bdd_init() {
	if (!bdd<B>::V.empty()) return;
#ifdef DEBUG
	int s;
	cout << "bdd_init" << '<' <<
		abi::__cxa_demangle(typeid(bdd<B>).name(), 0, 0, &s) <<
		'>' << endl;
#endif
	auto one = get_one<B>();
	bdd<B>::V.emplace_back(one);
	bdd<B>::V.emplace_back(one);
	bdd<B>::Mb.emplace(one, 1);
	//bdd<B>::Mb.emplace(B::one(), 1);
	bdd<B>::F = -(bdd<B>::T = 1);
	bdd_handle<B>::hfalse = bdd_handle<B>::get(bdd<B>::get(-1));
	bdd_handle<B>::htrue = bdd_handle<B>::get(bdd<B>::get(1));
}

template<typename B> bdd<B>::initializer::initializer() { bdd_init<B>(); }
#endif
