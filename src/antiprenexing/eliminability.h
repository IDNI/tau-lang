// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file eliminability.h
 * @brief Per-block union-find analysis deciding which block variables may be
 * eliminated, and why the others may not.
 *
 * Replaces the conjunct-level guards `push_ex_block_into_clause`'s
 * `is_quant_removable_in_clause` and `treat_ex_quantified_clause`'s
 * `blocks_elimination` used to apply. Those were all-or-nothing per clause;
 * this is per *component*, so a reference freezes only the variables that
 * actually reach it.
 *
 * Unlike its `block_*` / `boole_*` siblings, which self-include their `.tmpl.h`,
 * this header does not: `normal_forms.h` includes `eliminability.tmpl.h` after
 * `normal_forms.tmpl.h`, because from Task 2 onward the definitions here are
 * consumed by `antiprenexing.tmpl.h`, which has that ordering requirement.
 */

#ifndef __IDNI__TAU__ANTIPRENEXING_ELIMINABILITY_H__
#define __IDNI__TAU__ANTIPRENEXING_ELIMINABILITY_H__

#include <algorithm>

#include "tau_tree.h"
#include "union_find_with_sets.h"

namespace idni::tau_lang {

/**
 * @brief Why a block variable may or may not be eliminated.
 *
 * The four meanings the single `skip` predicate used to conflate. The
 * distinction that matters most is `frozen` ("do not touch") versus
 * `solver_owned` ("this has a destination -- send it there"): honouring a
 * `frozen`-style skip on bitvector content would disable the very blasting the
 * pipeline relies on.
 *
 * The underlying values encode the join order and are relied on by `join`.
 */
enum class elim_verdict {
	eliminable    = 0, ///< Nothing blocks removing this variable.
	solver_owned  = 1, ///< Bitvector content the solver/blasting should decide.
	arith_residue = 2, ///< Bitvector arithmetic blasting cannot express.
	frozen        = 3  ///< Entangled with an unresolved reference or a kept binder.
};

/**
 * @brief Join two verdicts. Total, always succeeds, no error type.
 *
 * Order, bottom to top:
 * `eliminable < solver_owned < arith_residue < frozen`.
 *
 * `frozen` is top because a reference makes content untouchable by any
 * destination, the solver included. `arith_residue` sits above `solver_owned`
 * because arithmetic the solver cannot express must not be routed to it -- the
 * narrower verdict wins where both apply.
 */
inline elim_verdict join(elim_verdict a, elim_verdict b) {
	return static_cast<elim_verdict>(
		std::max(static_cast<int>(a), static_cast<int>(b)));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ANTIPRENEXING_ELIMINABILITY_H__
