// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "backends/bdds/var_dict.h"

using namespace idni::tau_lang;

TEST_SUITE("var_dict") {

	TEST_CASE("round trip: name to id and back") {
		sym_t id = var_dict("BA-5-round-trip");
		auto name = var_dict(id);
		REQUIRE(name.has_value());
		CHECK(name.value() == "BA-5-round-trip");
	}

	// BA-5: var_dict(sym_t) used to return a const char* into a
	// function-local static buffer for auto-generated names (ids past the
	// end of the dict), so a second such call overwrote the first call's
	// buffer before it was read. Returning std::string by value fixes
	// this; two consecutive auto-generated-name lookups must both remain
	// valid and distinct.
	TEST_CASE("two consecutive auto-generated names do not clobber each other") {
		sym_t next = var_dict("BA-5-next-marker") + 1;
		auto first_r = var_dict(next);
		auto second_r = var_dict(next + 1);
		REQUIRE(first_r.has_value());
		REQUIRE(second_r.has_value());
		std::string first = first_r.value();
		std::string second = second_r.value();
		CHECK(first != second);
		auto next_again = var_dict(next);
		auto next_plus_1_again = var_dict(next + 1);
		REQUIRE(next_again.has_value());
		REQUIRE(next_plus_1_again.has_value());
		CHECK(next_again.value() == first);
		CHECK(next_plus_1_again.value() == second);
	}
}
