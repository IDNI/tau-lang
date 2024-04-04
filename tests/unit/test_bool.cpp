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

#include "doctest.h"
#include "bool.h"

namespace testing = doctest;

TEST_SUITE("operator==") {

	TEST_CASE("Bool == bool") {
		CHECK( Bool(true) == true );
		CHECK( Bool(false) == false );
		CHECK( Bool(true) != false );
		CHECK( Bool(false) != true );
	}

	TEST_CASE("operator&") {
		CHECK( (Bool(true) & Bool(true)) == true );
		CHECK( (Bool(true) & Bool(false)) == false );
		CHECK( (Bool(false) & Bool(true)) == false );
		CHECK( (Bool(false) & Bool(false)) == false );
	}

	TEST_CASE("operator|") {
		CHECK( (Bool(true) | Bool(true)) == true );
		CHECK( (Bool(true) | Bool(false)) == true );
		CHECK( (Bool(false) | Bool(true)) == true );
		CHECK( (Bool(false) | Bool(false)) == false );
	}

	TEST_CASE("operator^") {
		CHECK( (Bool(true) ^ Bool(true)) == false );
		CHECK( (Bool(true) ^ Bool(false)) == true );
		CHECK( (Bool(false) ^ Bool(true)) == true );
		CHECK( (Bool(false) ^ Bool(false)) == false );
	}

	TEST_CASE("operator+") {
		CHECK( (Bool(true) + Bool(true)) == false );
		CHECK( (Bool(true) + Bool(false)) == true );
		CHECK( (Bool(false) + Bool(true)) == true );
		CHECK( (Bool(false) + Bool(false)) == false );
	}

	TEST_CASE("operator~") {
		CHECK( (~Bool(true)) == Bool(false) );
		CHECK( (~Bool(false)) == Bool(true) );
	}
}
