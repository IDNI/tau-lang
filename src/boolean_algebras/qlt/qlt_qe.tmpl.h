// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_qe.tmpl.h
 * @brief Quantifier elimination for qlt, the dense linear order over rationals.
 *
 * Included from qlt_descriptor.tmpl.h and nowhere else, so it instantiates only
 * when qlt is in the pack -- which is why the interval computation needs no
 * "is qlt in the pack" guard of its own.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_QE_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_QE_TMPL_H__

#include <limits>

#include "boolean_algebras/qlt/qlt.h"
#include "tau_tree.h"

namespace idni::tau_lang {

// DLO interval computation for (Q,<).
// Collects the satisfying interval I for ∃var. body where body is a
// conjunction of DLO comparison atoms "var op {c}:qlt" (c finite singleton).
// Returns the interval if determined, nullopt if undetermined. A conjunct
// that does not mention var is the caller's when it has a free variable and
// declines the whole computation when it has none: a closed conjunct (a
// nested binder, an unfolded constant atom) may be false.
//   ∃var. body satisfiable  ↔  !result.is_empty()
//   ∀var. body tautology    ↔   result.is_full()
template<NodeType node>
static std::optional<qlt> qlt_dlo_qe_interval(tref var, tref body) {
	using tau = tree<node>;
	qlt acc = qlt::top();
	bool undetermined = false;
	// Free-variable endpoint bounds (symbolic DLO reasoning).
	// Used to detect contradictions like (a < var && var < a) where a is free.
	// Keys: the RHS tree (the free-var side of the constraint).
	subtree_set<node> lower_strict;    // { fv : fv < var }
	subtree_set<node> lower_nonstrict; // { fv : fv <= var }
	subtree_set<node> upper_strict;    // { fv : var < fv }
	subtree_set<node> upper_nonstrict; // { fv : var <= fv }
	subtree_set<node> eq_free;         // { fv : var = fv }
	subtree_set<node> neq_free;        // { fv : var != fv }
	std::function<void(tref)> collect = [&](tref n) {
		if (!n || undetermined || acc.is_empty()) return;
		const auto& t = tau::get(n);
		if (t.equals_T()) return;
		if (t.equals_F()) { acc = qlt::bottom(); return; }
		if (!t.is(tau::wff)) { undetermined = true; return; }
		if (!t.has_child()) return;
		size_t op = t[0].value.get_nt();
		if (op == tau::wff_and) {
			collect(t[0].first());
			collect(t[0].second());
			return;
		}
		if (!contains<node>(n, var)) {
			if (get_free_vars<node>(n).empty()) undetermined = true;
			return;
		}
		// Helper lambda: given (raw_op, lhs, rhs, negate), accumulate the
		// corresponding DLO interval into acc. raw_op is the comparison
		// operator before direction-flip and optional negation.
		auto accumulate_interval = [&](size_t raw_op, tref lhs_t, tref rhs_t, bool negate) {
			bool var_in_lhs = contains<node>(lhs_t, var);
			bool var_in_rhs = contains<node>(rhs_t, var);
			if (var_in_lhs && var_in_rhs) { undetermined = true; return; }
			// The side holding var must be var itself: a compound such
			// as `x & {3}` is no bound on x.
			const auto& var_side = tau::get(var_in_lhs ? lhs_t : rhs_t);
			if (!var_side.is(tau::bf) || !var_side.has_child()
				|| tau::get(var_side.first()) != tau::get(var)) {
				undetermined = true; return;
			}
			const auto& cst = tau::get(var_in_lhs ? rhs_t : lhs_t)[0];
			if (!cst.is_ba_constant()) {
				// Handle typed zero (bf_f = -∞) and typed one (bf_t = +∞) as DLO bounds
				if (cst.is(tau::bf_f) || cst.is(tau::bf_t)) {
					bool cst_is_min = cst.is(tau::bf_f); // bf_f = -∞, bf_t = +∞
					// No point is an end of the order: var = 1 never
					// holds, var != 1 always does (qlt's wff_eq hook
					// folds a bare variable's case before it gets here).
					if (raw_op == tau::bf_eq || raw_op == tau::bf_neq) {
						bool holds = raw_op == tau::bf_neq;
						if (negate) holds = !holds;
						if (holds) return;
						acc = qlt::bottom(); return;
					}
					// Determine if constraint is trivially satisfied (i.e., no restriction on x)
					// x > -∞, x >= -∞ are trivially true; x < +∞, x <= +∞ are trivially true
					bool trivially_sat;
					if (var_in_lhs) {
						trivially_sat = cst_is_min
							? (raw_op == tau::bf_gt || raw_op == tau::bf_gteq)
							: (raw_op == tau::bf_lt || raw_op == tau::bf_lteq);
					} else {
						// cst op x: -∞ < x, -∞ <= x are trivially true
						trivially_sat = cst_is_min
							? (raw_op == tau::bf_lt || raw_op == tau::bf_lteq)
							: (raw_op == tau::bf_gt || raw_op == tau::bf_gteq);
					}
					if (negate) trivially_sat = !trivially_sat;
					if (trivially_sat) return; // no constraint added
					acc = qlt::bottom(); return; // contradiction
				}
				// ∃x.(x ≠ free_var) ≡ T over DLO for any free_var.
				// bf_neq is symmetric so direction flip doesn't change it.
				if (raw_op == tau::bf_neq && !negate) {
					neq_free.insert(var_in_lhs ? rhs_t
						: lhs_t);
					return;
				}
				// Only a variable is a symbolic endpoint: another term
				// (`y & {3}`) need not denote a point.
				if (!cst.is(tau::variable)) { undetermined = true; return; }
				// Symbolic free-variable endpoints: classify into lower/upper
				// bounds so we can detect contradictions like a<var && var<a.
				// Determine effective direction (normalise to var <op> fv).
				tref fv_tree = var_in_lhs ? rhs_t : lhs_t;
				auto eff_op = raw_op;
				if (var_in_rhs) {
					if      (raw_op == tau::bf_lt)   eff_op = tau::bf_gt;
					else if (raw_op == tau::bf_gt)   eff_op = tau::bf_lt;
					else if (raw_op == tau::bf_lteq) eff_op = tau::bf_gteq;
					else if (raw_op == tau::bf_gteq) eff_op = tau::bf_lteq;
				}
				if (negate) {
					if      (eff_op == tau::bf_lt)   eff_op = tau::bf_gteq;
					else if (eff_op == tau::bf_gt)   eff_op = tau::bf_lteq;
					else if (eff_op == tau::bf_lteq) eff_op = tau::bf_gt;
					else if (eff_op == tau::bf_gteq) eff_op = tau::bf_lt;
					else if (eff_op == tau::bf_eq)   eff_op = tau::bf_neq;
					else if (eff_op == tau::bf_neq)  eff_op = tau::bf_eq;
				}
				// eff_op is now in "var <op> fv" form.
				if      (eff_op == tau::bf_lt)    upper_strict.insert(fv_tree);
				else if (eff_op == tau::bf_lteq)  upper_nonstrict.insert(fv_tree);
				else if (eff_op == tau::bf_gt)    lower_strict.insert(fv_tree);
				else if (eff_op == tau::bf_gteq)  lower_nonstrict.insert(fv_tree);
				else if (eff_op == tau::bf_eq)    eq_free.insert(fv_tree);
				else if (eff_op == tau::bf_neq)   neq_free.insert(fv_tree);
				else { undetermined = true; }
				return;
			}
			auto cv = cst.get_ba_constant();
			if (!std::holds_alternative<qlt>(cv)) { undetermined = true; return; }
			const qlt& qba = std::get<qlt>(cv);
			if (qba.pieces.size() != 1) { undetermined = true; return; }
			const auto& piece = qba.pieces[0];
			if (piece.lo.val != piece.hi.val) { undetermined = true; return; }
			if (!piece.lo.val.is_finite()) { undetermined = true; return; }
			const qlt_rational& c = piece.lo.val;
			auto eff_op = raw_op;
			if (var_in_rhs) {
				if      (raw_op == tau::bf_lt)   eff_op = tau::bf_gt;
				else if (raw_op == tau::bf_gt)   eff_op = tau::bf_lt;
				else if (raw_op == tau::bf_lteq) eff_op = tau::bf_gteq;
				else if (raw_op == tau::bf_gteq) eff_op = tau::bf_lteq;
			}
			if (negate) {
				if      (eff_op == tau::bf_lt)   eff_op = tau::bf_gteq;
				else if (eff_op == tau::bf_gt)   eff_op = tau::bf_lteq;
				else if (eff_op == tau::bf_lteq) eff_op = tau::bf_gt;
				else if (eff_op == tau::bf_gteq) eff_op = tau::bf_lt;
				else if (eff_op == tau::bf_eq)   eff_op = tau::bf_neq;
				else if (eff_op == tau::bf_neq)  eff_op = tau::bf_eq;
			}
			qlt interval;
			auto neg_inf = qlt_rational::make_neg_inf();
			auto pos_inf = qlt_rational::make_pos_inf();
			qlt_piece p;
			if (eff_op == tau::bf_lt) {
				p.lo = qlt_endpoint{neg_inf, qlt_bound::OPEN};
				p.hi = qlt_endpoint{c, qlt_bound::OPEN};
				interval = qlt{{p}};
			} else if (eff_op == tau::bf_lteq) {
				p.lo = qlt_endpoint{neg_inf, qlt_bound::OPEN};
				p.hi = qlt_endpoint{c, qlt_bound::CLOSED};
				interval = qlt{{p}};
			} else if (eff_op == tau::bf_gt) {
				p.lo = qlt_endpoint{c, qlt_bound::OPEN};
				p.hi = qlt_endpoint{pos_inf, qlt_bound::OPEN};
				interval = qlt{{p}};
			} else if (eff_op == tau::bf_gteq) {
				p.lo = qlt_endpoint{c, qlt_bound::CLOSED};
				p.hi = qlt_endpoint{pos_inf, qlt_bound::OPEN};
				interval = qlt{{p}};
			} else if (eff_op == tau::bf_eq) {
				p.lo = qlt_endpoint{c, qlt_bound::CLOSED};
				p.hi = qlt_endpoint{c, qlt_bound::CLOSED};
				interval = qlt{{p}};
			} else if (eff_op == tau::bf_neq) {
				qlt_piece p1, p2;
				p1.lo = qlt_endpoint{neg_inf, qlt_bound::OPEN};
				p1.hi = qlt_endpoint{c, qlt_bound::OPEN};
				p2.lo = qlt_endpoint{c, qlt_bound::OPEN};
				p2.hi = qlt_endpoint{pos_inf, qlt_bound::OPEN};
				interval = qlt{{p1, p2}};
			} else { undetermined = true; return; }
			acc = acc & interval;
		};
		// Handle negated comparison: ¬(var op c) → var op' c
		if (op == tau::wff_neg) {
			tref inner = t[0].first();
			const auto& ti = tau::get(inner);
			if (!ti.is(tau::wff) || !ti.has_child()) {
				undetermined = true; return;
			}
			size_t iop = ti[0].value.get_nt();
			// Normalize NNF negated-comparison variants (bf_nXxx → positive)
			if      (iop == tau::bf_ngt)   iop = tau::bf_lteq;
			else if (iop == tau::bf_nlt)   iop = tau::bf_gteq;
			else if (iop == tau::bf_ngteq) iop = tau::bf_lt;
			else if (iop == tau::bf_nlteq) iop = tau::bf_gt;
			if (iop != tau::bf_lt  && iop != tau::bf_lteq &&
			    iop != tau::bf_gt  && iop != tau::bf_gteq &&
			    iop != tau::bf_eq  && iop != tau::bf_neq) {
				undetermined = true; return;
			}
			accumulate_interval(iop, ti[0].first(), ti[0].second(), true);
			return;
		}
		// NNF converts ¬(x op c) to bf_nXxx(x,c). Normalize to positive form.
		if      (op == tau::bf_ngt)   op = tau::bf_lteq;
		else if (op == tau::bf_nlt)   op = tau::bf_gteq;
		else if (op == tau::bf_ngteq) op = tau::bf_lt;
		else if (op == tau::bf_nlteq) op = tau::bf_gt;
		if (op != tau::bf_lt  && op != tau::bf_lteq  &&
		    op != tau::bf_gt  && op != tau::bf_gteq  &&
		    op != tau::bf_eq  && op != tau::bf_neq) {
			undetermined = true; return;
		}
		// Comparison atom: wff(bf_op(lhs_bf, rhs_bf))
		accumulate_interval(op, t[0].first(), t[0].second(), false);
	};
	collect(body);
	// Symbolic contradiction detection via free-variable endpoints.
	// (fv < var && var < fv)  → empty (fv < fv impossible).
	// (fv <= var && var < fv) → empty (fv <= var < fv impossible).
	// (fv < var && var <= fv) → empty.
	// (var = fv1 && var = fv2) with fv1 != fv2 syntactically → empty.
	// (var = fv && var < fv) → empty.
	// (var = fv && var > fv) → empty.
	// (var = fv && var != fv), (fv <= var <= fv && var != fv) → empty.
	auto has_same = [](const subtree_set<node>& a, const subtree_set<node>& b) {
		for (tref t : a) if (b.contains(t)) return true;
		return false;
	};
	auto pinned_and_excluded = [&] {
		for (tref t : neq_free)
			if (eq_free.contains(t) || (lower_nonstrict.contains(t)
				&& upper_nonstrict.contains(t))) return true;
		return false;
	};
	if (has_same(lower_strict, upper_strict)
	 || has_same(lower_strict, upper_nonstrict)
	 || has_same(lower_nonstrict, upper_strict)
	 || has_same(lower_strict, eq_free)
	 || has_same(upper_strict, eq_free)
	 || pinned_and_excluded())
		acc = qlt::bottom();
	// Two distinct equalities to different free vars → contradictory iff
	// we can't prove they're equal. Stay undetermined in this case.
	if (eq_free.size() > 1) undetermined = true;
	// AN-1: symbolic endpoints are recorded above but never
	// intersected into `acc`, so a non-empty verdict is sound only
	// when they cannot shrink the interval to empty. A one-sided
	// symbolic bound is harmless over (Q,<) iff `acc` is unbounded
	// on that side (a point beyond any finite set of free endpoints
	// always exists). Anything whose truth depends on the ORDER of
	// free endpoints -- bounds on both sides, or an equality
	// combined with any other constraint -- is undetermined, not T:
	// `ex x (a < x && x < b)` is `a < b`, false at a = b.
	if (!undetermined && !acc.is_empty()) {
		const bool has_lower = !lower_strict.empty()
					|| !lower_nonstrict.empty();
		const bool has_upper = !upper_strict.empty()
					|| !upper_nonstrict.empty();
		if (!eq_free.empty()) {
			if (has_lower || has_upper || !acc.is_full())
				undetermined = true;
		} else if (has_lower && has_upper) {
			// Exception: a non-strict cycle through one
			// shared endpoint (fv <= var && var <= fv) is
			// satisfied by var := fv for ANY fv, provided
			// nothing else restricts var. Any other
			// two-sided combination depends on the free
			// endpoints' order.
			if (!(lower_strict.empty()
				&& upper_strict.empty()
				&& lower_nonstrict.size() == 1
				&& upper_nonstrict.size() == 1
				&& upper_nonstrict.contains(
					*lower_nonstrict.begin())
				&& acc.is_full()))
				undetermined = true;
		}
		else if (has_lower && !acc.pieces.back()
					.hi.val.is_pos_inf())
			undetermined = true;
		else if (has_upper && !acc.pieces.front()
					.lo.val.is_neg_inf())
			undetermined = true;
	}
	// A symbolic disequality removes one point: it cannot empty a set with
	// an interior point, but it can empty a point (an equality, the
	// non-strict cycle through one endpoint, or a constant singleton).
	if (!undetermined && !acc.is_empty() && !neq_free.empty()) {
		const bool pinned_sym = !eq_free.empty()
			|| (!lower_nonstrict.empty() && !upper_nonstrict.empty());
		bool has_interior = false;
		for (const auto& piece : acc.pieces)
			if (qlt_sem_cmp(piece.lo.val, piece.hi.val)
				== std::partial_ordering::less)
				has_interior = true;
		if (pinned_sym || !has_interior) undetermined = true;
	}
	if (undetermined) {
		// If we already derived an empty interval symbolically, prefer that
		// (it is a definitive answer; the BA fallback would wrongly say SAT).
		if (acc.is_empty()) return acc;
		return std::nullopt;
	}
	return acc;
}

// qlt variables denote points and qlt constants sets of points. Where a term
// combines a variable x with constants, x reads as the singleton {x}: so
// `c & x != 0` says that x lies in c, `x = c` that c is {x}, and `x = 0`,
// `x = 1` never hold. `<`, `<=`, `>` and `>=` compare points, the typed 0 and
// 1 standing below and above every point.
//
// The finite endpoints of the constants of a formula cut Q into cells: each
// endpoint, each open gap between consecutive endpoints and the two rays. An
// order automorphism of Q that fixes every endpoint fixes every constant, and
// one such automorphism maps any point of a gap onto any other point of it, so
// every point of a cell gives the formula the same truth. Values already given
// to other variables count as endpoints too. One point per cell thus decides a
// quantifier exactly: `ex` is the disjunction over the cells, `all` the
// conjunction.

// True when n parameters exceed the qlt-cells-max-params option.
inline bool qlt_cells_too_many(size_t n) {
	const size_t cap = qlt_cells_max_params();
	return cap && n > cap;
}

namespace qlt_cells_detail {

// The greatest integer not above the finite rational r.
inline qlt_rational floor_of(const qlt_rational& r) {
	return qlt_rational(r.p / r.q - (r.p < 0 && r.p % r.q != 0 ? 1 : 0), 1);
}

// The least integer not below the finite rational r.
inline qlt_rational ceil_of(const qlt_rational& r) {
	const qlt_rational f = floor_of(r);
	// f < r <= LLONG_MAX, so f.p + 1 fits
	return f == r ? f : qlt_rational(f.p + 1, 1);
}

// A point strictly between lo and hi, either of which may be missing: 0 when
// it lies there, else the integer nearest to 0, else the midpoint; nullopt
// when an unbounded side holds no rational the representation fits.
inline std::optional<qlt_rational> between(
	const std::optional<qlt_rational>& lo,
	const std::optional<qlt_rational>& hi)
{
	const qlt_rational zero(0, 1);
	auto inside = [&](const qlt_rational& r) {
		return (!lo || *lo < r) && (!hi || r < *hi);
	};
	if (inside(zero)) return zero;
	const auto c = lo && !(*lo < zero)
		? floor_of(*lo).add(qlt_rational(1, 1))
		: ceil_of(*hi).add(qlt_rational(-1, 1));
	if (c && inside(*c)) return c;
	if (!lo || !hi) return std::nullopt;
	return lo->midpoint(*hi);
}

// One cell: an endpoint, an open gap or a ray, with a representative point.
struct cell {
	qlt_piece piece;
	qlt_rational point; // a point of piece
};

// The cells of Q cut by pts, which is sorted and free of repeats; nullopt
// when a cell holds no rational the representation fits (see between).
inline std::optional<std::vector<cell>> cells_of(
	const std::vector<qlt_rational>& pts)
{
	const qlt_endpoint below{ qlt_rational::make_neg_inf(), qlt_bound::OPEN };
	const qlt_endpoint above{ qlt_rational::make_pos_inf(), qlt_bound::OPEN };
	std::vector<cell> out;
	std::optional<qlt_rational> lo;
	auto gap = [&](const std::optional<qlt_rational>& hi) {
		auto point = between(lo, hi);
		if (!point) return false;
		out.push_back({ { lo ? qlt_endpoint{ *lo, qlt_bound::OPEN } : below,
			hi ? qlt_endpoint{ *hi, qlt_bound::OPEN } : above },
			*point });
		return true;
	};
	for (const auto& p : pts) {
		if (!gap(p)) return std::nullopt;
		out.push_back({ { { p, qlt_bound::CLOSED }, { p, qlt_bound::CLOSED } },
			p });
		lo = p;
	}
	if (!gap(std::nullopt)) return std::nullopt;
	return out;
}

// Smaller denominators first, then smaller magnitudes, a positive before its
// negative: the order in which a model prefers its values.
inline bool simpler(const qlt_rational& a, const qlt_rational& b) {
	const long long ma = a.p < 0 ? -a.p : a.p, mb = b.p < 0 ? -b.p : b.p;
	if (a.q != b.q) return a.q < b.q;
	if (ma != mb) return ma < mb;
	return a.p > b.p;
}

// The qlt constant {v}.
inline qlt point_set(const qlt_rational& v) {
	return qlt{ { { { v, qlt_bound::CLOSED }, { v, qlt_bound::CLOSED } } } };
}

} // namespace qlt_cells_detail

/**
 * @brief The truth of a qlt formula under point values of its variables.
 *
 * Reads Boolean connectives, quantifiers over qlt variables (decided by
 * cells), `=` and `!=` between Boolean combinations of qlt constants,
 * variables and the typed 0 and 1, and the order atoms between variables,
 * constants and the typed 0 and 1 (every point of one side below every point
 * of the other, see qlt_order_holds). A named endpoint is read
 * through its value in @ref named. Anything else, a variable or a name
 * without a value, a spent budget or a cell holding no rational the
 * representation fits is nullopt.
 */
template<NodeType node>
class qlt_point_eval {
	using tau = tree<node>;
public:
	/// The values of the variables, innermost binding last.
	std::vector<std::pair<tref, qlt_rational>> env;
	/// The values given to named endpoints.
	std::vector<std::pair<std::string, qlt_rational>> named;

	/**
	 * @brief The finite endpoints of the qlt constants @p fm holds, sorted
	 * and without repeats; nullopt when one is inexact or, unless
	 * @p skip_names, a named endpoint, whose position in Q is unknown.
	 */
	static std::optional<std::vector<qlt_rational>> ends_of(tref fm,
		bool skip_names = false)
	{
		std::vector<qlt_rational> ends;
		for (tref k : tau::get(fm).select_all(
			is<node, tau::ba_constant>))
		{
			auto v = tau::get(k).get_ba_constant();
			if (!std::holds_alternative<qlt>(v)) continue;
			const qlt& q = std::get<qlt>(v);
			if (q.inexact) return std::nullopt;
			for (const auto& piece : q.pieces)
				for (const auto& e : { piece.lo.val, piece.hi.val }) {
					if (e.is_sym() && !skip_names)
						return std::nullopt;
					if (e.is_finite()) ends.push_back(e);
				}
		}
		std::sort(ends.begin(), ends.end());
		ends.erase(std::unique(ends.begin(), ends.end()), ends.end());
		return ends;
	}

	/// The names of the named endpoints of the qlt constants @p fm holds.
	static std::vector<std::string> names_of(tref fm) {
		std::vector<std::string> names;
		for (tref k : tau::get(fm).select_all(
			is<node, tau::ba_constant>))
		{
			auto v = tau::get(k).get_ba_constant();
			if (!std::holds_alternative<qlt>(v)) continue;
			for (const auto& piece : std::get<qlt>(v).pieces)
				for (const auto& e : { piece.lo.val, piece.hi.val })
					if (e.is_sym()) names.push_back(e.sym);
		}
		std::sort(names.begin(), names.end());
		names.erase(std::unique(names.begin(), names.end()), names.end());
		return names;
	}

	/// True when @p var is a variable of the qlt type.
	static bool is_point_var(tref var) {
		const auto& t = tau::get(var);
		return t.is(tau::variable)
			&& ba_descriptor<qlt, node>::owns_type(t.get_ba_type());
	}

	/// @brief Evaluate over the finite endpoints @p ends (sorted, without
	/// repeats), with a budget of `qlt_cells_budget()` cell visits (0 =
	/// unbounded).
	explicit qlt_point_eval(std::vector<qlt_rational> ends)
		: ends(std::move(ends)) {}

	/// False once exhausted, which makes every answer nullopt; otherwise
	/// spends one unit.
	bool spend() {
		if (exhausted()) return false;
		--budget;
		return true;
	}
	/// True once the budget is spent or out_of_range: a verdict reached
	/// since is not one.
	bool exhausted() const { return !budget || beyond; }
	/// True once a cell held no rational the representation fits, as past
	/// the greatest `long long`.
	bool out_of_range() const { return beyond; }

	/// The cells cut by the endpoints and by the values in @ref env; none,
	/// and out_of_range from then on, when a cell holds no rational the
	/// representation fits.
	std::vector<qlt_cells_detail::cell> cells() {
		std::vector<qlt_rational> pts(ends);
		for (const auto& [_, v] : env) pts.push_back(v);
		for (const auto& [_, v] : named) pts.push_back(v);
		std::sort(pts.begin(), pts.end());
		pts.erase(std::unique(pts.begin(), pts.end()), pts.end());
		auto cs = qlt_cells_detail::cells_of(pts);
		if (!cs) beyond = true;
		return cs ? std::move(*cs) : std::vector<qlt_cells_detail::cell>{};
	}

	/// The truth of @p fm under @ref env and @ref named; nullopt when a
	/// part of it is not read (see the class comment) or the evaluator is
	/// exhausted.
	std::optional<bool> holds(tref fm) {
		const auto& t = tau::get(fm);
		if (t.equals_T()) return true;
		if (t.equals_F()) return false;
		if (!t.is(tau::wff) || !t.has_child()) return std::nullopt;
		const auto& c = t[0];
		const auto op = c.value.nt;
		if (op == tau::wff_neg) {
			auto a = holds(c.first());
			if (!a) return std::nullopt;
			return !*a;
		}
		if (op == tau::wff_and || op == tau::wff_or) {
			const bool unit = op == tau::wff_or;
			auto a = holds(c.first());
			if (a && *a == unit) return a;
			auto b = holds(c.second());
			if (b && *b == unit) return b;
			if (!a || !b) return std::nullopt;
			return !unit;
		}
		if (op == tau::wff_imply || op == tau::wff_equiv
			|| op == tau::wff_xor)
		{
			auto a = holds(c.first());
			auto b = holds(c.second());
			if (op == tau::wff_imply && ((a && !*a) || (b && *b)))
				return true;
			if (!a || !b) return std::nullopt;
			if (op == tau::wff_imply) return !*a || *b;
			return (*a == *b) == (op == tau::wff_equiv);
		}
		if (op == tau::wff_ex || op == tau::wff_all)
			return quantified(c.first(), c.second(),
				op == tau::wff_all);
		return compare(static_cast<size_t>(op), c.first(), c.second());
	}

	/// The comparison @p op between the terms @p lhs and @p rhs.
	std::optional<bool> compare(size_t op, tref lhs, tref rhs) const {
		if (op == tau::bf_eq || op == tau::bf_neq) {
			auto a = term(lhs), b = term(rhs);
			if (!a || !b) return std::nullopt;
			const qlt d = *a ^ *b;
			if (d.inexact) return std::nullopt;
			return d.is_empty() == (op == tau::bf_eq);
		}
		bool strict, swap;
		if (op == tau::bf_lt || op == tau::bf_nlt) strict = true, swap = false;
		else if (op == tau::bf_lteq || op == tau::bf_nlteq)
			strict = false, swap = false;
		else if (op == tau::bf_gt || op == tau::bf_ngt)
			strict = true, swap = true;
		else if (op == tau::bf_gteq || op == tau::bf_ngteq)
			strict = false, swap = true;
		else return std::nullopt;
		const bool negate = op == tau::bf_nlt || op == tau::bf_nlteq
			|| op == tau::bf_ngt || op == tau::bf_ngteq;
		auto a = operand(lhs), b = operand(rhs);
		if (!a || !b) return std::nullopt;
		auto h = swap ? qlt_order_holds(*b, *a, strict)
			: qlt_order_holds(*a, *b, strict);
		if (!h) return std::nullopt;
		return *h != negate;
	}

	/// `all var body` when @p universal, `ex var body` otherwise.
	std::optional<bool> quantified(tref var, tref body, bool universal) {
		if (!is_point_var(var)) return std::nullopt;
		bool unknown = false;
		const auto cs = cells();
		if (exhausted()) return std::nullopt;
		for (const auto& c : cs) {
			if (!spend()) return std::nullopt;
			env.emplace_back(var, c.point);
			auto v = holds(body);
			env.pop_back();
			if (!v) unknown = true;
			else if (*v != universal) return v;
		}
		if (unknown) return std::nullopt;
		return universal;
	}

	/**
	 * @brief Calls @p f once per joint position of the named endpoints
	 * @p names over the endpoints, with a value of each in @ref named;
	 * stops when @p f returns false.
	 * @param names The names to place.
	 * @param i Index of the first name not yet placed (0 from a caller).
	 * @param f Callback returning whether to continue.
	 * @return False when @p f stopped the walk or the evaluator is
	 * exhausted.
	 */
	template <typename F>
	bool each_naming(const std::vector<std::string>& names, size_t i,
		F&& f)
	{
		if (i == names.size()) return f();
		const auto cs = cells();
		if (exhausted()) return false;
		for (const auto& c : cs) {
			if (!spend()) return false;
			named.emplace_back(names[i], c.point);
			const bool go = each_naming(names, i + 1, f);
			named.pop_back();
			if (!go) return false;
		}
		return true;
	}

	/**
	 * @brief Calls @p f once per joint position of @p vars over the
	 * endpoints, with a point of it in @ref env; stops when @p f returns
	 * false.
	 * @param vars The qlt variables to place.
	 * @param i Index of the first variable not yet placed (0 from a caller).
	 * @param f Callback returning whether to continue.
	 * @return False when @p f stopped the walk or the evaluator is
	 * exhausted.
	 */
	template <typename F>
	bool each_position(const trefs& vars, size_t i, F&& f) {
		if (i == vars.size()) return f();
		const auto cs = cells();
		if (exhausted()) return false;
		for (const auto& c : cs) {
			if (!spend()) return false;
			env.emplace_back(vars[i], c.point);
			const bool go = each_position(vars, i + 1, f);
			env.pop_back();
			if (!go) return false;
		}
		return true;
	}

private:
	std::vector<qlt_rational> ends;
	bool beyond = false;
	size_t budget = qlt_cells_budget() ? qlt_cells_budget()
		: std::numeric_limits<size_t>::max();

	// The innermost value bound to var in env, nullopt when unbound.
	std::optional<qlt_rational> value_of(tref var) const {
		for (auto it = env.rbegin(); it != env.rend(); ++it)
			if (tau::subtree_equals(it->first, var)) return it->second;
		return std::nullopt;
	}

	// e itself, or the value of the named endpoint it names; nullopt for
	// a name without a value.
	std::optional<qlt_rational> value_of(const qlt_rational& e) const {
		if (!e.is_sym()) return e;
		for (const auto& [name, v] : named) if (name == e.sym) return v;
		return std::nullopt;
	}

	// q with each named endpoint at its value.
	std::optional<qlt> resolved(const qlt& q) const {
		bool sym = false;
		for (const auto& p : q.pieces)
			sym = sym || p.lo.val.is_sym() || p.hi.val.is_sym();
		if (!sym) return q;
		qlt r = qlt::bottom();
		for (const auto& p : q.pieces) {
			auto lo = value_of(p.lo.val), hi = value_of(p.hi.val);
			if (!lo || !hi) return std::nullopt;
			if (*hi < *lo) continue;
			r = r | qlt{ { { { *lo, p.lo.bound }, { *hi, p.hi.bound } } } };
		}
		if (r.inexact) return std::nullopt;
		return r;
	}

	// The qlt set a term denotes under env and named; nullopt for a
	// shape not read or an inexact result.
	std::optional<qlt> term(tref n) const {
		const auto& t = tau::get(n);
		if (!t.is(tau::bf) || !t.has_child()) return std::nullopt;
		const auto& c = t[0];
		if (c.is(tau::bf_t)) return qlt::top();
		if (c.is(tau::bf_f)) return qlt::bottom();
		if (c.is(tau::variable)) {
			auto v = value_of(t.first());
			if (!v) return std::nullopt;
			return qlt_cells_detail::point_set(*v);
		}
		if (c.is_ba_constant()) {
			auto v = c.get_ba_constant();
			if (!std::holds_alternative<qlt>(v)) return std::nullopt;
			const qlt& q = std::get<qlt>(v);
			if (q.inexact) return std::nullopt;
			return resolved(q);
		}
		std::optional<qlt> r;
		if (c.is(tau::bf_neg)) {
			if (auto a = term(c.first())) r = ~*a;
		} else if (c.is(tau::bf_and) || c.is(tau::bf_or)
			|| c.is(tau::bf_xor))
		{
			auto a = term(c.first()), b = term(c.second());
			if (a && b) r = c.is(tau::bf_and) ? *a & *b
				: c.is(tau::bf_or) ? *a | *b : *a ^ *b;
		}
		if (!r || r->inexact) return std::nullopt;
		return r;
	}

	// A side of an order atom: the typed 0 or 1, a variable as the set
	// holding its point, or a constant with its named endpoints valued.
	std::optional<qlt_order_side> operand(tref n) const {
		const auto& t = tau::get(n);
		if (!t.is(tau::bf) || !t.has_child()) return std::nullopt;
		const auto& c = t[0];
		if (c.is(tau::variable)) {
			auto v = value_of(t.first());
			if (!v) return std::nullopt;
			return qlt_order_side{ 0,
				qlt_cells_detail::point_set(*v) };
		}
		auto side = qlt_constant_side<node>(c);
		if (!side || side->end) return side;
		auto q = resolved(side->points);
		if (!q) return std::nullopt;
		return qlt_order_side{ 0, std::move(*q) };
	}
};

// The free variables of body other than var, nullopt when one is not a qlt
// variable.
template<NodeType node>
static std::optional<trefs> qlt_point_params(tref var, tref body) {
	using tau = tree<node>;
	trefs params;
	for (tref v : get_free_vars<node>(body)) {
		if (tau::subtree_equals(v, var)) continue;
		if (!qlt_point_eval<node>::is_point_var(v)) return std::nullopt;
		params.push_back(v);
	}
	return params;
}

// body with each meet of two qlt variables compared with 0 read for points:
// `x & y' = 0` is `x = y`, `x & y = 0` is `x != y` and `x' & y' = 0` is F,
// their disequalities the negations.
template<NodeType node>
static tref qlt_point_meets(tref body) {
	using tau = tree<node>;
	// (the variable's bf, complemented) of a bare or complemented variable
	auto operand = [](tref n) -> std::optional<std::pair<tref, bool>> {
		const auto& t = tau::get(n);
		if (!t.is(tau::bf) || !t.has_child()) return std::nullopt;
		bool neg = false;
		tref v = n;
		if (t[0].is(tau::bf_neg)) neg = true, v = t[0].first();
		const auto& vt = tau::get(v);
		if (!vt.is(tau::bf) || !vt.child_is(tau::variable)
			|| !qlt_point_eval<node>::is_point_var(vt.first()))
			return std::nullopt;
		return std::pair{ v, neg };
	};
	subtree_map<node, tref> changes;
	for (tref a : tau::get(body).select_all([](tref n) {
		const auto& t = tau::get(n);
		return t.is(tau::wff) && t.has_child()
			&& (t[0].is(tau::bf_eq) || t[0].is(tau::bf_neq)); }))
	{
		const auto& at = tau::get(a)[0];
		tref l = at.first(), rr = at.second();
		if (tau::get(l).child_is(tau::bf_f)) std::swap(l, rr);
		if (!tau::get(rr).child_is(tau::bf_f)
			|| !tau::get(l).child_is(tau::bf_and)) continue;
		const auto& m = tau::get(l)[0];
		auto u = operand(m.first()), v = operand(m.second());
		if (!u || !v) continue;
		const bool eq = at.is(tau::bf_eq);
		tref out;
		if (u->second && v->second) out = eq ? tau::_F() : tau::_T();
		else if (u->second == v->second)
			out = eq ? tau::build_bf_neq(u->first, v->first)
				: tau::build_bf_eq(u->first, v->first);
		else out = eq ? tau::build_bf_eq(u->first, v->first)
				: tau::build_bf_neq(u->first, v->first);
		changes.emplace(a, out);
	}
	if (changes.empty()) return body;
	return rewriter::replace<node>(body, changes);
}

// `ex var body` (`all var body` when universal) decided by cells, when it has
// the same truth for every value of the other free variables of body and for
// every position of its named endpoints, at most qlt-cells-max-params of each;
// nullopt otherwise.
template<NodeType node>
static std::optional<bool> qlt_cells_qe(tref var, tref body, bool universal) {
	auto ends = qlt_point_eval<node>::ends_of(body, true);
	if (!ends) return std::nullopt;
	auto params = qlt_point_params<node>(var, body);
	const auto names = qlt_point_eval<node>::names_of(body);
	if (!params || qlt_cells_too_many(params->size())
		|| qlt_cells_too_many(names.size())) return std::nullopt;
	qlt_point_eval<node> ev(std::move(*ends));
	std::optional<bool> verdict;
	bool decided = true;
	ev.each_naming(names, 0, [&] {
		return ev.each_position(*params, 0, [&] {
			auto v = ev.quantified(var, body, universal);
			if (!v || (verdict && *verdict != *v))
				return decided = false;
			verdict = v;
			return true;
		});
	});
	if (!decided || ev.exhausted()) return std::nullopt;
	return verdict;
}

// `ex var body` with one other free variable y, as the set of the values of y
// for which it holds: `U & y != 0` for U the union of the cells of y where it
// does. With named endpoints U is spelled with the names, and it stands only
// when it is spelled the same for every position of the names among the
// finite endpoints. nullptr when body has another shape, a cell is undecided
// or U depends on where the names lie.
template<NodeType node>
static tref qlt_cells_residual(tref var, tref body) {
	using tau = tree<node>;
	if (const auto& t = tau::get(body); t.has_child()
		&& t[0].value.nt == tau::wff_ex) body = t[0].second();
	auto ends = qlt_point_eval<node>::ends_of(body, true);
	if (!ends) return nullptr;
	const auto names = qlt_point_eval<node>::names_of(body);
	if (qlt_cells_too_many(names.size())) return nullptr;
	auto params = qlt_point_params<node>(var, body);
	if (!params || params->size() != 1) return nullptr;
	const tref y = params->front();
	qlt_point_eval<node> ev(std::move(*ends));
	// an endpoint at a name's value, spelled as the name
	auto spelled = [&](qlt_endpoint e) {
		if (e.val.is_finite())
			for (const auto& [name, v] : ev.named)
				if (v == e.val) {
					e.val = qlt_rational::make_sym(name);
					break;
				}
		return e;
	};
	std::optional<std::vector<qlt_piece>> sat;
	bool decided = true;
	ev.each_naming(names, 0, [&] {
		std::vector<qlt_piece> pieces;
		bool open = false; // the last piece may still grow
		for (const auto& c : ev.cells()) {
			ev.env.emplace_back(y, c.point);
			auto v = ev.quantified(var, body, false);
			ev.env.pop_back();
			if (!v) return decided = false;
			if (!*v) { open = false; continue; }
			if (open) pieces.back().hi = spelled(c.piece.hi);
			else pieces.push_back({ spelled(c.piece.lo),
				spelled(c.piece.hi) });
			open = true;
		}
		if (sat && *sat != pieces) return decided = false;
		sat = std::move(pieces);
		return true;
	});
	if (!decided || ev.exhausted() || !sat) return nullptr;
	if (sat->empty()) return tau::_F();
	qlt values = qlt::bottom();
	for (const auto& p : *sat) values = values | qlt{ { p } };
	if (values.inexact) return nullptr;
	if (values.is_full()) return tau::_T();
	tref cst = tau::get(tau::bf, { tau::get_ba_constant(
		typename tau::constant(values), tau::get(y).get_ba_type()) });
	return tau::build_bf_neq_0(tau::build_bf_and(cst,
		tau::get(tau::bf, y)));
}

// The truth of the comparison op between two terms without variables that
// hold named endpoints, when it is the same for every position of the names
// among the finite endpoints; nullopt otherwise, or when no name occurs.
template<NodeType node>
static std::optional<bool> qlt_named_ground_truth(size_t op, tref lhs,
	tref rhs)
{
	using eval = qlt_point_eval<node>;
	if (!get_free_vars<node>(lhs).empty()
		|| !get_free_vars<node>(rhs).empty()) return std::nullopt;
	auto names = eval::names_of(lhs);
	for (auto& n : eval::names_of(rhs)) names.push_back(std::move(n));
	std::sort(names.begin(), names.end());
	names.erase(std::unique(names.begin(), names.end()), names.end());
	if (names.empty() || qlt_cells_too_many(names.size()))
		return std::nullopt;
	auto l = eval::ends_of(lhs, true), r = eval::ends_of(rhs, true);
	if (!l || !r) return std::nullopt;
	l->insert(l->end(), r->begin(), r->end());
	std::sort(l->begin(), l->end());
	l->erase(std::unique(l->begin(), l->end()), l->end());
	eval ev(std::move(*l));
	std::optional<bool> verdict;
	bool decided = true;
	ev.each_naming(names, 0, [&] {
		auto v = ev.compare(op, lhs, rhs);
		if (!v || (verdict && *verdict != *v)) return decided = false;
		verdict = v;
		return true;
	});
	if (!decided || ev.exhausted()) return std::nullopt;
	return verdict;
}

// The truth of a formula without variables, when it is the same for every
// position of its named endpoints among the finite endpoints; nullopt
// otherwise, or past the cell bounds.
template<NodeType node>
static std::optional<bool> qlt_ground_truth(tref fm) {
	using eval = qlt_point_eval<node>;
	const auto names = eval::names_of(fm);
	if (qlt_cells_too_many(names.size())) return std::nullopt;
	auto ends = eval::ends_of(fm, true);
	if (!ends) return std::nullopt;
	eval ev(std::move(*ends));
	std::optional<bool> verdict;
	bool decided = true;
	ev.each_naming(names, 0, [&] {
		auto v = ev.holds(fm);
		if (!v || (verdict && *verdict != *v)) return decided = false;
		verdict = v;
		return true;
	});
	if (!decided || ev.exhausted()) return std::nullopt;
	return verdict;
}

// True when body is a conjunction of disequations `var != t` with t free of
// var. Such a body excludes finitely many points from infinitely many, so
// `ex var body` holds.
template<NodeType node>
static bool qlt_only_excludes(tref var, tref body) {
	using tau = tree<node>;
	auto is_bare_var = [&](tref side) {
		const auto& t = tau::get(side);
		return t.child_is(tau::variable)
			&& tau::get(t.first()) == tau::get(var);
	};
	bool any = false;
	std::function<bool(tref)> check = [&](tref n) {
		const auto& t = tau::get(n);
		if (t.equals_T()) return true;
		if (!t.is(tau::wff) || !t.has_child()) return false;
		if (t[0].is(tau::wff_and))
			return check(t[0].first()) && check(t[0].second());
		const tree<node>* at = &t[0];
		if (t[0].is(tau::wff_neg)) {
			const auto& ti = tau::get(t[0].first());
			if (!ti.is(tau::wff) || !ti.has_child()
				|| !ti[0].is(tau::bf_eq)) return false;
			at = &ti[0];
		} else if (!t[0].is(tau::bf_neq)) return false;
		tref lhs = at->first(), rhs = at->second();
		const bool in_l = contains<node>(lhs, var);
		const bool in_r = contains<node>(rhs, var);
		if (in_l == in_r || !is_bare_var(in_l ? lhs : rhs)) return false;
		return any = true;
	};
	return check(body) && any;
}

// The omcat_qe capability: answers satisfiability rather than handing core the
// interval, which stays qlt's own. body is either a bare existential scoped
// conjunction or a wff_ex/wff_all node, whose quantifier decides which end of
// the interval is asked about. Without a determined interval the cells decide
// (see qlt_point_eval); nullopt means the truth depends on the other
// variables in a way neither reads.
template<NodeType node>
static std::optional<bool> qlt_omcat_qe(tref var, tref body) {
	using tau = tree<node>;
	tref inner = body;
	bool universal = false;
	if (const auto& t = tau::get(body); t.has_child()) {
		if (auto op = t[0].value.nt; op == tau::wff_ex) inner = t[0].second();
		else if (op == tau::wff_all) {
			inner = t[0].second();
			universal = true;
		}
	}
	inner = qlt_point_meets<node>(inner);
	if (auto interval = qlt_dlo_qe_interval<node>(var, inner))
		return universal ? interval->is_full() : !interval->is_empty();
	if (auto r = qlt_cells_qe<node>(var, inner, universal)) return r;
	if (!universal && qlt_only_excludes<node>(var, inner)) return true;
	return std::nullopt;
}

// Fourier-Motzkin elimination for the dense order without
// endpoints that qlt_dlo_qe_interval reasons in. For body a conjunction of
// atoms `L op var` / `var op U` (op among <, <=, >, >= and their negations)
// whose var side is var itself and whose other side does not mention var,
//   ex var (/\ L_i <_i var  /\  var <_j U_j)  ==  /\ L_i <_ij U_j
// where <_ij is strict iff either bound is strict. Density gives a point
// strictly between L and U, and the absence of endpoints the one-sided case
// (which qlt_dlo_qe_interval already decides, so only the two-sided case is
// answered here). A variable pinned to one point t, a variable or a
// single-point constant -- by `var = t` or by `t <= var && var <= t` -- is
// eliminated by substituting t, whatever the other conjuncts are. Disequalities `var != c_k` beside one lower bound L
// and one upper bound U are eliminated by density: an interval with an
// interior point is not exhausted by finitely many points, so
//   ex var (L <  var ... var <  U && /\ var != c_k)  ==  L < U  (either strict)
//   ex var (L <= var && var <= U && /\ var != c_k)
//       ==  L < U || (L = U && /\ L != c_k)
// (L a variable or a constant, so never a typed 0/1). Anything else -- a
// disequality beside several bounds, a compound term, a typed 0/1 sentinel
// (an endpoint) -- returns nullptr and leaves the binder in place.
template<NodeType node>
static tref qlt_dlo_fm_residual(tref var, tref body) {
	using tau = tree<node>;
	tref inner = body;
	if (const auto& t = tau::get(body); t.has_child()
		&& t[0].value.nt == tau::wff_ex) inner = t[0].second();
	std::vector<std::pair<tref, bool>> lower, upper; // (term, strict)
	auto is_bare_var = [&](tref side) {
		const auto& st = tau::get(side);
		return st.is(tau::bf) && st.has_child()
			&& tau::get(st.first()) == tau::get(var);
	};
	auto is_sentinel = [](tref side) {
		const auto& st = tau::get(side);
		return st.has_child() && (st[0].is(tau::bf_f) || st[0].is(tau::bf_t));
	};
	// A term denoting a point: a variable or a single-point constant.
	auto is_point = [](tref term) {
		const auto& t = tau::get(term);
		if (!t.is(tau::bf) || !t.has_child()) return false;
		if (t[0].is(tau::variable)) return true;
		if (!t[0].is_ba_constant()) return false;
		auto v = t[0].get_ba_constant();
		if (!std::holds_alternative<qlt>(v)) return false;
		const qlt& q = std::get<qlt>(v);
		return !q.inexact && q.pieces.size() == 1
			&& q.pieces[0].lo.val.is_finite()
			&& q.pieces[0].lo.val == q.pieces[0].hi.val
			&& q.pieces[0].lo.bound == qlt_bound::CLOSED
			&& q.pieces[0].hi.bound == qlt_bound::CLOSED;
	};
	// A bound on var read off one atom, normalised to `var op other` with
	// op among <, <=, >, >= and =; nullopt for any other atom. A
	// bound must be a point: against a constant holding several points the
	// order reads every one of them.
	struct bound { typename node::T op; tref other; };
	auto read_bound = [&](tref n) -> std::optional<bound> {
		const auto& t = tau::get(n);
		if (!t.is(tau::wff) || !t.has_child()) return std::nullopt;
		auto op = t[0].value.nt;
		bool negate = false;
		const tree<node>* at = &t[0];
		if (op == tau::wff_neg) {
			const auto& ti = tau::get(t[0].first());
			if (!ti.is(tau::wff) || !ti.has_child()) return std::nullopt;
			at = &ti[0];
			op = ti[0].value.nt;
			negate = true;
		}
		if      (op == tau::bf_ngt)   op = tau::bf_lteq;
		else if (op == tau::bf_nlt)   op = tau::bf_gteq;
		else if (op == tau::bf_ngteq) op = tau::bf_lt;
		else if (op == tau::bf_nlteq) op = tau::bf_gt;
		if (op == tau::bf_eq && negate) return std::nullopt;
		if (op != tau::bf_lt && op != tau::bf_lteq && op != tau::bf_eq
			&& op != tau::bf_gt && op != tau::bf_gteq) return std::nullopt;
		tref lhs = at->first(), rhs = at->second();
		const bool in_l = contains<node>(lhs, var);
		const bool in_r = contains<node>(rhs, var);
		if (in_l == in_r) return std::nullopt;
		if (!is_bare_var(in_l ? lhs : rhs)) return std::nullopt;
		tref other = in_l ? rhs : lhs;
		if (is_sentinel(other) || !is_point(other)) return std::nullopt;
		// Normalise to `var op other`.
		if (in_r) {
			if      (op == tau::bf_lt)   op = tau::bf_gt;
			else if (op == tau::bf_gt)   op = tau::bf_lt;
			else if (op == tau::bf_lteq) op = tau::bf_gteq;
			else if (op == tau::bf_gteq) op = tau::bf_lteq;
		}
		if (negate) {
			if      (op == tau::bf_lt)   op = tau::bf_gteq;
			else if (op == tau::bf_gt)   op = tau::bf_lteq;
			else if (op == tau::bf_lteq) op = tau::bf_gt;
			else                         op = tau::bf_lt;
		}
		return bound{ static_cast<size_t>(op), other };
	};
	trefs conjs;
	std::function<void(tref)> flatten = [&](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::wff) && t.has_child()
			&& t[0].value.nt == tau::wff_and)
			flatten(t[0].first()), flatten(t[0].second());
		else conjs.push_back(n);
	};
	flatten(inner);
	// ex var (var = t && phi) == phi[var := t], and t <= var <= t is var = t.
	// The pin must not be rebound inside the scope, or substituting it there
	// would capture it.
	auto rebinds = [&](tref term) {
		for (tref v : get_free_vars<node>(term))
			if (tau::get(inner).find_top([&](tref m) {
				return is_quantifier<node>(m)
					&& tau::get(tau::get(m)[0].first())
						== tau::get(v); }))
				return true;
		return false;
	};
	trefs le, ge;
	for (tref c : conjs) {
		auto b = read_bound(c);
		if (!b) continue;
		tref pin = nullptr;
		if (b->op == tau::bf_eq) pin = b->other;
		else if (b->op == tau::bf_lteq) {
			for (tref g : ge) if (tau::get(g) == tau::get(b->other)) pin = g;
			le.push_back(b->other);
		} else if (b->op == tau::bf_gteq) {
			for (tref l : le) if (tau::get(l) == tau::get(b->other)) pin = l;
			ge.push_back(b->other);
		}
		if (pin && is_point(pin) && !rebinds(pin)) {
			subtree_map<node, tref> changes;
			for (tref occ : tau::get(inner).select_all(is_bare_var))
				changes.emplace(occ, pin);
			return rewriter::replace<node>(inner, changes);
		}
	}
	// The c of a disequality `var != c`, spelled `var != c` or `!(var = c)`.
	auto read_excluded = [&](tref n) -> tref {
		const auto& t = tau::get(n);
		if (!t.is(tau::wff) || !t.has_child()) return nullptr;
		const tree<node>* at = &t[0];
		if (t[0].is(tau::wff_neg)) {
			const auto& ti = tau::get(t[0].first());
			if (!ti.is(tau::wff) || !ti.has_child()
				|| !ti[0].is(tau::bf_eq)) return nullptr;
			at = &ti[0];
		} else if (!t[0].is(tau::bf_neq)) return nullptr;
		tref lhs = at->first(), rhs = at->second();
		const bool in_l = contains<node>(lhs, var);
		const bool in_r = contains<node>(rhs, var);
		if (in_l == in_r || !is_bare_var(in_l ? lhs : rhs)) return nullptr;
		return in_l ? rhs : lhs;
	};
	trefs excluded;
	for (tref c : conjs) {
		if (tau::get(c).equals_T()) continue;
		if (auto b = read_bound(c); b && b->op != tau::bf_eq) {
			if (b->op == tau::bf_lt || b->op == tau::bf_lteq)
				upper.emplace_back(b->other, b->op == tau::bf_lt);
			else lower.emplace_back(b->other, b->op == tau::bf_gt);
			continue;
		}
		tref e = read_excluded(c);
		if (!e) return nullptr;
		// No point is an end of the order, so `var != 0/1` always holds.
		if (!is_sentinel(e)) excluded.push_back(e);
	}
	if (lower.empty() || upper.empty()) return nullptr;
	if (!excluded.empty()) {
		if (lower.size() != 1 || upper.size() != 1) return nullptr;
		const auto& [l, ls] = lower.front();
		const auto& [u, us] = upper.front();
		if (ls || us) return tau::build_bf_lt(l, u);
		// L = U is the single-point case: L must be a point for `L != c`
		// to be all it takes.
		const auto& lt = tau::get(l);
		if (!lt.is(tau::bf) || !lt.has_child()
			|| !(lt[0].is(tau::variable) || lt[0].is_ba_constant()))
			return nullptr;
		trefs point{ tau::build_bf_eq(l, u) };
		for (tref e : excluded) point.push_back(tau::build_bf_neq(l, e));
		return tau::build_wff_or(tau::build_bf_lt(l, u),
			tau::build_wff_and(point));
	}
	trefs out;
	for (const auto& [l, ls] : lower)
		for (const auto& [u, us] : upper)
			out.push_back(ls || us ? tau::build_bf_lt(l, u)
				: tau::build_bf_lteq(l, u));
	return tau::build_wff_and(out);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_QE_TMPL_H__
