// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "boolean_algebras/bdds/var_dict.h"

using namespace idni::tau_lang;

TEST_SUITE("var_dict") {

	TEST_CASE("round trip: name to id and back") {
		sym_t id = var_dict("BA-5-round-trip");
		CHECK(var_dict(id) == "BA-5-round-trip");
	}

	// BA-5: var_dict(sym_t) used to return a const char* into a
	// function-local static buffer for auto-generated names (ids past the
	// end of the dict), so a second such call overwrote the first call's
	// buffer before it was read. Returning std::string by value fixes
	// this; two consecutive auto-generated-name lookups must both remain
	// valid and distinct.
	TEST_CASE("two consecutive auto-generated names do not clobber each other") {
		sym_t next = var_dict("BA-5-next-marker") + 1;
		std::string first = var_dict(next);
		std::string second = var_dict(next + 1);
		CHECK(first != second);
		CHECK(var_dict(next) == first);
		CHECK(var_dict(next + 1) == second);
	}
}
