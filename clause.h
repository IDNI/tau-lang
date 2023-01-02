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
#include "bdd_handle.h"

template<typename B> struct clause : public pair<hbdd<B>, set<hbdd<B>>> {
	clause() {}
	clause(const hbdd<B>& f) { this->first = f; }

	static const clause<B>& zero() {
		static clause<B> r(bdd_handle<B>::htrue);
		return r;
	}

	static const clause<B>& one() {
		static clause<B> r(bdd_handle<B>::hfalse);
		return r;
	}

	void to_zero() { *this = zero(); }
	void to_one() { *this = one(); }

	bool empty() const { return !this->first && this->second.empty(); }

	bool operator==(bool b) const {
		return	this->first && (
			b ? (this->first == false) : (this->first == true));
	}

	clause<B>& operator+=(const hbdd<B>& f) {
		if (!(f->get_uelim() == false)) return *this = zero();
		this->first = (!this->first ? f : (this->first | f));
		return *this;
	}

	clause<B>& operator-=(const hbdd<B>& f) {
		return	f == false ? *this = zero()
			: (this->second.insert(f), *this);
	}

	clause<B> operator&(clause<B> x) const {
		if (*this == false || x == false) return zero();
		if (x == true) return *this;
		if (*this == true) return x;
		if (this->first) x += this->first;
		for (const hbdd<B>& f : this->second) x -= f;
		return x.simplify(), x;
	}

	set<clause<B>> operator~() const {
		set<clause<B>> r;
		clause<B> c;
		if (this->first) r.insert(c -= this->first);
		for (const hbdd<B>& f : this->second) 
			r.insert(clause<B>() += f);
		return r;
	}

	bool ex(int_t v) {
		set<hbdd<B>> s;
		if (!this->first) {
			for (hbdd<B> g : this->second) s.insert(g->ex(v));
			return this->second = s, true;
		}
		hbdd<B> f = this->first;
//		cout << f << endl;
		if (hbdd<B> g = f->all(v); g == true) return false;
		else this->first = g;
//		cout << this->first << endl;
		for (hbdd<B> g : this->second)
			if ((g = g->condition(v, f)) == false) return false;
			else s.insert(g);
		return this->second = s, true;
	}

	clause<B> subst(int_t s, const hbdd<B>& t) const {
		clause r;
		const hbdd<B>& f = this->first.subst(s, t);
		if (f == hbdd<B>::htrue) return zero();
		r += f;
		for (const hbdd<B>& g : this->second)
			if ((f = g.subst(s, t))->zero()) return zero();
			else r -= f;
		r.simplify();
	}

	void simplify() {
		return;
		if (!this->first || *this == true || *this == false) return;
		set<hbdd<B>> s;
		for (const hbdd<B>& f : this->second)
			if (const hbdd<B>& g = f & ~this->first; g->zero())
				{ *this = zero(); return; }
			else s.insert(g);
		this->second = s;
	}
};
