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
