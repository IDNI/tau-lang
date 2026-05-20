// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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

// =============================================================================
// Canonical decomposition helpers (Phase 1 + Phase 2)
// =============================================================================

/// Recursively clone the formula tree, normalizing every halfspace leaf.
inline hsb::node_ptr normalize_all_leaves(const hsb::node_ptr& n) {
	if (!n) return hsb::mk_bot();
	switch (n->k) {
	case hsb::kind::bot:
	case hsb::kind::top:      return n;
	case hsb::kind::halfspace: return hsb::mk_hs(n->hs.normalize());
	case hsb::kind::not_:      return hsb::mk_not(normalize_all_leaves(n->inner));
	case hsb::kind::and_:      return hsb::mk_and(normalize_all_leaves(n->lhs),
	                                               normalize_all_leaves(n->rhs));
	case hsb::kind::or_:       return hsb::mk_or(normalize_all_leaves(n->lhs),
	                                              normalize_all_leaves(n->rhs));
	}
	return hsb::mk_bot();
}

/// Boundary expression for x[var_idx] derived from a normalized halfspace.
struct boundary {
	std::vector<double> bw; ///< coefficients for x[0]..x[var_idx-1]
	double bb = 0.0;        ///< constant term
	bool upper = true;      ///< true = upper bound (h.w[i]>0), false = lower bound
};

/// Build the boundary for halfspace @p h at variable @p i (|h.w[i]| must equal 1).
inline boundary make_boundary(const hsb_halfspace& h, size_t i) {
	boundary bd;
	bd.bw.assign(i, 0.0);
	if (h.w[i] > 0.0) { // upper: x[i] ≤ B  →  B = -Σ_{k<i} h.w[k]*x[k] - h.b
		bd.upper = true;
		for (size_t k = 0; k < i; ++k) bd.bw[k] = -h.w[k];
		bd.bb = -h.b;
	} else {             // lower: x[i] ≥ B  →  B = +Σ_{k<i} h.w[k]*x[k] + h.b
		bd.upper = false;
		for (size_t k = 0; k < i; ++k) bd.bw[k] = +h.w[k];
		bd.bb = +h.b;
	}
	return bd;
}

/// Lexicographic less-than on boundary (bw, bb) — ignores upper/lower flag.
inline bool boundary_lt(const boundary& a, const boundary& b_) {
	size_t n = std::max(a.bw.size(), b_.bw.size());
	for (size_t k = 0; k < n; ++k) {
		double lv = k < a.bw.size()  ? a.bw[k]  : 0.0;
		double rv = k < b_.bw.size() ? b_.bw[k] : 0.0;
		if (hsb_detail::feq(lv, rv)) continue;
		return lv < rv;
	}
	if (hsb_detail::feq(a.bb, b_.bb)) return false;
	return a.bb < b_.bb;
}

/// Two boundaries represent the same hyperplane iff (bw, bb) are feq-equal.
inline bool boundary_eq(const boundary& a, const boundary& b_) {
	if (a.bw.size() != b_.bw.size()) {
		// allow different sizes if trailing entries are all zero
		size_t n = std::max(a.bw.size(), b_.bw.size());
		for (size_t k = 0; k < n; ++k) {
			double lv = k < a.bw.size()  ? a.bw[k]  : 0.0;
			double rv = k < b_.bw.size() ? b_.bw[k] : 0.0;
			if (!hsb_detail::feq(lv, rv)) return false;
		}
		return hsb_detail::feq(a.bb, b_.bb);
	}
	for (size_t k = 0; k < a.bw.size(); ++k)
		if (!hsb_detail::feq(a.bw[k], b_.bw[k])) return false;
	return hsb_detail::feq(a.bb, b_.bb);
}

/// Build the linear_constraint encoding an interval bound on x[i].
/// upper=true  → x[i] ≤ B  (w'[i]=+1, w'[k]=-bw[k], b'=-bb, strict=false)
/// upper=false → x[i] > B  (w'[i]=-1, w'[k]=+bw[k], b'=+bb, strict=true)
inline linear_constraint make_interval_constraint(
	const boundary& bd, size_t i, size_t dim, bool upper)
{
	linear_constraint lc;
	lc.w.assign(dim, 0.0);
	lc.strict = !upper; // lower bound uses strict >
	if (upper) {
		lc.w[i] = +1.0;
		for (size_t k = 0; k < i && k < bd.bw.size(); ++k) lc.w[k] = -bd.bw[k];
		lc.b = -bd.bb;
	} else {
		lc.w[i] = -1.0;
		for (size_t k = 0; k < i && k < bd.bw.size(); ++k) lc.w[k] = +bd.bw[k];
		lc.b = +bd.bb;
	}
	return lc;
}

/// Reconstruct an hsb value from a list of clauses (each = conjunction of linear_constraints).
inline hsb reconstruct_hsb(
	const std::vector<std::vector<linear_constraint>>& clauses, size_t dim)
{
	hsb result = hsb::bottom();
	for (auto& clause : clauses) {
		if (clause.empty()) { return hsb::top(); } // empty conjunction = tautology
		hsb conj = hsb::top();
		for (auto& lc : clause) {
			hsb_halfspace h;
			h.w = lc.w;
			if (h.w.size() < dim) h.w.resize(dim, 0.0);
			h.b = lc.b;
			// Ensure lex-leading sign matches strict flag
			// (make_interval_constraint already encodes this correctly)
			conj = conj & hsb::from_halfspace(std::move(h));
		}
		result = result | conj;
	}
	return result;
}

/// Core recursive canonical decomposition over the DNF.
/// @p dnf    Current disjuncts (each is a conjunction of normalized halfspaces).
/// @p var_idx  Variable dimension being processed (decreasing from dim-1 to 0).
/// @p dim    Ambient dimension (fixed).
/// @p path   Accumulated interval constraints for the current branch.
/// @p out    Output clauses (one per leaf path that is LP-feasible).
inline void decompose(
	const std::vector<std::vector<hsb_halfspace>>& dnf,
	int var_idx,
	size_t dim,
	std::vector<linear_constraint>& path,
	std::vector<std::vector<linear_constraint>>& out)
{
	// Leaf: all variables processed.
	if (var_idx < 0) {
		if (lra_feasible(path, dim)) out.push_back(path);
		return;
	}
	size_t i = static_cast<size_t>(var_idx);

	// ── Step 1: collect and sort unique boundaries for x[i] ─────────────────
	std::vector<boundary> bds;
	for (auto& conj : dnf) {
		for (auto& h : conj) {
			if (i >= h.w.size() || hsb_detail::feq(h.w[i], 0.0)) continue;
			boundary bd = make_boundary(h, i);
			// Insert only if not already present
			bool found = false;
			for (auto& e : bds) if (boundary_eq(e, bd)) { found = true; break; }
			if (!found) bds.push_back(std::move(bd));
		}
	}
	std::sort(bds.begin(), bds.end(), boundary_lt);
	size_t m = bds.size();

	// Base case: no constraints on x[i] in any disjunct — skip this variable.
	if (m == 0) {
		decompose(dnf, var_idx - 1, dim, path, out);
		return;
	}

	// ── Step 2: iterate over the m+1 intervals ───────────────────────────────
	// Build a map from (bw,bb) → 1-based lex position for fast lookup.
	// position k means the boundary is B_k (1-indexed).
	// Use index in bds + 1 as the lex position (after sorting).
	auto lex_pos = [&](const hsb_halfspace& h) -> size_t {
		boundary bd = make_boundary(h, i);
		for (size_t p = 0; p < m; ++p)
			if (boundary_eq(bds[p], bd)) return p + 1; // 1-indexed
		return 0; // should not happen
	};

	for (size_t j = 1; j <= m + 1; ++j) {
		// Push interval constraints onto path.
		size_t path_before = path.size();
		if (j == 1) {
			// I_1: x[i] ≤ B_1
			path.push_back(make_interval_constraint(bds[0], i, dim, /*upper=*/true));
		} else if (j == m + 1) {
			// I_{m+1}: x[i] > B_m
			path.push_back(make_interval_constraint(bds[m-1], i, dim, /*upper=*/false));
		} else {
			// I_j (2..m): B_{j-1} < x[i] ≤ B_j
			path.push_back(make_interval_constraint(bds[j-2], i, dim, /*upper=*/false));
			path.push_back(make_interval_constraint(bds[j-1], i, dim, /*upper=*/true));
		}

		// Build dnf_j by filtering each conjunction.
		std::vector<std::vector<hsb_halfspace>> dnf_j;
		bool any_surviving = false;
		for (auto& conj : dnf) {
			std::vector<hsb_halfspace> conj_j;
			bool violated = false;
			for (auto& h : conj) {
				if (i >= h.w.size() || hsb_detail::feq(h.w[i], 0.0)) {
					// Constraint does not involve x[i] — carry forward.
					conj_j.push_back(h);
					continue;
				}
				size_t k = lex_pos(h);
				if (h.w[i] > 0.0) { // upper bound: x[i] ≤ B_k
					if (j <= k && j <= m) {
						// Satisfied: interval upper ≤ B_k, drop.
					} else {
						// Violated (j > k, or j == m+1).
						violated = true; break;
					}
				} else { // lower bound: x[i] > B_k (or ≥)
					if (k < j) {
						// Satisfied: B_k ≤ interval lower, drop.
					} else {
						// Violated (k >= j).
						violated = true; break;
					}
				}
			}
			if (!violated) {
				dnf_j.push_back(std::move(conj_j));
				any_surviving = true;
			}
		}

		if (!any_surviving) {
			// Prune: no disjunct survives in this interval.
			path.resize(path_before);
			continue;
		}

		// Check if all surviving conjunctions are empty (tautology under path).
		bool all_empty = true;
		for (auto& c : dnf_j) if (!c.empty()) { all_empty = false; break; }

		if (var_idx == 0 || all_empty) {
			// Leaf or tautology: record if LP-feasible.
			if (lra_feasible(path, dim)) out.push_back(path);
		} else {
			decompose(dnf_j, var_idx - 1, dim, path, out);
		}

		path.resize(path_before);
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
 * @brief Canonical decomposition of @p x into a unique disjunction of
 *        disjoint convex polyhedra (lex-half-open intervals, d dimensions).
 *
 * Implements the two-phase algorithm from the canonical_decomposition design doc:
 *   Phase 1 — boundary normalization (divide by |coeff of highest-indexed var|).
 *   Phase 2 — recursive interval slicing (highest variable down to lowest).
 *
 * Two semantically equal formulas always produce the same output.
 *
 * @param x  The hsb element to normalise.
 */
inline hsb normalize_hsb(const hsb& x) {
	// Fast paths: structural constants and semantic collapse.
	if (x.root->k == hsb::kind::bot || x.root->k == hsb::kind::top)
		return x;
	if (is_hsb_zero(x)) return hsb::bottom();
	if (is_hsb_one(x))  return hsb::top();

	// Phase 1: NNF, then normalize every halfspace leaf.
	auto norm = detail::normalize_all_leaves(detail::to_nnf(x.root));

	size_t dim = detail::infer_dim(norm);
	if (dim == 0) return x; // degenerate (no halfspaces)

	// Convert to DNF and extract halfspace lists per disjunct.
	std::vector<std::vector<hsb::node_ptr>> raw_dnf;
	detail::to_dnf(norm, raw_dnf);

	std::vector<std::vector<hsb_halfspace>> dnf;
	dnf.reserve(raw_dnf.size());
	for (auto& conj : raw_dnf) {
		std::vector<hsb_halfspace> hs_conj;
		hs_conj.reserve(conj.size());
		for (auto& nd : conj)
			if (nd->k == hsb::kind::halfspace) hs_conj.push_back(nd->hs);
		dnf.push_back(std::move(hs_conj));
	}

	// Phase 2: recursive interval slicing.
	std::vector<detail::linear_constraint> path;
	std::vector<std::vector<detail::linear_constraint>> clauses;
	detail::decompose(dnf, static_cast<int>(dim) - 1, dim, path, clauses);

	if (clauses.empty()) return hsb::bottom();

	return detail::reconstruct_hsb(clauses, dim);
}

/// @brief No-op symbol simplifier (required by the tau dispatcher interface).
inline tref simplify_hsb_symbol(tref sym) { return sym; }

/// @brief No-op term simplifier (required by the tau dispatcher interface).
inline tref simplify_hsb_term(tref t) { return t; }

/**
 * @brief Returns a proper sub-element y with `bot < y < x`.
 *
 * Finds a feasible interior point p of @p x via DNF + cvc5, then returns
 * `x & H_{e_1, -p_1}` (the intersection with the splitting halfspace
 * `{x_1 < p_1}`).
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

		auto pt = detail::find_feasible_point(cs, dim);
		if (!pt) continue;

		// Try x & {x_0 < p_0}
		hsb_halfspace h;
		h.w.assign(dim, 0.0); h.w[0] = 1.0; h.b = -(*pt)[0];
		hsb split = x & hsb::from_halfspace(h);
		if (!is_hsb_zero(split) && split != x) return split;

		// Try the other side
		hsb_halfspace h_neg;
		h_neg.w.assign(dim, 0.0); h_neg.w[0] = -1.0; h_neg.b = (*pt)[0];
		split = x & hsb::from_halfspace(h_neg);
		if (!is_hsb_zero(split) && split != x) return split;
	}

	// Fallback: try splitting at 0 along the first axis
	if (x.root->k == hsb::kind::halfspace) {
		for (double sign : {1.0, -1.0}) {
			hsb_halfspace h;
			h.w.assign(dim, 0.0); h.w[0] = sign; h.b = 0.0;
			hsb sub = x & hsb::from_halfspace(h);
			if (!is_hsb_zero(sub) && sub != x) return sub;
		}
	} else {
		auto leaf = detail::find_leaf_halfspace(x.root);
		if (leaf) {
			hsb sub = x & hsb{leaf};
			if (!is_hsb_zero(sub) && sub != x) return sub;
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
