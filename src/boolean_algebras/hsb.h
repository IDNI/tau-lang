// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_H__

#include <algorithm>
#include <cassert>
#include <cmath>
#include <compare>
#include <functional>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>

#include "tau_tree.h"
#include "splitter_types.h"
#include "boolean_algebras/cvc5/cvc5.h"

namespace idni::tau_lang {

namespace hsb_detail {
	inline bool feq(double a, double b) noexcept {
		return !(a < b) && !(b < a);
	}
}

// -----------------------------------------------------------------------------
// hsb — atomless Boolean algebra of lex-half-open polyhedra in R^d (LP_d^Q).
//
// Generalizes qint (1D half-open intervals [a,b)) to d dimensions using
// canonical halfspaces.  For w in R^d \ {0}, define:
//   L(w) = min { i : w_i != 0 }           (lex-leading index)
//   s(w) = sign(w_{L(w)}) in {+1, -1}     (lex-leading sign)
//
// The canonical halfspace H_{w,b} is:
//   { x : <w,x> + b <= 0 }  if s(w) = -1  (closed — boundary included)
//   { x : <w,x> + b <  0 }  if s(w) = +1  (open — boundary excluded)
//
// Key properties:
//   1. Complement: R^d \ H_{w,b} = H_{-w,-b}  (lex sign flips => open<->closed)
//   2. No equalities: {<v,x> = c} is never expressible => atomless
//   3. 1D reduction: d=1 gives exactly qint's half-open intervals
// -----------------------------------------------------------------------------

struct hsb_halfspace {
	std::vector<double> w;  // normal vector (d dimensions)
	double b = 0.0;         // bias: <w,x> + b {<=,<} 0

	size_t dimension() const noexcept { return w.size(); }

	// L(w): index of first non-zero component
	size_t lex_leading_index() const noexcept {
		for (size_t i = 0; i < w.size(); ++i)
			if (!hsb_detail::feq(w[i], 0.0)) return i;
		return w.size(); // degenerate: zero vector
	}

	// s(w): sign of w_{L(w)}
	int lex_leading_sign() const noexcept {
		for (size_t i = 0; i < w.size(); ++i) {
			if (w[i] > 0.0) return +1;
			if (w[i] < 0.0) return -1;
		}
		return 0; // degenerate: zero vector
	}

	// s(w) = +1 => strict (<), s(w) = -1 => non-strict (<=)
	bool is_strict() const noexcept { return lex_leading_sign() == +1; }

	// Evaluate <w,x> + b
	double eval(const std::vector<double>& x) const noexcept {
		double sum = b;
		size_t n = std::min(w.size(), x.size());
		for (size_t i = 0; i < n; ++i) sum += w[i] * x[i];
		return sum;
	}

	// Check if x satisfies this halfspace constraint
	bool contains(const std::vector<double>& x) const noexcept {
		double v = eval(x);
		return is_strict() ? (v < 0.0) : (v <= 0.0);
	}

	// Negate: H_{-w,-b} (complement halfspace)
	hsb_halfspace negate() const {
		hsb_halfspace r;
		r.w.resize(w.size());
		for (size_t i = 0; i < w.size(); ++i) r.w[i] = -w[i];
		r.b = -b;
		return r;
	}

	bool operator==(const hsb_halfspace& o) const noexcept {
		if (w.size() != o.w.size()) return false;
		if (!hsb_detail::feq(b, o.b)) return false;
		for (size_t i = 0; i < w.size(); ++i)
			if (!hsb_detail::feq(w[i], o.w[i])) return false;
		return true;
	}
	bool operator!=(const hsb_halfspace& o) const noexcept {
		return !(*this == o);
	}

	std::string to_string() const {
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
};

// -----------------------------------------------------------------------------
// hsb: formula tree of canonical halfspaces combined with Boolean operations
// -----------------------------------------------------------------------------

struct hsb {
	enum class kind : uint8_t {
		bot, top,
		halfspace,
		and_, or_, not_
	};

	struct node {
		kind k = kind::bot;
		hsb_halfspace hs;                 // for kind::halfspace
		std::shared_ptr<node> lhs;        // for and_, or_
		std::shared_ptr<node> rhs;        // for and_, or_
		std::shared_ptr<node> inner;      // for not_

		bool struct_eq(const node& o) const {
			if (k != o.k) return false;
			switch (k) {
			case kind::bot:
			case kind::top:       return true;
			case kind::halfspace: return hs == o.hs;
			case kind::not_:      return inner->struct_eq(*o.inner);
			case kind::and_:
			case kind::or_:       return lhs->struct_eq(*o.lhs)
			                          && rhs->struct_eq(*o.rhs);
			}
			return false;
		}

		bool is_complement_of(const node& o) const {
			if (k == kind::not_ && inner->struct_eq(o)) return true;
			if (o.k == kind::not_ && o.inner->struct_eq(*this)) return true;
			// H_{w,b} and H_{-w,-b} are complements
			if (k == kind::halfspace && o.k == kind::halfspace)
				return hs == o.hs.negate();
			return false;
		}

		std::string to_string() const {
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
	};
	using node_ptr = std::shared_ptr<node>;

	node_ptr root;

	hsb() : root(mk_bot()) {}
	explicit hsb(node_ptr r) : root(std::move(r)) {}

	// --- node factories ---
	static node_ptr mk_bot() {
		auto p = std::make_shared<node>();
		p->k = kind::bot;
		return p;
	}
	static node_ptr mk_top() {
		auto p = std::make_shared<node>();
		p->k = kind::top;
		return p;
	}
	static node_ptr mk_hs(hsb_halfspace h) {
		auto p = std::make_shared<node>();
		p->k = kind::halfspace;
		p->hs = std::move(h);
		return p;
	}
	static node_ptr mk_and(node_ptr l, node_ptr r) {
		auto p = std::make_shared<node>();
		p->k = kind::and_;
		p->lhs = std::move(l);
		p->rhs = std::move(r);
		return p;
	}
	static node_ptr mk_or(node_ptr l, node_ptr r) {
		auto p = std::make_shared<node>();
		p->k = kind::or_;
		p->lhs = std::move(l);
		p->rhs = std::move(r);
		return p;
	}
	static node_ptr mk_not(node_ptr i) {
		auto p = std::make_shared<node>();
		p->k = kind::not_;
		p->inner = std::move(i);
		return p;
	}

	// --- element factories ---
	static hsb bottom() { return hsb{mk_bot()}; }
	static hsb top()    { return hsb{mk_top()}; }

	// Construct from a single halfspace
	static hsb from_halfspace(hsb_halfspace h) {
		return hsb{mk_hs(std::move(h))};
	}

	// --- BA operations with structural simplifications ---

	hsb operator&(const hsb& o) const {
		// absorption / identity / complement
		if (root->k == kind::bot || o.root->k == kind::bot) return bottom();
		if (root->k == kind::top) return o;
		if (o.root->k == kind::top) return *this;
		if (root->struct_eq(*o.root)) return *this;           // A & A = A
		if (root->is_complement_of(*o.root)) return bottom(); // A & ~A = bot
		return hsb{mk_and(root, o.root)};
	}

	hsb operator|(const hsb& o) const {
		if (root->k == kind::bot) return o;
		if (o.root->k == kind::bot) return *this;
		if (root->k == kind::top || o.root->k == kind::top) return top();
		if (root->struct_eq(*o.root)) return *this;           // A | A = A
		if (root->is_complement_of(*o.root)) return top();    // A | ~A = top
		return hsb{mk_or(root, o.root)};
	}

	hsb operator~() const {
		if (root->k == kind::bot) return top();
		if (root->k == kind::top) return bottom();
		// ~~A = A
		if (root->k == kind::not_) return hsb{root->inner};
		// For a single halfspace: complement is the negated halfspace
		if (root->k == kind::halfspace)
			return hsb{mk_hs(root->hs.negate())};
		// General case: wrap in not_
		return hsb{mk_not(root)};
	}

	hsb operator^(const hsb& o) const {
		return (*this | o) & ~(*this & o);
	}

	// --- comparison ---
	bool operator==(const hsb& o) const {
		return root->struct_eq(*o.root);
	}
	bool operator!=(const hsb& o) const { return !(*this == o); }
	bool operator==(bool b) const {
		return b ? (root->k == kind::top) : (root->k == kind::bot);
	}
	bool operator!=(bool b) const { return !(*this == b); }

	bool operator<(const hsb& o) const {
		if (*this == o) return false;
		return to_string() < o.to_string();
	}
	auto operator<=>(const hsb& o) const {
		if (*this == o) return std::strong_ordering::equal;
		return to_string() < o.to_string()
			? std::strong_ordering::less
			: std::strong_ordering::greater;
	}

	std::string to_string() const {
		return root ? root->to_string() : "bot";
	}

	static hsb from_string(const std::string& s) {
		std::string t = s;
		// trim
		t.erase(0, t.find_first_not_of(" \t\n\r"));
		auto last = t.find_last_not_of(" \t\n\r");
		if (last != std::string::npos) t = t.substr(0, last + 1);

		if (t == "top") return top();
		if (t == "bot" || t == "bottom") return bottom();
		return bottom(); // fallback for unparseable
	}
};

// --- stream output ---
inline std::ostream& operator<<(std::ostream& os, const hsb& h) {
	return os << h.to_string();
}

// -----------------------------------------------------------------------------
// LP feasibility checker (Fourier-Motzkin elimination for small d)
// Used by is_hsb_zero to check emptiness of conjunctions of halfspaces.
// -----------------------------------------------------------------------------

namespace detail {

struct linear_constraint {
	std::vector<double> w;
	double b;
	bool strict; // true => < 0, false => <= 0
};

// Collect all halfspace constraints from a conjunction (AND-tree).
// Returns false if the node is structurally unsatisfiable (contains bot).
inline bool collect_conjunction(const hsb::node_ptr& n,
                                std::vector<linear_constraint>& out) {
	if (!n) return false;
	switch (n->k) {
	case hsb::kind::bot:       return false;
	case hsb::kind::top:       return true;
	case hsb::kind::halfspace: {
		out.push_back({n->hs.w, n->hs.b, n->hs.is_strict()});
		return true;
	}
	case hsb::kind::and_:
		return collect_conjunction(n->lhs, out)
		    && collect_conjunction(n->rhs, out);
	default:
		// or_ / not_ should not appear in a conjunction
		return true; // conservatively treat as satisfiable
	}
}

// Check LP feasibility of a conjunction of linear constraints using cvc5 QF_LRA.
// Returns true if the system is feasible.
inline bool lra_feasible(const std::vector<linear_constraint>& cs, size_t dim) {
	if (cs.empty()) return true;

	cvc5::Solver solver(cvc5_term_manager);
	solver.setOption("produce-models", "true");
	solver.setLogic("QF_LRA");

	// Create real-valued variables x[0], ..., x[dim-1]
	std::vector<cvc5::Term> vars(dim);
	for (size_t i = 0; i < dim; ++i)
		vars[i] = cvc5_term_manager.mkConst(
			cvc5_term_manager.getRealSort(),
			"x" + std::to_string(i));

	auto mk_rational = [](double v) -> cvc5::Term {
		// Convert double to fixed-point string (avoids scientific notation
		// which cvc5::mkReal does not accept)
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(20) << v;
		return cvc5_term_manager.mkReal(oss.str());
	};

	for (auto& c : cs) {
		// Build <w, x> + b
		cvc5::Term sum = mk_rational(c.b);
		for (size_t i = 0; i < std::min(c.w.size(), dim); ++i) {
			if (hsb_detail::feq(c.w[i], 0.0)) continue;
			cvc5::Term coeff = mk_rational(c.w[i]);
			cvc5::Term prod = cvc5_term_manager.mkTerm(
				cvc5::Kind::MULT, {coeff, vars[i]});
			sum = cvc5_term_manager.mkTerm(cvc5::Kind::ADD, {sum, prod});
		}

		cvc5::Term zero = cvc5_term_manager.mkReal(0);
		cvc5::Term constraint;
		if (c.strict)
			constraint = cvc5_term_manager.mkTerm(cvc5::Kind::LT, {sum, zero});
		else
			constraint = cvc5_term_manager.mkTerm(cvc5::Kind::LEQ, {sum, zero});

		solver.assertFormula(constraint);
	}

	return solver.checkSat().isSat();
}

// Find a feasible point for a conjunction of constraints using cvc5 QF_LRA
// model extraction. Returns nullopt if infeasible.
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
			cvc5::Term coeff = mk_rational(c.w[i]);
			cvc5::Term prod = cvc5_term_manager.mkTerm(
				cvc5::Kind::MULT, {coeff, vars[i]});
			sum = cvc5_term_manager.mkTerm(cvc5::Kind::ADD, {sum, prod});
		}
		cvc5::Term zero = cvc5_term_manager.mkReal(0);
		cvc5::Term constraint;
		if (c.strict)
			constraint = cvc5_term_manager.mkTerm(cvc5::Kind::LT, {sum, zero});
		else
			constraint = cvc5_term_manager.mkTerm(cvc5::Kind::LEQ, {sum, zero});
		solver.assertFormula(constraint);
	}

	if (!solver.checkSat().isSat())
		return std::nullopt;

	std::vector<double> pt(dim);
	for (size_t i = 0; i < dim; ++i) {
		cvc5::Term val = solver.getValue(vars[i]);
		std::string vs = val.getRealValue();
		auto slash = vs.find('/');
		if (slash != std::string::npos)
			pt[i] = std::stod(vs.substr(0, slash))
			      / std::stod(vs.substr(slash + 1));
		else
			pt[i] = std::stod(vs);
	}
	return pt;
}

// Forward declaration
inline hsb::node_ptr push_neg(const hsb::node_ptr& n);

// Convert a formula node to DNF (vector of conjunctions), where each
// conjunction is a vector of halfspace-level nodes.
inline void to_dnf(const hsb::node_ptr& n,
                    std::vector<std::vector<hsb::node_ptr>>& dnf) {
	if (!n) { dnf.clear(); return; }
	switch (n->k) {
	case hsb::kind::bot:
		// empty — contributes nothing to DNF
		break;
	case hsb::kind::top:
		dnf.push_back({});  // empty conjunction = true
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
	case hsb::kind::not_: {
		// Push negation down to literals using De Morgan's laws
		auto neg = push_neg(n->inner);
		to_dnf(neg, dnf);
		break;
	}
	}
}

// Push negation down to halfspace leaves (NNF conversion)
inline hsb::node_ptr push_neg(const hsb::node_ptr& n) {
	if (!n) return hsb::mk_bot();
	switch (n->k) {
	case hsb::kind::bot: return hsb::mk_top();
	case hsb::kind::top: return hsb::mk_bot();
	case hsb::kind::halfspace:
		return hsb::mk_hs(n->hs.negate());
	case hsb::kind::not_:
		return n->inner;  // ~~A = A
	case hsb::kind::and_:
		return hsb::mk_or(push_neg(n->lhs), push_neg(n->rhs));
	case hsb::kind::or_:
		return hsb::mk_and(push_neg(n->lhs), push_neg(n->rhs));
	}
	return hsb::mk_bot();
}

// Convert to NNF (negation normal form) — push all negations to leaves
inline hsb::node_ptr to_nnf(const hsb::node_ptr& n) {
	if (!n) return hsb::mk_bot();
	switch (n->k) {
	case hsb::kind::bot:
	case hsb::kind::top:
	case hsb::kind::halfspace:
		return n;
	case hsb::kind::and_:
		return hsb::mk_and(to_nnf(n->lhs), to_nnf(n->rhs));
	case hsb::kind::or_:
		return hsb::mk_or(to_nnf(n->lhs), to_nnf(n->rhs));
	case hsb::kind::not_:
		return push_neg(n->inner);
	}
	return hsb::mk_bot();
}

// Infer dimension from the formula tree
inline size_t infer_dim(const hsb::node_ptr& n) {
	if (!n) return 0;
	switch (n->k) {
	case hsb::kind::halfspace:
		return n->hs.dimension();
	case hsb::kind::and_:
	case hsb::kind::or_:
		return std::max(infer_dim(n->lhs), infer_dim(n->rhs));
	case hsb::kind::not_:
		return infer_dim(n->inner);
	default:
		return 0;
	}
}

// Extract first leaf halfspace node from the formula tree
inline hsb::node_ptr find_leaf_halfspace(const hsb::node_ptr& n) {
	if (!n) return nullptr;
	switch (n->k) {
	case hsb::kind::halfspace: return n;
	case hsb::kind::and_:
	case hsb::kind::or_: {
		auto l = find_leaf_halfspace(n->lhs);
		return l ? l : find_leaf_halfspace(n->rhs);
	}
	case hsb::kind::not_:
		return find_leaf_halfspace(n->inner);
	default:
		return nullptr;
	}
}

} // namespace detail

// --- free functions for dispatcher ---

inline bool is_hsb_zero(const hsb& x) {
	if (x.root->k == hsb::kind::bot) return true;
	if (x.root->k == hsb::kind::top) return false;

	// Convert to NNF, then to DNF and check each disjunct
	auto nnf = detail::to_nnf(x.root);
	std::vector<std::vector<hsb::node_ptr>> dnf;
	detail::to_dnf(nnf, dnf);

	if (dnf.empty()) return true;

	size_t dim = detail::infer_dim(x.root);
	if (dim == 0) dim = 1;

	for (auto& conj : dnf) {
		std::vector<detail::linear_constraint> cs;
		bool ok = true;
		for (auto& nd : conj) {
			if (!detail::collect_conjunction(nd, cs)) { ok = false; break; }
		}
		if (!ok) continue;
		if (detail::lra_feasible(cs, dim)) return false;
	}
	return true;
}

inline bool is_hsb_one(const hsb& x) {
	return is_hsb_zero(~x);
}

inline hsb normalize_hsb(const hsb& x) {
	if (x.root->k == hsb::kind::bot || x.root->k == hsb::kind::top)
		return x;
	if (is_hsb_zero(x)) return hsb::bottom();
	if (is_hsb_one(x))  return hsb::top();
	return x;
}

inline tref simplify_hsb_symbol(tref sym) { return sym; }
inline tref simplify_hsb_term(tref t) { return t; }

// Splitter: given non-zero x, find y with bot < y < x.
// 1. Convert to DNF, find a feasible point p in the first satisfiable cell
// 2. Return x & H_{e_1, -p_1} where H_{e_1,-p_1} = {x_1 < p_1}
inline hsb hsb_splitter(const hsb& x, splitter_type /*st*/) {
	if (x.root->k == hsb::kind::bot) return hsb::bottom();
	if (x.root->k == hsb::kind::top) {
		// Split top at x_1 = 0: return {x_1 < 0}
		hsb_halfspace h;
		h.w = {1.0};
		h.b = 0.0;
		return hsb::from_halfspace(h);
	}

	size_t dim = detail::infer_dim(x.root);
	if (dim == 0) dim = 1;

	// Find a feasible point via DNF
	auto nnf = detail::to_nnf(x.root);
	std::vector<std::vector<hsb::node_ptr>> dnf;
	detail::to_dnf(nnf, dnf);

	for (auto& conj : dnf) {
		std::vector<detail::linear_constraint> cs;
		bool ok = true;
		for (auto& nd : conj) {
			if (!detail::collect_conjunction(nd, cs)) { ok = false; break; }
		}
		if (!ok) continue;
		auto pt = detail::find_feasible_point(cs, dim);
		if (pt) {
			// Split at x_1 = p_1: H_{e_1, -p_1} = {x_1 - p_1 < 0} = {x_1 < p_1}
			// s(e_1) = +1 => strict (open), which is canonical
			hsb_halfspace h;
			h.w.assign(dim, 0.0);
			h.w[0] = 1.0;
			h.b = -(*pt)[0];

			hsb split = x & hsb::from_halfspace(h);
			if (!is_hsb_zero(split) && split != x)
				return split;

			// If splitting at p_1 didn't work, try the other side
			hsb_halfspace h_neg;
			h_neg.w.assign(dim, 0.0);
			h_neg.w[0] = -1.0;
			h_neg.b = (*pt)[0];
			split = x & hsb::from_halfspace(h_neg);
			if (!is_hsb_zero(split) && split != x)
				return split;
		}
	}
	// Fallback: for a single halfspace, split at the midpoint of the
	// feasible direction. For compound formulas, try the leaf approach.
	if (x.root->k == hsb::kind::halfspace) {
		// Single halfspace H_{w,b}: split using a perpendicular cut
		// Create H_{e_1, 0} = {x_1 < 0} as splitter
		hsb_halfspace h_split;
		h_split.w.assign(dim, 0.0);
		h_split.w[0] = 1.0;
		h_split.b = 0.0;
		hsb sub = x & hsb::from_halfspace(h_split);
		if (!is_hsb_zero(sub) && sub != x) return sub;
		// Try the other side
		hsb_halfspace h_split_neg;
		h_split_neg.w.assign(dim, 0.0);
		h_split_neg.w[0] = -1.0;
		h_split_neg.b = 0.0;
		sub = x & hsb::from_halfspace(h_split_neg);
		if (!is_hsb_zero(sub) && sub != x) return sub;
	} else {
		auto leaf = detail::find_leaf_halfspace(x.root);
		if (leaf) {
			hsb sub = x & hsb{leaf};
			if (!is_hsb_zero(sub) && sub != x) return sub;
		}
	}
	return x;
}

// A fixed non-trivial element: {x_1 < 0} (s(e_1)=+1 => strict)
inline hsb hsb_splitter_one() {
	hsb_halfspace h;
	h.w = {1.0};
	h.b = 0.0; // x_1 + 0 < 0 i.e. x_1 < 0
	return hsb::from_halfspace(h);
}

// --- parsing ---
template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type>
parse_hsb(const std::string& src) {
	std::string s = src;
	// trim
	s.erase(0, s.find_first_not_of(" \t\n\r"));
	auto last = s.find_last_not_of(" \t\n\r");
	if (last != std::string::npos) s = s.substr(0, last + 1);

	// strip outer braces if present
	if (!s.empty() && s.front() == '{' && s.back() == '}')
		s = s.substr(1, s.size() - 2);
	s.erase(0, s.find_first_not_of(" \t\n\r"));
	last = s.find_last_not_of(" \t\n\r");
	if (last != std::string::npos) s = s.substr(0, last + 1);

	if (s == "top") {
		return typename node<BAs...>::constant_with_type{
			std::variant<BAs...>{ hsb::top() },
			hsb_type<node<BAs...>>() };
	}
	if (s == "bot" || s == "bottom") {
		return typename node<BAs...>::constant_with_type{
			std::variant<BAs...>{ hsb::bottom() },
			hsb_type<node<BAs...>>() };
	}

	// Parse conjunction of halfspace constraints separated by " & "
	// Each constraint: x[i]*coeff + ... + bias {<, <=} 0
	hsb result = hsb::top();
	std::vector<std::string> parts;
	{
		std::string cur;
		for (size_t i = 0; i < s.size(); ++i) {
			if (s[i] == '&') {
				parts.push_back(cur);
				cur.clear();
			} else {
				cur += s[i];
			}
		}
		parts.push_back(cur);
	}

	auto parse_one_constraint = [](const std::string& part)
		-> std::optional<hsb_halfspace>
	{
		// Extract LHS expression (strict/non-strict determined by lex-leading sign)
		std::string expr;
		auto leq = part.find("<=");
		if (leq != std::string::npos) {
			expr = part.substr(0, leq);
		} else {
			auto lt = part.find('<');
			if (lt != std::string::npos) {
				expr = part.substr(0, lt);
			} else {
				return std::nullopt;
			}
		}
		// Trim expression
		while (!expr.empty() && expr.back() == ' ') expr.pop_back();

		// Parse linear expression: sum of "coeff*x[i]" or "x[i]*coeff"
		// or plain "x[i]" or numeric constants (bias).
		// Tokens separated by + or - (as binary operators).
		std::map<size_t, double> coeffs;
		double bias = 0.0;

		// Normalize: replace " - " with " + -" and "(" with ""
		std::string norm;
		for (size_t i = 0; i < expr.size(); ++i) {
			if (expr[i] == '(' || expr[i] == ')') continue;
			norm += expr[i];
		}

		// Split on '+' respecting signs
		std::vector<std::string> tokens;
		{
			std::string cur;
			for (size_t i = 0; i < norm.size(); ++i) {
				if (norm[i] == '+' || (norm[i] == '-' && i > 0
					&& norm[i-1] != '*' && norm[i-1] != 'e'
					&& norm[i-1] != 'E'))
				{
					if (!cur.empty()) tokens.push_back(cur);
					cur.clear();
					if (norm[i] == '-') cur = "-";
				} else {
					cur += norm[i];
				}
			}
			if (!cur.empty()) tokens.push_back(cur);
		}

		for (auto& tok : tokens) {
			// Trim
			while (!tok.empty() && tok.front() == ' ') tok.erase(0,1);
			while (!tok.empty() && tok.back() == ' ') tok.pop_back();
			if (tok.empty()) continue;

			// Check if token contains x[...]
			auto xpos = tok.find("x[");
			if (xpos != std::string::npos) {
				auto bracket_end = tok.find(']', xpos);
				if (bracket_end == std::string::npos) continue;
				size_t idx = static_cast<size_t>(std::stoi(
					tok.substr(xpos + 2, bracket_end - xpos - 2)));
				// Find coefficient
				double coeff = 1.0;
				auto star = tok.find('*');
				if (star != std::string::npos) {
					if (star < xpos) {
						// coeff*x[i]
						std::string cs = tok.substr(0, star);
						while (!cs.empty() && cs.back() == ' ') cs.pop_back();
						while (!cs.empty() && cs.front() == ' ') cs.erase(0,1);
						if (cs == "-") coeff = -1.0;
						else if (cs == "+" || cs.empty()) coeff = 1.0;
						else coeff = std::stod(cs);
					} else {
						// x[i]*coeff
						std::string cs = tok.substr(star + 1);
						while (!cs.empty() && cs.front() == ' ') cs.erase(0,1);
						while (!cs.empty() && cs.back() == ' ') cs.pop_back();
						if (cs == "-") coeff = -1.0;
						else if (cs == "+" || cs.empty()) coeff = 1.0;
						else coeff = std::stod(cs);
					}
					// Check for leading minus before the term
					if (xpos > 0 && star > xpos) {
						// coeff is after x[i], sign might be leading
					}
				} else {
					// Just x[i] with possible sign
					std::string pre = tok.substr(0, xpos);
					while (!pre.empty() && pre.back() == ' ') pre.pop_back();
					if (pre == "-") coeff = -1.0;
				}
				coeffs[idx] += coeff;
			} else {
				// Pure numeric constant (bias term)
				bias += std::stod(tok);
			}
		}

		// Build the halfspace
		size_t dim = 1;
		for (auto& [idx, _] : coeffs)
			if (idx + 1 > dim) dim = idx + 1;

		hsb_halfspace h;
		h.w.resize(dim, 0.0);
		for (auto& [idx, c] : coeffs)
			h.w[idx] = c;
		h.b = bias;
		return h;
	};

	for (auto& part : parts) {
		part.erase(0, part.find_first_not_of(" \t\n\r"));
		auto lp = part.find_last_not_of(" \t\n\r");
		if (lp != std::string::npos) part = part.substr(0, lp + 1);
		if (part.empty()) continue;

		auto maybe_h = parse_one_constraint(part);
		if (!maybe_h) continue;
		result = result & hsb::from_halfspace(*maybe_h);
	}

	return typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ result },
		hsb_type<node<BAs...>>() };
}

} // namespace idni::tau_lang

// Hash specialization for hsb
template<>
struct std::hash<idni::tau_lang::hsb> {
	size_t operator()(const idni::tau_lang::hsb& h) const noexcept {
		return std::hash<std::string>{}(h.to_string());
	}
};

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_H__
