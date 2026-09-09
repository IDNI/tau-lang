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
	TEST_CASE("empty leaf")       { CHECK(wire_parses("{ a: \"\" }")); }
	TEST_CASE("leaf with spaces and punctuation") {
		CHECK(wire_parses("{ a: \"x | {y}: z, w.\" }"));
	}
	TEST_CASE("zero whitespace")  { CHECK(wire_parses("{a:\"0\",p:{x:\"1\"}}")); }
	TEST_CASE("empty object rejected") { CHECK_FALSE(wire_parses("{ }")); }
	// Phase-2 backlog (wire-format escaping): a double quote can never appear
	// inside a leaf -- value_chars is (printable & ~'"')* and there is no
	// escape production (parser/adt.tgf). Pin the current rejection so adding
	// escaping later consciously updates this.
	TEST_CASE("quote inside a leaf is unrepresentable") {
		CHECK_FALSE(wire_parses("{ a: \"x\\\"y\" }"));
	}
	TEST_CASE("digit-leading key rejected") { CHECK_FALSE(wire_parses("{ 1a: \"0\" }")); }
}
