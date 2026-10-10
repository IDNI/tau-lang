// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ask.tmpl.h
 * @brief Template implementations for ask.h. Included by ask.h.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ASK_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ASK_TMPL_H__

#include <optional>

namespace idni::tau_lang::anti_prenexing {

template <NodeType node>
answer ask(tref k) {
	DBG(assert(get_free_vars<node>(k).empty());)
	// `solver_memo` is not taint-aware: the wrapper records every verdict,
	// `unknown` included, and only the flush retires one (§1).
	return memoised<node, table::solver_memo>(k, [k] {
		const std::optional<bv_sat_status> r =
			bv_formula_sat_status<node>(k);
		if (r == bv_sat_status::sat) return answer::sat;
		if (r == bv_sat_status::unsat) return answer::unsat;
		// A solver that gave up, or a query it could not translate.
		return answer::unknown;
	});
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ASK_TMPL_H__
