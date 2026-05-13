// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "boolean_algebras/bool_ba.h"

TEST_SUITE("bool boolean algebra") {

	TEST_CASE("Bool == bool") {
		CHECK( Bool(true) == true );
		CHECK( Bool(false) == false );
		CHECK( Bool(true) != false );
		CHECK( Bool(false) != true );
	}

	TEST_CASE("operator&") {
		CHECK( (Bool(true) & Bool(true)) == true );
		CHECK( (Bool(true) & Bool(false)) == false );
		CHECK( (Bool(false) & Bool(true)) == false );
		CHECK( (Bool(false) & Bool(false)) == false );
	}

	TEST_CASE("operator|") {
		CHECK( (Bool(true) | Bool(true)) == true );
		CHECK( (Bool(true) | Bool(false)) == true );
		CHECK( (Bool(false) | Bool(true)) == true );
		CHECK( (Bool(false) | Bool(false)) == false );
	}

	TEST_CASE("operator^") {
		CHECK( (Bool(true) ^ Bool(true)) == false );
		CHECK( (Bool(true) ^ Bool(false)) == true );
		CHECK( (Bool(false) ^ Bool(true)) == true );
		CHECK( (Bool(false) ^ Bool(false)) == false );
	}

	TEST_CASE("operator+") {
		CHECK( (Bool(true) + Bool(true)) == false );
		CHECK( (Bool(true) + Bool(false)) == true );
		CHECK( (Bool(false) + Bool(true)) == true );
		CHECK( (Bool(false) + Bool(false)) == false );
	}

	TEST_CASE("operator~") {
		CHECK( (~Bool(true)) == Bool(false) );
		CHECK( (~Bool(false)) == Bool(true) );
	}

	// BA-18: Bool::zero()/one(), std::hash<Bool> and normalize_bool were
	// untested (all operators and bool comparisons were already covered
	// above).

	TEST_CASE("zero/one static singletons") {
		CHECK( Bool::zero() == Bool(false) );
		CHECK( Bool::one() == Bool(true) );
		CHECK( Bool::zero().is_zero() );
		CHECK_FALSE( Bool::zero().is_one() );
		CHECK( Bool::one().is_one() );
		CHECK_FALSE( Bool::one().is_zero() );
		// zero()/one() return a reference to the same static singleton
		CHECK( &Bool::zero() == &Bool::zero() );
		CHECK( &Bool::one() == &Bool::one() );
	}

	TEST_CASE("std::hash<Bool>") {
		std::hash<Bool> h;
		CHECK( h(Bool(true)) == h(Bool(true)) );
		CHECK( h(Bool(false)) == h(Bool(false)) );
		CHECK( h(Bool(true)) != h(Bool(false)) );
	}

	TEST_CASE("normalize_bool is the identity") {
		CHECK( normalize_bool(Bool(true)) == Bool(true) );
		CHECK( normalize_bool(Bool(false)) == Bool(false) );
	}
}
