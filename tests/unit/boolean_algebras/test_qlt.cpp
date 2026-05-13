// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Comprehensive tests for qlt — the dense linear order (Q, <).
// Covers: qlt_rational arithmetic/parsing, interval helpers (piece_empty,
// overlap, adjacent, merge), BA operations (|, &, ~, ^), normalisation,
// splitter, is_zero/is_one, and to_string.
// Parser and LTL integration are tested separately in test_qlt_grammar.cpp
// and test_qlt_oracle.cpp.

#include "test_init.h"
#include "boolean_algebras/qlt.h"

using namespace idni::tau_lang;

// ─── helpers ──────────────────────────────────────────────────────────────────

static qlt make_interval(
	qlt_rational lo, qlt_bound lo_b,
	qlt_rational hi, qlt_bound hi_b)
{
	qlt_piece p;
	p.lo = qlt_endpoint{lo, lo_b};
	p.hi = qlt_endpoint{hi, hi_b};
	return qlt{{p}};
}

static qlt oo(long long a, long long b) { // (a, b)
	return make_interval(
		qlt_rational(a, 1), qlt_bound::OPEN,
		qlt_rational(b, 1), qlt_bound::OPEN);
}
static qlt cc(long long a, long long b) { // [a, b]
	return make_interval(
		qlt_rational(a, 1), qlt_bound::CLOSED,
		qlt_rational(b, 1), qlt_bound::CLOSED);
}
static qlt co(long long a, long long b) { // [a, b)
	return make_interval(
		qlt_rational(a, 1), qlt_bound::CLOSED,
		qlt_rational(b, 1), qlt_bound::OPEN);
}
static qlt oc(long long a, long long b) { // (a, b]
	return make_interval(
		qlt_rational(a, 1), qlt_bound::OPEN,
		qlt_rational(b, 1), qlt_bound::CLOSED);
}
static qlt neg_inf_open(long long b) { // (-inf, b)
	return make_interval(
		qlt_rational::make_neg_inf(), qlt_bound::OPEN,
		qlt_rational(b, 1),           qlt_bound::OPEN);
}
static qlt neg_inf_closed(long long b) { // (-inf, b]
	return make_interval(
		qlt_rational::make_neg_inf(), qlt_bound::OPEN,
		qlt_rational(b, 1),           qlt_bound::CLOSED);
}
static qlt open_pos_inf(long long a) { // (a, +inf)
	return make_interval(
		qlt_rational(a, 1),            qlt_bound::OPEN,
		qlt_rational::make_pos_inf(),  qlt_bound::OPEN);
}
static qlt closed_pos_inf(long long a) { // [a, +inf)
	return make_interval(
		qlt_rational(a, 1),            qlt_bound::CLOSED,
		qlt_rational::make_pos_inf(),  qlt_bound::OPEN);
}

// ─── qlt_rational ─────────────────────────────────────────────────────────────

TEST_SUITE("qlt_rational — construction and properties") {

TEST_CASE("default is 0/1") {
	qlt_rational r;
	CHECK(r.p == 0);
	CHECK(r.q == 1);
	CHECK(r.is_finite());
}

TEST_CASE("integer construction") {
	qlt_rational r(7, 1);
	CHECK(r.p == 7);
	CHECK(r.q == 1);
	CHECK(r.is_finite());
}

TEST_CASE("fraction normalisation: 2/4 → 1/2") {
	qlt_rational r(2, 4);
	CHECK(r.p == 1);
	CHECK(r.q == 2);
}

TEST_CASE("fraction normalisation: -6/9 → -2/3") {
	qlt_rational r(-6, 9);
	CHECK(r.p == -2);
	CHECK(r.q == 3);
}

TEST_CASE("negative denominator flips sign: 3/-4 → -3/4") {
	qlt_rational r(3, -4);
	CHECK(r.p == -3);
	CHECK(r.q == 4);
}

TEST_CASE("zero is 0/1 after normalisation") {
	qlt_rational r(0, 5);
	CHECK(r.p == 0);
	CHECK(r.q == 1);
}

TEST_CASE("pos_inf factory") {
	auto r = qlt_rational::make_pos_inf();
	CHECK(r.is_pos_inf());
	CHECK(!r.is_neg_inf());
	CHECK(!r.is_finite());
	CHECK(!r.is_sym());
}

TEST_CASE("neg_inf factory") {
	auto r = qlt_rational::make_neg_inf();
	CHECK(r.is_neg_inf());
	CHECK(!r.is_pos_inf());
	CHECK(!r.is_finite());
}

TEST_CASE("sym factory") {
	auto r = qlt_rational::make_sym("alpha");
	CHECK(r.is_sym());
	CHECK(r.sym == "alpha");
	CHECK(!r.is_finite());
	CHECK(!r.is_pos_inf());
	CHECK(!r.is_neg_inf());
}

TEST_CASE("equality of equal fractions") {
	CHECK(qlt_rational(1, 2) == qlt_rational(2, 4));
}

TEST_CASE("inequality of different fractions") {
	CHECK(qlt_rational(1, 3) != qlt_rational(1, 4));
}

TEST_CASE("pos_inf equals pos_inf") {
	CHECK(qlt_rational::make_pos_inf() == qlt_rational::make_pos_inf());
}

TEST_CASE("neg_inf equals neg_inf") {
	CHECK(qlt_rational::make_neg_inf() == qlt_rational::make_neg_inf());
}

TEST_CASE("pos_inf != neg_inf") {
	CHECK(qlt_rational::make_pos_inf() != qlt_rational::make_neg_inf());
}

TEST_CASE("sym equality by name") {
	CHECK(qlt_rational::make_sym("a") == qlt_rational::make_sym("a"));
	CHECK(qlt_rational::make_sym("a") != qlt_rational::make_sym("b"));
}

TEST_CASE("ordering: -inf < 0") {
	CHECK(qlt_rational::make_neg_inf() < qlt_rational(0, 1));
}

TEST_CASE("ordering: 0 < +inf") {
	CHECK(qlt_rational(0, 1) < qlt_rational::make_pos_inf());
}

TEST_CASE("ordering: -inf < +inf") {
	CHECK(qlt_rational::make_neg_inf() < qlt_rational::make_pos_inf());
}

TEST_CASE("ordering: 1/3 < 1/2") {
	CHECK(qlt_rational(1, 3) < qlt_rational(1, 2));
}

TEST_CASE("ordering: -1 < 0 < 1") {
	CHECK(qlt_rational(-1, 1) < qlt_rational(0, 1));
	CHECK(qlt_rational(0, 1)  < qlt_rational(1, 1));
}

TEST_CASE("ordering: sym > +inf") {
	CHECK(qlt_rational::make_pos_inf() < qlt_rational::make_sym("x"));
}

TEST_CASE("ordering: sym lex order") {
	CHECK(qlt_rational::make_sym("a") < qlt_rational::make_sym("b"));
	CHECK(!(qlt_rational::make_sym("b") < qlt_rational::make_sym("a")));
}

TEST_CASE("ordering: reflexive <=") {
	qlt_rational r(3, 7);
	CHECK(r <= r);
	CHECK(r >= r);
}

TEST_CASE("midpoint of 0 and 1 is 1/2") {
	qlt_rational a(0, 1), b(1, 1);
	auto m = a.midpoint(b);
	CHECK(m == qlt_rational(1, 2));
}

TEST_CASE("midpoint of -1 and 1 is 0") {
	qlt_rational a(-1, 1), b(1, 1);
	auto m = a.midpoint(b);
	CHECK(m == qlt_rational(0, 1));
}

TEST_CASE("midpoint of 1/3 and 2/3 is 1/2") {
	qlt_rational a(1, 3), b(2, 3);
	auto m = a.midpoint(b);
	CHECK(m == qlt_rational(1, 2));
}

TEST_CASE("operator+: 1/3 + 1/6 = 1/2") {
	qlt_rational a(1, 3), b(1, 6);
	auto s = a + b;
	CHECK(s == qlt_rational(1, 2));
}

TEST_CASE("to_string: integer") {
	CHECK(qlt_rational(5, 1).to_string() == "5");
}

TEST_CASE("to_string: negative integer") {
	CHECK(qlt_rational(-3, 1).to_string() == "-3");
}

TEST_CASE("to_string: fraction") {
	CHECK(qlt_rational(1, 3).to_string() == "1/3");
}

TEST_CASE("to_string: +inf") {
	CHECK(qlt_rational::make_pos_inf().to_string() == "+inf");
}

TEST_CASE("to_string: -inf") {
	CHECK(qlt_rational::make_neg_inf().to_string() == "-inf");
}

TEST_CASE("to_string: sym") {
	CHECK(qlt_rational::make_sym("c").to_string() == "c");
}

TEST_CASE("parse: integer zero") {
	qlt_rational r;
	CHECK(qlt_rational::parse("0", r));
	CHECK(r == qlt_rational(0, 1));
}

TEST_CASE("parse: positive integer") {
	qlt_rational r;
	CHECK(qlt_rational::parse("42", r));
	CHECK(r == qlt_rational(42, 1));
}

TEST_CASE("parse: negative integer") {
	qlt_rational r;
	CHECK(qlt_rational::parse("-7", r));
	CHECK(r == qlt_rational(-7, 1));
}

TEST_CASE("parse: fraction 3/5") {
	qlt_rational r;
	CHECK(qlt_rational::parse("3/5", r));
	CHECK(r == qlt_rational(3, 5));
}

TEST_CASE("parse: decimal 0.5") {
	qlt_rational r;
	CHECK(qlt_rational::parse("0.5", r));
	CHECK(r == qlt_rational(1, 2));
}

TEST_CASE("parse: decimal 1.25") {
	qlt_rational r;
	CHECK(qlt_rational::parse("1.25", r));
	CHECK(r == qlt_rational(5, 4));
}

TEST_CASE("parse: +inf") {
	qlt_rational r;
	CHECK(qlt_rational::parse("+inf", r));
	CHECK(r.is_pos_inf());
}

TEST_CASE("parse: -inf") {
	qlt_rational r;
	CHECK(qlt_rational::parse("-inf", r));
	CHECK(r.is_neg_inf());
}

TEST_CASE("parse: bare inf") {
	qlt_rational r;
	CHECK(qlt_rational::parse("inf", r));
	CHECK(r.is_pos_inf());
}

TEST_CASE("parse: identifier → sym") {
	qlt_rational r;
	CHECK(qlt_rational::parse("myconst", r));
	CHECK(r.is_sym());
	CHECK(r.sym == "myconst");
}

TEST_CASE("parse: identifier with underscore") {
	qlt_rational r;
	CHECK(qlt_rational::parse("x_1", r));
	CHECK(r.is_sym());
}

TEST_CASE("parse: whitespace stripped") {
	qlt_rational r;
	CHECK(qlt_rational::parse("  3 ", r));
	CHECK(r == qlt_rational(3, 1));
}

TEST_CASE("parse: invalid string returns false") {
	qlt_rational r;
	CHECK(!qlt_rational::parse("3.1.4", r));
}

} // TEST_SUITE qlt_rational

// ─── piece helpers ────────────────────────────────────────────────────────────

TEST_SUITE("qlt piece helpers") {

TEST_CASE("qlt_piece_empty: open singleton (1,1) is empty") {
	qlt_piece p;
	p.lo = qlt_endpoint{qlt_rational(1, 1), qlt_bound::OPEN};
	p.hi = qlt_endpoint{qlt_rational(1, 1), qlt_bound::OPEN};
	CHECK(qlt_piece_empty(p));
}

TEST_CASE("qlt_piece_empty: half-open [1,1) is empty") {
	qlt_piece p;
	p.lo = qlt_endpoint{qlt_rational(1, 1), qlt_bound::CLOSED};
	p.hi = qlt_endpoint{qlt_rational(1, 1), qlt_bound::OPEN};
	CHECK(qlt_piece_empty(p));
}

TEST_CASE("qlt_piece_empty: closed singleton [1,1] is non-empty") {
	qlt_piece p;
	p.lo = qlt_endpoint{qlt_rational(1, 1), qlt_bound::CLOSED};
	p.hi = qlt_endpoint{qlt_rational(1, 1), qlt_bound::CLOSED};
	CHECK(!qlt_piece_empty(p));
}

TEST_CASE("qlt_piece_empty: inverted interval lo > hi is empty") {
	qlt_piece p;
	p.lo = qlt_endpoint{qlt_rational(2, 1), qlt_bound::OPEN};
	p.hi = qlt_endpoint{qlt_rational(1, 1), qlt_bound::OPEN};
	CHECK(qlt_piece_empty(p));
}

TEST_CASE("qlt_piece_empty: open interval (0,1) is non-empty") {
	qlt_piece p;
	p.lo = qlt_endpoint{qlt_rational(0, 1), qlt_bound::OPEN};
	p.hi = qlt_endpoint{qlt_rational(1, 1), qlt_bound::OPEN};
	CHECK(!qlt_piece_empty(p));
}

TEST_CASE("qlt_piece_empty: symbolic closed singleton [c,c] is non-empty") {
	qlt_piece p;
	p.lo = qlt_endpoint{qlt_rational::make_sym("c"), qlt_bound::CLOSED};
	p.hi = qlt_endpoint{qlt_rational::make_sym("c"), qlt_bound::CLOSED};
	CHECK(!qlt_piece_empty(p));
}

TEST_CASE("qlt_piece_empty: symbolic open singleton (c,c) is empty") {
	qlt_piece p;
	p.lo = qlt_endpoint{qlt_rational::make_sym("c"), qlt_bound::OPEN};
	p.hi = qlt_endpoint{qlt_rational::make_sym("c"), qlt_bound::OPEN};
	CHECK(qlt_piece_empty(p));
}

TEST_CASE("qlt_pieces_overlap: (0,2) and (1,3) overlap") {
	qlt_piece a, b;
	a.lo = {qlt_rational(0,1), qlt_bound::OPEN};
	a.hi = {qlt_rational(2,1), qlt_bound::OPEN};
	b.lo = {qlt_rational(1,1), qlt_bound::OPEN};
	b.hi = {qlt_rational(3,1), qlt_bound::OPEN};
	CHECK(qlt_pieces_overlap(a, b));
}

TEST_CASE("qlt_pieces_overlap: (0,1) and (2,3) do not overlap") {
	qlt_piece a, b;
	a.lo = {qlt_rational(0,1), qlt_bound::OPEN};
	a.hi = {qlt_rational(1,1), qlt_bound::OPEN};
	b.lo = {qlt_rational(2,1), qlt_bound::OPEN};
	b.hi = {qlt_rational(3,1), qlt_bound::OPEN};
	CHECK(!qlt_pieces_overlap(a, b));
}

TEST_CASE("qlt_pieces_overlap: touching open ends (0,1) and (1,2) do not overlap") {
	qlt_piece a, b;
	a.lo = {qlt_rational(0,1), qlt_bound::OPEN};
	a.hi = {qlt_rational(1,1), qlt_bound::OPEN};
	b.lo = {qlt_rational(1,1), qlt_bound::OPEN};
	b.hi = {qlt_rational(2,1), qlt_bound::OPEN};
	CHECK(!qlt_pieces_overlap(a, b));
}

TEST_CASE("qlt_pieces_overlap: [0,1] and [1,2] overlap at 1") {
	qlt_piece a, b;
	a.lo = {qlt_rational(0,1), qlt_bound::CLOSED};
	a.hi = {qlt_rational(1,1), qlt_bound::CLOSED};
	b.lo = {qlt_rational(1,1), qlt_bound::CLOSED};
	b.hi = {qlt_rational(2,1), qlt_bound::CLOSED};
	CHECK(qlt_pieces_overlap(a, b));
}

TEST_CASE("qlt_pieces_adjacent: (0,1) and [1,2) are adjacent") {
	qlt_piece a, b;
	a.lo = {qlt_rational(0,1), qlt_bound::OPEN};
	a.hi = {qlt_rational(1,1), qlt_bound::OPEN};
	b.lo = {qlt_rational(1,1), qlt_bound::CLOSED};
	b.hi = {qlt_rational(2,1), qlt_bound::OPEN};
	CHECK(qlt_pieces_adjacent(a, b));
}

TEST_CASE("qlt_pieces_adjacent: (0,1) and (2,3) are not adjacent") {
	qlt_piece a, b;
	a.lo = {qlt_rational(0,1), qlt_bound::OPEN};
	a.hi = {qlt_rational(1,1), qlt_bound::OPEN};
	b.lo = {qlt_rational(2,1), qlt_bound::OPEN};
	b.hi = {qlt_rational(3,1), qlt_bound::OPEN};
	CHECK(!qlt_pieces_adjacent(a, b));
}

TEST_CASE("qlt_merge: merge (0,1) and (1/2,2) → (0,2)") {
	qlt_piece a, b;
	a.lo = {qlt_rational(0,1), qlt_bound::OPEN};
	a.hi = {qlt_rational(1,1), qlt_bound::OPEN};
	b.lo = {qlt_rational(1,2), qlt_bound::OPEN};
	b.hi = {qlt_rational(2,1), qlt_bound::OPEN};
	auto m = qlt_merge(a, b);
	CHECK(m.lo.val == qlt_rational(0, 1));
	CHECK(m.hi.val == qlt_rational(2, 1));
	CHECK(m.lo.bound == qlt_bound::OPEN);
	CHECK(m.hi.bound == qlt_bound::OPEN);
}

TEST_CASE("qlt_merge: merge [0,1] and [1,2] → [0,2]") {
	qlt_piece a, b;
	a.lo = {qlt_rational(0,1), qlt_bound::CLOSED};
	a.hi = {qlt_rational(1,1), qlt_bound::CLOSED};
	b.lo = {qlt_rational(1,1), qlt_bound::CLOSED};
	b.hi = {qlt_rational(2,1), qlt_bound::CLOSED};
	auto m = qlt_merge(a, b);
	CHECK(m.lo.val == qlt_rational(0, 1));
	CHECK(m.hi.val == qlt_rational(2, 1));
	CHECK(m.lo.bound == qlt_bound::CLOSED);
	CHECK(m.hi.bound == qlt_bound::CLOSED);
}

} // TEST_SUITE piece helpers

// ─── basic construction ───────────────────────────────────────────────────────

TEST_SUITE("qlt — basic construction") {

TEST_CASE("bottom is empty") {
	auto bot = qlt::bottom();
	CHECK(bot.is_empty());
	CHECK(!bot.is_full());
	CHECK(bot == false);
	CHECK(bot != true);
}

TEST_CASE("top is full") {
	auto top = qlt::top();
	CHECK(top.is_full());
	CHECK(!top.is_empty());
	CHECK(top == true);
	CHECK(top != false);
}

TEST_CASE("bottom to_string is 'bot'") {
	CHECK(qlt::bottom().to_string() == "bot");
}

TEST_CASE("top to_string is 'top'") {
	CHECK(qlt::top().to_string() == "top");
}

TEST_CASE("bottom != top") {
	CHECK(qlt::bottom() != qlt::top());
}

TEST_CASE("single open interval (0,1) is neither empty nor full") {
	auto a = oo(0, 1);
	CHECK(!a.is_empty());
	CHECK(!a.is_full());
	CHECK(a.pieces.size() == 1);
}

TEST_CASE("single closed interval [0,1] is non-empty") {
	auto a = cc(0, 1);
	CHECK(!a.is_empty());
	CHECK(a.pieces.size() == 1);
	CHECK(a.pieces[0].lo.bound == qlt_bound::CLOSED);
	CHECK(a.pieces[0].hi.bound == qlt_bound::CLOSED);
}

TEST_CASE("singleton [2,2] has one piece") {
	auto a = cc(2, 2);
	CHECK(a.pieces.size() == 1);
	CHECK(a.pieces[0].lo.val == qlt_rational(2, 1));
	CHECK(a.pieces[0].hi.val == qlt_rational(2, 1));
}

TEST_CASE("equality of identical intervals") {
	CHECK(oo(0, 1) == oo(0, 1));
	CHECK(co(0, 1) == co(0, 1));
	CHECK(cc(0, 1) == cc(0, 1));
}

TEST_CASE("inequality of different bound types at same endpoints") {
	CHECK(oo(0, 1) != co(0, 1));
	CHECK(oo(0, 1) != oc(0, 1));
	CHECK(oo(0, 1) != cc(0, 1));
}

TEST_CASE("operator< provides strict total order") {
	auto a = oo(0, 1);
	auto b = oo(0, 2);
	bool a_lt_b = (a < b);
	bool b_lt_a = (b < a);
	CHECK((a_lt_b || b_lt_a)); // at least one of the two must hold
	CHECK(!(a < a));
}

TEST_CASE("operator<=> yields equal for identical qlt") {
	auto a = co(0, 1);
	CHECK((a <=> a) == std::strong_ordering::equal);
}

TEST_CASE("operator<=> yields less/greater for distinct qlt") {
	auto a = oo(0, 1);
	auto b = oo(0, 2);
	if (a < b) CHECK((a <=> b) == std::strong_ordering::less);
	else       CHECK((b <=> a) == std::strong_ordering::less);
}

TEST_CASE("to_string: open interval (0, 1)") {
	auto a = oo(0, 1);
	CHECK(a.to_string() == "(0, 1)");
}

TEST_CASE("to_string: closed interval [0, 1]") {
	auto a = cc(0, 1);
	CHECK(a.to_string() == "[0, 1]");
}

TEST_CASE("to_string: half-open [0, 1)") {
	auto a = co(0, 1);
	CHECK(a.to_string() == "[0, 1)");
}

TEST_CASE("to_string: singleton [3, 3] displays as '3'") {
	auto a = cc(3, 3);
	CHECK(a.to_string() == "3");
}

TEST_CASE("to_string: fraction singleton [1/2, 1/2] displays as '1/2'") {
	auto a = make_interval(
		qlt_rational(1, 2), qlt_bound::CLOSED,
		qlt_rational(1, 2), qlt_bound::CLOSED);
	CHECK(a.to_string() == "1/2");
}

} // TEST_SUITE basic construction

// ─── BA operations ────────────────────────────────────────────────────────────

TEST_SUITE("qlt — BA operations") {

TEST_CASE("union with bot is identity") {
	auto a = oo(0, 1);
	CHECK((a | qlt::bottom()) == a);
	CHECK((qlt::bottom() | a) == a);
}

TEST_CASE("union with top absorbs") {
	auto a = oo(0, 1);
	CHECK((a | qlt::top()) == qlt::top());
	CHECK((qlt::top() | a) == qlt::top());
}

TEST_CASE("intersection with top is identity") {
	auto a = oo(0, 1);
	CHECK((a & qlt::top()) == a);
	CHECK((qlt::top() & a) == a);
}

TEST_CASE("intersection with bot absorbs") {
	auto a = oo(0, 1);
	CHECK((a & qlt::bottom()) == qlt::bottom());
	CHECK((qlt::bottom() & a) == qlt::bottom());
}

TEST_CASE("complement of bot is top") {
	CHECK(~qlt::bottom() == qlt::top());
}

TEST_CASE("complement of top is bot") {
	CHECK(~qlt::top() == qlt::bottom());
}

TEST_CASE("double complement is identity: top") {
	CHECK(~~qlt::top() == qlt::top());
}

TEST_CASE("double complement is identity: bot") {
	CHECK(~~qlt::bottom() == qlt::bottom());
}

TEST_CASE("double complement is identity: open interval") {
	auto a = oo(0, 1);
	CHECK(~~a == a);
}

TEST_CASE("double complement is identity: closed interval") {
	auto a = cc(0, 1);
	CHECK(~~a == a);
}

TEST_CASE("complement law: A & ~A = bot") {
	auto a = oo(0, 1);
	CHECK((a & ~a) == qlt::bottom());
}

TEST_CASE("complement law: A | ~A = top") {
	auto a = oo(0, 1);
	CHECK((a | ~a) == qlt::top());
}

TEST_CASE("complement of (0,1) is (-inf,0] | [1,+inf)") {
	auto a = oo(0, 1);
	auto c = ~a;
	CHECK(c.pieces.size() == 2);
	CHECK(c.pieces[0].lo.val.is_neg_inf());
	CHECK(c.pieces[0].hi.val == qlt_rational(0, 1));
	CHECK(c.pieces[0].hi.bound == qlt_bound::CLOSED);
	CHECK(c.pieces[1].lo.val == qlt_rational(1, 1));
	CHECK(c.pieces[1].lo.bound == qlt_bound::CLOSED);
	CHECK(c.pieces[1].hi.val.is_pos_inf());
}

TEST_CASE("complement of [0,1] is (-inf,0) | (1,+inf)") {
	auto a = cc(0, 1);
	auto c = ~a;
	CHECK(c.pieces.size() == 2);
	CHECK(c.pieces[0].hi.bound == qlt_bound::OPEN);
	CHECK(c.pieces[1].lo.bound == qlt_bound::OPEN);
}

TEST_CASE("complement of (-inf,0) is [0,+inf)") {
	auto a = neg_inf_open(0);
	auto c = ~a;
	CHECK(c.pieces.size() == 1);
	CHECK(c.pieces[0].lo.val == qlt_rational(0, 1));
	CHECK(c.pieces[0].lo.bound == qlt_bound::CLOSED);
	CHECK(c.pieces[0].hi.val.is_pos_inf());
}

TEST_CASE("complement of [0,+inf) is (-inf,0)") {
	auto a = closed_pos_inf(0);
	auto c = ~a;
	CHECK(c.pieces.size() == 1);
	CHECK(c.pieces[0].lo.val.is_neg_inf());
	CHECK(c.pieces[0].hi.val == qlt_rational(0, 1));
	CHECK(c.pieces[0].hi.bound == qlt_bound::OPEN);
}

TEST_CASE("idempotence: A & A = A") {
	auto a = oo(0, 1);
	CHECK((a & a) == a);
}

TEST_CASE("idempotence: A | A = A") {
	auto a = oo(0, 1);
	CHECK((a | a) == a);
}

TEST_CASE("intersection of disjoint intervals is bot") {
	auto a = oo(0, 1);
	auto b = oo(2, 3);
	CHECK((a & b) == qlt::bottom());
}

TEST_CASE("intersection of overlapping intervals") {
	auto a = oo(0, 2);
	auto b = oo(1, 3);
	auto r = a & b;
	CHECK(r.pieces.size() == 1);
	CHECK(r.pieces[0].lo.val == qlt_rational(1, 1));
	CHECK(r.pieces[0].hi.val == qlt_rational(2, 1));
}

TEST_CASE("intersection of touching open/closed: (0,1] & [1,2) = {1}") {
	auto a = oc(0, 1); // (0, 1]
	auto b = co(1, 2); // [1, 2)
	auto r = a & b;
	CHECK(r.pieces.size() == 1);
	CHECK(r.pieces[0].lo.val == qlt_rational(1, 1));
	CHECK(r.pieces[0].hi.val == qlt_rational(1, 1));
	CHECK(r.pieces[0].lo.bound == qlt_bound::CLOSED);
	CHECK(r.pieces[0].hi.bound == qlt_bound::CLOSED);
}

TEST_CASE("intersection of touching open ends (0,1) & (1,2) is empty") {
	auto a = oo(0, 1);
	auto b = oo(1, 2);
	CHECK((a & b) == qlt::bottom());
}

TEST_CASE("union of adjacent [0,1) and [1,2) merges to [0,2)") {
	auto a = co(0, 1);
	auto b = co(1, 2);
	auto r = a | b;
	CHECK(r.pieces.size() == 1);
	CHECK(r.pieces[0].lo.val == qlt_rational(0, 1));
	CHECK(r.pieces[0].hi.val == qlt_rational(2, 1));
}

TEST_CASE("union of overlapping (0,2) and (1,3) merges to (0,3)") {
	auto a = oo(0, 2);
	auto b = oo(1, 3);
	auto r = a | b;
	CHECK(r.pieces.size() == 1);
	CHECK(r.pieces[0].lo.val == qlt_rational(0, 1));
	CHECK(r.pieces[0].hi.val == qlt_rational(3, 1));
}

TEST_CASE("union of disjoint intervals keeps both pieces") {
	auto a = oo(0, 1);
	auto b = oo(2, 3);
	auto r = a | b;
	CHECK(r.pieces.size() == 2);
}

TEST_CASE("symmetric difference: A ^ A = bot") {
	auto a = oo(0, 1);
	CHECK((a ^ a) == qlt::bottom());
}

TEST_CASE("symmetric difference: A ^ bot = A") {
	auto a = oo(0, 1);
	CHECK((a ^ qlt::bottom()) == a);
}

TEST_CASE("symmetric difference: A ^ top = ~A") {
	auto a = oo(0, 1);
	CHECK((a ^ qlt::top()) == ~a);
}

TEST_CASE("commutativity of union") {
	auto a = oo(0, 1);
	auto b = oo(2, 3);
	CHECK((a | b) == (b | a));
}

TEST_CASE("commutativity of intersection") {
	auto a = oo(0, 2);
	auto b = oo(1, 3);
	CHECK((a & b) == (b & a));
}

TEST_CASE("associativity of union") {
	auto a = oo(0, 1);
	auto b = oo(1, 2);
	auto c = oo(2, 3);
	CHECK(((a | b) | c) == (a | (b | c)));
}

TEST_CASE("associativity of intersection") {
	auto a = oo(0, 3);
	auto b = oo(1, 4);
	auto c = oo(0, 2);
	CHECK(((a & b) & c) == (a & (b & c)));
}

TEST_CASE("De Morgan: ~(A | B) = ~A & ~B") {
	auto a = oo(0, 1);
	auto b = oo(2, 3);
	CHECK((~(a | b)) == (~a & ~b));
}

TEST_CASE("De Morgan: ~(A & B) = ~A | ~B") {
	auto a = oo(0, 2);
	auto b = oo(1, 3);
	CHECK((~(a & b)) == (~a | ~b));
}

TEST_CASE("absorption: A & (A | B) = A") {
	auto a = oo(0, 1);
	auto b = oo(2, 3);
	CHECK((a & (a | b)) == a);
}

TEST_CASE("absorption: A | (A & B) = A") {
	auto a = oo(0, 3);
	auto b = oo(1, 2);
	CHECK((a | (a & b)) == a);
}

TEST_CASE("distributivity: A & (B | C) = (A & B) | (A & C)") {
	auto a = oo(0, 4);
	auto b = oo(1, 2);
	auto c = oo(3, 5);
	CHECK((a & (b | c)) == ((a & b) | (a & c)));
}

TEST_CASE("distributivity: A | (B & C) = (A | B) & (A | C)") {
	auto a = oo(0, 1);
	auto b = oo(0, 3);
	auto c = oo(-1, 2);
	CHECK((a | (b & c)) == ((a | b) & (a | c)));
}

TEST_CASE("bot & bot = bot") {
	CHECK((qlt::bottom() & qlt::bottom()) == qlt::bottom());
}

TEST_CASE("top | top = top") {
	CHECK((qlt::top() | qlt::top()) == qlt::top());
}

TEST_CASE("bot | bot = bot") {
	CHECK((qlt::bottom() | qlt::bottom()) == qlt::bottom());
}

TEST_CASE("top & top = top") {
	CHECK((qlt::top() & qlt::top()) == qlt::top());
}

TEST_CASE("triple negation equals single negation") {
	auto a = oo(0, 1);
	CHECK((~~~a) == (~a));
}

TEST_CASE("intersection sub-interval: (0,1) & (0,1/2) = (0,1/2)") {
	auto a = oo(0, 1);
	auto b = make_interval(
		qlt_rational(0, 1), qlt_bound::OPEN,
		qlt_rational(1, 2), qlt_bound::OPEN);
	auto r = a & b;
	CHECK(r == b);
}

TEST_CASE("union absorbs sub-interval: (0,2) | (0,1) = (0,2)") {
	auto a = oo(0, 2);
	auto b = oo(0, 1);
	CHECK((a | b) == a);
}

} // TEST_SUITE BA operations

// ─── is_zero / is_one ─────────────────────────────────────────────────────────

TEST_SUITE("qlt — is_zero / is_one") {

TEST_CASE("bot is zero") {
	CHECK(is_qlt_zero(qlt::bottom()));
}

TEST_CASE("top is one") {
	CHECK(is_qlt_one(qlt::top()));
}

TEST_CASE("top is not zero") {
	CHECK(!is_qlt_zero(qlt::top()));
}

TEST_CASE("bot is not one") {
	CHECK(!is_qlt_one(qlt::bottom()));
}

TEST_CASE("single open interval is not zero") {
	CHECK(!is_qlt_zero(oo(0, 1)));
}

TEST_CASE("single open interval is not one") {
	CHECK(!is_qlt_one(oo(0, 1)));
}

TEST_CASE("A & ~A is zero") {
	auto a = oo(0, 1);
	CHECK(is_qlt_zero(a & ~a));
}

TEST_CASE("A | ~A is one") {
	auto a = oo(0, 1);
	CHECK(is_qlt_one(a | ~a));
}

TEST_CASE("disjoint union is not zero") {
	auto a = oo(0, 1) | oo(2, 3);
	CHECK(!is_qlt_zero(a));
}

TEST_CASE("disjoint union is not one") {
	auto a = oo(0, 1) | oo(2, 3);
	CHECK(!is_qlt_one(a));
}

TEST_CASE("(-inf,+inf) is one") {
	CHECK(is_qlt_one(qlt::top()));
}

TEST_CASE("normalize_qlt is identity") {
	auto a = oo(0, 1);
	CHECK(normalize_qlt(a) == a);
	CHECK(normalize_qlt(qlt::top()) == qlt::top());
	CHECK(normalize_qlt(qlt::bottom()) == qlt::bottom());
}

} // TEST_SUITE is_zero / is_one

// ─── splitter ─────────────────────────────────────────────────────────────────

TEST_SUITE("qlt — splitter") {

TEST_CASE("splitter of bot is bot") {
	auto s = qlt_splitter(qlt::bottom(), splitter_type::upper);
	CHECK(s == qlt::bottom());
}

TEST_CASE("splitter of top is non-empty") {
	auto s = qlt_splitter(qlt::top(), splitter_type::upper);
	CHECK(!s.is_empty());
}

TEST_CASE("splitter of top is strict sub-element of top") {
	auto s = qlt_splitter(qlt::top(), splitter_type::upper);
	CHECK(s != qlt::top());
}

TEST_CASE("splitter of (0,1) is non-empty") {
	auto a = oo(0, 1);
	auto s = qlt_splitter(a, splitter_type::upper);
	CHECK(!s.is_empty());
}

TEST_CASE("splitter of (0,1) is a strict sub-element") {
	auto a = oo(0, 1);
	auto s = qlt_splitter(a, splitter_type::upper);
	CHECK(s != a);
	// s must be a sub-interval: s & a == s
	CHECK((s & a) == s);
}

TEST_CASE("splitter of closed singleton [5,5] is non-empty") {
	auto a = cc(5, 5);
	auto s = qlt_splitter(a, splitter_type::upper);
	CHECK(!s.is_empty());
}

TEST_CASE("splitter of (-inf,0) is non-empty and sub-element") {
	auto a = neg_inf_open(0);
	auto s = qlt_splitter(a, splitter_type::upper);
	CHECK(!s.is_empty());
	CHECK((s & a) == s);
}

TEST_CASE("splitter of (0,+inf) is non-empty and sub-element") {
	auto a = open_pos_inf(0);
	auto s = qlt_splitter(a, splitter_type::upper);
	CHECK(!s.is_empty());
	CHECK((s & a) == s);
}

TEST_CASE("splitter_one is non-empty") {
	auto s = qlt_splitter_one();
	CHECK(!s.is_empty());
}

TEST_CASE("splitter_one is not top") {
	CHECK(qlt_splitter_one() != qlt::top());
}

TEST_CASE("splitter_one is (0,1)") {
	auto s = qlt_splitter_one();
	CHECK(s == oo(0, 1));
}

TEST_CASE("splitter of two-piece union uses first piece") {
	auto a = oo(0, 1) | oo(2, 3);
	auto s = qlt_splitter(a, splitter_type::upper);
	CHECK(!s.is_empty());
	CHECK((s & a) == s); // s is a sub-element
}

} // TEST_SUITE splitter

// ─── normalisation ────────────────────────────────────────────────────────────

TEST_SUITE("qlt — normalisation") {

TEST_CASE("union of three overlapping intervals normalises to one") {
	auto a = oo(0, 2);
	auto b = oo(1, 3);
	auto c = oo(2, 4);
	auto r = a | b | c;
	CHECK(r.pieces.size() == 1);
	CHECK(r.pieces[0].lo.val == qlt_rational(0, 1));
	CHECK(r.pieces[0].hi.val == qlt_rational(4, 1));
}

TEST_CASE("union of three disjoint intervals keeps three pieces") {
	auto a = oo(0, 1);
	auto b = oo(2, 3);
	auto c = oo(4, 5);
	auto r = a | b | c;
	CHECK(r.pieces.size() == 3);
}

TEST_CASE("union of reversed order pieces normalises (sorted)") {
	// b | a: even though b.lo > a.lo, result should be sorted
	auto a = oo(0, 1);
	auto b = oo(2, 3);
	auto r = b | a;
	CHECK(r.pieces.size() == 2);
	CHECK(r.pieces[0].lo.val < r.pieces[1].lo.val);
}

TEST_CASE("union of adjacent half-opens [0,1) and [1,2) collapses to [0,2)") {
	auto a = co(0, 1);
	auto b = co(1, 2);
	auto r = a | b;
	CHECK(r.pieces.size() == 1);
	CHECK(r.pieces[0].lo.bound == qlt_bound::CLOSED);
	CHECK(r.pieces[0].hi.bound == qlt_bound::OPEN);
	CHECK(r.pieces[0].lo.val == qlt_rational(0, 1));
	CHECK(r.pieces[0].hi.val == qlt_rational(2, 1));
}

TEST_CASE("complement of union equals intersection of complements") {
	auto a = oo(0, 1);
	auto b = oo(2, 3);
	CHECK(~(a | b) == (~a & ~b));
}

TEST_CASE("union with self is idempotent after normalisation") {
	auto a = oo(0, 1) | oo(2, 3);
	CHECK((a | a) == a);
}

TEST_CASE("intersection distributes over union after normalisation") {
	auto a = oo(0, 5);
	auto b = oo(1, 2);
	auto c = oo(3, 4);
	auto lhs = a & (b | c);
	auto rhs = (a & b) | (a & c);
	CHECK(lhs == rhs);
}

TEST_CASE("to_string of union of two intervals") {
	auto a = oo(0, 1) | oo(2, 3);
	auto s = a.to_string();
	// Should contain two interval representations separated by " | "
	CHECK(s.find(" | ") != std::string::npos);
}

TEST_CASE("to_string of single piece with -inf endpoint") {
	auto a = neg_inf_open(0);
	auto s = a.to_string();
	CHECK(s.find("-inf") != std::string::npos);
}

TEST_CASE("to_string of single piece with +inf endpoint") {
	auto a = open_pos_inf(0);
	auto s = a.to_string();
	CHECK(s.find("+inf") != std::string::npos);
}

} // TEST_SUITE normalisation

// ─── hash ──────────────────────────────────────────────────────────────────────

TEST_SUITE("qlt — hash") {

TEST_CASE("equal qlt values have equal hashes") {
	auto h = std::hash<qlt>{};
	CHECK(h(oo(0, 1)) == h(oo(0, 1)));
}

TEST_CASE("equal qlt_rational values have equal hashes") {
	auto h = std::hash<qlt_rational>{};
	CHECK(h(qlt_rational(1, 2)) == h(qlt_rational(2, 4)));
}

TEST_CASE("pos_inf and neg_inf have different hashes") {
	auto h = std::hash<qlt_rational>{};
	CHECK(h(qlt_rational::make_pos_inf()) != h(qlt_rational::make_neg_inf()));
}

TEST_CASE("bot and top have different hashes") {
	auto h = std::hash<qlt>{};
	CHECK(h(qlt::bottom()) != h(qlt::top()));
}

} // TEST_SUITE hash
