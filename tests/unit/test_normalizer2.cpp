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

#include "../../src/doctest.h"
#include "../../src/normalizer2.h"
#include "../../src/bdd_handle.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("steps parsing") {

	TEST_CASE("apply_defs") {
		CHECK( apply_defs<Bool>.size() == 5 );
	}

	TEST_CASE("apply_defs_once") {
		CHECK( apply_defs_once<Bool>.size() == 5 );
	}

	TEST_CASE("elim_for_all") {
		CHECK( elim_for_all<Bool>.size() == 1 );
	}

	TEST_CASE("to_dnf_wff") {
		CHECK( to_dnf_wff<Bool>.size() == 7 );
	}

	TEST_CASE("simplify_bf") {
		CHECK( simplify_bf<Bool>.size() == 16 );
	}

	TEST_CASE("simplify_wff") {
		CHECK( simplify_wff<Bool>.size() == 16 );
	}

	TEST_CASE("apply_cb") {
		CHECK( apply_cb<Bool>.size() == 6 );
	}

	TEST_CASE("apply_speed_up_cb") {
		CHECK( apply_speed_up_cb<Bool>.size() == 4 );
	}

	TEST_CASE("squeeze_positives") {
		CHECK( squeeze_positives<Bool>.size() == 1 );
	}

	TEST_CASE("clause_simplify_bf") {
		CHECK( clause_simplify_bf<Bool>.size() == 2 );
	}

	TEST_CASE("clause_simplify_wff") {
		CHECK( clause_simplify_wff<Bool>.size() == 2 );
	}

	TEST_CASE("wff_remove_existential") {
		CHECK( wff_remove_existential<Bool>.size() == 1 );
	}

	TEST_CASE("bf_positives_upwards") {
		CHECK( bf_positives_upwards<Bool>.size() == 5 );
	}

	TEST_CASE("bf_positives_upwards") {
		CHECK( bf_positives_upwards<Bool>.size() == 5 );
	}

	TEST_CASE("bf_elim_quantifiers") {
		CHECK( bf_elim_quantifiers<Bool>.size() == 2 );
	}

	TEST_CASE("trivialities") {
		CHECK( trivialities<Bool>.size() == 4 );
	}
}

TEST_SUITE("steps"){
// TODO (MEDIUM) write tests to check steps

}

TEST_SUITE("repeat_each"){
// TODO (MEDIUM) write tests to check repeat_each

}

TEST_SUITE("repeat_all"){
// TODO (MEDIUM) write tests to check repeat_all

}

TEST_SUITE("repeat"){
// TODO (MEDIUM) write tests to check repeat

}

TEST_SUITE("operator|"){
// TODO (MEDIUM) write tests to check operator| (all versions)

}
