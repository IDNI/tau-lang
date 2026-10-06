// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "backends/bdds/bdd_handle.h"
#include "boolean_algebras/sbf/sbf_ba.h"

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

namespace {

using hb = bdd_handle<Bool>;

// f with the constants of the partial assignment m substituted
hbdd<Bool> under(const hbdd<Bool>& f, const std::map<int_t, Bool>& m) {
	std::map<int_t, hbdd<Bool>> c;
	for (const auto& [v, b] : m) c.emplace(v, b == true ? hb::htrue : hb::hfalse);
	return f->compose(c);
}

// The function over variables 1..3 whose truth table is the bits of t
hbdd<Bool> from_truth_table(unsigned t) {
	hbdd<Bool> f = hb::hfalse;
	for (unsigned row = 0; row < 8; ++row) {
		if (((t >> row) & 1u) == 0) continue;
		hbdd<Bool> c = hb::htrue;
		for (uint_t v = 1; v <= 3; ++v)
			c = c & hb::bit(((row >> (v - 1)) & 1u) != 0, v);
		f = f | c;
	}
	return f;
}

} // namespace

TEST_SUITE("get_one_zero") {
	TEST_CASE("a node whose low child is one is left by its high branch") {
		bdd_init<Bool>();
		// v1 ? v2 : 1
		auto f = (hb::bit(true, 1) & hb::bit(true, 2)) | hb::bit(false, 1);
		auto z = f->get_one_zero();
		REQUIRE(z.has_value());
		CHECK(under(f, z.value()) == false);
	}

	TEST_CASE("every function over three variables but one has a zero witness") {
		bdd_init<Bool>();
		for (unsigned t = 0; t < 255; ++t) {
			CAPTURE(t);
			auto f = from_truth_table(t);
			auto z = f->get_one_zero();
			REQUIRE(z.has_value());
			CHECK(under(f, z.value()) == false);
		}
		CHECK_FALSE(from_truth_table(255)->get_one_zero().has_value());
	}

	TEST_CASE("lgrs parametrizes only zeros") {
		bdd_init<Bool>();
		for (unsigned t = 0; t < 255; ++t) {
			CAPTURE(t);
			auto f = from_truth_table(t);
			auto s = f->lgrs();
			REQUIRE(s.has_value());
			CHECK(f->compose(s.value()) == false);
		}
	}
}

TEST_SUITE("Bool engine members") {
	TEST_CASE("eliminations and evaluation over the two leaves") {
		bdd_init<Bool>();
		auto a = hb::bit(true, 1), b = hb::bit(true, 2);
		auto f = a & ~b;
		CHECK(hb::htrue->get_uelim() == true);
		CHECK(hb::hfalse->get_uelim() == false);
		CHECK(f->get_uelim() == false);
		CHECK(hb::htrue->get_eelim() == true);
		CHECK(hb::hfalse->get_eelim() == false);
		CHECK(f->get_eelim() == true);
		CHECK((a | ~a)->get_uelim() == true);
		for (unsigned row = 0; row < 4; ++row) {
			std::map<int_t, Bool> m{ { 1, Bool((row & 1u) != 0) },
				{ 2, Bool((row & 2u) != 0) } };
			CHECK(f->eval(m) == (row == 1));
		}
	}
}
