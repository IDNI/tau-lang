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

	using type_t = tree<node_t>::type_t;

	bool match(const type_t& type1, const type_t& type2, const type_t& result) {
		auto merged = merge_ba_types<node_t>(type1, type2);
		if (!merged) return false;
		return (result.first == merged.value().first
			&& result.second == merged.value().second);
	}

	bool nomatch(const type_t& type1, const type_t& type2) {
		auto merged = merge_ba_types<node_t>(type1, type2);
		return !merged.has_value();
	}

	TEST_CASE("untyped with untyped") {
		CHECK(match({0, nullptr}, {0, nullptr}, {0, nullptr}));
		CHECK(match({0, (tref)1}, {0, nullptr}, {0, nullptr}));
		CHECK(match({0, nullptr}, {0, (tref)1}, {0, nullptr}));
	}

	TEST_CASE("untyped with any type") {
		CHECK(match({0, nullptr}, {1, nullptr}, {1, nullptr}));
		CHECK(match({0, nullptr}, {1, (tref)1}, {1, (tref)1}));
		CHECK(match({1, nullptr}, {0, nullptr}, {1, nullptr}));
		CHECK(match({1, (tref)1}, {0, nullptr}, {1, (tref)1}));
		CHECK(match({0, nullptr}, {0, nullptr}, {0, nullptr}));
	}

	TEST_CASE("same types all possible subtypes") {
		CHECK(match({1, nullptr}, {1, nullptr}, {1, nullptr}));
		CHECK(match({1, (tref)1}, {1, (tref)1}, {1, (tref)1}));
		CHECK(match({1, nullptr}, {1, (tref)1}, {1, (tref)1}));
		CHECK(match({1, (tref)1}, {1, nullptr}, {1, (tref)1}));
	}

	TEST_CASE("different types") {
		CHECK(nomatch({1, nullptr}, {2, nullptr}));
		CHECK(nomatch({1, (tref)1}, {1, (tref)2}));
	}
}
