// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <cvc5/cvc5.h>

#include "test_init.h"
#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/variant_ba.h"

TEST_SUITE("variant Boolen algebra") {

	TEST_CASE("variant Bool == Bool") {
		CHECK( variant_ba<cvc5::Term, Bool>(Bool(true)) == true );
		CHECK( variant_ba<cvc5::Term, Bool>(Bool(false)) == false );
		CHECK( variant_ba<cvc5::Term, Bool>(Bool(true)) != false );
		CHECK( variant_ba<cvc5::Term, Bool>(Bool(false)) != true );
	}

	TEST_CASE("operator&") {
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(true)) & variant_ba<cvc5::Term, Bool>(Bool(true))) == true );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(true)) & variant_ba<cvc5::Term, Bool>(Bool(false))) == false );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(false)) & variant_ba<cvc5::Term, Bool>(Bool(true))) == false );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(false)) & variant_ba<cvc5::Term, Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator|") {
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(true)) | variant_ba<cvc5::Term, Bool>(Bool(true))) == true );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(true)) | variant_ba<cvc5::Term, Bool>(Bool(false))) == true );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(false)) | variant_ba<cvc5::Term, Bool>(Bool(true))) == true );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(false)) | variant_ba<cvc5::Term, Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator^") {
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(true)) ^ variant_ba<cvc5::Term, Bool>(Bool(true))) == false );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(true)) ^ variant_ba<cvc5::Term, Bool>(Bool(false))) == true );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(false)) ^ variant_ba<cvc5::Term, Bool>(Bool(true))) == true );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(false)) ^ variant_ba<cvc5::Term, Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator+") {
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(true)) + variant_ba<cvc5::Term, Bool>(Bool(true))) == false );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(true)) + variant_ba<cvc5::Term, Bool>(Bool(false))) == true );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(false)) + variant_ba<cvc5::Term, Bool>(Bool(true))) == true );
		CHECK( (variant_ba<cvc5::Term, Bool>(Bool(false)) + variant_ba<cvc5::Term, Bool>(Bool(false))) == false );
	}

	TEST_CASE("operator~") {
		CHECK( ~variant_ba<cvc5::Term, Bool>(Bool(true)) == false );
		CHECK( ~variant_ba<cvc5::Term, Bool>(Bool(false)) == true );
	}
}