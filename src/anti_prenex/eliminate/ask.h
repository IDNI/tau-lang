// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ask.h
 * @brief §7 `ASK`: the solver's verdict on a closed bitvector query, cached
 * in §1 `solver_memo`.
 *
 * The solver is cvc5 behind `bv_formula_sat_status` (bv_ba.h), a fresh
 * solver per query of which only the verdict is read. It reads the
 * connectives, the binders, the atoms and every term operator of the
 * bitvector types, and has no case for a functional quantifier or a
 * reference: a query holding one does not translate, which is `unknown`,
 * the same answer as a solver that gives up.
 *
 * Two caches sit on the way: `solver_memo` here, keyed by the canonical
 * query and flushed when the solver configuration changes (§1 cache scope),
 * and the library's own memo inside `bv_formula_sat_status`, keyed by the
 * node.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ASK_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ASK_H__

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/prims.h"
#include "../../boolean_algebras/bv_ba.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §7 `ASK(q)`: the solver's verdict on the CLOSED query @p q, under
 * `solver_memo` keyed by the query's canonical binder ids, so that
 * alpha-variants meet one entry.
 *
 * `unknown` is cached too: the table is valid per solver configuration and
 * is flushed when it changes. A query the solver cannot read — a functional
 * quantifier or a reference inside — is `unknown`. No ctx: nothing
 * component-scoped is read. A free variable in @p q is a caller bug,
 * Debug-asserted.
 */
template <NodeType node>
answer ask(tref q);

} // namespace idni::tau_lang::anti_prenexing

#include "ask.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ASK_H__
