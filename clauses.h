#include "clause.h"

template<typename... BAs> struct clauses : public tuple<clause<BAs>...> {
	clauses() {}
	clauses(bool b) {
//		auto f = [&b](auto& x) { b ? x.to_one() : x.to_zero(); };
//		(f(get<clause<BAs>>(*this)), ...);
		apply([&b](auto& x) { b ? x.to_one() : x.to_zero(); });
	}
	template<typename B> clauses(const clause<B>& c) {
		get<clause<B>>(*this) = c;
	}

	void apply(auto f) { (f(get<clause<BAs>>(*this)), ...); }
	void apply(auto f) const { (f(get<clause<BAs>>(*this)), ...); }

	template<typename B> void put(const clause<B>& c) {
		if (c == false) clear_all();
		get<clause<B>>(*this) = (c == true) ? clause<B>() : c;
	}

	template<typename B> clauses& clear() { put(clause<B>()); return *this; }
	void clear_all() { (clear<BAs>(), ...); }

	template<typename B> clauses& subst(int_t v, const hbdd<B>& t) {
		return put(get<clause<B>>(*this).subst(v, t)), *this;
	}

	template<typename B> clauses& operator&=(const clause<B>& c) {
		return put(get<clause<B>>(*this) & c), *this;
	}

	clauses operator&(clauses cs) const {
//		auto f = [&cs](auto x) { cs &= x; };
		apply([&cs](auto x) { cs &= x; });
//		(f(get<clause<BAs>>(*this)), ...);
		return cs;
	}

	template<typename B>
	static void put(tuple<set<clause<BAs>>...>& t,
			const set<clause<B>>& s) {
		get<set<clause<B>>>(t) = s;
	}

	tuple<set<clause<BAs>>...> operator~() const {
		tuple<set<clause<BAs>>...> r;
		apply([&r](auto& x) { put(r, ~x); });
//		auto f = [&r](auto& x) { put(r, ~x); };
//		(f(get<clause<BAs>>(*this)), ...);
		return r;
	}

	bool ex(int_t v) {
//		auto f = [v](auto& c) { if (!c.ex(v)) throw 0; };
		try {
			apply([v](auto& c) { if (!c.ex(v)) throw 0; });
		} catch (...) { return false; }
		return true;
	}
};
