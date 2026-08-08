// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "qlt.h"

namespace idni::tau_lang {

// BA1-4: pedantic-clean 128-bit alias for overflow-safe rational arithmetic.
__extension__ typedef __int128 int128_t_;

// --- internal helper ---

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-overflow"
static long long qlt_gcd(long long a, long long b) {
	unsigned long long ua = a < 0 ? 0ULL - (unsigned long long)a : (unsigned long long)a;
	unsigned long long ub = b < 0 ? 0ULL - (unsigned long long)b : (unsigned long long)b;
	while (ub) { ua %= ub; std::swap(ua, ub); }
	return (long long)(ua ? ua : 1ULL);
}
#pragma GCC diagnostic pop

// --- qlt_rational ---

void qlt_rational::normalise() {
	if (is_sym()) return; // symbolic: no reduction
	if (q < 0) { p = -p; q = -q; }
	long long g = qlt_gcd(p < 0 ? -p : p, q);
	p /= g; q /= g;
}

bool qlt_rational::operator==(const qlt_rational& o) const {
	if (is_sym() || o.is_sym()) return sym == o.sym;
	if (pos_inf || o.pos_inf) return pos_inf && o.pos_inf;
	if (neg_inf || o.neg_inf) return neg_inf && o.neg_inf;
	return p == o.p && q == o.q;
}

bool qlt_rational::operator<(const qlt_rational& o) const {
	// Both symbolic: lex
	if (is_sym() && o.is_sym()) return sym < o.sym;
	// Symbolic > everything non-symbolic
	if (is_sym())   return false;
	if (o.is_sym()) return true;
	// Both non-symbolic
	if (neg_inf) return !o.neg_inf;
	if (o.pos_inf) return !pos_inf;
	if (pos_inf || o.neg_inf) return false;
	// BA1-4: cross-multiplication overflows long long for parse-reachable
	// magnitudes (~9.2e18); widen to __int128.
	return (int128_t_) p * o.q < (int128_t_) o.p * q;
}

qlt_rational qlt_rational::midpoint(const qlt_rational& o) const {
	// BA1-4: compute wide, then reduce by gcd before narrowing -- the
	// unreduced cross products overflow long long for large operands.
	int128_t_ num = (int128_t_) p * o.q + (int128_t_) o.p * q;
	int128_t_ den = (int128_t_) 2 * q * o.q;
	int128_t_ a = num < 0 ? -num : num, b = den < 0 ? -den : den;
	while (b) { int128_t_ t = a % b; a = b; b = t; }
	if (a > 1) num /= a, den /= a;
	// A gcd-irreducible result outside long long keeps the historical
	// truncation (extreme parse-level literals only).
	return qlt_rational((long long) num, (long long) den);
}

qlt_rational qlt_rational::operator+(const qlt_rational& o) const {
	return qlt_rational(p * o.q + o.p * q, q * o.q);
}

std::string qlt_rational::to_string() const {
	if (!sym.empty()) return sym;
	if (pos_inf) return "+inf";
	if (neg_inf) return "-inf";
	if (q == 1) return std::to_string(p);
	return std::to_string(p) + "/" + std::to_string(q);
}

bool qlt_rational::parse(const std::string& s, qlt_rational& out) {
	std::string t = s;
	t.erase(0, t.find_first_not_of(" \t\n\r"));
	auto last = t.find_last_not_of(" \t\n\r");
	if (last != std::string::npos) t = t.substr(0, last + 1);

	if (t == "+inf" || t == "inf") { out = make_pos_inf(); return true; }
	if (t == "-inf") { out = make_neg_inf(); return true; }

	// Named uninterpreted constant: identifier that is not a number
	if (!t.empty() && (std::isalpha((unsigned char)t[0]) || t[0] == '_')) {
		bool ok = true;
		for (char c : t)
			if (!std::isalnum((unsigned char)c) && c != '_') { ok = false; break; }
		if (ok) { out = make_sym(t); return true; }
	}

	auto slash = t.find('/');
	if (slash != std::string::npos) {
		long long p2 = 0, q2 = 0;
		auto pstr = t.substr(0, slash);
		auto qstr = t.substr(slash + 1);
		pstr.erase(0, pstr.find_first_not_of(" \t"));
		qstr.erase(0, qstr.find_first_not_of(" \t"));
		auto lp = pstr.find_last_not_of(" \t");
		if (lp != std::string::npos) pstr = pstr.substr(0, lp + 1);
		auto lq = qstr.find_last_not_of(" \t");
		if (lq != std::string::npos) qstr = qstr.substr(0, lq + 1);
		auto [pp, pe] = std::from_chars(pstr.data(), pstr.data() + pstr.size(), p2);
		auto [qp, qe] = std::from_chars(qstr.data(), qstr.data() + qstr.size(), q2);
		if (pe != std::errc{} || qe != std::errc{}) return false;
		if (q2 <= 0) return false;
		out = qlt_rational(p2, q2);
		return true;
	}

	// Decimal notation: "0.45", "-1.5", "3.14", ".5", "5."
	auto dot = t.find('.');
	if (dot != std::string::npos) {
		// Split into integer and fractional parts; both optional individually
		// (".5" and "5." are valid).
		auto int_str  = t.substr(0, dot);
		auto frac_str = t.substr(dot + 1);
		// Reject further dots or slashes in the fraction part.
		if (frac_str.find('.') != std::string::npos) return false;
		if (frac_str.find('/') != std::string::npos) return false;
		bool negative = false;
		if (!int_str.empty() && (int_str[0] == '+' || int_str[0] == '-')) {
			negative = (int_str[0] == '-');
			int_str = int_str.substr(1);
		}
		// After stripping sign, both parts must be digits (int_str may be
		// empty for ".5"; frac_str may be empty for "5.").
		auto is_digits = [](const std::string& s) {
			for (char c : s)
				if (c < '0' || c > '9') return false;
			return !s.empty();
		};
		if (int_str.empty() && frac_str.empty()) return false;
		if (!int_str.empty() && !is_digits(int_str)) return false;
		if (!frac_str.empty() && !is_digits(frac_str)) return false;
		long long ipart = 0;
		if (!int_str.empty()) {
			auto [ip, ie] = std::from_chars(int_str.data(),
			                  int_str.data() + int_str.size(), ipart);
			if (ie != std::errc{}) return false;
		}
		long long fpart = 0;
		long long scale = 1;
		if (!frac_str.empty()) {
			auto [fp, fe] = std::from_chars(frac_str.data(),
			                  frac_str.data() + frac_str.size(), fpart);
			if (fe != std::errc{}) return false;
			for (size_t i = 0; i < frac_str.size(); ++i) {
				if (scale > 1'000'000'000'000'000LL) return false; // overflow guard
				scale *= 10;
			}
		}
		// ipart + fpart/scale = (ipart*scale + fpart)/scale
		long long num = ipart * scale + fpart;
		if (negative) num = -num;
		out = qlt_rational(num, scale);
		return true;
	}

	long long val = 0;
	auto [ptr, ec] = std::from_chars(t.data(), t.data() + t.size(), val);
	if (ec != std::errc{}) return false;
	const char* tend = t.data() + t.size();
	while (ptr < tend && (*ptr == ' ' || *ptr == '\t')) ++ptr;
	if (ptr < tend) return false;
	out = qlt_rational(val, 1);
	return true;
}

// --- semantic endpoint comparison ---

// `qlt_rational::operator<` is a canonical total order used for sorting,
// hashing and canonical forms: it places named constants after +inf.  That is
// NOT the order of Q.  A named constant denotes an unknown rational, so the
// only decidable facts about it are that it lies strictly between -inf and
// +inf and that it equals itself; its position relative to a specific
// rational, or to a differently-named constant, is unknown.
//
// Reading the canonical order as semantic is what made `{c} & ~{c}` come out
// as `(c, +inf)`: `qlt_hi_min({c,CLOSED}, {+inf,OPEN})` answered `+inf`
// because `c < +inf` is false canonically.
std::partial_ordering qlt_sem_cmp(const qlt_rational& a, const qlt_rational& b)
{
	if (a == b) return std::partial_ordering::equivalent;
	if (a.is_sym()) {
		if (b.is_neg_inf()) return std::partial_ordering::greater;
		if (b.is_pos_inf()) return std::partial_ordering::less;
		// vs a specific rational or another named constant
		return std::partial_ordering::unordered;
	}
	if (b.is_sym()) {
		if (a.is_neg_inf()) return std::partial_ordering::less;
		if (a.is_pos_inf()) return std::partial_ordering::greater;
		return std::partial_ordering::unordered;
	}
	// Both non-symbolic: the canonical order is the semantic one.
	return a < b ? std::partial_ordering::less
		     : std::partial_ordering::greater;
}

// --- interval helpers ---

// (BA1-3: qlt_lo_less deleted -- zero callers, and its tie-break was
// inverted relative to its own comment and to normalise's live comparator.)

// Does piece p contain point val (for overlap testing)?
// Pieces are intervals, so we check lo <= x < hi (or lo < x <= hi, etc.)
// For overlap detection we check if two intervals overlap at all.
// Intervals [a1,b1) and [a2,b2) overlap iff a1 < b2 and a2 < b1 (for half-open).
// For general bounds, intervals overlap iff not (b1 < a2 or b2 < a1).

// Returns: does the interval "include" point x as its left end?
// i.e., is x above the lo boundary?
// If either is a named constant the ordering is unknown → return false (conservative).
bool qlt_above_lo(const qlt_endpoint& lo, const qlt_rational& x) {
	if (lo.val.is_sym() || x.is_sym()) return false;
	if (lo.val.is_neg_inf()) return true;
	if (lo.bound == qlt_bound::CLOSED) return x >= lo.val;
	return x > lo.val;
}

bool qlt_below_hi(const qlt_endpoint& hi, const qlt_rational& x) {
	if (hi.val.is_sym() || x.is_sym()) return false;
	if (hi.val.is_pos_inf()) return true;
	if (hi.bound == qlt_bound::CLOSED) return x <= hi.val;
	return x < hi.val;
}

// Compare two endpoints as upper bounds: which is smaller?
// Returns true if a is semantically below b, or they sit at the same value but
// a is "tighter" (open < closed at the same point).  An undecidable comparison
// (symbolic endpoint) is not "less".
bool qlt_hi_less(const qlt_endpoint& a, const qlt_endpoint& b) {
	auto c = qlt_sem_cmp(a.val, b.val);
	if (c == std::partial_ordering::less)    return true;
	if (c == std::partial_ordering::greater) return false;
	if (c == std::partial_ordering::unordered) return false; // unknown
	// same val: OPEN upper < CLOSED upper (open endpoint excludes the point, so it's "lower")
	if (a.val.is_pos_inf()) return false;
	return (int)a.bound < (int)b.bound; // OPEN(0) < CLOSED(1)
}

// The lower endpoint that is further right (for intersection lower).
// `nullopt` when the two values are not semantically comparable.
std::optional<qlt_endpoint> qlt_lo_max(const qlt_endpoint& a, const qlt_endpoint& b) {
	auto c = qlt_sem_cmp(a.val, b.val);
	if (c == std::partial_ordering::unordered) return std::nullopt;
	if (c == std::partial_ordering::less)      return b;
	if (c == std::partial_ordering::greater)   return a;
	if (a.val.is_neg_inf()) return a;
	// same value: OPEN is more restrictive (excludes the point)
	return (a.bound == qlt_bound::OPEN) ? a : b;
}

// The upper endpoint that is further left (for intersection upper).
// `nullopt` when the two values are not semantically comparable.
std::optional<qlt_endpoint> qlt_hi_min(const qlt_endpoint& a, const qlt_endpoint& b) {
	auto c = qlt_sem_cmp(a.val, b.val);
	if (c == std::partial_ordering::unordered) return std::nullopt;
	if (c == std::partial_ordering::less)      return a;
	if (c == std::partial_ordering::greater)   return b;
	if (a.val.is_pos_inf()) return a;
	// same value: OPEN is more restrictive
	return (a.bound == qlt_bound::OPEN) ? a : b;
}

// The upper endpoint that is further right (for union upper)
qlt_endpoint qlt_hi_max(const qlt_endpoint& a, const qlt_endpoint& b) {
	if (qlt_hi_less(a, b)) return b;
	return a;
}

// Is interval empty? (lo >= hi in the sense that lo endpoint >= hi endpoint)
// When the two endpoints are not comparable (symbolic) the emptiness is
// unknown, and the piece is kept: this algebra over-approximates undecidable
// cases, because dropping a piece would turn a satisfiable constraint such as
// `x = {c} && 0 <= x <= 1` into a wrong UNSAT.
bool qlt_piece_empty(const qlt_piece& p) {
	auto c = qlt_sem_cmp(p.lo.val, p.hi.val);
	if (c == std::partial_ordering::greater)   return true;
	if (c == std::partial_ordering::unordered) return false; // assume non-empty
	if (c == std::partial_ordering::equivalent) {
		// [c, c] with the same name is a valid singleton, as is [x, x];
		// an open bound at either end makes it empty, and ±inf is never
		// a member of the interval.
		if (p.lo.val.is_pos_inf() || p.lo.val.is_neg_inf()) return true;
		return !(p.lo.bound == qlt_bound::CLOSED && p.hi.bound == qlt_bound::CLOSED);
	}
	return false;
}

// Do two intervals overlap (share at least one point)?
// An undecidable endpoint comparison is not an overlap: `normalise` only
// merges pieces it can merge exactly, and leaving two pieces unmerged keeps
// the union exact (only the canonical form is lost).
bool qlt_pieces_overlap(const qlt_piece& a, const qlt_piece& b) {
	auto hi_above_lo = [](const qlt_endpoint& hi, const qlt_endpoint& lo) -> bool {
		auto c = qlt_sem_cmp(hi.val, lo.val);
		if (c == std::partial_ordering::less)      return false;
		if (c == std::partial_ordering::greater)   return true;
		if (c == std::partial_ordering::unordered) return false;
		if (hi.val.is_pos_inf() || hi.val.is_neg_inf()) return false;
		return hi.bound == qlt_bound::CLOSED && lo.bound == qlt_bound::CLOSED;
	};
	return hi_above_lo(a.hi, b.lo) && hi_above_lo(b.hi, a.lo);
}

// Are two intervals "adjacent" (touching but not overlapping)?
// Two pieces touching at the *same* named constant are adjacent -- that is
// what makes `{c} | ~{c}` collapse to top.
bool qlt_pieces_adjacent(const qlt_piece& a, const qlt_piece& b) {
	if (a.hi.val != b.lo.val) return false;
	if (a.hi.val.is_pos_inf() || a.hi.val.is_neg_inf()) return false;
	return (a.hi.bound == qlt_bound::OPEN && b.lo.bound == qlt_bound::CLOSED)
		|| (a.hi.bound == qlt_bound::CLOSED && b.lo.bound == qlt_bound::OPEN)
		|| (a.hi.bound == qlt_bound::CLOSED && b.lo.bound == qlt_bound::CLOSED);
}

// May the two pieces be merged into one?  Overlapping or adjacent is not
// enough: the merged piece takes min(lo) and max(hi), so both of those have to
// be decidable too.  `(-inf,0)` and `(-inf,c)` do overlap, but their union is
// `(-inf, max(0,c))`, which is not representable -- merging them would silently
// drop one of the two upper bounds.
bool qlt_pieces_mergeable(const qlt_piece& a, const qlt_piece& b) {
	if (!qlt_pieces_overlap(a, b) && !qlt_pieces_adjacent(a, b)) return false;
	return qlt_sem_cmp(a.lo.val, b.lo.val) != std::partial_ordering::unordered
		&& qlt_sem_cmp(a.hi.val, b.hi.val) != std::partial_ordering::unordered;
}

// Merge two mergeable (see qlt_pieces_mergeable) intervals
qlt_piece qlt_merge(const qlt_piece& a, const qlt_piece& b) {
	// lower bound = min(a.lo, b.lo)
	qlt_endpoint lo, hi;
	auto c = qlt_sem_cmp(a.lo.val, b.lo.val);
	if (c == std::partial_ordering::less) lo = a.lo;
	else if (c == std::partial_ordering::greater) lo = b.lo;
	else {
		// same val: CLOSED is more inclusive
		lo.val = a.lo.val;
		lo.bound = (a.lo.bound == qlt_bound::CLOSED || b.lo.bound == qlt_bound::CLOSED)
			? qlt_bound::CLOSED : qlt_bound::OPEN;
	}
	// upper bound = max(a.hi, b.hi)
	hi = qlt_hi_max(a.hi, b.hi);
	return qlt_piece{lo, hi};
}

// Intersection of two pieces; nullopt when the result is empty.
//
// When every endpoint comparison is decidable this is exact: [max(lo), min(hi)].
// When it is not -- a named constant against a specific rational, or two
// different names -- there is no representable exact answer, and the three
// available choices are not equal:
//   * inventing a mixed endpoint pair (the old behaviour: `{c} & [0,1]` =
//     `[c,1]`) yields a set that is a subset of neither operand: always wrong;
//   * dropping the piece under-approximates, which would make
//     `x = {c} && 0 <= x <= 1` report UNSAT even though it holds for
//     c in [0,1] -- a silently wrong verdict;
//   * keeping one whole operand over-approximates (A & B is a subset of A), the
//     same conservative direction `qlt_piece_empty` already takes for symbolic
//     endpoints.
// We take the third.  The operand kept is the symbolic one -- it is the more
// specific description of the two, so `{c} & [0,1]` = `{c}`, the correct
// over-approximation of "{c} if c in [0,1], else bot".  If both are symbolic
// we keep the canonically smaller piece so that `&` stays commutative.
// Nothing decidable is lost by this: `({c} & [0,1]) & ~{c}` still collapses to
// bot through the exact equal-name path.
//
// Caveat -- this is a policy, not one-sided soundness.  Over-approximation is
// not closed under complement: `operator~` is exact, so negating an
// over-approximated set under-approximates.  `~({c} & [0,1])` computes as
// `~{c}` = `(-inf,c) | (c,+inf)`, whereas the true complement is all of Q when
// c is outside [0,1]; conjoining `x = {c}` with it then yields bot, a wrong
// UNSAT reached through this very convention.  The trade-off is deliberate:
// every choice here is wrong for some model, and this one at least keeps `&`
// exact wherever the comparison is decidable and never returns an endpoint
// pair that was not already present in an operand.  Sound treatment of
// undecidable comparisons needs case splits (or a constraint store), which
// this representation cannot express.
std::optional<qlt_piece> qlt_piece_intersect(const qlt_piece& a, const qlt_piece& b)
{
	auto lo = qlt_lo_max(a.lo, b.lo);
	auto hi = qlt_hi_min(a.hi, b.hi);
	if (lo && hi) {
		qlt_piece p{*lo, *hi};
		if (qlt_piece_empty(p)) return std::nullopt;
		return p;
	}
	const bool a_sym = a.lo.val.is_sym() || a.hi.val.is_sym();
	const bool b_sym = b.lo.val.is_sym() || b.hi.val.is_sym();
	// An undecidable comparison always involves a symbolic endpoint, so at
	// least one of the two flags is set here.
	if (a_sym && !b_sym) return a;
	if (b_sym && !a_sym) return b;
	if (a.lo.val != b.lo.val) return (a.lo.val < b.lo.val) ? a : b;
	return (a.hi.val < b.hi.val) ? a : b;
}

// --- qlt ---

qlt qlt::top() {
	qlt_piece p;
	p.lo = qlt_endpoint{ qlt_rational::make_neg_inf(), qlt_bound::OPEN };
	p.hi = qlt_endpoint{ qlt_rational::make_pos_inf(), qlt_bound::OPEN };
	return qlt{{ p }};
}

bool qlt::is_full() const {
	return pieces.size() == 1
		&& pieces[0].lo.val.is_neg_inf()
		&& pieces[0].hi.val.is_pos_inf();
}

bool qlt::operator<(const qlt& o) const {
	if (pieces.size() != o.pieces.size())
		return pieces.size() < o.pieces.size();
	for (size_t i = 0; i < pieces.size(); ++i) {
		if (pieces[i].lo.val != o.pieces[i].lo.val)
			return pieces[i].lo.val < o.pieces[i].lo.val;
		if (pieces[i].lo.bound != o.pieces[i].lo.bound)
			return (int)pieces[i].lo.bound < (int)o.pieces[i].lo.bound;
		if (pieces[i].hi.val != o.pieces[i].hi.val)
			return pieces[i].hi.val < o.pieces[i].hi.val;
		if (pieces[i].hi.bound != o.pieces[i].hi.bound)
			return (int)pieces[i].hi.bound < (int)o.pieces[i].hi.bound;
	}
	return false;
}

std::strong_ordering qlt::operator<=>(const qlt& o) const {
	if (*this == o) return std::strong_ordering::equal;
	return *this < o ? std::strong_ordering::less : std::strong_ordering::greater;
}

qlt qlt::operator|(const qlt& o) const {
	std::vector<qlt_piece> ps;
	ps.reserve(pieces.size() + o.pieces.size());
	for (auto& p : pieces) ps.push_back(p);
	for (auto& p : o.pieces) ps.push_back(p);
	return normalise(std::move(ps));
}

qlt qlt::operator&(const qlt& o) const {
	// Pairwise rather than the usual sorted merge-sweep: the sweep advances
	// by comparing upper endpoints, and with symbolic endpoints that
	// comparison can be undecidable, so it would skip pairs and silently
	// drop pieces (`({c} | {d}) & [0,1]` would lose `{d}`).  Both operands
	// hold a handful of pieces, so the quadratic pass costs nothing, and
	// `normalise` restores sortedness and merges what is mergeable.
	std::vector<qlt_piece> result;
	result.reserve(pieces.size() * o.pieces.size());
	for (const auto& p : pieces)
		for (const auto& q : o.pieces)
			if (auto r = qlt_piece_intersect(p, q); r)
				result.push_back(*r);
	return normalise(std::move(result));
}

qlt qlt::operator~() const {
	if (pieces.empty()) return top();
	std::vector<qlt_piece> result;
	// Start from -inf
	qlt_endpoint cur{ qlt_rational::make_neg_inf(), qlt_bound::OPEN };
	for (auto& p : pieces) {
		// gap from cur to p.lo
		qlt_endpoint new_hi{ p.lo.val,
			p.lo.bound == qlt_bound::CLOSED ? qlt_bound::OPEN : qlt_bound::CLOSED };
		qlt_piece gap{cur, new_hi};
		if (!qlt_piece_empty(gap)) result.push_back(gap);
		// next cur = just after p.hi
		cur = qlt_endpoint{ p.hi.val,
			p.hi.bound == qlt_bound::CLOSED ? qlt_bound::OPEN : qlt_bound::CLOSED };
	}
	// gap from last piece to +inf
	qlt_endpoint end{ qlt_rational::make_pos_inf(), qlt_bound::OPEN };
	qlt_piece tail{cur, end};
	if (!qlt_piece_empty(tail)) result.push_back(tail);
	return qlt{std::move(result)};
}

qlt qlt::operator^(const qlt& o) const {
	return (*this | o) & ~(*this & o);
}

std::string qlt::to_string() const {
	if (pieces.empty()) return "bot";
	if (is_full()) return "top";
	std::string s;
	for (size_t i = 0; i < pieces.size(); ++i) {
		if (i > 0) s += " | ";
		const auto& lo = pieces[i].lo;
		const auto& hi = pieces[i].hi;
		// Display singleton [c, c] (specific or symbolic) as just "c"
		if (lo.bound == qlt_bound::CLOSED && hi.bound == qlt_bound::CLOSED
		    && lo.val == hi.val && (lo.val.is_finite() || lo.val.is_sym()))
		{
			s += lo.val.to_string();
		} else {
			s += (lo.bound == qlt_bound::CLOSED ? "[" : "(");
			s += lo.val.to_string() + ", " + hi.val.to_string();
			s += (hi.bound == qlt_bound::CLOSED ? "]" : ")");
		}
	}
	return s;
}

// Build a normalised (sorted, merged) list from an unsorted set of pieces
qlt qlt::normalise(std::vector<qlt_piece> ps) {
	if (ps.empty()) return {};
	// remove empty pieces
	ps.erase(std::remove_if(ps.begin(), ps.end(), qlt_piece_empty), ps.end());
	if (ps.empty()) return {};
	// Sort by lower bound, in the canonical order.  The upper bound is a
	// tie-break rather than an afterthought: it makes the order total, so
	// structurally identical pieces always end up adjacent and get merged
	// below even when several pieces share a lower endpoint.
	std::sort(ps.begin(), ps.end(), [](const qlt_piece& a, const qlt_piece& b) {
		if (a.lo.val != b.lo.val) return a.lo.val < b.lo.val;
		// CLOSED lower bound comes first (further left)
		if (a.lo.bound != b.lo.bound)
			return (int)a.lo.bound > (int)b.lo.bound; // CLOSED=1 > OPEN=0
		if (a.hi.val != b.hi.val) return a.hi.val < b.hi.val;
		// OPEN upper bound comes first (further left)
		return (int)a.hi.bound < (int)b.hi.bound;
	});
	std::vector<qlt_piece> norm;
	norm.push_back(ps[0]);
	for (size_t k = 1; k < ps.size(); ++k) {
		auto& last = norm.back();
		// Structurally identical pieces always merge (that is what makes
		// `{c} | {c}` idempotent); beyond that only pieces whose union is
		// representable do -- see qlt_pieces_mergeable.
		if (last == ps[k] || qlt_pieces_mergeable(last, ps[k])) {
			last = qlt_merge(last, ps[k]);
		} else {
			norm.push_back(ps[k]);
		}
	}
	return qlt{std::move(norm)};
}

// --- parsing helpers ---

std::optional<qlt> qlt_eval_interval(
	const qlt_parser::tree::traverser& interval_node)
{
	using tt = qlt_parser::tree::traverser;
	using type = qlt_parser::nonterminal;

	auto children = (interval_node | tt::children)();

	std::vector<std::string> endpoints;
	qlt_bound lo_b = qlt_bound::OPEN, hi_b = qlt_bound::OPEN;
	bool found_bracket = false;

	for (auto& child : children) {
		auto child_nt = child | tt::nonterminal;
		if      (child_nt == type::interval_cc) { lo_b = qlt_bound::CLOSED; hi_b = qlt_bound::CLOSED; found_bracket = true; }
		else if (child_nt == type::interval_co) { lo_b = qlt_bound::CLOSED; hi_b = qlt_bound::OPEN;   found_bracket = true; }
		else if (child_nt == type::interval_oc) { lo_b = qlt_bound::OPEN;   hi_b = qlt_bound::CLOSED; found_bracket = true; }
		else if (child_nt == type::interval_oo) { lo_b = qlt_bound::OPEN;   hi_b = qlt_bound::OPEN;   found_bracket = true; }
		else {
			auto ep = child | tt::terminals;
			if (!ep.empty()) endpoints.push_back(ep);
		}
	}

	if (endpoints.size() < 2 || !found_bracket) return std::nullopt;

	qlt_rational lo_r, hi_r;
	if (!qlt_rational::parse(endpoints[0], lo_r)) return std::nullopt;
	if (!qlt_rational::parse(endpoints[1], hi_r)) return std::nullopt;

	if (!lo_r.is_sym() && !hi_r.is_sym()) {
		if (hi_r < lo_r) return std::nullopt;
		if (lo_r == hi_r &&
		    !(lo_b == qlt_bound::CLOSED && hi_b == qlt_bound::CLOSED))
			return std::nullopt;
	}

	return qlt{{ { qlt_endpoint{lo_r, lo_b}, qlt_endpoint{hi_r, hi_b} } }};
}

std::optional<qlt> qlt_eval_parse_tree(
	const qlt_parser::tree::traverser& t)
{
	using tt = qlt_parser::tree::traverser;
	using type = qlt_parser::nonterminal;

	auto n  = t | tt::only_child;
	auto nt = n | tt::nonterminal;

	switch (nt) {
	case type::qlt_top:
		return qlt::top();

	case type::qlt_bot:
		return qlt::bottom();

	case type::qlt_singleton: {
		auto children = (n | tt::children)();
		if (children.empty()) return std::nullopt;
		auto s = children[0] | tt::terminals;
		qlt_rational val;
		if (!qlt_rational::parse(s, val)) return std::nullopt;
		if (val.is_pos_inf() || val.is_neg_inf()) return std::nullopt;
		return qlt{{ {
			qlt_endpoint{val, qlt_bound::CLOSED},
			qlt_endpoint{val, qlt_bound::CLOSED}
		}}};
	}

	case type::qlt_single: {
		auto children = (n | tt::children)();
		if (children.empty()) return std::nullopt;
		return qlt_eval_interval(children[0]);
	}

	case type::qlt_union: {
		// qlt_union children = [interval, qlt] (after __E_qlt_0 inlining)
		auto children = (n | tt::children)();
		if (children.size() < 2) return std::nullopt;

		auto left = qlt_eval_interval(children[0]);
		if (!left) return std::nullopt;

		auto right = qlt_eval_parse_tree(children[1]);
		if (!right) return std::nullopt;

		return *left | *right;
	}

	default:
		LOG_ERROR << "Unknown qlt node type\n";
		return std::nullopt;
	}
}

// --- stream output ---

std::ostream& operator<<(std::ostream& os, const qlt& q) {
	return os << q.to_string();
}

// --- free functions expected by the dispatcher ---

bool is_qlt_zero(const qlt& x) { return x.is_empty(); }
bool is_qlt_one(const qlt& x) { return x.is_full(); }
qlt normalize_qlt(const qlt& x) { return x; }
tref simplify_qlt_symbol(tref sym) { return sym; }
tref simplify_qlt_term(tref t) { return t; }

qlt qlt_splitter(const qlt& x, splitter_type /*st*/) {
	if (x.is_empty()) return qlt::bottom();
	const auto& p = x.pieces[0];

	// Finite endpoints: take left half of first piece
	auto lo_val = p.lo.val;
	auto hi_val = p.hi.val;

	if (lo_val.is_neg_inf() && hi_val.is_pos_inf()) {
		// Full piece — return (-1, 0)
		return qlt{{ { qlt_endpoint{qlt_rational(-1,1), qlt_bound::OPEN},
		                  qlt_endpoint{qlt_rational(0,1),  qlt_bound::OPEN} } }};
	}

	if (lo_val.is_neg_inf()) {
		// Take (-inf, midpoint)
		qlt_rational mid;
		if (hi_val.is_finite()) {
			// midpoint between hi - 1 and hi
			mid = qlt_rational(hi_val.p - hi_val.q, hi_val.q);
		} else {
			mid = qlt_rational(0, 1);
		}
		return qlt{{ { qlt_endpoint{qlt_rational::make_neg_inf(), qlt_bound::OPEN},
		                  qlt_endpoint{mid, qlt_bound::OPEN} } }};
	}

	if (hi_val.is_pos_inf()) {
		// Take [lo, lo+1)
		qlt_rational mid(lo_val.p + lo_val.q, lo_val.q);
		return qlt{{ { p.lo,
		                  qlt_endpoint{mid, qlt_bound::OPEN} } }};
	}

	// Both finite: use midpoint
	qlt_rational mid = lo_val.midpoint(hi_val);
	// If lo == mid or mid == hi (degenerate), just return the piece
	if (mid <= lo_val || mid >= hi_val)
		return qlt{{ p }};

	return qlt{{ { p.lo, qlt_endpoint{mid, qlt_bound::OPEN} } }};
}

qlt qlt_splitter_one() {
	// A fixed non-zero, non-full element: (0, 1)
	return qlt{{ { qlt_endpoint{qlt_rational(0,1), qlt_bound::OPEN},
	                  qlt_endpoint{qlt_rational(1,1), qlt_bound::OPEN} } }};
}

} // namespace idni::tau_lang
