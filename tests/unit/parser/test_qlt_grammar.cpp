// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "../parser/qlt_parser.generated.h"

using namespace idni::tau_lang;
using namespace qlt_parser_data;

// Minimal qlt parser instance for testing
struct qlt_parser_instance : public idni::parser<char, char> {
	static qlt_parser_instance& instance() {
		static qlt_parser_instance inst;
		return inst;
	}

	qlt_parser_instance() : idni::parser<char, char>(
		qlt_parser_data::grammar,
		qlt_parser_data::parser_options) {}

	size_t id(const std::basic_string<char>& name) {
		return qlt_parser_data::nts.get(name);
	}

	const std::basic_string<char>& name(size_t id) {
		return qlt_parser_data::nts.get(id);
	}
};

TEST_SUITE("qlt grammar parsing") {

// ===========================================================================
// Top-level constants
// ===========================================================================

TEST_CASE("qlt: parse 'top' constant") {
	auto result = qlt_parser_instance::instance().parse("top", 3);
	CHECK(result.found);
}

TEST_CASE("qlt: parse 'bot' constant") {
	auto result = qlt_parser_instance::instance().parse("bot", 3);
	CHECK(result.found);
}

TEST_CASE("qlt: parse 'bottom' constant") {
	auto result = qlt_parser_instance::instance().parse("bottom", 6);
	CHECK(result.found);
}

// ===========================================================================
// Singleton notation (bare rational or identifier -> [r, r])
// ===========================================================================

TEST_CASE("qlt: parse singleton integer '0'") {
	auto result = qlt_parser_instance::instance().parse("0", 1);
	CHECK(result.found);
}

TEST_CASE("qlt: parse singleton integer '5'") {
	auto result = qlt_parser_instance::instance().parse("5", 1);
	CHECK(result.found);
}

TEST_CASE("qlt: parse singleton negative integer '-3'") {
	auto result = qlt_parser_instance::instance().parse("-3", 2);
	CHECK(result.found);
}

TEST_CASE("qlt: parse singleton fraction '1/3'") {
	auto result = qlt_parser_instance::instance().parse("1/3", 3);
	CHECK(result.found);
}

TEST_CASE("qlt: parse singleton decimal '0.5'") {
	auto result = qlt_parser_instance::instance().parse("0.5", 3);
	CHECK(result.found);
}

TEST_CASE("qlt: parse singleton identifier 'a'") {
	auto result = qlt_parser_instance::instance().parse("a", 1);
	CHECK(result.found);
}

TEST_CASE("qlt: parse singleton identifier 'x_1'") {
	auto result = qlt_parser_instance::instance().parse("x_1", 3);
	CHECK(result.found);
}

TEST_CASE("qlt: parse singleton identifier 'foo'") {
	auto result = qlt_parser_instance::instance().parse("foo", 3);
	CHECK(result.found);
}

// ===========================================================================
// Closed-closed intervals [a, b]
// ===========================================================================

TEST_CASE("qlt: parse closed-closed interval [0, 1]") {
	auto result = qlt_parser_instance::instance().parse("[0, 1]", 6);
	CHECK(result.found);
}

TEST_CASE("qlt: parse closed-closed singleton [3, 3]") {
	auto result = qlt_parser_instance::instance().parse("[3, 3]", 6);
	CHECK(result.found);
}

TEST_CASE("qlt: parse closed-closed interval with fractions [1/3, 2/3]") {
	auto result = qlt_parser_instance::instance().parse("[1/3, 2/3]", 10);
	CHECK(result.found);
}

TEST_CASE("qlt: parse closed-closed interval with decimals [0.1, 0.9]") {
	auto result = qlt_parser_instance::instance().parse("[0.1, 0.9]", 10);
	CHECK(result.found);
}

TEST_CASE("qlt: parse closed-closed interval with negatives [-2, -1]") {
	auto result = qlt_parser_instance::instance().parse("[-2, -1]", 8);
	CHECK(result.found);
}

// ===========================================================================
// Closed-open intervals [a, b)
// ===========================================================================

TEST_CASE("qlt: parse closed-open interval [0, 1)") {
	auto result = qlt_parser_instance::instance().parse("[0, 1)", 6);
	CHECK(result.found);
}

TEST_CASE("qlt: parse closed-open interval [1, 2)") {
	auto result = qlt_parser_instance::instance().parse("[1, 2)", 6);
	CHECK(result.found);
}

TEST_CASE("qlt: parse closed-open interval with fraction [1/4, 3/4)") {
	auto result = qlt_parser_instance::instance().parse("[1/4, 3/4)", 10);
	CHECK(result.found);
}

TEST_CASE("qlt: parse closed-open interval with decimal [0.25, 0.75)") {
	auto result = qlt_parser_instance::instance().parse("[0.25, 0.75)", 12);
	CHECK(result.found);
}

// ===========================================================================
// Open-closed intervals (a, b]
// ===========================================================================

TEST_CASE("qlt: parse open-closed interval (0, 1]") {
	auto result = qlt_parser_instance::instance().parse("(0, 1]", 6);
	CHECK(result.found);
}

TEST_CASE("qlt: parse open-closed interval (-1, 0]") {
	auto result = qlt_parser_instance::instance().parse("(-1, 0]", 7);
	CHECK(result.found);
}

TEST_CASE("qlt: parse open-closed interval with fraction (1/4, 3/4]") {
	auto result = qlt_parser_instance::instance().parse("(1/4, 3/4]", 10);
	CHECK(result.found);
}

// ===========================================================================
// Open-open intervals (a, b)
// ===========================================================================

TEST_CASE("qlt: parse open-open interval (0, 1)") {
	auto result = qlt_parser_instance::instance().parse("(0, 1)", 6);
	CHECK(result.found);
}

TEST_CASE("qlt: parse open-open interval (-1, 1)") {
	auto result = qlt_parser_instance::instance().parse("(-1, 1)", 7);
	CHECK(result.found);
}

TEST_CASE("qlt: parse open-open interval with fraction (1/3, 2/3)") {
	auto result = qlt_parser_instance::instance().parse("(1/3, 2/3)", 10);
	CHECK(result.found);
}

// ===========================================================================
// Intervals with infinity endpoints
// ===========================================================================

TEST_CASE("qlt: parse (-inf, 0)") {
	auto result = qlt_parser_instance::instance().parse("(-inf, 0)", 9);
	CHECK(result.found);
}

TEST_CASE("qlt: parse (0, +inf)") {
	auto result = qlt_parser_instance::instance().parse("(0, +inf)", 9);
	CHECK(result.found);
}

TEST_CASE("qlt: parse (-inf, +inf)") {
	auto result = qlt_parser_instance::instance().parse("(-inf, +inf)", 12);
	CHECK(result.found);
}

TEST_CASE("qlt: parse [-inf, 0)") {
	auto result = qlt_parser_instance::instance().parse("[-inf, 0)", 9);
	CHECK(result.found);
}

TEST_CASE("qlt: parse (0, inf)") {
	auto result = qlt_parser_instance::instance().parse("(0, inf)", 8);
	CHECK(result.found);
}

// ===========================================================================
// Intervals with symbolic (named constant) endpoints
// ===========================================================================

TEST_CASE("qlt: parse interval with symbolic endpoint [a, b)") {
	auto result = qlt_parser_instance::instance().parse("[a, b)", 6);
	CHECK(result.found);
}

TEST_CASE("qlt: parse closed-closed symbolic singleton [c, c]") {
	auto result = qlt_parser_instance::instance().parse("[c, c]", 6);
	CHECK(result.found);
}

TEST_CASE("qlt: parse open interval with named constants (lo, hi)") {
	auto result = qlt_parser_instance::instance().parse("(lo, hi)", 8);
	CHECK(result.found);
}

// ===========================================================================
// Union of intervals
// ===========================================================================

TEST_CASE("qlt: parse union [0, 1) | [2, 3)") {
	auto result = qlt_parser_instance::instance()
		.parse("[0, 1) | [2, 3)", 15);
	CHECK(result.found);
}

TEST_CASE("qlt: parse union (-1, 0) | (1, 2)") {
	auto result = qlt_parser_instance::instance()
		.parse("(-1, 0) | (1, 2)", 16);
	CHECK(result.found);
}

TEST_CASE("qlt: parse union of mixed bracket types [0, 1) | (2, 3]") {
	auto result = qlt_parser_instance::instance()
		.parse("[0, 1) | (2, 3]", 15);
	CHECK(result.found);
}

TEST_CASE("qlt: parse union of three intervals") {
	auto result = qlt_parser_instance::instance()
		.parse("[0, 1) | [2, 3) | [4, 5)", 24);
	CHECK(result.found);
}

TEST_CASE("qlt: parse union with fractional intervals [1/4, 1/2) | [3/4, 1)") {
	auto result = qlt_parser_instance::instance()
		.parse("[1/4, 1/2) | [3/4, 1)", 21);
	CHECK(result.found);
}

// ===========================================================================
// Whitespace handling
// ===========================================================================

TEST_CASE("qlt: parse with leading/trailing whitespace") {
	auto result = qlt_parser_instance::instance()
		.parse("  [0, 1)  ", 10);
	CHECK(result.found);
}

TEST_CASE("qlt: parse interval with internal whitespace [ 0 , 1 )") {
	auto result = qlt_parser_instance::instance()
		.parse("[ 0 , 1 )", 9);
	CHECK(result.found);
}

TEST_CASE("qlt: parse union with extra whitespace around |") {
	auto result = qlt_parser_instance::instance()
		.parse("[0, 1)  |  [2, 3)", 17);
	CHECK(result.found);
}

TEST_CASE("qlt: parse top with surrounding whitespace") {
	auto result = qlt_parser_instance::instance()
		.parse("  top  ", 7);
	CHECK(result.found);
}

// ===========================================================================
// Integration with parse_qlt dispatcher
// ===========================================================================

TEST_CASE("qlt: dispatcher parse_qlt with top") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{top}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.is_full());
}

TEST_CASE("qlt: dispatcher parse_qlt with bottom") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{bot}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.is_empty());
}

TEST_CASE("qlt: dispatcher parse_qlt with bottom keyword") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{bottom}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.is_empty());
}

TEST_CASE("qlt: dispatcher parse_qlt closed-open interval") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[0, 1)}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK_FALSE(val.is_full());
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.bound == qlt_bound::CLOSED);
	CHECK(val.pieces[0].hi.bound == qlt_bound::OPEN);
}

TEST_CASE("qlt: dispatcher parse_qlt open-closed interval") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{(0, 1]}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.bound == qlt_bound::OPEN);
	CHECK(val.pieces[0].hi.bound == qlt_bound::CLOSED);
}

TEST_CASE("qlt: dispatcher parse_qlt closed-closed interval") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[0, 1]}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.bound == qlt_bound::CLOSED);
	CHECK(val.pieces[0].hi.bound == qlt_bound::CLOSED);
}

TEST_CASE("qlt: dispatcher parse_qlt open-open interval") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{(0, 1)}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.bound == qlt_bound::OPEN);
	CHECK(val.pieces[0].hi.bound == qlt_bound::OPEN);
}

TEST_CASE("qlt: dispatcher parse_qlt singleton integer") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{3}");
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
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{1/3}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.bound == qlt_bound::CLOSED);
	CHECK(val.pieces[0].hi.bound == qlt_bound::CLOSED);
}

TEST_CASE("qlt: dispatcher parse_qlt singleton identifier") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{myconst}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.pieces.size() == 1);
	CHECK(val.pieces[0].lo.val.is_sym());
	CHECK(val.pieces[0].lo.val.sym == "myconst");
}

TEST_CASE("qlt: dispatcher parse_qlt union two intervals") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>(
		"{[0, 1) | [2, 3)}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK(val.pieces.size() == 2);
}

TEST_CASE("qlt: dispatcher parse_qlt without braces") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("[0, 1)");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.pieces.size() == 1);
}

TEST_CASE("qlt: dispatcher parse_qlt non-dyadic fraction 1/3") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[1/3, 2/3)}");
	// qlt accepts any rational denominator (unlike qint)
	CHECK(result.has_value());
}

TEST_CASE("qlt: dispatcher parse_qlt fractional interval [1/3, 2/3]") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[1/3, 2/3]}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.pieces.size() == 1);
}

TEST_CASE("qlt: dispatcher parse_qlt decimal interval [0.1, 0.9)") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[0.1, 0.9)}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.pieces.size() == 1);
}

TEST_CASE("qlt: dispatcher parse_qlt with infinity (-inf, +inf)") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{(-inf, +inf)}");
	CHECK(result.has_value());
	auto val = std::get<qlt>(result->first);
	CHECK(val.is_full());
}

TEST_CASE("qlt: dispatcher parse_qlt symbolic closed-closed [a, b]") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[a, b]}");
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
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[1, 0)}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qlt: reject open singleton (3, 3) — empty interval") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{(3, 3)}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qlt: reject half-open singleton [3, 3)") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[3, 3)}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qlt: reject bare +inf as singleton") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{+inf}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qlt: reject bare -inf as singleton") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{-inf}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qlt: reject malformed interval (missing closing bracket)") {
	auto result = qlt_parser_instance::instance().parse("[0, 1", 5);
	CHECK_FALSE(result.found);
}

TEST_CASE("qlt: reject malformed interval (missing comma)") {
	auto result = qlt_parser_instance::instance().parse("[0 1)", 5);
	CHECK_FALSE(result.found);
}

TEST_CASE("qlt: reject empty string") {
	auto result = parse_qlt<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{}");
	CHECK_FALSE(result.has_value());
}

} // TEST_SUITE
