// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

#include "ba_types_inference.h"

using namespace idni::tau_lang;

TEST_SUITE("Configuration") {
	TEST_CASE("logging") {
		logging::trace();
	}
}

TEST_SUITE("merge_ba_types") {

	using type_t = size_t;

	bool match(const type_t& type1, const type_t& type2, const type_t& result) {
		auto merged = merge_ba_types<node_t>(type1, type2);
		if (!merged) return false;
		return result == merged.value();
	}

	bool nomatch(const type_t& type1, const type_t& type2) {
		auto merged = merge_ba_types<node_t>(type1, type2);
		return !merged.has_value();
	}

	TEST_CASE("untyped with untyped") {
		CHECK(match(0, 0, 0));
	}

	TEST_CASE("untyped with any type") {
		CHECK(match(0, 1, 1));
		CHECK(match(1, 0, 1));
	}

	TEST_CASE("same types all possible subtypes") {
		CHECK(match(1, 1, 1));
	}

	TEST_CASE("different types") {
		CHECK(nomatch(1, 2));
	}
}
