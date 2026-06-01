// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_TMPL_H__

#include "boolean_algebras/hsb.h"

namespace idni::tau_lang {

// =============================================================================
// hsb_halfspace — method definitions
// =============================================================================

inline size_t hsb_halfspace::dimension() const noexcept { return w.size(); }

inline size_t hsb_halfspace::lex_leading_index() const noexcept {
	for (size_t i = 0; i < w.size(); ++i)
		if (!hsb_detail::feq(w[i], 0.0)) return i;
	return w.size();
}

inline int hsb_halfspace::lex_leading_sign() const noexcept {
	for (size_t i = 0; i < w.size(); ++i) {
		if (w[i] > 0.0) return +1;
		if (w[i] < 0.0) return -1;
	}
	return 0;
}

inline bool hsb_halfspace::is_strict() const noexcept {
	return lex_leading_sign() == +1;
}

inline double hsb_halfspace::eval(const std::vector<double>& x) const noexcept {
	auto dim = std::min(w.size(), x.size());
	double sum = b;
	for (size_t i = 0; i < dim; ++i) sum += w[i] * x[i];
	return sum;
}

inline bool hsb_halfspace::contains(const std::vector<double>& x) const noexcept {
	double v = eval(x);
	return is_strict() ? (v < 0.0) : (v <= 0.0);
}

inline hsb_halfspace hsb_halfspace::negate() const {
	hsb_halfspace r;
	r.w.resize(w.size());
	for (size_t i = 0; i < w.size(); ++i) r.w[i] = -w[i];
	r.b = -b;
	return r;
}

inline bool hsb_halfspace::operator==(const hsb_halfspace& o) const noexcept {
	if (w.size() != o.w.size()) return false;
	if (!hsb_detail::feq(b, o.b)) return false;
	for (size_t i = 0; i < w.size(); ++i)
		if (!hsb_detail::feq(w[i], o.w[i])) return false;
	return true;
}

inline bool hsb_halfspace::operator!=(const hsb_halfspace& o) const noexcept {
	return !(*this == o);
}

inline hsb_halfspace hsb_halfspace::normalize() const {
	// Find the highest-indexed non-zero coefficient.
	size_t j = w.size();
	for (size_t k = w.size(); k > 0; --k)
		if (!hsb_detail::feq(w[k-1], 0.0)) { j = k - 1; break; }
	if (j == w.size()) return *this; // zero vector — nothing to do

	double divisor = std::abs(w[j]);
	hsb_halfspace r;
	r.w.resize(w.size());
	for (size_t k = 0; k < w.size(); ++k) r.w[k] = w[k] / divisor;
	r.b = b / divisor;
	return r;
}

inline bool hsb_halfspace::operator<(const hsb_halfspace& o) const noexcept {
	size_t n = std::max(w.size(), o.w.size());
	for (size_t k = 0; k < n; ++k) {
		double lv = k < w.size()   ? w[k]   : 0.0;
		double rv = k < o.w.size() ? o.w[k] : 0.0;
		if (hsb_detail::feq(lv, rv)) continue;
		return lv < rv;
	}
	if (hsb_detail::feq(b, o.b)) return false;
	return b < o.b;
}

inline std::string hsb_halfspace::to_string() const {
	std::ostringstream os;
	bool first = true;
	for (size_t i = 0; i < w.size(); ++i) {
		if (hsb_detail::feq(w[i], 0.0)) continue;
		if (!first && w[i] > 0.0) os << " + ";
		else if (w[i] < 0.0) os << (first ? "-" : " - ");
		double aw = std::abs(w[i]);
		if (!hsb_detail::feq(aw, 1.0)) os << aw << "*";
		os << "x[" << i << "]";
		first = false;
	}
	if (first) os << "0";
	if (b > 0.0) os << " + " << b;
	else if (b < 0.0) os << " - " << std::abs(b);
	os << (is_strict() ? " < 0" : " <= 0");
	return os.str();
}

// =============================================================================
// hsb_halfspace_pool — method definitions
// =============================================================================

inline size_t hsb_halfspace_pool::insert(const hsb_halfspace& h) {
	auto it = index_.find(h);
	if (it != index_.end()) return it->second;
	size_t idx = pool_.size();
	pool_.push_back(h);
	index_[h] = idx;
	return idx;
}

inline const hsb_halfspace& hsb_halfspace_pool::get(size_t idx) {
	return pool_[idx];
}

inline size_t hsb_halfspace_pool::complement_index(size_t idx) {
	return insert(pool_[idx].negate());
}

inline size_t hsb_halfspace_pool::size() {
	return pool_.size();
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

inline bool hsb::operator==(bool b) const {
	auto k = root_kind();
	return b ? (k == kind::top) : (k == kind::bot);
}
inline bool hsb::operator!=(bool b) const { return !(*this == b); }

inline bool hsb::operator<(const hsb& o) const {
	if (*this == o) return false;
	return to_string() < o.to_string();
}

inline std::strong_ordering hsb::operator<=>(const hsb& o) const {
	if (*this == o) return std::strong_ordering::equal;
	return to_string() < o.to_string()
		? std::strong_ordering::less
		: std::strong_ordering::greater;
}

// ── Serialization ─────────────────────────────────────────────────────────────

namespace hsb_detail {
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
/// Hashes via the tref pointer, consistent with operator== (O(1)).
template<>
struct std::hash<idni::tau_lang::hsb> {
	size_t operator()(const idni::tau_lang::hsb& h) const noexcept {
		return std::hash<const void*>{}(
			static_cast<const void*>(h.root_ref()));
	}
};

namespace idni::tau_lang {

// =============================================================================
// hsb_grammar_detail — parse-tree → hsb tree construction
// =============================================================================

namespace hsb_grammar_detail {

using tt   = hsb_parser::tree::traverser;
using type = hsb_parser::nonterminal;

struct linexpr_result {
	std::map<size_t, double> coeffs;
	double bias = 0.0;
};

inline double parse_unum(const std::string& s) {
	auto slash = s.find('/');
	if (slash != std::string::npos)
		return std::stod(s.substr(0, slash)) / std::stod(s.substr(slash + 1));
	return std::stod(s);
}

inline size_t eval_var(const tt& v) {
	return static_cast<size_t>(std::stoull((v | tt::only_child) | tt::terminals));
}

inline double eval_unum(const tt& u) {
	return parse_unum(u | tt::terminals);
}

inline linexpr_result eval_lterm(const tt& t) {
	auto child = t | tt::only_child;
	auto nt    = child | tt::nonterminal;
	auto ch    = (child | tt::children)();
	linexpr_result r;
	switch (nt) {
	case type::lt_coeff_var:     r.coeffs[eval_var(ch[1])] = +eval_unum(ch[0]); break;
	case type::lt_var_coeff:     r.coeffs[eval_var(ch[0])] = +eval_unum(ch[1]); break;
	case type::lt_neg_coeff_var: r.coeffs[eval_var(ch[1])] = -eval_unum(ch[0]); break;
	case type::lt_neg_var_coeff: r.coeffs[eval_var(ch[0])] = -eval_unum(ch[1]); break;
	case type::lt_neg_var:       r.coeffs[eval_var(ch[0])] = -1.0;              break;
	case type::lt_neg_const:     r.bias = -eval_unum(ch[0]);                    break;
	case type::lt_var:           r.coeffs[eval_var(ch[0])] = +1.0;              break;
	case type::lt_const:         r.bias = +eval_unum(ch[0]);                    break;
	default: break;
	}
	return r;
}

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
		return l;
	}
	case type::le_sub: {
		auto l = eval_linexpr(ch[0]);
		auto r = eval_lterm(ch[1]);
		for (auto& [i, c] : r.coeffs) l.coeffs[i] -= c;
		l.bias -= r.bias;
		return l;
	}
	case type::le_lterm:
		return eval_lterm(ch[0]);
	default:
		return {};
	}
}

inline std::optional<hsb_halfspace> build_halfspace(const linexpr_result& le) {
	size_t dim = 0;
	for (auto& [i, c] : le.coeffs) dim = std::max(dim, i + 1);
	if (dim == 0) return std::nullopt;
	hsb_halfspace h;
	h.w.assign(dim, 0.0);
	for (auto& [i, c] : le.coeffs) h.w[i] = c;
	h.b = le.bias;
	return h;
}

inline std::optional<tref> eval_parse_tree(const tt& t) {
	auto n  = t | tt::only_child;
	auto nt = n | tt::nonterminal;
	switch (nt) {
	case type::hsb_top:   return hsb::mk_top();
	case type::hsb_bot:   return hsb::mk_bot();
	case type::hsb_not: {
		auto inner = eval_parse_tree(n | tt::only_child);
		if (!inner) return std::nullopt;
		// reuse short-circuit logic from operator~
		return (~hsb(*inner)).root_ref();
	}
	case type::hsb_paren:
		return eval_parse_tree(n | tt::only_child);
	case type::hsb_and: {
		auto ch = (n | tt::children)();
		auto l = eval_parse_tree(ch[0]);
		auto r = eval_parse_tree(ch[1]);
		if (!l || !r) return std::nullopt;
		return (hsb(*l) & hsb(*r)).root_ref();
	}
	case type::hsb_or: {
		auto ch = (n | tt::children)();
		auto l = eval_parse_tree(ch[0]);
		auto r = eval_parse_tree(ch[1]);
		if (!l || !r) return std::nullopt;
		return (hsb(*l) | hsb(*r)).root_ref();
	}
	case type::hsb_hs: {
		auto hs_child     = (n | tt::only_child) | tt::only_child;
		auto linexpr_node = (hs_child | tt::children)()[0];
		auto opt_h = build_halfspace(eval_linexpr(linexpr_node));
		if (!opt_h) return std::nullopt;
		return hsb::mk_hs(*opt_h);
	}
	default:
		return std::nullopt;
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
 *              on success, or `std::nullopt` if the string does not conform to
 *              the hsb grammar.
 */
template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type>
parse_hsb(const std::string& src) {
	auto result = hsb_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) return std::nullopt;

	auto t = hsb_parser::tree::traverser(result.get_shaped_tree2())
		| hsb_parser::hsb;
	if (!t.has_value()) return std::nullopt;

	auto tval = hsb_grammar_detail::eval_parse_tree(t);
	if (!tval) return std::nullopt;

	return typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ hsb(*tval) },
		hsb_type<node<BAs...>>() };
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_TMPL_H__
