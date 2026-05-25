// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Comprehensive unit tests for qint — atomless Boolean algebra of real-valued
// intervals [a, b) (left-closed, right-open) on the extended real line.
// Tests cover: qint construction, BA operations, Boolean algebra laws,
// normalisation, is_zero/is_one, ordering, splitter, stream output, and hashing.

#include <cmath>
#include <limits>
#include <sstream>

#include "test_init.h"
#include "boolean_algebras/qint.h"

using idni::tau_lang::qint;
using idni::tau_lang::qint_splitter;
using idni::tau_lang::qint_splitter_one;
using idni::tau_lang::is_qint_zero;
using idni::tau_lang::is_qint_one;
using idni::tau_lang::normalize_qint;
using idni::tau_lang::splitter_type;

static constexpr double POS_INF =  std::numeric_limits<double>::infinity();
static constexpr double NEG_INF = -std::numeric_limits<double>::infinity();

// Construct a single-interval qint [lo, hi)
static qint qi(double lo, double hi) {
	return qint{{ {lo, hi} }};
}

// Get lo and hi of the i-th interval (0-indexed) in the sorted map
static std::pair<double,double> piece(const qint& q, size_t i) {
	auto it = q.intervals.begin();
	std::advance(it, i);
	return {it->first, it->second};
}

// Exact double equality without triggering -Wfloat-equal.
// Uses <= and >= so that CHECK(deq(a, b)) receives a bool and doctest never
// generates a double == double comparison.
static bool deq(double a, double b) {
	return !(a < b) && !(b < a);
}

// ============================================================================
TEST_SUITE("qint — basic construction") {
// ============================================================================

TEST_CASE("bottom is empty") {
	auto bot = qint::bottom();
	CHECK(bot.is_empty());
	CHECK_FALSE(bot.is_full());
	CHECK(bot == false);
	CHECK(bot != true);
	CHECK(bot.intervals.empty());
}

TEST_CASE("top is full") {
	auto top = qint::top();
	CHECK(top.is_full());
	CHECK_FALSE(top.is_empty());
	CHECK(top == true);
	CHECK(top != false);
	CHECK(top.intervals.size() == 1);
	CHECK(std::isinf(piece(top, 0).first));
	CHECK(piece(top, 0).first  < 0);
	CHECK(std::isinf(piece(top, 0).second));
	CHECK(piece(top, 0).second > 0);
}

TEST_CASE("top and bottom are not equal") {
	CHECK(qint::top() != qint::bottom());
}

TEST_CASE("single interval [0, 1)") {
	auto q = qi(0.0, 1.0);
	CHECK_FALSE(q.is_empty());
	CHECK_FALSE(q.is_full());
	CHECK(q.intervals.size() == 1);
	CHECK(deq(piece(q, 0).first,  0.0));
	CHECK(deq(piece(q, 0).second, 1.0));
}

TEST_CASE("single interval is not bottom or top") {
	auto q = qi(0.0, 1.0);
	CHECK(q != qint::bottom());
	CHECK(q != qint::top());
}

TEST_CASE("two identical qints are equal") {
	CHECK(qi(0.0, 1.0) == qi(0.0, 1.0));
}

TEST_CASE("two different qints are not equal") {
	CHECK(qi(0.0, 1.0) != qi(1.0, 2.0));
}

TEST_CASE("interval [-inf, 0) is not bottom") {
	auto q = qi(NEG_INF, 0.0);
	CHECK_FALSE(q.is_empty());
	CHECK_FALSE(q.is_full());
}

TEST_CASE("interval [0, +inf) is not top") {
	auto q = qi(0.0, POS_INF);
	CHECK_FALSE(q.is_empty());
	CHECK_FALSE(q.is_full());
}

TEST_CASE("interval [-inf, +inf) equals top") {
	auto q = qi(NEG_INF, POS_INF);
	CHECK(q.is_full());
	CHECK(q == qint::top());
}

} // TEST_SUITE qint — basic construction

// ============================================================================
TEST_SUITE("qint — BA operations") {
// ============================================================================

TEST_CASE("union: identity with bottom") {
	auto a = qi(0.0, 1.0);
	CHECK((a | qint::bottom()) == a);
	CHECK((qint::bottom() | a) == a);
}

TEST_CASE("union: absorb into top") {
	auto a = qi(0.0, 1.0);
	CHECK((a | qint::top()) == qint::top());
	CHECK((qint::top() | a) == qint::top());
}

TEST_CASE("intersection: identity with top") {
	auto a = qi(0.0, 1.0);
	CHECK((a & qint::top()) == a);
	CHECK((qint::top() & a) == a);
}

TEST_CASE("intersection: absorb into bottom") {
	auto a = qi(0.0, 1.0);
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
	auto a = qi(0.0, 1.0);
	CHECK(~~a == a);
}

TEST_CASE("double complement of top") {
	CHECK(~~qint::top() == qint::top());
}

TEST_CASE("double complement of bottom") {
	CHECK(~~qint::bottom() == qint::bottom());
}

TEST_CASE("complement of [0, 1) is [-inf, 0) | [1, +inf)") {
	auto c = ~qi(0.0, 1.0);
	CHECK(c.intervals.size() == 2);
	CHECK(std::isinf(piece(c, 0).first));
	CHECK(piece(c, 0).first < 0);
	CHECK(deq(piece(c, 0).second, 0.0));
	CHECK(deq(piece(c, 1).first,  1.0));
	CHECK(std::isinf(piece(c, 1).second));
	CHECK(piece(c, 1).second > 0);
}

TEST_CASE("complement of [-inf, 0) is [0, +inf)") {
	auto c = ~qi(NEG_INF, 0.0);
	CHECK(c.intervals.size() == 1);
	CHECK(deq(piece(c, 0).first,  0.0));
	CHECK(std::isinf(piece(c, 0).second));
	CHECK(piece(c, 0).second > 0);
}

TEST_CASE("complement of [0, +inf) is [-inf, 0)") {
	auto c = ~qi(0.0, POS_INF);
	CHECK(c.intervals.size() == 1);
	CHECK(std::isinf(piece(c, 0).first));
	CHECK(piece(c, 0).first < 0);
	CHECK(deq(piece(c, 0).second, 0.0));
}

TEST_CASE("complement law: A & ~A == bot") {
	auto a = qi(0.0, 1.0);
	CHECK((a & ~a) == qint::bottom());
}

TEST_CASE("complement law: A | ~A == top") {
	auto a = qi(0.0, 1.0);
	CHECK((a | ~a) == qint::top());
}

TEST_CASE("complement of two-piece union") {
	auto a = qi(0.0, 1.0);
	auto b = qi(2.0, 3.0);
	auto u = a | b;
	auto c = ~u;
	// complement of [0,1)|[2,3) is [-inf,0)|[1,2)|[3,+inf)
	CHECK(c.intervals.size() == 3);
	CHECK(std::isinf(piece(c, 0).first));
	CHECK(piece(c, 0).first < 0);
	CHECK(deq(piece(c, 0).second, 0.0));
	CHECK(deq(piece(c, 1).first,  1.0));
	CHECK(deq(piece(c, 1).second, 2.0));
	CHECK(deq(piece(c, 2).first,  3.0));
	CHECK(std::isinf(piece(c, 2).second));
	CHECK(piece(c, 2).second > 0);
}

TEST_CASE("union of two disjoint intervals has two pieces") {
	auto u = qi(0.0, 1.0) | qi(2.0, 3.0);
	CHECK(u.intervals.size() == 2);
	CHECK(deq(piece(u, 0).first,  0.0));
	CHECK(deq(piece(u, 0).second, 1.0));
	CHECK(deq(piece(u, 1).first,  2.0));
	CHECK(deq(piece(u, 1).second, 3.0));
}

TEST_CASE("union of overlapping intervals merges into one piece") {
	auto u = qi(0.0, 2.0) | qi(1.0, 3.0);
	CHECK(u.intervals.size() == 1);
	CHECK(deq(piece(u, 0).first,  0.0));
	CHECK(deq(piece(u, 0).second, 3.0));
}

TEST_CASE("union of adjacent intervals merges into one piece") {
	auto u = qi(0.0, 1.0) | qi(1.0, 2.0);
	CHECK(u.intervals.size() == 1);
	CHECK(deq(piece(u, 0).first,  0.0));
	CHECK(deq(piece(u, 0).second, 2.0));
}

TEST_CASE("union of same interval is idempotent") {
	auto a = qi(0.0, 1.0);
	CHECK((a | a) == a);
}

TEST_CASE("intersection of overlapping intervals") {
	auto inter = qi(0.0, 2.0) & qi(1.0, 3.0);
	CHECK(inter.intervals.size() == 1);
	CHECK(deq(piece(inter, 0).first,  1.0));
	CHECK(deq(piece(inter, 0).second, 2.0));
}

TEST_CASE("intersection of disjoint intervals is empty") {
	CHECK((qi(0.0, 1.0) & qi(2.0, 3.0)) == qint::bottom());
}

TEST_CASE("intersection of adjacent half-open intervals is empty") {
	// [0,1) and [1,2) share endpoint 1 but for half-open intervals the intersection is empty
	CHECK((qi(0.0, 1.0) & qi(1.0, 2.0)) == qint::bottom());
}

TEST_CASE("intersection of same interval is idempotent") {
	auto a = qi(0.0, 1.0);
	CHECK((a & a) == a);
}

TEST_CASE("intersection with containing interval equals inner") {
	auto inner = qi(0.25, 0.75);
	auto outer = qi(0.0, 1.0);
	CHECK((inner & outer) == inner);
}

TEST_CASE("xor with self is bottom") {
	auto a = qi(0.0, 1.0);
	CHECK((a ^ a) == qint::bottom());
}

TEST_CASE("xor with bottom is identity") {
	auto a = qi(0.0, 1.0);
	CHECK((a ^ qint::bottom()) == a);
}

TEST_CASE("xor with top is complement") {
	auto a = qi(0.0, 1.0);
	CHECK((a ^ qint::top()) == ~a);
}

TEST_CASE("xor of disjoint intervals equals their union") {
	auto a = qi(0.0, 1.0);
	auto b = qi(2.0, 3.0);
	CHECK((a ^ b) == (a | b));
}

TEST_CASE("xor of overlapping intervals") {
	auto a = qi(0.0, 2.0);
	auto b = qi(1.0, 3.0);
	auto xr = a ^ b;
	// (a|b) & ~(a&b) = [0,3) & ~[1,2) = [0,1)|[2,3)
	CHECK(xr.intervals.size() == 2);
	CHECK(deq(piece(xr, 0).first,  0.0));
	CHECK(deq(piece(xr, 0).second, 1.0));
	CHECK(deq(piece(xr, 1).first,  2.0));
	CHECK(deq(piece(xr, 1).second, 3.0));
}

} // TEST_SUITE qint — BA operations

// ============================================================================
TEST_SUITE("qint — Boolean algebra laws") {
// ============================================================================

TEST_CASE("idempotence of &") {
	auto a = qi(0.0, 1.0);
	CHECK((a & a) == a);
}

TEST_CASE("idempotence of |") {
	auto a = qi(0.0, 1.0);
	CHECK((a | a) == a);
}

TEST_CASE("commutativity of &") {
	auto a = qi(0.0, 2.0);
	auto b = qi(1.0, 3.0);
	CHECK((a & b) == (b & a));
}

TEST_CASE("commutativity of |") {
	auto a = qi(0.0, 1.0);
	auto b = qi(2.0, 3.0);
	CHECK((a | b) == (b | a));
}

TEST_CASE("associativity of &") {
	auto a = qi(0.0, 4.0);
	auto b = qi(1.0, 3.0);
	auto c = qi(2.0, 5.0);
	CHECK(((a & b) & c) == (a & (b & c)));
}

TEST_CASE("associativity of |") {
	auto a = qi(0.0, 1.0);
	auto b = qi(2.0, 3.0);
	auto c = qi(4.0, 5.0);
	CHECK(((a | b) | c) == (a | (b | c)));
}

TEST_CASE("De Morgan: ~(A & B) == ~A | ~B") {
	auto a = qi(0.0, 2.0);
	auto b = qi(1.0, 3.0);
	CHECK(~(a & b) == (~a | ~b));
}

TEST_CASE("De Morgan: ~(A | B) == ~A & ~B") {
	auto a = qi(0.0, 1.0);
	auto b = qi(2.0, 3.0);
	CHECK(~(a | b) == (~a & ~b));
}

TEST_CASE("absorption: A & (A | B) == A") {
	auto a = qi(0.0, 1.0);
	auto b = qi(2.0, 3.0);
	CHECK((a & (a | b)) == a);
}

TEST_CASE("absorption: A | (A & B) == A") {
	auto a = qi(0.0, 2.0);
	auto b = qi(1.0, 3.0);
	CHECK((a | (a & b)) == a);
}

TEST_CASE("distributivity: A & (B | C) == (A & B) | (A & C)") {
	auto a = qi(0.5, 3.0);
	auto b = qi(0.0, 2.0);
	auto c = qi(1.0, 4.0);
	CHECK((a & (b | c)) == ((a & b) | (a & c)));
}

TEST_CASE("distributivity: A | (B & C) == (A | B) & (A | C)") {
	auto a = qi(0.0, 1.0);
	auto b = qi(0.5, 3.0);
	auto c = qi(1.0, 4.0);
	CHECK((a | (b & c)) == ((a | b) & (a | c)));
}

TEST_CASE("triple negation equals single negation") {
	auto a = qi(0.0, 1.0);
	CHECK(~~~a == ~a);
}

TEST_CASE("complement law: A & ~A == bot (fractional interval)") {
	auto a = qi(0.25, 0.75);
	CHECK((a & ~a) == qint::bottom());
}

TEST_CASE("complement law: A | ~A == top (fractional interval)") {
	auto a = qi(0.25, 0.75);
	CHECK((a | ~a) == qint::top());
}

TEST_CASE("xor commutativity") {
	auto a = qi(0.0, 1.0);
	auto b = qi(0.5, 2.0);
	CHECK((a ^ b) == (b ^ a));
}

TEST_CASE("xor associativity") {
	auto a = qi(0.0, 2.0);
	auto b = qi(1.0, 3.0);
	auto c = qi(2.0, 4.0);
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
	CHECK(is_qint_zero(qi(0.0, 1.0)) == false);
}

TEST_CASE("non-trivial interval is not one") {
	CHECK(is_qint_one(qi(0.0, 1.0)) == false);
}

TEST_CASE("A & ~A is zero") {
	auto a = qi(0.0, 1.0);
	CHECK(is_qint_zero(a & ~a));
}

TEST_CASE("A | ~A is one") {
	auto a = qi(0.0, 1.0);
	CHECK(is_qint_one(a | ~a));
}

TEST_CASE("A ^ A is zero") {
	auto a = qi(0.25, 0.75);
	CHECK(is_qint_zero(a ^ a));
}

TEST_CASE("A ^ ~A is one") {
	auto a = qi(0.0, 1.0);
	CHECK(is_qint_one(a ^ ~a));
}

TEST_CASE("normalize_qint is identity for bottom") {
	CHECK(normalize_qint(qint::bottom()) == qint::bottom());
}

TEST_CASE("normalize_qint is identity for top") {
	CHECK(normalize_qint(qint::top()) == qint::top());
}

TEST_CASE("normalize_qint is identity for single interval") {
	auto a = qi(0.0, 1.0);
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
	auto a = qi(0.0, 1.0);
	CHECK(qint::bottom() < a);
	CHECK_FALSE(a < qint::bottom());
}

TEST_CASE("equal elements are not less-than each other") {
	auto a = qi(0.0, 1.0);
	CHECK_FALSE(a < a);
}

TEST_CASE("three-way comparison: equal") {
	auto a = qi(0.0, 1.0);
	auto b = qi(0.0, 1.0);
	auto cmp = a <=> b;
	CHECK(cmp == std::strong_ordering::equal);
}

TEST_CASE("three-way comparison: less") {
	auto bot = qint::bottom();
	auto a   = qi(0.0, 1.0);
	auto cmp = bot <=> a;
	CHECK(cmp == std::strong_ordering::less);
}

TEST_CASE("three-way comparison: greater") {
	auto a   = qi(0.0, 1.0);
	auto bot = qint::bottom();
	auto cmp = a <=> bot;
	CHECK(cmp == std::strong_ordering::greater);
}

TEST_CASE("strict ordering on lo endpoint") {
	auto a = qi(0.0, 1.0);
	auto b = qi(1.0, 2.0);
	CHECK(a < b);
	CHECK_FALSE(b < a);
}

TEST_CASE("ordering: same lo, different hi") {
	auto a = qi(0.0, 1.0);
	auto b = qi(0.0, 2.0);
	CHECK(a < b);
}

} // TEST_SUITE qint — ordering

// ============================================================================
TEST_SUITE("qint — splitter") {
// ============================================================================

TEST_CASE("splitter_one is [0, 0.5)") {
	auto s1 = qint_splitter_one();
	CHECK(s1.intervals.size() == 1);
	CHECK(deq(piece(s1, 0).first,  0.0));
	CHECK(deq(piece(s1, 0).second, 0.5));
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
	auto a = qi(0.0, 1.0);
	auto s = qint_splitter(a, splitter_type::upper);
	// s must be a subset: s & ~a == empty
	CHECK(is_qint_zero(s & ~a));
}

TEST_CASE("splitter of [0, 2) gives [0, 1) at midpoint") {
	auto a = qi(0.0, 2.0);
	auto s = qint_splitter(a, splitter_type::upper);
	CHECK(s.intervals.size() == 1);
	CHECK(deq(piece(s, 0).first,  0.0));
	CHECK(deq(piece(s, 0).second, 1.0));
}

TEST_CASE("splitter of [-inf, 0) is subset of [-inf, 0)") {
	auto a = qi(NEG_INF, 0.0);
	auto s = qint_splitter(a, splitter_type::upper);
	CHECK_FALSE(s.is_empty());
	CHECK(is_qint_zero(s & ~a));
}

TEST_CASE("splitter of [0, +inf) is subset of [0, +inf)") {
	auto a = qi(0.0, POS_INF);
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
	CHECK(qi(0.0, 1.0).to_string() == "[0, 1)");
}

TEST_CASE("single interval [0.5, 1) to_string") {
	CHECK(qi(0.5, 1.0).to_string() == "[0.5, 1)");
}

TEST_CASE("single interval [-1, 0) to_string") {
	CHECK(qi(-1.0, 0.0).to_string() == "[-1, 0)");
}

TEST_CASE("single interval [0, 0.25) to_string") {
	CHECK(qi(0.0, 0.25).to_string() == "[0, 0.25)");
}

TEST_CASE("single interval [0.75, 1) to_string") {
	CHECK(qi(0.75, 1.0).to_string() == "[0.75, 1)");
}

TEST_CASE("two-piece union [0, 1) | [2, 3) to_string") {
	auto u = qi(0.0, 1.0) | qi(2.0, 3.0);
	CHECK(u.to_string() == "[0, 1) | [2, 3)");
}

TEST_CASE("union of overlapping intervals normalises to one piece") {
	auto u = qi(0.0, 2.0) | qi(1.0, 3.0);
	CHECK(u.intervals.size() == 1);
	CHECK(u.to_string() == "[0, 3)");
}

TEST_CASE("union of adjacent intervals normalises to one piece") {
	auto u = qi(0.0, 1.0) | qi(1.0, 2.0);
	CHECK(u.intervals.size() == 1);
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
	oss << qi(0.0, 1.0);
	CHECK(oss.str() == "[0, 1)");
}

TEST_CASE("stream output for two-piece union") {
	std::ostringstream oss;
	oss << (qi(0.0, 1.0) | qi(2.0, 3.0));
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
	CHECK(h(qi(0.0, 1.0)) == h(qi(0.0, 1.0)));
}

TEST_CASE("hash of bottom and top differ") {
	std::hash<qint> h;
	CHECK(h(qint::bottom()) != h(qint::top()));
}

TEST_CASE("hash of distinct intervals likely differ") {
	std::hash<qint> h;
	CHECK(h(qi(0.0, 1.0)) != h(qi(1.0, 2.0)));
}

TEST_CASE("hash of two-piece union is stable") {
	std::hash<qint> h;
	auto u1 = qi(0.0, 1.0) | qi(2.0, 3.0);
	auto u2 = qi(0.0, 1.0) | qi(2.0, 3.0);
	CHECK(h(u1) == h(u2));
}

} // TEST_SUITE qint — hashing
