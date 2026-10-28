// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <sstream>

#include "test_init.h"
#include "tau_pack.h"
#include "test_tau_helpers.h"
#include "boolean_algebras/qint/parser/qint_parser.generated.h"
#include "test_grammar_helpers.h"

using namespace idni::tau_lang;
using namespace qint_parser_data;

TEST_SUITE("qint grammar parsing") {

// ═══════════════════════════════════════════════════════════════════════════
// Top-level Constants
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE_TEMPLATE_INVOKE(grammar_constants, qint_parser);

// ═══════════════════════════════════════════════════════════════════════════
// Integer Literals
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse integer literal '0'") { CHECK(grammar_parses<qint_parser>("0")); }

TEST_CASE("qint: parse integer literal '1'") { CHECK(grammar_parses<qint_parser>("1")); }

TEST_CASE("qint: reject bare integer literal '5'") { CHECK_FALSE(grammar_parses<qint_parser>("5")); }

TEST_CASE("qint: reject bare negative integer literal '-3'") { CHECK_FALSE(grammar_parses<qint_parser>("-3")); }

// ═══════════════════════════════════════════════════════════════════════════
// Single Intervals with Integer Endpoints
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse single interval [0, 1)") { CHECK(grammar_parses<qint_parser>("[0, 1)")); }

TEST_CASE("qint: parse single interval [1, 2)") { CHECK(grammar_parses<qint_parser>("[1, 2)")); }

TEST_CASE("qint: parse single interval [-1, 0)") { CHECK(grammar_parses<qint_parser>("[-1, 0)")); }

// ═══════════════════════════════════════════════════════════════════════════
// Single Intervals with Fractional Endpoints
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse interval with dyadic fraction [1/4, 3/4)") { CHECK(grammar_parses<qint_parser>("[1/4, 3/4)")); }

TEST_CASE("qint: parse interval with dyadic fraction [1/2, 1)") { CHECK(grammar_parses<qint_parser>("[1/2, 1)")); }

TEST_CASE("qint: parse interval with decimal [0.25, 0.75)") { CHECK(grammar_parses<qint_parser>("[0.25, 0.75)")); }

TEST_CASE("qint: parse interval with negative fraction [-3/4, -1/4)") { CHECK(grammar_parses<qint_parser>("[-3/4, -1/4)")); }

// ═══════════════════════════════════════════════════════════════════════════
// Intervals with Infinities
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse interval [-inf, 0)") { CHECK(grammar_parses<qint_parser>("[-inf, 0)")); }

TEST_CASE("qint: parse interval [0, +inf)") { CHECK(grammar_parses<qint_parser>("[0, +inf)")); }

TEST_CASE("qint: parse interval [-infinity, +infinity)") { CHECK(grammar_parses<qint_parser>("[-infinity, +infinity)")); }

TEST_CASE("qint: parse interval [1/2, inf)") { CHECK(grammar_parses<qint_parser>("[1/2, inf)")); }

// ═══════════════════════════════════════════════════════════════════════════
// Union of Intervals
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse union [0, 1) | [2, 3)") { CHECK(grammar_parses<qint_parser>("[0, 1) | [2, 3)")); }

TEST_CASE("qint: parse union [-1, 0) | [1, 2)") { CHECK(grammar_parses<qint_parser>("[-1, 0) | [1, 2)")); }

TEST_CASE("qint: parse union with fractional intervals [1/4, 1/2) | [3/4, 1)") { CHECK(grammar_parses<qint_parser>("[1/4, 1/2) | [3/4, 1)")); }

TEST_CASE("qint: parse union of three intervals") { CHECK(grammar_parses<qint_parser>("[0, 1/3) | [1/2, 2/3) | [5/6, 1)")); }

// ═══════════════════════════════════════════════════════════════════════════
// Whitespace Handling
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: parse with leading/trailing whitespace") { CHECK(grammar_parses<qint_parser>("  [0, 1)  ")); }

TEST_CASE("qint: parse interval with internal whitespace [0, 1)") { CHECK(grammar_parses<qint_parser>("[ 0 , 1 )")); }

TEST_CASE("qint: parse union with whitespace around |") { CHECK(grammar_parses<qint_parser>("[0, 1) | [2, 3)")); }

// ═══════════════════════════════════════════════════════════════════════════
// Error Cases
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("qint: reject invalid interval (lo > hi)") {
	auto result = parse_qint<TAU_PACK_BASE_BAS>("{[1, 0)}");
	CHECK_FALSE(result.has_value());
}

TEST_CASE("qint: accept non-dyadic fraction 1/3") {
	auto result = parse_qint<TAU_PACK_BASE_BAS>("{[1/3, 2/3)}");
	CHECK(result.has_value());
}

TEST_CASE("qint: accept non-dyadic decimal 0.1") {
	auto result = parse_qint<TAU_PACK_BASE_BAS>("{[0.1, 0.2)}");
	CHECK(result.has_value());
}

TEST_CASE("qint: a bare number other than 0 and 1 is refused as a point") {
	for (const char* src : { "{5}", "{-3}", "{0.5}", "{2/3}" }) {
		INFO(src);
		auto result = parse_qint<TAU_PACK_BASE_BAS>(src);
		CHECK_FALSE(result.has_value());
		std::ostringstream msg;
		result.report().print(msg);
		CHECK(msg.str().find("qint has no points") != std::string::npos);
	}
}

TEST_CASE("qint: {0} and {1} are bottom and top") {
	auto bot = parse_qint<TAU_PACK_BASE_BAS>("{0}");
	auto top = parse_qint<TAU_PACK_BASE_BAS>("{1}");
	REQUIRE(bot.has_value());
	REQUIRE(top.has_value());
	CHECK(std::get<qint>(bot.value().first).is_empty());
	CHECK(std::get<qint>(top.value().first).is_full());
}

TEST_CASE("qint: reject malformed interval (missing bracket)") { CHECK_FALSE(grammar_parses<qint_parser>("(0, 1)")); }

} // TEST_SUITE
