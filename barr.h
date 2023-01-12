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
typedef pair<pair<named, int_t>, vector<int_t>> call; // named+idx+args[]

struct calls : public set<call> {
};

template<typename... BAs>
struct barr : public msba<tuple<hbdd<barr<BAs...>>, hbdd<BAs>...>> {
	typedef msba<tuple<hbdd<barr<BAs...>>, hbdd<BAs>...>> base;
	typedef barr<BAs...> barr_t;
	using base::base;

	barr operator&(const barr& x) const { return base::operator&(x); }
	barr operator|(const barr& x) const { return base::operator&(x); }
	barr operator~() const { return base::operator~(); }

	barr(const base& t) : base(t) {}

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
