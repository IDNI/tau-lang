// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file driver.h
 * @brief §4 PHASE 4 DRIVER: ONE POST-ORDER PASS over a formula, in which
 * every quantifier run is eliminated exactly once, over a matrix that is
 * already final.
 *
 * WHAT ARRIVES: a formula as the phases before it leave one — binder ids
 * canonical, in NNF, negation at formula level (invariant 4), every chain
 * join-canonical.
 *
 * WHAT GOES OUT: an equivalent formula with every run pushed as far inward as
 * §6 reaches and its result folded into the context it stood in. What no step
 * reaches is RE-WRAPPED, never answered `F` (invariant 3).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__DRIVER__DRIVER_H__
#define __IDNI__TAU__ANTI_PRENEX__DRIVER__DRIVER_H__

#include "../foundations/fwd.h"
#include "../foundations/dag.h"
#include "../normalisers/joins.h"
#include "block.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §4 `COLLECT_RUN(h)`: the MAXIMAL same-kind run from the binder `h` —
 * its kind, its variables OUTERMOST FIRST, and the first node below them that
 * is not a binder of that kind, which is the matrix.
 *
 * Only same-kind quantifiers commute, so a KIND CHANGE ENDS THE RUN: what is
 * left below is a head in its own right and is collected when the matrix is
 * processed. An intermediate binder of the run is therefore never a head of
 * its own.
 *
 * NO TYPE TEST: a run may mix BA types, and §5's partition splits it into
 * type-homogeneous components (invariant 2).
 *
 * @p h is a binder node — its `wff` wrapper, which is what the whole module
 * passes around. The variables and the matrix come back TRIMMED of their
 * right siblings, the convention of every block and every formula here.
 */
template <NodeType node>
run<node> collect_run(tref h);

/**
 * @brief §4 `PROCESS_ALL_BLOCKS(φ, kf)`: the pass — every run collected, its
 * matrix processed FIRST, then the run eliminated over that final matrix
 * (§5), and the result folded into its context through the result joins
 * (invariant 6).
 *
 * NO FIXPOINT IS NEEDED. A run is always eliminated over a final matrix, and
 * a binder that survives elimination is final AS A UNIT: its inner structure
 * is never re-opened, and one left standing at the top of a processed matrix
 * stays inside that matrix rather than joining the enclosing run. A term is
 * never entered, so a functional quantifier is untouched here.
 *
 * RUN HEADS ARE MEMOISED for the whole pass, so a shared subtree is processed
 * ONCE, not once per site (the spec's assumption 1). The memo is exact
 * because a head's result is a pure function of the head and of @p kf.
 *
 * @p kf is the caller's callback (§1), handed to every block unchanged.
 * A formula holding no formula binder comes back AS THE SAME NODE: every step
 * of the pass is then the identity or a re-join of an already canonical
 * chain.
 */
template <NodeType node>
tref process_all_blocks(tref phi, const keep_functional_fn<node>& kf);

} // namespace idni::tau_lang::anti_prenexing

#include "driver.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__DRIVER__DRIVER_H__
