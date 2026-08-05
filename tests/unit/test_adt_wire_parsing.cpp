// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "adt_parser.generated.h"

static bool wire_parses(const std::string& s) {
	auto r = adt_parser::instance().parse(s.c_str(), s.size());
	return r.found;
}

TEST_SUITE("adt wire format") {
	TEST_CASE("flat")        { CHECK(wire_parses("{ a: \"x|y'\", b: \"#b00001010\" }")); }
	TEST_CASE("nested")      { CHECK(wire_parses("{ tag: \"1\", p: { a: \"0\", b: \"1\" }, q: { a: \"1\", b: \"0\" } }")); }
	TEST_CASE("quoted keys") { CHECK(wire_parses("{ \"a\": \"0\" }")); }
	TEST_CASE("no unquoted leaf") { CHECK_FALSE(wire_parses("{ a: 0 }")); }
	TEST_CASE("no bare value")    { CHECK_FALSE(wire_parses("\"0\"")); }
}
