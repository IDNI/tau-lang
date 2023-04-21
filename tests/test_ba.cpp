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
	TEST_CASE("the product of one 0/1 boolean algebra is the correct boolean algebra") {
	
		ba_product<ba_01> F(ba_01(false));
		ba_product<ba_01> T(ba_01(true));

		
		CHECK( get<0>(F && F).value == false );
		CHECK( get<0>(F && T).value == false);	
		CHECK( get<0>(T && F).value == false);	
		CHECK( get<0>(T && T).value == true);	

		CHECK( get<0>(!F).value == true);	
		CHECK( get<0>(!T).value == false);	
	}
	TEST_CASE("the product of two 0/1 boolean algebra is the correct boolean algebra") {
	
		ba_product<ba_01, ba_01> FF(ba_01(false), ba_01(false));
		ba_product<ba_01, ba_01> FT(ba_01(false), ba_01(true));
		ba_product<ba_01, ba_01> TF(ba_01(true), ba_01(false));
		ba_product<ba_01, ba_01> TT(ba_01(true), ba_01(true));

		
		CHECK( get<0>(FF && FF).value == false );
		CHECK( get<1>(FF && FF).value == false );

		CHECK( get<0>(FF && FT).value == false );
		CHECK( get<1>(FF && FT).value == false );
		
		CHECK( get<0>(FF && TF).value == false );
		CHECK( get<1>(FF && TF).value == false );
		
		CHECK( get<0>(FF && TT).value == false );
		CHECK( get<1>(FF && TT).value == false );
		
		CHECK( get<0>(FT && FF).value == false );
		CHECK( get<1>(FT && FF).value == false );
		
		CHECK( get<0>(FT && FT).value == false );
		CHECK( get<1>(FT && FT).value == true );
		
		CHECK( get<0>(FT && TF).value == false );
		CHECK( get<1>(FT && TF).value == false );
		
		CHECK( get<0>(FT && TT).value == false );
		CHECK( get<1>(FT && TT).value == true );
		
		CHECK( get<0>(TF && FF).value == false );
		CHECK( get<1>(TF && FF).value == false );
		
		CHECK( get<0>(TF && FT).value == false );
		CHECK( get<1>(TF && FT).value == false );
		
		CHECK( get<0>(TF && TF).value == true );
		CHECK( get<1>(TF && TF).value == false );
		
		CHECK( get<0>(TF && TT).value == true );
		CHECK( get<1>(TF && TT).value == false );
		
		CHECK( get<0>(TT && FF).value == false );
		CHECK( get<1>(TT && FF).value == false );
		
		CHECK( get<0>(TT && FT).value == false );
		CHECK( get<1>(TT && FT).value == true );
		
		CHECK( get<0>(TT && TF).value == true );
		CHECK( get<1>(TT && TF).value == false );
		
		CHECK( get<0>(TT && TT).value == true );
		CHECK( get<1>(TT && TT).value == true );

		CHECK( get<0>(!FF).value == true);	
		CHECK( get<1>(!FF).value == true);	

		CHECK( get<0>(!FT).value == true);
		CHECK( get<1>(!FT).value == false);

		CHECK( get<0>(!TF).value == false);
		CHECK( get<1>(!TF).value == true);

		CHECK( get<0>(!TT).value == false);
		CHECK( get<1>(!TT).value == false);
	}
}
