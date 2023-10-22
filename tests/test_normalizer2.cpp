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
#include "../src/normalizer2.h"
#include "../src/bdd_handle.h"

using namespace idni::rewriter;
using namespace idni::tau;
using namespace std;

namespace testing = doctest;

// TODO (HIGH) write tests that check the normalizer
// from parsing source, conversion to tau sources, final form, bindings, etc.

TEST_SUITE("parsing of steps") {
	
	TEST_CASE("apply_defs") {
		CHECK( apply_defs<Bool>.size() == 9 );
	}
	
	TEST_CASE("to_dnf_wff") {
		CHECK( to_dnf_wff<Bool>.size() == 6 );
	}
	
	TEST_CASE("to_dnf_cbf") {
		CHECK( to_dnf_cbf<Bool>.size() == 5 );
	}
	
	TEST_CASE("simplify_bf") {
		CHECK( simplify_bf<Bool>.size() == 14 );
	}
	
	TEST_CASE("simplify_wff") {
		CHECK( simplify_wff<Bool>.size() == 14 );
	}
	
	TEST_CASE("simplify_cbf") {
		CHECK( simplify_cbf<Bool>.size() == 14 );
	}
	
	TEST_CASE("apply_cb") {
		CHECK( apply_cb<Bool>.size() == 9 );
	}
	
	TEST_CASE("wff_reduce") {
		CHECK( wff_reduce<Bool>.size() == 7 );
	}
	
	TEST_CASE("trivialities") {
		CHECK( trivialities<Bool>.size() == 4 );
	}
}

TEST_SUITE("steps"){
// TODO (HIGH) write tests to check steps

}

TEST_SUITE("repeat_each"){
// TODO (HIGH) write tests to check repeat_each

}

TEST_SUITE("repeat_all"){
// TODO (HIGH) write tests to check repeat_all

}

TEST_SUITE("operator|"){
// TODO (HIGH) write tests to check operator|

}
