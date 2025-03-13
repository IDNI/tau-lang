// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "boolean_algebras/bool_ba.h"

namespace testing = doctest;

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
}
