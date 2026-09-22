// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file push_block.h
 * @brief §6 `PUSH_BLOCK`: the one entry of the push — the memo wrapper that
 * canonicalises the key, and the dispatcher that hands a formula to the class
 * of step its shape belongs to.
 *
 * WHAT ARRIVES: a block `X` — a component's, or a sub-block a step has
 * narrowed — scoping a formula `φ` whose terms are in §1's representation
 * under the COMPONENT's order (`ctx.order`), which stays the live order
 * however narrow `X` gets.
 *
 * WHAT GOES OUT: a formula equivalent to `∃X.φ`, pushed as far inward as the
 * steps reach. What no step reaches is RE-WRAPPED, never answered `F`
 * (invariant 3).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__PUSH__PUSH_BLOCK_H__
#define __IDNI__TAU__ANTI_PRENEX__PUSH__PUSH_BLOCK_H__

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/prims.h"
#include "../normalisers/joins.h"
#include "../eliminate/eliminate_block.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §6 `PUSH_BLOCK(φ, X, ctx)`: `∃X.φ` pushed inward. The `push_memo`
 * wrapper.
 *
 * KEY CANONICALISATION, two halves of one idea — nothing enters the key that
 * `∃X` does not scope:
 * 1. `X` is narrowed to `X ∩ FV(φ)`, keeping `X`'s order, which is also the
 *    tight downstream quantification. An empty narrowed block leaves `φ`
 *    alone; `T` and `F` take that exit, their free-variable set being empty.
 * 2. THE STRIP, `∃X(indep ∧ dep) = indep ∧ ∃X.dep`: a conjunction's members
 *    that the narrowed block does not touch ride outside it. What a formula
 *    says about FREE variables then never enters the key, so states that
 *    differ only in the facts they carry along converge to one entry.
 *
 * THE KEY is `(REWRAP(φ, X), keep_functional)` (§1): the wrap node carries
 * the ordered `X`, so formula and block are one key part, and the flag joins
 * it because the entry embeds what `DISCHARGE` emitted, which the flag
 * switches. The key names everything the push reads, so the table is GLOBAL —
 * across blocks, components and runs. It is taint-aware: a computation that
 * hit a budget is returned but not recorded (§1, cache scope). It is a cache
 * and does not exist in Debug builds, where this is a plain call.
 */
template <NodeType node>
tref push_block(tref phi, const block& X, ctx<node>& c);

/**
 * @brief §6 `PUSH_BLOCK_UNCACHED(φ, X, ctx)`: the dispatcher, cheapest class
 * first (invariant 8).
 *
 * 1. A NEGATIVE TREE (§1) goes whole to the leaf (§7): it is one conjunct,
 *    and pushing per member would hand the elimination one unit per
 *    non-literal member instead of one.
 * 2. A DISJUNCTION or a CONJUNCTION re-wraps. The push over a junction is not
 *    part of the module; the block comes back around the formula, undecided
 *    (invariant 3).
 * 3. A LITERAL or a BINDER UNIT is a one-literal (or one-unit) clause and
 *    goes to the leaf (§7), which may swallow a unit whole into a query. An
 *    order atom is a literal too, and comes back from the method's freeze as
 *    exactly the re-wrap below.
 * 4. Anything else — a reference, a temporal operator, or `¬` over one — is
 *    re-wrapped: pushed as far as it goes (invariant 3).
 *
 * `X` is non-empty and already narrowed to `X ∩ FV(φ)`, the wrapper's
 * contract.
 */
template <NodeType node>
tref push_block_uncached(tref phi, const block& X, ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

#include "push_block.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__PUSH_BLOCK_H__
