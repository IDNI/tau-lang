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
// Returns the interval if determined, nullopt if undetermined.
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
		// Helper lambda: given (raw_op, lhs, rhs, negate), accumulate the
		// corresponding DLO interval into acc. raw_op is the comparison
		// operator before direction-flip and optional negation.
		auto accumulate_interval = [&](size_t raw_op, tref lhs_t, tref rhs_t, bool negate) {
			bool var_in_lhs = contains<node>(lhs_t, var);
			bool var_in_rhs = contains<node>(rhs_t, var);
			if (!var_in_lhs && !var_in_rhs) return;
			if (var_in_lhs && var_in_rhs) { undetermined = true; return; }
			// var must be the only free variable on its side.
			// A compound like (o1 & i1) introduces i1 as an extra free var
			// and cannot be treated as a simple DLO constraint on o1.
			tref var_side = var_in_lhs ? lhs_t : rhs_t;
			for (tref sv : get_free_vars<node>(var_side))
				if (tau::get(sv) != tau::get(var)) { undetermined = true; return; }
			const auto& cst = tau::get(var_in_lhs ? rhs_t : lhs_t)[0];
			if (!cst.is_ba_constant()) {
				// Handle typed zero (bf_f = -∞) and typed one (bf_t = +∞) as DLO bounds
				if (cst.is(tau::bf_f) || cst.is(tau::bf_t)) {
					bool cst_is_min = cst.is(tau::bf_f); // bf_f = -∞, bf_t = +∞
					// bf_eq / bf_neq against a sentinel (bf_f/bf_t) don't fit the
					// open-interval DLO model — they assert the variable IS (or
					// ISN'T) the sentinel.  Return undetermined so the caller
					// falls through to BA-level satisfiability, which can
					// correctly discharge var = bf_t / var = bf_f by ∃-substitution.
					if (raw_op == tau::bf_eq || raw_op == tau::bf_neq) {
						undetermined = true; return;
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
				if (raw_op == tau::bf_neq && !negate) return;
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
				else if (eff_op == tau::bf_neq)   { /* var != fv: always sat over DLO */ }
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
				if (contains<node>(n, var)) undetermined = true;
				return;
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
				if (contains<node>(n, var)) undetermined = true;
				return;
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
			if (contains<node>(n, var)) undetermined = true;
			return;
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
	auto has_same = [](const subtree_set<node>& a, const subtree_set<node>& b) {
		for (tref t : a) if (b.contains(t)) return true;
		return false;
	};
	if (has_same(lower_strict, upper_strict)
	 || has_same(lower_strict, upper_nonstrict)
	 || has_same(lower_nonstrict, upper_strict)
	 || has_same(lower_strict, eq_free)
	 || has_same(upper_strict, eq_free))
		acc = qlt::bottom();
	// Two distinct equalities to different free vars → contradictory iff
	// we can't prove they're equal. Stay undetermined in this case.
	if (eq_free.size() > 1) undetermined = true;
	if (undetermined) {
		// If we already derived an empty interval symbolically, prefer that
		// (it is a definitive answer; the BA fallback would wrongly say SAT).
		if (acc.is_empty()) return acc;
		return std::nullopt;
	}
	return acc;
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
	if (!interval) return std::nullopt;
	return universal ? interval->is_full() : !interval->is_empty();
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_QE_TMPL_H__
