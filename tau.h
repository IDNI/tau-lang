#include "nso.h"
#define ever ;;

template<typename... BAs> struct tau;

template<typename... BAs> struct tau_lit {
	bool neg = false;
	tau<BAs...> f;

	tau_lit(const tau<BAs...>& f) : f(f) {}

	tau_lit operator~() const {
		tau r = *this;
		r.neg = !this->neg;
		return r;
	}

	bool operator<(const tau_lit& x) const {
		if (neg != x.neg) return neg < x.neg;
		return f < x.f;
	}
};

template<typename... BAs> struct tau_clause : public set<tau_lit<BAs...>> {
	tau_clause operator&(tau_clause x) const {
		for (auto& t : *this) x.insert(t);
		return x;
	}

	tau<BAs...> operator~() {
		tau<BAs...> r(false);
		for (auto& t : *this) r.insert(~t);
		return r;
	}

	static vector<tau_lit<BAs...>> iter(
		const vector<tau_lit<BAs...>>& v,
		const vector<set<int_t>>& q,
		const set<int_t>& init,
		const vector<int_t>& order) {
	}

	static tau<BAs...> quant(
		const vector<tau_lit<BAs...>>& v, vector<set<int_t>> q) {
		tau<BAs...> f(true);
		for (auto& x : v)
			if (v.neg) f = f & ~v.f;
			else f = f & v.f;
		size_t n = q.size();
		bool ex = true;
		while (n--) for (int_t x : q[n]) f = ex ? f.ex(x) : f.all(x);
		return f;
	}

	bool sat(	const vector<set<int_t>>& q,
			const set<int_t>& init,
			const vector<int_t>& order) const {
		vector<vector<tau_lit<BAs...>>> v(*this);
		for (ever) {
			v.push_back(iter(v.back()), q, init, order);
			auto x = quant(v.back(), q);
			if (x == false) return false;
			if (x == true) return true;
			size_t n = v.size() - 2;
			while (n--) if (v[n] == v.back()) return true;
		}
	}
};

template<typename... BAs> struct tau : public set<tau_clause<BAs...>> {
	typedef set<set<tau_lit<BAs...>>> base;
	enum type { ZERO, ONE, NONE } t;

	tau(bool b) : t(b ? ONE : ZERO) {}

	static const tau& zero() {
		static nso r(false);
		return r;
	}

	static const tau& one() {
		static nso r(true);
		return r;
	}

	bool operator==(bool b) const { return t == (b ? ONE : ZERO); }

	bool operator==(const tau& x) const {
		return t == x.t && (base)(*this) == (base)(x);
	}

	bool operator<(const tau& x) const {
		return t != x.t ? t < x.t : ((base)(*this) < (base)(x));
	}

	tau operator|(const tau& x) const {
		if (x == true || *this == true) return one();
		if (x == false) return *this;
		if (*this == false) return x;
		tau r = *this;
		for (auto& s : x) r.insert(s);
		return r;
	}

	tau operator&(const tau& t) const {
		if (t == false || *this == false) return zero();
		if (t == true) return *this;
		if (*this == true) return t;
		tau r;
		for (auto& x : *this)
			for (auto& y : t) {
				auto z = x & y;
				if (!z.empty()) r.insert(z);
			}
		return r;
	}

	tau operator~() const {
		if (*this == false) return one();
		if (*this == true) return zero();
		tau r;
		for (auto& t : *this) r = r | ~t;
		return r;
	}

	bool sat() const {
		for (auto& c : *this) if (c.sat()) return true;
		return false;
	}
private:
	tau() : t(NONE) {}
};
