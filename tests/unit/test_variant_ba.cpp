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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "bool_ba.h"
#include "variant_ba.h"

namespace testing = doctest;

TEST_SUITE("variant Boolen algebra") {

	TEST_CASE("variant Bool == Bool") {
		CHECK( variant_ba<Bool>(Bool(true)) == true );
		CHECK( variant_ba<Bool>(Bool(false)) == false );
		CHECK( variant_ba<Bool>(Bool(true)) != false );
		CHECK( variant_ba<Bool>(Bool(false)) != true );
	}

	TEST_CASE("operator&") {
		CHECK( (variant_ba<Bool>(Bool(true)) & variant_ba<Bool>(Bool(true))) == true );
		CHECK( (variant_ba<Bool>(Bool(true)) & variant_ba<Bool>(Bool(false))) == false );
		CHECK( (variant_ba<Bool>(Bool(false)) & variant_ba<Bool>(Bool(true))) == false );
		CHECK( (variant_ba<Bool>(Bool(false)) & variant_ba<Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator|") {
		CHECK( (variant_ba<Bool>(Bool(true)) | variant_ba<Bool>(Bool(true))) == true );
		CHECK( (variant_ba<Bool>(Bool(true)) | variant_ba<Bool>(Bool(false))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) | variant_ba<Bool>(Bool(true))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) | variant_ba<Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator^") {
		CHECK( (variant_ba<Bool>(Bool(true)) ^ variant_ba<Bool>(Bool(true))) == false );
		CHECK( (variant_ba<Bool>(Bool(true)) ^ variant_ba<Bool>(Bool(false))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) ^ variant_ba<Bool>(Bool(true))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) ^ variant_ba<Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator+") {
		CHECK( (variant_ba<Bool>(Bool(true)) + variant_ba<Bool>(Bool(true))) == false );
		CHECK( (variant_ba<Bool>(Bool(true)) + variant_ba<Bool>(Bool(false))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) + variant_ba<Bool>(Bool(true))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) + variant_ba<Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator~") {
		CHECK( ~variant_ba<Bool>(Bool(true)) == false );
		CHECK( ~variant_ba<Bool>(Bool(false)) == true );
	}
}