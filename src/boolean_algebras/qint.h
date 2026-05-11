// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT_H__

#include <climits>
#include <compare>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <functional>
#include <sstream>
#include <charconv>

#include "tau_tree.h"
#include "splitter_types.h"
#include "../parser/qint_parser.generated.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// qint — atomless Boolean algebra of rational intervals [a, b) (left-closed,
// right-open) on the extended real line.
//
// Endpoints are dyadic rationals represented as fixed-point numbers with
// scale 2^SCALE, i.e. the real value is raw / 2^SCALE.  Literals accept both
// fractional (1/4) and decimal (0.25) syntax.  Non-dyadic rationals (whose
// denominator after simplification is not a power of 2, e.g. 1/3, 0.1) are
// rejected by the parser.
//
// Special sentinels: LLONG_MAX represents +inf, LLONG_MIN represents -inf.
// -----------------------------------------------------------------------------

struct qint_rational {
	static constexpr int SCALE = 30;
	static constexpr long long POS_INF = LLONG_MAX;
	static constexpr long long NEG_INF = LLONG_MIN;

	long long raw = 0; // value = raw / 2^SCALE (unless sentinel)

	qint_rational() = default;
	explicit constexpr qint_rational(long long r) noexcept : raw(r) {}

	bool is_pos_inf() const noexcept { return raw == POS_INF; }
	bool is_neg_inf() const noexcept { return raw == NEG_INF; }
	bool is_finite() const noexcept { return raw != POS_INF && raw != NEG_INF; }

	bool operator==(const qint_rational& o) const noexcept { return raw == o.raw; }
	bool operator!=(const qint_rational& o) const noexcept { return raw != o.raw; }
	bool operator<(const qint_rational& o) const noexcept { return raw < o.raw; }
	bool operator<=(const qint_rational& o) const noexcept { return raw <= o.raw; }
	bool operator>(const qint_rational& o) const noexcept { return raw > o.raw; }
	bool operator>=(const qint_rational& o) const noexcept { return raw >= o.raw; }

	// midpoint between this and o (both must be finite)
	qint_rational midpoint(const qint_rational& o) const noexcept {
		// (a + b) / 2 avoiding overflow
		long long a = raw, b = o.raw;
		return qint_rational{ a / 2 + b / 2 + (a % 2 + b % 2) / 2 };
	}

	// Parse a dyadic rational from a string.
	// Accepts integers, fractions p/q (q must be power of 2), or "-inf"/"+inf"/
	// Returns false on parse error.
	static bool parse(const std::string& s, qint_rational& out) {
		std::string t = s;
		// trim whitespace
		t.erase(0, t.find_first_not_of(" \t\n\r"));
		auto last = t.find_last_not_of(" \t\n\r");
		if (last != std::string::npos) t = t.substr(0, last + 1);

		if (t == "+inf" || t == "inf" || t == "+infinity") {
			out = qint_rational{ POS_INF }; return true;
		}
		if (t == "-inf" || t == "-infinity") {
			out = qint_rational{ NEG_INF }; return true;
		}

		// check for fraction p/q
		auto slash = t.find('/');
		if (slash != std::string::npos) {
			long long p = 0, q = 0;
			auto pstr = t.substr(0, slash);
			auto qstr = t.substr(slash + 1);
			// trim
			pstr.erase(0, pstr.find_first_not_of(" \t"));
			qstr.erase(0, qstr.find_first_not_of(" \t"));
			auto last_p = pstr.find_last_not_of(" \t");
			if (last_p != std::string::npos) pstr = pstr.substr(0, last_p + 1);
			auto last_q = qstr.find_last_not_of(" \t");
			if (last_q != std::string::npos) qstr = qstr.substr(0, last_q + 1);

			auto [p_ptr, p_ec] = std::from_chars(pstr.data(), pstr.data() + pstr.size(), p);
			auto [q_ptr, q_ec] = std::from_chars(qstr.data(), qstr.data() + qstr.size(), q);
			if (p_ec != std::errc{} || q_ec != std::errc{}) return false;
			if (q <= 0) return false;
			// check q is a power of 2
			if ((q & (q - 1)) != 0) return false;
			// compute p / q * 2^SCALE = p * 2^SCALE / q
			// q is a power of 2, say q = 2^k, so result = p * 2^(SCALE-k)
			int k = 0;
			long long tmp = q;
			while (tmp > 1) { tmp >>= 1; k++; }
			int shift = SCALE - k;
			if (shift >= 0)
				out = qint_rational{ p << shift };
			else
				out = qint_rational{ p >> (-shift) };
			return true;
		}

		// decimal: "0.5", "-1.25", ".125", "3." (anything with a dot).
		// Converts to a fraction with denom = 10^|frac|; if after simplification
		// the denom is not a power of 2 the value is not a dyadic rational and
		// parsing fails.
		auto dot = t.find('.');
		if (dot != std::string::npos) {
			auto int_str  = t.substr(0, dot);
			auto frac_str = t.substr(dot + 1);
			if (frac_str.find('.') != std::string::npos) return false;
			if (frac_str.find('/') != std::string::npos) return false;
			bool negative = false;
			if (!int_str.empty() && (int_str[0] == '+' || int_str[0] == '-')) {
				negative = (int_str[0] == '-');
				int_str = int_str.substr(1);
			}
			auto is_digits = [](const std::string& s) {
				for (char c : s) if (c < '0' || c > '9') return false;
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
			long long denom = 1;
			if (!frac_str.empty()) {
				auto [fp, fe] = std::from_chars(frac_str.data(),
				                  frac_str.data() + frac_str.size(), fpart);
				if (fe != std::errc{}) return false;
				for (size_t i = 0; i < frac_str.size(); ++i) {
					if (denom > 1'000'000'000'000LL) return false;
					denom *= 10;
				}
			}
			long long num = ipart * denom + fpart;
			if (negative) num = -num;
			// Simplify by dividing both num and denom by their gcd.
			auto gcd_pos = [](long long a, long long b) -> long long {
				unsigned long long ua = a < 0 ? 0ULL - (unsigned long long)a
				                               : (unsigned long long)a;
				unsigned long long ub = (unsigned long long)b;
				while (ub) { ua %= ub; std::swap(ua, ub); }
				return (long long)(ua ? ua : 1ULL);
			};
			long long g = gcd_pos(num, denom);
			if (g > 0) { num /= g; denom /= g; }
			// denom must be a power of 2 for a dyadic rational.
			if (denom <= 0 || (denom & (denom - 1)) != 0) return false;
			int k = 0;
			long long tmp = denom;
			while (tmp > 1) { tmp >>= 1; ++k; }
			int shift = SCALE - k;
			if (shift >= 0) out = qint_rational{ num << shift };
			else            out = qint_rational{ num >> (-shift) };
			return true;
		}

		// integer
		long long val = 0;
		auto [ptr, ec] = std::from_chars(t.data(), t.data() + t.size(), val);
		if (ec != std::errc{}) return false;
		// check no remaining non-space chars
		const char* tend = t.data() + t.size();
		while (ptr < tend && (*ptr == ' ' || *ptr == '\t')) ++ptr;
		if (ptr < tend) return false;
		out = qint_rational{ val << SCALE };
		return true;
	}

	std::string to_string() const {
		if (raw == POS_INF) return "+inf";
		if (raw == NEG_INF) return "-inf";
		// find lowest bit
		long long r = raw;
		if (r == 0) return "0";
		bool neg = r < 0;
		if (neg) r = -r;
		int trailing = 0;
		while (trailing < SCALE && (r & 1) == 0) { r >>= 1; trailing++; }
		// value = r / 2^(SCALE - trailing)
		int denom_pow = SCALE - trailing;
		if (denom_pow == 0) {
			return (neg ? "-" : "") + std::to_string(r);
		}
		long long denom = 1LL << denom_pow;
		return (neg ? "-" : "") + std::to_string(r) + "/" + std::to_string(denom);
	}
};

// -----------------------------------------------------------------------------
// qint: finite union of disjoint half-open intervals [lo, hi)
// Intervals are sorted by lo endpoint. The set is normalised (no adjacent or
// overlapping pieces).
// -----------------------------------------------------------------------------

struct qint {
	struct piece {
		qint_rational lo, hi;
		bool operator==(const piece& o) const noexcept {
			return lo == o.lo && hi == o.hi;
		}
	};

	std::vector<piece> pieces; // sorted, disjoint, normalised

	// --- factories ---
	static qint bottom() { return {}; }
	static qint top() {
		return qint{{ piece{ qint_rational{qint_rational::NEG_INF},
		                          qint_rational{qint_rational::POS_INF} } }};
	}

	bool is_empty() const noexcept { return pieces.empty(); }
	bool is_full() const noexcept {
		return pieces.size() == 1
			&& pieces[0].lo.raw == qint_rational::NEG_INF
			&& pieces[0].hi.raw == qint_rational::POS_INF;
	}

	bool operator==(const qint& o) const noexcept {
		return pieces == o.pieces;
	}
	bool operator!=(const qint& o) const noexcept {
		return !(*this == o);
	}
	bool operator==(bool b) const noexcept {
		return b ? is_full() : is_empty();
	}
	bool operator!=(bool b) const noexcept { return !(*this == b); }

	// Ordering for use in maps (lexicographic on intervals)
	bool operator<(const qint& o) const noexcept {
		if (pieces.size() != o.pieces.size())
			return pieces.size() < o.pieces.size();
		for (size_t i = 0; i < pieces.size(); ++i) {
			if (pieces[i].lo != o.pieces[i].lo)
				return pieces[i].lo < o.pieces[i].lo;
			if (pieces[i].hi != o.pieces[i].hi)
				return pieces[i].hi < o.pieces[i].hi;
		}
		return false;
	}

	// Union of two normalised sets
	qint operator|(const qint& o) const {
		// merge sorted piece lists, then normalise
		std::vector<piece> merged;
		merged.reserve(pieces.size() + o.pieces.size());
		for (auto& p : pieces) merged.push_back(p);
		for (auto& p : o.pieces) merged.push_back(p);
		return from_sorted_union(std::move(merged));
	}

	// Intersection
	qint operator&(const qint& o) const {
		std::vector<piece> result;
		size_t i = 0, j = 0;
		while (i < pieces.size() && j < o.pieces.size()) {
			auto lo = pieces[i].lo < o.pieces[j].lo
				? o.pieces[j].lo : pieces[i].lo;  // max
			auto hi = pieces[i].hi < o.pieces[j].hi
				? pieces[i].hi : o.pieces[j].hi;  // min
			if (lo < hi) result.push_back({lo, hi});
			// advance the piece that ends first
			if (pieces[i].hi < o.pieces[j].hi) ++i;
			else if (o.pieces[j].hi < pieces[i].hi) ++j;
			else { ++i; ++j; }
		}
		return qint{std::move(result)};
	}

	// Complement (wrt extended real line)
	qint operator~() const {
		if (pieces.empty()) return top();
		std::vector<piece> result;
		qint_rational cur{ qint_rational::NEG_INF };
		for (auto& p : pieces) {
			if (cur < p.lo)
				result.push_back({cur, p.lo});
			cur = p.hi;
		}
		if (cur.raw != qint_rational::POS_INF)
			result.push_back({cur, qint_rational{qint_rational::POS_INF}});
		return qint{std::move(result)};
	}

	// Symmetric difference
	qint operator^(const qint& o) const {
		return (*this | o) & ~(*this & o);
	}

	// three-way comparison (for ordered containers)
	auto operator<=>(const qint& o) const noexcept {
		if (*this == o) return std::strong_ordering::equal;
		return *this < o
			? std::strong_ordering::less
			: std::strong_ordering::greater;
	}

	std::string to_string() const {
		if (pieces.empty()) return "bot";
		if (is_full()) return "top";
		std::string s;
		for (size_t i = 0; i < pieces.size(); ++i) {
			if (i > 0) s += " | ";
			s += "[" + pieces[i].lo.to_string() + ", "
			         + pieces[i].hi.to_string() + ")";
		}
		return s;
	}

private:
	// Build normalised union from a (possibly unsorted, overlapping) list of pieces
	static qint from_sorted_union(std::vector<piece> ps) {
		if (ps.empty()) return {};
		std::sort(ps.begin(), ps.end(), [](const piece& a, const piece& b) {
			return a.lo < b.lo || (a.lo == b.lo && a.hi < b.hi);
		});
		std::vector<piece> norm;
		norm.push_back(ps[0]);
		for (size_t k = 1; k < ps.size(); ++k) {
			auto& last = norm.back();
			if (ps[k].lo <= last.hi) {
				// overlapping or adjacent — extend
				if (last.hi < ps[k].hi) last.hi = ps[k].hi;
			} else {
				norm.push_back(ps[k]);
			}
		}
		return qint{std::move(norm)};
	}
};

// --- stream output ---
inline std::ostream& operator<<(std::ostream& os, const qint& d) {
	return os << d.to_string();
}

// --- free functions expected by the dispatcher ---

inline bool is_qint_zero(const qint& x) { return x.is_empty(); }
inline bool is_qint_one(const qint& x) { return x.is_full(); }
inline qint normalize_qint(const qint& x) { return x; }
inline tref simplify_qint_symbol(tref sym) { return sym; }
inline tref simplify_qint_term(tref t) { return t; }

// Splitter: given a non-zero element x, return a non-zero element strictly
// contained in x. We take the first piece [lo, hi) and split at midpoint.
inline qint qint_splitter(const qint& x, splitter_type /*st*/) {
	if (x.is_empty()) return qint::bottom();
	const auto& p = x.pieces[0];
	qint_rational mid;
	if (p.lo.raw == qint_rational::NEG_INF && p.hi.raw == qint_rational::POS_INF) {
		// full piece — use [-1, 0)
		mid = qint_rational{ 0 };
		return qint{{ {qint_rational{-(1LL << qint_rational::SCALE)}, mid} }};
	} else if (p.lo.raw == qint_rational::NEG_INF) {
		mid = qint_rational{ p.hi.raw - 1 };
		if (mid >= p.hi) mid = qint_rational{ p.hi.raw - (1LL << (qint_rational::SCALE - 10)) };
		return qint{{ {p.lo, mid} }};
	} else if (p.hi.raw == qint_rational::POS_INF) {
		mid = qint_rational{ p.lo.raw + (1LL << (qint_rational::SCALE - 10)) };
		return qint{{ {p.lo, mid} }};
	} else {
		mid = p.lo.midpoint(p.hi);
		if (mid <= p.lo || mid >= p.hi) {
			// interval too thin to split — just return the piece itself
			return qint{{ p }};
		}
		return qint{{ {p.lo, mid} }};
	}
}

inline qint qint_splitter_one() {
	// A fixed non-zero, non-full element: [0, 1/2)
	return qint{{ {qint_rational{0},
	                    qint_rational{1LL << (qint_rational::SCALE - 1)}} }};
}

// --- parsing ---

// Helper: extract a qint piece from an interval parse tree node.
// interval has 2 endpoint children; endpoint subtrees contain the terminal chars.
inline std::optional<qint> qint_eval_interval(
	const qint_parser::tree::traverser& interval_node)
{
	using tt = qint_parser::tree::traverser;
	auto interval_children = (interval_node | tt::children)();

	std::vector<std::string> endpoints;
	for (auto& child : interval_children) {
		auto ep = child | tt::terminals;
		if (!ep.empty()) endpoints.push_back(ep);
	}

	if (endpoints.size() < 2) return std::nullopt;

	qint_rational lo, hi;
	if (!qint_rational::parse(endpoints[0], lo) ||
	    !qint_rational::parse(endpoints[1], hi))
		return std::nullopt; // non-dyadic or unparseable endpoint

	if (!(lo < hi)) return std::nullopt; // invalid bounds

	return qint{{ {lo, hi} }};
}

// Evaluate a parsed qint parse tree node to a qint value.
// Returns nullopt on semantic errors (invalid bounds, non-dyadic endpoints).
inline std::optional<qint> qint_eval_parse_tree(
	const qint_parser::tree::traverser& t)
{
	using tt = qint_parser::tree::traverser;
	using type = qint_parser::nonterminal;

	auto n  = t | tt::only_child;
	auto nt = n | tt::nonterminal;

	switch (nt) {
	case type::qint_top:
		return qint::top();

	case type::qint_bot:
		return qint::bottom();

	case type::qint_integer: {
		auto int_str = n | tt::terminals;
		long long val = 0;
		auto [ptr, ec] = std::from_chars(int_str.data(),
			int_str.data() + int_str.size(), val);
		if (ec != std::errc{}) return std::nullopt;

		if (val == 0) return qint::bottom();
		if (val == 1) return qint::top();

		qint_rational lo{ val * (1LL << qint_rational::SCALE) };
		qint_rational hi{ (val + 1) * (1LL << qint_rational::SCALE) };
		return qint{{ {lo, hi} }};
	}

	case type::qint_single: {
		// qint_single => interval
		auto children = (n | tt::children)();
		if (children.empty()) return std::nullopt;
		return qint_eval_interval(children[0]);
	}

	case type::qint_union: {
		// ( interval _ '|' _ qint ) :qint_union — after inlining __E_qint_0:
		// qint_union children = [interval, qint]
		auto children = (n | tt::children)();
		if (children.size() < 2) return std::nullopt;

		auto left = qint_eval_interval(children[0]);
		if (!left) return std::nullopt;

		auto right = qint_eval_parse_tree(children[1]);
		if (!right) return std::nullopt;

		return *left | *right;
	}

	default:
		LOG_ERROR << "Unknown qint node type\n";
		return std::nullopt;
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<qint> parse_qint_grammar(const std::string& src) {
	auto result = qint_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) return std::nullopt;

	auto t = qint_parser::tree::traverser(result.get_shaped_tree2())
		| qint_parser::qint;
	if (!t.has_value()) return std::nullopt;

	return qint_eval_parse_tree(t);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_qint(
	const std::string& src)
{
	std::string s = src;
	// trim
	s.erase(0, s.find_first_not_of(" \t\n\r"));
	auto last = s.find_last_not_of(" \t\n\r");
	if (last != std::string::npos) s = s.substr(0, last + 1);

	// strip outer braces if present
	if (!s.empty() && s.front() == '{' && s.back() == '}')
		s = s.substr(1, s.size() - 2);
	// trim again
	s.erase(0, s.find_first_not_of(" \t\n\r"));
	last = s.find_last_not_of(" \t\n\r");
	if (last != std::string::npos) s = s.substr(0, last + 1);

	// Parse using the qint grammar
	auto qval = parse_qint_grammar<BAs...>(s);
	if (!qval) return std::nullopt;

	return typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ *qval },
		qint_type<node<BAs...>>() };
}

} // namespace idni::tau_lang

// Hash specialization for qint
template<>
struct std::hash<idni::tau_lang::qint> {
	size_t operator()(const idni::tau_lang::qint& d) const noexcept {
		size_t h = 0;
		for (auto& p : d.pieces) {
			// combine raw values
			h ^= std::hash<long long>{}(p.lo.raw) * 2654435761ULL;
			h ^= std::hash<long long>{}(p.hi.raw) * 2246822519ULL;
		}
		return h;
	}
};
#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT_H__
