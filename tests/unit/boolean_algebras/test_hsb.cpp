// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Comprehensive tests for hsb — atomless Boolean algebra of lex-half-open
// polyhedra in R^d (LP_d^Q).
// 200+ tests covering: algebra basics, is_zero/is_one/equality, splitter,
// atomlessness, parser, dispatcher, and LTL(hsb) integration.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "boolean_algebras/hsb.h"

using idni::tau_lang::hsb;
using idni::tau_lang::hsb_halfspace;
using idni::tau_lang::hsb_splitter;
using idni::tau_lang::hsb_splitter_one;
using idni::tau_lang::is_hsb_zero;
using idni::tau_lang::is_hsb_one;
using idni::tau_lang::normalize_hsb;
using idni::tau_lang::splitter_type;

// --- helpers ---

// Make halfspace H_{w,b}: <w,x> + b {<=,<} 0
static hsb make_hs(std::vector<double> w, double b) {
	hsb_halfspace h;
	h.w = std::move(w);
	h.b = b;
	return hsb::from_halfspace(h);
}

// Call gc() to flush interning caches between test cases.
static void do_gc() {
	std::unordered_set<tref> keep;
	tau::gc(keep);
}

struct gc_fixture { gc_fixture() { do_gc(); } };

// Parse a tau spec string and return the main wff tref.
static tref spec(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

// Parse an hsb string directly through eval_parse_tree.
static std::optional<hsb> eval_from_str(const std::string& s) {
	auto result = hsb_parser::instance().parse(s.c_str(), s.size());
	if (!result.found) return std::nullopt;
	auto t = hsb_parser::tree::traverser(result.get_shaped_tree2())
		| hsb_parser::hsb;
	if (!t.has_value()) return std::nullopt;
	return idni::tau_lang::hsb_grammar_detail::eval_parse_tree(t);
}

TEST_SUITE("hsb — basic construction") {

	TEST_CASE("bottom and top") {
		auto bot = hsb::bottom();
		auto top = hsb::top();
		CHECK(bot == false);
		CHECK(top == true);
		CHECK(bot != true);
		CHECK(top != false);
		CHECK(bot.to_string() == "bot");
		CHECK(top.to_string() == "top");
	}

	TEST_CASE("from_halfspace") {
		// {x_1 < 0}: w = (1), b = 0, s(w)=+1 => strict
		auto h = make_hs({1.0}, 0.0);
		CHECK(h != hsb::bottom());
		CHECK(h != hsb::top());
		CHECK(h.root->k == hsb::kind::halfspace);
		CHECK(h.root->hs.is_strict() == true);
	}

	TEST_CASE("from_halfspace non-strict") {
		// {x_1 <= 0}: w = (-1), b = 0, s(w)=-1 => non-strict
		auto h = make_hs({-1.0}, 0.0);
		CHECK(h.root->k == hsb::kind::halfspace);
		CHECK(h.root->hs.is_strict() == false);
	}

	TEST_CASE("from_string top/bot") {
		CHECK(hsb::from_string("top") == hsb::top());
		CHECK(hsb::from_string("bot") == hsb::bottom());
		CHECK(hsb::from_string("bottom") == hsb::bottom());
	}

	TEST_CASE("halfspace with positive bias") {
		auto h = make_hs({1.0}, 5.0);
		CHECK(h.root->k == hsb::kind::halfspace);
		CHECK(h.root->hs.b == doctest::Approx(5.0));
	}

	TEST_CASE("halfspace with negative bias") {
		auto h = make_hs({1.0}, -5.0);
		CHECK(h.root->k == hsb::kind::halfspace);
		CHECK(h.root->hs.b == doctest::Approx(-5.0));
	}

	TEST_CASE("2D halfspace construction") {
		auto h = make_hs({1.0, -2.0}, 3.0);
		CHECK(h.root->k == hsb::kind::halfspace);
		CHECK(h.root->hs.w.size() == 2);
		CHECK(h.root->hs.w[0] == doctest::Approx(1.0));
		CHECK(h.root->hs.w[1] == doctest::Approx(-2.0));
	}

	TEST_CASE("3D halfspace construction") {
		auto h = make_hs({1.0, 0.0, -3.0}, 2.5);
		CHECK(h.root->k == hsb::kind::halfspace);
		CHECK(h.root->hs.w.size() == 3);
	}

	TEST_CASE("halfspace with all-negative normal") {
		auto h = make_hs({-1.0, -2.0, -3.0}, 0.0);
		CHECK(h.root->hs.lex_leading_sign() == -1);
		CHECK(h.root->hs.is_strict() == false);
	}

	TEST_CASE("halfspace with all-positive normal") {
		auto h = make_hs({1.0, 2.0, 3.0}, 0.0);
		CHECK(h.root->hs.lex_leading_sign() == +1);
		CHECK(h.root->hs.is_strict() == true);
	}

	TEST_CASE("halfspace with leading zero components") {
		auto h = make_hs({0.0, 0.0, 1.0}, 0.0);
		CHECK(h.root->hs.lex_leading_sign() == +1);
		CHECK(h.root->hs.lex_leading_index() == 2);
	}

	TEST_CASE("halfspace with fractional coefficients") {
		auto h = make_hs({0.5, -0.3}, 0.7);
		CHECK(h.root->k == hsb::kind::halfspace);
		CHECK(h.root->hs.w[0] == doctest::Approx(0.5));
		CHECK(h.root->hs.w[1] == doctest::Approx(-0.3));
	}

	TEST_CASE("bottom equals false") {
		CHECK(hsb::bottom() == false);
		CHECK_FALSE(hsb::bottom() == true);
	}

	TEST_CASE("top equals true") {
		CHECK(hsb::top() == true);
		CHECK_FALSE(hsb::top() == false);
	}

	TEST_CASE("bottom not equal to top") {
		CHECK(hsb::bottom() != hsb::top());
	}

	TEST_CASE("two identical halfspaces are equal") {
		auto h1 = make_hs({1.0, -2.0}, 3.0);
		auto h2 = make_hs({1.0, -2.0}, 3.0);
		CHECK(h1 == h2);
	}

	TEST_CASE("two different halfspaces are not equal") {
		auto h1 = make_hs({1.0}, 0.0);
		auto h2 = make_hs({1.0}, 1.0);
		CHECK(h1 != h2);
	}

	TEST_CASE("4D halfspace construction") {
		auto h = make_hs({1.0, 0.0, -1.0, 2.0}, -0.5);
		CHECK(h.root->k == hsb::kind::halfspace);
		CHECK(h.root->hs.w.size() == 4);
	}

	TEST_CASE("5D halfspace construction") {
		auto h = make_hs({0.1, 0.2, 0.3, 0.4, 0.5}, -1.0);
		CHECK(h.root->k == hsb::kind::halfspace);
		CHECK(h.root->hs.w.size() == 5);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("halfspace with very large bias") {
		auto h = make_hs({1.0}, -1000000.0);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("halfspace with very small bias") {
		auto h = make_hs({1.0}, 1000000.0);
		CHECK(is_hsb_zero(h) == false);
	}

} // TEST_SUITE basic construction

TEST_SUITE("hsb — BA operations") {

	TEST_CASE("and with bot/top") {
		auto a = make_hs({1.0}, 0.0);
		CHECK((a & hsb::bottom()) == hsb::bottom());
		CHECK((hsb::bottom() & a) == hsb::bottom());
		CHECK((a & hsb::top()) == a);
		CHECK((hsb::top() & a) == a);
	}

	TEST_CASE("or with bot/top") {
		auto a = make_hs({1.0}, 0.0);
		CHECK((a | hsb::bottom()) == a);
		CHECK((hsb::bottom() | a) == a);
		CHECK((a | hsb::top()) == hsb::top());
		CHECK((hsb::top() | a) == hsb::top());
	}

	TEST_CASE("complement of bot/top") {
		CHECK(~hsb::bottom() == hsb::top());
		CHECK(~hsb::top() == hsb::bottom());
	}

	TEST_CASE("double complement") {
		auto a = make_hs({1.0}, 0.0);
		CHECK(~~a == a);
	}

	TEST_CASE("complement of halfspace is negated halfspace") {
		auto h = make_hs({1.0}, 0.0);
		auto c = ~h;
		CHECK(c.root->k == hsb::kind::halfspace);
		CHECK(c.root->hs.w[0] == doctest::Approx(-1.0));
		CHECK(c.root->hs.b == doctest::Approx(0.0));
		CHECK(c.root->hs.is_strict() == false);
	}

	TEST_CASE("complement closure: ~H is canonical") {
		auto h1 = make_hs({1.0, 0.0}, -0.5);
		auto c1 = ~h1;
		CHECK(c1.root->hs.lex_leading_sign() == -1);
		CHECK(c1.root->hs.is_strict() == false);

		auto h2 = make_hs({-1.0, 0.0}, 0.5);
		auto c2 = ~h2;
		CHECK(c2.root->hs.lex_leading_sign() == +1);
		CHECK(c2.root->hs.is_strict() == true);
	}

	TEST_CASE("idempotence: A & A = A, A | A = A") {
		auto a = make_hs({1.0}, 0.0);
		CHECK((a & a) == a);
		CHECK((a | a) == a);
	}

	TEST_CASE("complement law: A & ~A = bot, A | ~A = top") {
		auto a = make_hs({1.0}, 0.0);
		CHECK((a & ~a) == hsb::bottom());
		CHECK((a | ~a) == hsb::top());
	}

	TEST_CASE("xor") {
		auto a = make_hs({1.0}, 0.0);
		auto b = make_hs({0.0, 1.0}, 0.0);
		auto x = a ^ b;
		CHECK(x.root->k != hsb::kind::bot);
	}

	TEST_CASE("xor with self is bot") {
		auto a = make_hs({1.0}, 0.0);
		CHECK((a ^ a) == hsb::bottom());
	}

	TEST_CASE("commutativity of and (semantic)") {
		auto a = make_hs({1.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		CHECK(is_hsb_zero((a & b) ^ (b & a)) == true);
	}

	TEST_CASE("commutativity of or (semantic)") {
		auto a = make_hs({1.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		CHECK(is_hsb_zero((a | b) ^ (b | a)) == true);
	}

	TEST_CASE("associativity of and (semantic)") {
		auto a = make_hs({1.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		auto c = make_hs({1.0, 1.0}, -3.0);
		CHECK(is_hsb_zero(((a & b) & c) ^ (a & (b & c))) == true);
	}

	TEST_CASE("associativity of or (semantic)") {
		auto a = make_hs({1.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		auto c = make_hs({1.0, 1.0}, -3.0);
		CHECK(is_hsb_zero(((a | b) | c) ^ (a | (b | c))) == true);
	}

	TEST_CASE("De Morgan: ~(A & B) = ~A | ~B (semantic)") {
		auto a = make_hs({1.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		CHECK(is_hsb_zero(~(a & b) ^ (~a | ~b)) == true);
	}

	TEST_CASE("De Morgan: ~(A | B) = ~A & ~B (semantic)") {
		auto a = make_hs({1.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		CHECK(is_hsb_zero(~(a | b) ^ (~a & ~b)) == true);
	}

	TEST_CASE("absorption: A & (A | B) = A (semantic)") {
		auto a = make_hs({1.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		CHECK(is_hsb_zero((a & (a | b)) ^ a) == true);
	}

	TEST_CASE("absorption: A | (A & B) = A (semantic)") {
		auto a = make_hs({1.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		CHECK(is_hsb_zero((a | (a & b)) ^ a) == true);
	}

	TEST_CASE("distributivity: A & (B | C) = (A & B) | (A & C) (semantic)") {
		auto a = make_hs({1.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		auto c = make_hs({1.0, 1.0}, -3.0);
		CHECK(is_hsb_zero((a & (b | c)) ^ ((a & b) | (a & c))) == true);
	}

	TEST_CASE("bot & bot = bot") {
		CHECK((hsb::bottom() & hsb::bottom()) == hsb::bottom());
	}

	TEST_CASE("top & top = top") {
		CHECK((hsb::top() & hsb::top()) == hsb::top());
	}

	TEST_CASE("bot | bot = bot") {
		CHECK((hsb::bottom() | hsb::bottom()) == hsb::bottom());
	}

	TEST_CASE("top | top = top") {
		CHECK((hsb::top() | hsb::top()) == hsb::top());
	}

	TEST_CASE("complement of 2D halfspace") {
		auto h = make_hs({1.0, -2.0}, 3.0);
		auto c = ~h;
		CHECK(c.root->hs.w[0] == doctest::Approx(-1.0));
		CHECK(c.root->hs.w[1] == doctest::Approx(2.0));
		CHECK(c.root->hs.b == doctest::Approx(-3.0));
	}

	TEST_CASE("double complement of 2D compound") {
		auto a = make_hs({1.0, 0.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		auto compound = a & b;
		CHECK(~~compound == compound);
	}

	TEST_CASE("xor commutativity (semantic)") {
		auto a = make_hs({1.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		CHECK(is_hsb_zero((a ^ b) ^ (b ^ a)) == true);
	}

	TEST_CASE("complement law on 2D halfspace") {
		auto h = make_hs({1.0, -2.0}, 3.0);
		CHECK((h & ~h) == hsb::bottom());
		CHECK((h | ~h) == hsb::top());
	}

	TEST_CASE("triple negation equals single negation") {
		auto a = make_hs({1.0}, -1.0);
		CHECK(~~~a == ~a);
	}

	TEST_CASE("and of three elements") {
		auto a = make_hs({1.0, 0.0}, -5.0);
		auto b = make_hs({0.0, 1.0}, -5.0);
		auto c = make_hs({1.0, 1.0}, -5.0);
		auto all = a & b & c;
		CHECK(is_hsb_zero(all) == false);
	}

	TEST_CASE("or of three elements") {
		auto a = make_hs({1.0}, 0.0);
		auto b = make_hs({-1.0}, 0.0);
		auto c = make_hs({0.0, 1.0}, 0.0);
		auto any = a | b | c;
		CHECK(is_hsb_zero(any) == false);
	}

} // TEST_SUITE BA operations

TEST_SUITE("hsb — structural simplifications") {

	TEST_CASE("not-not reduction") {
		auto a = make_hs({1.0, 2.0}, -3.0);
		auto a_and_b = a & make_hs({0.0, 1.0}, 0.0);
		auto neg = ~a_and_b;
		auto neg_neg = ~neg;
		CHECK(neg_neg == a_and_b);
	}

	TEST_CASE("bot absorbs in and") {
		auto a = make_hs({1.0}, 0.0);
		auto b = make_hs({0.0, 1.0}, 0.0);
		auto compound = a & b;
		CHECK((compound & hsb::bottom()) == hsb::bottom());
	}

	TEST_CASE("top is identity in and") {
		auto a = make_hs({1.0}, 0.0);
		auto b = make_hs({0.0, 1.0}, 0.0);
		auto compound = a & b;
		CHECK((compound & hsb::top()) == compound);
	}

	TEST_CASE("top is identity in or") {
		auto a = make_hs({1.0}, 0.0);
		CHECK((a | hsb::top()) == hsb::top());
	}

	TEST_CASE("bot is identity in or") {
		auto a = make_hs({1.0}, 0.0);
		CHECK((a | hsb::bottom()) == a);
	}

	TEST_CASE("A ^ bot = A") {
		auto a = make_hs({1.0}, -1.0);
		CHECK((a ^ hsb::bottom()) == a);
	}

	TEST_CASE("A ^ top = ~A") {
		auto a = make_hs({1.0}, -1.0);
		CHECK((a ^ hsb::top()) == ~a);
	}

} // TEST_SUITE structural simplifications

TEST_SUITE("hsb — is_zero / is_one") {

	TEST_CASE("bot is zero") {
		CHECK(is_hsb_zero(hsb::bottom()) == true);
	}

	TEST_CASE("top is one") {
		CHECK(is_hsb_one(hsb::top()) == true);
	}

	TEST_CASE("top is not zero") {
		CHECK(is_hsb_zero(hsb::top()) == false);
	}

	TEST_CASE("bot is not one") {
		CHECK(is_hsb_one(hsb::bottom()) == false);
	}

	TEST_CASE("single halfspace is not zero") {
		auto h = make_hs({1.0}, 0.0);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("single halfspace is not one") {
		auto h = make_hs({1.0}, 0.0);
		CHECK(is_hsb_one(h) == false);
	}

	TEST_CASE("contradictory conjunction is zero") {
		auto a = make_hs({1.0}, 0.0);
		auto c = ~a;
		auto both = a & c;
		CHECK(both == hsb::bottom());
	}

	TEST_CASE("feasibility: contradictory halfspaces via LP") {
		auto left = make_hs({1.0}, 1.0);   // x_1 < -1
		auto right = make_hs({-1.0}, 2.0); // x_1 >= 2
		auto both = left & right;
		CHECK(is_hsb_zero(both) == true);
	}

	TEST_CASE("feasibility: compatible halfspaces not zero") {
		auto a = make_hs({1.0}, -1.0);
		auto b = make_hs({-1.0}, -1.0);
		auto both = a & b;
		CHECK(is_hsb_zero(both) == false);
	}

	TEST_CASE("2D: feasibility of compatible halfspaces") {
		auto a = make_hs({1.0, 0.0}, -1.0);
		auto b = make_hs({0.0, 1.0}, -1.0);
		auto both = a & b;
		CHECK(is_hsb_zero(both) == false);
	}

	TEST_CASE("2D: infeasible diagonal constraints") {
		auto a = make_hs({1.0, 1.0}, 10.0);
		auto b = make_hs({-1.0, -1.0}, -5.0);
		auto both = a & b;
		CHECK(is_hsb_zero(both) == true);
	}

	TEST_CASE("normalize reduces contradictions") {
		auto a = make_hs({1.0}, 1.0);
		auto b = make_hs({-1.0}, 2.0);
		auto both = a & b;
		auto n = normalize_hsb(both);
		CHECK(n == hsb::bottom());
	}

	TEST_CASE("single non-strict halfspace is not zero") {
		auto h = make_hs({-1.0}, 0.0);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("single non-strict halfspace is not one") {
		auto h = make_hs({-1.0}, 0.0);
		CHECK(is_hsb_one(h) == false);
	}

	TEST_CASE("parallel constraints: compatible") {
		// x_1 < 5 AND x_1 < 10 => non-empty
		auto a = make_hs({1.0}, -5.0);
		auto b = make_hs({1.0}, -10.0);
		CHECK(is_hsb_zero(a & b) == false);
	}

	TEST_CASE("parallel constraints: redundant") {
		// x_1 < 5 AND x_1 < 3 => same as x_1 < 3
		auto a = make_hs({1.0}, -5.0);
		auto b = make_hs({1.0}, -3.0);
		auto both = a & b;
		CHECK(is_hsb_zero(both) == false);
	}

	TEST_CASE("3D feasibility: compatible") {
		auto a = make_hs({1.0, 0.0, 0.0}, -5.0);
		auto b = make_hs({0.0, 1.0, 0.0}, -5.0);
		auto c = make_hs({0.0, 0.0, 1.0}, -5.0);
		CHECK(is_hsb_zero(a & b & c) == false);
	}

	TEST_CASE("3D feasibility: contradictory") {
		auto a = make_hs({1.0, 0.0, 0.0}, 10.0);
		auto b = make_hs({-1.0, 0.0, 0.0}, 0.0);
		CHECK(is_hsb_zero(a & b) == true);
	}

	TEST_CASE("4D feasibility: compatible cross-dimensional") {
		auto a = make_hs({1.0, 0.0, 0.0, 0.0}, -10.0);
		auto b = make_hs({0.0, 1.0, 0.0, 0.0}, -10.0);
		auto c = make_hs({0.0, 0.0, 1.0, 0.0}, -10.0);
		auto d = make_hs({0.0, 0.0, 0.0, 1.0}, -10.0);
		CHECK(is_hsb_zero(a & b & c & d) == false);
	}

	TEST_CASE("4D feasibility: contradictory") {
		auto a = make_hs({1.0, 1.0, 1.0, 1.0}, 100.0);
		auto b = make_hs({-1.0, -1.0, -1.0, -1.0}, 0.0);
		CHECK(is_hsb_zero(a & b) == true);
	}

	TEST_CASE("disjunction is not zero when one branch feasible") {
		auto empty = make_hs({1.0}, 1.0) & make_hs({-1.0}, 2.0);
		auto nonempty = make_hs({1.0}, -5.0);
		CHECK(is_hsb_zero(empty | nonempty) == false);
	}

	TEST_CASE("disjunction of two empty is zero") {
		auto e1 = make_hs({1.0}, 1.0) & make_hs({-1.0}, 2.0);
		auto e2 = make_hs({1.0}, 3.0) & make_hs({-1.0}, 5.0);
		CHECK(is_hsb_zero(e1 | e2) == true);
	}

	TEST_CASE("is_one via complement: non-strict halfspace complement is not one") {
		auto h = make_hs({-1.0}, 0.0);
		CHECK(is_hsb_one(h) == false);
	}

	TEST_CASE("equality via symmetric difference: equal elements") {
		auto a = make_hs({1.0}, -5.0);
		auto b = make_hs({1.0}, -5.0);
		CHECK(is_hsb_zero(a ^ b) == true);
	}

	TEST_CASE("equality via symmetric difference: unequal elements") {
		auto a = make_hs({1.0}, -5.0);
		auto b = make_hs({1.0}, -3.0);
		CHECK(is_hsb_zero(a ^ b) == false);
	}

	TEST_CASE("normalize of top is top") {
		auto n = normalize_hsb(hsb::top());
		CHECK(n == hsb::top());
	}

	TEST_CASE("normalize of bot is bot") {
		auto n = normalize_hsb(hsb::bottom());
		CHECK(n == hsb::bottom());
	}

	TEST_CASE("normalize of single halfspace preserves non-emptiness") {
		auto h = make_hs({1.0}, -5.0);
		auto n = normalize_hsb(h);
		CHECK(is_hsb_zero(n) == false);
	}

	TEST_CASE("2D mixed strict/non-strict compatible") {
		auto strict = make_hs({1.0, 0.0}, -1.0);
		auto non_strict = make_hs({-1.0, 0.0}, -1.0);
		CHECK(is_hsb_zero(strict & non_strict) == false);
	}

	TEST_CASE("2D diagonal contradictory with offset") {
		auto a = make_hs({1.0, -1.0}, 20.0);
		auto b = make_hs({-1.0, 1.0}, -10.0);
		CHECK(is_hsb_zero(a & b) == true);
	}

	TEST_CASE("redundant constraint does not affect feasibility") {
		auto h = make_hs({1.0}, -5.0);
		auto redundant = make_hs({1.0}, -10.0);
		auto both = h & redundant;
		CHECK(is_hsb_zero(both) == false);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("complement of A|B is empty when A|B is top") {
		auto a = make_hs({1.0}, 0.0);
		auto b = ~a;
		auto full = a | b;
		CHECK(is_hsb_one(full) == true);
		CHECK(is_hsb_zero(~full) == true);
	}

	TEST_CASE("tight bounds: exactly touching boundary") {
		// x >= 5 AND x < 5 is empty (atomless)
		auto geq5 = make_hs({-1.0}, 5.0);
		auto lt5 = make_hs({1.0}, -5.0);
		CHECK(is_hsb_zero(geq5 & lt5) == true);
	}

	TEST_CASE("narrow feasible region") {
		// x >= 4.999 AND x < 5.001
		auto lower = make_hs({-1.0}, 4.999);
		auto upper = make_hs({1.0}, -5.001);
		CHECK(is_hsb_zero(lower & upper) == false);
	}

} // TEST_SUITE is_zero / is_one

TEST_SUITE("hsb — splitter") {

	TEST_CASE("splitter of bot is bot") {
		auto s = hsb_splitter(hsb::bottom(), splitter_type::lower);
		CHECK(s == hsb::bottom());
	}

	TEST_CASE("splitter of top is non-trivial") {
		auto s = hsb_splitter(hsb::top(), splitter_type::lower);
		CHECK(s != hsb::bottom());
		CHECK(s != hsb::top());
	}

	TEST_CASE("splitter of single halfspace produces strict sub-element") {
		auto h = make_hs({1.0}, -5.0);
		auto s = hsb_splitter(h, splitter_type::lower);
		CHECK(s != hsb::bottom());
	}

	TEST_CASE("splitter_one is non-trivial") {
		auto s = hsb_splitter_one();
		CHECK(s != hsb::bottom());
		CHECK(s != hsb::top());
		CHECK(is_hsb_zero(s) == false);
		CHECK(is_hsb_one(s) == false);
	}

	TEST_CASE("atomlessness: splitter of non-zero yields strict sub-element") {
		auto h1 = make_hs({1.0}, -5.0);
		auto s1 = hsb_splitter(h1, splitter_type::lower);
		CHECK(is_hsb_zero(s1) == false);

		auto h2 = make_hs({-1.0}, -3.0);
		auto s2 = hsb_splitter(h2, splitter_type::lower);
		CHECK(is_hsb_zero(s2) == false);
	}

	TEST_CASE("splitter of top with upper type") {
		auto s = hsb_splitter(hsb::top(), splitter_type::upper);
		CHECK(s != hsb::bottom());
		CHECK(s != hsb::top());
	}

	TEST_CASE("splitter in 2D") {
		auto h = make_hs({1.0, 0.0}, -5.0) & make_hs({0.0, 1.0}, -5.0);
		auto s = hsb_splitter(h, splitter_type::lower);
		CHECK(is_hsb_zero(s) == false);
	}

	TEST_CASE("splitter in 3D") {
		auto h = make_hs({1.0, 0.0, 0.0}, -5.0)
			& make_hs({0.0, 1.0, 0.0}, -5.0)
			& make_hs({0.0, 0.0, 1.0}, -5.0);
		auto s = hsb_splitter(h, splitter_type::lower);
		CHECK(is_hsb_zero(s) == false);
	}

	TEST_CASE("splitter produces canonical halfspace") {
		auto h = make_hs({1.0}, -5.0);
		auto s = hsb_splitter(h, splitter_type::lower);
		// The splitter should be a conjunction or halfspace, not arbitrary
		CHECK(s != hsb::bottom());
	}

	TEST_CASE("splitter of complement") {
		auto h = make_hs({1.0}, -5.0);
		auto c = ~h;
		auto s = hsb_splitter(c, splitter_type::lower);
		CHECK(is_hsb_zero(s) == false);
	}

	TEST_CASE("splitter of disjunction") {
		auto a = make_hs({1.0}, 5.0);     // x_1 < -5
		auto b = make_hs({-1.0}, 5.0);    // x_1 >= 5
		auto disj = a | b;
		auto s = hsb_splitter(disj, splitter_type::lower);
		CHECK(is_hsb_zero(s) == false);
	}

	TEST_CASE("splitter_one is a halfspace") {
		auto s = hsb_splitter_one();
		CHECK(s.root->k == hsb::kind::halfspace);
	}

	TEST_CASE("splitter of conjunction") {
		auto a = make_hs({1.0}, -3.0);
		auto b = make_hs({-1.0}, -2.0);
		auto conj = a & b;
		if (!is_hsb_zero(conj)) {
			auto s = hsb_splitter(conj, splitter_type::lower);
			CHECK(is_hsb_zero(s) == false);
			CHECK(is_hsb_zero(conj & s) == false);
		}
	}

	TEST_CASE("splitter of 2D region") {
		auto h1 = make_hs({1.0, 0.0}, -10.0);
		auto h2 = make_hs({-1.0, 0.0}, 0.0);
		auto region = h1 & h2;
		if (!is_hsb_zero(region)) {
			auto s = hsb_splitter(region, splitter_type::lower);
			CHECK(is_hsb_zero(s) == false);
			CHECK(is_hsb_zero(s & ~region) == true);
		}
	}

	TEST_CASE("splitter of 3D region") {
		auto h = make_hs({1.0, 0.0, 0.0}, -5.0);
		auto s = hsb_splitter(h, splitter_type::lower);
		CHECK(is_hsb_zero(s) == false);
		CHECK(is_hsb_zero(h & s) == false);
	}

	TEST_CASE("splitter result is sub-element") {
		auto h = make_hs({1.0}, -5.0);
		auto s = hsb_splitter(h, splitter_type::lower);
		CHECK(is_hsb_zero(s & ~h) == true);
	}

	TEST_CASE("splitter complement is sub-element") {
		auto h = make_hs({1.0}, -5.0);
		auto s = hsb_splitter(h, splitter_type::lower);
		auto other_half = h & ~s;
		CHECK(is_hsb_zero(other_half & ~h) == true);
	}

	TEST_CASE("splitter of wide interval") {
		auto a = make_hs({1.0}, -100.0);
		auto b = make_hs({-1.0}, 0.0);
		auto region = a & b;
		auto s = hsb_splitter(region, splitter_type::lower);
		CHECK(is_hsb_zero(s) == false);
		CHECK(is_hsb_zero(s & ~region) == true);
	}

	TEST_CASE("repeated split refines") {
		auto h = make_hs({1.0}, -10.0);
		auto s1 = hsb_splitter(h, splitter_type::lower);
		auto left = h & s1;
		if (!is_hsb_zero(left)) {
			auto s2 = hsb_splitter(left, splitter_type::lower);
			CHECK(is_hsb_zero(s2) == false);
			CHECK(is_hsb_zero(left & s2) == false);
		}
	}

	TEST_CASE("splitter of negated halfspace") {
		auto h = make_hs({1.0}, 0.0);
		auto neg = ~h;
		auto s = hsb_splitter(neg, splitter_type::lower);
		CHECK(is_hsb_zero(s) == false);
	}

} // TEST_SUITE splitter

TEST_SUITE("hsb — atomlessness") {

	TEST_CASE("repeated splitting does not hit bottom (1D)") {
		auto h = make_hs({1.0}, -10.0);
		for (int i = 0; i < 5; ++i) {
			auto s = hsb_splitter(h, splitter_type::lower);
			CHECK(is_hsb_zero(s) == false);
			h = s;
		}
	}

	TEST_CASE("repeated splitting does not hit bottom (2D)") {
		auto h = make_hs({1.0, 0.0}, -10.0) & make_hs({0.0, 1.0}, -10.0);
		for (int i = 0; i < 3; ++i) {
			auto s = hsb_splitter(h, splitter_type::lower);
			CHECK(is_hsb_zero(s) == false);
			h = s;
		}
	}

	TEST_CASE("for non-empty S, exists strict sub-element S'") {
		auto S = make_hs({1.0}, -5.0);
		auto S_prime = hsb_splitter(S, splitter_type::lower);
		CHECK(is_hsb_zero(S_prime) == false);
		// S' is contained in S: S' & ~S should be zero
		CHECK(is_hsb_zero(S_prime & ~S) == true);
	}

	TEST_CASE("for non-empty S', S \\ S' is non-empty") {
		auto S = make_hs({1.0}, -5.0);
		auto S_prime = hsb_splitter(S, splitter_type::lower);
		auto diff = S & ~S_prime;
		CHECK(is_hsb_zero(diff) == false);
	}

	TEST_CASE("splitter of non-strict halfspace yields non-empty sub") {
		auto h = make_hs({-1.0}, -3.0);
		auto s = hsb_splitter(h, splitter_type::lower);
		CHECK(is_hsb_zero(s) == false);
		CHECK(is_hsb_zero(s & ~h) == true);
	}

	TEST_CASE("splitter of 2D diagonal yields non-empty sub") {
		auto h = make_hs({1.0, 1.0}, -10.0);
		auto s = hsb_splitter(h, splitter_type::lower);
		CHECK(is_hsb_zero(s) == false);
	}

	TEST_CASE("top can be split indefinitely") {
		auto h = hsb::top();
		for (int i = 0; i < 5; ++i) {
			auto s = hsb_splitter(h, splitter_type::lower);
			CHECK(is_hsb_zero(s) == false);
			CHECK(is_hsb_one(s) == false);
			h = s;
		}
	}

	TEST_CASE("splitter_one splits top into two non-empty parts") {
		auto s = hsb_splitter_one();
		auto other = hsb::top() & ~s;
		CHECK(is_hsb_zero(s) == false);
		CHECK(is_hsb_zero(other) == false);
	}

	TEST_CASE("split of non-empty 3D region") {
		auto h = make_hs({1.0, 0.0, 0.0}, -1.0)
			& make_hs({0.0, 1.0, 0.0}, -1.0)
			& make_hs({0.0, 0.0, 1.0}, -1.0);
		auto s = hsb_splitter(h, splitter_type::lower);
		CHECK(is_hsb_zero(s) == false);
		auto rest = h & ~s;
		CHECK(is_hsb_zero(rest) == false);
	}

	TEST_CASE("split of complement region") {
		auto h = make_hs({1.0}, -5.0);
		auto c = ~h;
		auto s = hsb_splitter(c, splitter_type::lower);
		CHECK(is_hsb_zero(s) == false);
		// S is contained in complement: s & ~c = bot
		CHECK(is_hsb_zero(s & ~c) == true);
	}

} // TEST_SUITE atomlessness

TEST_SUITE("hsb — 1D reduction") {

	TEST_CASE("1D half-open interval pattern: [a, b)") {
		double a_val = 2.0, b_val = 5.0;
		auto lower = make_hs({-1.0}, a_val);
		auto upper = make_hs({1.0}, -b_val);
		auto interval = lower & upper;
		CHECK(is_hsb_zero(interval) == false);
	}

	TEST_CASE("1D empty interval: a >= b") {
		double a_val = 5.0, b_val = 2.0;
		auto lower = make_hs({-1.0}, a_val);
		auto upper = make_hs({1.0}, -b_val);
		auto interval = lower & upper;
		CHECK(is_hsb_zero(interval) == true);
	}

	TEST_CASE("1D complement produces complementary interval") {
		auto h = make_hs({1.0}, -3.0);
		auto c = ~h;
		CHECK(c.root->hs.w[0] == doctest::Approx(-1.0));
		CHECK(c.root->hs.b == doctest::Approx(3.0));
		CHECK(c.root->hs.is_strict() == false);
	}

	TEST_CASE("1D interval [0, 1) is non-empty") {
		auto lower = make_hs({-1.0}, 0.0);
		auto upper = make_hs({1.0}, -1.0);
		CHECK(is_hsb_zero(lower & upper) == false);
	}

	TEST_CASE("1D interval [-1, 0) is non-empty") {
		auto lower = make_hs({-1.0}, -1.0);
		auto upper = make_hs({1.0}, 0.0);
		CHECK(is_hsb_zero(lower & upper) == false);
	}

} // TEST_SUITE 1D reduction

TEST_SUITE("hsb — singleton exclusion") {

	TEST_CASE("single-point set is empty (atomless property)") {
		auto geq3 = make_hs({-1.0}, 3.0);
		auto lt3  = make_hs({1.0}, -3.0);
		auto point = geq3 & lt3;
		CHECK(is_hsb_zero(point) == true);
	}

	TEST_CASE("2D single point is empty") {
		auto a = make_hs({-1.0, 0.0}, 0.0);
		auto b = make_hs({1.0, 0.0}, 0.0);
		auto pt = a & b;
		CHECK(is_hsb_zero(pt) == true);
	}

	TEST_CASE("single point via two constraints") {
		auto geq0 = make_hs({-1.0}, 0.0);
		auto lt0 = make_hs({1.0}, 0.0);
		CHECK(is_hsb_zero(geq0 & lt0) == true);
	}

	TEST_CASE("hyperplane in 2D is empty") {
		// x_1 + x_2 = 5 => x_1+x_2 <= 5 AND x_1+x_2 >= 5
		// But canonical form makes one strict, so intersection is empty
		auto leq = make_hs({1.0, 1.0}, -5.0);   // x_1+x_2 < 5 (strict)
		auto geq = make_hs({-1.0, -1.0}, 5.0);  // x_1+x_2 >= 5
		CHECK(is_hsb_zero(leq & geq) == true);
	}

} // TEST_SUITE singleton exclusion

TEST_SUITE("hsb — hsb_halfspace") {

	TEST_CASE("lex_leading_index") {
		hsb_halfspace h;
		h.w = {0.0, 0.0, 3.0, -1.0};
		CHECK(h.lex_leading_index() == 2);
	}

	TEST_CASE("lex_leading_sign positive") {
		hsb_halfspace h;
		h.w = {0.0, 2.0, -1.0};
		CHECK(h.lex_leading_sign() == +1);
	}

	TEST_CASE("lex_leading_sign negative") {
		hsb_halfspace h;
		h.w = {0.0, -3.0, 1.0};
		CHECK(h.lex_leading_sign() == -1);
	}

	TEST_CASE("negate") {
		hsb_halfspace h;
		h.w = {1.0, -2.0};
		h.b = 3.0;
		auto n = h.negate();
		CHECK(n.w[0] == doctest::Approx(-1.0));
		CHECK(n.w[1] == doctest::Approx(2.0));
		CHECK(n.b == doctest::Approx(-3.0));
	}

	TEST_CASE("eval") {
		hsb_halfspace h;
		h.w = {2.0, 3.0};
		h.b = -1.0;
		std::vector<double> x = {1.0, 1.0};
		CHECK(h.eval(x) == doctest::Approx(4.0));
	}

	TEST_CASE("contains: strict") {
		hsb_halfspace h;
		h.w = {1.0};
		h.b = 0.0;
		CHECK(h.contains({-1.0}) == true);
		CHECK(h.contains({0.0}) == false);
		CHECK(h.contains({1.0}) == false);
	}

	TEST_CASE("contains: non-strict") {
		hsb_halfspace h;
		h.w = {-1.0};
		h.b = 0.0;
		CHECK(h.contains({1.0}) == true);
		CHECK(h.contains({0.0}) == true);
		CHECK(h.contains({-1.0}) == false);
	}

	TEST_CASE("to_string") {
		hsb_halfspace h;
		h.w = {1.0};
		h.b = 0.0;
		std::string s = h.to_string();
		CHECK(s.find("x[0]") != std::string::npos);
		CHECK(s.find("< 0") != std::string::npos);
	}

	TEST_CASE("lex_leading_index with single element") {
		hsb_halfspace h;
		h.w = {5.0};
		CHECK(h.lex_leading_index() == 0);
	}

	TEST_CASE("eval with zero coefficients") {
		hsb_halfspace h;
		h.w = {0.0, 0.0, 1.0};
		h.b = -2.0;
		CHECK(h.eval({1.0, 2.0, 3.0}) == doctest::Approx(1.0));
	}

	TEST_CASE("negate flips sign") {
		hsb_halfspace h;
		h.w = {-3.0};
		h.b = 7.0;
		auto n = h.negate();
		CHECK(n.w[0] == doctest::Approx(3.0));
		CHECK(n.b == doctest::Approx(-7.0));
	}

	TEST_CASE("negate twice returns original") {
		hsb_halfspace h;
		h.w = {1.0, -2.0, 3.0};
		h.b = -4.0;
		auto nn = h.negate().negate();
		CHECK(nn.w[0] == doctest::Approx(h.w[0]));
		CHECK(nn.w[1] == doctest::Approx(h.w[1]));
		CHECK(nn.w[2] == doctest::Approx(h.w[2]));
		CHECK(nn.b == doctest::Approx(h.b));
	}

	TEST_CASE("is_strict for positive lex-leading sign") {
		hsb_halfspace h;
		h.w = {1.0, -1.0};
		CHECK(h.is_strict() == true);
	}

	TEST_CASE("is_strict for negative lex-leading sign") {
		hsb_halfspace h;
		h.w = {-1.0, 1.0};
		CHECK(h.is_strict() == false);
	}

	TEST_CASE("contains 2D: strict halfspace") {
		hsb_halfspace h;
		h.w = {1.0, 1.0};
		h.b = -2.0;
		CHECK(h.contains({0.0, 0.0}) == true);
		CHECK(h.contains({1.0, 1.0}) == false);
		CHECK(h.contains({2.0, 0.0}) == false);
	}

	TEST_CASE("to_string with 2D halfspace") {
		hsb_halfspace h;
		h.w = {1.0, -2.0};
		h.b = 3.0;
		std::string s = h.to_string();
		CHECK(s.find("x[0]") != std::string::npos);
		CHECK(s.find("x[1]") != std::string::npos);
	}

} // TEST_SUITE hsb_halfspace

TEST_SUITE("hsb — multi-dimensional") {

	TEST_CASE("3D conjunction is satisfiable") {
		auto a = make_hs({1.0, 0.0, 0.0}, -1.0);
		auto b = make_hs({0.0, 1.0, 0.0}, -1.0);
		auto c = make_hs({0.0, 0.0, 1.0}, -1.0);
		auto all = a & b & c;
		CHECK(is_hsb_zero(all) == false);
	}

	TEST_CASE("3D contradiction is empty") {
		auto a = make_hs({1.0, 0.0, 0.0}, 10.0);
		auto b = make_hs({-1.0, 0.0, 0.0}, 0.0);
		auto both = a & b;
		CHECK(is_hsb_zero(both) == true);
	}

	TEST_CASE("2D diagonal halfspace") {
		auto h = make_hs({1.0, 1.0}, -1.0);
		CHECK(h.root->hs.is_strict() == true);
		CHECK(is_hsb_zero(h) == false);
		CHECK(h.root->hs.contains({0.0, 0.0}) == true);
	}

	TEST_CASE("or of disjoint regions is non-empty") {
		auto left  = make_hs({1.0}, 5.0);
		auto right = make_hs({-1.0}, 5.0);
		auto both  = left | right;
		CHECK(is_hsb_zero(both) == false);
	}

	TEST_CASE("3D compound with diagonal") {
		auto a = make_hs({1.0, 1.0, 0.0}, -5.0);
		auto b = make_hs({0.0, 0.0, 1.0}, -5.0);
		CHECK(is_hsb_zero(a & b) == false);
	}

	TEST_CASE("4D with mixed normal vectors") {
		auto a = make_hs({1.0, -1.0, 0.0, 0.0}, 0.0);
		auto b = make_hs({0.0, 0.0, 1.0, -1.0}, 0.0);
		CHECK(is_hsb_zero(a & b) == false);
	}

} // TEST_SUITE multi-dimensional

TEST_SUITE("hsb — parser") {

	TEST_CASE("parse top") {
		auto r = parse_hsb<bas_pack>("top");
		REQUIRE(r.has_value());
		CHECK(std::get<hsb>(r->first) == hsb::top());
	}

	TEST_CASE("parse bot") {
		auto r = parse_hsb<bas_pack>("bot");
		REQUIRE(r.has_value());
		CHECK(std::get<hsb>(r->first) == hsb::bottom());
	}

	TEST_CASE("parse bottom") {
		auto r = parse_hsb<bas_pack>("bottom");
		REQUIRE(r.has_value());
		CHECK(std::get<hsb>(r->first) == hsb::bottom());
	}

	TEST_CASE("parse single constraint: x[0] < 0") {
		auto r = parse_hsb<bas_pack>("x[0] < 0");
		REQUIRE(r.has_value());
		auto h = std::get<hsb>(r->first);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("parse single constraint: x[0]*0.5 + 0.7 < 0") {
		auto r = parse_hsb<bas_pack>("x[0]*0.5 + 0.7 < 0");
		REQUIRE(r.has_value());
		auto h = std::get<hsb>(r->first);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("parse conjunction: x[0] < 0 & x[1] < 0") {
		auto r = parse_hsb<bas_pack>("(x[0] < 0 & x[1] < 0)");
		REQUIRE(r.has_value());
		auto h = std::get<hsb>(r->first);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("parse non-strict: x[0] <= 0") {
		auto r = parse_hsb<bas_pack>("x[0] <= 0");
		REQUIRE(r.has_value());
		auto h = std::get<hsb>(r->first);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("parse with negative coefficient: -1*x[0] < 0") {
		auto r = parse_hsb<bas_pack>("-1*x[0] < 0");
		REQUIRE(r.has_value());
		auto h = std::get<hsb>(r->first);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("parse with coefficient after variable: x[0]*2 < 0") {
		auto r = parse_hsb<bas_pack>("x[0]*2 < 0");
		REQUIRE(r.has_value());
		auto h = std::get<hsb>(r->first);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("parse with bias: x[0] + 5 < 0") {
		auto r = parse_hsb<bas_pack>("x[0] + 5 < 0");
		REQUIRE(r.has_value());
		auto h = std::get<hsb>(r->first);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("parse with negative bias: x[0] + -3 < 0") {
		auto r = parse_hsb<bas_pack>("x[0] + -3 < 0");
		REQUIRE(r.has_value());
		auto h = std::get<hsb>(r->first);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("parse with whitespace trimming") {
		auto r = parse_hsb<bas_pack>("  x[0] < 0  ");
		REQUIRE(r.has_value());
	}

	TEST_CASE("parse empty string returns nullopt") {
		auto r = parse_hsb<bas_pack>("");
		// Empty input is not a valid HSB formula
		CHECK(!r.has_value());
	}

	TEST_CASE("parse multivariate: 0.5*x[0] + -0.3*x[1] + 0.7 < 0") {
		auto r = parse_hsb<bas_pack>(
			"0.5*x[0] + -0.3*x[1] + 0.7 < 0");
		REQUIRE(r.has_value());
		auto h = std::get<hsb>(r->first);
		CHECK(is_hsb_zero(h) == false);
	}

	TEST_CASE("parse produces correct type tag") {
		auto r = parse_hsb<bas_pack>("top");
		REQUIRE(r.has_value());
		// The second element of the pair should be the hsb type reference
		// We just check it's not null
		CHECK(r->second != tref{});
	}

	TEST_CASE("parsed conjunction feasibility") {
		// Parse two constraints whose conjunction is feasible
		auto r = parse_hsb<bas_pack>(
			"(x[0] + 5 < 0 & x[0] + -10 <= 0)");
		REQUIRE(r.has_value());
		CHECK(is_hsb_zero(std::get<hsb>(r->first)) == false);
	}

	TEST_CASE("parse single variable no coefficient") {
		auto r = parse_hsb<bas_pack>("x[0] + -1 < 0");
		REQUIRE(r.has_value());
		CHECK(is_hsb_zero(std::get<hsb>(r->first)) == false);
	}

	TEST_CASE("parse large coefficient") {
		auto r = parse_hsb<bas_pack>("1000*x[0] + -500 < 0");
		REQUIRE(r.has_value());
		CHECK(is_hsb_zero(std::get<hsb>(r->first)) == false);
	}

	TEST_CASE("parse zero bias") {
		auto r = parse_hsb<bas_pack>("x[0] < 0");
		REQUIRE(r.has_value());
		CHECK(is_hsb_zero(std::get<hsb>(r->first)) == false);
	}

	TEST_CASE("parse bot returns bottom") {
		auto r = parse_hsb<bas_pack>("bot");
		REQUIRE(r.has_value());
		CHECK(std::get<hsb>(r->first) == hsb::bottom());
	}

} // TEST_SUITE parser

TEST_SUITE("hsb — dispatcher") {

	TEST_CASE("hsb type is recognized" * doctest::test_suite("hsb — dispatcher")) {
		gc_fixture gc;
		auto tt = hsb_type<node_t>();
		CHECK(is_hsb_type<node_t>(tt) == true);
	}

	TEST_CASE("hsb type is not qint type") {
		gc_fixture gc;
		auto tt = hsb_type<node_t>();
		CHECK(is_qint_type<node_t>(tt) == false);
	}

	TEST_CASE("ba_constants::get for hsb top") {
		gc_fixture gc;
		auto result = ba_constants<node_t>::get("top", hsb_type<node_t>());
		REQUIRE(result.has_value());
		CHECK(std::get<hsb>(result->first) == hsb::top());
	}

	TEST_CASE("ba_constants::get for hsb bot") {
		gc_fixture gc;
		auto result = ba_constants<node_t>::get("bot", hsb_type<node_t>());
		REQUIRE(result.has_value());
		CHECK(std::get<hsb>(result->first) == hsb::bottom());
	}

	TEST_CASE("dispatcher types() includes hsb") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		auto types = dispatcher_t::types();
		bool found = false;
		for (auto& t : types) if (t == "hsb") found = true;
		CHECK(found == true);
	}

	TEST_CASE("dispatcher one() returns top for hsb") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		auto s = dispatcher_t::one(hsb_type<node_t>());
		CHECK(s == "top");
	}

	TEST_CASE("dispatcher zero() returns bot for hsb") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		auto s = dispatcher_t::zero(hsb_type<node_t>());
		CHECK(s == "bot");
	}

	TEST_CASE("dispatcher is_syntactic_one for hsb top") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{hsb::top()};
		CHECK(dispatcher_t::is_syntactic_one(v) == true);
	}

	TEST_CASE("dispatcher is_syntactic_zero for hsb bot") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{hsb::bottom()};
		CHECK(dispatcher_t::is_syntactic_zero(v) == true);
	}

	TEST_CASE("dispatcher is_syntactic_one for hsb bot is false") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{hsb::bottom()};
		CHECK(dispatcher_t::is_syntactic_one(v) == false);
	}

	TEST_CASE("dispatcher is_syntactic_zero for hsb top is false") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{hsb::top()};
		CHECK(dispatcher_t::is_syntactic_zero(v) == false);
	}

	TEST_CASE("dispatcher normalize for hsb") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		// Create a contradictory element
		auto contra = make_hs({1.0}, 1.0) & make_hs({-1.0}, 2.0);
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{contra};
		auto result = dispatcher_t::normalize(v);
		CHECK(std::holds_alternative<hsb>(result));
		CHECK(std::get<hsb>(result) == hsb::bottom());
	}

	TEST_CASE("dispatcher splitter for hsb") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{hsb::top()};
		auto result = dispatcher_t::splitter(v, splitter_type::lower);
		CHECK(std::holds_alternative<hsb>(result));
		auto s = std::get<hsb>(result);
		CHECK(is_hsb_zero(s) == false);
		CHECK(is_hsb_one(s) == false);
	}

	TEST_CASE("dispatcher is_syntactic_one for halfspace") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		auto h = make_hs({1.0}, 0.0);
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{h};
		CHECK(dispatcher_t::is_syntactic_one(v) == false);
	}

	TEST_CASE("dispatcher is_syntactic_zero for halfspace") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		auto h = make_hs({1.0}, 0.0);
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{h};
		CHECK(dispatcher_t::is_syntactic_zero(v) == false);
	}

	TEST_CASE("dispatcher normalize preserves non-empty hsb") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		auto h = make_hs({1.0}, -5.0);
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{h};
		auto result = dispatcher_t::normalize(v);
		CHECK(std::holds_alternative<hsb>(result));
		CHECK(is_hsb_zero(std::get<hsb>(result)) == false);
	}

	TEST_CASE("dispatcher normalize of top is top") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{hsb::top()};
		auto result = dispatcher_t::normalize(v);
		CHECK(std::get<hsb>(result) == hsb::top());
	}

	TEST_CASE("dispatcher normalize of bot is bot") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{hsb::bottom()};
		auto result = dispatcher_t::normalize(v);
		CHECK(std::get<hsb>(result) == hsb::bottom());
	}

	TEST_CASE("dispatcher is_zero for top") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{hsb::top()};
		CHECK(dispatcher_t::is_zero(v) == false);
	}

	TEST_CASE("dispatcher is_zero for bot") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{hsb::bottom()};
		CHECK(dispatcher_t::is_zero(v) == true);
	}

	TEST_CASE("dispatcher is_one for top") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{hsb::top()};
		CHECK(dispatcher_t::is_one(v) == true);
	}

	TEST_CASE("dispatcher is_one for bot") {
		gc_fixture gc;
		using dispatcher_t = base_ba_dispatcher<qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
		std::variant<qint, qlt, nlang_ba, bv, sbf_ba, hsb> v{hsb::bottom()};
		CHECK(dispatcher_t::is_one(v) == false);
	}

} // TEST_SUITE dispatcher

// LTL realizability tests require ltlsynt (Spot >= 2.10) on PATH.
// ltlsynt is a required dependency — tests will fail if not installed.

TEST_SUITE("hsb — LTL integration") {

	TEST_CASE("parse hsb-typed constant in spec") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = {top}:hsb.");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("F(o1:hsb = {top}:hsb) is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec("F (o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(o1:hsb = {bot}:hsb) is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec("G (o1[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("o1:hsb = {top}:hsb is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = {top}:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("F(o1:hsb != {bot}:hsb) is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec("F (o1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(o1:hsb != {bot}:hsb) is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec("G (o1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(o1:hsb = {top}:hsb) is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec("G (o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("always (i:hsb != bot -> o:hsb != bot) is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec(
			"G (i1[t]:hsb != {bot}:hsb -> o1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(F(o1:hsb != {bot}:hsb)) is REALIZABLE (liveness)") {
		gc_fixture gc;
		tref fm = spec("G (F (o1[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(o1:hsb = {bot}:hsb) & F(o1:hsb != {bot}:hsb) is UNREALIZABLE") {
		gc_fixture gc;
		tref fm = spec(
			"G (o1[t]:hsb = {bot}:hsb) && F (o1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("F(o1:hsb = {top}:hsb) is trivially REALIZABLE") {
		gc_fixture gc;
		tref fm = spec("F (o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("two hsb outputs") {
		gc_fixture gc;
		tref fm = spec(
			"o1[t]:hsb = {top}:hsb && o2[t]:hsb = {top}:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("i:hsb != bot -> o:hsb = top") {
		gc_fixture gc;
		tref fm = spec(
			"i1[t]:hsb != {bot}:hsb -> o1[t]:hsb = {top}:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("always output equals top") {
		gc_fixture gc;
		tref fm = spec("G (o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("eventually output equals top") {
		gc_fixture gc;
		tref fm = spec("F (o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("always output not bot (safety)") {
		gc_fixture gc;
		tref fm = spec("G (o1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("simple implication with hsb") {
		gc_fixture gc;
		tref fm = spec(
			"G (i1[t]:hsb = {top}:hsb -> o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("hsb output with disjunction") {
		gc_fixture gc;
		tref fm = spec(
			"o1[t]:hsb = {top}:hsb || o1[t]:hsb = {bot}:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(F(o:hsb = top)) liveness") {
		gc_fixture gc;
		tref fm = spec("G (F (o1[t]:hsb = {top}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("unrealizable: G(i = top) & G(o = bot) -> contradiction") {
		gc_fixture gc;
		// System must output bot, but input is always top and must respond
		// This is realizable since system can always output bot
		tref fm = spec("G (o1[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("output equality to top is realizable") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = {top}:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("output equality to bot is realizable") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = {bot}:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("always implies eventually") {
		gc_fixture gc;
		tref fm = spec("G (o1[t]:hsb = {top}:hsb) -> F (o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("conjunction of always and eventually: o = top forever") {
		gc_fixture gc;
		tref fm = spec(
			"G (o1[t]:hsb = {top}:hsb) && F (o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("two output variables always equal") {
		gc_fixture gc;
		tref fm = spec(
			"G (o1[t]:hsb = o2[t]:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("negation of unrealizable is realizable") {
		gc_fixture gc;
		tref fm = spec("F (o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("always: input implies output response") {
		gc_fixture gc;
		tref fm = spec(
			"G (i1[t]:hsb != {bot}:hsb -> o1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("hsb constant not equal to bot in spec") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb != {bot}:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("multiple temporal operators: GF with hsb") {
		gc_fixture gc;
		tref fm = spec("G (F (o1[t]:hsb = {top}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("always output not equal to top (possible)") {
		gc_fixture gc;
		tref fm = spec("G (o1[t]:hsb != {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("simple hsb assignment") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = {top}:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("contradiction: o = top AND o = bot is UNREALIZABLE") {
		gc_fixture gc;
		tref fm = spec(
			"o1[t]:hsb = {top}:hsb && o1[t]:hsb = {bot}:hsb.");
		REQUIRE(fm != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("disjunction: o = top OR o = bot is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec(
			"o1[t]:hsb = {top}:hsb || o1[t]:hsb = {bot}:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("nested always-eventually pattern") {
		gc_fixture gc;
		tref fm = spec("G (F (o1[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("safety spec: always not bot") {
		gc_fixture gc;
		tref fm = spec("G (o1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("liveness spec: eventually top") {
		gc_fixture gc;
		tref fm = spec("F (o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("response: i != bot implies F(o != bot)") {
		gc_fixture gc;
		tref fm = spec(
			"G (i1[t]:hsb != {bot}:hsb -> F (o1[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("two outputs: G(o1 = top) and G(o2 = top)") {
		gc_fixture gc;
		tref fm = spec(
			"G (o1[t]:hsb = {top}:hsb) && G (o2[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("output tracks input: G(o1 = i1)") {
		gc_fixture gc;
		tref fm = spec("G (o1[t]:hsb = i1[t]:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("output differs from bot whenever input is top") {
		gc_fixture gc;
		tref fm = spec(
			"G (i1[t]:hsb = {top}:hsb -> o1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("unrealizable: G(o = top) and G(o != top)") {
		gc_fixture gc;
		tref fm = spec(
			"G (o1[t]:hsb = {top}:hsb) && G (o1[t]:hsb != {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("F(o1 = top) and F(o1 = bot) — both eventualities achievable") {
		gc_fixture gc;
		tref fm = spec(
			"F (o1[t]:hsb = {top}:hsb) && F (o1[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(o1 != bot || o2 != bot) — at least one output non-bot") {
		gc_fixture gc;
		tref fm = spec(
			"G (o1[t]:hsb != {bot}:hsb || o2[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("GF(o = top) and GF(o = bot) — infinitely often alternating") {
		gc_fixture gc;
		tref fm = spec(
			"G (F (o1[t]:hsb = {top}:hsb)) && G (F (o1[t]:hsb = {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

} // TEST_SUITE LTL integration

TEST_SUITE("hsb — full pipeline constraint parsing") {

	//
	// Integration tests verifying that full hsb constraint syntax (not just
	// {top}/{bot}) is parsed end-to-end through the grammar pipeline:
	//   spec string → grammar (ba_constant/source) → dispatcher → parse_hsb → hsb

	TEST_CASE("pipeline: o1[t]:hsb = { x[0] < 0 }:hsb parses") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = { x[0] < 0 }:hsb.");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("pipeline: G (o1[t]:hsb != { x[0]*0.5 + 0.7 < 0 }:hsb) parses") {
		gc_fixture gc;
		tref fm = spec(
			"G (o1[t]:hsb != { x[0]*0.5 + 0.7 < 0 }:hsb).");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("pipeline: conjunction { (x[0] < 0 & x[1] <= 0) }:hsb parses") {
		gc_fixture gc;
		tref fm = spec(
			"o1[t]:hsb = { (x[0] < 0 & x[1] <= 0) }:hsb.");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("pipeline: multivariate { 0.5*x[0] + -0.3*x[1] + 0.7 < 0 }:hsb parses") {
		gc_fixture gc;
		tref fm = spec(
			"o1[t]:hsb = { 0.5*x[0] + -0.3*x[1] + 0.7 < 0 }:hsb.");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("pipeline: constraint equality is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = { x[0] < 0 }:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("pipeline: G with constraint inequality is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec(
			"G (o1[t]:hsb != { x[0]*0.5 + 0.7 < 0 }:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("pipeline: non-strict inequality { x[0] <= 0 }:hsb parses") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = { x[0] <= 0 }:hsb.");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("pipeline: negative coefficient { -1*x[0] < 0 }:hsb parses") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = { -1*x[0] < 0 }:hsb.");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("pipeline: constraint with bias { x[0] + 5 < 0 }:hsb parses") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = { x[0] + 5 < 0 }:hsb.");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("pipeline: constraint compared against top is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec(
			"o1[t]:hsb = { x[0] < 0 }:hsb && "
			"o1[t]:hsb != {top}:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("pipeline: mixed constraint and top/bot constants") {
		gc_fixture gc;
		tref fm = spec(
			"G (o1[t]:hsb != {bot}:hsb -> "
			"o2[t]:hsb = { x[0] < 0 }:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("pipeline: 3-variable constraint parses") {
		gc_fixture gc;
		tref fm = spec(
			"o1[t]:hsb = { x[0] + x[1] + x[2] < 0 }:hsb.");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("pipeline: coefficient after variable { x[0]*2 < 0 }:hsb parses") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = { x[0]*2 < 0 }:hsb.");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("pipeline: constraint with negative bias { x[0] + -3 < 0 }:hsb parses") {
		gc_fixture gc;
		tref fm = spec("o1[t]:hsb = { x[0] + -3 < 0 }:hsb.");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("pipeline: temporal spec with constraint literal is REALIZABLE") {
		gc_fixture gc;
		tref fm = spec(
			"F (o1[t]:hsb = { x[0] < 0 }:hsb) && "
			"G (o1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("pipeline: two constraint constants in one spec") {
		gc_fixture gc;
		tref fm = spec(
			"o1[t]:hsb = { x[0] < 0 }:hsb && "
			"o2[t]:hsb = { x[1] < 0 }:hsb.");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

} // TEST_SUITE full pipeline constraint parsing

TEST_SUITE("hsb — joint LTL(hsb, bv) specs") {

	//
	// Tests for the unified LTL(hsb, bv) architecture where both bv (bitvector)
	// and hsb (halfspaces) are first-class types in the same formula.
	// This is critical for Mode A (full synthesis): tokens are bv[K], activations
	// are hsb in R^d, and the synthesis pipeline handles both jointly.
	//
	// Ref: llm-is-tau.md §1: "A transformer is a quantified LTL(hsb, bv) formula."
	// Ref: re-evaluation §Stage 2: "The synthesis pipeline is mode-agnostic."

	// --- F(hsb) && F(bv) simple conjunction ---

	TEST_CASE("F(o1:hsb=top) && F(o2:bv=0b10110101) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"F (o1[t]:hsb = {top}:hsb) && F (o2[t]:bv = {#b10110101}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("F(o1:hsb!=bot) && F(o2:bv=255) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"F (o1[t]:hsb != {bot}:hsb) && F (o2[t]:bv = {255}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- G(hsb) && F(bv) ---

	TEST_CASE("G(o1:hsb=top) && F(o2:bv=0b00001111) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o1[t]:hsb = {top}:hsb) && F (o2[t]:bv = {#b00001111}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- G(bv) && F(hsb) ---

	TEST_CASE("G(o1:bv=0b11111111) && F(o2:hsb!=bot) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o1[t]:bv = {#b11111111}:bv) && F (o2[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Safety pattern with mixed types ---

	TEST_CASE("G(i:hsb!=bot -> o:bv=0b10110101) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i1[t]:hsb != {bot}:hsb -> o1[t]:bv = {#b10110101}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(i:bv=255 -> o:hsb=top) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i1[t]:bv = {255}:bv -> o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Liveness with mixed types ---

	TEST_CASE("GF(o1:hsb!=bot) && GF(o2:bv!=0) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o1[t]:hsb != {bot}:hsb)) && "
			"G (F (o2[t]:bv != {0}:bv)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Unrealizable mixed-type spec ---

	TEST_CASE("G(o:hsb=top) && G(o:hsb=bot) UNREALIZABLE — same output contradicts") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o1[t]:hsb = {top}:hsb) && G (o1[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
	}

	// --- Three types: hsb + bv + sbf ---

	TEST_CASE("F(hsb) && F(bv) && F(sbf) three-type conjunction REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"F (o1[t]:hsb = {top}:hsb) && "
			"F (o2[t]:bv = {#b10110101}:bv) && "
			"F (o3[t]:sbf = {X & Y}:sbf).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Multi-output mixed-type: two hsb + one bv ---

	TEST_CASE("G(o1:hsb=top) && G(o2:hsb=top) && F(o3:bv=255) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o1[t]:hsb = {top}:hsb) && "
			"G (o2[t]:hsb = {top}:hsb) && "
			"F (o3[t]:bv = {255}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Mixed-type response pattern ---

	TEST_CASE("G(i:bv!=0 -> F(o:hsb!=bot)) REALIZABLE — response pattern") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i1[t]:bv != {0}:bv -> F (o1[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Mode A spec-model pattern: output token + admissibility ---

	TEST_CASE("G(o_token:bv=0b10110101) && G(o_admit:hsb!=bot) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o1[t]:bv = {#b10110101}:bv) && "
			"G (o2[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Temporal: alternating hsb and bv liveness ---

	TEST_CASE("GF(o1:hsb=top) && GF(o2:bv=0b11111111) alternating REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o1[t]:hsb = {top}:hsb)) && "
			"G (F (o2[t]:bv = {#b11111111}:bv)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Input hsb, output bv: the constrained-decoding pattern ---

	TEST_CASE("G(i_embed:hsb!=bot -> o_tok:bv!=0) constrained decoding REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i1[t]:hsb != {bot}:hsb -> o1[t]:bv != {0}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Input bv, output hsb: spec-side hsb constraint ---

	TEST_CASE("G(i_tok:bv=255 -> o_admissibility:hsb=top) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i1[t]:bv = {255}:bv -> o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Disjunction across types ---

	TEST_CASE("G(o:hsb=top || o2:bv=255) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o1[t]:hsb = {top}:hsb || o2[t]:bv = {255}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Mixed hsb + qint (dyadic rationals in 1D) ---

	TEST_CASE("F(o:hsb=top) && F(o2:qint=[0,1)) REALIZABLE") {
		gc_fixture gc;
		tref fm = spec(
			"F (o1[t]:hsb = {top}:hsb) && "
			"F (o2[t]:qint = {[0, 1)}:qint).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Four types: hsb + bv + sbf + qint ---

	TEST_CASE("F(hsb) && F(bv) && F(sbf) && F(qint) four-type REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"F (o1[t]:hsb = {top}:hsb) && "
			"F (o2[t]:bv = {#b10110101}:bv) && "
			"F (o3[t]:sbf = {X}:sbf) && "
			"F (o4[t]:qint = {[0, 1)}:qint).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Until with mixed types ---

	TEST_CASE("(o:hsb=top) U (o2:bv=255) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"(o1[t]:hsb = {top}:hsb) U (o2[t]:bv = {255}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Multiple bv widths + hsb ---

	TEST_CASE("F(o1:bv8=255) && F(o2:hsb=top) && F(o3:bv=0b1) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"F (o1[t]:bv = {255}:bv) && "
			"F (o2[t]:hsb = {top}:hsb) && "
			"F (o3[t]:bv = {#b1}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

} // TEST_SUITE joint LTL(hsb, bv) specs

TEST_SUITE("hsb — nested temporal LTL(hsb) specs") {

	//
	// Tests modeling the str-LTL two-axis temporal pattern from str-ltl.tex.
	// The inner/outer axes are encoded via a tick segmentation marker (sbf-typed)
	// that divides the trace into "macro steps" (sentences/turns).
	//
	// Pattern:
	//   o_tick : sbf — marks sentence boundaries
	//   o_token : bv — token-level output (inner)
	//   o_admit : hsb — admissibility region (inner/outer)
	//
	// Ref: str-ltl.tex §3: tick-segmented reduction
	// Ref: llm-is-tau.md §2: two temporal axes
	// Ref: tau_neuro.tex Definition 17: two-cadence Mealy automaton

	// --- Frame formula: ticks happen infinitely often ---

	TEST_CASE("GF(tick) frame formula REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o_tick[t]:sbf = {tick}:sbf)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Tick-guarded hsb: at every tick, admissibility is nontrivial ---

	TEST_CASE("G(tick -> admit!=bot) tick-guarded constraint REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_tick[t]:sbf = {tick}:sbf -> o_admit[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Tick-guarded bv: at every tick, output token is nonzero ---

	TEST_CASE("G(tick -> token!=0) tick-guarded bv REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_tick[t]:sbf = {tick}:sbf -> o_token[t]:bv != {0}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Combined frame + constraint: two-axis spec ---

	TEST_CASE("GF(tick) && G(tick -> admit!=bot) two-axis REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o_tick[t]:sbf = {tick}:sbf)) && "
			"G (o_tick[t]:sbf = {tick}:sbf -> o_admit[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Full two-axis with bv + hsb: frame + inner + outer ---

	TEST_CASE("GF(tick) && G(tick->admit!=bot) && G(token!=0->admit!=bot) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o_tick[t]:sbf = {tick}:sbf)) && "
			"G (o_tick[t]:sbf = {tick}:sbf -> o_admit[t]:hsb != {bot}:hsb) && "
			"G (o_token[t]:bv != {0}:bv -> o_admit[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Two-point annotation pattern: Point A (input) + Point B (output) ---
	// Ref: tau_neuro.tex Definition 16: B = B_A x B_B

	TEST_CASE("G(i_embed:hsb!=bot -> o_admit:hsb!=bot) && GF(tick) two-point REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_embed[t]:hsb != {bot}:hsb -> o_admit[t]:hsb != {bot}:hsb) && "
			"G (F (o_tick[t]:sbf = {tick}:sbf)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Constrained decoding pattern: token gated by hsb admissibility ---

	TEST_CASE("G(admit=bot -> token=0) constrained decoding REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_admit[t]:hsb = {bot}:hsb -> o_token[t]:bv = {0}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Safety: never output token when inadmissible ---

	TEST_CASE("G(~admit -> ~token) safety constraint REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_admit[t]:hsb = {bot}:hsb -> o_token[t]:bv = {0}:bv) && "
			"G (F (o_admit[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Liveness: eventually produce a nonzero token after each tick ---

	TEST_CASE("G(tick -> F(token!=0)) post-tick liveness REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_tick[t]:sbf = {tick}:sbf -> F (o_token[t]:bv != {0}:bv)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Response: input admissibility implies eventual output token ---

	TEST_CASE("G(i:hsb!=bot -> F(o:bv!=0)) input-to-output response REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i1[t]:hsb != {bot}:hsb -> F (o1[t]:bv != {0}:bv)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Multi-stream two-axis: tick + embed(hsb) + token(bv) + admit(hsb) ---

	TEST_CASE("four-stream two-axis spec REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o_tick[t]:sbf = {tick}:sbf)) && "
			"G (o_tick[t]:sbf = {tick}:sbf -> o_admit[t]:hsb != {bot}:hsb) && "
			"G (o_token[t]:bv != {0}:bv -> o_admit[t]:hsb != {bot}:hsb) && "
			"G (i_embed[t]:hsb != {bot}:hsb -> o_token[t]:bv != {0}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Unrealizable two-axis: contradictory tick constraints ---

	TEST_CASE("G(tick=X) && G(tick!=X) contradictory ticks UNREALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_tick[t]:sbf = {tick}:sbf) && "
			"G (o_tick[t]:sbf != {tick}:sbf).");
		REQUIRE(fm != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
	}

	// --- Until across types: admissible until tick ---

	TEST_CASE("(admit!=bot) U (tick) mixed-axis until REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"(o_admit[t]:hsb != {bot}:hsb) U (o_tick[t]:sbf = {tick}:sbf).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Accumulator-like pattern: count via alternating ---

	TEST_CASE("GF(token=255) && GF(token=0) alternating bv REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o_token[t]:bv = {255}:bv)) && "
			"G (F (o_token[t]:bv = {0}:bv)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

} // TEST_SUITE two-axis temporal patterns

TEST_SUITE("hsb — multi-variable LTL(hsb) specs") {

	//
	// Tests validating the infrastructure needed for BA-Hom (ba_hom.md):
	// an injective BA homomorphism h : NL → BA(hsb) constructed live during
	// chat via Sikorski extension over MUS-enumerated contradiction structure.
	//
	// P5 (Sikorski criterion): for every MUS {g1,...,gk} with conjunction = ⊥
	// in BA_NL, we need h(g1) ∧ ... ∧ h(gk) = ⊥ in BA(hsb). This requires
	// is_hsb_zero on conjunctions of hsb atoms.
	//
	// P3 (Contradiction testing): conjunction satisfiability of multiple hsb
	// atoms — is_hsb_zero returns false iff the conjunction is satisfiable.
	//
	// Ref: ba_hom.md §8 (P5: Sikorski extension)
	// Ref: ba_hom.md §6 (P3: Contradiction testing)
	// Ref: LP_d_CONSTRUCTION.md (atomless BA, emptiness decidable by LP)

	// =====================================================================
	// P5: Sikorski criterion — conjunction emptiness
	// =====================================================================

	// --- Pairwise contradictions (2-element MUS) ---

	TEST_CASE("Conjunction emptiness: h(p) ∧ h(¬p) = ⊥ (complementary halfspaces)") {
		gc_fixture gc;
		auto hp = make_hs({1.0}, 0.0);     // x < 0
		auto hnp = ~hp;                     // complement: x >= 0
		CHECK(is_hsb_zero(hp & hnp));
	}

	TEST_CASE("Conjunction emptiness: h(p) ∧ h(q) = ⊥ (contradictory 1D atoms)") {
		gc_fixture gc;
		auto hp = make_hs({1.0}, 1.0);     // x + 1 < 0 → x < -1
		auto hq = make_hs({-1.0}, 2.0);    // -x + 2 ≤ 0 → x ≥ 2
		CHECK(is_hsb_zero(hp & hq));
	}

	TEST_CASE("Conjunction emptiness: h(p) ∧ h(q) ≠ ⊥ (compatible 1D atoms)") {
		gc_fixture gc;
		auto hp = make_hs({1.0}, 0.0);     // x < 0
		auto hq = make_hs({1.0}, 1.0);     // x + 1 < 0 → x < -1
		CHECK_FALSE(is_hsb_zero(hp & hq)); // x < -1 is nonempty
	}

	TEST_CASE("Conjunction emptiness: 2D pairwise — contradictory halfplanes") {
		gc_fixture gc;
		auto hp = make_hs({1.0, 0.0}, 5.0);   // x1 + 5 < 0 → x1 < -5
		auto hq = make_hs({-1.0, 0.0}, 10.0);  // -x1 + 10 ≤ 0 → x1 ≥ 10
		CHECK(is_hsb_zero(hp & hq));
	}

	TEST_CASE("Conjunction emptiness: 2D pairwise — compatible halfplanes") {
		gc_fixture gc;
		auto hp = make_hs({1.0, 0.0}, -1.0);  // x1 - 1 < 0 → x1 < 1
		auto hq = make_hs({0.0, 1.0}, -1.0);  // x2 - 1 < 0 → x2 < 1
		CHECK_FALSE(is_hsb_zero(hp & hq));    // quadrant x1<1, x2<1 is nonempty
	}

	// --- 3-element MUS (triple contradiction) ---

	TEST_CASE("Conjunction emptiness: 3-element MUS in 1D") {
		gc_fixture gc;
		auto h1 = make_hs({1.0}, 0.0);     // x < 0
		auto h2 = make_hs({-1.0}, 0.0);    // -x ≤ 0 → x ≥ 0 (complement of x<0 isn't quite this)
		auto h3 = make_hs({1.0}, -1.0);    // x - 1 < 0 → x < 1
		// h1 ∧ h2: x < 0 AND x ≥ 0 → empty (this is already a 2-MUS)
		CHECK(is_hsb_zero(h1 & h2));
		// h1 ∧ h3: x < 0 AND x < 1 → x < 0 (nonempty)
		CHECK_FALSE(is_hsb_zero(h1 & h3));
	}

	TEST_CASE("Conjunction emptiness: 3-element MUS in 2D — pairwise contradictions") {
		gc_fixture gc;
		// h1: x1 < -1 (lex +1, strict)
		// h2: x1 ≥ 2  (lex -1, non-strict)
		auto h1 = make_hs({1.0, 0.0}, 1.0);   // x1 + 1 < 0 → x1 < -1
		auto h2 = make_hs({-1.0, 0.0}, 2.0);  // -x1 + 2 ≤ 0 → x1 ≥ 2
		auto h3 = make_hs({0.0, 1.0}, 0.0);   // x2 < 0
		// h1 ∧ h2: x1 < -1 AND x1 ≥ 2 → empty
		CHECK(is_hsb_zero(h1 & h2));
		// h1 ∧ h2 ∧ h3: also empty
		CHECK(is_hsb_zero(h1 & h2 & h3));
	}

	TEST_CASE("Conjunction emptiness: 3 compatible atoms — nonempty triple conjunction") {
		gc_fixture gc;
		auto h1 = make_hs({1.0, 0.0}, 0.0);   // x1 < 0
		auto h2 = make_hs({0.0, 1.0}, 0.0);   // x2 < 0
		auto h3 = make_hs({1.0, 1.0}, 0.0);   // x1 + x2 < 0
		// All three compatible in the third quadrant
		CHECK_FALSE(is_hsb_zero(h1 & h2 & h3));
	}

	// --- 4-element and 5-element MUS (bounded k_max = 5 from ba_hom.md §7.2) ---

	TEST_CASE("Conjunction emptiness: 4-element conjunction in 2D") {
		gc_fixture gc;
		// Box: -5 < x1 < 5 AND -5 < x2 < 5
		auto h1 = make_hs({1.0, 0.0}, -5.0);   // x1 < 5
		auto h2 = make_hs({-1.0, 0.0}, -5.0);  // x1 > -5 (lex: -x1 -5 ≤ 0)
		auto h3 = make_hs({0.0, 1.0}, -5.0);   // x2 < 5
		auto h4 = make_hs({0.0, -1.0}, -5.0);  // x2 > -5
		CHECK_FALSE(is_hsb_zero(h1 & h2 & h3 & h4)); // box is nonempty
	}

	TEST_CASE("Conjunction emptiness: 4-element contradictory conjunction in 2D") {
		gc_fixture gc;
		auto h1 = make_hs({1.0, 0.0}, 0.0);   // x1 < 0
		auto h2 = make_hs({-1.0, 0.0}, 1.0);  // -x1 + 1 ≤ 0 → x1 ≥ 1
		auto h3 = make_hs({0.0, 1.0}, 0.0);   // x2 < 0
		auto h4 = make_hs({0.0, -1.0}, 1.0);  // -x2 + 1 ≤ 0 → x2 ≥ 1
		// h1 ∧ h2 already empty (x1 < 0 AND x1 ≥ 1)
		CHECK(is_hsb_zero(h1 & h2 & h3 & h4));
	}

	TEST_CASE("Conjunction emptiness: 5-element conjunction (k_max boundary)") {
		gc_fixture gc;
		// 5 compatible halfplanes forming a nonempty pentagonal region
		auto h1 = make_hs({1.0, 0.0}, -10.0);   // x1 < 10
		auto h2 = make_hs({-1.0, 0.0}, -10.0);  // x1 > -10
		auto h3 = make_hs({0.0, 1.0}, -10.0);   // x2 < 10
		auto h4 = make_hs({0.0, -1.0}, -10.0);  // x2 > -10
		auto h5 = make_hs({1.0, 1.0}, -15.0);   // x1 + x2 < 15
		CHECK_FALSE(is_hsb_zero(h1 & h2 & h3 & h4 & h5));
	}

	// =====================================================================
	// P3: Contradiction testing — conjunction satisfiability
	// =====================================================================

	TEST_CASE("P3: two satisfiable atoms — conjunction nonempty") {
		gc_fixture gc;
		auto a = make_hs({1.0}, -5.0);   // x < 5
		auto b = make_hs({-1.0}, 0.0);   // x ≥ 0
		CHECK_FALSE(is_hsb_zero(a & b)); // 0 ≤ x < 5 is nonempty
	}

	TEST_CASE("P3: two unsatisfiable atoms — conjunction empty") {
		gc_fixture gc;
		auto a = make_hs({1.0}, 5.0);    // x + 5 < 0 → x < -5
		auto b = make_hs({-1.0}, 10.0);  // -x + 10 ≤ 0 → x ≥ 10
		CHECK(is_hsb_zero(a & b));
	}

	TEST_CASE("P3: 3D contradiction — parallel planes squeezing to empty") {
		gc_fixture gc;
		// x1 < -100 AND x1 >= 100 (regardless of x2, x3)
		auto a = make_hs({1.0, 0.0, 0.0}, 100.0);    // x1 + 100 < 0 → x1 < -100
		auto b = make_hs({-1.0, 0.0, 0.0}, 100.0);   // -x1 + 100 ≤ 0 → x1 ≥ 100
		CHECK(is_hsb_zero(a & b));
	}

	TEST_CASE("P3: disjunction satisfiability — at least one branch nonempty") {
		gc_fixture gc;
		auto a = make_hs({1.0}, 1.0);    // x < -1 (nonempty)
		auto b = make_hs({-1.0}, 2.0);   // -x + 2 ≤ 0 → x ≥ 2 (nonempty)
		auto disj = a | b;
		CHECK_FALSE(is_hsb_zero(disj));  // union of two nonempty sets
	}

	TEST_CASE("P3: mixed polarity — h(p) ∧ ¬h(q) satisfiability") {
		gc_fixture gc;
		auto hp = make_hs({1.0, 0.0}, -5.0);  // x1 < 5
		auto hq = make_hs({0.0, 1.0}, -3.0);  // x2 < 3
		auto mixed = hp & (~hq);               // x1 < 5 AND x2 ≥ 3
		CHECK_FALSE(is_hsb_zero(mixed));       // nonempty
	}

	TEST_CASE("P3: mixed polarity — h(p) ∧ ¬h(p) always empty") {
		gc_fixture gc;
		auto hp = make_hs({1.0, 0.0}, -5.0);
		CHECK(is_hsb_zero(hp & (~hp)));
	}

	// =====================================================================
	// BA homomorphism properties — h commutes with Boolean ops
	// =====================================================================
	// These tests verify that BA(hsb) operations satisfy the homomorphism
	// axioms needed by ba_hom.md Property 1.

	TEST_CASE("BA-hom P1: h(⊤) = ⊤_hsb") {
		gc_fixture gc;
		CHECK(is_hsb_one(hsb::top()));
		CHECK_FALSE(is_hsb_zero(hsb::top()));
	}

	TEST_CASE("BA-hom P1: h(⊥) = ⊥_hsb") {
		gc_fixture gc;
		CHECK(is_hsb_zero(hsb::bottom()));
		CHECK_FALSE(is_hsb_one(hsb::bottom()));
	}

	TEST_CASE("BA-hom P1: ¬h(p) = h(¬p) — complement involution") {
		gc_fixture gc;
		auto hp = make_hs({1.0, 0.0}, -3.0);  // x1 < 3
		auto neg_hp = ~hp;                      // x1 ≥ 3
		// Double complement = identity
		CHECK(is_hsb_zero((~(~hp)) ^ hp));     // symmetric diff is zero
	}

	TEST_CASE("BA-hom P1: h(p ∧ q) = h(p) ∧ h(q) — conjunction preservation") {
		gc_fixture gc;
		auto hp = make_hs({1.0, 0.0}, 0.0);   // x1 < 0
		auto hq = make_hs({0.0, 1.0}, 0.0);   // x2 < 0
		auto conj = hp & hq;
		// Conjunction should equal intersection: third quadrant
		CHECK_FALSE(is_hsb_zero(conj));
		// Verify it's strictly smaller than either operand
		CHECK_FALSE(is_hsb_zero(hp & (~conj))); // hp has points not in conj
		CHECK_FALSE(is_hsb_zero(hq & (~conj))); // hq has points not in conj
	}

	TEST_CASE("BA-hom P1: h(p ∨ q) = h(p) ∨ h(q) — disjunction preservation") {
		gc_fixture gc;
		auto hp = make_hs({1.0, 0.0}, 0.0);   // x1 < 0
		auto hq = make_hs({0.0, 1.0}, 0.0);   // x2 < 0
		auto disj = hp | hq;
		// Disjunction should contain both operands
		CHECK(is_hsb_zero(hp & (~disj)));      // hp ⊆ disj
		CHECK(is_hsb_zero(hq & (~disj)));      // hq ⊆ disj
		// Disjunction should be strictly larger than conjunction
		auto conj = hp & hq;
		CHECK_FALSE(is_hsb_zero(disj & (~conj))); // disj has points not in conj
	}

	TEST_CASE("BA-hom P1: De Morgan — ¬(h(p) ∧ h(q)) = ¬h(p) ∨ ¬h(q)") {
		gc_fixture gc;
		auto hp = make_hs({1.0, 0.0}, -2.0);
		auto hq = make_hs({0.0, 1.0}, -3.0);
		auto lhs = ~(hp & hq);
		auto rhs = (~hp) | (~hq);
		CHECK(is_hsb_zero(lhs ^ rhs));  // semantically equal
	}

	// =====================================================================
	// Coherence enforcement — h(P) ∧ commitments consistency
	// =====================================================================
	// Ref: ba_hom.md §10.5: when the model asserts P, check h(P) ∧ commitments ≠ ⊥.

	TEST_CASE("Coherence: single commitment compatible with new assertion") {
		gc_fixture gc;
		auto commitment = make_hs({1.0, 0.0}, -5.0); // x1 < 5
		auto new_assert = make_hs({0.0, 1.0}, -3.0);  // x2 < 3
		CHECK_FALSE(is_hsb_zero(commitment & new_assert)); // compatible
	}

	TEST_CASE("Coherence: single commitment contradicts new assertion") {
		gc_fixture gc;
		auto commitment = make_hs({1.0}, 5.0);   // x + 5 < 0 → x < -5
		auto new_assert = make_hs({-1.0}, 10.0);  // -x + 10 ≤ 0 → x ≥ 10
		CHECK(is_hsb_zero(commitment & new_assert)); // contradictory
	}

	TEST_CASE("Coherence: accumulated commitments — 3 compatible, then contradiction") {
		gc_fixture gc;
		// Accumulate 3 compatible commitments
		auto c1 = make_hs({1.0, 0.0}, -10.0);  // x1 < 10
		auto c2 = make_hs({-1.0, 0.0}, -5.0);  // x1 > -5  (lex: -x1-5 ≤ 0)
		auto c3 = make_hs({0.0, 1.0}, -10.0);  // x2 < 10
		auto accumulated = c1 & c2 & c3;
		CHECK_FALSE(is_hsb_zero(accumulated)); // -5 < x1 < 10, x2 < 10

		// New assertion compatible
		auto p_ok = make_hs({0.0, -1.0}, -5.0); // x2 > -5
		CHECK_FALSE(is_hsb_zero(accumulated & p_ok));

		// New assertion contradictory (x1 < -100, but commitments say x1 > -5)
		auto p_bad = make_hs({1.0, 0.0}, 100.0); // x1 < -100
		CHECK(is_hsb_zero(accumulated & p_bad));
	}

	TEST_CASE("Coherence: negated commitment — previously denied proposition") {
		gc_fixture gc;
		// Commitment: ¬p (model previously denied p)
		auto hp = make_hs({1.0, 0.0}, -3.0);   // p: x1 < 3
		auto commitment = ~hp;                   // ¬p: x1 ≥ 3
		// New assertion: p (model now asserts p — contradiction!)
		CHECK(is_hsb_zero(commitment & hp));
	}

	TEST_CASE("Coherence: 4 accumulated commitments forming a box") {
		gc_fixture gc;
		auto c1 = make_hs({1.0, 0.0}, -5.0);   // x1 < 5
		auto c2 = make_hs({-1.0, 0.0}, -1.0);  // x1 > -1 (lex: -x1-1 ≤ 0)
		auto c3 = make_hs({0.0, 1.0}, -5.0);   // x2 < 5
		auto c4 = make_hs({0.0, -1.0}, -1.0);  // x2 > -1
		auto box = c1 & c2 & c3 & c4;
		CHECK_FALSE(is_hsb_zero(box));  // box (-1,5) x (-1,5) is nonempty

		// Interior point assertion — should be compatible
		auto interior = make_hs({1.0, 0.0}, -2.0) & make_hs({0.0, 1.0}, -2.0);
		// x1 < 2 AND x2 < 2 — overlaps with box
		CHECK_FALSE(is_hsb_zero(box & interior));
	}

	// =====================================================================
	// MUS-enumerated structure — testing MUS properties via is_hsb_zero
	// =====================================================================
	// Ref: ba_hom.md §2.5: MUS = minimal unsatisfiable subset

	TEST_CASE("MUS: minimal — removing any element makes it satisfiable") {
		gc_fixture gc;
		auto h1 = make_hs({1.0}, 0.0);     // x < 0
		auto h2 = make_hs({-1.0}, 1.0);    // -x + 1 ≤ 0 → x ≥ 1
		// {h1, h2} is a 2-MUS:
		CHECK(is_hsb_zero(h1 & h2));       // conjunction empty
		CHECK_FALSE(is_hsb_zero(h1));      // each alone nonempty
		CHECK_FALSE(is_hsb_zero(h2));
	}

	TEST_CASE("MUS: 2-MUS complement pair") {
		gc_fixture gc;
		auto h = make_hs({1.0, 0.0, 0.0}, -5.0); // x1 < 5
		auto nh = ~h;                               // x1 ≥ 5
		// {h, ¬h} is always a 2-MUS
		CHECK(is_hsb_zero(h & nh));
		CHECK_FALSE(is_hsb_zero(h));
		CHECK_FALSE(is_hsb_zero(nh));
	}

	TEST_CASE("MUS: non-MUS — proper subset already unsatisfiable") {
		gc_fixture gc;
		auto h1 = make_hs({1.0}, 0.0);     // x < 0
		auto h2 = make_hs({-1.0}, 1.0);    // -x + 1 ≤ 0 → x ≥ 1
		auto h3 = make_hs({1.0}, -5.0);    // x - 5 < 0 → x < 5
		// {h1, h2, h3} is unsatisfiable (h1 ∧ h2 already empty)
		CHECK(is_hsb_zero(h1 & h2 & h3));
		// But {h1, h2} is already a MUS — so {h1, h2, h3} is NOT minimal
		CHECK(is_hsb_zero(h1 & h2)); // strict subset already empty
	}

	// =====================================================================
	// LTL(hsb) specs for coherence enforcement
	// =====================================================================
	// Ref: ba_hom.md §10.5: coherence = hsb satisfiability in temporal context

	TEST_CASE("LTL coherence: G(commitment -> admit!=bot) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_commit[t]:hsb != {bot}:hsb -> o_admit[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("LTL coherence: G(new_assert -> F(checked)) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_assert[t]:hsb != {bot}:hsb -> "
			"F (o_verdict[t]:sbf = {ok}:sbf)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("LTL coherence: G(admit=bot -> token=0) masking REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_admit[t]:hsb = {bot}:hsb -> o_token[t]:bv = {0}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("LTL coherence: GF(admit!=bot) liveness REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o_admit[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("LTL multi-variable: full pipeline — tick + coherence + token gating REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o_tick[t]:sbf = {tick}:sbf)) && "
			"G (o_admit[t]:hsb = {bot}:hsb -> o_token[t]:bv = {0}:bv) && "
			"G (i_commit[t]:hsb != {bot}:hsb -> o_admit[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// =====================================================================
	// Cell-count arrangement bound (improvements.md Item 3)
	// =====================================================================
	// With n halfspaces in d dimensions, the arrangement has at most
	// Σ_{i=0}^{d} C(n,i) cells. In low d, this can be small.
	// These tests verify that distinct halfspace arrangements produce
	// distinguishable (non-equivalent) regions.

	TEST_CASE("Arrangement: 2 halfspaces in 1D — 3 cells") {
		gc_fixture gc;
		// Two halfspaces in 1D create 3 regions: (-∞, a), [a, b), [b, ∞)
		auto h1 = make_hs({1.0}, 0.0);     // x < 0
		auto h2 = make_hs({1.0}, -5.0);    // x < 5
		// Region 1: x < 0 (h1 ∧ h2)
		CHECK_FALSE(is_hsb_zero(h1 & h2));
		// Region 2: 0 ≤ x < 5 (¬h1 ∧ h2)
		CHECK_FALSE(is_hsb_zero((~h1) & h2));
		// Region 3: x ≥ 5 (¬h1 ∧ ¬h2)
		CHECK_FALSE(is_hsb_zero((~h1) & (~h2)));
		// Impossible region: x < 0 AND x ≥ 5 (h1 ∧ ¬h2 is NOT necessarily empty)
		// Actually h1 ∧ ¬h2 = x < 0 AND x ≥ 5 ... h2 is x < 5, so ¬h2 is x ≥ 5
		// h1 ∧ ¬h2 = x < 0 AND x ≥ 5 → empty
		CHECK(is_hsb_zero(h1 & (~h2)));
	}

	TEST_CASE("Arrangement: 3 halfplanes in 2D — up to 7 cells") {
		gc_fixture gc;
		auto h1 = make_hs({1.0, 0.0}, 0.0);   // x1 < 0
		auto h2 = make_hs({0.0, 1.0}, 0.0);   // x2 < 0
		auto h3 = make_hs({1.0, 1.0}, 0.0);   // x1 + x2 < 0
		// Enumerate some cells to verify they are distinct and nonempty
		// Cell: x1 < 0, x2 < 0 (third quadrant, also x1+x2 < 0)
		CHECK_FALSE(is_hsb_zero(h1 & h2 & h3));
		// Cell: x1 ≥ 0, x2 < 0, x1+x2 < 0
		CHECK_FALSE(is_hsb_zero((~h1) & h2 & h3));
		// Cell: x1 < 0, x2 ≥ 0, x1+x2 < 0
		CHECK_FALSE(is_hsb_zero(h1 & (~h2) & h3));
		// Cell: x1 ≥ 0, x2 ≥ 0, x1+x2 ≥ 0 (first quadrant)
		CHECK_FALSE(is_hsb_zero((~h1) & (~h2) & (~h3)));
	}

	TEST_CASE("Arrangement: distinguishability — two atoms with same geometry are equal") {
		gc_fixture gc;
		auto h1 = make_hs({1.0}, -3.0);  // x < 3
		auto h2 = make_hs({1.0}, -3.0);  // x < 3 (same)
		// Symmetric difference should be zero (semantically equal)
		CHECK(is_hsb_zero(h1 ^ h2));
	}

	TEST_CASE("Arrangement: distinguishability — two atoms with different geometry are not equal") {
		gc_fixture gc;
		auto h1 = make_hs({1.0}, -3.0);  // x < 3
		auto h2 = make_hs({1.0}, -5.0);  // x < 5
		// Symmetric difference should be nonempty (semantically different)
		CHECK_FALSE(is_hsb_zero(h1 ^ h2));
	}

	// =====================================================================
	// Bounded-k Sikorski criterion coverage (improvements.md Item 2)
	// =====================================================================
	// Verify that the Sikorski criterion check is exhaustive for small k.

	TEST_CASE("Bounded-k: k=2 exhaustive on 3 generators") {
		gc_fixture gc;
		// Three generators, check all 2-element subsets
		auto g1 = make_hs({1.0}, 0.0);      // x < 0
		auto g2 = make_hs({-1.0}, 1.0);     // x ≥ 1
		auto g3 = make_hs({1.0}, -5.0);     // x < 5
		// g1 ∧ g2: x < 0 AND x ≥ 1 → empty (MUS!)
		CHECK(is_hsb_zero(g1 & g2));
		// g1 ∧ g3: x < 0 AND x < 5 → x < 0 (nonempty)
		CHECK_FALSE(is_hsb_zero(g1 & g3));
		// g2 ∧ g3: x ≥ 1 AND x < 5 → [1, 5) (nonempty)
		CHECK_FALSE(is_hsb_zero(g2 & g3));
		// Found 1 MUS: {g1, g2}
	}

	TEST_CASE("Bounded-k: k=2 with polarity on 2 generators") {
		gc_fixture gc;
		auto g1 = make_hs({1.0, 0.0}, -3.0);  // x1 < 3
		auto g2 = make_hs({0.0, 1.0}, -4.0);  // x2 < 4
		// Check all polarity combos for pair {g1, g2}:
		CHECK_FALSE(is_hsb_zero(g1 & g2));       // (+, +): nonempty
		CHECK_FALSE(is_hsb_zero(g1 & (~g2)));    // (+, -): nonempty
		CHECK_FALSE(is_hsb_zero((~g1) & g2));    // (-, +): nonempty
		CHECK_FALSE(is_hsb_zero((~g1) & (~g2))); // (-, -): nonempty
		// No contradictions found — generators are independent
	}

} // TEST_SUITE multi-variable LTL(hsb) specs

TEST_SUITE("hsb — stateful LTL(hsb) specs") {

	//
	// Tests validating LTL(hsb) spec patterns from accumulators.md:
	// bounded-counter thresholds, adaptive enforcement, commitment tracking,
	// failure escalation, and trust-based gating.
	//
	// Accumulators are bounded-codomain state variables whose update rules
	// are part of the spec. In tau-ltl, accumulator predicates become
	// ground atoms in the synthesis pipeline.

	// --- Threshold-gated enforcement: G(flag -> enforce) ---

	TEST_CASE("G(violation_flag:sbf -> enforce:hsb!=bot) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_violation[t]:sbf = {violation}:sbf -> "
			"o_enforce[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(high_count:sbf -> strict_mode:hsb!=bot) REALIZABLE — escalation") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_high_count[t]:sbf = {high}:sbf -> "
			"o_strict[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Adaptive enforcement: trust-based gating ---

	TEST_CASE("G(low_trust:sbf -> o_disclaimer:hsb!=bot) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_low_trust[t]:sbf = {low}:sbf -> "
			"o_disclaimer[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(high_trust:sbf -> o_relaxed:hsb=top) REALIZABLE — permissive mode") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_high_trust[t]:sbf = {trusted}:sbf -> "
			"o_mode[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Commitment tracking: eventually discharge ---

	TEST_CASE("G(pending:sbf -> F(discharged:hsb!=bot)) REALIZABLE — commitment") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_pending[t]:sbf = {pending}:sbf -> "
			"F (o_discharge[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("GF(o_check:hsb!=bot) liveness for periodic checking REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o_check[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Failure escalation: mode transitions ---

	TEST_CASE("G(fail:sbf -> F(recovery:hsb!=bot)) response to failure REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_fail[t]:sbf = {fail}:sbf -> "
			"F (o_recover[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(i_fail=fail && i_fail2=fail -> o_fallback:hsb!=bot) double failure REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_fail1[t]:sbf = {fail}:sbf && i_fail2[t]:sbf = {fail}:sbf -> "
			"o_fallback[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Contradictory accumulator thresholds ---

	TEST_CASE("G(o:hsb=top) && G(flag -> o:hsb=bot) UNREALIZABLE — conflicting modes") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o1[t]:hsb = {top}:hsb) && "
			"G (i_flag[t]:sbf = {on}:sbf -> o1[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
	}

	// --- Multi-accumulator: trust + violations + pending ---

	TEST_CASE("trust+violation+pending three-stream accumulator REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_trust[t]:sbf = {low}:sbf -> o_strict[t]:hsb != {bot}:hsb) && "
			"G (i_violation[t]:sbf = {violation}:sbf -> "
			"F (o_recovery[t]:hsb != {bot}:hsb)) && "
			"G (i_pending[t]:sbf = {pending}:sbf -> "
			"F (o_fulfill[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Forgiveness decay: after long compliance, relax ---

	TEST_CASE("G(compliant:sbf -> F(relaxed:hsb=top)) forgiveness REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_compliant[t]:sbf = {good}:sbf -> "
			"F (o_relax[t]:hsb = {top}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

} // TEST_SUITE stateful LTL(hsb) specs

TEST_SUITE("hsb — guarded LTL(hsb) specs") {

	//
	// Tests validating phase-qualified LTL(hsb) spec patterns from
	// stages_cot_revised.md: phase variable φ ∈ {INPUT, THINKING, RESPONSE},
	// phase-conditioned admissibility, desugared CoT operators.
	//
	// In tau-ltl, phase markers are sbf atoms; phase-qualified hsb atoms
	// are conjunctions of phase(sbf) and concept(hsb).

	// --- Phase-conditioned admissibility: different K_B per phase ---

	TEST_CASE("G(thinking_phase:sbf -> admit:hsb!=bot) phase-gated REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_phase[t]:sbf = {thinking}:sbf -> "
			"o_admit[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(response_phase:sbf -> admit:hsb!=bot) response gating REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_phase[t]:sbf = {response}:sbf -> "
			"o_admit[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- CONSIDERS(α) ≡ F(PHASE_THINKING ∧ @B.α) ---

	TEST_CASE("F(thinking && concept:hsb!=bot) CONSIDERS desugared REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"F (o_phase[t]:sbf = {thinking}:sbf && "
			"o_concept[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- PRE_COMMIT(α, β) ≡ G(thinking∧α → F(response∧β)) ---

	TEST_CASE("G(thinking&&plan:hsb!=bot -> F(response&&execute:hsb!=bot)) PRE_COMMIT REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_phase[t]:sbf = {thinking}:sbf && "
			"i_plan[t]:hsb != {bot}:hsb -> "
			"F (o_phase[t]:sbf = {response}:sbf && "
			"o_exec[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Phase coverage: at least one phase active ---

	TEST_CASE("G(think || respond) at-least-one-phase REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_think[t]:sbf = {think}:sbf || "
			"o_respond[t]:sbf = {respond}:sbf).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Phase transition: thinking → response ordered ---

	TEST_CASE("G(think:sbf -> F(respond:sbf)) phase ordering REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_think[t]:sbf = {think}:sbf -> "
			"F (o_respond[t]:sbf = {respond}:sbf)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Thinking budget: thinking tokens bounded ---

	TEST_CASE("G(thinking -> token:bv!=0) && GF(response:sbf) budget pattern REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_phase[t]:sbf = {thinking}:sbf -> "
			"o_token[t]:bv != {0}:bv) && "
			"G (F (o_phase[t]:sbf = {response}:sbf)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Steering thinking: bias toward high-value continuations ---

	TEST_CASE("G(guard && condition -> output:hsb!=bot) guarded REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_phase[t]:sbf = {thinking}:sbf && "
			"i_low_val[t]:sbf = {low}:sbf -> "
			"o_steer[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Phase-qualified + accumulator: thinking violation count ---

	TEST_CASE("G(thinking && violation -> F(response && recovery)) combined REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_phase[t]:sbf = {thinking}:sbf && "
			"i_viol[t]:sbf = {violation}:sbf -> "
			"F (o_phase[t]:sbf = {response}:sbf && "
			"o_recover[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Contradictory phase spec ---

	TEST_CASE("G(phase=thinking) && G(phase=response) UNREALIZABLE — phase conflict") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_phase[t]:sbf = {thinking}:sbf) && "
			"G (o_phase[t]:sbf = {response}:sbf).");
		REQUIRE(fm != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
	}

} // TEST_SUITE guarded LTL(hsb) specs

TEST_SUITE("hsb — conditional LTL(hsb) specs") {

	//
	// Tests validating LTL(hsb) patterns from spec_authorship_levels_3_4.md:
	// JIT halfspace extraction → insert into running spec → re-check
	// realizability. Level 3 = explicit user signals, Level 4 = implicit.

	// --- Level 3: user says "always do X" → G(concept) ---

	TEST_CASE("G(o_concept:hsb!=bot) — L3 always-positive adoption REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_concept[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Level 3: user says "never do Y" → G(¬concept) ---

	TEST_CASE("G(o_avoid:hsb=bot) — L3 always-negative adoption REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_avoid[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Level 3: conditional → G(scope → ¬concept) ---

	TEST_CASE("G(scope:sbf -> o_avoid:hsb=bot) — L3 conditional-negative REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_scope[t]:sbf = {inscope}:sbf -> "
			"o_avoid[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Level 3: conditional positive → G(scope → F(concept)) ---

	TEST_CASE("G(scope:sbf -> F(o_do:hsb!=bot)) — L3 conditional-positive REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_scope[t]:sbf = {inscope}:sbf -> "
			"F (o_do[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- JIT revision stacking: base spec + user revision ---

	TEST_CASE("base G(o:hsb!=bot) + revision G(flag->o:hsb=top) stacked REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o1[t]:hsb != {bot}:hsb) && "
			"G (i_flag[t]:sbf = {on}:sbf -> o1[t]:hsb = {top}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- JIT revision conflict: two contradictory user revisions ---

	TEST_CASE("G(o:hsb=top) && G(scope->o:hsb=bot) UNREALIZABLE — revision conflict") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o1[t]:hsb = {top}:hsb) && "
			"G (i_scope[t]:sbf = {active}:sbf -> o1[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
	}

	// --- Level 4: accumulated preference refinement ---

	TEST_CASE("GF(o_preferred:hsb!=bot) — L4 liveness preference REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o_preferred[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- PWR weakening pattern: unrealizable → weaken to realizable ---

	TEST_CASE("is_hsb_zero validates PWR conflict detection") {
		gc_fixture gc;
		// Simulate PWR conflict: two halfspaces that are jointly unsatisfiable
		auto existing = make_hs({1.0}, 5.0);   // x < -5
		auto revision = make_hs({-1.0}, 10.0); // x >= 10
		CHECK(is_hsb_zero(existing & revision)); // conflict detected

		// Weakened revision is compatible
		auto weakened = make_hs({-1.0}, -10.0); // x >= -10
		CHECK_FALSE(is_hsb_zero(existing & weakened)); // no conflict
	}

	// --- Multi-revision stacking: 3 L3 revisions compatible ---

	TEST_CASE("three L3 revisions stacked REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_cond1[t]:sbf = {c1}:sbf -> o1[t]:hsb != {bot}:hsb) && "
			"G (i_cond2[t]:sbf = {c2}:sbf -> o2[t]:hsb != {bot}:hsb) && "
			"G (i_cond3[t]:sbf = {c3}:sbf -> "
			"F (o3[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- L3+L4 combined: immediate + accumulated ---

	TEST_CASE("L3 G(avoid) + L4 GF(preferred) combined REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_avoid[t]:hsb = {bot}:hsb) && "
			"G (F (o_preferred[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Halfspace coherence under revision: is_hsb_zero infrastructure ---

	TEST_CASE("JIT halfspace coherence: new atom compatible with existing") {
		gc_fixture gc;
		// Existing atoms in the spec
		auto a1 = make_hs({1.0, 0.0}, -5.0);  // x1 < 5
		auto a2 = make_hs({0.0, 1.0}, -5.0);  // x2 < 5
		// JIT-extracted new atom from L3 revision
		auto a3 = make_hs({-1.0, 0.0}, -3.0); // x1 >= -3
		CHECK_FALSE(is_hsb_zero(a1 & a2 & a3)); // all compatible
	}

	TEST_CASE("JIT halfspace conflict: new atom contradicts existing") {
		gc_fixture gc;
		auto existing = make_hs({1.0, 0.0}, 5.0); // x1 < -5
		auto jit_new = make_hs({-1.0, 0.0}, 10.0); // x1 >= 10
		CHECK(is_hsb_zero(existing & jit_new)); // contradiction
	}

} // TEST_SUITE conditional LTL(hsb) specs

TEST_SUITE("hsb — mixed-sort LTL(hsb,sbf) specs") {

	//
	// Tests validating LTL(hsb) spec patterns from:
	// - llm_is_tau.md (680 lines): unified formula, two temporal axes,
	//   Voronoi cell decoding, cross-axis specs, operational modes
	// - stages_two_point_revised.md (677 lines): Point A (input) vs
	//   Point B (output) atom libraries, TwoPointAdmissibility
	// - stage2_revised.md (298 lines): JIT atom resolution in synthesis
	//
	// In tau-ltl, Point A/B annotations are modeled via distinct stream
	// names (i_pointA_*, o_pointB_*). Inner-temporal depth is modeled
	// as a second stream dimension with tick markers.

	// --- Two-point: Point A (input-side) constraints ---

	TEST_CASE("G(i_pointA:hsb!=bot -> o_admit:hsb!=bot) input-side gating REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_pointA[t]:hsb != {bot}:hsb -> "
			"o_admit[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(i_pointA:sbf=query -> o_pointB:hsb!=bot) A-to-B response REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_pointA[t]:sbf = {query}:sbf -> "
			"o_pointB[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Two-point: Point B (output-side) constraints ---

	TEST_CASE("G(o_pointB:hsb!=bot) output-side always-admit REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_pointB[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(o_KA:hsb!=bot && o_KB:hsb!=bot) dual admissibility REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_KA[t]:hsb != {bot}:hsb && "
			"o_KB[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Cross-axis: G_outer × tick-based inner-temporal ---

	TEST_CASE("GF(tick) && G(tick->inner_check:hsb!=bot) cross-axis REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (F (o_tick[t]:sbf = {tick}:sbf)) && "
			"G (o_tick[t]:sbf = {tick}:sbf -> "
			"o_inner[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(depth_mark:sbf -> layer_ok:hsb!=bot) per-layer constraint REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_depth[t]:sbf = {layer}:sbf -> "
			"o_safe[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Mechanistic: hidden-layer representation constraints ---

	TEST_CASE("G(dangerous:sbf -> F(safety_active:hsb!=bot)) safety circuit REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_danger[t]:sbf = {danger}:sbf -> "
			"F (o_safety[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("G(honest_early:sbf -> o_honest_late:hsb!=bot) anti-deception REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_honest[t]:sbf = {honest}:sbf -> "
			"o_honest[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Voronoi cell: argmax as halfspace intersection ---

	TEST_CASE("Voronoi cell as halfspace intersection — 3 tokens") {
		gc_fixture gc;
		// Token i wins iff for all j≠i: <W_u[i]-W_u[j], h> >= 0
		// make_hs(w, b): lex_leading_sign(w)=-1 → non-strict ≤, +1 → strict <
		// To get x0 >= x1: use w={-1,1}, lex_leading=-1, so -x0+x1 ≤ 0 → x0 ≥ x1
		auto h01 = make_hs({-1.0, 1.0}, 0.0);  // x0 >= x1 (token 0 beats 1)
		auto h02 = make_hs({-1.0, 0.0}, 0.0);  // x0 >= 0  (token 0 beats 2)
		auto voronoi_cell = h01 & h02;
		CHECK_FALSE(is_hsb_zero(voronoi_cell)); // nonempty — some h prefers token 0
	}

	TEST_CASE("Voronoi cell complement — excluded tokens") {
		gc_fixture gc;
		auto cell_0 = make_hs({-1.0, 1.0}, 0.0) & make_hs({-1.0, 0.0}, 0.0);
		auto not_cell_0 = ~cell_0;
		CHECK_FALSE(is_hsb_zero(not_cell_0)); // complement nonempty
		CHECK(is_hsb_zero(cell_0 & not_cell_0)); // cell ∩ complement = empty
	}

	TEST_CASE("Voronoi cells cover R^d — union of all cells") {
		gc_fixture gc;
		// 2D, 3 tokens. The 3 Voronoi cells partition R^2.
		// Cell 0: x0≥x1 ∧ x0≥0 (token 0 wins)
		auto c0 = make_hs({-1.0, 1.0}, 0.0) & make_hs({-1.0, 0.0}, 0.0);
		// Cell 1: x1>x0 ∧ x1≥0 (token 1 wins)
		auto c1 = make_hs({1.0, -1.0}, 0.0) & make_hs({0.0, -1.0}, 0.0);
		// Cell 2: 0>x0 ∧ 0>x1 (token 2 wins — both negative)
		auto c2 = make_hs({1.0, 0.0}, 0.0) & make_hs({0.0, 1.0}, 0.0);
		// Union should cover nearly all of R^2 (minus boundaries)
		auto union_all = c0 | c1 | c2;
		CHECK_FALSE(is_hsb_zero(union_all));
	}

	// --- Operational modes: spec-only vs full synthesis ---

	TEST_CASE("Spec-only mode: G(token:bv!=0 -> admit:hsb!=bot) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_token[t]:bv != {0}:bv -> "
			"o_admit[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("Verification mode: contradictory model+spec UNREALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_token[t]:bv = {0}:bv) && "
			"G (o_token[t]:bv != {0}:bv).");
		REQUIRE(fm != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
	}

	// --- Full pipeline composition: embedding + spec ---

	TEST_CASE("G(embed:hsb!=bot) && G(embed:hsb!=bot -> token:bv!=0) composition REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_embed[t]:hsb != {bot}:hsb) && "
			"G (o_embed[t]:hsb != {bot}:hsb -> "
			"o_token[t]:bv != {0}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Adaptive enforcement via accumulator + two-point ---

	TEST_CASE("G(pointA:sbf=viol -> F(pointB:hsb!=bot)) cross-point escalation REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_pointA[t]:sbf = {violation}:sbf -> "
			"F (o_pointB[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Distillation: smaller formula equivalent on distribution ---

	TEST_CASE("Distillation check: simplified formula is subset of original") {
		gc_fixture gc;
		// Original: x1 < 5 and x2 < 5
		auto original = make_hs({1.0, 0.0}, -5.0) & make_hs({0.0, 1.0}, -5.0);
		// Simplified: x1 < 3 and x2 < 3 (subset of original)
		auto simplified = make_hs({1.0, 0.0}, -3.0) & make_hs({0.0, 1.0}, -3.0);
		// simplified ⊆ original iff simplified & ~original = empty
		CHECK(is_hsb_zero(simplified & ~original));
	}

	TEST_CASE("Distillation: non-subset detected") {
		gc_fixture gc;
		auto original = make_hs({1.0, 0.0}, -3.0); // x1 < 3
		auto candidate = make_hs({1.0, 0.0}, -5.0); // x1 < 5 (superset)
		// candidate & ~original should be nonempty (the 3 <= x1 < 5 strip)
		CHECK_FALSE(is_hsb_zero(candidate & ~original));
	}

	// --- Two-point + CoT combined: input classification + phase-gated output ---

	TEST_CASE("G(pointA:sbf=legal && phase:sbf=response -> pointB:hsb!=bot) combined REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_pointA[t]:sbf = {legal}:sbf && "
			"i_phase[t]:sbf = {response}:sbf -> "
			"o_pointB[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Product Mealy state: multi-accumulator + two-point + phase ---

	TEST_CASE("trust + phase + two-point product spec REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_trust[t]:sbf = {low}:sbf -> o_strict[t]:hsb != {bot}:hsb) && "
			"G (i_phase[t]:sbf = {thinking}:sbf -> "
			"o_admit[t]:hsb != {bot}:hsb) && "
			"G (i_pointA[t]:sbf = {query}:sbf -> "
			"F (o_pointB[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

} // TEST_SUITE two-point annotation & inner-temporal patterns

TEST_SUITE("hsb — invariant and liveness LTL(hsb) specs") {

	//
	// Tests validating LTL(hsb) spec patterns from tau-neuro's
	// hypothesis_class.py _TEMPLATES list. These are the concrete
	// formula shapes the inductive synthesis pipeline proposes as
	// candidate specs. Each template must parse and synthesize correctly.
	//
	// Ref: tau_neuro/hypothesis_class.py lines 156-204
	// Ref: paper/tau_neuro.tex §5 (inductive spec synthesis)

	// --- Size 1: single temporal + single atom ---

	TEST_CASE("LTL template G(a0) — invariant REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec("G (o_a0[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("LTL template F(a0) — eventual REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec("F (o_a0[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("LTL template G(!a0) — negated invariant REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec("G (o_a0[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Size 2: conjunction/disjunction under G ---

	TEST_CASE("LTL template G(a0 && a1) — conjunction invariant REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_a0[t]:hsb != {bot}:hsb && "
			"o_a1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("LTL template G(a0 || a1) — disjunction invariant REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_a0[t]:hsb != {bot}:hsb || "
			"o_a1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Size 2: Until pattern ---

	TEST_CASE("LTL template a0 U a1 — until REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"(o_a0[t]:hsb != {bot}:hsb) U "
			"(o_a1[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Size 3: response pattern ---

	TEST_CASE("LTL template G(a0 -> F(a1)) — response REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_a0[t]:sbf = {trigger}:sbf -> "
			"F (o_a1[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("LTL template G(a0 -> F(a1 && a2)) — response with conjunction REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_a0[t]:sbf = {trigger}:sbf -> "
			"F (o_a1[t]:hsb != {bot}:hsb && "
			"o_a2[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Size 3: three-atom conjunction ---

	TEST_CASE("LTL template G(a0 && a1 && a2) — triple conjunction REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_a0[t]:hsb != {bot}:hsb && "
			"o_a1[t]:hsb != {bot}:hsb && "
			"o_a2[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Size 3: nested temporal ---

	TEST_CASE("LTL template GF(a0) — recurrence REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec("G (F (o_a0[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("LTL template FG(a0) — persistence REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec("F (G (o_a0[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Size 4: guarded until ---

	TEST_CASE("LTL template G(a0 -> (a1 U a2)) — guarded until REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_a0[t]:sbf = {guard}:sbf -> "
			"(o_a1[t]:hsb != {bot}:hsb U "
			"o_a2[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Two-point mixed templates (from hypothesis_class.py) ---

	TEST_CASE("two-point G(a0 -> !a1) — input blocks output REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_pointA[t]:sbf = {block}:sbf -> "
			"o_pointB[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("two-point G(a0 -> F(!a1)) — input eventually suppresses REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_pointA[t]:sbf = {suppress}:sbf -> "
			"F (o_pointB[t]:hsb = {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- CoT-aware templates ---

	TEST_CASE("CoT G(a0 -> (a0 U a1)) — thinking persists until response REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_think[t]:sbf = {think}:sbf -> "
			"(o_think[t]:sbf = {think}:sbf U "
			"o_respond[t]:sbf = {respond}:sbf)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Accumulator-conditioned templates ---

	TEST_CASE("accumulator G(acc0 -> a0) — threshold triggers output REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_acc[t]:sbf = {high}:sbf -> "
			"o_out[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("accumulator G(acc0 -> F(a0)) — threshold eventually triggers REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_acc[t]:sbf = {high}:sbf -> "
			"F (o_out[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("accumulator G(acc0 && a0 -> F(a1)) — joint trigger REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_acc[t]:sbf = {high}:sbf && "
			"i_cond[t]:sbf = {active}:sbf -> "
			"F (o_out[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("accumulator G(acc0 -> !a0) — threshold blocks output REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_acc[t]:sbf = {high}:sbf -> "
			"o_blocked[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

} // TEST_SUITE invariant and liveness LTL(hsb) specs

TEST_SUITE("hsb — spec composition LTL(hsb) specs") {

	//
	// Tests validating spec evolution patterns from tau-neuro's
	// spec_evolution.py and paper/tau_neuro.tex §5, §8, §10.7:
	// inductive synthesis, failure recovery, mode switching, session
	// persistence, BA-Hom incremental construction.

	// --- Spec consolidation: conjunction of multiple clauses ---

	TEST_CASE("Spec consolidation: 4 independent clauses realizable") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_polite[t]:hsb != {bot}:hsb) && "
			"G (o_concise[t]:hsb != {bot}:hsb) && "
			"G (i_topic[t]:sbf = {sensitive}:sbf -> "
			"o_careful[t]:hsb != {bot}:hsb) && "
			"G (F (o_helpful[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Clause addition: base spec + new clause still realizable ---

	TEST_CASE("Clause addition: G(a) + G(b -> F(c)) composed REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_base[t]:hsb != {bot}:hsb) && "
			"G (i_trigger[t]:sbf = {trig}:sbf -> "
			"F (o_new[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Clause removal: dropping clause preserves realizability ---

	TEST_CASE("Clause removal: removing contradictory clause restores realizability") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Full spec with contradiction
		tref full = spec(
			"G (o_x[t]:hsb = {top}:hsb) && "
			"G (i_flag[t]:sbf = {on}:sbf -> o_x[t]:hsb = {bot}:hsb).");
		REQUIRE(full != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(full));
		// After removing second clause
		tref reduced = spec("G (o_x[t]:hsb = {top}:hsb).");
		REQUIRE(reduced != nullptr);
		CHECK(is_tau_formula_sat<node_t>(reduced));
	}

	// --- Mode B (spec-only): token + admit constraints ---

	TEST_CASE("Mode B spec-only: G(admit:hsb!=bot) && G(token:bv!=0) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_admit[t]:hsb != {bot}:hsb) && "
			"G (o_token[t]:bv != {0}:bv).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Mode A (joint LLM+spec): embed + token + admit ---

	TEST_CASE("Multi-clause joint: G(embed -> token!=0) && G(admit:hsb!=bot) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_embed[t]:hsb != {bot}:hsb -> "
			"o_token[t]:bv != {0}:bv) && "
			"G (o_admit[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Incremental BA-Hom: adding generators preserves coherence ---

	TEST_CASE("incremental: 3 generators compatible after sequential add") {
		gc_fixture gc;
		// Simulate: g1 = x1 < 5, g2 = x2 < 5 (turn 1), g3 = x1 > -3 (turn 2)
		auto g1 = make_hs({1.0, 0.0}, -5.0);  // x1 < 5
		auto g2 = make_hs({0.0, 1.0}, -5.0);  // x2 < 5
		// Turn 1: check g1 & g2 compatible
		CHECK_FALSE(is_hsb_zero(g1 & g2));
		// Turn 2: add g3, check all compatible
		auto g3 = make_hs({-1.0, 0.0}, -3.0); // x1 >= -3
		CHECK_FALSE(is_hsb_zero(g1 & g2 & g3));
	}

	TEST_CASE("incremental: 4th generator causes contradiction") {
		gc_fixture gc;
		auto g1 = make_hs({1.0, 0.0}, -5.0);   // x1 < 5
		auto g2 = make_hs({0.0, 1.0}, -5.0);   // x2 < 5
		auto g3 = make_hs({-1.0, 0.0}, -3.0);  // x1 >= -3
		CHECK_FALSE(is_hsb_zero(g1 & g2 & g3)); // compatible
		// g4 contradicts g1: x1 >= 10 but g1 says x1 < 5
		auto g4 = make_hs({-1.0, 0.0}, 10.0);  // x1 >= 10
		CHECK(is_hsb_zero(g1 & g4));            // pairwise contradiction
		CHECK(is_hsb_zero(g1 & g2 & g3 & g4)); // full set contradiction
	}

	// --- Session persistence: Mealy state resume pattern ---

	TEST_CASE("Session resume: saved spec re-parses identically") {
		gc_fixture gc;
		bdd_init<Bool>();
		const char* spec_str =
			"G (o_polite[t]:hsb != {bot}:hsb) && "
			"G (i_topic[t]:sbf = {sensitive}:sbf -> "
			"o_careful[t]:hsb != {bot}:hsb).";
		tref fm1 = spec(spec_str);
		tref fm2 = spec(spec_str);
		REQUIRE(fm1 != nullptr);
		REQUIRE(fm2 != nullptr);
		// Same spec string produces parseable formulas (session restore)
		CHECK(is_tau_formula_sat<node_t>(fm1));
		CHECK(is_tau_formula_sat<node_t>(fm2));
	}

	// --- KL proxy: restrictive spec is harder to satisfy ---

	TEST_CASE("KL proxy: more restrictive spec has tighter hsb region") {
		gc_fixture gc;
		// Loose: x1 < 10
		auto loose = make_hs({1.0, 0.0}, -10.0);
		// Tight: x1 < 10 AND x1 > 5 (smaller region)
		auto tight = loose & make_hs({-1.0, 0.0}, 5.0);
		CHECK_FALSE(is_hsb_zero(loose));
		CHECK_FALSE(is_hsb_zero(tight));
		// tight is proper subset of loose
		CHECK(is_hsb_zero(tight & ~loose));      // tight ⊆ loose
		CHECK_FALSE(is_hsb_zero(loose & ~tight)); // loose ⊄ tight
	}

	// --- Synthesis timeout fallback: split spec into independent parts ---

	TEST_CASE("Spec splitting: independent sub-specs individually realizable") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Sub-spec 1
		tref sub1 = spec("G (o_a[t]:hsb != {bot}:hsb).");
		REQUIRE(sub1 != nullptr);
		CHECK(is_tau_formula_sat<node_t>(sub1));
		// Sub-spec 2
		tref sub2 = spec(
			"G (i_trigger[t]:sbf = {trig}:sbf -> "
			"F (o_b[t]:hsb != {bot}:hsb)).");
		REQUIRE(sub2 != nullptr);
		CHECK(is_tau_formula_sat<node_t>(sub2));
		// Combined
		tref combined = spec(
			"G (o_a[t]:hsb != {bot}:hsb) && "
			"G (i_trigger[t]:sbf = {trig}:sbf -> "
			"F (o_b[t]:hsb != {bot}:hsb)).");
		REQUIRE(combined != nullptr);
		CHECK(is_tau_formula_sat<node_t>(combined));
	}

} // TEST_SUITE spec composition & failure recovery patterns

TEST_SUITE("hsb — clause-level BA operations") {

	//
	// Tests validating clause-level spec manipulation for Mode A (joint
	// LLM+spec formulas). From tau-neuro spec_evolution.py PR #65:
	// ClauseOrigin, TaggedClause, ClauseRegistry, targeted_surgery.
	//
	// In Mode A, the joint formula Φ = Φ_M ∧ Φ_spec contains clauses
	// from multiple origins (LLM, spec pipeline, user). The synthesis
	// pipeline must be able to split, tag, deactivate, and rebuild
	// formulas while preserving realizability.

	// --- Clause splitting: each clause independently realizable ---

	TEST_CASE("Clause split: G(a) individually realizable") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref c1 = spec("G (o_polite[t]:hsb != {bot}:hsb).");
		REQUIRE(c1 != nullptr);
		CHECK(is_tau_formula_sat<node_t>(c1));
	}

	TEST_CASE("Clause split: G(trigger -> F(response)) individually realizable") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref c2 = spec(
			"G (i_ask[t]:sbf = {ask}:sbf -> "
			"F (o_answer[t]:hsb != {bot}:hsb)).");
		REQUIRE(c2 != nullptr);
		CHECK(is_tau_formula_sat<node_t>(c2));
	}

	TEST_CASE("Clause split: GF(a) individually realizable") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref c3 = spec("G (F (o_check[t]:hsb != {bot}:hsb)).");
		REQUIRE(c3 != nullptr);
		CHECK(is_tau_formula_sat<node_t>(c3));
	}

	// --- Joint formula: all clauses composed ---

	TEST_CASE("Joint 3-clause formula realizable") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref joint = spec(
			"G (o_polite[t]:hsb != {bot}:hsb) && "
			"G (i_ask[t]:sbf = {ask}:sbf -> "
			"F (o_answer[t]:hsb != {bot}:hsb)) && "
			"G (F (o_check[t]:hsb != {bot}:hsb)).");
		REQUIRE(joint != nullptr);
		CHECK(is_tau_formula_sat<node_t>(joint));
	}

	// --- LLM-side clause removal: drop LLM clause, keep spec clauses ---

	TEST_CASE("Drop LLM clause: remaining spec clauses still realizable") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Full joint: LLM clause + spec clause + user clause
		tref full = spec(
			"G (o_llm[t]:hsb != {bot}:hsb) && "
			"G (o_spec[t]:hsb != {bot}:hsb) && "
			"G (i_user[t]:sbf = {rule}:sbf -> "
			"F (o_user[t]:hsb != {bot}:hsb)).");
		REQUIRE(full != nullptr);
		CHECK(is_tau_formula_sat<node_t>(full));
		// After dropping LLM clause
		tref without_llm = spec(
			"G (o_spec[t]:hsb != {bot}:hsb) && "
			"G (i_user[t]:sbf = {rule}:sbf -> "
			"F (o_user[t]:hsb != {bot}:hsb)).");
		REQUIRE(without_llm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(without_llm));
	}

	// --- Targeted surgery: replace one clause while keeping others ---

	TEST_CASE("Targeted surgery: replace strict clause with weaker REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Original: always polite AND always concise (strict)
		tref strict = spec(
			"G (o_polite[t]:hsb != {bot}:hsb) && "
			"G (o_concise[t]:hsb != {bot}:hsb).");
		REQUIRE(strict != nullptr);
		CHECK(is_tau_formula_sat<node_t>(strict));
		// Surgery: replace "always concise" with "eventually concise"
		tref weaker = spec(
			"G (o_polite[t]:hsb != {bot}:hsb) && "
			"F (o_concise[t]:hsb != {bot}:hsb).");
		REQUIRE(weaker != nullptr);
		CHECK(is_tau_formula_sat<node_t>(weaker));
	}

	// --- Origin-aware: contradictory LLM clause makes joint unrealizable ---

	TEST_CASE("Contradictory LLM clause: joint UNREALIZABLE, spec-only REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		// LLM says always-top, spec says always-bot for same stream
		tref joint = spec(
			"G (o_x[t]:hsb = {top}:hsb) && "
			"G (o_x[t]:hsb = {bot}:hsb).");
		REQUIRE(joint != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(joint));
		// Drop LLM clause (always-top), keep spec clause
		tref spec_only = spec("G (o_x[t]:hsb = {bot}:hsb).");
		REQUIRE(spec_only != nullptr);
		CHECK(is_tau_formula_sat<node_t>(spec_only));
	}

	// --- Clause deactivation: inactive clauses excluded from formula ---

	TEST_CASE("Deactivate clause: 2 of 3 clauses still realizable") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Active clauses only
		tref active = spec(
			"G (o_a[t]:hsb != {bot}:hsb) && "
			"G (F (o_b[t]:hsb != {bot}:hsb)).");
		REQUIRE(active != nullptr);
		CHECK(is_tau_formula_sat<node_t>(active));
	}

	// --- Mode A 5-clause joint formula: LLM + spec + user origins ---

	TEST_CASE("Multi-clause 5-clause joint: mixed origins REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (o_llm1[t]:hsb != {bot}:hsb) && "
			"G (o_llm2[t]:hsb != {bot}:hsb) && "
			"G (o_spec1[t]:hsb != {bot}:hsb) && "
			"G (i_user[t]:sbf = {rule}:sbf -> "
			"F (o_user1[t]:hsb != {bot}:hsb)) && "
			"G (F (o_spec2[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Drop all LLM clauses, keep user+spec ---

	TEST_CASE("Drop all LLM clauses: user+spec subset REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref user_spec = spec(
			"G (o_spec1[t]:hsb != {bot}:hsb) && "
			"G (i_user[t]:sbf = {rule}:sbf -> "
			"F (o_user1[t]:hsb != {bot}:hsb)) && "
			"G (F (o_spec2[t]:hsb != {bot}:hsb)).");
		REQUIRE(user_spec != nullptr);
		CHECK(is_tau_formula_sat<node_t>(user_spec));
	}

	// --- hsb-level clause coherence: clauses don't overlap contradictorily ---

	TEST_CASE("Clause coherence: disjoint hsb regions non-contradictory") {
		gc_fixture gc;
		// Clause 1 region: x1 < 5
		auto r1 = make_hs({1.0, 0.0}, -5.0);
		// Clause 2 region: x1 > 10 (represented as -x1 < -10)
		auto r2 = make_hs({-1.0, 0.0}, 10.0);
		// Disjoint but union is nonempty
		CHECK(is_hsb_zero(r1 & r2));       // no overlap
		CHECK_FALSE(is_hsb_zero(r1 | r2)); // union covers both
	}

	TEST_CASE("Clause coherence: overlapping hsb regions compatible") {
		gc_fixture gc;
		// Clause 1: x1 < 10
		auto r1 = make_hs({1.0, 0.0}, -10.0);
		// Clause 2: x1 < 20
		auto r2 = make_hs({1.0, 0.0}, -20.0);
		// Overlapping: r1 ⊂ r2
		CHECK_FALSE(is_hsb_zero(r1 & r2));       // overlap nonempty
		CHECK(is_hsb_zero(r1 & ~r2));            // r1 ⊆ r2
		CHECK_FALSE(is_hsb_zero(r2 & ~r1));      // r2 ⊄ r1
	}

} // TEST_SUITE clause-level BA operations clause-level operations

TEST_SUITE("hsb — classification halfspace construction") {

	//
	// Tests validating the three-sorted structure from class-hbs-3sort.tex:
	// - Sort 1 (vec): bare vectors in Q^d
	// - Sort 2 (hsb): atomless Boolean algebra of halfspaces
	// - Sort 3 (Q): rationals under order
	// - Cross-sort: membership ε(h,H), projections π_i(h)
	//
	// Key result: classification-as-halfspace — any binary classification
	// prompt is exactly a halfspace test on the residual stream.

	// --- Classification-as-halfspace: w = W_u[y+] - W_u[y-], b = -log(τ/(1-τ)) ---

	TEST_CASE("Classification halfspace: threshold τ=0.5 gives zero offset") {
		gc_fixture gc;
		// w = W_u[yes] - W_u[no] = (1, -1), b = -log(0.5/0.5) = 0
		auto cls = make_hs({1.0, -1.0}, 0.0);
		CHECK_FALSE(is_hsb_zero(cls));
		CHECK_FALSE(is_hsb_zero(~cls));
	}

	TEST_CASE("Classification halfspace: threshold τ=0.7 gives negative offset") {
		gc_fixture gc;
		// w = (1, -1), b = -log(0.7/0.3) ≈ -0.847
		auto cls = make_hs({1.0, -1.0}, -0.847);
		CHECK_FALSE(is_hsb_zero(cls));
		CHECK_FALSE(is_hsb_zero(~cls));
	}

	TEST_CASE("Classification halfspace: two classifiers with same w, different τ") {
		gc_fixture gc;
		// Same normal but different thresholds: τ=0.5 vs τ=0.9
		// make_hs(w, b) with lex-leading + means ⟨w,x⟩+b < 0
		// τ=0.5: b=0 → x1-x2 < 0
		// τ=0.9: b=-2.197 → x1-x2 < 2.197 (wider region)
		auto c_strict = make_hs({1.0, -1.0}, 0.0);     // x1-x2 < 0
		auto c_wide = make_hs({1.0, -1.0}, -2.197);    // x1-x2 < 2.197
		// c_strict ⊂ c_wide (tighter bound is a subset of wider)
		CHECK_FALSE(is_hsb_zero(c_strict & c_wide)); // overlap (= c_strict)
		CHECK(is_hsb_zero(c_strict & ~c_wide));       // c_strict ⊆ c_wide
		CHECK_FALSE(is_hsb_zero(c_wide & ~c_strict)); // c_wide \ c_strict nonempty
	}

	TEST_CASE("Classification halfspace: different classifiers independent") {
		gc_fixture gc;
		// Classifier 1: "is medical" w1 = (1, 0, -1), b1 = 0
		auto c_med = make_hs({1.0, 0.0, -1.0}, 0.0);
		// Classifier 2: "cites source" w2 = (0, 1, -1), b2 = 0
		auto c_cite = make_hs({0.0, 1.0, -1.0}, 0.0);
		// Independent classifiers have overlapping regions
		CHECK_FALSE(is_hsb_zero(c_med & c_cite));
		CHECK_FALSE(is_hsb_zero(c_med & ~c_cite));  // medical but no citation
		CHECK_FALSE(is_hsb_zero(~c_med & c_cite));  // citation but not medical
	}

	// --- Surface formula examples from the paper ---

	TEST_CASE("Surface: citation discipline G(¬(medical ∧ ¬citation))") {
		gc_fixture gc;
		bdd_init<Bool>();
		// G(¬(commit ∈ H_medical ∧ ¬(commit ∈ H_citation)))
		// In tau-ltl: commit must cite when medical
		tref fm = spec(
			"G (i_medical[t]:sbf = {med}:sbf -> "
			"o_citation[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("Surface: bounded answer obligation G(question -> F(answer))") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_question[t]:sbf = {yesno}:sbf -> "
			"F (o_answer[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("Surface: confidence-gated abstention") {
		gc_fixture gc;
		bdd_init<Bool>();
		// If recommending and low confidence, must abstain
		tref fm = spec(
			"G (i_recommend[t]:sbf = {rec}:sbf -> "
			"o_confident[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Three-sorted: hsb + Q projections modeled as multi-type ---

	TEST_CASE("Three-sorted: hsb classifier + rational threshold REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Combines hsb sort (halfspace test) with sbf sort (rational threshold proxy)
		tref fm = spec(
			"G (o_cls[t]:hsb != {bot}:hsb && "
			"i_conf[t]:sbf = {high}:sbf -> "
			"F (o_commit[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- ω-categoricity: finite constants preserve decidability ---

	TEST_CASE("omega-cat: 3 named halfspace constants all satisfiable") {
		gc_fixture gc;
		// Three classifiers from binding table B
		auto h1 = make_hs({1.0, 0.0, 0.0}, -1.0);
		auto h2 = make_hs({0.0, 1.0, 0.0}, -1.0);
		auto h3 = make_hs({0.0, 0.0, 1.0}, -1.0);
		CHECK_FALSE(is_hsb_zero(h1));
		CHECK_FALSE(is_hsb_zero(h2));
		CHECK_FALSE(is_hsb_zero(h3));
		CHECK_FALSE(is_hsb_zero(h1 & h2 & h3));
	}

	TEST_CASE("omega-cat: adding PWR constant preserves consistency") {
		gc_fixture gc;
		// Original 2 constants — both lex-leading positive
		// h1: x1-x2 < 0, h2: x1 < 5
		auto h1 = make_hs({1.0, -1.0}, 0.0);
		auto h2 = make_hs({1.0, 0.0}, -5.0);
		CHECK_FALSE(is_hsb_zero(h1 & h2));
		// PWR adds a third constant — still finite, still consistent
		// h3: x2 < 3
		auto h3 = make_hs({0.0, 1.0}, -3.0);
		CHECK_FALSE(is_hsb_zero(h1 & h2 & h3));
	}

	// --- Binding table: translated constants coherence ---

	TEST_CASE("Binding table: same query different threshold nests") {
		gc_fixture gc;
		// H_{q, 0.5} and H_{q, 0.8} — same normal, different offset
		// make_hs(w,b) with lex-leading +: ⟨w,x⟩+b < 0
		// h_50: x1-x2 < 0 (strict), h_80: x1-x2 < 1.386 (wider)
		auto h_strict = make_hs({1.0, -1.0}, 0.0);
		auto h_wide = make_hs({1.0, -1.0}, -1.386);
		// Strict region is a subset of wide region
		CHECK(is_hsb_zero(h_strict & ~h_wide));      // h_strict ⊆ h_wide
		CHECK_FALSE(is_hsb_zero(h_wide & ~h_strict)); // h_wide \ h_strict nonempty
	}

	TEST_CASE("Binding table: negated classifier = complement halfspace") {
		gc_fixture gc;
		auto h = make_hs({1.0, -1.0}, 0.0);
		// ¬H is exactly ~h
		CHECK_FALSE(is_hsb_zero(~h));
		CHECK(is_hsb_zero(h & ~h));   // tautology: H ∧ ¬H = ⊥
		CHECK(is_hsb_one(h | ~h));    // H ∨ ¬H = ⊤
	}

	// --- Multi-halfspace Mealy step: k tests per transition ---

	TEST_CASE("4 independent halfspaces simultaneously nonempty") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Mealy step tests 4 halfspace predicates
		tref fm = spec(
			"G (o_h1[t]:hsb != {bot}:hsb) && "
			"G (o_h2[t]:hsb != {bot}:hsb) && "
			"G (i_h3[t]:sbf = {cls}:sbf -> "
			"o_h4[t]:hsb != {bot}:hsb) && "
			"G (F (o_h5[t]:hsb != {bot}:hsb)).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Soundness: contradictory classifiers → unrealizable ---

	TEST_CASE("Contradictory classifiers: opposite halfspaces disjoint") {
		gc_fixture gc;
		// make_hs({1,0}, -5): lex-leading +, x1 < 5
		// make_hs({-1,0}, 5): lex-leading -, -x1+5 ≤ 0, i.e. x1 ≥ 5
		// Together: x1 < 5 ∧ x1 ≥ 5 = ⊥
		auto h_low = make_hs({1.0, 0.0}, -5.0);
		auto h_high = make_hs({-1.0, 0.0}, 5.0);
		CHECK(is_hsb_zero(h_low & h_high));
	}

	TEST_CASE("Contradictory classifiers: spec-level unrealizability") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Requires output to simultaneously be in H and ¬H
		tref fm = spec(
			"G (o_cls[t]:hsb = {top}:hsb) && "
			"G (o_cls[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK_FALSE(is_tau_formula_sat<node_t>(fm));
	}

	// --- Conservativity: pipeline filters but doesn't extend ---

	TEST_CASE("Conservativity: filtered subset is realizable subset") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Full spec with safety + liveness
		tref full = spec(
			"G (o_safe[t]:hsb != {bot}:hsb) && "
			"G (F (o_live[t]:hsb != {bot}:hsb)).");
		REQUIRE(full != nullptr);
		CHECK(is_tau_formula_sat<node_t>(full));
		// Safety-only filter (conservative subset)
		tref safe_only = spec("G (o_safe[t]:hsb != {bot}:hsb).");
		REQUIRE(safe_only != nullptr);
		CHECK(is_tau_formula_sat<node_t>(safe_only));
	}

} // TEST_SUITE classification halfspace construction LTL(hsb,vec,Q)

TEST_SUITE("hsb — concept halfspace composition") {

	//
	// Tests from class-hbs-3sort.tex v2 (PR #68): contrastive halfspace
	// extraction (w = h⁺ - h⁻), Boolean-combination concept bindings,
	// n-ary relation halfspaces, and containment/disjointness checks.

	// --- Contrastive extraction: w = h⁺ - h⁻, τ = midpoint ---

	TEST_CASE("Contrastive: single concept halfspace nonempty") {
		gc_fixture gc;
		// w = h⁺ - h⁻ for "medical advice", τ = midpoint
		auto h_med = make_hs({2.0, -1.0, 0.5}, -3.0);
		CHECK_FALSE(is_hsb_zero(h_med));
		CHECK_FALSE(is_hsb_zero(~h_med));
	}

	TEST_CASE("Contrastive: multi-pair averaged direction") {
		gc_fixture gc;
		// R1: average of m contrastive pairs — same concept, averaged w
		auto h_avg = make_hs({1.5, -0.8, 0.3}, -2.5);
		CHECK_FALSE(is_hsb_zero(h_avg));
	}

	// --- Boolean-combination concept binding ---

	TEST_CASE("Boolean concept: conjunction of 3 facets nonempty") {
		gc_fixture gc;
		// ⟦medical advice⟧ := H_health ∧ H_prescriptive ∧ ¬H_fictional
		auto h_health = make_hs({1.0, 0.0, 0.0}, -2.0);      // x1 < 2
		auto h_prescr = make_hs({0.0, 1.0, 0.0}, -2.0);      // x2 < 2
		auto h_fiction = make_hs({0.0, 0.0, 1.0}, -2.0);      // x3 < 2
		auto med_advice = h_health & h_prescr & ~h_fiction;
		CHECK_FALSE(is_hsb_zero(med_advice));
	}

	TEST_CASE("Boolean concept: disjunction for broader concept") {
		gc_fixture gc;
		// ⟦harmful⟧ := H_violent ∨ H_illegal ∨ H_discriminatory
		auto h_v = make_hs({1.0, 0.0, 0.0}, -1.0);
		auto h_i = make_hs({0.0, 1.0, 0.0}, -1.0);
		auto h_d = make_hs({0.0, 0.0, 1.0}, -1.0);
		auto harmful = h_v | h_i | h_d;
		CHECK_FALSE(is_hsb_zero(harmful));
		// Each facet is a subset of the disjunction
		CHECK(is_hsb_zero(h_v & ~harmful));
		CHECK(is_hsb_zero(h_i & ~harmful));
		CHECK(is_hsb_zero(h_d & ~harmful));
	}

	// --- Geometric BA composition: concept intersection ---

	TEST_CASE("BA composition: concept intersection = cell intersection") {
		gc_fixture gc;
		// ⟦medical⟧ and ⟦citation⟧ have independent normal vectors
		auto med = make_hs({1.0, 0.0}, -5.0);
		auto cite = make_hs({0.0, 1.0}, -5.0);
		// Intersection is nonempty (medical AND cites)
		CHECK_FALSE(is_hsb_zero(med & cite));
		// Difference regions (medical but no citation, etc.)
		CHECK_FALSE(is_hsb_zero(med & ~cite));
		CHECK_FALSE(is_hsb_zero(~med & cite));
	}

	TEST_CASE("BA composition: 4-concept intersection nontrivial") {
		gc_fixture gc;
		auto c1 = make_hs({1.0, 0.0, 0.0, 0.0}, -3.0);
		auto c2 = make_hs({0.0, 1.0, 0.0, 0.0}, -3.0);
		auto c3 = make_hs({0.0, 0.0, 1.0, 0.0}, -3.0);
		auto c4 = make_hs({0.0, 0.0, 0.0, 1.0}, -3.0);
		CHECK_FALSE(is_hsb_zero(c1 & c2 & c3 & c4));
	}

	// --- Containment and disjointness ---

	TEST_CASE("Containment: narrow ⊂ wide halfspace") {
		gc_fixture gc;
		// H_Doctor is a more restrictive halfspace than H_Person
		// Doctor: x1 < 3, Person: x1 < 10
		auto doctor = make_hs({1.0, 0.0}, -3.0);
		auto person = make_hs({1.0, 0.0}, -10.0);
		CHECK(is_hsb_zero(doctor & ~person));       // Doctor ⊆ Person
		CHECK_FALSE(is_hsb_zero(person & ~doctor)); // Person ⊄ Doctor
	}

	TEST_CASE("Disjointness: opposite halfspaces empty intersection") {
		gc_fixture gc;
		// Disjoint classes: opposite halfspaces
		auto cat = make_hs({1.0, 0.0}, -5.0);      // x1 < 5
		auto dog = make_hs({-1.0, 0.0}, 5.0);      // x1 ≥ 5
		CHECK(is_hsb_zero(cat & dog));
	}

	TEST_CASE("Equivalence: identical halfspaces") {
		gc_fixture gc;
		// Two concept bindings that resolve to same halfspace
		auto a = make_hs({1.0, -1.0}, -3.0);
		auto b = make_hs({1.0, -1.0}, -3.0);
		CHECK(is_hsb_zero(a & ~b));   // a ⊆ b
		CHECK(is_hsb_zero(b & ~a));   // b ⊆ a (equivalent)
	}

	// --- Implication as LTL spec: G(A → B) ---

	TEST_CASE("Implication in LTL: G(A → B) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		tref fm = spec(
			"G (i_medical[t]:sbf = {med}:sbf -> "
			"o_cites[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("Disjointness in LTL: G(¬(A ∧ B)) REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Can always ensure outputs don't simultaneously satisfy both
		tref fm = spec(
			"G (o_cat[t]:hsb != {bot}:hsb -> "
			"o_dog[t]:hsb = {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Binary relation halfspaces ---

	TEST_CASE("Binary relation: answer-pair halfspace nonempty") {
		gc_fixture gc;
		// H_R for "is answer to" — extracted from joint-prompt residuals
		auto h_answer = make_hs({1.0, -0.5, 0.3}, -2.0);
		CHECK_FALSE(is_hsb_zero(h_answer));
	}

	TEST_CASE("Binary relation: contradicts vs entails disjoint") {
		gc_fixture gc;
		// Two binary relations with opposite directions, fully separated
		// contradicts: x1-x2 < 0 (lex-leading +, strict)
		auto h_contradicts = make_hs({1.0, -1.0, 0.0}, 0.0);
		// entails: x1-x2 ≥ 0 (lex-leading -, -x1+x2 ≤ 0)
		auto h_entails = make_hs({-1.0, 1.0, 0.0}, 0.0);
		CHECK(is_hsb_zero(h_contradicts & h_entails));
	}

	// --- Concept library growth under PWR ---

	TEST_CASE("PWR concept growth: 5 concepts remain consistent") {
		gc_fixture gc;
		// Library starts with 2, grows to 5 via PWR revisions
		auto c1 = make_hs({1.0, 0.0, 0.0}, -5.0);
		auto c2 = make_hs({0.0, 1.0, 0.0}, -5.0);
		CHECK_FALSE(is_hsb_zero(c1 & c2));
		// PWR adds 3 more
		auto c3 = make_hs({0.0, 0.0, 1.0}, -5.0);
		auto c4 = make_hs({1.0, 1.0, 0.0}, -8.0);
		auto c5 = make_hs({0.0, 1.0, 1.0}, -8.0);
		CHECK_FALSE(is_hsb_zero(c1 & c2 & c3 & c4 & c5));
	}

	// --- Surface spec with concept bindings ---

	TEST_CASE("Surface: multi-concept spec REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		// 3 concept-based obligations
		tref fm = spec(
			"G (i_med[t]:sbf = {med}:sbf -> "
			"o_cite[t]:hsb != {bot}:hsb) && "
			"G (i_yesno[t]:sbf = {yesno}:sbf -> "
			"F (o_ans[t]:hsb != {bot}:hsb)) && "
			"G (o_safe[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

} // TEST_SUITE concept halfspace composition

// =====================================================================
// Part 9: DL-as-Steering Dual (class-hbs-3sort v4/v5, tau-neuro PRs #69-70)
// =====================================================================
// Validates the "dual" section: same contrastive halfspaces used for
// extraction are also steering directions.  DL axioms translate to
// steering policies; the Mealy dispatches both verdicts and steering
// instructions.  Tests validate algebraic consistency of these patterns.

TEST_SUITE("hsb — halfspace complement and containment") {

	// --- Steering vector direction consistency ---

	TEST_CASE("Complement: +w and -w halfspaces are complements") {
		gc_fixture gc;
		// Concept "medical" direction w = (1,-1,0), threshold 0
		auto h_pos = make_hs({1.0, -1.0, 0.0}, 0.0);   // <w,x> < 0
		auto h_neg = make_hs({-1.0, 1.0, 0.0}, 0.0);    // <-w,x> ≤ 0
		// +w and -w partition: their disjunction covers everything
		CHECK(is_hsb_one(h_pos | h_neg));
		// Their conjunction is empty (strict + non-strict at same boundary)
		CHECK(is_hsb_zero(h_pos & h_neg));
	}

	TEST_CASE("Shifted halfspace stays within parent region") {
		gc_fixture gc;
		// Concept A region: x1 < 3  →  make_hs({1,0,0}, -3)
		auto H_A = make_hs({1.0, 0.0, 0.0}, -3.0);
		// Sub-region closer to concept center: x1 < 1
		auto H_deeper = make_hs({1.0, 0.0, 0.0}, -1.0);
		// Steering deeper into A is consistent: H_deeper ⊆ H_A
		CHECK(is_hsb_zero(H_deeper & ~H_A));
	}

	// --- DL axiom steering policies ---

	TEST_CASE("Containment: A fires → B region nonempty") {
		gc_fixture gc;
		// SubClassOf(A, B): A ⊆ B
		auto H_A = make_hs({1.0, 0.0, 0.0}, -2.0);  // x1 < 2
		auto H_B = make_hs({1.0, 0.0, 0.0}, -5.0);  // x1 < 5
		CHECK(is_hsb_zero(H_A & ~H_B));  // A ⊆ B
		// Steering toward B when in A is sound: A & B = A (trivially)
		CHECK_FALSE(is_hsb_zero(H_A & H_B));
	}

	TEST_CASE("Complement subtraction: A minus B nonempty") {
		gc_fixture gc;
		// DisjointClasses(A, B): A ∩ B = ∅
		// A: x1 < 2 (lex-leading +, strict)
		auto H_A = make_hs({1.0, 0.0, 0.0}, -2.0);
		// B: x1 ≥ 3 → -x1+3 ≤ 0 → make_hs({-1,0,0}, 3)
		auto H_B = make_hs({-1.0, 0.0, 0.0}, 3.0);
		CHECK(is_hsb_zero(H_A & H_B));
		// Steering away from B when in A: ~B covers A entirely
		CHECK(is_hsb_zero(H_A & ~(~H_B)));
	}

	TEST_CASE("Multi-halfspace conjunction nonempty") {
		gc_fixture gc;
		// When H_A fires, both H_R and H_C must be steered toward
		auto H_A = make_hs({1.0, 0.0, 0.0}, -2.0);  // concept A
		auto H_R = make_hs({0.0, 1.0, 0.0}, -1.0);  // relation R
		auto H_C = make_hs({0.0, 0.0, 1.0}, -1.0);  // concept C
		// Steering target is R ∩ C: both directions active
		auto steer_target = H_R & H_C;
		CHECK_FALSE(is_hsb_zero(steer_target));
		// A ∩ steer_target is non-empty (consistent scheduling)
		CHECK_FALSE(is_hsb_zero(H_A & steer_target));
	}

	TEST_CASE("Sequential subtraction yields nonempty remainder") {
		gc_fixture gc;
		// R fires: x2 < 0
		auto H_R = make_hs({0.0, 1.0, 0.0}, 0.0);
		// After R fires once, steer away: ~H_R
		auto steer_away = ~H_R;
		// R & steer_away = ∅ (correctly suppresses second firing)
		CHECK(is_hsb_zero(H_R & steer_away));
		// steer_away covers complement: allows non-R
		CHECK(is_hsb_one(H_R | steer_away));
	}

	// --- Multi-direction steering composition ---

	TEST_CASE("Halfspace composition: conjunction of shifted regions") {
		gc_fixture gc;
		// S+ = {w_medical, w_citation}, S- = {w_fiction}
		auto w_med  = make_hs({1.0, 0.0, 0.0}, -1.0);   // steer toward medical
		auto w_cite = make_hs({0.0, 1.0, 0.0}, -1.0);   // steer toward citation
		auto w_fict = make_hs({0.0, 0.0, 1.0}, -1.0);   // steer away from fiction
		// Combined steering target: medical ∩ citation ∩ ¬fiction
		auto combined = w_med & w_cite & ~w_fict;
		CHECK_FALSE(is_hsb_zero(combined));
	}

	TEST_CASE("Halfspace composition: conflicting regions → empty") {
		gc_fixture gc;
		// Steer toward A and away from A simultaneously → contradiction
		auto H_A = make_hs({1.0, 0.0, 0.0}, -2.0);
		auto toward_A = H_A;
		auto away_A = ~H_A;
		CHECK(is_hsb_zero(toward_A & away_A));
	}

	// --- Mealy as steering dispatcher ---

	TEST_CASE("Multi-output LTL(hsb): verdict + region product REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Mealy outputs both verdict (accept/reject) and steering set
		// Model as product spec: hard invariant AND soft steering hint
		// Hard: G(input → output safe)
		// Soft: steering direction consistent with hard invariant
		auto H_safe   = make_hs({1.0, 0.0}, -1.0);  // safe region
		auto H_steer  = make_hs({1.0, 0.0}, -0.5);  // deeper safe (steering)
		// Steering sub-region ⊆ safe region
		CHECK(is_hsb_zero(H_steer & ~H_safe));
		// Hard spec: G(safe)
		tref fm = spec("G (o_v[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
	}

	TEST_CASE("Multi-output LTL(hsb): soft region does not weaken invariant REALIZABLE") {
		gc_fixture gc;
		// Hard invariant: A ⊆ B (SubClassOf enforced by Mealy)
		auto H_A = make_hs({1.0, 0.0, 0.0}, -2.0);
		auto H_B = make_hs({1.0, 0.0, 0.0}, -5.0);
		CHECK(is_hsb_zero(H_A & ~H_B));
		// Soft steering: push deeper into B (x1 < 3)
		auto H_steer_B = make_hs({1.0, 0.0, 0.0}, -3.0);
		// Steered region still satisfies hard invariant
		CHECK(is_hsb_zero(H_steer_B & ~H_B));
		// And still covers A: A ⊆ steer_B ⊆ B
		CHECK(is_hsb_zero(H_A & ~H_steer_B));
	}

	// --- Complementarity: steering ≠ guarantee ---

	TEST_CASE("Soft region complement is nonempty (non-trivial)") {
		gc_fixture gc;
		// Even with strong steering, the Mealy must still verify
		// Model: wide region H_steer (soft) vs narrow invariant H_hard
		auto H_steer = make_hs({1.0, 0.0, 0.0}, -10.0); // x1 < 10
		auto H_hard  = make_hs({1.0, 0.0, 0.0}, -2.0);  // x1 < 2
		// Steering is weaker: H_hard ⊂ H_steer (strict subset)
		CHECK(is_hsb_zero(H_hard & ~H_steer));            // hard ⊆ steer
		CHECK_FALSE(is_hsb_zero(H_steer & ~H_hard));      // steer ⊄ hard
		// The Mealy enforces H_hard; steering only biases toward H_steer
	}

	TEST_CASE("Joint hard+soft LTL(hsb) spec REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Product spec: hard safety invariant + soft liveness goal
		tref fm = spec(
			"G (o_safe[t]:hsb != {bot}:hsb) && "
			"F (o_goal[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	// --- Linear-separability limit: polyhedral cells ---

	TEST_CASE("Linear-separability: Boolean cell extends beyond single HS") {
		gc_fixture gc;
		// Single halfspace: linear boundary. Boolean combination: polyhedral.
		auto h1 = make_hs({1.0, 0.0}, -3.0);    // x1 < 3
		auto h2 = make_hs({-1.0, 0.0}, -1.0);  // -x1 + (-1) ≤ 0 → x1 ≥ -1
		auto h3 = make_hs({0.0, 1.0}, -3.0);   // x2 < 3
		auto h4 = make_hs({0.0, -1.0}, -1.0);  // -x2 + (-1) ≤ 0 → x2 ≥ -1
		// Rectangular cell: -1 ≤ x1 < 3 ∧ -1 ≤ x2 < 3
		auto rect = h1 & h2 & h3 & h4;
		CHECK_FALSE(is_hsb_zero(rect));
		// Not representable by any single halfspace
	}

	TEST_CASE("Linear-separability: 5-facet polyhedral cell non-empty") {
		gc_fixture gc;
		// 5 halfspaces forming a pentagonal-like cell in 3D
		auto f1 = make_hs({1.0, 0.0, 0.0}, -5.0);
		auto f2 = make_hs({0.0, 1.0, 0.0}, -5.0);
		auto f3 = make_hs({0.0, 0.0, 1.0}, -5.0);
		auto f4 = make_hs({-1.0, -1.0, 0.0}, 0.0);
		auto f5 = make_hs({0.0, -1.0, -1.0}, 0.0);
		auto cell = f1 & f2 & f3 & f4 & f5;
		CHECK_FALSE(is_hsb_zero(cell));
	}

	// --- Complement and containment patterns in LTL ---

	TEST_CASE("LTL: G(A → B) implication REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Hard enforcement: G(A → B)
		tref fm = spec(
			"G (i_a[t]:sbf = {a}:sbf -> o_b[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

	TEST_CASE("LTL: G(¬(A ∧ B)) + F(A) + F(B) disjoint regions REALIZABLE") {
		gc_fixture gc;
		bdd_init<Bool>();
		// Disjoint concepts: never both at same time, but each reachable
		tref fm = spec(
			"G !(o_a[t]:hsb != {bot}:hsb && o_b[t]:hsb != {bot}:hsb) && "
			"F (o_a[t]:hsb != {bot}:hsb) && "
			"F (o_b[t]:hsb != {bot}:hsb).");
		REQUIRE(fm != nullptr);
		CHECK(is_tau_formula_sat<node_t>(fm));
	}

} // TEST_SUITE halfspace complement and containment

TEST_SUITE("hsb — eval_parse_tree") {

	// ─── Constants ───────────────────────────────────────────────────────────────

	TEST_CASE("eval: top") {
		auto r = eval_from_str("top");
		REQUIRE(r.has_value());
		CHECK(*r == hsb::top());
	}

	TEST_CASE("eval: bot") {
		auto r = eval_from_str("bot");
		REQUIRE(r.has_value());
		CHECK(*r == hsb::bottom());
	}

	TEST_CASE("eval: bottom") {
		auto r = eval_from_str("bottom");
		REQUIRE(r.has_value());
		CHECK(*r == hsb::bottom());
	}

	// ─── Simple halfspaces ───────────────────────────────────────────────────────

	TEST_CASE("eval: x[0] < 0") {
		auto r = eval_from_str("x[0] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({1.0}, 0.0));
	}

	TEST_CASE("eval: x[0] <= 0 same canonical result as x[0] < 0") {
		auto r = eval_from_str("x[0] <= 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({1.0}, 0.0));
	}

	TEST_CASE("eval: 2*x[0] < 0 (coeff*var form)") {
		auto r = eval_from_str("2*x[0] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({2.0}, 0.0));
	}

	TEST_CASE("eval: x[0]*3 < 0 (var*coeff form)") {
		auto r = eval_from_str("x[0]*3 < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({3.0}, 0.0));
	}

	TEST_CASE("eval: -x[0] < 0 (unary minus on var)") {
		auto r = eval_from_str("-x[0] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({-1.0}, 0.0));
	}

	TEST_CASE("eval: -2*x[0] < 0 (neg coeff*var form)") {
		auto r = eval_from_str("-2*x[0] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({-2.0}, 0.0));
	}

	TEST_CASE("eval: 1/2*x[0] < 0 (fractional coefficient)") {
		auto r = eval_from_str("1/2*x[0] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({0.5}, 0.0));
	}

	TEST_CASE("eval: 1.5*x[0] < 0 (decimal coefficient)") {
		auto r = eval_from_str("1.5*x[0] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({1.5}, 0.0));
	}

	// ─── Bias term ───────────────────────────────────────────────────────────────

	TEST_CASE("eval: x[0] + 1 < 0 (positive bias)") {
		auto r = eval_from_str("x[0] + 1 < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({1.0}, 1.0));
	}

	TEST_CASE("eval: x[0] - 1 < 0 (negative bias)") {
		auto r = eval_from_str("x[0] - 1 < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({1.0}, -1.0));
	}

	TEST_CASE("eval: x[0] + 1/2 < 0 (fractional bias)") {
		auto r = eval_from_str("x[0] + 1/2 < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({1.0}, 0.5));
	}

	// ─── Multi-variable ──────────────────────────────────────────────────────────

	TEST_CASE("eval: x[0] + x[1] < 0 (2D)") {
		auto r = eval_from_str("x[0] + x[1] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({1.0, 1.0}, 0.0));
	}

	TEST_CASE("eval: 2*x[0] + 3*x[1] < 0 (2D with coefficients)") {
		auto r = eval_from_str("2*x[0] + 3*x[1] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({2.0, 3.0}, 0.0));
	}

	TEST_CASE("eval: x[0] + x[1] + x[2] < 0 (3D)") {
		auto r = eval_from_str("x[0] + x[1] + x[2] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({1.0, 1.0, 1.0}, 0.0));
	}

	// ─── Coefficient accumulation for repeated variables ─────────────────────────

	TEST_CASE("eval: 3*x[0] + 2*x[0] accumulates to w[0]=5") {
		auto r = eval_from_str("3*x[0] + 2*x[0] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({5.0}, 0.0));
		CHECK(*r != make_hs({2.0}, 0.0));  // not overwritten by second term
		CHECK(*r != make_hs({3.0}, 0.0));  // not left as first term
	}

	TEST_CASE("eval: 3*x[0] - 2*x[0] accumulates to w[0]=1") {
		auto r = eval_from_str("3*x[0] - 2*x[0] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({1.0}, 0.0));
	}

	TEST_CASE("eval: x[0] + x[0] accumulates to w[0]=2") {
		auto r = eval_from_str("x[0] + x[0] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({2.0}, 0.0));
	}

	TEST_CASE("eval: 5*x[0] - 3*x[0] - x[0] accumulates to w[0]=1") {
		auto r = eval_from_str("5*x[0] - 3*x[0] - x[0] < 0");
		REQUIRE(r.has_value());
		CHECK(*r == make_hs({1.0}, 0.0));
	}

	// ─── Boolean operations ──────────────────────────────────────────────────────

	TEST_CASE("eval: ~top == bot") {
		auto r = eval_from_str("~top");
		REQUIRE(r.has_value());
		CHECK(*r == hsb::bottom());
	}

	TEST_CASE("eval: ~bot == top") {
		auto r = eval_from_str("~bot");
		REQUIRE(r.has_value());
		CHECK(*r == hsb::top());
	}

	TEST_CASE("eval: ~(x[0] < 0) satisfies complement law") {
		auto h   = eval_from_str("x[0] < 0");
		auto neg = eval_from_str("~(x[0] < 0)");
		REQUIRE(h.has_value());
		REQUIRE(neg.has_value());
		CHECK(is_hsb_zero(*h & *neg));
		CHECK(is_hsb_one(*h | *neg));
	}

	TEST_CASE("eval: ~~(x[0] < 0) == x[0] < 0") {
		auto r  = eval_from_str("x[0] < 0");
		auto rr = eval_from_str("~~(x[0] < 0)");
		REQUIRE(r.has_value());
		REQUIRE(rr.has_value());
		CHECK(*r == *rr);
	}

	TEST_CASE("eval: (x[0] < 0 & ~(x[0] < 0)) == bot") {
		auto r = eval_from_str("(x[0] < 0 & ~(x[0] < 0))");
		REQUIRE(r.has_value());
		CHECK(is_hsb_zero(*r));
	}

	TEST_CASE("eval: (x[0] < 0 | ~(x[0] < 0)) == top") {
		auto r = eval_from_str("(x[0] < 0 | ~(x[0] < 0))");
		REQUIRE(r.has_value());
		CHECK(is_hsb_one(*r));
	}

	TEST_CASE("eval: conjunction (x[0] < 0 & x[1] < 0)") {
		auto r = eval_from_str("(x[0] < 0 & x[1] < 0)");
		REQUIRE(r.has_value());
		CHECK(*r == (make_hs({1.0}, 0.0) & make_hs({0.0, 1.0}, 0.0)));
	}

	TEST_CASE("eval: disjunction (x[0] < 0 | x[1] < 0)") {
		auto r = eval_from_str("(x[0] < 0 | x[1] < 0)");
		REQUIRE(r.has_value());
		CHECK(*r == (make_hs({1.0}, 0.0) | make_hs({0.0, 1.0}, 0.0)));
	}

	// ─── Paren (transparent) ─────────────────────────────────────────────────────

	TEST_CASE("eval: (x[0] < 0) paren is transparent") {
		auto plain  = eval_from_str("x[0] < 0");
		auto parens = eval_from_str("(x[0] < 0)");
		REQUIRE(plain.has_value());
		REQUIRE(parens.has_value());
		CHECK(*plain == *parens);
	}

	// ─── Invalid input returns nullopt ───────────────────────────────────────────

	TEST_CASE("eval: unsupported operator returns nullopt") {
		CHECK_FALSE(eval_from_str("x[0] > 0").has_value());
	}

	TEST_CASE("eval: empty string returns nullopt") {
		CHECK_FALSE(eval_from_str("").has_value());
	}

	TEST_CASE("eval: missing comparison returns nullopt") {
		CHECK_FALSE(eval_from_str("x[0]").has_value());
	}

} // TEST_SUITE eval_parse_tree
