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
		auto op = t[0].value.nt;
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
			auto iop = ti[0].value.nt;
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

// The finite rational p of a qlt constant that is exactly the point [p,p].
template<NodeType node>
static std::optional<qlt_rational> qlt_point_constant(const tree<node>& c) {
	if (!c.is_ba_constant()) return std::nullopt;
	auto v = c.get_ba_constant();
	if (!std::holds_alternative<qlt>(v)) return std::nullopt;
	const qlt& q = std::get<qlt>(v);
	if (q.inexact || q.pieces.size() != 1) return std::nullopt;
	const auto& p = q.pieces[0];
	if (!p.lo.val.is_finite() || p.lo.val != p.hi.val
		|| p.lo.bound != qlt_bound::CLOSED
		|| p.hi.bound != qlt_bound::CLOSED) return std::nullopt;
	return p.lo.val;
}

// The bf constant of one point p when every occurrence of var in body is an
// operand of `p & var` or `p & var'`, nullptr otherwise.
template<NodeType node>
static tref qlt_point_meet_of(tref var, tref body) {
	using tau = tree<node>;
	tref point = nullptr;
	bool ok = true;
	auto is_bare_var = [&](tref n) {
		const auto& t = tau::get(n);
		return t.child_is(tau::variable)
			&& tau::get(t.first()) == tau::get(var);
	};
	auto is_var_or_neg = [&](tref n) {
		const auto& t = tau::get(n);
		if (t.child_is(tau::bf_neg)) return is_bare_var(t[0].first());
		return is_bare_var(n);
	};
	auto meets_point = [&](tref cst, tref other) {
		const auto& c = tau::get(cst);
		if (!c.is(tau::bf) || !c.has_child() || !is_var_or_neg(other))
			return false;
		auto p = qlt_point_constant<node>(c[0]);
		if (!p) return false;
		if (!point) { point = cst; return true; }
		return *p == *qlt_point_constant<node>(tau::get(point)[0]);
	};
	std::function<void(tref)> walk = [&](tref n) {
		if (!ok) return;
		const auto& t = tau::get(n);
		if (t.is(tau::variable)) {
			if (t == tau::get(var)) ok = false;
			return;
		}
		if (t.is(tau::bf) && t.child_is(tau::bf_and)) {
			tref a = t[0].first(), b = t[0].second();
			if (meets_point(a, b) || meets_point(b, a)) return;
		}
		for (tref c : t.children()) walk(c);
	};
	walk(body);
	return ok ? point : nullptr;
}

// The truth of a closed formula whose terms are qlt constants combined by the
// Boolean operations, or nullopt when it holds anything else.
template<NodeType node>
static std::optional<bool> qlt_ground_truth(tref fm) {
	using tau = tree<node>;
	std::function<std::optional<qlt>(tref)> term
		= [&](tref n) -> std::optional<qlt> {
		const auto& t = tau::get(n);
		if (!t.is(tau::bf) || !t.has_child()) return std::nullopt;
		if (t.child_is(tau::bf_t)) return qlt::top();
		if (t.child_is(tau::bf_f)) return qlt::bottom();
		if (t[0].is_ba_constant()) {
			auto v = t[0].get_ba_constant();
			if (!std::holds_alternative<qlt>(v)) return std::nullopt;
			const qlt& q = std::get<qlt>(v);
			if (q.inexact) return std::nullopt;
			return q;
		}
		std::optional<qlt> r;
		if (t.child_is(tau::bf_neg)) {
			auto a = term(t[0].first());
			if (a) r = ~*a;
		} else if (t.child_is(tau::bf_and) || t.child_is(tau::bf_or)
			|| t.child_is(tau::bf_xor))
		{
			auto a = term(t[0].first()), b = term(t[0].second());
			if (a && b) r = t.child_is(tau::bf_and) ? *a & *b
				: t.child_is(tau::bf_or) ? *a | *b : *a ^ *b;
		}
		if (!r || r->inexact) return std::nullopt;
		return r;
	};
	std::function<std::optional<bool>(tref)> truth
		= [&](tref n) -> std::optional<bool> {
		const auto& t = tau::get(n);
		if (t.equals_T()) return true;
		if (t.equals_F()) return false;
		if (!t.is(tau::wff) || !t.has_child()) return std::nullopt;
		const auto op = t[0].value.nt;
		if (op == tau::wff_neg) {
			auto a = truth(t[0].first());
			if (!a) return std::nullopt;
			return !*a;
		}
		if (op == tau::wff_and || op == tau::wff_or) {
			auto a = truth(t[0].first());
			if (!a || *a == (op == tau::wff_or)) return a;
			return truth(t[0].second());
		}
		if (op == tau::bf_eq || op == tau::bf_neq) {
			auto a = term(t[0].first()), b = term(t[0].second());
			if (!a || !b) return std::nullopt;
			const qlt d = *a ^ *b;
			if (d.inexact) return std::nullopt;
			return d.is_empty() == (op == tau::bf_eq);
		}
		return std::nullopt;
	};
	return truth(fm);
}

// The truth of closed body with var := the point v of type ba_type, or
// nullopt when qlt_ground_truth cannot decide it.
template<NodeType node>
static std::optional<bool> qlt_point_instance(tref var, tref body,
	const qlt_rational& v, size_t ba_type)
{
	using tau = tree<node>;
	qlt point;
	point.pieces.push_back({ qlt_endpoint{ v, qlt_bound::CLOSED },
		qlt_endpoint{ v, qlt_bound::CLOSED } });
	tref value = tau::get(tau::bf, { tau::get_ba_constant(
		typename tau::constant(point), ba_type) });
	subtree_map<node, tref> changes;
	for (tref occ : tau::get(body).select_all([&](tref n) {
		const auto& t = tau::get(n);
		return t.child_is(tau::variable)
			&& tau::get(t.first()) == tau::get(var); }))
		changes.emplace(occ, value);
	return qlt_ground_truth<node>(rewriter::replace<node>(body, changes));
}

// `ex var phi` (or `all var phi` when universal) for a closed phi in which var
// meets one point p only as `p & var` and `p & var'`. p is an atom whether var
// denotes a point or a set, so each such term is 0 or p by whether var
// contains p; var := p and var := p + 1 are the two cases.
template<NodeType node>
static std::optional<bool> qlt_point_meet_qe(tref var, tref body,
	bool universal)
{
	using tau = tree<node>;
	tref point = qlt_point_meet_of<node>(var, body);
	if (!point) return std::nullopt;
	const auto& pc = tau::get(point)[0];
	const qlt_rational p = *qlt_point_constant<node>(pc);
	const size_t type = pc.get_ba_type();
	auto in = qlt_point_instance<node>(var, body, p, type);
	if (!in || *in != universal) return in;
	return qlt_point_instance<node>(var, body, p + qlt_rational(1, 1), type);
}

// A point witness of `ex var phi` (T), or a point counterexample of
// `all var phi` (F), for a closed phi; nullopt when no candidate is one. A
// point is a value of var whether var denotes a point or a set, so only that
// direction is answered. The candidates are the finite endpoints of the qlt
// constants, the midpoints between consecutive ones and a point beyond each
// end.
template<NodeType node>
static std::optional<bool> qlt_point_witness_qe(tref var, tref body,
	bool universal)
{
	using tau = tree<node>;
	std::vector<qlt_rational> ends;
	size_t type = 0;
	for (tref c : tau::get(body).select_all([](tref n) {
		return tau::get(n).is_ba_constant(); }))
	{
		const auto& t = tau::get(c);
		auto v = t.get_ba_constant();
		if (!std::holds_alternative<qlt>(v)) continue;
		type = t.get_ba_type();
		for (const auto& piece : std::get<qlt>(v).pieces)
			for (const auto& e : { piece.lo.val, piece.hi.val })
				if (e.is_finite()) ends.push_back(e);
	}
	if (!type) return std::nullopt;
	std::sort(ends.begin(), ends.end());
	ends.erase(std::unique(ends.begin(), ends.end()), ends.end());
	// Each candidate re-evaluates the whole body.
	if (ends.size() > 32) return std::nullopt;
	std::vector<qlt_rational> candidates;
	if (ends.empty()) candidates.emplace_back(0, 1);
	else {
		candidates.push_back(ends.front() + qlt_rational(-1, 1));
		for (size_t i = 0; i < ends.size(); ++i) {
			if (i) candidates.push_back(ends[i - 1].midpoint(ends[i]));
			candidates.push_back(ends[i]);
		}
		candidates.push_back(ends.back() + qlt_rational(1, 1));
	}
	for (const auto& v : candidates)
		if (auto in = qlt_point_instance<node>(var, body, v, type);
			in && *in != universal) return in;
	return std::nullopt;
}

// True when body is a conjunction of disequations `var != t` with t free of
// var. Such a body excludes finitely many values from infinitely many, points
// or sets alike, so `ex var body` holds.
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
// the interval is asked about.
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
	auto interval = qlt_dlo_qe_interval<node>(var, inner);
	if (!interval) {
		if (!universal && qlt_only_excludes<node>(var, inner))
			return true;
		// An ordering atom is the interval computation's alone: a
		// substituted point would make a compound side such as
		// `x & {3}` comparable through the order's 0/1 ends.
		if (tau::get(inner).find_top([](tref n) {
			const auto& t = tau::get(n);
			return t.is(tau::bf_lt) || t.is(tau::bf_lteq)
				|| t.is(tau::bf_gt) || t.is(tau::bf_gteq)
				|| t.is(tau::bf_nlt) || t.is(tau::bf_nlteq)
				|| t.is(tau::bf_ngt) || t.is(tau::bf_ngteq); }))
			return std::nullopt;
		if (auto r = qlt_point_meet_qe<node>(var, inner, universal))
			return r;
		return qlt_point_witness_qe<node>(var, inner, universal);
	}
	return universal ? interval->is_full() : !interval->is_empty();
}

// Fourier-Motzkin elimination for the dense order without
// endpoints that qlt_dlo_qe_interval reasons in. For body a conjunction of
// atoms `L op var` / `var op U` (op among <, <=, >, >= and their negations)
// whose var side is var itself and whose other side does not mention var,
//   ex var (/\ L_i <_i var  /\  var <_j U_j)  ==  /\ L_i <_ij U_j
// where <_ij is strict iff either bound is strict. Density gives a point
// strictly between L and U, and the absence of endpoints the one-sided case
// (which qlt_dlo_qe_interval already decides, so only the two-sided case is
// answered here). A variable pinned to one term t -- by `var = t` or by
// `t <= var && var <= t` -- is eliminated by substituting t, whatever the
// other conjuncts are. Disequalities `var != c_k` beside one lower bound L
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
	// A bound on var read off one atom, normalised to `var op other` with
	// op among <, <=, >, >= and =; nullopt for any other atom.
	struct bound { size_t op; tref other; };
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
		if (is_sentinel(other)) return std::nullopt;
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
		if (pin && !rebinds(pin)) {
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
