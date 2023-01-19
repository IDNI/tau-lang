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

hbdd<Bool, true, true, false> operator|(string x, string y) {
	bool nx = '\'' == x.back();
	bool ny = '\'' == y.back();
	if (nx) x.pop_back();
	if (ny) y.pop_back();
	return	bdd_handle<Bool, true, true, false>::bit(!nx, dict(x)) |
		bdd_handle<Bool, true, true, false>::bit(!ny, dict(y));
}

hbdd<Bool, true, true, false> operator&(string x, string y) {
	bool nx = '\'' == x.back();
	bool ny = '\'' == y.back();
	if (nx) x.pop_back();
	if (ny) y.pop_back();
	return	bdd_handle<Bool, true, true, false>::bit(!nx, dict(x)) &
		bdd_handle<Bool, true, true, false>::bit(!ny, dict(y));
}

hbdd<Bool, true, true, false> operator|(string x, const hbdd<Bool, true, true, false>& y) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool, true, true, false>::bit(!nx, dict(x)) | y;
}

hbdd<Bool, true, true, false> operator&(string x, const hbdd<Bool, true, true, false>& y) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool, true, true, false>::bit(!nx, dict(x)) & y;
}

hbdd<Bool, true, true, false> operator|(const hbdd<Bool, true, true, false>& y, string x) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool, true, true, false>::bit(!nx, dict(x)) | y;
}

hbdd<Bool, true, true, false> operator&(const hbdd<Bool, true, true, false>& y, string x) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool, true, true, false>::bit(!nx, dict(x)) & y;
}
