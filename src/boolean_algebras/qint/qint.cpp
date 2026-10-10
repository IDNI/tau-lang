// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <limits>
#include <ostream>
#include <vector>

#include "qint.h"

namespace idni::tau_lang {

// =============================================================================
// qint_rational — exact endpoints
// =============================================================================

// Pedantic-clean 128-bit alias: products of two 64-bit parts are exact in it.
#if defined(_MSC_VER)
#	include <__msvc_int128.hpp>
using int128_t_ = std::_Signed128;
#else
__extension__ typedef __int128 int128_t_;
#endif

namespace {

// |v|; v must not be the minimum int128.
int128_t_ abs128(int128_t_ v) { return v < 0 ? -v : v; }

// gcd(|a|, |b|), or 1 when both are 0, so it is always a safe divisor.
int128_t_ gcd128(int128_t_ a, int128_t_ b) {
	a = abs128(a), b = abs128(b);
	while (b) { int128_t_ t = a % b; a = b; b = t; }
	return a ? a : 1;
}

// True when v fits a long long.
bool fits(int128_t_ v) {
	return v >= std::numeric_limits<long long>::min()
		&& v <= std::numeric_limits<long long>::max();
}

// The exact rational num/den, or nullopt when den is 0 or the reduced value
// does not fit a 64-bit numerator and denominator.
std::optional<qint_rational> make_rational(int128_t_ num, int128_t_ den) {
	if (den == 0) return std::nullopt;
	if (den < 0) num = -num, den = -den;
	int128_t_ g = gcd128(num, den);
	num /= g, den /= g;
	if (!fits(num) || !fits(den)) return std::nullopt;
	qint_rational r;
	r.p = (long long) num, r.q = (long long) den;
	return r;
}

// a + b for finite a and an integer b, exactly.
std::optional<qint_rational> add_int(const qint_rational& a, long long b) {
	return make_rational((int128_t_) a.p + (int128_t_) b * a.q, a.q);
}

// The midpoint of two finite rationals, exactly.
std::optional<qint_rational> midpoint(const qint_rational& a,
	const qint_rational& b)
{
	// The reduced denominators are at most 2^63, so the doubled product of
	// two of them could overflow; reduce the sum over lcm(a.q, b.q) instead.
	int128_t_ g = gcd128(a.q, b.q);
	int128_t_ l = (int128_t_) a.q / g * b.q;
	int128_t_ num = (int128_t_) a.p * (l / a.q) + (int128_t_) b.p * (l / b.q);
	if (num % 2 == 0) return make_rational(num / 2, l);
	if (l > std::numeric_limits<int128_t_>::max() / 2) return std::nullopt;
	return make_rational(num, l * 2);
}

} // namespace

std::optional<qint_rational> qint_rational::fraction(long long num,
	long long den)
{
	return make_rational(num, den);
}

std::strong_ordering qint_rational::operator<=>(
	const qint_rational& o) const noexcept
{
	if (is_inf() || o.is_inf()) {
		// -inf < finite < +inf; an infinity's p is its sign
		const int a = is_inf() ? (p > 0 ? 1 : -1) : 0;
		const int b = o.is_inf() ? (o.p > 0 ? 1 : -1) : 0;
		return a <=> b;
	}
	return (int128_t_) p * o.q <=> (int128_t_) o.p * q;
}

// =============================================================================
// qint_detail — endpoint parsing and formatting
// =============================================================================

namespace qint_detail {

namespace {

// A decimal number with an optional sign, fraction and exponent, exactly:
// "12", "-1.5", ".5", "5.", "2e-3", "1.25E+2".
std::optional<qint_rational> parse_decimal(const std::string& t) {
	size_t i = 0;
	bool negative = false;
	if (i < t.size() && (t[i] == '+' || t[i] == '-'))
		negative = t[i++] == '-';
	int128_t_ mant = 0;
	int scale = 0; // value = mant * 10^(exp - scale)
	bool digits = false, dot = false;
	const int128_t_ cap = (int128_t_) 1 << 100;
	for (; i < t.size(); ++i) {
		const char c = t[i];
		if (c == '.') {
			if (dot) return std::nullopt;
			dot = true;
		} else if (c >= '0' && c <= '9') {
			digits = true;
			mant = mant * 10 + (c - '0');
			if (mant > cap) return std::nullopt;
			if (dot) ++scale;
		} else break;
	}
	if (!digits) return std::nullopt;
	long long exp = 0;
	if (i < t.size() && (t[i] == 'e' || t[i] == 'E')) {
		++i;
		bool eneg = false;
		if (i < t.size() && (t[i] == '+' || t[i] == '-'))
			eneg = t[i++] == '-';
		if (i == t.size()) return std::nullopt;
		for (; i < t.size(); ++i) {
			if (t[i] < '0' || t[i] > '9') return std::nullopt;
			exp = exp * 10 + (t[i] - '0');
			if (exp > 40) return std::nullopt;
		}
		if (eneg) exp = -exp;
	}
	if (i != t.size()) return std::nullopt;
	long long shift = exp - scale;
	if (shift > 38 || shift < -38) return std::nullopt;
	int128_t_ num = negative ? -mant : mant, den = 1;
	for (; shift > 0; --shift) {
		if (abs128(num) > cap) return std::nullopt;
		num *= 10;
	}
	for (; shift < 0; ++shift) den *= 10;
	return make_rational(num, den);
}

} // namespace

bool parse_endpoint(const std::string& s, qint_rational& out) {
	std::string t = s;
	auto trim = [](std::string& x) {
		x.erase(0, x.find_first_not_of(" \t\n\r"));
		auto l = x.find_last_not_of(" \t\n\r");
		if (l != std::string::npos) x = x.substr(0, l + 1);
	};
	trim(t);
	if (t.empty()) return false;

	if (t == "+inf" || t == "inf" || t == "+infinity" || t == "infinity")
		{ out = qint_rational::pos_inf(); return true; }
	if (t == "-inf" || t == "-infinity")
		{ out = qint_rational::neg_inf(); return true; }

	auto slash = t.find('/');
	if (slash == std::string::npos) {
		auto r = parse_decimal(t);
		if (!r) return false;
		out = *r;
		return true;
	}
	std::string ps = t.substr(0, slash), qs = t.substr(slash + 1);
	trim(ps); trim(qs);
	auto a = parse_decimal(ps), b = parse_decimal(qs);
	if (!a || !b || b->p == 0) return false;
	auto r = make_rational((int128_t_) a->p * b->q, (int128_t_) a->q * b->p);
	if (!r) return false;
	out = *r;
	return true;
}

std::string endpoint_to_string(const qint_rational& v) {
	if (v.is_inf()) return v.p > 0 ? "+inf" : "-inf";
	if (v.q == 1) return std::to_string(v.p);
	// A denominator of the form 2^a 5^b terminates in decimal.
	long long d = v.q;
	int twos = 0, fives = 0;
	while (d % 2 == 0) d /= 2, ++twos;
	while (d % 5 == 0) d /= 5, ++fives;
	if (d != 1) return std::to_string(v.p) + "/" + std::to_string(v.q);
	// p / (2^twos 5^fives) == p 2^(digits-twos) 5^(digits-fives) / 10^digits
	const int digits = std::max(twos, fives);
	int128_t_ scaled = (int128_t_) v.p;
	for (int i = twos; i < digits; ++i) scaled *= 2;
	for (int i = fives; i < digits; ++i) scaled *= 5;
	// scaled / 10^digits == v; print it with the point placed.
	const bool negative = scaled < 0;
	int128_t_ mag = abs128(scaled);
	std::string s;
	while (mag > 0) { s.insert(s.begin(), char('0' + (int) (mag % 10))); mag /= 10; }
	while ((int) s.size() <= digits) s.insert(s.begin(), '0');
	s.insert(s.end() - digits, '.');
	return negative ? "-" + s : s;
}

} // namespace qint_detail

// =============================================================================
// qint — member function implementations
// =============================================================================

qint qint::bottom() { return {}; }

qint qint::top() {
	return qint{{ {qint_rational::neg_inf(), qint_rational::pos_inf()} }};
}

bool qint::is_full() const noexcept {
	if (intervals.size() != 1) return false;
	auto it = intervals.begin();
	return it->first.is_neg_inf() && it->second.is_pos_inf();
}

bool qint::operator==(const qint& o) const noexcept {
	return intervals == o.intervals;
}
bool qint::operator!=(const qint& o) const noexcept { return !(*this == o); }
bool qint::operator==(bool b)        const noexcept { return b ? is_full() : is_empty(); }
bool qint::operator!=(bool b)        const noexcept { return !(*this == b); }

bool qint::operator<(const qint& o) const noexcept {
	return intervals < o.intervals;
}

std::strong_ordering qint::operator<=>(const qint& o) const noexcept {
	if (*this == o) return std::strong_ordering::equal;
	return *this < o ? std::strong_ordering::less : std::strong_ordering::greater;
}

qint qint::operator|(const qint& o) const {
	auto merged = intervals;
	for (auto& [lo, hi] : o.intervals) {
		auto it = merged.find(lo);
		if (it != merged.end()) it->second = std::max(it->second, hi);
		else merged[lo] = hi;
	}
	return normalize_map(std::move(merged));
}

qint qint::operator&(const qint& o) const {
	std::map<qint_rational, qint_rational> result;
	auto i = intervals.begin(), j = o.intervals.begin();
	while (i != intervals.end() && j != o.intervals.end()) {
		qint_rational lo = std::max(i->first,  j->first);
		qint_rational hi = std::min(i->second, j->second);
		if (lo < hi) result[lo] = hi;
		if      (i->second < j->second) ++i;
		else if (j->second < i->second) ++j;
		else { ++i; ++j; }
	}
	return qint{std::move(result)};
}

qint qint::operator~() const {
	if (intervals.empty()) return top();
	std::map<qint_rational, qint_rational> result;
	qint_rational cur = qint_rational::neg_inf();
	for (auto& [lo, hi] : intervals) {
		if (cur < lo) result[cur] = lo;
		cur = hi;
	}
	if (!cur.is_pos_inf()) result[cur] = qint_rational::pos_inf();
	return qint{std::move(result)};
}

qint qint::operator^(const qint& o) const {
	return (*this | o) & ~(*this & o);
}

std::string qint::to_string() const {
	if (intervals.empty()) return "bot";
	if (is_full()) return "top";
	std::string s;
	bool first = true;
	for (auto& [lo, hi] : intervals) {
		if (!first) s += " | ";
		first = false;
		s += "[" + qint_detail::endpoint_to_string(lo)
		   + ", " + qint_detail::endpoint_to_string(hi) + ")";
	}
	return s;
}

qint qint::normalize_map(std::map<qint_rational, qint_rational> m) {
	if (m.empty()) return {};
	std::map<qint_rational, qint_rational> result;
	qint_rational cur_lo = m.begin()->first, cur_hi = m.begin()->second;
	for (auto it = std::next(m.begin()); it != m.end(); ++it) {
		if (it->first <= cur_hi) { cur_hi = std::max(cur_hi, it->second); }
		else { result[cur_lo] = cur_hi; cur_lo = it->first; cur_hi = it->second; }
	}
	result[cur_lo] = cur_hi;
	return qint{std::move(result)};
}

// =============================================================================
// Stream output and free functions
// =============================================================================

std::ostream& operator<<(std::ostream& os, const qint& d) {
	return os << d.to_string();
}

bool is_qint_zero(const qint& x) { return x.is_empty(); }
bool is_qint_one (const qint& x) { return x.is_full(); }
qint normalize_qint(const qint& x) { return x; }
tref simplify_qint_symbol(tref sym) { return sym; }
tref simplify_qint_term(tref t) { return t; }

qint qint_splitter(const qint& x, splitter_type /*st*/) {
	if (x.is_empty()) return qint::bottom();
	const auto& [lo, hi] = *x.intervals.begin();
	if (lo.is_inf() && hi.is_inf()) return qint{{ {lo, qint_rational(0)} }};
	// A cut that does not fit leaves no smaller piece to return: the element
	// comes back unchanged, as the contract above allows.
	if (lo.is_inf()) {
		auto cut = add_int(hi, -1);
		if (!cut) return qint{{ {lo, hi} }};
		return qint{{ {lo, *cut} }};
	}
	if (hi.is_inf()) {
		auto cut = add_int(lo, 1);
		if (!cut) return qint{{ {lo, hi} }};
		return qint{{ {lo, *cut} }};
	}
	auto mid = midpoint(lo, hi);
	if (!mid) return qint{{ {lo, hi} }};
	return qint{{ {lo, *mid} }};
}

qint qint_splitter_one() {
	return qint{{ {qint_rational(0), *make_rational(1, 2)} }};
}

// =============================================================================
// Parse tree evaluation
// =============================================================================

std::optional<qint> qint_eval_interval(
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
	if (!qint_detail::parse_endpoint(endpoints[0], lo) ||
	    !qint_detail::parse_endpoint(endpoints[1], hi))
		return std::nullopt;

	if (!(lo < hi)) return std::nullopt;

	return qint{{ {lo, hi} }};
}

result<qint> qint_eval_parse_tree(
	const qint_parser::tree::traverser& t)
{
	using tt = qint_parser::tree::traverser;
	using type = qint_parser::nonterminal;

	result<qint> r;
	auto n  = t | tt::only_child;
	auto nt = n | tt::nonterminal;

	switch (nt) {
	case type::qint_top:
		return r.with_value(qint::top());

	case type::qint_bot:
		return r.with_value(qint::bottom());

	case type::qint_interval: {
		auto children = (n | tt::children)();
		if (children.empty()) return r;
		auto interval = qint_eval_interval(children[0]);
		if (!interval) return r;
		return r.with_value(*interval);
	}

	case type::qint_union: {
		// qint_union children = [interval, qint]
		auto children = (n | tt::children)();
		if (children.size() < 2) return r;

		auto left = qint_eval_interval(children[0]);
		if (!left) return r;

		TAU_TRY(auto right, qint_eval_parse_tree(children[1]));

		return r.with_value(*left | right);
	}

	default:
		return r.with_error(code::internal_error,
			"Unknown qint node type",
			{{label::value, qint_parser::instance().name(nt)}});
	}
}

std::uint64_t qint_hash(const qint& d) noexcept {
	std::uint64_t seed = d.intervals.size();
	for (auto& [lo, hi] : d.intervals)
		idni::hash_combine(seed, lo.p, lo.q, hi.p, hi.q);
	return seed;
}

} // namespace idni::tau_lang

// =============================================================================
// std::hash<qint> specialization
// =============================================================================

size_t std::hash<idni::tau_lang::qint>::operator()(
	const idni::tau_lang::qint& d) const noexcept
{
	return static_cast<size_t>(idni::tau_lang::qint_hash(d));
}
