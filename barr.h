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

#ifndef __BARR_H__ // boolean algebra with recurrence relations
#define __BARR_H__
#include "normalizer.h"

typedef pair<array<int_t, 2>, vector<int_t>> named; // name+idx+args[]

template<typename... BAs> struct call {
	named n;
	int_t idx;
	vector<variant<int_t, BAs...>> args;
	set<int_t> get_vars() const {
		set<int_t> r;
		for (auto& x : args)
			if (holds_alternative<int_t>(x))
				r.insert(get<int_t>(x));
		return r;
	}
	bool operator<(const call& c) const {
		if (idx != c.idx) return idx < c.idx;
		if (n != c.n) return n < c.n;
		return args < c.args;
	}
	bool operator==(const call& c) const {
		return idx == c.idx && n == c.n && args == c.args;
	}
};

template<typename... BAs> struct calls : public set<call<BAs...>> {
	typedef set<call<BAs...>> base;
	using base::base;
	bool neg = false;
	calls(const call<BAs...>& c) : base() { this->insert(c); }
	calls operator&(const calls& c) const {
		assert(neg == c.neg);
		calls r = c;
		r.insert(this->begin(), this->end());
		return r;
	}
	set<int_t> get_vars() const {
		set<int_t> r, s;
		for (auto& x : *this)
			s = x.get_vars(),
			r.insert(s.begin(), s.end());
		return r;
	}
	calls operator~() const {
		assert(!neg);
		calls c = *this;
		return c.neg = true, c;
	}
	bool operator<(const calls& c) const {
		if (neg != c.neg) return neg < c.neg;
		return (const base&)c < (base&)(*this);
	}
};

// put vec+map of named in barr, named->barr
// then operator& etc creates a tmpname for the dereffed barr&barr
// also whenever named points to something quantifier free and index free,
// it has to be eliminated.
// well or just quantifier free, and we get unravelling automatically.
template<typename... BAs>
//struct barr : public msba<tuple<hbdd<barr<BAs...>>, hbdd<BAs>...>, call> {
//	typedef msba<tuple<hbdd<barr<BAs...>>, hbdd<BAs>...>, call> base;
struct barr : public msba<tuple<BAs...>, calls<BAs...>> {
	typedef call<BAs...> call_t;
	typedef calls<BAs...> calls_t;
	typedef msba<tuple<BAs...>, calls_t> base;
	typedef barr<BAs...> barr_t;
	typedef tuple<set<BAs>..., set<calls<BAs>>...> clause_t;
	typedef set<clause_t> dnf_t;
	using base::base;
	inline static map<named, barr> N;
	barr(const barr& f) : base(f), qs(f.qs) {}
	vector<int_t> qs; // pos = ex, inner = q[0]
	barr(const base& t) : base(t) {}

	static void init() {
		bdd_init<Bool>();
		(bdd_init<BAs>(), ...);
		base::init();
	}

	template<typename T> static set<int_t> get_vars(const T& t) {
		return t.get_vars();
	}

	template<typename T>
	static set<int_t> get_vars(const hbdd<T>& t) {
		return t->get_vars();
	}

	map<typename base::elem, int_t> get_vars() const {
		map<typename base::elem, set<int_t>> r;
		set<int_t> v = this->b->get_vars();
		auto f = [&r](auto x) {
			set<int_t> v = get_vars(x);
			r[x].insert(v.begin(), v.end());
		};
		for (int_t x : v) visit(f, this->V[x]);
		return r;
	}

	static dnf_t break_calls(const clause_t& c) {
		sbf r = sbf_T, s = sbf_F;
		calls_t cs;
		for (const calls_t& x : std::get<set<calls_t>>(c))
			if (x.neg) continue;
			else for (const call_t& y : x) cs.insert(y);
		for (const call_t& x : cs)
			r = 	(r & bdd_handle<Bool>::
				bit(true, base::get(calls(x))));
		cs.clear();
		for (const calls_t& x : std::get<set<calls_t>>(c))
			if (x.neg) cs.insert(x.begin(), x.end());
		for (const call_t& x : cs)
			s = 	(s | bdd_handle<Bool>::
				bit(false, base::get(calls(x))));
		return base::dnf((r & s)->dnf());
	}

	static dnf_t break_calls(const dnf_t& d) {
		dnf_t r;
		for (auto t : d)
			for (auto x : break_calls(t))
				r.insert(x);
		return r;
	}

	void deref() {
	}
	void normalize() {
		set<pair<Bool, vector<int_t>>> s = this->b->dnf();
		dnf_t x = base::dnf(s);
		x = break_calls(x);
//		push_down_q();
		base::normalize(s);
	}

	barr& operator=(const barr& x) {
		((base&)*this) = (const base&)(x);
		return qs = x.qs, *this;
	}
	barr operator&(const barr& x) const { return base::operator&(x); }
	barr operator|(const barr& x) const { return base::operator|(x); }

	barr operator~() const {
		barr r(base::operator~());
		for (int_t q : qs) r.qs.push_back(-q);
		return r;
	}

	static barr zero() {
		static barr r(false);
		return r;
	}

	static barr one() {
		static barr r(true);
		return r;
	}
};
#endif
