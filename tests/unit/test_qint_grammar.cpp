// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "../parser/qint_parser.generated.h"

using namespace idni::tau_lang;
using namespace qint_parser_data;

// Minimal qint parser instance for testing
struct qint_parser_instance : public idni::parser<char, char> {
	static qint_parser_instance& instance() {
		static qint_parser_instance inst;
		return inst;
	}

	qint_parser_instance() : idni::parser<char, char>(
		qint_parser_data::grammar,
		qint_parser_data::parser_options) {}

	size_t id(const std::basic_string<char>& name) {
		return qint_parser_data::nts.get(name);
	}

	const std::basic_string<char>& name(size_t id) {
		return qint_parser_data::nts.get(id);
	}
};

TEST_SUITE("qint grammar parsing") {

// ═══════════════════════════════════════════════════════════════════════════
// Top-level Constants
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse 'top' constant") {
	auto result = qint_parser_instance::instance()
		.parse("top", 3);
	CHECK(result.found);
}

TEST_CASE("qint: parse 'bot' constant") {
	auto result = qint_parser_instance::instance()
		.parse("bot", 3);
	CHECK(result.found);
}

TEST_CASE("qint: parse 'bottom' constant") {
	auto result = qint_parser_instance::instance()
		.parse("bottom", 6);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Integer Literals
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse integer literal '0'") {
	auto result = qint_parser_instance::instance()
		.parse("0", 1);
	CHECK(result.found);
}

TEST_CASE("qint: parse integer literal '1'") {
	auto result = qint_parser_instance::instance()
		.parse("1", 1);
	CHECK(result.found);
}

TEST_CASE("qint: parse integer literal '5'") {
	auto result = qint_parser_instance::instance()
		.parse("5", 1);
	CHECK(result.found);
}

TEST_CASE("qint: parse negative integer literal '-3'") {
	auto result = qint_parser_instance::instance()
		.parse("-3", 2);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Single Intervals with Integer Endpoints
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse single interval [0, 1)") {
	auto result = qint_parser_instance::instance()
		.parse("[0, 1)", 6);
	CHECK(result.found);
}

TEST_CASE("qint: parse single interval [1, 2)") {
	auto result = qint_parser_instance::instance()
		.parse("[1, 2)", 6);
	CHECK(result.found);
}

TEST_CASE("qint: parse single interval [-1, 0)") {
	auto result = qint_parser_instance::instance()
		.parse("[-1, 0)", 7);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Single Intervals with Fractional Endpoints
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse interval with dyadic fraction [1/4, 3/4)") {
	auto result = qint_parser_instance::instance()
		.parse("[1/4, 3/4)", 10);
	CHECK(result.found);
}

TEST_CASE("qint: parse interval with dyadic fraction [1/2, 1)") {
	auto result = qint_parser_instance::instance()
		.parse("[1/2, 1)", 8);
	CHECK(result.found);
}

TEST_CASE("qint: parse interval with decimal [0.25, 0.75)") {
	auto result = qint_parser_instance::instance()
		.parse("[0.25, 0.75)", 12);
	CHECK(result.found);
}

TEST_CASE("qint: parse interval with negative fraction [-3/4, -1/4)") {
	auto result = qint_parser_instance::instance()
		.parse("[-3/4, -1/4)", 12);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Intervals with Infinities
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse interval [-inf, 0)") {
	auto result = qint_parser_instance::instance()
		.parse("[-inf, 0)", 9);
	CHECK(result.found);
}

TEST_CASE("qint: parse interval [0, +inf)") {
	auto result = qint_parser_instance::instance()
		.parse("[0, +inf)", 9);
	CHECK(result.found);
}

TEST_CASE("qint: parse interval [-infinity, +infinity)") {
	auto result = qint_parser_instance::instance()
		.parse("[-infinity, +infinity)", 22);
	CHECK(result.found);
}

TEST_CASE("qint: parse interval [1/2, inf)") {
	auto result = qint_parser_instance::instance()
		.parse("[1/2, inf)", 10);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Union of Intervals
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse union [0, 1) | [2, 3)") {
	auto result = qint_parser_instance::instance()
		.parse("[0, 1) | [2, 3)", 15);
	CHECK(result.found);
}

TEST_CASE("qint: parse union [-1, 0) | [1, 2)") {
	auto result = qint_parser_instance::instance()
		.parse("[-1, 0) | [1, 2)", 16);
	CHECK(result.found);
}

TEST_CASE("qint: parse union with fractional intervals [1/4, 1/2) | [3/4, 1)") {
	auto result = qint_parser_instance::instance()
		.parse("[1/4, 1/2) | [3/4, 1)", 21);
	CHECK(result.found);
}

TEST_CASE("qint: parse union of three intervals") {
	auto result = qint_parser_instance::instance()
		.parse("[0, 1/3) | [1/2, 2/3) | [5/6, 1)", 32);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Whitespace Handling
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse with leading/trailing whitespace") {
	auto result = qint_parser_instance::instance()
		.parse("  [0, 1)  ", 10);
	CHECK(result.found);
}

TEST_CASE("qint: parse interval with internal whitespace [0, 1)") {
	auto result = qint_parser_instance::instance()
		.parse("[ 0 , 1 )", 9);
	CHECK(result.found);
}

TEST_CASE("qint: parse union with whitespace around |") {
	auto result = qint_parser_instance::instance()
		.parse("[0, 1) | [2, 3)", 15);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Integration with parse_qint (existing dispatcher)
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: dispatcher parse_qint with top") {
	auto result = parse_qint<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{top}");
	CHECK(result.has_value());
	auto val = std::get<qint>(result->first);
	CHECK(val.is_full());
}

TEST_CASE("qint: dispatcher parse_qint with bottom") {
	auto result = parse_qint<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{bot}");
	CHECK(result.has_value());
	auto val = std::get<qint>(result->first);
	CHECK(val.is_empty());
}

TEST_CASE("qint: dispatcher parse_qint with single interval") {
	auto result = parse_qint<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[0, 1)}");
	CHECK(result.has_value());
	auto val = std::get<qint>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK_FALSE(val.is_full());
}

TEST_CASE("qint: dispatcher parse_qint with union") {
	auto result = parse_qint<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[0, 1) | [2, 3)}");
	CHECK(result.has_value());
	auto val = std::get<qint>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK(val.pieces.size() == 2);
}

TEST_CASE("qint: dispatcher parse_qint with fractional interval") {
	auto result = parse_qint<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[1/4, 3/4)}");
	CHECK(result.has_value());
	auto val = std::get<qint>(result->first);
	CHECK_FALSE(val.is_empty());
	CHECK(val.pieces.size() == 1);
}

// ═══════════════════════════════════════════════════════════════════════════
// Error Cases
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: reject invalid interval (lo > hi)") {
	auto result = parse_qint<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[1, 0)}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qint: reject non-dyadic fraction 1/3") {
	auto result = parse_qint<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[1/3, 2/3)}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qint: reject non-dyadic decimal 0.1") {
	auto result = parse_qint<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{[0.1, 0.2)}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qint: reject malformed interval (missing bracket)") {
	auto result = qint_parser_instance::instance()
		.parse("(0, 1)", 6);
	CHECK_FALSE(result.found);
}

} // TEST_SUITE
