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
#include "../src/ba.h"

namespace testing = doctest;

TEST_SUITE("product boolean algebra") {

	// Sample 0/1 boolean algebra for testing purposes.
	struct ba_01 {
		bool value;

		ba_01 operator&(const ba_01& that) { return {value & that.value}; }
		ba_01 operator!() { return {!value}; }
		auto operator<=>(const ba_01& that) const = default;
	};

	TEST_CASE("the product of one 0/1 boolean algebra is the correct boolean algebra") {
	
		ba_product<ba_01> F(ba_01(false));
		ba_product<ba_01> T(ba_01(true));

		CHECK( (F & F) == F);
		CHECK( (F & T) == F);
		CHECK( (T & F) == F);
		CHECK( (T & T) == T);


		CHECK( !F == T);	
		CHECK( !T == F);	
	}

	TEST_CASE("the product of two 0/1 boolean algebra is the correct boolean algebra") {
	
		ba_product<ba_01, ba_01> FF(ba_01(false), ba_01(false));
		ba_product<ba_01, ba_01> FT(ba_01(false), ba_01(true));
		ba_product<ba_01, ba_01> TF(ba_01(true), ba_01(false));
		ba_product<ba_01, ba_01> TT(ba_01(true), ba_01(true));

		CHECK( !FF == TT);
		CHECK( !FT == TF);
		CHECK( !TF == FT);
		CHECK( !TT == FF);

		CHECK( (FF & TT) == FF);
		CHECK( (FF & TF) == FF);	
		CHECK( (FF & FT) == FF);
		CHECK( (FF & FF) == FF);
		CHECK( (FT & TT) == FT);
		CHECK( (FT & TF) == FF);
		CHECK( (FT & FT) == FT);
		CHECK( (FT & FF) == FF);
		CHECK( (TF & TT) == TF);
		CHECK( (TF & FT) == FF);
		CHECK( (TF & TF) == TF);
		CHECK( (TF & FF) == FF);
		CHECK( (TT & TT) == TT);
		CHECK( (TT & TF) == TF);
		CHECK( (TT & FT) == FT);
		CHECK( (TT & FF) == FF);
	}
}
