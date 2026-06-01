// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_NORMALIZER_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_NORMALIZER_TMPL_H__

#include "boolean_algebras/hsb.h"

namespace idni::tau_lang {

// =============================================================================
// Canonical decomposition helpers (Phase 1 + Phase 2)
// — shared LRA infrastructure (linear_constraint, lra_feasible, …) comes from
//   hsb_splitter.tmpl.h which is included before this header.
// =============================================================================

namespace detail {

/// Clone the formula tree, normalizing every halfspace leaf (post_order).
inline tref normalize_all_leaves(tref root) {
	auto f = [](tref n) -> tref {
		auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
		if (k != hsb::kind::halfspace) return n;
		size_t idx = hsb_tree::get(n).value.data;
		const auto& h = hsb_halfspace_pool::get(idx);
		auto hn = h.normalize();
		if (hn == h) return n;
		return hsb::mk_hs(hn);
	};
	return post_order<hsb_node>(root).apply_unique(f);
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
	size_t n = std::max(a.bw.size(), b_.bw.size());
	for (size_t k = 0; k < n; ++k) {
		double lv = k < a.bw.size()  ? a.bw[k]  : 0.0;
		double rv = k < b_.bw.size() ? b_.bw[k] : 0.0;
		if (!hsb_detail::feq(lv, rv)) return false;
	}
	return hsb_detail::feq(a.bb, b_.bb);
}

/// Build the linear_constraint encoding an interval bound on x[i].
inline linear_constraint make_interval_constraint(
	const boundary& bd, size_t i, size_t dim, bool upper)
{
	linear_constraint lc;
	lc.w.assign(dim, 0.0);
	lc.strict = !upper;
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
		if (clause.empty()) { return hsb::top(); }
		hsb conj = hsb::top();
		for (auto& lc : clause) {
			hsb_halfspace h;
			h.w = lc.w;
			if (h.w.size() < dim) h.w.resize(dim, 0.0);
			h.b = lc.b;
			conj = conj & hsb::from_halfspace(h);
		}
		result = result | conj;
	}
	return result;
}

/// Core recursive canonical decomposition over the DNF.
inline void decompose(
	const std::vector<std::vector<hsb_halfspace>>& dnf,
	int var_idx,
	size_t dim,
	std::vector<linear_constraint>& path,
	std::vector<std::vector<linear_constraint>>& out)
{
	if (var_idx < 0) {
		if (lra_feasible(path, dim)) out.push_back(path);
		return;
	}
	size_t i = static_cast<size_t>(var_idx);

	std::vector<boundary> bds;
	for (auto& conj : dnf) {
		for (auto& h : conj) {
			if (i >= h.w.size() || hsb_detail::feq(h.w[i], 0.0)) continue;
			boundary bd = make_boundary(h, i);
			bool found = false;
			for (auto& e : bds) if (boundary_eq(e, bd)) { found = true; break; }
			if (!found) bds.push_back(std::move(bd));
		}
	}
	std::sort(bds.begin(), bds.end(), boundary_lt);
	size_t m = bds.size();

	if (m == 0) {
		decompose(dnf, var_idx - 1, dim, path, out);
		return;
	}

	auto lex_pos = [&](const hsb_halfspace& h) -> size_t {
		boundary bd = make_boundary(h, i);
		for (size_t p = 0; p < m; ++p)
			if (boundary_eq(bds[p], bd)) return p + 1;
		return 0;
	};

	for (size_t j = 1; j <= m + 1; ++j) {
		size_t path_before = path.size();
		if (j == 1) {
			path.push_back(make_interval_constraint(bds[0], i, dim, true));
		} else if (j == m + 1) {
			path.push_back(make_interval_constraint(bds[m-1], i, dim, false));
		} else {
			path.push_back(make_interval_constraint(bds[j-2], i, dim, false));
			path.push_back(make_interval_constraint(bds[j-1], i, dim, true));
		}

		std::vector<std::vector<hsb_halfspace>> dnf_j;
		bool any_surviving = false;
		for (auto& conj : dnf) {
			std::vector<hsb_halfspace> conj_j;
			bool violated = false;
			for (auto& h : conj) {
				if (i >= h.w.size() || hsb_detail::feq(h.w[i], 0.0)) {
					conj_j.push_back(h);
					continue;
				}
				size_t k = lex_pos(h);
				if (h.w[i] > 0.0) {
					if (j <= k && j <= m) {
						// Satisfied: drop.
					} else {
						violated = true; break;
					}
				} else {
					if (k < j) {
						// Satisfied: drop.
					} else {
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
			path.resize(path_before);
			continue;
		}

		bool all_empty = true;
		for (auto& c : dnf_j) if (!c.empty()) { all_empty = false; break; }

		if (var_idx == 0 || all_empty) {
			if (lra_feasible(path, dim)) out.push_back(path);
		} else {
			decompose(dnf_j, var_idx - 1, dim, path, out);
		}

		path.resize(path_before);
	}
}

} // namespace detail

// =============================================================================
// Public API — normalize_hsb
// =============================================================================

/**
 * @brief Canonical decomposition of @p x into a unique disjunction of
 *        disjoint convex polyhedra (lex-half-open intervals, d dimensions).
 */
inline hsb normalize_hsb(const hsb& x) {
	auto k = x.root_kind();
	if (k == hsb::kind::bot || k == hsb::kind::top) return x;
	if (is_hsb_zero(x)) return hsb::bottom();
	if (is_hsb_one(x))  return hsb::top();

	// Phase 1: NNF, then normalize every halfspace leaf.
	auto nnf_root  = detail::to_nnf(x.root_ref());
	auto norm_root = detail::normalize_all_leaves(nnf_root);

	size_t dim = detail::infer_dim(norm_root);
	if (dim == 0) return x;

	// Convert to DNF and extract halfspace lists per disjunct.
	std::vector<std::vector<tref>> raw_dnf;
	detail::to_dnf(norm_root, raw_dnf);

	std::vector<std::vector<hsb_halfspace>> dnf;
	dnf.reserve(raw_dnf.size());
	for (auto& conj : raw_dnf) {
		std::vector<hsb_halfspace> hs_conj;
		hs_conj.reserve(conj.size());
		for (auto nd : conj) {
			auto nk = static_cast<hsb::kind>(hsb_tree::get(nd).value.nt);
			if (nk == hsb::kind::halfspace)
				hs_conj.push_back(
					hsb_halfspace_pool::get(hsb_tree::get(nd).value.data));
		}
		dnf.push_back(std::move(hs_conj));
	}

	// Phase 2: recursive interval slicing.
	std::vector<detail::linear_constraint> path;
	std::vector<std::vector<detail::linear_constraint>> clauses;
	detail::decompose(dnf, static_cast<int>(dim) - 1, dim, path, clauses);

	if (clauses.empty()) return hsb::bottom();

	return detail::reconstruct_hsb(clauses, dim);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_NORMALIZER_TMPL_H__
