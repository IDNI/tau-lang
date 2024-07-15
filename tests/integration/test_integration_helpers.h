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

#ifndef __TEST_INTEGRATION_HELPERS_H__
#define __TEST_INTEGRATION_HELPERS_H__

#include "doctest.h"
#include "nso_rr.h"
#include "bool_ba.h"
#include "bdd_handle.h"
#include "normalizer.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

template<typename...BAs>
bool operator==(nso<BAs...> n, std::string s) {
	std::stringstream ss;
	ss << n;
	return ss.str() == s;
}

#endif // __TEST_INTEGRATION_HELPERS_H__