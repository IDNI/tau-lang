// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/variant_ba.h"

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