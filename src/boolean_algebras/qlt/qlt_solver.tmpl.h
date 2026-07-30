// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_solver.tmpl.h
 * @brief qlt's solver for pure ordering systems, which the BA-level
 *        solve_inequality_system cannot handle.
 *
 * Included from qlt_descriptor.tmpl.h and nowhere else.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_SOLVER_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_SOLVER_TMPL_H__

#include "boolean_algebras/qlt/qlt.h"
#include "boolean_algebras/qlt/qlt_qe.tmpl.h"
#include "solver_types.h"

namespace idni::tau_lang {

// Defined in solver.tmpl.h, which is reached only after this file.
template <NodeType node> trefs get_variables(equality eq);

// Pick a concrete witness rational from the first non-empty, non-symbolic piece.
// Returns nullopt if none found.
template <NodeType node>
std::optional<qlt_rational> qlt_pick_witness(const qlt& interval) {
	for (const auto& p : interval.pieces) {
		const auto& lo = p.lo.val;
		const auto& hi = p.hi.val;
		if (lo.is_sym() || hi.is_sym()) continue;
		if (lo.is_neg_inf() && hi.is_pos_inf()) return qlt_rational(0, 1);
		if (lo.is_neg_inf() && hi.is_finite())
			return qlt_rational(hi.p - hi.q, hi.q); // hi - 1
		if (lo.is_finite() && hi.is_pos_inf())
			return qlt_rational(lo.p + lo.q, lo.q); // lo + 1
		// both finite
		if (lo == hi) {
			if (p.lo.bound == qlt_bound::CLOSED && p.hi.bound == qlt_bound::CLOSED)
				return lo;
			continue; // empty degenerate interval
		}
		return lo.midpoint(hi);
	}
	return {};
}

// The omcat_solve_inequality_system capability: picks a concrete witness per
// variable of a pure DLO ordering system, or declines when the system is not
// one the interval computation determines.
template <NodeType node>
static std::optional<solution<node>> qlt_omcat_solve_inequality_system(
	const inequality_system<node>& sys, const solver_options& options)
{
	using tau = tree<node>;

	// Collect unique variables (pointer identity suffices for interned nodes)
	trefs all_vars;
	for (tref atom : sys) {
		for (tref v : get_variables<node>(atom)) {
			bool already = false;
			for (tref u : all_vars) if (u == v) { already = true; break; }
			if (!already) all_vars.push_back(v);
		}
	}
	if (all_vars.empty()) return solution<node>{};

	// Build conjunction of all constraints for qlt_dlo_qe_interval
	tref body = tau::_T();
	for (tref atom : sys)
		body = tau::build_wff_and(body, atom);

	solution<node> result;
	for (tref var : all_vars) {
		// qlt_dlo_qe_interval expects a raw variable node; get_variables returns bf[variable]
		tref var_raw = tau::get(var).first();
		auto interval = qlt_dlo_qe_interval<node>(var_raw, body);
		// If undetermined (cross-variable or unsupported), bail out
		if (!interval) return {};
		if (interval->is_empty()) return {};

		auto witness = qlt_pick_witness<node>(*interval);
		if (!witness) return {};

		// Build singleton qlt and convert to tree constant
		qlt_piece p;
		p.lo = qlt_endpoint{*witness, qlt_bound::CLOSED};
		p.hi = qlt_endpoint{*witness, qlt_bound::CLOSED};
		qlt singleton{{p}};
		typename tau::constant c = qlt{singleton};
		// ba_constants::get returns a raw ba_constant; wrap in bf for solution value
		tref witness_raw = ba_constants<node>::get(c, options.type_id);
		tref witness_node = tau::get(tau::bf, witness_raw);
		result[var] = witness_node;
	}
	return result;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_SOLVER_TMPL_H__
