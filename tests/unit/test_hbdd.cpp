// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "boolean_algebras/bdds/bdd_handle.h"
#include "boolean_algebras/sbf_ba.h"

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

TEST_SUITE("Bool helpers") {
	TEST_CASE("zero/one/normalize/hash") {
		CHECK(Bool::zero().is_zero());
		CHECK(Bool::one().is_one());
		CHECK(normalize_bool(Bool(false)) == Bool(false));
		CHECK(normalize_bool(Bool(true)) == Bool(true));
		CHECK(std::hash<Bool>{}(Bool(false)) == 0);
		CHECK(std::hash<Bool>{}(Bool(true)) == 1);
	}
}

TEST_SUITE("BDD_Splitter") {
	TEST_CASE("DNF_clause_deletion1") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(true, 1);
		auto a2 = bdd_handle<Bool>::bit(true, 2);
		auto a3 = bdd_handle<Bool>::bit(true, 3);

		auto d1 = a1 | a2 | a3;
		CHECK(sbf_splitter(d1, splitter_type::upper) == (a1 | a2));
	}

	TEST_CASE("DNF_clause_deletion2") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(false, 1);
		auto a2 = bdd_handle<Bool>::bit(false, 2);
		auto a3 = bdd_handle<Bool>::bit(false, 3);

		auto d1 = a1 | a2 | a3;
		CHECK(sbf_splitter(d1, splitter_type::upper) == (a1 | a2));
	}

	TEST_CASE("single_DNF_clause") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(true, 1);
		auto a2 = bdd_handle<Bool>::bit(true, 2);
		auto a3 = bdd_handle<Bool>::bit(true, 3);
		auto a4 = bdd_handle<Bool>::bit(true, 4);

		auto d1 = a1 & a2 & a3;
		CHECK(sbf_splitter(d1, splitter_type::upper) == (d1 & a4));
	}

	TEST_CASE("splitter strategies produce proper refinements") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(true, 1);
		auto a2 = bdd_handle<Bool>::bit(true, 2);
		auto a3 = bdd_handle<Bool>::bit(true, 3);
		auto d1 = a1 | a2 | a3;
		auto lower = sbf_splitter(d1, splitter_type::lower);
		auto middle = sbf_splitter(d1, splitter_type::middle);
		auto upper = sbf_splitter(d1, splitter_type::upper);
		CHECK(lower != d1);
		CHECK(middle != d1);
		CHECK(upper == (a1 | a2));
		CHECK((lower & ~d1) == false);
		CHECK((middle & ~d1) == false);
		CHECK((upper & ~d1) == false);
	}

	TEST_CASE("bad splitter adds a fresh clause on unsplittable input") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(true, 1);
		auto a2 = bdd_handle<Bool>::bit(true, 2);
		auto bad = sbf_splitter(a1 & a2, splitter_type::bad);
		CHECK(bad != (a1 & a2));
		CHECK((bad & ~(a1 & a2)) == false);
		CHECK(((a1 & a2) & ~bad) != false);
	}
}
