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

#include "dict.h"
#include "barr.h"
#include "bool.h"

#include <cstring>

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


hbdd<Bool> operator|(string x, string y) {
	bool nx = '\'' == x.back();
	bool ny = '\'' == y.back();
	if (nx) x.pop_back();
	if (ny) y.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) |
		bdd_handle<Bool>::bit(!ny, dict(y));
}

hbdd<Bool> operator&(string x, string y) {
	bool nx = '\'' == x.back();
	bool ny = '\'' == y.back();
	if (nx) x.pop_back();
	if (ny) y.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) &
		bdd_handle<Bool>::bit(!ny, dict(y));
}

hbdd<Bool> operator|(string x, const hbdd<Bool>& y) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) | y;
}

hbdd<Bool> operator&(string x, const hbdd<Bool>& y) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) & y;
}

hbdd<Bool> operator|(const hbdd<Bool>& y, string x) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) | y;
}

hbdd<Bool> operator&(const hbdd<Bool>& y, string x) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) & y;
}
