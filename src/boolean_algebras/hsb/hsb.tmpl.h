// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_TMPL_H__

#include "boolean_algebras/hsb/hsb.h"
#include "tau_diagnostics.h"

#include <sstream>

namespace idni::tau_lang {

// =============================================================================
// hsb_halfspace — method definitions
// =============================================================================

inline size_t hsb_halfspace::dimension() const noexcept { return w.size(); }

inline size_t hsb_halfspace::lex_leading_index() const noexcept {
	for (size_t i = 0; i < w.size(); ++i)
		if (!w[i].is_zero()) return i;
	return w.size();
}

inline int hsb_halfspace::lex_leading_sign() const noexcept {
	for (const auto& c : w)
		if (int sg = c.sign(); sg != 0) return sg;
	return 0;
}

inline bool hsb_halfspace::is_strict() const noexcept {
	return lex_leading_sign() == +1;
}

inline hsb_rational hsb_halfspace::eval(const std::vector<hsb_rational>& x) const {
	auto dim = std::min(w.size(), x.size());
	hsb_rational sum = b;
	for (size_t i = 0; i < dim; ++i) sum += w[i] * x[i];
	return sum;
}

inline bool hsb_halfspace::contains(const std::vector<hsb_rational>& x) const {
	int sg = eval(x).sign();
	return is_strict() ? (sg < 0) : (sg <= 0);
}

inline hsb_halfspace hsb_halfspace::negate() const {
	hsb_halfspace r;
	r.w.resize(w.size());
	for (size_t i = 0; i < w.size(); ++i) r.w[i] = -w[i];
	r.b = -b;
	return r;
}

inline bool hsb_halfspace::operator==(const hsb_halfspace& o) const noexcept {
	return w == o.w && b == o.b;
}

inline bool hsb_halfspace::operator!=(const hsb_halfspace& o) const noexcept {
	return !(*this == o);
}

inline hsb_halfspace hsb_halfspace::normalize() const {
	// Find the highest-indexed non-zero coefficient.
	size_t j = w.size();
	for (size_t k = w.size(); k > 0; --k)
		if (!w[k-1].is_zero()) { j = k - 1; break; }
	if (j == w.size()) return *this; // zero vector — nothing to do

	hsb_rational divisor = w[j].abs();
	hsb_halfspace r;
	r.w.resize(w.size());
	for (size_t k = 0; k < w.size(); ++k) r.w[k] = w[k] / divisor;
	r.b = b / divisor;
	return r;
}

inline bool hsb_halfspace::operator<(const hsb_halfspace& o) const {
	static const hsb_rational zero;
	size_t n = std::max(w.size(), o.w.size());
	for (size_t k = 0; k < n; ++k) {
		const auto& lv = k < w.size()   ? w[k]   : zero;
		const auto& rv = k < o.w.size() ? o.w[k] : zero;
		if (auto c = lv <=> rv; c != 0) return c < 0;
	}
	return b < o.b;
}

inline std::string hsb_halfspace::to_string() const {
	std::ostringstream os;
	bool first = true;
	for (size_t i = 0; i < w.size(); ++i) {
		int sg = w[i].sign();
		if (sg == 0) continue;
		if (!first && sg > 0) os << " + ";
		else if (sg < 0) os << (first ? "-" : " - ");
		hsb_rational aw = w[i].abs();
		if (aw != 1) os << aw.to_string() << "*";
		os << "x[" << i << "]";
		first = false;
	}
	if (first) os << "0";
	if (b.sign() > 0) os << " + " << b.to_string();
	else if (b.sign() < 0) os << " - " << b.abs().to_string();
	os << (is_strict() ? " < 0" : " <= 0");
	return os.str();
}

// =============================================================================
// hsb_halfspace_pool — method definitions
// =============================================================================

inline size_t hsb_halfspace_pool::insert(const hsb_halfspace& h) {
	auto it = index_.find(h);
	if (it != index_.end()) return it->second;
	// Index 0 is reserved: hsb_node uses data = 0 as the "no data"
	// sentinel, so real halfspaces must be 1-indexed. Slot 0 holds an
	// inert default-constructed entry that is never handed out.
	if (pool_.empty()) pool_.emplace_back();
	size_t idx = pool_.size();
	pool_.push_back(h);
	index_[h] = idx;
	return idx;
}

inline const hsb_halfspace& hsb_halfspace_pool::get(size_t idx) {
	static const hsb_halfspace empty{};
	if (idx >= pool_.size()) {
		DBG(assert(false));
		return empty;
	}
	return pool_[idx];
}

inline size_t hsb_halfspace_pool::complement_index(size_t idx) {
	return insert(get(idx).negate());
}

inline size_t hsb_halfspace_pool::size() {
	// Exclude the reserved sentinel entry at index 0.
	return pool_.empty() ? 0 : pool_.size() - 1;
}

// =============================================================================
// hsb — constructor, factory and operator definitions
// =============================================================================

inline hsb::hsb() : root(hsb_tree::geth(mk_bot())) {}
inline hsb::hsb(tref r)  : root(r ? hsb_tree::geth(r) : hsb_tree::geth(mk_bot())) {}
inline hsb::hsb(htref h) : root(h ? h : hsb_tree::geth(mk_bot())) {}

// ── Static node factories ────────────────────────────────────────────────────

inline tref hsb::mk_bot() {
	static const htref h = hsb_tree::geth(
		hsb_tree::get(make_hsb_node(hsb_parser::hsb_bot)));
	return h->get();
}

inline tref hsb::mk_top() {
	static const htref h = hsb_tree::geth(
		hsb_tree::get(make_hsb_node(hsb_parser::hsb_top)));
	return h->get();
}

inline tref hsb::mk_hs(const hsb_halfspace& h) {
	return mk_hs_by_index(hsb_halfspace_pool::insert(h));
}

inline tref hsb::mk_hs_by_index(size_t pool_idx) {
	return hsb_tree::get(make_hsb_node(hsb_parser::hsb_hs, pool_idx));
}

inline tref hsb::mk_and(tref l, tref r) {
	return hsb_tree::get(make_hsb_node(hsb_parser::hsb_and), l, r);
}

inline tref hsb::mk_or(tref l, tref r) {
	return hsb_tree::get(make_hsb_node(hsb_parser::hsb_or), l, r);
}

inline tref hsb::mk_not(tref inner) {
	return hsb_tree::get(make_hsb_node(hsb_parser::hsb_not), inner);
}

// ── Element factories ────────────────────────────────────────────────────────

inline hsb hsb::bottom() { return hsb(mk_bot()); }
inline hsb hsb::top()    { return hsb(mk_top()); }

inline hsb hsb::from_halfspace(const hsb_halfspace& h) {
	return hsb(mk_hs(h));
}

// ── Tree accessor helpers ────────────────────────────────────────────────────

inline hsb::kind hsb::root_kind() const noexcept {
	if (!root_ref()) return kind::bot;
	return static_cast<kind>(hsb_tree::get(root_ref()).value.nt);
}

inline const hsb_halfspace& hsb::root_halfspace() const {
	DBG(assert(root_kind() == kind::halfspace));
	return hsb_halfspace_pool::get(hsb_tree::get(root_ref()).value.data);
}

inline hsb hsb::lhs() const {
	return hsb(hsb_tree::get(root_ref()).first());
}

inline hsb hsb::rhs() const {
	return hsb(hsb_tree::get(root_ref()).second());
}

inline hsb hsb::inner() const {
	return hsb(hsb_tree::get(root_ref()).first());
}

// ── Complement check helper ───────────────────────────────────────────────────

namespace hsb_detail {
// True when a and b are syntactic complements: one is `not` of the other, or
// both are halfspace leaves whose pool indices complement each other. May
// insert the complement of a's halfspace into the pool.
inline bool is_complement_tref(tref a, tref b) {
	auto ka = static_cast<hsb::kind>(hsb_tree::get(a).value.nt);
	auto kb = static_cast<hsb::kind>(hsb_tree::get(b).value.nt);
	if (ka == hsb::kind::not_ && hsb_tree::get(a).first() == b) return true;
	if (kb == hsb::kind::not_ && hsb_tree::get(b).first() == a) return true;
	if (ka == hsb::kind::halfspace && kb == hsb::kind::halfspace) {
		size_t ia = hsb_tree::get(a).value.data;
		size_t ib = hsb_tree::get(b).value.data;
		return hsb_halfspace_pool::complement_index(ia) == ib;
	}
	return false;
}
} // namespace hsb_detail

// ── Boolean operations ────────────────────────────────────────────────────────

inline hsb hsb::operator&(const hsb& o) const {
	tref a = root_ref(), b = o.root_ref();
	auto ka = root_kind(), kb = o.root_kind();
	if (ka == kind::bot || kb == kind::bot) return bottom();
	if (ka == kind::top) return o;
	if (kb == kind::top) return *this;
	if (a == b) return *this;
	if (hsb_detail::is_complement_tref(a, b)) return bottom();
	return hsb(mk_and(a, b));
}

inline hsb hsb::operator|(const hsb& o) const {
	tref a = root_ref(), b = o.root_ref();
	auto ka = root_kind(), kb = o.root_kind();
	if (ka == kind::bot) return o;
	if (kb == kind::bot) return *this;
	if (ka == kind::top || kb == kind::top) return top();
	if (a == b) return *this;
	if (hsb_detail::is_complement_tref(a, b)) return top();
	return hsb(mk_or(a, b));
}

inline hsb hsb::operator~() const {
	tref n = root_ref();
	auto k = root_kind();
	if (k == kind::bot) return top();
	if (k == kind::top) return bottom();
	if (k == kind::not_) return hsb(hsb_tree::get(n).first());
	if (k == kind::halfspace) {
		size_t idx = hsb_tree::get(n).value.data;
		return hsb(mk_hs_by_index(hsb_halfspace_pool::complement_index(idx)));
	}
	return hsb(mk_not(n));
}

inline hsb hsb::operator^(const hsb& o) const {
	return (*this | o) & ~(*this & o);
}

inline bool hsb::operator==(const hsb& o) const noexcept {
	return root_ref() == o.root_ref();
}
inline bool hsb::operator!=(const hsb& o) const noexcept {
	return !(*this == o);
}

namespace hsb_detail {
// Content hash of w and b, in uint64_t on every platform.
inline std::uint64_t halfspace_hash(const hsb_halfspace& h) {
	std::uint64_t seed = h.w.size();
	for (const auto& x : h.w) idni::hash_combine(seed, x.hash());
	idni::hash_combine(seed, h.b.hash());
	return seed;
}

// The tree shares subtrees, so memo hashes each one once per call.
inline std::uint64_t tree_content_hash(tref n,
	std::unordered_map<tref, std::uint64_t>& memo)
{
	if (!n) return 0;
	if (auto it = memo.find(n); it != memo.end()) return it->second;
	const auto& t = hsb_tree::get(n);
	std::uint64_t seed = 0;
	idni::hash_combine(seed, static_cast<std::uint64_t>(t.value.nt));
	if (static_cast<hsb::kind>(t.value.nt) == hsb::kind::halfspace)
		idni::hash_combine(seed,
			halfspace_hash(hsb_halfspace_pool::get(t.value.data)));
	for (tref c : t.children())
		idni::hash_combine(seed, tree_content_hash(c, memo));
	return memo.emplace(n, seed).first->second;
}
} // namespace hsb_detail

inline std::uint64_t hsb::content_hash() const {
	if (content_hash_cache == 0) {
		std::unordered_map<tref, std::uint64_t> memo;
		content_hash_cache = hsb_detail::tree_content_hash(root_ref(), memo);
		// 0 marks "not computed"
		if (content_hash_cache == 0) content_hash_cache = 1;
	}
	return content_hash_cache;
}

inline bool hsb::operator==(bool b) const {
	auto k = root_kind();
	return b ? (k == kind::top) : (k == kind::bot);
}
inline bool hsb::operator!=(bool b) const { return !(*this == b); }

namespace hsb_detail {
// A total order on interned trees that agrees with tref equality: the node
// kind, then the halfspace values, then the children from left to right.
// The pool interns halfspaces by hsb_halfspace::operator<, so two leaves
// with different pool indices are never equivalent under it.
inline std::strong_ordering tree_order(tref a, tref b) {
	if (a == b) return std::strong_ordering::equal;
	if (!a || !b)
		return a ? std::strong_ordering::greater
			: std::strong_ordering::less;
	const auto& ta = hsb_tree::get(a);
	const auto& tb = hsb_tree::get(b);
	if (ta.value.nt != tb.value.nt)
		return static_cast<size_t>(ta.value.nt)
			<=> static_cast<size_t>(tb.value.nt);
	switch (static_cast<hsb::kind>(ta.value.nt)) {
	case hsb::kind::halfspace: {
		const auto& ha = hsb_halfspace_pool::get(ta.value.data);
		const auto& hb = hsb_halfspace_pool::get(tb.value.data);
		if (ha < hb) return std::strong_ordering::less;
		if (hb < ha) return std::strong_ordering::greater;
		return ta.value.data <=> tb.value.data;
	}
	case hsb::kind::not_:
		return tree_order(ta.first(), tb.first());
	case hsb::kind::and_:
	case hsb::kind::or_:
		if (auto c = tree_order(ta.first(), tb.first()); c != 0) return c;
		return tree_order(ta.second(), tb.second());
	default:
		return std::strong_ordering::equal;
	}
}
} // namespace hsb_detail

inline bool hsb::operator<(const hsb& o) const {
	return std::is_lt(*this <=> o);
}

inline std::strong_ordering hsb::operator<=>(const hsb& o) const {
	if (*this == o) return std::strong_ordering::equal;
	// Distinct trees can denote the same set and still differ; the tree
	// order separates whatever the printed text does not.
	if (auto c = to_string() <=> o.to_string(); c != 0) return c;
	return hsb_detail::tree_order(root_ref(), o.root_ref());
}

// ── Serialization ─────────────────────────────────────────────────────────────

namespace hsb_detail {
// Render the formula tree rooted at n; null renders as "bot".
inline std::string to_string_tref(tref n) {
	if (!n) return "bot";
	auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
	switch (k) {
	case hsb::kind::bot: return "bot";
	case hsb::kind::top: return "top";
	case hsb::kind::halfspace:
		return hsb_halfspace_pool::get(hsb_tree::get(n).value.data).to_string();
	case hsb::kind::not_:
		return "~(" + to_string_tref(hsb_tree::get(n).first()) + ")";
	case hsb::kind::and_:
		return "(" + to_string_tref(hsb_tree::get(n).first())
		     + " & " + to_string_tref(hsb_tree::get(n).second()) + ")";
	case hsb::kind::or_:
		return "(" + to_string_tref(hsb_tree::get(n).first())
		     + " | " + to_string_tref(hsb_tree::get(n).second()) + ")";
	}
	return "";
}
} // namespace hsb_detail

inline std::string hsb::to_string() const {
	return hsb_detail::to_string_tref(root_ref());
}

inline hsb hsb::from_string(const std::string& s) {
	std::string t = s;
	t.erase(0, t.find_first_not_of(" \t\n\r"));
	auto last = t.find_last_not_of(" \t\n\r");
	if (last != std::string::npos) t = t.substr(0, last + 1);
	if (t == "top") return top();
	if (t == "bot" || t == "bottom") return bottom();
	return bottom();
}

// ── Non-member operators ───────────────────────────────────────────────────────

inline std::ostream& operator<<(std::ostream& os, const hsb& h) {
	return os << h.to_string();
}

} // namespace idni::tau_lang

/// @brief `std::hash` specialisation for `idni::tau_lang::hsb`.
/// Equal roots give equal content, so this agrees with operator==.
template<>
struct std::hash<idni::tau_lang::hsb> {
	size_t operator()(const idni::tau_lang::hsb& h) const noexcept {
		return static_cast<size_t>(h.content_hash());
	}
};

namespace idni::tau_lang {

// =============================================================================
// hsb_grammar_detail — parse-tree → hsb tree construction
// =============================================================================

namespace hsb_grammar_detail {

using tt   = hsb_parser::tree::traverser;
using type = hsb_parser::nonterminal;

// A linear expression: coefficient per variable index, plus a constant.
// ok is false once a number in it failed to read.
struct linexpr_result {
	std::map<size_t, hsb_rational> coeffs;
	hsb_rational bias;
	bool ok = true;
};

// The exact value of an unsigned integer, decimal or fraction; nullopt for a
// fraction with a zero denominator, which build_halfspace then refuses.
inline std::optional<hsb_rational> parse_unum(const std::string& s) {
	return hsb_rational::parse(s);
}

// The index n of a variable node `x[n]`.
inline size_t eval_var(const tt& v) {
	return static_cast<size_t>(std::stoull((v | tt::only_child) | tt::terminals));
}

// One signed term of a linear expression: a scaled variable or a constant.
inline linexpr_result eval_lterm(const tt& t) {
	auto child = t | tt::only_child;
	auto nt    = child | tt::nonterminal;
	auto ch    = (child | tt::children)();
	linexpr_result r;
	// The number of an unsigned-number node, negated when neg; a malformed
	// one clears r.ok.
	auto num = [&r](const tt& u, bool neg) {
		auto v = parse_unum(u | tt::terminals);
		if (!v) { r.ok = false; return hsb_rational{}; }
		return neg ? -*v : *v;
	};
	switch (nt) {
	case type::lt_coeff_var:     r.coeffs[eval_var(ch[1])] = num(ch[0], false); break;
	case type::lt_var_coeff:     r.coeffs[eval_var(ch[0])] = num(ch[1], false); break;
	case type::lt_neg_coeff_var: r.coeffs[eval_var(ch[1])] = num(ch[0], true);  break;
	case type::lt_neg_var_coeff: r.coeffs[eval_var(ch[0])] = num(ch[1], true);  break;
	case type::lt_neg_var:       r.coeffs[eval_var(ch[0])] = -1;                break;
	case type::lt_neg_const:     r.bias = num(ch[0], true);                     break;
	case type::lt_var:           r.coeffs[eval_var(ch[0])] = 1;                 break;
	case type::lt_const:         r.bias = num(ch[0], false);                    break;
	default: break;
	}
	return r;
}

// A sum/difference of terms, collected per variable.
inline linexpr_result eval_linexpr(const tt& t) {
	auto child = t | tt::only_child;
	auto nt    = child | tt::nonterminal;
	auto ch    = (child | tt::children)();
	switch (nt) {
	case type::le_add: {
		auto l = eval_linexpr(ch[0]);
		auto r = eval_lterm(ch[1]);
		for (auto& [i, c] : r.coeffs) l.coeffs[i] += c;
		l.bias += r.bias;
		l.ok = l.ok && r.ok;
		return l;
	}
	case type::le_sub: {
		auto l = eval_linexpr(ch[0]);
		auto r = eval_lterm(ch[1]);
		for (auto& [i, c] : r.coeffs) l.coeffs[i] -= c;
		l.bias -= r.bias;
		l.ok = l.ok && r.ok;
		return l;
	}
	case type::le_lterm:
		return eval_lterm(ch[0]);
	default:
		return {};
	}
}

// The halfspace `le <op> 0`, of dimension one past the highest variable;
// nullopt when no variable is left or a number failed to read.
inline std::optional<hsb_halfspace> build_halfspace(const linexpr_result& le) {
	if (!le.ok) return std::nullopt;
	size_t dim = 0;
	for (auto& [i, c] : le.coeffs) dim = std::max(dim, i + 1);
	if (dim == 0) return std::nullopt;
	hsb_halfspace h;
	h.w.assign(dim, hsb_rational{});
	for (auto& [i, c] : le.coeffs) h.w[i] = c;
	h.b = le.bias;
	return h;
}

// The formula tree of a parsed hsb literal, built through the simplifying
// operators. An error for a halfspace whose written strictness does not
// match its canonical one; no value and no error for a malformed halfspace
// or an unknown node, which parse_hsb turns into an error.
inline result<tref> eval_parse_tree(const tt& t) {
	result<tref> r;
	auto n  = t | tt::only_child;
	auto nt = n | tt::nonterminal;
	switch (nt) {
	case type::hsb_top:   return r.with_value(hsb::mk_top());
	case type::hsb_bot:   return r.with_value(hsb::mk_bot());
	case type::hsb_not: {
		TAU_TRY(auto inner, eval_parse_tree(n | tt::only_child));
		// reuse short-circuit logic from operator~
		return r.with_value((~hsb(inner)).root_ref());
	}
	case type::hsb_paren:
		return eval_parse_tree(n | tt::only_child);
	case type::hsb_and: {
		auto ch = (n | tt::children)();
		TAU_TRY(auto l, eval_parse_tree(ch[0]));
		TAU_TRY(auto rhs, eval_parse_tree(ch[1]));
		return r.with_value((hsb(l) & hsb(rhs)).root_ref());
	}
	case type::hsb_or: {
		auto ch = (n | tt::children)();
		TAU_TRY(auto l, eval_parse_tree(ch[0]));
		TAU_TRY(auto rhs, eval_parse_tree(ch[1]));
		return r.with_value((hsb(l) | hsb(rhs)).root_ref());
	}
	case type::hsb_hs: {
		// `lhs op rhs` with both sides linear expressions; the half-space
		// is `(lhs - rhs) op 0`, so `x[0] < 1` is `x[0] - 1 < 0` and
		// `x[0] <= x[1]` is `x[0] - x[1] <= 0`.
		auto hs_child = (n | tt::only_child) | tt::only_child;
		auto ch       = (hs_child | tt::children)();
		if (ch.size() < 2) return r;
		linexpr_result lhs = eval_linexpr(ch[0]);
		linexpr_result rhs = eval_linexpr(ch[1]);
		for (auto& [i, c] : rhs.coeffs) lhs.coeffs[i] -= c;
		lhs.bias -= rhs.bias;
		lhs.ok = lhs.ok && rhs.ok;
		// A coefficient that cancelled leaves no dimension behind.
		for (auto it = lhs.coeffs.begin(); it != lhs.coeffs.end(); )
			if (it->second.is_zero())
				it = lhs.coeffs.erase(it);
			else ++it;
		auto opt_h = build_halfspace(lhs);
		if (!opt_h) return r;
		// Strictness is canonical in this algebra (hsb.h): a half-space
		// whose leading coefficient is positive is open, one whose leading
		// coefficient is negative is closed, and the other two sets are not
		// elements of the algebra at all. A literal whose operator names
		// one of those two sets is REJECTED: reading the element that does
		// exist would silently change the set the user wrote.
		const bool wrote_strict = (hs_child | tt::nonterminal) == type::hs_lt;
		if (wrote_strict != opt_h->is_strict()) {
			std::ostringstream oss;
			oss << "the half-space written "
				<< (wrote_strict ? "strict (`<`)" : "closed (`<=`)")
				<< " is not an element of the lex-half-open algebra: with "
				<< (opt_h->is_strict() ? "a positive" : "a negative")
				<< " leading coefficient only the "
				<< (opt_h->is_strict() ? "open" : "closed")
				<< " set exists, `" << opt_h->to_string() << "`. Write that "
				"set, or put the leading variable on the other side of "
				"the comparison to name the other bound (e.g. `x[0] < 1` "
				"is open, `1 <= x[0]` is closed).";
			r.error(code::parse_error, oss.str());
			return r;
		}
		return r.with_value(hsb::mk_hs(*opt_h));
	}
	default:
		return r;
	}
}

} // namespace hsb_grammar_detail

// =============================================================================
// parse_hsb template definition
// =============================================================================

/**
 * @brief Parses an hsb literal from @p src using the hsb grammar.
 *
 * @tparam BAs  The full BA-pack used in the tau `node<BAs...>` type.
 * @param  src  Source string in hsb syntax (without surrounding `{}`).
 * @returns     A `constant_with_type` variant wrapping the parsed `hsb` value
 *              on success, or a report explaining the refusal on failure.
 */
template <typename... BAs>
requires BAsPack<BAs...>
result<typename node<BAs...>::constant_with_type>
parse_hsb(const std::string& src) {
	result<typename node<BAs...>::constant_with_type> r;
	auto parsed = hsb_parser::instance().parse(src.c_str(), src.size());
	if (!parsed.found) {
		r.error(code::parse_error, parsed.parse_error
			.to_str(hsb_parser::error::info_lvl::INFO_BASIC));
		return r;
	}

	auto t = hsb_parser::tree::traverser(parsed.get_shaped_tree2())
		| hsb_parser::hsb;
	if (!t.has_value()) {
		r.error(code::parse_error, "No hsb value in parse tree");
		return r;
	}

	// TAU_TRY_OR_ATTR evaluates its attr list before the call, so it would
	// intern the source text on every successful parse. Attach it by hand,
	// only in the error branch.
	auto hsb_child = hsb_grammar_detail::eval_parse_tree(t);
	const bool hsb_child_well_formed = hsb_child.is_well_formed();
	auto hsb_tval_opt = r.merge_take(std::move(hsb_child));
	if (!hsb_tval_opt && !hsb_child_well_formed && !r.has_error()) {
		r.error(code::parse_error, "failed to parse an hsb constant",
			{{label::value, src}});
	}
	if (!hsb_tval_opt) return r;
	tref tval = std::move(*hsb_tval_opt);

	return r.with_value(typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ hsb(tval) },
		hsb_type<node<BAs...>>() });
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_TMPL_H__
