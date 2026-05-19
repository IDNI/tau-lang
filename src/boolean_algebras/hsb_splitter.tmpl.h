// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_SPLITTER_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_SPLITTER_TMPL_H__

#include "boolean_algebras/hsb.h"

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
				cs, dim, axis, (*pt_a)[axis], true);
			if (!pt_b)
				pt_b = detail::find_second_feasible_point(
					cs, dim, axis, (*pt_a)[axis], false);
			if (!pt_b) continue;

			double c = ((*pt_a)[axis] + (*pt_b)[axis]) / 2.0;
			// s = {x[axis] < c}: w[axis]=1.0, b=-c, s(w)=+1 (strict)
			hsb_halfspace h;
			h.w.assign(dim, 0.0); h.w[axis] = 1.0; h.b = -c;
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

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_SPLITTER_TMPL_H__
