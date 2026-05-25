// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <charconv>
#include <cmath>
#include <iomanip>
#include <limits>
#include <ostream>
#include <sstream>
#include <vector>

#include "qint.h"

namespace idni::tau_lang {

// =============================================================================
// qint_detail — endpoint parsing and formatting
// =============================================================================

namespace qint_detail {

bool parse_endpoint(const std::string& s, double& out) {
	std::string t = s;
	auto trim = [](std::string& x) {
		x.erase(0, x.find_first_not_of(" \t\n\r"));
		auto l = x.find_last_not_of(" \t\n\r");
		if (l != std::string::npos) x = x.substr(0, l + 1);
	};
	trim(t);
	if (t.empty()) return false;

	if (t == "+inf" || t == "inf" || t == "+infinity" || t == "infinity")
		{ out = POS_INF; return true; }
	if (t == "-inf" || t == "-infinity")
		{ out = NEG_INF; return true; }

	// fraction p/q — accepts all rationals (not just dyadic)
	auto slash = t.find('/');
	if (slash != std::string::npos) {
		std::string ps = t.substr(0, slash), qs = t.substr(slash + 1);
		trim(ps); trim(qs);
		try {
			size_t pp, qp;
			double p = std::stod(ps, &pp), q = std::stod(qs, &qp);
			if (pp != ps.size() || qp != qs.size()) return false;
			if (std::fpclassify(q) == FP_ZERO) return false;
			out = p / q;
			return true;
		} catch (...) { return false; }
	}

	// decimal or integer
	try {
		size_t pos;
		out = std::stod(t, &pos);
		while (pos < t.size() && (t[pos] == ' ' || t[pos] == '\t')) ++pos;
		return pos == t.size();
	} catch (...) { return false; }
}

std::string endpoint_to_string(double v) {
	if (std::isinf(v)) return v > 0 ? "+inf" : "-inf";
	std::ostringstream os;
	os << std::setprecision(17) << v;
	return os.str();
}

} // namespace qint_detail

// =============================================================================
// qint — member function implementations
// =============================================================================

qint qint::bottom() { return {}; }

qint qint::top() {
	return qint{{ {qint_detail::NEG_INF, qint_detail::POS_INF} }};
}

bool qint::is_full() const noexcept {
	if (intervals.size() != 1) return false;
	auto it = intervals.begin();
	return std::isinf(it->first)  && it->first  < 0
	    && std::isinf(it->second) && it->second > 0;
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
	std::map<double,double> merged = intervals;
	for (auto& [lo, hi] : o.intervals) {
		auto it = merged.find(lo);
		if (it != merged.end()) it->second = std::max(it->second, hi);
		else merged[lo] = hi;
	}
	return normalize_map(std::move(merged));
}

qint qint::operator&(const qint& o) const {
	std::map<double,double> result;
	auto i = intervals.begin(), j = o.intervals.begin();
	while (i != intervals.end() && j != o.intervals.end()) {
		double lo = std::max(i->first,  j->first);
		double hi = std::min(i->second, j->second);
		if (lo < hi) result[lo] = hi;
		if      (i->second < j->second) ++i;
		else if (j->second < i->second) ++j;
		else { ++i; ++j; }
	}
	return qint{std::move(result)};
}

qint qint::operator~() const {
	if (intervals.empty()) return top();
	std::map<double,double> result;
	double cur = qint_detail::NEG_INF;
	for (auto& [lo, hi] : intervals) {
		if (cur < lo) result[cur] = lo;
		cur = hi;
	}
	if (cur < qint_detail::POS_INF) result[cur] = qint_detail::POS_INF;
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

qint qint::normalize_map(std::map<double,double> m) {
	if (m.empty()) return {};
	std::map<double,double> result;
	double cur_lo = m.begin()->first, cur_hi = m.begin()->second;
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
	auto& [lo, hi] = *x.intervals.begin();
	using namespace qint_detail;
	if (std::isinf(lo) && std::isinf(hi)) return qint{{ {NEG_INF, 0.0} }};
	if (std::isinf(lo))                   return qint{{ {NEG_INF, hi - 1.0} }};
	if (std::isinf(hi))                   return qint{{ {lo, lo + 1.0} }};
	double mid = lo / 2.0 + hi / 2.0;
	if (mid <= lo || mid >= hi) return qint{{ {lo, hi} }}; // degenerate
	return qint{{ {lo, mid} }};
}

qint qint_splitter_one() {
	return qint{{ {0.0, 0.5} }};
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

	double lo, hi;
	if (!qint_detail::parse_endpoint(endpoints[0], lo) ||
	    !qint_detail::parse_endpoint(endpoints[1], hi))
		return std::nullopt;

	if (!(lo < hi)) return std::nullopt;

	return qint{{ {lo, hi} }};
}

std::optional<qint> qint_eval_parse_tree(
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

		double lo = static_cast<double>(val);
		double hi = static_cast<double>(val + 1);
		return qint{{ {lo, hi} }};
	}

	case type::qint_single: {
		auto children = (n | tt::children)();
		if (children.empty()) return std::nullopt;
		return qint_eval_interval(children[0]);
	}

	case type::qint_union: {
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

} // namespace idni::tau_lang

// =============================================================================
// std::hash<qint> specialization
// =============================================================================

size_t std::hash<idni::tau_lang::qint>::operator()(
	const idni::tau_lang::qint& d) const noexcept
{
	size_t h = 0;
	for (auto& [lo, hi] : d.intervals) {
		h ^= std::hash<double>{}(lo) * 2654435761ULL;
		h ^= std::hash<double>{}(hi) * 2246822519ULL;
	}
	return h;
}
