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
bool operator==(const hbdd<B>& x, bool b) { return b ? x->one() : x->zero(); }

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
	static unordered_map<bdd_node, std::shared_ptr<bdd_handle>> Mn;
	static map<B, std::shared_ptr<bdd_handle>> Mb;
	static hbdd<B> htrue, hfalse;

	static hbdd<B> get(const bdd_node& x) {
		if (auto it = Mn.find(x); it != Mn.end())
			return it->second;//.lock();
		hbdd<B> h(new bdd_handle);
		h->b = bdd<B>::add(x);
		Mn.emplace(x, shared_ptr<bdd_handle<B>>(h));
		return h;
	}

	static hbdd<B> get(const B& x) {
		if (auto it = Mb.find(x); it != Mb.end())
			return it->second;//.lock();
		hbdd<B> h(new bdd_handle);
		h->b = bdd<B>::add(x);
		Mb.emplace(x, shared_ptr<bdd_handle<B>>(h));
		return h;
	}

	static hbdd<B> get(const bdd<B>& x) {
		return	x.leaf() ? get(std::get<B>(x))
			: get(std::get<bdd_node>(x));
	}

	static hbdd<B> get(int_t t) { return get(bdd<B>::get(t)); }

	bdd<B> get() const { return bdd<B>::get(b); }

	bool zero() const { return b == bdd<B>::F; }
	bool one() const { return b == bdd<B>::T; }

	static hbdd<B> bit(bool b, int_t v) {
		if (bdd<B>::V.empty()) bdd<B>::init();
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

	set<pair<B, vector<int_t>>> dnf() const {
		set<pair<B, vector<int_t>>> r;
		vector<int_t> v;
		return bdd<B>::dnf(b, r, v), r;
	}
#ifndef DEBUG
private:
#endif
	int_t b;
};

template<typename B> void bdd<B>::init() {
	bdd<B>::V.emplace_back(B::zero());
	bdd<B>::V.emplace_back(B::one());
	bdd<B>::Mb.emplace(B::one(), 1);
	bdd<B>::F = -(bdd<B>::T = 1);
	bdd_handle<B>::hfalse = bdd_handle<B>::get(bdd<B>::get(-1));
	bdd_handle<B>::htrue = bdd_handle<B>::get(bdd<B>::get(1));
}
#endif