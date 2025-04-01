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
#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/bdds/bdd_handle.h"

namespace testing = doctest;

TEST_SUITE("operator==") {

	TEST_CASE("hbdd == bool") {
		CHECK( get_one<Bool>() == true );
		CHECK( get_zero<Bool>() == false );
		CHECK( get_one<Bool>() != false );
		CHECK( get_zero<Bool>() != true );
	}
}

TEST_SUITE("operator&") {

	TEST_CASE("hbdd & hbdd") {
		CHECK( (get_one<Bool>() & get_one<Bool>()) == true );
		CHECK( (get_one<Bool>() & get_zero<Bool>()) == false );
		CHECK( (get_zero<Bool>() & get_one<Bool>()) == false );
		CHECK( (get_zero<Bool>() & get_zero<Bool>()) == false );
	}
}

TEST_SUITE("operator|") {

	TEST_CASE("hbdd | hbdd") {
		CHECK( (get_one<Bool>() | get_one<Bool>()) == true );
		CHECK( (get_one<Bool>() | get_zero<Bool>()) == true );
		CHECK( (get_zero<Bool>() | get_one<Bool>()) == true );
		CHECK( (get_zero<Bool>() | get_zero<Bool>()) == false );
	}
}

TEST_SUITE("operator^") {

	TEST_CASE("hbdd ^ hbdd") {
		CHECK( (get_one<Bool>() ^ get_one<Bool>()) == false );
		CHECK( (get_one<Bool>() ^ get_zero<Bool>()) == true );
		CHECK( (get_zero<Bool>() ^ get_one<Bool>()) == true );
		CHECK( (get_zero<Bool>() ^ get_zero<Bool>()) == false );
	}
}

TEST_SUITE("operator+") {

	TEST_CASE("hbdd + hbdd") {
		CHECK( (get_one<Bool>() + get_one<Bool>()) == false );
		CHECK( (get_one<Bool>() + get_zero<Bool>()) == true );
		CHECK( (get_zero<Bool>() + get_one<Bool>()) == true );
		CHECK( (get_zero<Bool>() + get_zero<Bool>()) == false );
	}
}

TEST_SUITE("operator~") {

	TEST_CASE("hbdd ~") {
		CHECK( (~get_one<Bool>()) == get_zero<Bool>() );
		CHECK( (~get_zero<Bool>()) == get_one<Bool>() );
	}
}

TEST_SUITE("BDD_Splitter") {
	TEST_CASE("DNF_clause_deletion1") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(true, 1);
		auto a2 = bdd_handle<Bool>::bit(true, 2);
		auto a3 = bdd_handle<Bool>::bit(true, 3);

		auto d1 = a1 | a2 | a3;
		CHECK(splitter(d1, splitter_type::upper) == (a1 | a2));
	}

	TEST_CASE("DNF_clause_deletion2") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(false, 1);
		auto a2 = bdd_handle<Bool>::bit(false, 2);
		auto a3 = bdd_handle<Bool>::bit(false, 3);

		auto d1 = a1 | a2 | a3;
		CHECK(splitter(d1, splitter_type::upper) == (a1 | a2));
	}

	TEST_CASE("single_DNF_clause") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(true, 1);
		auto a2 = bdd_handle<Bool>::bit(true, 2);
		auto a3 = bdd_handle<Bool>::bit(true, 3);
		auto a4 = bdd_handle<Bool>::bit(true, 4);

		auto d1 = a1 & a2 & a3;
		CHECK(splitter(d1, splitter_type::upper) == (d1 & a4));
	}
}
