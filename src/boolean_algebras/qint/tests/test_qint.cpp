// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Comprehensive unit tests for qint — atomless Boolean algebra of real-valued
// intervals [a, b) (left-closed, right-open) on the extended real line.
// Tests cover: qint construction, BA operations, Boolean algebra laws,
// normalisation, is_zero/is_one, ordering, splitter, stream output, and hashing.

#include <cmath>
#include <limits>
#include <sstream>

#include "test_init.h"
#include "boolean_algebras/qint/qint.h"

using idni::tau_lang::qint;
using idni::tau_lang::qint_splitter;
using idni::tau_lang::qint_splitter_one;
using idni::tau_lang::is_qint_zero;
using idni::tau_lang::is_qint_one;
using idni::tau_lang::normalize_qint;
using idni::tau_lang::splitter_type;
using idni::tau_lang::qint_eval_parse_tree;

using idni::tau_lang::qint_rational;

static constexpr double POS_INF =  std::numeric_limits<double>::infinity();
static constexpr double NEG_INF = -std::numeric_limits<double>::infinity();

// The fraction num/den; the test expects it to fit.
static qint_rational fr(long long num, long long den) {
	auto r = qint_rational::fraction(num, den);
	REQUIRE(r);
	return *r;
}

// The exact endpoint of a double. Every finite value used here is dyadic, so
// doubling reaches an integer.
static qint_rational R(double d) {
	if (std::isinf(d)) return d > 0
		? qint_rational::pos_inf() : qint_rational::neg_inf();
	long long den = 1;
	while (std::abs(d - std::trunc(d)) > 0.0) d *= 2, den *= 2;
	return fr(static_cast<long long>(d), den);
}

// Construct a single-interval qint [lo, hi)
static qint qi(double lo, double hi) {
	return qint{{ {R(lo), R(hi)} }};
}
static qint qi(qint_rational lo, qint_rational hi) {
	return qint{{ {lo, hi} }};
}

// Get lo and hi of the i-th interval (0-indexed) in the sorted map
static std::pair<qint_rational, qint_rational> piece(const qint& q, size_t i) {
	auto it = q.intervals.begin();
	std::advance(it, i);
	return {it->first, it->second};
}

static bool deq(const qint_rational& a, double b) { return a == R(b); }

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
	CHECK(piece(top, 0).first.is_inf());
	CHECK(piece(top, 0).first  < 0);
	CHECK(piece(top, 0).second.is_inf());
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
	CHECK(piece(c, 0).first.is_inf());
	CHECK(piece(c, 0).first < 0);
	CHECK(deq(piece(c, 0).second, 0.0));
	CHECK(deq(piece(c, 1).first,  1.0));
	CHECK(piece(c, 1).second.is_inf());
	CHECK(piece(c, 1).second > 0);
}

TEST_CASE("complement of [-inf, 0) is [0, +inf)") {
	auto c = ~qi(NEG_INF, 0.0);
	CHECK(c.intervals.size() == 1);
	CHECK(deq(piece(c, 0).first,  0.0));
	CHECK(piece(c, 0).second.is_inf());
	CHECK(piece(c, 0).second > 0);
}

TEST_CASE("complement of [0, +inf) is [-inf, 0)") {
	auto c = ~qi(0.0, POS_INF);
	CHECK(c.intervals.size() == 1);
	CHECK(piece(c, 0).first.is_inf());
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
	CHECK(piece(c, 0).first.is_inf());
	CHECK(piece(c, 0).first < 0);
	CHECK(deq(piece(c, 0).second, 0.0));
	CHECK(deq(piece(c, 1).first,  1.0));
	CHECK(deq(piece(c, 1).second, 2.0));
	CHECK(deq(piece(c, 2).first,  3.0));
	CHECK(piece(c, 2).second.is_inf());
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

TEST_CASE("splitter of [-inf, LLONG_MIN) is the input, not a degenerate interval") {
	// hi - 1 does not fit at hi == LLONG_MIN: no proper sub-element is
	// representable, so the splitter must return the input unchanged
	auto a = qi(qint_rational::neg_inf(),
		qint_rational(std::numeric_limits<long long>::min()));
	auto s = qint_splitter(a, splitter_type::upper);
	CHECK(s == a);
	CHECK_FALSE(s.is_empty());
	CHECK(s.intervals.size() == 1);
	CHECK(piece(s, 0).first < piece(s, 0).second); // non-degenerate
}

TEST_CASE("splitter of [LLONG_MAX, +inf) is the input, not a degenerate interval") {
	// lo + 1 does not fit at lo == LLONG_MAX: no proper sub-element is
	// representable, so the splitter must return the input unchanged
	auto a = qi(qint_rational(std::numeric_limits<long long>::max()),
		qint_rational::pos_inf());
	auto s = qint_splitter(a, splitter_type::upper);
	CHECK(s == a);
	CHECK_FALSE(s.is_empty());
	CHECK(s.intervals.size() == 1);
	CHECK(piece(s, 0).first < piece(s, 0).second); // non-degenerate
}

TEST_CASE("splitter of [-inf, 0) still splits properly (not saturated)") {
	auto a = qi(NEG_INF, 0.0);
	auto s = qint_splitter(a, splitter_type::upper);
	CHECK_FALSE(s.is_empty());
	CHECK(is_qint_zero(s & ~a)); // subset of input
	CHECK(s != a);               // proper sub-element
	CHECK(deq(piece(s, 0).second, -1.0));
}

TEST_CASE("splitter of [0, +inf) still splits properly (not saturated)") {
	auto a = qi(0.0, POS_INF);
	auto s = qint_splitter(a, splitter_type::upper);
	CHECK_FALSE(s.is_empty());
	CHECK(is_qint_zero(s & ~a)); // subset of input
	CHECK(s != a);               // proper sub-element
	CHECK(deq(piece(s, 0).second, 1.0));
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

// ============================================================================
TEST_SUITE("qint — parse tree evaluation") {
// ============================================================================

// Parse a qint source string via the qint grammar and evaluate the parse
// tree, mirroring parse_qint_grammar() (qint.tmpl.h) without needing a
// BA pack.
static result<qint> parse_q(const std::string& src) {
	result<qint> r;
	auto parsed = qint_parser::instance().parse(src.c_str(), src.size());
	if (!parsed.found) return r;
	auto t = qint_parser::tree::traverser(parsed.get_shaped_tree2())
		| qint_parser::qint;
	if (!t.has_value()) return r;
	return qint_eval_parse_tree(t);
}

TEST_CASE("bare integer 0 parses to bottom") {
	auto q = parse_q("0");
	REQUIRE(q.has_value());
	CHECK(q->is_empty());
}

TEST_CASE("bare integer 1 parses to top") {
	auto q = parse_q("1");
	REQUIRE(q.has_value());
	CHECK(q->is_full());
}

TEST_CASE("bare numbers other than 0 and 1 are rejected") {
	// qint has no points: `5` must not read as an element
	for (const char* src : { "5", "-3", "10", "01", "2", "-1", "0.5",
		"1/2", "4503599627370495", "9223372036854775807" })
	{
		INFO(src);
		CHECK_FALSE(parse_q(src).has_value());
	}
}

} // TEST_SUITE qint — parse tree evaluation

// The qint simplifiers are identities and had no caller in the suite.
TEST_CASE("simplify_qint_symbol/term are identities") {
	CHECK(idni::tau_lang::simplify_qint_symbol(nullptr) == nullptr);
	CHECK(idni::tau_lang::simplify_qint_term(nullptr) == nullptr);
}

// ============================================================================
TEST_SUITE("qint — exact endpoints") {
// ============================================================================

static qint_rational ep(const char* s) {
	qint_rational r;
	REQUIRE(idni::tau_lang::qint_detail::parse_endpoint(s, r));
	return r;
}
static bool rejects(const char* s) {
	qint_rational r;
	return !idni::tau_lang::qint_detail::parse_endpoint(s, r);
}

// GitHub #186
TEST_CASE("a fraction and its decimal rounding stay distinct") {
	CHECK(ep("1/3") != ep("0.3333333333333333"));
	CHECK(ep("0.3333333333333333") < ep("1/3"));
	auto a = qi(qint_rational(0), ep("1/3"));
	auto b = qi(qint_rational(0), ep("0.3333333333333333"));
	CHECK(a != b);
	CHECK_FALSE((a & ~b).is_empty());
}

TEST_CASE("decimals, exponents and fractions parse exactly") {
	CHECK(ep("0.25") == fr(1, 4));
	CHECK(ep(".5") == fr(1, 2));
	CHECK(ep("5.") == qint_rational(5));
	CHECK(ep("-1.5") == fr(-3, 2));
	CHECK(ep("2e-3") == fr(1, 500));
	CHECK(ep("1.25E+2") == qint_rational(125));
	CHECK(ep("2/4") == fr(1, 2));
	CHECK(ep("1/-3") == fr(-1, 3));
	CHECK(ep("0.5/3") == fr(1, 6));
	CHECK(ep("-inf").is_neg_inf());
	CHECK(ep("+inf").is_pos_inf());
}

TEST_CASE("a value that does not fit exactly is rejected, not rounded") {
	CHECK(rejects("1/0"));
	CHECK(rejects("1e40"));
	CHECK(rejects("0.00000000000000000000000000000000000000001"));
	CHECK(rejects("99999999999999999999"));
	CHECK(rejects("1.2.3"));
	CHECK(rejects("abc"));
}

TEST_CASE("endpoints print as integers, terminating decimals or p/q") {
	using idni::tau_lang::qint_detail::endpoint_to_string;
	CHECK(endpoint_to_string(qint_rational(3)) == "3");
	CHECK(endpoint_to_string(fr(-1, 4)) == "-0.25");
	CHECK(endpoint_to_string(fr(1, 500)) == "0.002");
	CHECK(endpoint_to_string(fr(1, 3)) == "1/3");
	CHECK(endpoint_to_string(fr(-5, 6)) == "-5/6");
	CHECK(qi(qint_rational(0), ep("1/3")).to_string() == "[0, 1/3)");
}

TEST_CASE("a fraction with a zero denominator or out of range is rejected") {
	constexpr long long min = std::numeric_limits<long long>::min();
	constexpr long long max = std::numeric_limits<long long>::max();
	CHECK_FALSE(qint_rational::fraction(1, 0));
	CHECK_FALSE(qint_rational::fraction(0, 0));
	CHECK_FALSE(qint_rational::fraction(min, -1));
	CHECK_FALSE(qint_rational::fraction(1, min));
	CHECK(qint_rational::fraction(min, 1) == qint_rational(min));
	CHECK(qint_rational::fraction(max, -1) == qint_rational(-max));
	CHECK(qint_rational::fraction(6, -4) == fr(-3, 2));
}

TEST_CASE("the midpoint split is exact") {
	auto s = qint_splitter(qi(qint_rational(0), ep("1/3")),
		splitter_type::upper);
	CHECK(s == qi(qint_rational(0), fr(1, 6)));
}

} // TEST_SUITE qint — exact endpoints
