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

template<typename B, bool inv_in, bool inv_out, bool varshift> struct bdd_handle;
template<typename B, bool inv_in, bool inv_out, bool varshift> using hbdd = sp<bdd_handle<B, inv_in, inv_out, varshift>>;

template<typename B, bool inv_in, bool inv_out, bool varshift>
bool operator==(const hbdd<B, inv_in, inv_out, varshift>& x, bool b) {
	return b ? x->is_one() : x->is_zero();
}

template<typename B, bool inv_in, bool inv_out, bool varshift>
hbdd<B, inv_in, inv_out, varshift> operator&(const hbdd<B, inv_in, inv_out, varshift>& x, const hbdd<B, inv_in, inv_out, varshift>& y) { return (*x) & y; }

template<typename B, bool inv_in, bool inv_out, bool varshift>
hbdd<B, inv_in, inv_out, varshift> operator|(const hbdd<B, inv_in, inv_out, varshift>& x, const hbdd<B, inv_in, inv_out, varshift>& y) { return (*x) | y; }

template<typename B, bool inv_in, bool inv_out, bool varshift>
hbdd<B, inv_in, inv_out, varshift> operator+(const hbdd<B, inv_in, inv_out, varshift>& x, const hbdd<B, inv_in, inv_out, varshift>& y) { return (y&~x)|(x&~y); }

template<typename B, bool inv_in, bool inv_out, bool varshift> 
hbdd<B, inv_in, inv_out, varshift> operator~(const hbdd<B, inv_in, inv_out, varshift>& x) { return ~(*x); }

#ifdef DEBUG
template<typename B, bool inv_in, bool inv_out, bool varshift> 
bool operator==(const hbdd<B, inv_in, inv_out, varshift>& x, const hbdd<B, inv_in, inv_out, varshift>& y) {
	assert((&*x == &*y) == (x->b == y->b));
	return x->b == y->b;
}
#endif

template<typename B, bool inv_in, bool inv_out, bool varshift> struct bdd_handle {
	typedef unordered_map<bdd_node, shared_ptr<bdd_handle>> mn_type;
	typedef map<B, std::shared_ptr<bdd_handle>> mb_type;
	typedef hbdd<B, inv_in, inv_out, varshift> hbdd_t;
	inline static unordered_map<bdd_node, shared_ptr<bdd_handle>> Mn;
	inline static map<B, std::shared_ptr<bdd_handle>> Mb;
	inline static hbdd_t htrue, hfalse;

	// nonworking hack to call init
	template<typename T, T> struct dummy_type {};
	typedef dummy_type<mn_type&, Mn> dummy_mn_type;
	typedef dummy_type<mb_type&, Mb> dummy_mb_type;
	static bool dummy;

//	bdd_handle();

	static hbdd_t get(const bdd_node& x) {
		if (auto it = Mn.find(x); it != Mn.end())
			return it->second;//.lock();
		hbdd_t h = make_shared<bdd_handle<B, inv_in, inv_out, varshift>>();//(new bdd_handle);
		return h->b = bdd<B, inv_in, inv_out, varshift>::add(x), Mn.emplace(x, h), h;
	}

	static hbdd_t get(const B& x) {
		if (auto it = Mb.find(x); it != Mb.end())
			return it->second;//.lock();
		hbdd_t h = make_shared<bdd_handle<B, inv_in, inv_out, varshift>>();//(new bdd_handle);
		return h->b = bdd<B, inv_in, inv_out, varshift>::add(x), Mb.emplace(x, h), h;
	}

	static hbdd_t get(const bdd<B, inv_in, inv_out, varshift>& x) {
		return	x.leaf() ? get(std::get<B>(x))
			: get(std::get<bdd_node>(x));
	}

	static hbdd_t get(int_t t) { return get(bdd<B, inv_in, inv_out, varshift>::get(t)); }

	bdd<B, inv_in, inv_out, varshift> get() const { return bdd<B, inv_in, inv_out, varshift>::get(b); }

	bool is_zero() const { return b == bdd<B, inv_in, inv_out, varshift>::F; }
	bool is_one() const { return b == bdd<B, inv_in, inv_out, varshift>::T; }

	static hbdd_t one() { return get(bdd<B, inv_in, inv_out, varshift>::T); }
	static hbdd_t zero() { return get(bdd<B, inv_in, inv_out, varshift>::F); }

	static hbdd_t bit(bool b, int_t v) {
		DBG(assert(v > 0);)
		hbdd_t r = get(bdd<B, inv_in, inv_out, varshift>::bit(b ? v : -v));
		//hbdd_t r = get(bdd_node(v, bdd<B, inv_in, inv_out, varshift>::T, bdd<B, inv_in, inv_out, varshift>::F));
		DBG(assert(r);)
		return r;
	}

	B get_uelim() const { return bdd<B, inv_in, inv_out, varshift>::get_uelim(b); }
	B get_eelim() const { return bdd<B, inv_in, inv_out, varshift>::get_eelim(b); }

	hbdd_t operator&(const hbdd_t& x) const {
		const bdd<B, inv_in, inv_out, varshift> &xx = x->get();
		const bdd<B, inv_in, inv_out, varshift> &yy = get();
		if (xx.leaf()) {
#ifndef DEBUG
			if (std::get<B>(xx) == true) return get(*this);
			if (std::get<B>(xx) == false) return hfalse;
#endif
			if (yy.leaf())
				return	bdd_handle<B, inv_in, inv_out, varshift>::get(
					std::get<B>(xx) & std::get<B>(yy));
			return get(bdd<B, inv_in, inv_out, varshift>::bdd_and(b, std::get<B>(xx)));
		} else if (yy.leaf()) {
#ifndef DEBUG
			if (std::get<B, inv_in, inv_out, varshift>(yy) == true) return x;
			if (std::get<B, inv_in, inv_out, varshift>(yy) == false) return hfalse;
#endif
			return get(bdd<B, inv_in, inv_out, varshift>::bdd_and(x->b, std::get<B>(yy)));
		}
		return get(bdd<B, inv_in, inv_out, varshift>::bdd_and(x->b, b));
	}
	hbdd_t operator~() const { return get(
		bdd<B, inv_in, inv_out, varshift>::bdd_and(
			bdd<B, inv_in, inv_out, varshift>::T,
			bdd<B, inv_in, inv_out, varshift>::bdd_not(b))); }
	hbdd_t operator|(const hbdd_t& x) const { return ~((~x) & (~*this)); }

	hbdd_t ex(int_t v) const { return get(bdd<B, inv_in, inv_out, varshift>::ex(b, v)); }
	hbdd_t all(int_t v) const { return get(bdd<B, inv_in, inv_out, varshift>::all(b, v)); }

	hbdd_t subst(size_t v, const hbdd_t& x) const {
#ifdef DEBUG
//		assert( get(bdd<B, inv_in, inv_out, varshift>::subst(b, v, x->b)) ==
//			((sub0(v) & ~x) | (sub1(v) & x)));
#endif
		return (sub0(v) & ~x) | (sub1(v) & x);
//		return get(bdd<B, inv_in, inv_out, varshift>::subst(b, v, x->b));
	}

	hbdd_t sub0(size_t v) const { return get(bdd<B, inv_in, inv_out, varshift>::sub0(b, v)); }
	hbdd_t sub1(size_t v) const { return get(bdd<B, inv_in, inv_out, varshift>::sub1(b, v)); }

	hbdd_t condition(size_t v, const hbdd_t& f) const {
		return subst(v, f->sub0(v)) | subst(v, ~(f->sub1(v)));
	}

	void dnf(function<bool(const pair<B, vector<int_t>>&)> f) const {
		vector<int_t> v;
		bdd<B, inv_in, inv_out, varshift>::dnf(b, v, [f](const pair<B, vector<int_t>>& v) {
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
		return bdd<B, inv_in, inv_out, varshift>::get_vars(b, r), r;
	}

	map<int_t, B> get_one_zero() const {
		map<int_t, B> m;
		bdd<B, inv_in, inv_out, varshift>::get_one_zero(b, m);
//#ifdef DEBUG
//		auto d = dnf();
//		bool t = false;
//		for (auto x : d) t |= (r == x.second);
//		assert(!t);
//#endif
		return m;
	}

	hbdd_t compose(const map<int_t, hbdd_t>& m) const {
		map<int_t, int_t> p;
		for (auto& x : m) p.emplace(x.first, x.second->b);
		return get(bdd<B, inv_in, inv_out, varshift>::compose(b, p));
	}

	B eval(map<int_t, B>& m) const { return bdd<B, inv_in, inv_out, varshift>::eval(b, m); }

	map<int_t, hbdd_t> lgrs() const {
		map<int_t, hbdd_t> r;
		if (b == bdd<B, inv_in, inv_out, varshift>::F) return r;
		DBG(assert((b != bdd<B, inv_in, inv_out, varshift>::T));)
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

/*template<typename B> void bdd_init() requires is_sp<B, inv_in, inv_out, varshift> {
	if (!bdd<B, inv_in, inv_out, varshift>::V.empty()) return;
#ifdef DEBUG
	int s;
	cout << "bdd_init" << '<' <<
		abi::__cxa_demangle(typeid(bdd<B, inv_in, inv_out, varshift>).name(), 0, 0, &s) <<
		'>' << endl;
#endif
	auto one = get_one<B, inv_in, inv_out, varshift>();
	bdd<B, inv_in, inv_out, varshift>::V.emplace_back(one);
	bdd<B, inv_in, inv_out, varshift>::V.emplace_back(one);
	bdd<B, inv_in, inv_out, varshift>::Mb.emplace(one, 1);
	bdd<B, inv_in, inv_out, varshift>::F = -(bdd<B, inv_in, inv_out, varshift>::T = 1);
	bdd_handle<B, inv_in, inv_out, varshift>::hfalse = bdd_handle<B, inv_in, inv_out, varshift>::get(bdd<B, inv_in, inv_out, varshift>::get(-1));
	bdd_handle<B, inv_in, inv_out, varshift>::htrue = bdd_handle<B, inv_in, inv_out, varshift>::get(bdd<B, inv_in, inv_out, varshift>::get(1));
}*/

template<typename B, bool inv_in, bool inv_out, bool varshift> void bdd_init() {
	if (!bdd<B, inv_in, inv_out, varshift>::V.empty()) return;
#ifdef DEBUG
	int s;
	cout << "bdd_init" << '<' <<
		abi::__cxa_demangle(typeid(bdd<B, inv_in, inv_out, varshift>).name(), 0, 0, &s) <<
		'>' << endl;
#endif
	auto one = get_one<B>();
	if(!inv_out) {
		auto zero = ~one;
		bdd<B, inv_in, inv_out, varshift>::V.emplace_back(zero);
		bdd<B, inv_in, inv_out, varshift>::Mb.emplace(zero, 0);
	} else {
		bdd<B, inv_in, inv_out, varshift>::V.emplace_back(one);
	}
	bdd<B, inv_in, inv_out, varshift>::V.emplace_back(one);
	bdd<B, inv_in, inv_out, varshift>::Mb.emplace(one, 1);
	//bdd<B, inv_in, inv_out, varshift>::Mb.emplace(B::one(), 1);
	if(inv_out) bdd<B, inv_in, inv_out, varshift>::F = -(bdd<B, inv_in, inv_out, varshift>::T = 1);
	else {
		bdd<B, inv_in, inv_out, varshift>::F = 0;
		bdd<B, inv_in, inv_out, varshift>::T = 1;
	}
	bdd_handle<B, inv_in, inv_out, varshift>::hfalse =
		bdd_handle<B, inv_in, inv_out, varshift>::get(
			bdd<B, inv_in, inv_out, varshift>::get(
				bdd<B, inv_in, inv_out, varshift>::bdd_not(1)));
	bdd_handle<B, inv_in, inv_out, varshift>::htrue =
		bdd_handle<B, inv_in, inv_out, varshift>::get(
			bdd<B, inv_in, inv_out, varshift>::get(1));
}

template<typename B, bool inv_in, bool inv_out, bool varshift> 
bdd<B, inv_in, inv_out, varshift>::initializer::initializer() { bdd_init<B, inv_in, inv_out, varshift>(); }
#endif
