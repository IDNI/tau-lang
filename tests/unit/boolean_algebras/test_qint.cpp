// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Comprehensive unit tests for qint — atomless Boolean algebra of dyadic
// rational intervals [a, b) (left-closed, right-open) on the extended real line.
// Tests cover: qint_rational (parse, to_string, comparisons, midpoint),
// qint construction, BA operations, Boolean algebra laws, normalisation,
// is_zero/is_one, ordering, splitter, stream output, and hashing.

#include <sstream>

#include "test_init.h"
#include "boolean_algebras/qint.h"

using idni::tau_lang::qint;
using idni::tau_lang::qint_rational;
using idni::tau_lang::qint_splitter;
using idni::tau_lang::qint_splitter_one;
using idni::tau_lang::is_qint_zero;
using idni::tau_lang::is_qint_one;
using idni::tau_lang::normalize_qint;
using idni::tau_lang::splitter_type;

// 2^SCALE — fixed-point unit: raw value representing the integer 1
// Named QU ("qint unit") to avoid collision with idni::tau_lang::S() from dict.h
static constexpr long long QU = 1LL << qint_rational::SCALE;

// Construct a single-piece qint [lo_raw, hi_raw)
static qint qi(long long lo_raw, long long hi_raw) {
	return qint{{ {qint_rational{lo_raw}, qint_rational{hi_raw}} }};
}

// ============================================================================
TEST_SUITE("qint_rational — construction and sentinels") {
// ============================================================================

TEST_CASE("default construction is zero") {
	qint_rational r;
	CHECK(r.raw == 0);
	CHECK(r.is_finite());
	CHECK_FALSE(r.is_pos_inf());
	CHECK_FALSE(r.is_neg_inf());
}

TEST_CASE("explicit construction from raw") {
	qint_rational r{42};
	CHECK(r.raw == 42);
}

TEST_CASE("pos_inf sentinel") {
	qint_rational r{qint_rational::POS_INF};
	CHECK(r.is_pos_inf());
	CHECK_FALSE(r.is_finite());
	CHECK_FALSE(r.is_neg_inf());
}

TEST_CASE("neg_inf sentinel") {
	qint_rational r{qint_rational::NEG_INF};
	CHECK(r.is_neg_inf());
	CHECK_FALSE(r.is_finite());
	CHECK_FALSE(r.is_pos_inf());
}

TEST_CASE("finite value is not an infinity") {
	CHECK(qint_rational{0}.is_finite());
	CHECK(qint_rational{QU}.is_finite());
	CHECK(qint_rational{-QU}.is_finite());
}

} // TEST_SUITE qint_rational — construction and sentinels

// ============================================================================
TEST_SUITE("qint_rational — comparison operators") {
// ============================================================================

TEST_CASE("equality and inequality") {
	qint_rational a{0}, b{QU};
	CHECK(a == a);
	CHECK(a != b);
	CHECK_FALSE(a == b);
}

TEST_CASE("less-than ordering") {
	qint_rational a{0}, b{QU}, c{2*QU};
	CHECK(a < b);
	CHECK(b < c);
	CHECK_FALSE(b < a);
}

TEST_CASE("less-or-equal") {
	qint_rational a{0}, b{QU};
	CHECK(a <= b);
	CHECK(a <= a);
	CHECK_FALSE(b <= a);
}

TEST_CASE("greater-than ordering") {
	qint_rational a{2*QU}, b{QU};
	CHECK(a > b);
	CHECK_FALSE(b > a);
}

TEST_CASE("greater-or-equal") {
	qint_rational a{QU}, b{QU};
	CHECK(a >= b);
	qint_rational c{2*QU};
	CHECK(c >= a);
}

TEST_CASE("neg_inf is smallest") {
	qint_rational neg{qint_rational::NEG_INF};
	CHECK(neg < qint_rational{0});
	CHECK(neg < qint_rational{qint_rational::POS_INF});
}

TEST_CASE("pos_inf is largest") {
	qint_rational pos{qint_rational::POS_INF};
	CHECK(qint_rational{0} < pos);
	CHECK(qint_rational{qint_rational::NEG_INF} < pos);
}

} // TEST_SUITE qint_rational — comparison operators

// ============================================================================
TEST_SUITE("qint_rational — midpoint") {
// ============================================================================

TEST_CASE("midpoint of 0 and 2") {
	qint_rational a{0}, b{2*QU};
	auto mid = a.midpoint(b);
	CHECK(mid.raw == QU);
}

TEST_CASE("midpoint of 0 and 1/2") {
	qint_rational a{0}, b{QU/2};
	auto mid = a.midpoint(b);
	CHECK(mid.raw == QU/4);
}

TEST_CASE("midpoint is strictly between endpoints") {
	qint_rational a{0}, b{QU};
	auto mid = a.midpoint(b);
	CHECK(mid > a);
	CHECK(mid < b);
}

TEST_CASE("midpoint of symmetric interval around zero") {
	qint_rational a{-QU}, b{QU};
	auto mid = a.midpoint(b);
	CHECK(mid.raw == 0);
}

TEST_CASE("midpoint of two equal values") {
	qint_rational a{QU};
	auto mid = a.midpoint(a);
	CHECK(mid.raw == QU);
}

} // TEST_SUITE qint_rational — midpoint

// ============================================================================
TEST_SUITE("qint_rational — parse") {
// ============================================================================

TEST_CASE("parse +inf") {
	qint_rational r;
	CHECK(qint_rational::parse("+inf", r));
	CHECK(r.is_pos_inf());
}

TEST_CASE("parse inf") {
	qint_rational r;
	CHECK(qint_rational::parse("inf", r));
	CHECK(r.is_pos_inf());
}

TEST_CASE("parse +infinity") {
	qint_rational r;
	CHECK(qint_rational::parse("+infinity", r));
	CHECK(r.is_pos_inf());
}

TEST_CASE("parse -inf") {
	qint_rational r;
	CHECK(qint_rational::parse("-inf", r));
	CHECK(r.is_neg_inf());
}

TEST_CASE("parse -infinity") {
	qint_rational r;
	CHECK(qint_rational::parse("-infinity", r));
	CHECK(r.is_neg_inf());
}

TEST_CASE("parse integer 0") {
	qint_rational r;
	CHECK(qint_rational::parse("0", r));
	CHECK(r.raw == 0);
}

TEST_CASE("parse integer 1") {
	qint_rational r;
	CHECK(qint_rational::parse("1", r));
	CHECK(r.raw == QU);
}

TEST_CASE("parse integer -1") {
	qint_rational r;
	CHECK(qint_rational::parse("-1", r));
	CHECK(r.raw == -QU);
}

TEST_CASE("parse integer 2") {
	qint_rational r;
	CHECK(qint_rational::parse("2", r));
	CHECK(r.raw == 2*QU);
}

TEST_CASE("parse integer with surrounding whitespace") {
	qint_rational r;
	CHECK(qint_rational::parse("  1  ", r));
	CHECK(r.raw == QU);
}

TEST_CASE("parse fraction 1/2") {
	qint_rational r;
	CHECK(qint_rational::parse("1/2", r));
	CHECK(r.raw == QU/2);
}

TEST_CASE("parse fraction 1/4") {
	qint_rational r;
	CHECK(qint_rational::parse("1/4", r));
	CHECK(r.raw == QU/4);
}

TEST_CASE("parse fraction 3/4") {
	qint_rational r;
	CHECK(qint_rational::parse("3/4", r));
	CHECK(r.raw == 3*(QU/4));
}

TEST_CASE("parse fraction -1/2") {
	qint_rational r;
	CHECK(qint_rational::parse("-1/2", r));
	CHECK(r.raw == -(QU/2));
}

TEST_CASE("parse fraction -3/4") {
	qint_rational r;
	CHECK(qint_rational::parse("-3/4", r));
	CHECK(r.raw == -(3*(QU/4)));
}

TEST_CASE("parse fraction 1/1 equals integer 1") {
	qint_rational r;
	CHECK(qint_rational::parse("1/1", r));
	CHECK(r.raw == QU);
}

TEST_CASE("parse decimal 0.5") {
	qint_rational r;
	CHECK(qint_rational::parse("0.5", r));
	CHECK(r.raw == QU/2);
}

TEST_CASE("parse decimal 0.25") {
	qint_rational r;
	CHECK(qint_rational::parse("0.25", r));
	CHECK(r.raw == QU/4);
}

TEST_CASE("parse decimal 0.75") {
	qint_rational r;
	CHECK(qint_rational::parse("0.75", r));
	CHECK(r.raw == 3*(QU/4));
}

TEST_CASE("parse decimal -0.5") {
	qint_rational r;
	CHECK(qint_rational::parse("-0.5", r));
	CHECK(r.raw == -(QU/2));
}

TEST_CASE("parse decimal 1.5") {
	qint_rational r;
	CHECK(qint_rational::parse("1.5", r));
	CHECK(r.raw == QU + QU/2);
}

TEST_CASE("parse decimal and fraction agree on 1/2") {
	qint_rational rfrac, rdec;
	qint_rational::parse("1/2", rfrac);
	qint_rational::parse("0.5", rdec);
	CHECK(rfrac == rdec);
}

TEST_CASE("parse decimal and fraction agree on 1/4") {
	qint_rational rfrac, rdec;
	qint_rational::parse("1/4", rfrac);
	qint_rational::parse("0.25", rdec);
	CHECK(rfrac == rdec);
}

TEST_CASE("parse rejects non-dyadic fraction 1/3") {
	qint_rational r;
	CHECK_FALSE(qint_rational::parse("1/3", r));
}

TEST_CASE("parse rejects non-dyadic fraction 2/3") {
	qint_rational r;
	CHECK_FALSE(qint_rational::parse("2/3", r));
}

TEST_CASE("parse rejects non-dyadic decimal 0.1") {
	qint_rational r;
	CHECK_FALSE(qint_rational::parse("0.1", r));
}

TEST_CASE("parse rejects non-dyadic decimal 0.3") {
	qint_rational r;
	CHECK_FALSE(qint_rational::parse("0.3", r));
}

TEST_CASE("parse rejects empty string") {
	qint_rational r;
	CHECK_FALSE(qint_rational::parse("", r));
}

TEST_CASE("parse rejects alphabetic junk") {
	qint_rational r;
	CHECK_FALSE(qint_rational::parse("abc", r));
}

TEST_CASE("parse rejects zero denominator") {
	qint_rational r;
	CHECK_FALSE(qint_rational::parse("1/0", r));
}

TEST_CASE("parse rejects non-power-of-2 denominator 6") {
	qint_rational r;
	CHECK_FALSE(qint_rational::parse("1/6", r));
}

} // TEST_SUITE qint_rational — parse

// ============================================================================
TEST_SUITE("qint_rational — to_string") {
// ============================================================================

TEST_CASE("to_string of 0") {
	CHECK(qint_rational{0}.to_string() == "0");
}

TEST_CASE("to_string of integer 1") {
	CHECK(qint_rational{QU}.to_string() == "1");
}

TEST_CASE("to_string of integer -1") {
	CHECK(qint_rational{-QU}.to_string() == "-1");
}

TEST_CASE("to_string of integer 2") {
	CHECK(qint_rational{2*QU}.to_string() == "2");
}

TEST_CASE("to_string of 1/2") {
	CHECK(qint_rational{QU/2}.to_string() == "1/2");
}

TEST_CASE("to_string of 1/4") {
	CHECK(qint_rational{QU/4}.to_string() == "1/4");
}

TEST_CASE("to_string of 3/4") {
	CHECK(qint_rational{3*(QU/4)}.to_string() == "3/4");
}

TEST_CASE("to_string of -1/2") {
	CHECK(qint_rational{-(QU/2)}.to_string() == "-1/2");
}

TEST_CASE("to_string of +inf") {
	CHECK(qint_rational{qint_rational::POS_INF}.to_string() == "+inf");
}

TEST_CASE("to_string of -inf") {
	CHECK(qint_rational{qint_rational::NEG_INF}.to_string() == "-inf");
}

TEST_CASE("parse/to_string roundtrip for 1/4") {
	qint_rational r;
	CHECK(qint_rational::parse("1/4", r));
	CHECK(r.to_string() == "1/4");
}

TEST_CASE("parse/to_string roundtrip for 3/4") {
	qint_rational r;
	CHECK(qint_rational::parse("3/4", r));
	CHECK(r.to_string() == "3/4");
}

TEST_CASE("parse/to_string roundtrip for -3/4") {
	qint_rational r;
	CHECK(qint_rational::parse("-3/4", r));
	CHECK(r.to_string() == "-3/4");
}

} // TEST_SUITE qint_rational — to_string

// ============================================================================
TEST_SUITE("qint — basic construction") {
// ============================================================================

TEST_CASE("bottom is empty") {
	auto bot = qint::bottom();
	CHECK(bot.is_empty());
	CHECK_FALSE(bot.is_full());
	CHECK(bot == false);
	CHECK(bot != true);
	CHECK(bot.pieces.empty());
}

TEST_CASE("top is full") {
	auto top = qint::top();
	CHECK(top.is_full());
	CHECK_FALSE(top.is_empty());
	CHECK(top == true);
	CHECK(top != false);
	CHECK(top.pieces.size() == 1);
	CHECK(top.pieces[0].lo.is_neg_inf());
	CHECK(top.pieces[0].hi.is_pos_inf());
}

TEST_CASE("top and bottom are not equal") {
	CHECK(qint::top() != qint::bottom());
}

TEST_CASE("single interval [0, 1)") {
	auto q = qi(0, QU);
	CHECK_FALSE(q.is_empty());
	CHECK_FALSE(q.is_full());
	CHECK(q.pieces.size() == 1);
	CHECK(q.pieces[0].lo.raw == 0);
	CHECK(q.pieces[0].hi.raw == QU);
}

TEST_CASE("single interval is not bottom or top") {
	auto q = qi(0, QU);
	CHECK(q != qint::bottom());
	CHECK(q != qint::top());
}

TEST_CASE("two identical qints are equal") {
	CHECK(qi(0, QU) == qi(0, QU));
}

TEST_CASE("two different qints are not equal") {
	CHECK(qi(0, QU) != qi(QU, 2*QU));
}

TEST_CASE("interval [-inf, 0) is not bottom") {
	auto q = qi(qint_rational::NEG_INF, 0);
	CHECK_FALSE(q.is_empty());
	CHECK_FALSE(q.is_full());
}

TEST_CASE("interval [0, +inf) is not top") {
	auto q = qi(0, qint_rational::POS_INF);
	CHECK_FALSE(q.is_empty());
	CHECK_FALSE(q.is_full());
}

TEST_CASE("interval [-inf, +inf) equals top") {
	auto q = qi(qint_rational::NEG_INF, qint_rational::POS_INF);
	CHECK(q.is_full());
	CHECK(q == qint::top());
}

TEST_CASE("piece equality") {
	qint::piece p1{qint_rational{0}, qint_rational{QU}};
	qint::piece p2{qint_rational{0}, qint_rational{QU}};
	qint::piece p3{qint_rational{QU}, qint_rational{2*QU}};
	CHECK(p1 == p2);
	CHECK_FALSE(p1 == p3);
}

} // TEST_SUITE qint — basic construction

// ============================================================================
TEST_SUITE("qint — BA operations") {
// ============================================================================

TEST_CASE("union: identity with bottom") {
	auto a = qi(0, QU);
	CHECK((a | qint::bottom()) == a);
	CHECK((qint::bottom() | a) == a);
}

TEST_CASE("union: absorb into top") {
	auto a = qi(0, QU);
	CHECK((a | qint::top()) == qint::top());
	CHECK((qint::top() | a) == qint::top());
}

TEST_CASE("intersection: identity with top") {
	auto a = qi(0, QU);
	CHECK((a & qint::top()) == a);
	CHECK((qint::top() & a) == a);
}

TEST_CASE("intersection: absorb into bottom") {
	auto a = qi(0, QU);
	CHECK((a & qint::bottom()) == qint::bottom());
	CHECK((qint::bottom() & a) == qint::bottom());
}

TEST_CASE("complement of bottom is top") {
	CHECK(~qint::bottom() == qint::top());
}

TEST_CASE("complement of top is bottom") {
	CHECK(~qint::top() == qint::bottom());
}

TEST_CASE("double complement is identity") {
	auto a = qi(0, QU);
	CHECK(~~a == a);
}

TEST_CASE("double complement of top") {
	CHECK(~~qint::top() == qint::top());
}

TEST_CASE("double complement of bottom") {
	CHECK(~~qint::bottom() == qint::bottom());
}

TEST_CASE("complement of [0, 1) is [-inf, 0) | [1, +inf)") {
	auto c = ~qi(0, QU);
	CHECK(c.pieces.size() == 2);
	CHECK(c.pieces[0].lo.is_neg_inf());
	CHECK(c.pieces[0].hi.raw == 0);
	CHECK(c.pieces[1].lo.raw == QU);
	CHECK(c.pieces[1].hi.is_pos_inf());
}

TEST_CASE("complement of [-inf, 0) is [0, +inf)") {
	auto c = ~qi(qint_rational::NEG_INF, 0);
	CHECK(c.pieces.size() == 1);
	CHECK(c.pieces[0].lo.raw == 0);
	CHECK(c.pieces[0].hi.is_pos_inf());
}

TEST_CASE("complement of [0, +inf) is [-inf, 0)") {
	auto c = ~qi(0, qint_rational::POS_INF);
	CHECK(c.pieces.size() == 1);
	CHECK(c.pieces[0].lo.is_neg_inf());
	CHECK(c.pieces[0].hi.raw == 0);
}

TEST_CASE("complement law: A & ~A == bot") {
	auto a = qi(0, QU);
	CHECK((a & ~a) == qint::bottom());
}

TEST_CASE("complement law: A | ~A == top") {
	auto a = qi(0, QU);
	CHECK((a | ~a) == qint::top());
}

TEST_CASE("complement of two-piece union") {
	auto a = qi(0, QU);
	auto b = qi(2*QU, 3*QU);
	auto u = a | b;
	auto c = ~u;
	// complement of [0,1)|[2,3) is [-inf,0)|[1,2)|[3,+inf)
	CHECK(c.pieces.size() == 3);
	CHECK(c.pieces[0].lo.is_neg_inf());
	CHECK(c.pieces[0].hi.raw == 0);
	CHECK(c.pieces[1].lo.raw == QU);
	CHECK(c.pieces[1].hi.raw == 2*QU);
	CHECK(c.pieces[2].lo.raw == 3*QU);
	CHECK(c.pieces[2].hi.is_pos_inf());
}

TEST_CASE("union of two disjoint intervals has two pieces") {
	auto u = qi(0, QU) | qi(2*QU, 3*QU);
	CHECK(u.pieces.size() == 2);
	CHECK(u.pieces[0].lo.raw == 0);
	CHECK(u.pieces[0].hi.raw == QU);
	CHECK(u.pieces[1].lo.raw == 2*QU);
	CHECK(u.pieces[1].hi.raw == 3*QU);
}

TEST_CASE("union of overlapping intervals merges into one piece") {
	auto u = qi(0, 2*QU) | qi(QU, 3*QU);
	CHECK(u.pieces.size() == 1);
	CHECK(u.pieces[0].lo.raw == 0);
	CHECK(u.pieces[0].hi.raw == 3*QU);
}

TEST_CASE("union of adjacent intervals merges into one piece") {
	auto u = qi(0, QU) | qi(QU, 2*QU);
	CHECK(u.pieces.size() == 1);
	CHECK(u.pieces[0].lo.raw == 0);
	CHECK(u.pieces[0].hi.raw == 2*QU);
}

TEST_CASE("union of same interval is idempotent") {
	auto a = qi(0, QU);
	CHECK((a | a) == a);
}

TEST_CASE("intersection of overlapping intervals") {
	auto inter = qi(0, 2*QU) & qi(QU, 3*QU);
	CHECK(inter.pieces.size() == 1);
	CHECK(inter.pieces[0].lo.raw == QU);
	CHECK(inter.pieces[0].hi.raw == 2*QU);
}

TEST_CASE("intersection of disjoint intervals is empty") {
	CHECK((qi(0, QU) & qi(2*QU, 3*QU)) == qint::bottom());
}

TEST_CASE("intersection of adjacent half-open intervals is empty") {
	// [0,1) and [1,2) share endpoint 1 but for half-open intervals the intersection is empty
	CHECK((qi(0, QU) & qi(QU, 2*QU)) == qint::bottom());
}

TEST_CASE("intersection of same interval is idempotent") {
	auto a = qi(0, QU);
	CHECK((a & a) == a);
}

TEST_CASE("intersection with containing interval equals inner") {
	auto inner = qi(QU/4, 3*(QU/4));
	auto outer = qi(0, QU);
	CHECK((inner & outer) == inner);
}

TEST_CASE("xor with self is bottom") {
	auto a = qi(0, QU);
	CHECK((a ^ a) == qint::bottom());
}

TEST_CASE("xor with bottom is identity") {
	auto a = qi(0, QU);
	CHECK((a ^ qint::bottom()) == a);
}

TEST_CASE("xor with top is complement") {
	auto a = qi(0, QU);
	CHECK((a ^ qint::top()) == ~a);
}

TEST_CASE("xor of disjoint intervals equals their union") {
	auto a = qi(0, QU);
	auto b = qi(2*QU, 3*QU);
	CHECK((a ^ b) == (a | b));
}

TEST_CASE("xor of overlapping intervals") {
	auto a = qi(0, 2*QU);
	auto b = qi(QU, 3*QU);
	auto xr = a ^ b;
	// (a|b) & ~(a&b) = [0,3) & ~[1,2) = [0,1)|[2,3)
	CHECK(xr.pieces.size() == 2);
	CHECK(xr.pieces[0].lo.raw == 0);
	CHECK(xr.pieces[0].hi.raw == QU);
	CHECK(xr.pieces[1].lo.raw == 2*QU);
	CHECK(xr.pieces[1].hi.raw == 3*QU);
}

} // TEST_SUITE qint — BA operations

// ============================================================================
TEST_SUITE("qint — Boolean algebra laws") {
// ============================================================================

TEST_CASE("idempotence of &") {
	auto a = qi(0, QU);
	CHECK((a & a) == a);
}

TEST_CASE("idempotence of |") {
	auto a = qi(0, QU);
	CHECK((a | a) == a);
}

TEST_CASE("commutativity of &") {
	auto a = qi(0, 2*QU);
	auto b = qi(QU, 3*QU);
	CHECK((a & b) == (b & a));
}

TEST_CASE("commutativity of |") {
	auto a = qi(0, QU);
	auto b = qi(2*QU, 3*QU);
	CHECK((a | b) == (b | a));
}

TEST_CASE("associativity of &") {
	auto a = qi(0, 4*QU);
	auto b = qi(QU, 3*QU);
	auto c = qi(2*QU, 5*QU);
	CHECK(((a & b) & c) == (a & (b & c)));
}

TEST_CASE("associativity of |") {
	auto a = qi(0, QU);
	auto b = qi(2*QU, 3*QU);
	auto c = qi(4*QU, 5*QU);
	CHECK(((a | b) | c) == (a | (b | c)));
}

TEST_CASE("De Morgan: ~(A & B) == ~A | ~B") {
	auto a = qi(0, 2*QU);
	auto b = qi(QU, 3*QU);
	CHECK(~(a & b) == (~a | ~b));
}

TEST_CASE("De Morgan: ~(A | B) == ~A & ~B") {
	auto a = qi(0, QU);
	auto b = qi(2*QU, 3*QU);
	CHECK(~(a | b) == (~a & ~b));
}

TEST_CASE("absorption: A & (A | B) == A") {
	auto a = qi(0, QU);
	auto b = qi(2*QU, 3*QU);
	CHECK((a & (a | b)) == a);
}

TEST_CASE("absorption: A | (A & B) == A") {
	auto a = qi(0, 2*QU);
	auto b = qi(QU, 3*QU);
	CHECK((a | (a & b)) == a);
}

TEST_CASE("distributivity: A & (B | C) == (A & B) | (A & C)") {
	auto a = qi(QU/2, 3*QU);
	auto b = qi(0, 2*QU);
	auto c = qi(QU, 4*QU);
	CHECK((a & (b | c)) == ((a & b) | (a & c)));
}

TEST_CASE("distributivity: A | (B & C) == (A | B) & (A | C)") {
	auto a = qi(0, QU);
	auto b = qi(QU/2, 3*QU);
	auto c = qi(QU, 4*QU);
	CHECK((a | (b & c)) == ((a | b) & (a | c)));
}

TEST_CASE("triple negation equals single negation") {
	auto a = qi(0, QU);
	CHECK(~~~a == ~a);
}

TEST_CASE("complement law: A & ~A == bot (fractional interval)") {
	auto a = qi(QU/4, 3*(QU/4));
	CHECK((a & ~a) == qint::bottom());
}

TEST_CASE("complement law: A | ~A == top (fractional interval)") {
	auto a = qi(QU/4, 3*(QU/4));
	CHECK((a | ~a) == qint::top());
}

TEST_CASE("xor commutativity") {
	auto a = qi(0, QU);
	auto b = qi(QU/2, 2*QU);
	CHECK((a ^ b) == (b ^ a));
}

TEST_CASE("xor associativity") {
	auto a = qi(0, 2*QU);
	auto b = qi(QU, 3*QU);
	auto c = qi(2*QU, 4*QU);
	CHECK(((a ^ b) ^ c) == (a ^ (b ^ c)));
}

TEST_CASE("bot & bot == bot") {
	CHECK((qint::bottom() & qint::bottom()) == qint::bottom());
}

TEST_CASE("top | top == top") {
	CHECK((qint::top() | qint::top()) == qint::top());
}

TEST_CASE("bot | bot == bot") {
	CHECK((qint::bottom() | qint::bottom()) == qint::bottom());
}

TEST_CASE("top & top == top") {
	CHECK((qint::top() & qint::top()) == qint::top());
}

TEST_CASE("bot | top == top") {
	CHECK((qint::bottom() | qint::top()) == qint::top());
}

TEST_CASE("bot & top == bot") {
	CHECK((qint::bottom() & qint::top()) == qint::bottom());
}

} // TEST_SUITE qint — Boolean algebra laws

// ============================================================================
TEST_SUITE("qint — is_zero / is_one") {
// ============================================================================

TEST_CASE("bottom is zero") {
	CHECK(is_qint_zero(qint::bottom()) == true);
}

TEST_CASE("top is one") {
	CHECK(is_qint_one(qint::top()) == true);
}

TEST_CASE("top is not zero") {
	CHECK(is_qint_zero(qint::top()) == false);
}

TEST_CASE("bottom is not one") {
	CHECK(is_qint_one(qint::bottom()) == false);
}

TEST_CASE("non-trivial interval is not zero") {
	CHECK(is_qint_zero(qi(0, QU)) == false);
}

TEST_CASE("non-trivial interval is not one") {
	CHECK(is_qint_one(qi(0, QU)) == false);
}

TEST_CASE("A & ~A is zero") {
	auto a = qi(0, QU);
	CHECK(is_qint_zero(a & ~a));
}

TEST_CASE("A | ~A is one") {
	auto a = qi(0, QU);
	CHECK(is_qint_one(a | ~a));
}

TEST_CASE("A ^ A is zero") {
	auto a = qi(QU/4, 3*(QU/4));
	CHECK(is_qint_zero(a ^ a));
}

TEST_CASE("A ^ ~A is one") {
	auto a = qi(0, QU);
	CHECK(is_qint_one(a ^ ~a));
}

TEST_CASE("normalize_qint is identity for bottom") {
	CHECK(normalize_qint(qint::bottom()) == qint::bottom());
}

TEST_CASE("normalize_qint is identity for top") {
	CHECK(normalize_qint(qint::top()) == qint::top());
}

TEST_CASE("normalize_qint is identity for single interval") {
	auto a = qi(0, QU);
	CHECK(normalize_qint(a) == a);
}

} // TEST_SUITE qint — is_zero / is_one

// ============================================================================
TEST_SUITE("qint — ordering") {
// ============================================================================

TEST_CASE("bottom < top") {
	CHECK(qint::bottom() < qint::top());
	CHECK_FALSE(qint::top() < qint::bottom());
}

TEST_CASE("bottom < any non-empty interval") {
	auto a = qi(0, QU);
	CHECK(qint::bottom() < a);
	CHECK_FALSE(a < qint::bottom());
}

TEST_CASE("equal elements are not less-than each other") {
	auto a = qi(0, QU);
	CHECK_FALSE(a < a);
}

TEST_CASE("three-way comparison: equal") {
	auto a = qi(0, QU);
	auto b = qi(0, QU);
	auto cmp = a <=> b;
	CHECK(cmp == std::strong_ordering::equal);
}

TEST_CASE("three-way comparison: less") {
	auto bot = qint::bottom();
	auto a   = qi(0, QU);
	auto cmp = bot <=> a;
	CHECK(cmp == std::strong_ordering::less);
}

TEST_CASE("three-way comparison: greater") {
	auto a   = qi(0, QU);
	auto bot = qint::bottom();
	auto cmp = a <=> bot;
	CHECK(cmp == std::strong_ordering::greater);
}

TEST_CASE("strict ordering on lo endpoint") {
	auto a = qi(0, QU);
	auto b = qi(QU, 2*QU);
	CHECK(a < b);
	CHECK_FALSE(b < a);
}

TEST_CASE("ordering: same lo, different hi") {
	auto a = qi(0, QU);
	auto b = qi(0, 2*QU);
	CHECK(a < b);
}

} // TEST_SUITE qint — ordering

// ============================================================================
TEST_SUITE("qint — splitter") {
// ============================================================================

TEST_CASE("splitter_one is [0, 1/2)") {
	auto s1 = qint_splitter_one();
	CHECK(s1.pieces.size() == 1);
	CHECK(s1.pieces[0].lo.raw == 0);
	CHECK(s1.pieces[0].hi.raw == QU/2);
}

TEST_CASE("splitter_one is non-empty") {
	CHECK_FALSE(qint_splitter_one().is_empty());
}

TEST_CASE("splitter_one is non-full") {
	CHECK_FALSE(qint_splitter_one().is_full());
}

TEST_CASE("splitter of bottom is bottom") {
	CHECK(qint_splitter(qint::bottom(), splitter_type::upper) == qint::bottom());
}

TEST_CASE("splitter of top is non-empty") {
	auto s = qint_splitter(qint::top(), splitter_type::upper);
	CHECK_FALSE(s.is_empty());
}

TEST_CASE("splitter of top is non-full") {
	auto s = qint_splitter(qint::top(), splitter_type::upper);
	CHECK_FALSE(s.is_full());
}

TEST_CASE("splitter of top: complement within top is non-empty (atomless)") {
	auto s = qint_splitter(qint::top(), splitter_type::upper);
	CHECK_FALSE((~s & qint::top()).is_empty());
}

TEST_CASE("splitter result is subset of input") {
	auto a = qi(0, QU);
	auto s = qint_splitter(a, splitter_type::upper);
	// s must be a subset: s & ~a == empty
	CHECK(is_qint_zero(s & ~a));
}

TEST_CASE("splitter of [0, 2) gives [0, 1) at midpoint") {
	auto a = qi(0, 2*QU);
	auto s = qint_splitter(a, splitter_type::upper);
	CHECK(s.pieces.size() == 1);
	CHECK(s.pieces[0].lo.raw == 0);
	CHECK(s.pieces[0].hi.raw == QU);
}

TEST_CASE("splitter of [-inf, 0) is subset of [-inf, 0)") {
	auto a = qi(qint_rational::NEG_INF, 0);
	auto s = qint_splitter(a, splitter_type::upper);
	CHECK_FALSE(s.is_empty());
	CHECK(is_qint_zero(s & ~a));
}

TEST_CASE("splitter of [0, +inf) is subset of [0, +inf)") {
	auto a = qi(0, qint_rational::POS_INF);
	auto s = qint_splitter(a, splitter_type::upper);
	CHECK_FALSE(s.is_empty());
	CHECK(is_qint_zero(s & ~a));
}

TEST_CASE("splitter of top: complement is non-empty (middle type)") {
	auto s = qint_splitter(qint::top(), splitter_type::middle);
	CHECK_FALSE(s.is_empty());
	CHECK_FALSE(s.is_full());
}

TEST_CASE("splitter of top: complement is non-empty (lower type)") {
	auto s = qint_splitter(qint::top(), splitter_type::lower);
	CHECK_FALSE(s.is_empty());
	CHECK_FALSE(s.is_full());
}

} // TEST_SUITE qint — splitter

// ============================================================================
TEST_SUITE("qint — to_string and stream output") {
// ============================================================================

TEST_CASE("bottom to_string is 'bot'") {
	CHECK(qint::bottom().to_string() == "bot");
}

TEST_CASE("top to_string is 'top'") {
	CHECK(qint::top().to_string() == "top");
}

TEST_CASE("single interval [0, 1) to_string") {
	CHECK(qi(0, QU).to_string() == "[0, 1)");
}

TEST_CASE("single interval [1/2, 1) to_string") {
	CHECK(qi(QU/2, QU).to_string() == "[1/2, 1)");
}

TEST_CASE("single interval [-1, 0) to_string") {
	CHECK(qi(-QU, 0).to_string() == "[-1, 0)");
}

TEST_CASE("single interval [0, 1/4) to_string") {
	CHECK(qi(0, QU/4).to_string() == "[0, 1/4)");
}

TEST_CASE("single interval [3/4, 1) to_string") {
	CHECK(qi(3*(QU/4), QU).to_string() == "[3/4, 1)");
}

TEST_CASE("two-piece union [0, 1) | [2, 3) to_string") {
	auto u = qi(0, QU) | qi(2*QU, 3*QU);
	CHECK(u.to_string() == "[0, 1) | [2, 3)");
}

TEST_CASE("union of overlapping intervals normalises to one piece") {
	auto u = qi(0, 2*QU) | qi(QU, 3*QU);
	CHECK(u.pieces.size() == 1);
	CHECK(u.to_string() == "[0, 3)");
}

TEST_CASE("union of adjacent intervals normalises to one piece") {
	auto u = qi(0, QU) | qi(QU, 2*QU);
	CHECK(u.pieces.size() == 1);
	CHECK(u.to_string() == "[0, 2)");
}

TEST_CASE("stream output for bottom") {
	std::ostringstream oss;
	oss << qint::bottom();
	CHECK(oss.str() == "bot");
}

TEST_CASE("stream output for top") {
	std::ostringstream oss;
	oss << qint::top();
	CHECK(oss.str() == "top");
}

TEST_CASE("stream output for [0, 1)") {
	std::ostringstream oss;
	oss << qi(0, QU);
	CHECK(oss.str() == "[0, 1)");
}

TEST_CASE("stream output for two-piece union") {
	std::ostringstream oss;
	oss << (qi(0, QU) | qi(2*QU, 3*QU));
	CHECK(oss.str() == "[0, 1) | [2, 3)");
}

} // TEST_SUITE qint — to_string and stream output

// ============================================================================
TEST_SUITE("qint — hashing") {
// ============================================================================

TEST_CASE("hash of bottom is stable") {
	std::hash<qint> h;
	auto bot = qint::bottom();
	CHECK(h(bot) == h(bot));
}

TEST_CASE("hash of top is stable") {
	std::hash<qint> h;
	auto top = qint::top();
	CHECK(h(top) == h(top));
}

TEST_CASE("hash of equal qints is equal") {
	std::hash<qint> h;
	CHECK(h(qi(0, QU)) == h(qi(0, QU)));
}

TEST_CASE("hash of bottom and top differ") {
	std::hash<qint> h;
	CHECK(h(qint::bottom()) != h(qint::top()));
}

TEST_CASE("hash of distinct intervals likely differ") {
	std::hash<qint> h;
	CHECK(h(qi(0, QU)) != h(qi(QU, 2*QU)));
}

TEST_CASE("hash of two-piece union is stable") {
	std::hash<qint> h;
	auto u1 = qi(0, QU) | qi(2*QU, 3*QU);
	auto u2 = qi(0, QU) | qi(2*QU, 3*QU);
	CHECK(h(u1) == h(u2));
}

} // TEST_SUITE qint — hashing
