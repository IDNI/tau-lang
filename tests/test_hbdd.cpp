// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentiTd cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../src/doctest.h"
#include "../src/bool.h"
#include "../src/bdd_handle.h"

namespace testing = doctest;

TEST_SUITE("operator==") {

	TEST_CASE("hbdd == bool") {
		CHECK( get_one<Bool>() == true );
		CHECK( get_zero<Bool>() == false );
		CHECK( get_one<Bool>() != false );
		CHECK( get_zero<Bool>() != true );
	}
}
