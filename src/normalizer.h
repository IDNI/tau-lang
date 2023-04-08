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
#ifndef __NORMALIZER_H__
#define __NORMALIZER_H__
#include "msba.h"
#include "bdd_handle.h"

template <typename N /* parse tree node adapter */, typename B /* boolean algebra */>
class builder {

	enum struct bf_t { VAR, ELEM, TRUE, FALSE, REF, AND, NOT, FOR_ALL, EXISTS };
	enum struct cbf_t { REF, BF, CONDITION, AND, NOT };
	enum struct wwf_t { CBF, REF, NOT, AND, FOR_ALL, EXISTS };
	enum struct defs_t { CBF, WWF };

	bdd_handle<B> build_bf(N node) { 
		switch (node.get_type()) {
			case bf_t::VAR: /* build_bf_var */; break;
			case bf_t::ELEM: /* build_bf_elem */; break;
			case bf_t::TRUE: return B::one();
			case bf_t::FALSE: return B::zero();
			case bf_t::REF: /* build_bf_ref */; break;
			case bf_t::AND: return build_bf(node.left()) && build_bf(node.right()); 
			case bf_t::NOT: return !build_bf(node.child());
			case bf_t::FOR_ALL: /* build_bf_for_all */; break;
			case bf_t::EXISTS: /* build_bf_exists */; break;
			// TODO change by assert(false);
			default: break;
		}
		bdd_handle<B> b; return b; 
	}

	bdd_handle<B> build_cbf(N node) { 
		switch (node.get_type()) {
			case cbf_t::REF: /* build_cbf_ref */; break;
			case cbf_t::BF: return build_bf(node);
			case cbf_t::CONDITION: /* build_cbf_condition*/ ; break;
			case cbf_t::AND: return build_bf(node.left()) && build_bf(node.right()); 
			case cbf_t::NOT: return !build_bf(node.child());
			// TODO change by assert(false);
			default: bdd_handle<B> b; return b;

		}
		bdd_handle<B> b; return b; 
	}

	bdd_handle<B> build_wwf(N node) { 
		switch (node.get_type()) {
			case wwf_t::CBF: return build_cbf(node);
			case wwf_t::REF: /* build_wwf_ref */; break;
			case wwf_t::NOT: return !build_wwf(node.child());
			case wwf_t::AND: return build_wwf(node.left()) && build_wwf(node.right()); 
			case wwf_t::FOR_ALL: /* build_wwf_for_all */; break;
			case wwf_t::EXISTS: /* build_wwf_exists */; break;
			// TODO change by assert(false);
			default: break;
		}
		bdd_handle<B> b; return b; 
	}

public: 

	bdd_handle<B> build(N node) {
		for (auto& child: node.get_defs())
			switch(child.get_type()) {
				case defs_t::CBF: build_cbf(node); break;
				case defs_t::WFF: build_wwf(node); break;
				// TODO change by assert(false);
				default: break;
			}
		bdd_handle<B> b; return b; 
	};

};

template<typename... BDDs, typename... aux>
struct normalizer<tuple<BDDs...>, aux...> {
	typedef msba<tuple<BDDs...>, aux...> msba_t;
	typedef tuple<array<set<BDDs>, 2>..., array<set<aux>, 2>...> splitted;

	template<typename T> using arr_t = array<set<remove_cvref_t<T>>, 2>;

	static splitted to_tuple(
		const vector<int_t>& pos, const vector<int_t>& neg) {
		splitted r;
		auto f = [&r](const auto& x) {
			std::get<arr_t<decltype(x)>>(r)[0].insert(x); };
		auto g = [&r](const auto& x) {
			std::get<arr_t<decltype(x)>>(r)[1].insert(x); };
		for (int_t i : pos) visit(f, msba_t::V[i]);
		for (int_t i : neg) visit(g, msba_t::V[i]);
		return r;
	}

	template<typename B>
	static msba_t normalize(const set<B>& pos, const set<B>& neg) {
//		for (const B& x : pos)
//			for (const B& y : neg)
//				if ((x & y) == y)
//					return msba_t(false);
		msba_t r(true);
		for (const auto& x : pos) r = (r & msba_t(true, x));
		for (const auto& x : neg) r = (r & msba_t(false, x));
		return r;
	}

	template<typename B, auto o = bdd_options<>::create()>
	static msba_t normalize( const set<hbdd<B, o>>& pos,
		const set<hbdd<B, o>>& neg) {
		for (auto x : pos)
			for (auto y : neg)
				if ((x & y) == y)
					return msba_t(false);
		hbdd<B, o> p = ~get_one<hbdd<B, o>>();
		for (const auto& x : pos) p = (p | x);
		msba_t r(true, p);
		hbdd<B, o> np = ~p;
		if (!(p->get_uelim() == false)) return msba_t(false); 
		auto t = p->lgrs();
		for (const hbdd<B, o>& x : neg) {
			hbdd<B, o> z = x->compose(t) & np;
			//hbdd<B> z = (x & np);
			if (!(z->get_uelim() == false)) continue;
			else if ((r = (r & msba_t(false, z))) == false) return r;
		}
		return r;
	}

	static msba_t normalize(const splitted& t) {
		msba_t r(true);
		auto f = [&r](const auto& x) {
			if (auto t = normalize(x[0], x[1]); t == false) throw 0;
			else r = (r & t);
		};
		try {	(f(std::get<array<set<BDDs>, 2>>(t)), ...);
			return r;
		} catch (...) { return sbf_F; }
	}
};
#endif
