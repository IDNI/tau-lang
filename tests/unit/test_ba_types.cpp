// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

#include "ba_types.h"

TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}
}

TEST_SUITE("assumptions") {

	/*TEST_CASE("nat_type_id is 0") {
		// Assumed in some parts of the code
		CHECK( idni::tau_lang::nat_type_id<node_t>() == 0 );
	}*/

	TEST_CASE("untyped_type_id is 0") {
		// Assumed in some parts of the code
		CHECK( idni::tau_lang::untyped_type_id<node_t>() == 0 );
	}

}

TEST_SUITE("unify") {

	using type_t = size_t;

	bool match(const type_t& type1, const type_t& type2, const type_t& result) {
		auto merged = unify<node_t>(type1, type2);
		return result == merged.value();
	}

	bool nomatch(const type_t& type1, const type_t& type2) {
		auto merged = unify<node_t>(type1, type2);
		return !merged;
	}

	TEST_CASE("untyped with untyped") {
		CHECK(match(0, 0, 0));
	}

	TEST_CASE("untyped with any type") {
		CHECK(match(0, 1, 1));
		CHECK(match(1, 0, 1));
	}

	TEST_CASE("same types") {
		CHECK(match(2, 2, 2));
	}

	TEST_CASE("different types") {
		CHECK(nomatch(2, 3));
	}
}
