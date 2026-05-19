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
	double sum = b;
	size_t n = std::min(w.size(), x.size());
	for (size_t i = 0; i < n; ++i) sum += w[i] * x[i];
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
// detail — LP feasibility helpers (QF_LRA via cvc5)
// =============================================================================

namespace detail {

struct linear_constraint {
	std::vector<double> w;
	double b;
	bool strict; ///< true => strict (<), false => non-strict (<=)
};

/// Collect all halfspace constraints from an AND-tree into @p out.
/// Returns false iff the node is structurally unsatisfiable (contains bot).
inline bool collect_conjunction(const hsb::node_ptr& n,
                                std::vector<linear_constraint>& out) {
	if (!n) return false;
	switch (n->k) {
	case hsb::kind::bot:       return false;
	case hsb::kind::top:       return true;
	case hsb::kind::halfspace:
		out.push_back({n->hs.w, n->hs.b, n->hs.is_strict()});
		return true;
	case hsb::kind::and_:
		return collect_conjunction(n->lhs, out)
		    && collect_conjunction(n->rhs, out);
	default:
		return true; // or_ / not_ treated conservatively as satisfiable
	}
}

/// Check LP feasibility of @p cs using cvc5 QF_LRA.
inline bool lra_feasible(const std::vector<linear_constraint>& cs, size_t dim) {
	if (cs.empty()) return true;

	cvc5::Solver solver(cvc5_term_manager);
	solver.setOption("produce-models", "true");
	solver.setLogic("QF_LRA");

	std::vector<cvc5::Term> vars(dim);
	for (size_t i = 0; i < dim; ++i)
		vars[i] = cvc5_term_manager.mkConst(
			cvc5_term_manager.getRealSort(),
			"x" + std::to_string(i));

	auto mk_rational = [](double v) -> cvc5::Term {
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(20) << v;
		return cvc5_term_manager.mkReal(oss.str());
	};

	for (auto& c : cs) {
		cvc5::Term sum = mk_rational(c.b);
		for (size_t i = 0; i < std::min(c.w.size(), dim); ++i) {
			if (hsb_detail::feq(c.w[i], 0.0)) continue;
			cvc5::Term prod = cvc5_term_manager.mkTerm(
				cvc5::Kind::MULT, {mk_rational(c.w[i]), vars[i]});
			sum = cvc5_term_manager.mkTerm(cvc5::Kind::ADD, {sum, prod});
		}
		cvc5::Term zero = cvc5_term_manager.mkReal(0);
		cvc5::Term con = c.strict
			? cvc5_term_manager.mkTerm(cvc5::Kind::LT,  {sum, zero})
			: cvc5_term_manager.mkTerm(cvc5::Kind::LEQ, {sum, zero});
		solver.assertFormula(con);
	}

	return solver.checkSat().isSat();
}

/// Find a feasible point for @p cs via cvc5 model extraction.
/// Returns `nullopt` if the system is infeasible.
inline std::optional<std::vector<double>>
find_feasible_point(const std::vector<linear_constraint>& cs, size_t dim) {
	if (cs.empty())
		return std::vector<double>(dim, 0.0);

	cvc5::Solver solver(cvc5_term_manager);
	solver.setOption("produce-models", "true");
	solver.setLogic("QF_LRA");

	std::vector<cvc5::Term> vars(dim);
	for (size_t i = 0; i < dim; ++i)
		vars[i] = cvc5_term_manager.mkConst(
			cvc5_term_manager.getRealSort(),
			"x" + std::to_string(i));

	auto mk_rational = [](double v) -> cvc5::Term {
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(20) << v;
		return cvc5_term_manager.mkReal(oss.str());
	};

	for (auto& c : cs) {
		cvc5::Term sum = mk_rational(c.b);
		for (size_t i = 0; i < std::min(c.w.size(), dim); ++i) {
			if (hsb_detail::feq(c.w[i], 0.0)) continue;
			cvc5::Term prod = cvc5_term_manager.mkTerm(
				cvc5::Kind::MULT, {mk_rational(c.w[i]), vars[i]});
			sum = cvc5_term_manager.mkTerm(cvc5::Kind::ADD, {sum, prod});
		}
		cvc5::Term zero = cvc5_term_manager.mkReal(0);
		cvc5::Term con = c.strict
			? cvc5_term_manager.mkTerm(cvc5::Kind::LT,  {sum, zero})
			: cvc5_term_manager.mkTerm(cvc5::Kind::LEQ, {sum, zero});
		solver.assertFormula(con);
	}

	if (!solver.checkSat().isSat())
		return std::nullopt;

	std::vector<double> pt(dim);
	for (size_t i = 0; i < dim; ++i) {
		std::string vs = solver.getValue(vars[i]).getRealValue();
		auto slash = vs.find('/');
		if (slash != std::string::npos)
			pt[i] = std::stod(vs.substr(0, slash))
			      / std::stod(vs.substr(slash + 1));
		else
			pt[i] = std::stod(vs);
	}
	return pt;
}

/// Find a feasible point for @p cs that additionally satisfies x[axis] > val (greater_than=true)
/// or x[axis] < val (greater_than=false).  Returns `nullopt` if infeasible.
inline std::optional<std::vector<double>>
find_second_feasible_point(const std::vector<linear_constraint>& cs,
                           size_t dim, size_t axis, double val, bool greater_than) {
	auto ext = cs;
	linear_constraint extra;
	extra.w.assign(dim, 0.0);
	if (greater_than) { extra.w[axis] = -1.0; extra.b =  val; extra.strict = true; }
	else              { extra.w[axis] =  1.0; extra.b = -val; extra.strict = true; }
	ext.push_back(extra);
	return find_feasible_point(ext, dim);
}

// Forward declaration needed by to_dnf.
inline hsb::node_ptr push_neg(const hsb::node_ptr& n);

/// Convert a formula node to DNF (vector of conjunctions of halfspace nodes).
inline void to_dnf(const hsb::node_ptr& n,
                   std::vector<std::vector<hsb::node_ptr>>& dnf) {
	if (!n) { dnf.clear(); return; }
	switch (n->k) {
	case hsb::kind::bot:
		break; // contributes nothing
	case hsb::kind::top:
		dnf.push_back({}); // empty conjunction = tautology
		break;
	case hsb::kind::halfspace:
		dnf.push_back({n});
		break;
	case hsb::kind::and_: {
		std::vector<std::vector<hsb::node_ptr>> left, right;
		to_dnf(n->lhs, left);
		to_dnf(n->rhs, right);
		for (auto& l : left)
			for (auto& r : right) {
				auto conj = l;
				conj.insert(conj.end(), r.begin(), r.end());
				dnf.push_back(std::move(conj));
			}
		break;
	}
	case hsb::kind::or_:
		to_dnf(n->lhs, dnf);
		to_dnf(n->rhs, dnf);
		break;
	case hsb::kind::not_:
		to_dnf(push_neg(n->inner), dnf);
		break;
	}
}

/// Push negation down to halfspace leaves (NNF step).
inline hsb::node_ptr push_neg(const hsb::node_ptr& n) {
	if (!n) return hsb::mk_bot();
	switch (n->k) {
	case hsb::kind::bot:       return hsb::mk_top();
	case hsb::kind::top:       return hsb::mk_bot();
	case hsb::kind::halfspace: return hsb::mk_hs(n->hs.negate());
	case hsb::kind::not_:      return n->inner; // ~~A = A
	case hsb::kind::and_:      return hsb::mk_or(push_neg(n->lhs), push_neg(n->rhs));
	case hsb::kind::or_:       return hsb::mk_and(push_neg(n->lhs), push_neg(n->rhs));
	}
	return hsb::mk_bot();
}

/// Convert to NNF (push all negations to leaves).
inline hsb::node_ptr to_nnf(const hsb::node_ptr& n) {
	if (!n) return hsb::mk_bot();
	switch (n->k) {
	case hsb::kind::bot:
	case hsb::kind::top:
	case hsb::kind::halfspace: return n;
	case hsb::kind::and_:      return hsb::mk_and(to_nnf(n->lhs), to_nnf(n->rhs));
	case hsb::kind::or_:       return hsb::mk_or(to_nnf(n->lhs), to_nnf(n->rhs));
	case hsb::kind::not_:      return push_neg(n->inner);
	}
	return hsb::mk_bot();
}

/// Infer ambient dimension from the formula tree.
inline size_t infer_dim(const hsb::node_ptr& n) {
	if (!n) return 0;
	switch (n->k) {
	case hsb::kind::halfspace: return n->hs.dimension();
	case hsb::kind::and_:
	case hsb::kind::or_:       return std::max(infer_dim(n->lhs), infer_dim(n->rhs));
	case hsb::kind::not_:      return infer_dim(n->inner);
	default:                   return 0;
	}
}

/// Return the first halfspace leaf found in the formula tree, or nullptr.
inline hsb::node_ptr find_leaf_halfspace(const hsb::node_ptr& n) {
	if (!n) return nullptr;
	switch (n->k) {
	case hsb::kind::halfspace: return n;
	case hsb::kind::and_:
	case hsb::kind::or_: {
		auto l = find_leaf_halfspace(n->lhs);
		return l ? l : find_leaf_halfspace(n->rhs);
	}
	case hsb::kind::not_: return find_leaf_halfspace(n->inner);
	default:              return nullptr;
	}
}

} // namespace detail

// =============================================================================
// Free function implementations
// =============================================================================

/**
 * @brief Returns true iff @p x is semantically equivalent to bottom (∅).
 *
 * Converts @p x to DNF then checks each disjunct for LP-feasibility via
 * cvc5 QF_LRA.  Returns `true` iff every disjunct is infeasible.
 *
 * @param x  The hsb element to test.
 */
inline bool is_hsb_zero(const hsb& x) {
	if (x.root->k == hsb::kind::bot) return true;
	if (x.root->k == hsb::kind::top) return false;

	auto nnf = detail::to_nnf(x.root);
	std::vector<std::vector<hsb::node_ptr>> dnf;
	detail::to_dnf(nnf, dnf);

	if (dnf.empty()) return true;

	size_t dim = detail::infer_dim(x.root);
	if (dim == 0) dim = 1;

	for (auto& conj : dnf) {
		std::vector<detail::linear_constraint> cs;
		bool ok = true;
		for (auto& nd : conj)
			if (!detail::collect_conjunction(nd, cs)) { ok = false; break; }
		if (!ok) continue;
		if (detail::lra_feasible(cs, dim)) return false;
	}
	return true;
}

/**
 * @brief Returns true iff @p x is semantically equivalent to top (R^d).
 * @param x  The hsb element to test.
 */
inline bool is_hsb_one(const hsb& x) {
	return is_hsb_zero(~x);
}

/**
 * @brief Normalises @p x: collapses to `bottom()` or `top()` if semantically
 *        zero or one; otherwise returns @p x unchanged.
 * @param x  The hsb element to normalise.
 */
inline hsb normalize_hsb(const hsb& x) {
	if (x.root->k == hsb::kind::bot || x.root->k == hsb::kind::top)
		return x;
	if (is_hsb_zero(x)) return hsb::bottom();
	if (is_hsb_one(x))  return hsb::top();
	return x;
}

/// @brief No-op symbol simplifier (required by the tau dispatcher interface).
inline tref simplify_hsb_symbol(tref sym) { return sym; }

/// @brief No-op term simplifier (required by the tau dispatcher interface).
inline tref simplify_hsb_term(tref t) { return t; }

/**
 * @brief Returns a proper sub-element y with `bot < y < x`.
 *
 * For each satisfiable DNF clause of @p x, finds a feasible interior point A via
 * cvc5, then scans axes from the highest-indexed variable downward to find a second
 * point B with B[axis] ≠ A[axis].  Returns `x & {x[axis] < (A[axis]+B[axis])/2}`.
 *
 * @param x   A non-zero, non-top hsb element.
 * @param st  Splitter hint (currently unused).
 */
inline hsb hsb_splitter(const hsb& x, splitter_type /*st*/) {
	if (x.root->k == hsb::kind::bot) return hsb::bottom();
	if (x.root->k == hsb::kind::top) {
		hsb_halfspace h; h.w = {1.0}; h.b = 0.0;
		return hsb::from_halfspace(h);
	}

	size_t dim = detail::infer_dim(x.root);
	if (dim == 0) dim = 1;

	auto nnf = detail::to_nnf(x.root);
	std::vector<std::vector<hsb::node_ptr>> dnf;
	detail::to_dnf(nnf, dnf);

	for (auto& conj : dnf) {
		std::vector<detail::linear_constraint> cs;
		bool ok = true;
		for (auto& nd : conj)
			if (!detail::collect_conjunction(nd, cs)) { ok = false; break; }
		if (!ok) continue;

		auto pt_a = detail::find_feasible_point(cs, dim);
		if (!pt_a) continue;

		// Scan axes from highest to lowest seeking a dimension with thickness.
		for (size_t axis = dim; axis-- > 0; ) {
			auto pt_b = detail::find_second_feasible_point(
				cs, dim, static_cast<size_t>(axis), (*pt_a)[axis], true);
			if (!pt_b)
				pt_b = detail::find_second_feasible_point(
					cs, dim, static_cast<size_t>(axis), (*pt_a)[axis], false);
			if (!pt_b) continue;

			double c = ((*pt_a)[axis] + (*pt_b)[axis]) / 2.0;
			// s = {x[axis] < c}: w[axis]=1.0, b=-c, s(w)=+1 (strict)
			hsb_halfspace h;
			h.w.assign(dim, 0.0); h.w[static_cast<size_t>(axis)] = 1.0; h.b = -c;
			hsb split = x & hsb::from_halfspace(h);
			if (!is_hsb_zero(split) && split != x) return split;
			split = x & ~hsb::from_halfspace(h);
			if (!is_hsb_zero(split) && split != x) return split;
		}
	}
	return x;
}

/**
 * @brief Returns a fixed non-trivial seed element `{x_1 < 0}`.
 *
 * The seed has lex-leading sign +1 (strict), so it is a canonical open
 * half-space in dimension 1.
 */
inline hsb hsb_splitter_one() {
	hsb_halfspace h; h.w = {1.0}; h.b = 0.0;
	return hsb::from_halfspace(h);
}

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

inline hsb build_halfspace(const linexpr_result& le) {
	size_t dim = 0;
	for (auto& [i, c] : le.coeffs) dim = std::max(dim, i + 1);
	if (dim == 0) dim = 1;
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
