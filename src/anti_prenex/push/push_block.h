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
 *
 * THE HUB, the module's one documented exception to "each header includes
 * what it needs": the two declarations below come first, then the step files
 * `push/fast_paths.h` and `push/junctions.h` — each holding its own
 * `.tmpl.h` and not including this one back, their bodies calling
 * `push_block` — and last this file's own `.tmpl.h`, so every declaration
 * precedes every template body and any include order works. Including this
 * file alone brings the whole push with it.
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
 * 1. THE FAST PATHS (2a, 2b, `push/fast_paths.h`), for a formula that is NOT
 *    a conjunction: one census walk, and what fires ends in small clauses at
 *    the leaf. A conjunction tries them inside `PUSH_OVER_CONJUNCTION`
 *    instead, after the scope narrowing, so that what they are asked of
 *    carries no material the narrowing would have taken out.
 * 2. A NEGATIVE TREE (§1) goes whole to the leaf (§7): it is one conjunct,
 *    and pushing per member would hand the elimination one unit per
 *    non-literal member instead of one. Where 2a applies it has taken the
 *    tree above already.
 * 3. A DISJUNCTION is pushed per disjunct (2d, `push/junctions.h`), smallest
 *    first, so a disjunct deciding `T` leaves the dearer ones unpushed.
 * 4. A CONJUNCTION goes through the ladder a conjunction offers
 *    (`push/junctions.h`): scope narrowing, the fast paths, the consistency
 *    check, the witness step, the case witness — and the re-wrap of
 *    invariant 3 below all of them, in place of §6's licensed decomposition
 *    and expansion, which this module does not carry.
 * 5. A LITERAL or a BINDER UNIT is a one-literal (or one-unit) clause and
 *    goes to the leaf (§7), which may swallow a unit whole into a query. An
 *    order atom is a literal too, and comes back from the method's freeze as
 *    exactly the re-wrap below.
 * 6. Anything else — a reference, a temporal operator, or `¬` over one — is
 *    re-wrapped: pushed as far as it goes (invariant 3).
 *
 * `X` is non-empty and already narrowed to `X ∩ FV(φ)`, the wrapper's
 * contract.
 */
template <NodeType node>
tref push_block_uncached(tref phi, const block& X, ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

// The steps, after the declarations they call back into (see THE HUB above).
#include "fast_paths.h"
#include "junctions.h"

#include "push_block.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__PUSH_BLOCK_H__
