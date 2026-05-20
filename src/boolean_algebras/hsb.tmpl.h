// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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
// hsb::node — method definitions
// =============================================================================

inline bool hsb::node::operator==(const node& o) const {
	if (k != o.k) return false;
	switch (k) {
	case kind::bot:
	case kind::top:       return true;
	case kind::halfspace: return hs == o.hs;
	case kind::not_:      return *inner == *o.inner;
	case kind::and_:
	case kind::or_:       return *lhs == *o.lhs && *rhs == *o.rhs;
	}
	return false;
}

inline bool hsb::node::is_complement_of(const node& o) const {
	if (k == kind::not_ && *inner == o) return true;
	if (o.k == kind::not_ && *o.inner == *this) return true;
	if (k == kind::halfspace && o.k == kind::halfspace)
		return hs == o.hs.negate();
	return false;
}

inline std::string hsb::node::to_string() const {
	switch (k) {
	case kind::bot:       return "bot";
	case kind::top:       return "top";
	case kind::halfspace: return hs.to_string();
	case kind::not_:      return "~(" + inner->to_string() + ")";
	case kind::and_:      return "(" + lhs->to_string()
	                           + " & " + rhs->to_string() + ")";
	case kind::or_:       return "(" + lhs->to_string()
	                           + " | " + rhs->to_string() + ")";
	}
	return "";
}

// =============================================================================
// hsb — constructor, factory and operator definitions
// =============================================================================

inline hsb::hsb() : root(mk_bot()) {}
inline hsb::hsb(node_ptr r) : root(std::move(r)) {}

inline hsb::node_ptr hsb::mk_bot() {
	auto p = std::make_shared<node>(); p->k = kind::bot; return p;
}
inline hsb::node_ptr hsb::mk_top() {
	auto p = std::make_shared<node>(); p->k = kind::top; return p;
}
inline hsb::node_ptr hsb::mk_hs(hsb_halfspace h) {
	auto p = std::make_shared<node>();
	p->k = kind::halfspace; p->hs = std::move(h); return p;
}
inline hsb::node_ptr hsb::mk_and(node_ptr l, node_ptr r) {
	auto p = std::make_shared<node>();
	p->k = kind::and_; p->lhs = std::move(l); p->rhs = std::move(r); return p;
}
inline hsb::node_ptr hsb::mk_or(node_ptr l, node_ptr r) {
	auto p = std::make_shared<node>();
	p->k = kind::or_; p->lhs = std::move(l); p->rhs = std::move(r); return p;
}
inline hsb::node_ptr hsb::mk_not(node_ptr i) {
	auto p = std::make_shared<node>();
	p->k = kind::not_; p->inner = std::move(i); return p;
}

inline hsb hsb::bottom() { return hsb{mk_bot()}; }
inline hsb hsb::top()    { return hsb{mk_top()}; }

inline hsb hsb::from_halfspace(hsb_halfspace h) {
	return hsb{mk_hs(std::move(h))};
}

inline hsb hsb::operator&(const hsb& o) const {
	if (root->k == kind::bot || o.root->k == kind::bot) return bottom();
	if (root->k == kind::top) return o;
	if (o.root->k == kind::top) return *this;
	if (*root == *o.root) return *this;
	if (root->is_complement_of(*o.root)) return bottom();
	return hsb{mk_and(root, o.root)};
}

inline hsb hsb::operator|(const hsb& o) const {
	if (root->k == kind::bot) return o;
	if (o.root->k == kind::bot) return *this;
	if (root->k == kind::top || o.root->k == kind::top) return top();
	if (*root == *o.root) return *this;
	if (root->is_complement_of(*o.root)) return top();
	return hsb{mk_or(root, o.root)};
}

inline hsb hsb::operator~() const {
	if (root->k == kind::bot) return top();
	if (root->k == kind::top) return bottom();
	if (root->k == kind::not_) return hsb{root->inner};
	if (root->k == kind::halfspace) return hsb{mk_hs(root->hs.negate())};
	return hsb{mk_not(root)};
}

inline hsb hsb::operator^(const hsb& o) const {
	return (*this | o) & ~(*this & o);
}

inline bool hsb::operator==(const hsb& o) const { return *root == *o.root; }
inline bool hsb::operator!=(const hsb& o) const { return !(*this == o); }

inline bool hsb::operator==(bool b) const {
	return b ? (root->k == kind::top) : (root->k == kind::bot);
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

inline std::string hsb::to_string() const {
	return root ? root->to_string() : "bot";
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

// =============================================================================
// Non-member operators
// =============================================================================

inline std::ostream& operator<<(std::ostream& os, const hsb& h) {
	return os << h.to_string();
}

} // namespace idni::tau_lang

/// @brief `std::hash` specialisation for `idni::tau_lang::hsb` (hashes via `to_string()`).
template<>
struct std::hash<idni::tau_lang::hsb> {
	size_t operator()(const idni::tau_lang::hsb& h) const noexcept {
		return std::hash<std::string>{}(h.to_string());
	}
};

namespace idni::tau_lang {

// =============================================================================
// hsb_grammar_detail — parse-tree → hsb evaluation
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

inline std::optional<hsb> build_halfspace(const linexpr_result& le) {
	size_t dim = 0;
	for (auto& [i, c] : le.coeffs) dim = std::max(dim, i + 1);
	if (dim == 0) return std::nullopt;
	hsb_halfspace h;
	h.w.assign(dim, 0.0);
	for (auto& [i, c] : le.coeffs) h.w[i] = c;
	h.b = le.bias;
	return hsb::from_halfspace(std::move(h));
}

inline std::optional<hsb> eval_parse_tree(const tt& t) {
	auto n  = t | tt::only_child;
	auto nt = n | tt::nonterminal;
	switch (nt) {
	case type::hsb_top:   return hsb::top();
	case type::hsb_bot:   return hsb::bottom();
	case type::hsb_not: {
		auto inner = eval_parse_tree(n | tt::only_child);
		if (!inner) return std::nullopt;
		return ~(*inner);
	}
	case type::hsb_paren:
		return eval_parse_tree(n | tt::only_child);
	case type::hsb_and: {
		auto ch = (n | tt::children)();
		auto l = eval_parse_tree(ch[0]);
		auto r = eval_parse_tree(ch[1]);
		if (!l || !r) return std::nullopt;
		return *l & *r;
	}
	case type::hsb_or: {
		auto ch = (n | tt::children)();
		auto l = eval_parse_tree(ch[0]);
		auto r = eval_parse_tree(ch[1]);
		if (!l || !r) return std::nullopt;
		return *l | *r;
	}
	case type::hsb_hs: {
		auto hs_child     = (n | tt::only_child) | tt::only_child;
		auto linexpr_node = (hs_child | tt::children)()[0];
		return build_halfspace(eval_linexpr(linexpr_node));
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

	auto hval = hsb_grammar_detail::eval_parse_tree(t);
	if (!hval) return std::nullopt;

	return typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ *hval },
		hsb_type<node<BAs...>>() };
}

} // namespace idni::tau_lang
