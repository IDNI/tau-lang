// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/variant_ba.h"

namespace testing = doctest;

using namespace idni::tau_lang;

TEST_SUITE("variant Boolen algebra") {

	TEST_CASE("variant Bool == Bool") {
		CHECK( variant_ba<Bool>(Bool(true)) == true );
		CHECK( variant_ba<Bool>(Bool(false)) == false );
		CHECK( variant_ba<Bool>(Bool(true)) != false );
		CHECK( variant_ba<Bool>(Bool(false)) != true );
	}

	TEST_CASE("operator&") {
		CHECK( (variant_ba<Bool>(Bool(true)) & variant_ba<Bool>(Bool(true))) == true );
		CHECK( (variant_ba<Bool>(Bool(true)) & variant_ba<Bool>(Bool(false))) == false );
		CHECK( (variant_ba<Bool>(Bool(false)) & variant_ba<Bool>(Bool(true))) == false );
		CHECK( (variant_ba<Bool>(Bool(false)) & variant_ba<Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator|") {
		CHECK( (variant_ba<Bool>(Bool(true)) | variant_ba<Bool>(Bool(true))) == true );
		CHECK( (variant_ba<Bool>(Bool(true)) | variant_ba<Bool>(Bool(false))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) | variant_ba<Bool>(Bool(true))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) | variant_ba<Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator^") {
		CHECK( (variant_ba<Bool>(Bool(true)) ^ variant_ba<Bool>(Bool(true))) == false );
		CHECK( (variant_ba<Bool>(Bool(true)) ^ variant_ba<Bool>(Bool(false))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) ^ variant_ba<Bool>(Bool(true))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) ^ variant_ba<Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator+") {
		CHECK( (variant_ba<Bool>(Bool(true)) + variant_ba<Bool>(Bool(true))) == false );
		CHECK( (variant_ba<Bool>(Bool(true)) + variant_ba<Bool>(Bool(false))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) + variant_ba<Bool>(Bool(true))) == true );
		CHECK( (variant_ba<Bool>(Bool(false)) + variant_ba<Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator~") {
		CHECK( ~variant_ba<Bool>(Bool(true)) == false );
		CHECK( ~variant_ba<Bool>(Bool(false)) == true );
	}
}