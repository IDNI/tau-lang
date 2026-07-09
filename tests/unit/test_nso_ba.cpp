// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// BA-3: dedicated unit tests for nso_ba (boolean_algebras/nso_ba.h), the BA
// operators (&, |, ~, ^, +, is_zero/is_one, ==, !=, <=>, <, <=, >, >=) defined
// directly on tree<node<BAs...>> so that trees themselves satisfy the BA
// interface expected by the solver infrastructure.

#include "test_init.h"
#include "test_Bool_helpers.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("nso_ba bf operators") {

	TEST_CASE("and/or/xor/neg dispatch to the bf builders") {
		auto pbf = parse_bf();
		tref x = tau::get("X", pbf);
		tref y = tau::get("Y", pbf);
		REQUIRE(x != nullptr);
		REQUIRE(y != nullptr);

		CHECK( (tau::get(x) & tau::get(y)) == tau::get(tau::build_bf_and(x, y)) );
		CHECK( (tau::get(x) | tau::get(y)) == tau::get(tau::build_bf_or(x, y)) );
		CHECK( (tau::get(x) ^ tau::get(y)) == tau::get(tau::build_bf_xor(x, y)) );
		// operator+ is a synonym for operator^
		CHECK( (tau::get(x) + tau::get(y)) == tau::get(tau::build_bf_xor(x, y)) );
		CHECK( (~tau::get(x)) == tau::get(tau::build_bf_neg(x)) );
	}

	TEST_CASE("zero/one absorption and identity") {
		auto pbf = parse_bf();
		tref x    = tau::get("X", pbf);
		tref zero = tau::get("0", pbf);
		tref one  = tau::get("1", pbf);
		REQUIRE(x != nullptr);
		REQUIRE(zero != nullptr);
		REQUIRE(one != nullptr);

		CHECK( (tau::get(x) & tau::get(zero)) == tau::get(zero) );
		CHECK( (tau::get(x) & tau::get(one))  == tau::get(x) );
		CHECK( (tau::get(x) | tau::get(zero)) == tau::get(x) );
		CHECK( (tau::get(x) | tau::get(one))  == tau::get(one) );
		CHECK( (tau::get(x) ^ tau::get(zero)) == tau::get(x) );
		CHECK( (~tau::get(zero)) == tau::get(one) );
		CHECK( (~tau::get(one))  == tau::get(zero) );
	}

	TEST_CASE("is_zero/is_one predicates") {
		auto pbf = parse_bf();
		tref x    = tau::get("X", pbf);
		tref zero = tau::get("0", pbf);
		tref one  = tau::get("1", pbf);

		CHECK( is_zero(tau::get(zero)) );
		CHECK_FALSE( is_one(tau::get(zero)) );
		CHECK( is_one(tau::get(one)) );
		CHECK_FALSE( is_zero(tau::get(one)) );
		// a bare variable is neither the syntactic zero nor one
		CHECK_FALSE( is_zero(tau::get(x)) );
		CHECK_FALSE( is_one(tau::get(x)) );
	}

	TEST_CASE("equality/inequality and bf == bool / bool == bf") {
		auto pbf = parse_bf();
		tref x     = tau::get("X", pbf);
		tref x_dup = tau::get("X", pbf);
		tref y     = tau::get("Y", pbf);
		tref zero  = tau::get("0", pbf);
		tref one   = tau::get("1", pbf);

		CHECK( tau::get(x) == tau::get(x_dup) );
		CHECK( tau::get(x) != tau::get(y) );

		CHECK( tau::get(zero) == false );
		CHECK( false == tau::get(zero) );
		CHECK( tau::get(one) == true );
		CHECK( true == tau::get(one) );
	}

	TEST_CASE("ordering operators are consistent with operator<=>") {
		auto pbf = parse_bf();
		tau lt = tau::get(tau::get("X", pbf));
		tau rt = tau::get(tau::get("Y", pbf));

		CHECK( (lt <=> lt) == std::weak_ordering::equivalent );
		auto cmp = lt <=> rt;
		CHECK( (lt < rt)  == (cmp < 0) );
		CHECK( (lt <= rt) == (cmp <= 0) );
		CHECK( (lt > rt)  == (cmp > 0) );
		CHECK( (lt >= rt) == (cmp >= 0) );
		// exactly one direction should hold since lt != rt
		CHECK( (lt < rt) != (lt > rt) );
	}
}

TEST_SUITE("nso_ba wff operators") {

	TEST_CASE("and/or/xor/neg on wff T/F constants") {
		tref t = tau::_T();
		tref f = tau::_F();

		CHECK( (tau::get(t) & tau::get(f)) == tau::get(f) );
		CHECK( (tau::get(t) | tau::get(f)) == tau::get(t) );
		CHECK( (tau::get(t) ^ tau::get(f)) == tau::get(t) );
		CHECK( (tau::get(t) ^ tau::get(t)) == tau::get(f) );
		CHECK( (~tau::get(t)) == tau::get(f) );
		CHECK( (~tau::get(f)) == tau::get(t) );

		CHECK( is_one(tau::get(t)) );
		CHECK( is_zero(tau::get(f)) );
		CHECK( tau::get(t) == true );
		CHECK( tau::get(f) == false );
	}

	TEST_CASE("mixing bf and wff operands throws (nso_ba wrong types)") {
		auto pbf = parse_bf();
		tref x = tau::get("X", pbf);
		tref t = tau::_T();

		CHECK_THROWS_AS( tau::get(x) & tau::get(t), std::logic_error );
		CHECK_THROWS_AS( tau::get(x) | tau::get(t), std::logic_error );
		CHECK_THROWS_AS( tau::get(x) ^ tau::get(t), std::logic_error );
	}
}
