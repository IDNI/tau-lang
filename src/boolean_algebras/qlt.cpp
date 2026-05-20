// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "qlt.h"

namespace idni::tau_lang {

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
	return p * o.q < o.p * q;
}

qlt_rational qlt_rational::midpoint(const qlt_rational& o) const {
	long long num = p * o.q + o.p * q;
	long long den = 2 * q * o.q;
	return qlt_rational(num, den);
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

// --- interval helpers ---

// Compare: lo1 endpoint < lo2 endpoint (for sorting)
bool qlt_lo_less(const qlt_piece& a, const qlt_piece& b) {
	if (a.lo.val != b.lo.val) return a.lo.val < b.lo.val;
	// -inf [closed/-inf open: treat equal; for finite: CLOSED < OPEN
	if (a.lo.val.is_neg_inf()) return false;
	return (int)a.lo.bound < (int)b.lo.bound; // OPEN(0) < CLOSED(1)? No: closed is lower
	// Actually for a lower endpoint: CLOSED means includes the point (lower),
	// OPEN means excludes it (so the interval starts just after).
	// For sorting: CLOSED endpoint at x is to the "left" of OPEN endpoint at x.
}

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
// Returns true if a.val < b.val, or same val but a is "tighter" (open < closed at same point)
bool qlt_hi_less(const qlt_endpoint& a, const qlt_endpoint& b) {
	if (a.val != b.val) return a.val < b.val;
	// same val: OPEN upper < CLOSED upper (open endpoint excludes the point, so it's "lower")
	if (a.val.is_pos_inf()) return false;
	return (int)a.bound < (int)b.bound; // OPEN(0) < CLOSED(1)
}

// The lower endpoint that is further right (for intersection lower)
qlt_endpoint qlt_lo_max(const qlt_endpoint& a, const qlt_endpoint& b) {
	if (a.val != b.val) return (a.val < b.val) ? b : a;
	if (a.val.is_neg_inf()) return a;
	// same value: OPEN is more restrictive (excludes the point)
	return (a.bound == qlt_bound::OPEN) ? a : b;
}

// The upper endpoint that is further left (for intersection upper)
qlt_endpoint qlt_hi_min(const qlt_endpoint& a, const qlt_endpoint& b) {
	if (a.val != b.val) return (a.val < b.val) ? a : b;
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
// For symbolic endpoints the ordering is unknown, so assume non-empty (conservative).
bool qlt_piece_empty(const qlt_piece& p) {
	if (p.lo.val.is_sym() || p.hi.val.is_sym()) {
		// [c, c] with same sym name is a valid singleton; anything else is kept
		if (p.lo.val == p.hi.val)
			return !(p.lo.bound == qlt_bound::CLOSED && p.hi.bound == qlt_bound::CLOSED);
		return false; // different symbolic endpoints: assume non-empty
	}
	if (p.lo.val > p.hi.val) return true;
	if (p.lo.val == p.hi.val) {
		if (p.lo.val.is_pos_inf() || p.lo.val.is_neg_inf()) return true;
		return !(p.lo.bound == qlt_bound::CLOSED && p.hi.bound == qlt_bound::CLOSED);
	}
	return false;
}

// Do two intervals overlap (share at least one point)?
// If any endpoint is a named constant, conservatively return false (ordering unknown).
bool qlt_pieces_overlap(const qlt_piece& a, const qlt_piece& b) {
	if (a.lo.val.is_sym() || a.hi.val.is_sym() ||
	    b.lo.val.is_sym() || b.hi.val.is_sym()) return false;
	auto hi_above_lo = [](const qlt_endpoint& hi, const qlt_endpoint& lo) -> bool {
		if (hi.val < lo.val) return false;
		if (hi.val > lo.val) return true;
		if (hi.val.is_pos_inf() || hi.val.is_neg_inf()) return false;
		return hi.bound == qlt_bound::CLOSED && lo.bound == qlt_bound::CLOSED;
	};
	return hi_above_lo(a.hi, b.lo) && hi_above_lo(b.hi, a.lo);
}

// Are two intervals "adjacent" (touching but not overlapping)?
// If any endpoint is symbolic, conservatively return false.
bool qlt_pieces_adjacent(const qlt_piece& a, const qlt_piece& b) {
	if (a.lo.val.is_sym() || a.hi.val.is_sym() ||
	    b.lo.val.is_sym() || b.hi.val.is_sym()) return false;
	if (a.hi.val != b.lo.val) return false;
	if (a.hi.val.is_pos_inf() || a.hi.val.is_neg_inf()) return false;
	return (a.hi.bound == qlt_bound::OPEN && b.lo.bound == qlt_bound::CLOSED)
		|| (a.hi.bound == qlt_bound::CLOSED && b.lo.bound == qlt_bound::OPEN)
		|| (a.hi.bound == qlt_bound::CLOSED && b.lo.bound == qlt_bound::CLOSED);
}

// Merge two overlapping or adjacent intervals
qlt_piece qlt_merge(const qlt_piece& a, const qlt_piece& b) {
	// lower bound = min(a.lo, b.lo)
	qlt_endpoint lo, hi;
	if (a.lo.val < b.lo.val) lo = a.lo;
	else if (b.lo.val < a.lo.val) lo = b.lo;
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
	std::vector<qlt_piece> result;
	size_t i = 0, j = 0;
	while (i < pieces.size() && j < o.pieces.size()) {
		auto lo = qlt_lo_max(pieces[i].lo, o.pieces[j].lo);
		auto hi = qlt_hi_min(pieces[i].hi, o.pieces[j].hi);
		qlt_piece p{lo, hi};
		if (!qlt_piece_empty(p)) result.push_back(p);
		// advance piece that ends first
		if (qlt_hi_less(pieces[i].hi, o.pieces[j].hi)) ++i;
		else if (qlt_hi_less(o.pieces[j].hi, pieces[i].hi)) ++j;
		else { ++i; ++j; }
	}
	return qlt{std::move(result)};
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
	// sort by lower bound
	std::sort(ps.begin(), ps.end(), [](const qlt_piece& a, const qlt_piece& b) {
		if (a.lo.val != b.lo.val) return a.lo.val < b.lo.val;
		// CLOSED lower bound comes first (further left)
		return (int)a.lo.bound > (int)b.lo.bound; // CLOSED=1 > OPEN=0
	});
	std::vector<qlt_piece> norm;
	norm.push_back(ps[0]);
	for (size_t k = 1; k < ps.size(); ++k) {
		auto& last = norm.back();
		if (qlt_pieces_overlap(last, ps[k]) || qlt_pieces_adjacent(last, ps[k])) {
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
