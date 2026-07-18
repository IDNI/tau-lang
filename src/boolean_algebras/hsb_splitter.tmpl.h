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
/// @pre Must only be called on individual DNF clauses (pure conjunctions of
///      halfspaces); or_/not_ nodes at the clause root indicate a logic error.
inline bool collect_conjunction(tref n,
                                std::vector<linear_constraint>& out) {
	if (!n) return false;
	auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
	switch (k) {
	case hsb::kind::bot:  return false;
	case hsb::kind::top:  return true;
	case hsb::kind::halfspace: {
		const auto& hs = hsb_halfspace_pool::get(hsb_tree::get(n).value.data);
		out.push_back({hs.w, hs.b, hs.is_strict()});
		return true;
	}
	case hsb::kind::and_:
		return collect_conjunction(hsb_tree::get(n).first(), out)
		    && collect_conjunction(hsb_tree::get(n).second(), out);
	default:
		// or_/not_ should not appear in a DNF clause; treat conservatively.
		return true;
	}
}

/// Set up a cvc5 QF_LRA solver, create real variables x0…x_{dim-1}, assert
/// all constraints in @p cs, and return the variable terms.
inline std::vector<cvc5::Term> setup_lra_solver(
		cvc5::Solver& solver,
		const std::vector<linear_constraint>& cs,
		size_t dim) {
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
	return vars;
}

/// Check LP feasibility of @p cs using cvc5 QF_LRA.
inline bool lra_feasible(const std::vector<linear_constraint>& cs, size_t dim) {
	if (cs.empty()) return true;
	cvc5::Solver solver(cvc5_term_manager);
	setup_lra_solver(solver, cs, dim);
	return solver.checkSat().isSat();
}

/// Find a feasible point for @p cs via cvc5 model extraction.
inline std::optional<std::vector<double>>
find_feasible_point(const std::vector<linear_constraint>& cs, size_t dim) {
	if (cs.empty())
		return std::vector<double>(dim, 0.0);

	cvc5::Solver solver(cvc5_term_manager);
	auto vars = setup_lra_solver(solver, cs, dim);

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

/// Find a feasible point additionally satisfying x[axis] > val or < val.
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
inline tref push_neg(tref n);

/// Convert a formula node to DNF (vector of conjunctions of halfspace nodes).
inline void to_dnf(tref n, std::vector<std::vector<tref>>& dnf) {
	if (!n) { dnf.clear(); return; }
	auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
	switch (k) {
	case hsb::kind::bot:
		break; // contributes nothing
	case hsb::kind::top:
		dnf.push_back({}); // empty conjunction = tautology
		break;
	case hsb::kind::halfspace:
		dnf.push_back({n});
		break;
	case hsb::kind::and_: {
		std::vector<std::vector<tref>> left, right;
		to_dnf(hsb_tree::get(n).first(), left);
		to_dnf(hsb_tree::get(n).second(), right);
		for (auto& l : left)
			for (auto& r : right) {
				auto conj = l;
				conj.insert(conj.end(), r.begin(), r.end());
				dnf.push_back(std::move(conj));
			}
		break;
	}
	case hsb::kind::or_:
		to_dnf(hsb_tree::get(n).first(), dnf);
		to_dnf(hsb_tree::get(n).second(), dnf);
		break;
	case hsb::kind::not_:
		to_dnf(push_neg(hsb_tree::get(n).first()), dnf);
		break;
	}
}

/// Push negation down to halfspace leaves (NNF step).
inline tref push_neg(tref n) {
	if (!n) return hsb::mk_bot();
	auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
	switch (k) {
	case hsb::kind::bot:       return hsb::mk_top();
	case hsb::kind::top:       return hsb::mk_bot();
	case hsb::kind::halfspace: {
		size_t idx = hsb_tree::get(n).value.data;
		return hsb::mk_hs_by_index(hsb_halfspace_pool::complement_index(idx));
	}
	case hsb::kind::not_:      return hsb_tree::get(n).first(); // ~~A = A
	case hsb::kind::and_:
		return hsb::mk_or(push_neg(hsb_tree::get(n).first()),
		                  push_neg(hsb_tree::get(n).second()));
	case hsb::kind::or_:
		return hsb::mk_and(push_neg(hsb_tree::get(n).first()),
		                   push_neg(hsb_tree::get(n).second()));
	}
	return hsb::mk_bot();
}

/// Convert to NNF using post_order — each not_ node has its child push_neg'd.
inline tref to_nnf(tref root) {
	auto f = [](tref n) -> tref {
		auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
		if (k != hsb::kind::not_) return n;
		return push_neg(hsb_tree::get(n).first());
	};
	return post_order<hsb_node>(root).apply_unique(f);
}

/// Infer ambient dimension from the formula tree.
inline size_t infer_dim(tref n) {
	if (!n) return 0;
	auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
	switch (k) {
	case hsb::kind::halfspace:
		return hsb_halfspace_pool::get(hsb_tree::get(n).value.data).dimension();
	case hsb::kind::and_:
	case hsb::kind::or_:
		return std::max(infer_dim(hsb_tree::get(n).first()),
		                infer_dim(hsb_tree::get(n).second()));
	case hsb::kind::not_:
		return infer_dim(hsb_tree::get(n).first());
	default: return 0;
	}
}

} // namespace detail

// =============================================================================
// Free function implementations
// =============================================================================

/**
 * @brief Returns true iff @p x is semantically equivalent to bottom (∅).
 */
inline bool is_hsb_zero(const hsb& x) {
	auto k = x.root_kind();
	if (k == hsb::kind::bot) return true;
	if (k == hsb::kind::top) return false;

	auto nnf = detail::to_nnf(x.root_ref());
	std::vector<std::vector<tref>> dnf;
	detail::to_dnf(nnf, dnf);

	if (dnf.empty()) return true;

	size_t dim = detail::infer_dim(x.root_ref());
	if (dim == 0) dim = 1;

	for (auto& conj : dnf) {
		std::vector<detail::linear_constraint> cs;
		bool ok = true;
		for (auto nd : conj)
			if (!detail::collect_conjunction(nd, cs)) { ok = false; break; }
		if (!ok) continue;
		if (detail::lra_feasible(cs, dim)) return false;
	}
	return true;
}

/**
 * @brief Returns true iff @p x is semantically equivalent to top (R^d).
 */
inline bool is_hsb_one(const hsb& x) {
	return is_hsb_zero(~x);
}

/// @brief No-op symbol simplifier (required by the tau dispatcher interface).
inline tref simplify_hsb_symbol(tref sym) { return sym; }

/// @brief No-op term simplifier (required by the tau dispatcher interface).
inline tref simplify_hsb_term(tref t) { return t; }

/**
 * @brief Returns a proper sub-element y with `bot < y < x`.
 */
inline hsb hsb_splitter(const hsb& x, splitter_type /*st*/) {
	auto k = x.root_kind();
	if (k == hsb::kind::bot) return hsb::bottom();
	if (k == hsb::kind::top) {
		hsb_halfspace h; h.w = {1.0}; h.b = 0.0;
		return hsb::from_halfspace(h);
	}

	size_t dim = detail::infer_dim(x.root_ref());
	if (dim == 0) dim = 1;

	auto nnf = detail::to_nnf(x.root_ref());
	std::vector<std::vector<tref>> dnf;
	detail::to_dnf(nnf, dnf);

	for (auto& conj : dnf) {
		std::vector<detail::linear_constraint> cs;
		bool ok = true;
		for (auto nd : conj)
			if (!detail::collect_conjunction(nd, cs)) { ok = false; break; }
		if (!ok) continue;

		auto pt_a = detail::find_feasible_point(cs, dim);
		if (!pt_a) continue;

		for (size_t axis = dim; axis-- > 0; ) {
			auto pt_b = detail::find_second_feasible_point(
				cs, dim, axis, (*pt_a)[axis], true);
			if (!pt_b)
				pt_b = detail::find_second_feasible_point(
					cs, dim, axis, (*pt_a)[axis], false);
			if (!pt_b) continue;

			double c = ((*pt_a)[axis] + (*pt_b)[axis]) / 2.0;
			hsb_halfspace h;
			h.w.assign(dim, 0.0); h.w[axis] = 1.0; h.b = -c;
			hsb hs   = hsb::from_halfspace(h);
			hsb lower = x & hs;
			hsb upper = x & ~hs;
			if (!is_hsb_zero(lower) && !is_hsb_zero(upper)) return lower;
		}
	}
	return x;
}

/**
 * @brief Returns a fixed non-trivial seed element `{x[0] < 0}`.
 */
inline hsb hsb_splitter_one() {
	hsb_halfspace h; h.w = {1.0}; h.b = 0.0;
	return hsb::from_halfspace(h);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_SPLITTER_TMPL_H__
