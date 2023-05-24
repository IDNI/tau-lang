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
#include "../src/rewriting.h"
#include "test_rewriting/rewriting_parser.generated.h"

using namespace idni;

namespace testing = doctest;

TEST_SUITE("rewriting engine: helper functions") {

	size_t ignore = rewriting_parser::ignore;
	size_t variable = rewriting_parser::var;

	const char* ceci_est_une_var = "t(a b).";

	TEST_CASE("is_var") {

	}

	TEST_CASE("is_terminal") {
	}

	TEST_CASE("is_ignore") {
	}

	TEST_CASE("get_symbol") {
	}
}

TEST_SUITE("rewriting engine") {

	TEST_CASE("helper functions") {
	}

	TEST_CASE("test 1") {
	}

	TEST_CASE("test 2") {
	}
}
