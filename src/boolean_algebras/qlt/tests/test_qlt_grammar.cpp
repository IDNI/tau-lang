// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "tau_pack.h"
#include "test_tau_helpers.h"
#include "boolean_algebras/qlt/parser/qlt_parser.generated.h"
#include "test_grammar_helpers.h"

using namespace idni::tau_lang;
using namespace qlt_parser_data;

TEST_SUITE("qlt grammar parsing") {

// ===========================================================================
// Top-level constants
// ===========================================================================

TEST_CASE_TEMPLATE_INVOKE(grammar_constants, qlt_parser);

// ===========================================================================
// Singleton notation (bare rational or identifier -> [r, r])
// ===========================================================================

TEST_CASE("qlt: parse singleton integer '0'") { CHECK(grammar_parses<qlt_parser>("0")); }

TEST_CASE("qlt: parse singleton integer '5'") { CHECK(grammar_parses<qlt_parser>("5")); }

TEST_CASE("qlt: parse singleton negative integer '-3'") { CHECK(grammar_parses<qlt_parser>("-3")); }

TEST_CASE("qlt: parse singleton fraction '1/3'") { CHECK(grammar_parses<qlt_parser>("1/3")); }

TEST_CASE("qlt: parse singleton decimal '0.5'") { CHECK(grammar_parses<qlt_parser>("0.5")); }

TEST_CASE("qlt: parse singleton identifier 'a'") { CHECK(grammar_parses<qlt_parser>("a")); }

TEST_CASE("qlt: parse singleton identifier 'x_1'") { CHECK(grammar_parses<qlt_parser>("x_1")); }

TEST_CASE("qlt: parse singleton identifier 'foo'") { CHECK(grammar_parses<qlt_parser>("foo")); }

// ===========================================================================
// Closed-closed intervals [a, b]
// ===========================================================================

TEST_CASE("qlt: parse closed-closed interval [0, 1]") { CHECK(grammar_parses<qlt_parser>("[0, 1]")); }

TEST_CASE("qlt: parse closed-closed singleton [3, 3]") { CHECK(grammar_parses<qlt_parser>("[3, 3]")); }

TEST_CASE("qlt: parse closed-closed interval with fractions [1/3, 2/3]") { CHECK(grammar_parses<qlt_parser>("[1/3, 2/3]")); }

TEST_CASE("qlt: parse closed-closed interval with decimals [0.1, 0.9]") { CHECK(grammar_parses<qlt_parser>("[0.1, 0.9]")); }

TEST_CASE("qlt: parse closed-closed interval with negatives [-2, -1]") { CHECK(grammar_parses<qlt_parser>("[-2, -1]")); }

// ===========================================================================
// Closed-open intervals [a, b)
// ===========================================================================

TEST_CASE("qlt: parse closed-open interval [0, 1)") { CHECK(grammar_parses<qlt_parser>("[0, 1)")); }

TEST_CASE("qlt: parse closed-open interval [1, 2)") { CHECK(grammar_parses<qlt_parser>("[1, 2)")); }

TEST_CASE("qlt: parse closed-open interval with fraction [1/4, 3/4)") { CHECK(grammar_parses<qlt_parser>("[1/4, 3/4)")); }

TEST_CASE("qlt: parse closed-open interval with decimal [0.25, 0.75)") { CHECK(grammar_parses<qlt_parser>("[0.25, 0.75)")); }

// ===========================================================================
// Open-closed intervals (a, b]
// ===========================================================================

TEST_CASE("qlt: parse open-closed interval (0, 1]") { CHECK(grammar_parses<qlt_parser>("(0, 1]")); }

TEST_CASE("qlt: parse open-closed interval (-1, 0]") { CHECK(grammar_parses<qlt_parser>("(-1, 0]")); }

TEST_CASE("qlt: parse open-closed interval with fraction (1/4, 3/4]") { CHECK(grammar_parses<qlt_parser>("(1/4, 3/4]")); }

// ===========================================================================
// Open-open intervals (a, b)
// ===========================================================================

TEST_CASE("qlt: parse open-open interval (0, 1)") { CHECK(grammar_parses<qlt_parser>("(0, 1)")); }

TEST_CASE("qlt: parse open-open interval (-1, 1)") { CHECK(grammar_parses<qlt_parser>("(-1, 1)")); }

TEST_CASE("qlt: parse open-open interval with fraction (1/3, 2/3)") { CHECK(grammar_parses<qlt_parser>("(1/3, 2/3)")); }

// ===========================================================================
// Intervals with infinity endpoints
// ===========================================================================

TEST_CASE("qlt: parse (-inf, 0)") { CHECK(grammar_parses<qlt_parser>("(-inf, 0)")); }

TEST_CASE("qlt: parse (0, +inf)") { CHECK(grammar_parses<qlt_parser>("(0, +inf)")); }

TEST_CASE("qlt: parse (-inf, +inf)") { CHECK(grammar_parses<qlt_parser>("(-inf, +inf)")); }

TEST_CASE("qlt: parse [-inf, 0)") { CHECK(grammar_parses<qlt_parser>("[-inf, 0)")); }

TEST_CASE("qlt: parse (0, inf)") { CHECK(grammar_parses<qlt_parser>("(0, inf)")); }

// ===========================================================================
// Intervals with symbolic (named constant) endpoints
// ===========================================================================

TEST_CASE("qlt: parse interval with symbolic endpoint [a, b)") { CHECK(grammar_parses<qlt_parser>("[a, b)")); }

TEST_CASE("qlt: parse closed-closed symbolic singleton [c, c]") { CHECK(grammar_parses<qlt_parser>("[c, c]")); }

TEST_CASE("qlt: parse open interval with named constants (lo, hi)") { CHECK(grammar_parses<qlt_parser>("(lo, hi)")); }

// ===========================================================================
// Union of intervals
// ===========================================================================

TEST_CASE("qlt: parse union [0, 1) | [2, 3)") { CHECK(grammar_parses<qlt_parser>("[0, 1) | [2, 3)")); }

TEST_CASE("qlt: parse union (-1, 0) | (1, 2)") { CHECK(grammar_parses<qlt_parser>("(-1, 0) | (1, 2)")); }

TEST_CASE("qlt: parse union of mixed bracket types [0, 1) | (2, 3]") { CHECK(grammar_parses<qlt_parser>("[0, 1) | (2, 3]")); }

TEST_CASE("qlt: parse union of three intervals") { CHECK(grammar_parses<qlt_parser>("[0, 1) | [2, 3) | [4, 5)")); }

TEST_CASE("qlt: parse union with fractional intervals [1/4, 1/2) | [3/4, 1)") { CHECK(grammar_parses<qlt_parser>("[1/4, 1/2) | [3/4, 1)")); }

// ===========================================================================
// Whitespace handling
// ===========================================================================

TEST_CASE("qlt: parse with leading/trailing whitespace") { CHECK(grammar_parses<qlt_parser>("  [0, 1)  ")); }

TEST_CASE("qlt: parse interval with internal whitespace [ 0 , 1 )") { CHECK(grammar_parses<qlt_parser>("[ 0 , 1 )")); }

TEST_CASE("qlt: parse union with extra whitespace around |") { CHECK(grammar_parses<qlt_parser>("[0, 1)  |  [2, 3)")); }

TEST_CASE("qlt: parse top with surrounding whitespace") { CHECK(grammar_parses<qlt_parser>("  top  ")); }

// ===========================================================================
// Integration with parse_qlt dispatcher
// ===========================================================================

TEST_CASE("qlt: dispatcher parse_qlt with top") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{top}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.is_full());
}

TEST_CASE("qlt: dispatcher parse_qlt with bottom") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{bot}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.is_empty());
}

TEST_CASE("qlt: dispatcher parse_qlt with bottom keyword") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{bottom}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.is_empty());
}

TEST_CASE("qlt: dispatcher parse_qlt closed-open interval") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{[0, 1)}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK_FALSE(val.is_full());
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.bound == qlt_bound::CLOSED);
	CHECK(val.pieces[0].hi.bound == qlt_bound::OPEN);
}

TEST_CASE("qlt: dispatcher parse_qlt open-closed interval") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{(0, 1]}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.bound == qlt_bound::OPEN);
	CHECK(val.pieces[0].hi.bound == qlt_bound::CLOSED);
}

TEST_CASE("qlt: dispatcher parse_qlt closed-closed interval") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{[0, 1]}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.bound == qlt_bound::CLOSED);
	CHECK(val.pieces[0].hi.bound == qlt_bound::CLOSED);
}

TEST_CASE("qlt: dispatcher parse_qlt open-open interval") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{(0, 1)}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.bound == qlt_bound::OPEN);
	CHECK(val.pieces[0].hi.bound == qlt_bound::OPEN);
}

TEST_CASE("qlt: dispatcher parse_qlt singleton integer") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{3}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK(val.pieces.size() == 1);
	// singleton [3, 3]
	CHECK(val.pieces[0].lo.bound == qlt_bound::CLOSED);
	CHECK(val.pieces[0].hi.bound == qlt_bound::CLOSED);
	CHECK(val.pieces[0].lo.val == val.pieces[0].hi.val);
}

TEST_CASE("qlt: dispatcher parse_qlt singleton fraction") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{1/3}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.bound == qlt_bound::CLOSED);
	CHECK(val.pieces[0].hi.bound == qlt_bound::CLOSED);
}

TEST_CASE("qlt: dispatcher parse_qlt singleton identifier") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{myconst}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.val.is_sym());
	CHECK(val.pieces[0].lo.val.sym == "myconst");
}

TEST_CASE("qlt: dispatcher parse_qlt union two intervals") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>(
		"{[0, 1) | [2, 3)}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK(val.pieces.size() == 2);
}

TEST_CASE("qlt: dispatcher parse_qlt without braces") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("[0, 1)");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.pieces.size() == 1);
}

TEST_CASE("qlt: dispatcher parse_qlt non-dyadic fraction 1/3") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{[1/3, 2/3)}");
	// qlt accepts any rational denominator (unlike qint)
	CHECK(result.has_value());
}

TEST_CASE("qlt: dispatcher parse_qlt fractional interval [1/3, 2/3]") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{[1/3, 2/3]}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.pieces.size() == 1);
}

TEST_CASE("qlt: dispatcher parse_qlt decimal interval [0.1, 0.9)") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{[0.1, 0.9)}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.pieces.size() == 1);
}

TEST_CASE("qlt: dispatcher parse_qlt with infinity (-inf, +inf)") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{(-inf, +inf)}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.is_full());
}

TEST_CASE("qlt: a closed bracket at an infinity reads as open") {
	auto closed = parse_qlt<TAU_PACK_BASE_BAS>("{[-inf, 0]}");
	auto open = parse_qlt<TAU_PACK_BASE_BAS>("{(-inf, 0]}");
	REQUIRE(closed.has_value());
	REQUIRE(open.has_value());
	auto c = std::get<qlt>(closed->first), o = std::get<qlt>(open->first);
	CHECK(c == o);
	CHECK(qlt_hash(c) == qlt_hash(o));
	CHECK(c.to_string() == o.to_string());
	auto full = parse_qlt<TAU_PACK_BASE_BAS>("{[-inf, +inf]}");
	REQUIRE(full.has_value());
	CHECK(std::get<qlt>(full->first).is_full());
	CHECK(std::get<qlt>(full->first) == qlt::top());
}

TEST_CASE("qlt: an endpoint may carry a leading +") {
	auto plus = parse_qlt<TAU_PACK_BASE_BAS>("{[+5, +13/2]}");
	auto bare = parse_qlt<TAU_PACK_BASE_BAS>("{[5, 13/2]}");
	REQUIRE(plus.has_value());
	REQUIRE(bare.has_value());
	CHECK(std::get<qlt>(plus->first) == std::get<qlt>(bare->first));
	CHECK(parse_qlt<TAU_PACK_BASE_BAS>("{+5}").has_value());
	CHECK_FALSE(parse_qlt<TAU_PACK_BASE_BAS>("{[+-5, 6]}").has_value());
}

TEST_CASE("qlt: reject the interval [+inf, +inf]") {
	CHECK_FALSE(parse_qlt<TAU_PACK_BASE_BAS>("{[+inf, +inf]}").has_value());
}

TEST_CASE("qlt: dispatcher parse_qlt symbolic closed-closed [a, b]") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{[a, b]}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.val.is_sym());
	CHECK(val.pieces[0].hi.val.is_sym());
}

// ===========================================================================
// Error cases
// ===========================================================================

TEST_CASE("qlt: reject interval with lo > hi") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{[1, 0)}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qlt: reject open singleton (3, 3) — empty interval") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{(3, 3)}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qlt: reject half-open singleton [3, 3)") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{[3, 3)}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qlt: reject bare +inf as singleton") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{+inf}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qlt: reject bare -inf as singleton") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{-inf}");
	CHECK_FALSE(result.has_value());
}

// A literal that parses but has no value carries the reason as an error.
TEST_CASE("qlt: an invalid literal is an error of qlt_eval_parse_tree") {
	for (std::string src : { "[1, 0)", "(3, 3)", "+inf", "1/0",
		"[0, 1] | [1, 0)", "[0, 1/0]" })
	{
		CAPTURE(src);
		auto parsed = qlt_parser::instance().parse(src.c_str(), src.size());
		REQUIRE(parsed.found);
		auto t = qlt_parser::tree::traverser(parsed.get_shaped_tree2())
			| qlt_parser::qlt;
		REQUIRE(t.has_value());
		auto r = qlt_eval_parse_tree(t);
		CHECK_FALSE(r.has_value());
		CHECK(r.has_error());
	}
}

TEST_CASE("qlt: reject malformed interval (missing closing bracket)") { CHECK_FALSE(grammar_parses<qlt_parser>("[0, 1")); }

TEST_CASE("qlt: reject malformed interval (missing comma)") { CHECK_FALSE(grammar_parses<qlt_parser>("[0 1)")); }

TEST_CASE("qlt: reject empty string") {
	auto result = parse_qlt<TAU_PACK_BASE_BAS>("{}");
	CHECK_FALSE(result.has_value());
}

} // TEST_SUITE
