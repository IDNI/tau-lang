// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "../parser/hsb_parser.generated.h"

using namespace idni::tau_lang;
using namespace hsb_parser_data;

// Minimal hsb parser instance for grammar-level tests
struct hsb_parser_instance : public idni::parser<char, char> {
	static hsb_parser_instance& instance() {
		static hsb_parser_instance inst;
		return inst;
	}

	hsb_parser_instance() : idni::parser<char, char>(
		hsb_parser_data::grammar,
		hsb_parser_data::parser_options) {}

	size_t id(const std::basic_string<char>& name) {
		return hsb_parser_data::nts.get(name);
	}

	const std::basic_string<char>& name(size_t id) {
		return hsb_parser_data::nts.get(id);
	}
};

TEST_SUITE("hsb grammar parsing") {

	// ═══════════════════════════════════════════════════════════════════════════
	// Top-level Constants
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse 'top' constant") {
		auto result = hsb_parser_instance::instance().parse("top", 3);
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse 'bot' constant") {
		auto result = hsb_parser_instance::instance().parse("bot", 3);
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse 'bottom' constant") {
		auto result = hsb_parser_instance::instance().parse("bottom", 6);
		CHECK(result.found);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Single Halfspaces — Strict (<)
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse 'x[0] < 0'") {
		std::string s = "x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse 'x[1] < 0' (second variable)") {
		std::string s = "x[1] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '2*x[0] < 0' (integer coefficient, coeff*var)") {
		std::string s = "2*x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse 'x[0]*3 < 0' (var*coeff form)") {
		std::string s = "x[0]*3 < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '1.5*x[0] < 0' (decimal coefficient)") {
		std::string s = "1.5*x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '1/2*x[0] < 0' (fraction coefficient)") {
		std::string s = "1/2*x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '-x[0] < 0' (unary minus on variable)") {
		std::string s = "-x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '-2*x[0] < 0' (unary minus with coefficient)") {
		std::string s = "-2*x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Single Halfspaces — Non-strict (<=)
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse 'x[0] <= 0'") {
		std::string s = "x[0] <= 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '-x[0] <= 0'") {
		std::string s = "-x[0] <= 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '2*x[0] <= 0'") {
		std::string s = "2*x[0] <= 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Multi-variable Halfspaces
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse 'x[0] + x[1] < 0' (two variables, addition)") {
		std::string s = "x[0] + x[1] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse 'x[0] - x[1] < 0' (subtraction)") {
		std::string s = "x[0] - x[1] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '2*x[0] + 3*x[1] < 0' (multiple coefficients)") {
		std::string s = "2*x[0] + 3*x[1] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse 'x[0] + x[1] + x[2] < 0' (3D halfspace)") {
		std::string s = "x[0] + x[1] + x[2] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '-x[0] + x[1] < 0' (mixed signs)") {
		std::string s = "-x[0] + x[1] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Halfspaces with Bias (constant term)
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse 'x[0] + 1 < 0' (positive integer bias)") {
		std::string s = "x[0] + 1 < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse 'x[0] - 1 < 0' (negative bias term)") {
		std::string s = "x[0] - 1 < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse 'x[0] + 1/2 < 0' (fractional bias)") {
		std::string s = "x[0] + 1/2 < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse 'x[0] + 2.5 < 0' (decimal bias)") {
		std::string s = "x[0] + 2.5 < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '-x[0] + 2.5 < 0' (negative lead + decimal bias)") {
		std::string s = "-x[0] + 2.5 < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '2*x[0] + x[1] - 3 <= 0' (multi-var + bias)") {
		std::string s = "2*x[0] + x[1] - 3 <= 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Negation
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse '~top'") {
		std::string s = "~top";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '~bot'") {
		std::string s = "~bot";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '~(x[0] < 0)' (negated halfspace)") {
		std::string s = "~(x[0] < 0)";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '~~(x[0] < 0)' (double negation)") {
		std::string s = "~~(x[0] < 0)";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '~(x[0] <= 0)'") {
		std::string s = "~(x[0] <= 0)";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Conjunction (parens required)
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse '(x[0] < 0 & x[1] < 0)' (basic conjunction)") {
		std::string s = "(x[0] < 0 & x[1] < 0)";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '(top & bot)'") {
		std::string s = "(top & bot)";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse nested conjunction '(x[0] < 0 & (x[1] < 0 & x[2] < 0))'") {
		std::string s = "(x[0] < 0 & (x[1] < 0 & x[2] < 0))";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '(x[0] + x[1] < 0 & x[2] <= 0)'") {
		std::string s = "(x[0] + x[1] < 0 & x[2] <= 0)";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Disjunction (parens required)
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse '(x[0] < 0 | x[1] < 0)' (basic disjunction)") {
		std::string s = "(x[0] < 0 | x[1] < 0)";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '(top | bot)'") {
		std::string s = "(top | bot)";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse nested disjunction '((x[0] < 0 | x[1] < 0) | x[2] < 0)'") {
		std::string s = "((x[0] < 0 | x[1] < 0) | x[2] < 0)";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Mixed Boolean Combinations
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse '~(x[0] < 0 & x[1] < 0)' (not-and via paren)") {
		std::string s = "~(x[0] < 0 & x[1] < 0)";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '(~(x[0] < 0) | ~(x[1] < 0))'") {
		std::string s = "(~(x[0] < 0) | ~(x[1] < 0))";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse '((x[0] < 0 & x[1] < 0) | x[2] < 0)'") {
		std::string s = "((x[0] < 0 & x[1] < 0) | x[2] < 0)";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Whitespace Handling
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: parse with leading/trailing whitespace") {
		std::string s = "  top  ";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse with extra spaces around operator") {
		std::string s = "x[0]  <  0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse with spaces inside conjunction") {
		std::string s = "( x[0] < 0 & x[1] < 0 )";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	TEST_CASE("hsb: parse with spaces around +") {
		std::string s = "x[0]  +  x[1]  <  0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK(result.found);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Error Cases
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: reject 'x[0]' (no comparison)") {
		std::string s = "x[0]";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject 'x[0] > 0' (unsupported operator)") {
		std::string s = "x[0] > 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject 'x[0] < 1' (rhs is not 0)") {
		std::string s = "x[0] < 1";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject 'x[0] < 0 & x[1] < 0' (& without parens)") {
		std::string s = "x[0] < 0 & x[1] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject 'x[0] < 0 | x[1] < 0' (| without parens)") {
		std::string s = "x[0] < 0 | x[1] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject empty string") {
		auto result = hsb_parser_instance::instance().parse("", 0);
		CHECK_FALSE(result.found);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Malformed numeric literals
	// ═══════════════════════════════════════════════════════════════════════════

	TEST_CASE("hsb: reject '1//2*x[0] < 0' (double slash)") {
		std::string s = "1//2*x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject '1.2.3*x[0] < 0' (double dot)") {
		std::string s = "1.2.3*x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject '.5*x[0] < 0' (leading dot, no integer part)") {
		std::string s = ".5*x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject '1.*x[0] < 0' (trailing dot, no fractional part)") {
		std::string s = "1.*x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject '1/2/3*x[0] < 0' (chained slashes)") {
		std::string s = "1/2/3*x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject '1.2/3*x[0] < 0' (decimal then slash)") {
		std::string s = "1.2/3*x[0] < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject 'x[0] + 1//2 < 0' (double slash in bias)") {
		std::string s = "x[0] + 1//2 < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

	TEST_CASE("hsb: reject 'x[0] + .5 < 0' (leading dot in bias)") {
		std::string s = "x[0] + .5 < 0";
		auto result = hsb_parser_instance::instance().parse(s.c_str(), s.size());
		CHECK_FALSE(result.found);
	}

} // TEST_SUITE
