// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_NORMALIZER_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_NORMALIZER_TMPL_H__

#include "boolean_algebras/hsb/hsb.h"

namespace idni::tau_lang {

// =============================================================================
// Canonical decomposition helpers (Phase 1 + Phase 2)
// — shared LRA infrastructure (linear_constraint, lra_feasible, …) comes from
//   hsb_splitter.tmpl.h which is included before this header.
// =============================================================================

namespace detail {

/// Returns @p root with every halfspace leaf replaced by its normalize()d form
/// (highest-indexed coefficient ±1); unchanged leaves keep their node.
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
	std::vector<hsb_rational> bw; ///< coefficients for x[0]..x[var_idx-1]
	hsb_rational bb;              ///< constant term
	bool upper = true;      ///< true = upper bound (h.w[i]>0), false = lower bound
};

/// Build the boundary for halfspace @p h at variable @p i, solving h for x[i].
/// @pre h is normalized with |h.w[i]| = 1 and no non-zero coefficient above i.
inline boundary make_boundary(const hsb_halfspace& h, size_t i) {
	boundary bd;
	bd.bw.assign(i, hsb_rational{});
	if (h.w[i].sign() > 0) { // upper: x[i] ≤ B  →  B = -Σ_{k<i} h.w[k]*x[k] - h.b
		bd.upper = true;
		for (size_t k = 0; k < i; ++k) bd.bw[k] = -h.w[k];
		bd.bb = -h.b;
	} else {             // lower: x[i] ≥ B  →  B = +Σ_{k<i} h.w[k]*x[k] + h.b
		bd.upper = false;
		for (size_t k = 0; k < i; ++k) bd.bw[k] = h.w[k];
		bd.bb = h.b;
	}
	return bd;
}

/// Lexicographic less-than on boundary (bw, bb) — ignores upper/lower flag.
/// Missing components read as 0.
inline bool boundary_lt(const boundary& a, const boundary& b_) {
	static const hsb_rational zero;
	size_t n = std::max(a.bw.size(), b_.bw.size());
	for (size_t k = 0; k < n; ++k) {
		const auto& lv = k < a.bw.size()  ? a.bw[k]  : zero;
		const auto& rv = k < b_.bw.size() ? b_.bw[k] : zero;
		if (auto c = lv <=> rv; c != 0) return c < 0;
	}
	return a.bb < b_.bb;
}

/// Two boundaries represent the same hyperplane iff (bw, bb) are equal,
/// missing components reading as 0.
inline bool boundary_eq(const boundary& a, const boundary& b_) {
	return !boundary_lt(a, b_) && !boundary_lt(b_, a);
}

/// Build the linear_constraint encoding a bound of x[i] by @p bd in @p dim
/// dimensions: x[i] <= B when @p upper, else the strict x[i] > B. The flag
/// @p upper overrides bd.upper.
inline linear_constraint make_interval_constraint(
	const boundary& bd, size_t i, size_t dim, bool upper)
{
	linear_constraint lc;
	lc.w.assign(dim, hsb_rational{});
	lc.strict = !upper;
	if (upper) {
		lc.w[i] = 1;
		for (size_t k = 0; k < i && k < bd.bw.size(); ++k) lc.w[k] = -bd.bw[k];
		lc.b = -bd.bb;
	} else {
		lc.w[i] = -1;
		for (size_t k = 0; k < i && k < bd.bw.size(); ++k) lc.w[k] = bd.bw[k];
		lc.b = bd.bb;
	}
	return lc;
}

/// Reconstruct an hsb value from a list of clauses (each = conjunction of
/// linear_constraints), padding each weight vector to @p dim. No clause gives
/// bottom; an empty clause gives top.
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
			if (h.w.size() < dim) h.w.resize(dim);
			h.b = lc.b;
			conj = conj & hsb::from_halfspace(h);
		}
		result = result | conj;
	}
	return result;
}

inline void decompose(
	const std::vector<std::vector<hsb_halfspace>>& dnf,
	int var_idx,
	size_t dim,
	std::vector<linear_constraint>& path,
	std::vector<std::vector<linear_constraint>>& out);

/// The cells of one variable x[i]: the conjuncts of a DNF read against a
/// choice of side, x[i] <= B or x[i] > B, for each boundary B of x[i].
struct variable_cells {
	const std::vector<std::vector<hsb_halfspace>>& dnf;
	int var_idx;
	size_t dim;
	std::vector<linear_constraint>& path;
	std::vector<std::vector<linear_constraint>>& out;
	/// The distinct boundaries of x[i], sorted by boundary_lt.
	std::vector<boundary> bds;
	/// Per conjunct and halfspace: 1 + the index of its boundary in bds, or 0
	/// when the halfspace does not mention x[i].
	std::vector<std::vector<size_t>> pos;
	/// Per boundary: +1 for x[i] <= B, -1 for x[i] > B, 0 while undecided.
	std::vector<int> side;

	variable_cells(const std::vector<std::vector<hsb_halfspace>>& dnf_,
		int var_idx_, size_t dim_, std::vector<linear_constraint>& path_,
		std::vector<std::vector<linear_constraint>>& out_)
		: dnf(dnf_), var_idx(var_idx_), dim(dim_), path(path_), out(out_)
	{
		size_t i = static_cast<size_t>(var_idx);
		auto mentions = [i](const hsb_halfspace& h) {
			return i < h.w.size() && !h.w[i].is_zero();
		};
		for (auto& conj : dnf) for (auto& h : conj) {
			if (!mentions(h)) continue;
			boundary bd = make_boundary(h, i);
			bool found = false;
			for (auto& e : bds) if (boundary_eq(e, bd)) { found = true; break; }
			if (!found) bds.push_back(std::move(bd));
		}
		std::sort(bds.begin(), bds.end(), boundary_lt);
		side.assign(bds.size(), 0);
		for (auto& conj : dnf) {
			auto& pc = pos.emplace_back();
			for (auto& h : conj) {
				size_t k = 0;
				if (mentions(h)) {
					boundary bd = make_boundary(h, i);
					for (size_t p = 0; p < bds.size(); ++p)
						if (boundary_eq(bds[p], bd)) { k = p + 1; break; }
				}
				pc.push_back(k);
			}
		}
	}

	/// Whether halfspace @p h, which mentions x[@p i], holds on side @p s of
	/// its boundary.
	static bool holds(const hsb_halfspace& h, size_t i, int s) {
		return (h.w[i].sign() > 0) == (s > 0);
	}

	/// Split the cell of the path on the first boundary an alive conjunct
	/// leaves undecided, until every alive conjunct is decided.
	/// @pre the path is LRA-feasible
	void split() {
		size_t i = static_cast<size_t>(var_idx);
		// The first undecided boundary of a conjunct that is still alive.
		size_t next = bds.size();
		bool alive_any = false, full = false;
		for (size_t c = 0; c < dnf.size(); ++c) {
			bool dead = false, decided = true, lower_free = true;
			size_t first = bds.size();
			for (size_t h = 0; h < dnf[c].size(); ++h) {
				size_t k = pos[c][h];
				if (k == 0) { lower_free = false; continue; }
				int s = side[k - 1];
				if (s == 0) { decided = false; first = std::min(first, k - 1); }
				else if (!holds(dnf[c][h], i, s)) { dead = true; break; }
			}
			if (dead) continue;
			alive_any = true;
			if (decided && lower_free) full = true;
			next = std::min(next, first);
		}
		if (!alive_any) return;
		// Some conjunct holds on the whole cell.
		if (full) { out.push_back(path); return; }
		if (next == bds.size()) { finish(); return; }
		// The side the path forces is implied and needs no constraint.
		auto le = make_interval_constraint(bds[next], i, dim, true);
		auto gt = make_interval_constraint(bds[next], i, dim, false);
		path.push_back(le);
		bool le_ok = lra_feasible(path, dim);
		path.back() = gt;
		bool gt_ok = lra_feasible(path, dim);
		path.pop_back();
		for (int s : { 1, -1 }) {
			if (!(s > 0 ? le_ok : gt_ok)) continue;
			bool both = le_ok && gt_ok;
			if (both) path.push_back(s > 0 ? le : gt);
			side[next] = s;
			split();
			side[next] = 0;
			if (both) path.pop_back();
		}
	}

	/// Every alive conjunct is decided on x[i]: recurse on the halfspaces
	/// left in the lower variables.
	void finish() {
		size_t i = static_cast<size_t>(var_idx);
		std::vector<std::vector<hsb_halfspace>> dnf_j;
		for (size_t c = 0; c < dnf.size(); ++c) {
			std::vector<hsb_halfspace> conj_j;
			bool dead = false;
			for (size_t h = 0; h < dnf[c].size(); ++h) {
				size_t k = pos[c][h];
				if (k == 0) conj_j.push_back(dnf[c][h]);
				else if (!holds(dnf[c][h], i, side[k - 1])) {
					dead = true; break;
				}
			}
			if (!dead) dnf_j.push_back(std::move(conj_j));
		}
		if (var_idx == 0) out.push_back(path);
		else decompose(dnf_j, var_idx - 1, dim, path, out);
	}
};

/// Core recursive canonical decomposition over the DNF of normalized halfspaces.
/// Splits x[var_idx] on the side of each of its boundaries, keeping only the
/// LRA-feasible cells, so the cells partition the path even where two
/// boundaries cross; keeps the conjuncts each cell satisfies, and recurses on
/// var_idx - 1. A side the path already forces adds no constraint.
/// @param dnf disjunction of conjunctions of normalized halfspaces
/// @param var_idx index of the variable to slice; below 0 the path is complete
/// @param dim number of dimensions
/// @param path in/out: the interval constraints chosen so far, LRA-feasible;
/// restored on return
/// @param out receives each complete path as one clause
inline void decompose(
	const std::vector<std::vector<hsb_halfspace>>& dnf,
	int var_idx,
	size_t dim,
	std::vector<linear_constraint>& path,
	std::vector<std::vector<linear_constraint>>& out)
{
	if (dnf.empty()) return;
	if (var_idx < 0) { out.push_back(path); return; }
	variable_cells(dnf, var_idx, dim, path, out).split();
}

} // namespace detail

// =============================================================================
// Public API — normalize_hsb
// =============================================================================

/**
 * @brief Canonical decomposition of @p x into a unique disjunction of
 *        disjoint convex polyhedra (lex-half-open intervals, d dimensions).
 * @param x the value to normalize
 * @return bottom or top when @p x is empty or full, @p x itself when it
 * mentions no variable, else the reconstructed decomposition. Each cell is
 * checked for feasibility through the LRA solver.
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

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_NORMALIZER_TMPL_H__
