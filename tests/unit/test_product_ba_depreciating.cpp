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

#include "boolean_algebras/product_ba.h"
#include "doctest.h"

namespace testing = doctest;

TEST_SUITE("product boolean algebra") {

	// Classical 0/1 boolean algebra for testing purposes.
	struct ba_01 {
		bool value;

		ba_01 operator&(const ba_01& that) { return ba_01(value & that.value); }
		ba_01 operator|(const ba_01& that) { return ba_01(value | that.value); }
		ba_01 operator^(const ba_01& that) { return ba_01(value ^ that.value); }
		ba_01 operator~() { return {!value}; }
		auto operator<=>(const ba_01& that) const = default;
	};

	struct ba_01p {
		bool value;

		ba_01p operator&(const ba_01p& that) { return ba_01p(value & that.value); }
		ba_01p operator|(const ba_01p& that) { return ba_01p(value | that.value); }
		ba_01p operator^(const ba_01p& that) { return ba_01p(value ^ that.value); }
		ba_01p operator~() { return {!value}; }
		auto operator<=>(const ba_01p& that) const = default;
	};

	TEST_CASE("the product of one 0/1 boolean algebra is the correct boolean algebra") {

		product_ba<ba_01> F(ba_01(false));
		product_ba<ba_01> T(ba_01(true));

		CHECK( ~F == T); CHECK( ~T == F);

		CHECK( (F & F) == F); CHECK( (F & T) == F);
		CHECK( (T & F) == F); CHECK( (T & T) == T);

		CHECK( (F | F) == F); CHECK( (F | T) == T);
		CHECK( (T | F) == T); CHECK( (T | T) == T);

		CHECK( (F ^ F) == F); CHECK( (F ^ T) == T);
		CHECK( (T ^ F) == T); CHECK( (T ^ T) == F);
	}

	TEST_CASE("the product of two 0/1 boolean algebra is the correct boolean algebra") {

		product_ba<ba_01, ba_01p> FF(ba_01(false), ba_01p(false));
		product_ba<ba_01, ba_01p> FT(ba_01(false), ba_01p(true));
		product_ba<ba_01, ba_01p> TF(ba_01(true), ba_01p(false));
		product_ba<ba_01, ba_01p> TT(ba_01(true), ba_01p(true));

		CHECK( ~FF == TT); CHECK( ~FT == TF);
		CHECK( ~TF == FT); CHECK( ~TT == FF);

		CHECK( (FF & TT) == FF); CHECK( (FF & TF) == FF);
		CHECK( (FF & FT) == FF); CHECK( (FF & FF) == FF);
		CHECK( (FT & TT) == FT); CHECK( (FT & TF) == FF);
		CHECK( (FT & FT) == FT); CHECK( (FT & FF) == FF);
		CHECK( (TF & TT) == TF); CHECK( (TF & FT) == FF);
		CHECK( (TF & TF) == TF); CHECK( (TF & FF) == FF);
		CHECK( (TT & TT) == TT); CHECK( (TT & TF) == TF);
		CHECK( (TT & FT) == FT); CHECK( (TT & FF) == FF);

		CHECK( (FF | TT) == TT); CHECK( (FF | TF) == TF);
		CHECK( (FF | FT) == FT); CHECK( (FF | FF) == FF);
		CHECK( (FT | TT) == TT); CHECK( (FT | TF) == TT);
		CHECK( (FT | FT) == FT); CHECK( (FT | FF) == FT);
		CHECK( (TF | TT) == TT); CHECK( (TF | FT) == TT);
		CHECK( (TF | TF) == TF); CHECK( (TF | FF) == TF);
		CHECK( (TT | TT) == TT); CHECK( (TT | TF) == TT);
		CHECK( (TT | FT) == TT); CHECK( (TT | FF) == TT);

		CHECK( (FF ^ TT) == TT); CHECK( (FF ^ TF) == TF);
		CHECK( (FF ^ FT) == FT); CHECK( (FF ^ FF) == FF);
		CHECK( (FT ^ TT) == TF); CHECK( (FT ^ TF) == TT);
		CHECK( (FT ^ FT) == FF); CHECK( (FT ^ FF) == FT);
		CHECK( (TF ^ TT) == FT); CHECK( (TF ^ FT) == TT);
		CHECK( (TF ^ TF) == FF); CHECK( (TF ^ FF) == TF);
		CHECK( (TT ^ TT) == FF); CHECK( (TT ^ TF) == FT);
		CHECK( (TT ^ FT) == TF); CHECK( (TT ^ FF) == TT);
	}
}
