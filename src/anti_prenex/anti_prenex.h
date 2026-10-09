// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file anti_prenex.h
 * @brief Anti-prenexing, written from `anti_prenex.md` (the spec).
 *
 * The module's single entry header: it includes every layer in dependency
 * order and declares the top-level procedure `anti_prenex` (spec §3,
 * `ANTI_PRENEX`). The spec is the sole source of truth. Every function
 * carries a comment naming the spec section it implements, and names mirror
 * the spec one to one in snake case (`PUSH_BLOCK` → `push_block`). A `§n`
 * reference anywhere in the module points into the spec.
 *
 * The namespace is `idni::tau_lang::anti_prenexing`, distinct from the entry
 * function `anti_prenex` of the module in `src/antiprenexing/`, which this
 * module does not include.
 *
 * What the push cannot reach comes back re-wrapped, undecided rather than
 * answered (invariant 3). The finite and bitvector methods of §7 are not part
 * of the module, so `eliminate_block` re-wraps a block of a type without a
 * method. A call past its time limit returns `nullptr` instead (§3).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_H__
#define __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_H__

#include <chrono>

// foundations
#include "foundations/fwd.h"
#include "foundations/options.h"
#include "foundations/dag.h"
#include "foundations/terms.h"
#include "foundations/subst.h"
#include "foundations/prims.h"
#include "foundations/ctx.h"
// normalisers
#include "normalisers/joins.h"
#include "normalisers/nnf.h"
#include "normalisers/simplify.h"
// witness
#include "witness/witness.h"
// the elimination core
#include "shared/parts.h"
#include "shared/cofactors.h"
#include "eliminate/conditions.h"
#include "eliminate/atomless.h"
#include "eliminate/eliminate_block.h"
// the push — the hub, which includes the steps
#include "push/push_block.h"
// the driver
#include "driver/block.h"
#include "driver/driver.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 `ANTI_PRENEX(φ, keep_functional = (· ↦ false), time_limit = ∞)`
 * — the whole pipeline.
 *
 * Phases, in this fixed order: 0 `CANONICALISE_BINDER_IDS`; 1
 * `RESOLVE_FUNCTIONAL_PLAIN`, `TO_NNF` then `SIMPLIFY(φ, ref_args = true)`; 2
 * `ELIMINATE_BY_SUBSTITUTION` then `SIMPLIFY`; 3 `NORMALIZE_OPERATORS`; 4
 * `PROCESS_ALL_BLOCKS`; 5 `SIMPLIFY`, `FINISH_TERMS`,
 * `FOLD_DEGENERATE_BINDERS`, `CANONICALISE_BINDER_IDS`. A formula without a
 * quantifier is returned as is.
 *
 * @param phi a `wff` node
 * @param kf  §1 `keep_functional` (fwd.h `keep_functional_fn`): a yes allows
 *            a functional-quantifier chain to stay symbolic instead of being
 *            resolved, a closed piece of it still folding (§1). The default
 *            keeps nothing
 * @param time_limit §3 `time_limit`, the one cutoff on the call's total
 *            work, none by default. Past it the call aborts WHOLE: nothing
 *            partial is returned, no computation it cuts short writes a
 *            table entry, and the caller still holds @p phi, sound as it
 *            stands (invariant 3). The clock is read at each case `EXPAND`
 *            builds, each arm `DECOMPOSE_ARMS` pushes and each case-witness
 *            branch, so an input that reaches none of them returns in full
 *            whatever the limit
 * @return the anti-prenexed formula, a `wff` node, or `nullptr` past
 *         @p time_limit
 */
template <NodeType node>
tref anti_prenex(tref phi,
	const keep_functional_fn<node>& kf = keep_no_functional<node>,
	std::chrono::steady_clock::duration time_limit =
		std::chrono::steady_clock::duration::max());

} // namespace idni::tau_lang::anti_prenexing

#include "anti_prenex.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__ANTI_PRENEX_H__
