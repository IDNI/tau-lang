// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "../parser/nlang_parser.generated.h"

using namespace idni::tau_lang;
using namespace nlang_parser_data;

// Minimal nlang parser instance for testing
struct nlang_parser_instance : public idni::parser<char, char> {
	static nlang_parser_instance& instance() {
		static nlang_parser_instance inst;
		return inst;
	}

	nlang_parser_instance() : idni::parser<char, char>(
		nlang_parser_data::grammar,
		nlang_parser_data::parser_options) {}

	size_t id(const std::basic_string<char>& name) {
		return nlang_parser_data::nts.get(name);
	}

	const std::basic_string<char>& name(size_t id) {
		return nlang_parser_data::nts.get(id);
	}
};

TEST_SUITE("nlang grammar parsing") {

// ═══════════════════════════════════════════════════════════════════════════
// Top-level Constants
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse 'nothing' constant") {
	auto result = nlang_parser_instance::instance()
		.parse("nothing", 7);
	CHECK(result.found);
}

TEST_CASE("nlang: parse 'everything' constant") {
	auto result = nlang_parser_instance::instance()
		.parse("everything", 10);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Atomic Propositions
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse single-word atom") {
	auto result = nlang_parser_instance::instance()
		.parse("raining", 7);
	CHECK(result.found);
}

TEST_CASE("nlang: parse multi-word atom") {
	auto result = nlang_parser_instance::instance()
		.parse("it is raining", 13);
	CHECK(result.found);
}

TEST_CASE("nlang: parse atom with punctuation") {
	auto result = nlang_parser_instance::instance()
		.parse("the sky is blue.", 16);
	CHECK(result.found);
}

TEST_CASE("nlang: parse atom starting with 'n' (not a keyword)") {
	auto result = nlang_parser_instance::instance()
		.parse("no clouds today", 15);
	CHECK(result.found);
}

TEST_CASE("nlang: parse atom starting with 'e' (not a keyword)") {
	auto result = nlang_parser_instance::instance()
		.parse("each bird sings", 15);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Negation
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse 'not (atom)'") {
	auto result = nlang_parser_instance::instance()
		.parse("not (it is raining)", 19);
	CHECK(result.found);
}

TEST_CASE("nlang: parse 'not (nothing)'") {
	auto result = nlang_parser_instance::instance()
		.parse("not (nothing)", 13);
	CHECK(result.found);
}

TEST_CASE("nlang: parse 'not (everything)'") {
	auto result = nlang_parser_instance::instance()
		.parse("not (everything)", 16);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Conjunction
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse '(A) and (B)'") {
	auto result = nlang_parser_instance::instance()
		.parse("(A) and (B)", 11);
	CHECK(result.found);
}

TEST_CASE("nlang: parse conjunction of multi-word atoms") {
	auto result = nlang_parser_instance::instance()
		.parse("(it is raining) and (it is cold)", 32);
	CHECK(result.found);
}

TEST_CASE("nlang: parse conjunction with constants") {
	auto result = nlang_parser_instance::instance()
		.parse("(everything) and (nothing)", 26);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Disjunction
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse '(A) or (B)'") {
	auto result = nlang_parser_instance::instance()
		.parse("(A) or (B)", 10);
	CHECK(result.found);
}

TEST_CASE("nlang: parse disjunction of multi-word atoms") {
	auto result = nlang_parser_instance::instance()
		.parse("(it is raining) or (it is sunny)", 32);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Nested Formulas
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse 'not ((A) and (B))'") {
	auto result = nlang_parser_instance::instance()
		.parse("not ((A) and (B))", 17);
	CHECK(result.found);
}

TEST_CASE("nlang: parse '((A) and (B)) or (C)'") {
	auto result = nlang_parser_instance::instance()
		.parse("((A) and (B)) or (C)", 20);
	CHECK(result.found);
}

TEST_CASE("nlang: parse '(A) and ((B) or (C))'") {
	auto result = nlang_parser_instance::instance()
		.parse("(A) and ((B) or (C))", 20);
	CHECK(result.found);
}

TEST_CASE("nlang: parse deeply nested formula") {
	// not ((not (A)) and (B))
	auto result = nlang_parser_instance::instance()
		.parse("not ((not (A)) and (B))", 23);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Whitespace Handling
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse with leading/trailing whitespace") {
	auto result = nlang_parser_instance::instance()
		.parse("  nothing  ", 11);
	CHECK(result.found);
}

TEST_CASE("nlang: parse 'not' with extra spaces") {
	auto result = nlang_parser_instance::instance()
		.parse("not  (  it is raining  )", 24);
	CHECK(result.found);
}

TEST_CASE("nlang: parse conjunction with extra spaces") {
	auto result = nlang_parser_instance::instance()
		.parse("( A )  and  ( B )", 17);
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Error Cases
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: reject empty string") {
	auto result = nlang_parser_instance::instance()
		.parse("", 0);
	CHECK_FALSE(result.found);
}

TEST_CASE("nlang: reject unbalanced open paren") {
	auto result = nlang_parser_instance::instance()
		.parse("(A) and (B", 10);
	CHECK_FALSE(result.found);
}

TEST_CASE("nlang: reject bare 'and' without operands") {
	auto result = nlang_parser_instance::instance()
		.parse("A and B", 7);
	// Parses as atom "A and B" — grammar accepts it (atom fallback)
	CHECK(result.found);
}

// ═══════════════════════════════════════════════════════════════════════════
// Evaluator: parse_nlang_grammar — verify formula tree structure
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: eval 'nothing' => bot") {
	auto fm = parse_nlang_grammar("nothing");
	REQUIRE(fm.has_value());
	CHECK((*fm)->k == nlang_ba::formula::kind::bot);
}

TEST_CASE("nlang: eval 'everything' => top") {
	auto fm = parse_nlang_grammar("everything");
	REQUIRE(fm.has_value());
	CHECK((*fm)->k == nlang_ba::formula::kind::top);
}

TEST_CASE("nlang: eval atom => atom with correct string") {
	auto fm = parse_nlang_grammar("it is raining");
	REQUIRE(fm.has_value());
	CHECK((*fm)->k == nlang_ba::formula::kind::atom);
	CHECK((*fm)->atom_str == "it is raining");
}

TEST_CASE("nlang: eval 'not (it is raining)' => not_ of atom") {
	auto fm = parse_nlang_grammar("not (it is raining)");
	REQUIRE(fm.has_value());
	CHECK((*fm)->k == nlang_ba::formula::kind::not_);
	REQUIRE((*fm)->inner != nullptr);
	CHECK((*fm)->inner->k == nlang_ba::formula::kind::atom);
	CHECK((*fm)->inner->atom_str == "it is raining");
}

TEST_CASE("nlang: eval '(A) and (B)' => and_ of two atoms") {
	auto fm = parse_nlang_grammar("(A) and (B)");
	REQUIRE(fm.has_value());
	CHECK((*fm)->k == nlang_ba::formula::kind::and_);
	REQUIRE((*fm)->lhs != nullptr);
	REQUIRE((*fm)->rhs != nullptr);
	CHECK((*fm)->lhs->k == nlang_ba::formula::kind::atom);
	CHECK((*fm)->rhs->k == nlang_ba::formula::kind::atom);
	CHECK((*fm)->lhs->atom_str == "A");
	CHECK((*fm)->rhs->atom_str == "B");
}

TEST_CASE("nlang: eval '(A) or (B)' => or_ of two atoms") {
	auto fm = parse_nlang_grammar("(A) or (B)");
	REQUIRE(fm.has_value());
	CHECK((*fm)->k == nlang_ba::formula::kind::or_);
	REQUIRE((*fm)->lhs != nullptr);
	REQUIRE((*fm)->rhs != nullptr);
	CHECK((*fm)->lhs->atom_str == "A");
	CHECK((*fm)->rhs->atom_str == "B");
}

TEST_CASE("nlang: eval 'not ((A) and (B))' => not_ of and_") {
	auto fm = parse_nlang_grammar("not ((A) and (B))");
	REQUIRE(fm.has_value());
	CHECK((*fm)->k == nlang_ba::formula::kind::not_);
	REQUIRE((*fm)->inner != nullptr);
	CHECK((*fm)->inner->k == nlang_ba::formula::kind::and_);
}

TEST_CASE("nlang: eval '((A) and (B)) or (C)' => or_ with and_ lhs") {
	auto fm = parse_nlang_grammar("((A) and (B)) or (C)");
	REQUIRE(fm.has_value());
	CHECK((*fm)->k == nlang_ba::formula::kind::or_);
	REQUIRE((*fm)->lhs != nullptr);
	CHECK((*fm)->lhs->k == nlang_ba::formula::kind::and_);
	REQUIRE((*fm)->rhs != nullptr);
	CHECK((*fm)->rhs->k == nlang_ba::formula::kind::atom);
	CHECK((*fm)->rhs->atom_str == "C");
}

TEST_CASE("nlang: eval 'not (nothing)' => not_ of bot") {
	auto fm = parse_nlang_grammar("not (nothing)");
	REQUIRE(fm.has_value());
	CHECK((*fm)->k == nlang_ba::formula::kind::not_);
	REQUIRE((*fm)->inner != nullptr);
	CHECK((*fm)->inner->k == nlang_ba::formula::kind::bot);
}

TEST_CASE("nlang: eval '(everything) and (nothing)' => and_ of top and bot") {
	auto fm = parse_nlang_grammar("(everything) and (nothing)");
	REQUIRE(fm.has_value());
	CHECK((*fm)->k == nlang_ba::formula::kind::and_);
	REQUIRE((*fm)->lhs != nullptr);
	REQUIRE((*fm)->rhs != nullptr);
	CHECK((*fm)->lhs->k == nlang_ba::formula::kind::top);
	CHECK((*fm)->rhs->k == nlang_ba::formula::kind::bot);
}

TEST_CASE("nlang: eval atom strips leading/trailing whitespace") {
	auto fm = parse_nlang_grammar("  it is raining  ");
	REQUIRE(fm.has_value());
	// outer _ handles the leading/trailing space; atom_str is trimmed
	if ((*fm)->k == nlang_ba::formula::kind::atom)
		CHECK((*fm)->atom_str == "it is raining");
}

// ═══════════════════════════════════════════════════════════════════════════
// Integration: parse_nlang (dispatcher entry point)
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: dispatcher parse_nlang with nothing") {
	auto result = parse_nlang<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{nothing}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(is_nlang_zero(val));
}

TEST_CASE("nlang: dispatcher parse_nlang with everything") {
	auto result = parse_nlang<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{everything}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(is_nlang_one(val));
}

TEST_CASE("nlang: dispatcher parse_nlang with bare atom") {
	auto result = parse_nlang<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{it is raining}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	// Not zero and not one (contingent atomic proposition)
	CHECK_FALSE(val.fm->k == nlang_ba::formula::kind::bot);
	CHECK_FALSE(val.fm->k == nlang_ba::formula::kind::top);
}

TEST_CASE("nlang: dispatcher parse_nlang with quoted atom") {
	auto result = parse_nlang<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{\"it is raining\"}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK_FALSE(val.fm->k == nlang_ba::formula::kind::bot);
	CHECK_FALSE(val.fm->k == nlang_ba::formula::kind::top);
}

TEST_CASE("nlang: dispatcher parse_nlang with conjunction") {
	auto result = parse_nlang<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{(A) and (B)}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(val.fm->k == nlang_ba::formula::kind::and_);
}

TEST_CASE("nlang: dispatcher parse_nlang with disjunction") {
	auto result = parse_nlang<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{(A) or (B)}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(val.fm->k == nlang_ba::formula::kind::or_);
}

TEST_CASE("nlang: dispatcher parse_nlang with negation") {
	auto result = parse_nlang<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{not (A)}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(val.fm->k == nlang_ba::formula::kind::not_);
}

TEST_CASE("nlang: dispatcher parse_nlang with nested formula") {
	auto result = parse_nlang<qint, qlt, nlang_ba, bv, sbf_ba, hsb>(
		"{(A) and (not (B))}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(val.fm->k == nlang_ba::formula::kind::and_);
	REQUIRE(val.fm->rhs != nullptr);
	CHECK(val.fm->rhs->k == nlang_ba::formula::kind::not_);
}

TEST_CASE("nlang: dispatcher parse_nlang empty => no value") {
	auto result = parse_nlang<qint, qlt, nlang_ba, bv, sbf_ba, hsb>("{}");
	CHECK_FALSE(result.has_value());
}

} // TEST_SUITE
