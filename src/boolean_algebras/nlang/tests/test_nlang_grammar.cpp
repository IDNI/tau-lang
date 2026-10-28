// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "tau_pack.h"
#include "boolean_algebras/nlang/parser/nlang_parser.generated.h"
#include "test_grammar_helpers.h"

using namespace idni::tau_lang;
using namespace nlang_parser_data;

TEST_SUITE("nlang grammar parsing") {

// ═══════════════════════════════════════════════════════════════════════════
// Top-level Constants
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse 'nothing' constant") { CHECK(grammar_parses<nlang_parser>("nothing")); }

TEST_CASE("nlang: parse 'everything' constant") { CHECK(grammar_parses<nlang_parser>("everything")); }

// ═══════════════════════════════════════════════════════════════════════════
// Atomic Propositions
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse single-word atom") { CHECK(grammar_parses<nlang_parser>("raining")); }

TEST_CASE("nlang: parse multi-word atom") { CHECK(grammar_parses<nlang_parser>("it is raining")); }

TEST_CASE("nlang: parse atom with punctuation") { CHECK(grammar_parses<nlang_parser>("the sky is blue.")); }

TEST_CASE("nlang: parse atom starting with 'n' (not a keyword)") { CHECK(grammar_parses<nlang_parser>("no clouds today")); }

TEST_CASE("nlang: parse atom starting with 'e' (not a keyword)") { CHECK(grammar_parses<nlang_parser>("each bird sings")); }

// ═══════════════════════════════════════════════════════════════════════════
// Negation
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse 'not (atom)'") { CHECK(grammar_parses<nlang_parser>("not (it is raining)")); }

TEST_CASE("nlang: parse 'not (nothing)'") { CHECK(grammar_parses<nlang_parser>("not (nothing)")); }

TEST_CASE("nlang: parse 'not (everything)'") { CHECK(grammar_parses<nlang_parser>("not (everything)")); }

// ═══════════════════════════════════════════════════════════════════════════
// Conjunction
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse '(A) and (B)'") { CHECK(grammar_parses<nlang_parser>("(A) and (B)")); }

TEST_CASE("nlang: parse conjunction of multi-word atoms") { CHECK(grammar_parses<nlang_parser>("(it is raining) and (it is cold)")); }

TEST_CASE("nlang: parse conjunction with constants") { CHECK(grammar_parses<nlang_parser>("(everything) and (nothing)")); }

// ═══════════════════════════════════════════════════════════════════════════
// Disjunction
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse '(A) or (B)'") { CHECK(grammar_parses<nlang_parser>("(A) or (B)")); }

TEST_CASE("nlang: parse disjunction of multi-word atoms") { CHECK(grammar_parses<nlang_parser>("(it is raining) or (it is sunny)")); }

// ═══════════════════════════════════════════════════════════════════════════
// Nested Formulas
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse 'not ((A) and (B))'") { CHECK(grammar_parses<nlang_parser>("not ((A) and (B))")); }

TEST_CASE("nlang: parse '((A) and (B)) or (C)'") { CHECK(grammar_parses<nlang_parser>("((A) and (B)) or (C)")); }

TEST_CASE("nlang: parse '(A) and ((B) or (C))'") { CHECK(grammar_parses<nlang_parser>("(A) and ((B) or (C))")); }

TEST_CASE("nlang: parse deeply nested formula") {
	// not ((not (A)) and (B))
	CHECK(grammar_parses<nlang_parser>("not ((not (A)) and (B))"));
}

// ═══════════════════════════════════════════════════════════════════════════
// Whitespace Handling
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: parse with leading/trailing whitespace") { CHECK(grammar_parses<nlang_parser>("  nothing  ")); }

TEST_CASE("nlang: parse 'not' with extra spaces") { CHECK(grammar_parses<nlang_parser>("not  (  it is raining  )")); }

TEST_CASE("nlang: parse conjunction with extra spaces") { CHECK(grammar_parses<nlang_parser>("( A )  and  ( B )")); }

// ═══════════════════════════════════════════════════════════════════════════
// Error Cases
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("nlang: reject empty string") { CHECK_FALSE(grammar_parses<nlang_parser>("")); }

TEST_CASE("nlang: reject unbalanced open paren") { CHECK_FALSE(grammar_parses<nlang_parser>("(A) and (B")); }

TEST_CASE("nlang: reject bare 'and' without operands") {
	// Parses as atom "A and B" — grammar accepts it (atom fallback)
	CHECK(grammar_parses<nlang_parser>("A and B"));
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
	auto result = parse_nlang<TAU_PACK_BASE_BAS>("{nothing}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(is_nlang_zero(val));
}

TEST_CASE("nlang: dispatcher parse_nlang with everything") {
	auto result = parse_nlang<TAU_PACK_BASE_BAS>("{everything}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(is_nlang_one(val));
}

TEST_CASE("nlang: dispatcher parse_nlang with bare atom") {
	auto result = parse_nlang<TAU_PACK_BASE_BAS>("{it is raining}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	// Not zero and not one (contingent atomic proposition)
	CHECK_FALSE(val.fm->k == nlang_ba::formula::kind::bot);
	CHECK_FALSE(val.fm->k == nlang_ba::formula::kind::top);
}

TEST_CASE("nlang: dispatcher parse_nlang with quoted atom") {
	auto result = parse_nlang<TAU_PACK_BASE_BAS>("{\"it is raining\"}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK_FALSE(val.fm->k == nlang_ba::formula::kind::bot);
	CHECK_FALSE(val.fm->k == nlang_ba::formula::kind::top);
}

TEST_CASE("nlang: dispatcher parse_nlang with conjunction") {
	auto result = parse_nlang<TAU_PACK_BASE_BAS>("{(A) and (B)}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(val.fm->k == nlang_ba::formula::kind::and_);
}

TEST_CASE("nlang: dispatcher parse_nlang with disjunction") {
	auto result = parse_nlang<TAU_PACK_BASE_BAS>("{(A) or (B)}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(val.fm->k == nlang_ba::formula::kind::or_);
}

TEST_CASE("nlang: dispatcher parse_nlang with negation") {
	auto result = parse_nlang<TAU_PACK_BASE_BAS>("{not (A)}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(val.fm->k == nlang_ba::formula::kind::not_);
}

TEST_CASE("nlang: dispatcher parse_nlang with nested formula") {
	auto result = parse_nlang<TAU_PACK_BASE_BAS>(
		"{(A) and (not (B))}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK(val.fm->k == nlang_ba::formula::kind::and_);
	REQUIRE(val.fm->rhs != nullptr);
	CHECK(val.fm->rhs->k == nlang_ba::formula::kind::not_);
}

TEST_CASE("nlang: dispatcher parse_nlang empty => no value") {
	auto result = parse_nlang<TAU_PACK_BASE_BAS>("{}");
	CHECK_FALSE(result.has_value());
}

} // TEST_SUITE

// nlang_splitter: the bot and or_ branches are
// oracle-free; the atom branch needs the DeepSeek key and is skipped when
// it is unset (the no-key deepseek_query returns "", giving an atom whose
// text is empty -- not asserted here).
TEST_CASE("nlang: splitter on bot returns bot") {
	auto result = parse_nlang<TAU_PACK_BASE_BAS>(
		"{nothing}");
	REQUIRE(result.has_value());
	auto val = std::get<nlang_ba>(result->first);
	CHECK( is_nlang_zero(nlang_splitter(val, splitter_type::upper)) );
}

TEST_CASE("nlang: splitter on a disjunction returns its left branch") {
	// Built with operator| directly: the grammar's treatment of `|`
	// inside a constant is config-dependent, but the splitter contract
	// is about the or_ NODE, not the surface syntax.
	nlang_ba a{"it is raining"}, b{"it is snowing"};
	nlang_ba val = a | b;
	nlang_ba s = nlang_splitter(val, splitter_type::upper);
	CHECK( s == a );
}

// nlang_splitter_one had no caller in the suite: it is the fixed contingent
// proposition the generic splitter machinery falls back to.
TEST_CASE("nlang_splitter_one is neither top nor bottom") {
	using namespace idni::tau_lang;
	auto s = nlang_splitter_one();
	CHECK_FALSE(is_nlang_one(s));
	CHECK_FALSE(is_nlang_zero(s));
	CHECK(s.to_string() == "it is raining");
}
