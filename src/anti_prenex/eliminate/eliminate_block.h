// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file eliminate_block.h
 * @brief §7 `ELIMINATE_BLOCK`: the one entry of the elimination core — the
 * type table that picks a method, the memo wrapper, and the type-agnostic
 * pre-steps every method is spared.
 *
 * WHAT ARRIVES: a block — or a settled sub-block (§6, scope narrowing) —
 * scoping a single conjunctive clause. The clause travels as a NODE here,
 * because the memo key is built from it; the member view is taken ONCE, in
 * `eliminate_block_uncached`, and the methods work on the conjunct list
 * (conditions.h). A clause that is no conjunction — an atom, a negated atom,
 * a negative tree, a binder unit — is its own one-member view.
 *
 * WHAT GOES OUT: a formula equivalent to `∃X.clause`. What no method can
 * decide is RE-WRAPPED, never answered `F` (invariant 3).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ELIMINATE_BLOCK_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ELIMINATE_BLOCK_H__

#include "../../ba_types.h"
#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/prims.h"
#include "../normalisers/joins.h"
#include "../witness/witness.h"
#include "atomless.h"

namespace idni::tau_lang::anti_prenexing {

/// §7 the method a type's row names: `atomless`
/// (`ELIMINATE_ATOMLESS_CLAUSE`), `bitvector` (the router), and `none` for
/// the table's `⊥`, which re-wraps.
enum class method_kind { none, atomless, bitvector };

/**
 * @brief §7 `METHOD(τ)`: the leaf method for the Boolean-algebra type @p type.
 *
 * THE TABLE, one row per type. `tau` and `sbf` are the ATOMLESS types — no
 * "atomless" predicate exists anywhere in `src/`, so this row is the module's
 * statement of which types are atomless, and `EX_DISTRIBUTES_OVER_NEGATIVES`
 * reads the same row. The bitvector family names §7's router, for which no
 * method is wired, so it re-wraps like the `⊥` row.
 *
 * EVERY OTHER TYPE — the untyped id `0`, `bool`, `nat` — is OUTSIDE the
 * algorithm: not a row but a CALLER BUG, a block of such a type must never
 * reach the push. Debug asserts; Release answers `none`, so that the one
 * graceful exit (a re-wrap, invariant 3) still applies.
 *
 * Adding a type is one row here plus one method obeying §7's method contract.
 */
template <NodeType node>
method_kind method(ba_type_id type);

/**
 * @brief §7 `EX_DISTRIBUTES_OVER_NEGATIVES(τ)`: may `∃` be distributed over a
 * conjunction of negative literals of type @p type?
 *
 * True for the atomless types alone — `cor:Multivariate-BFs-over`. It fails
 * closed everywhere else: over `bv[1]`, `∃x(x ≠ 0 ∧ x′ ≠ 0)` is `F` while
 * each conjunct alone is satisfiable. The same row as `METHOD`'s, and the
 * same licence the atomless method uses when it conjoins one condition per
 * negative (invariant 1).
 */
template <NodeType node>
bool ex_distributes_over_negatives(ba_type_id type);

/**
 * @brief §7 `ELIMINATE_BLOCK(clause, X, ctx)`: `∃X.clause`, eliminated as far
 * as the type's method reaches. The `elim_memo` wrapper.
 *
 * `T` and `F` come back as they are, and `X` is first narrowed to
 * `X ∩ FV(clause)`, so that nothing enters the key which `∃X` does not
 * scope. An empty narrowed block leaves the clause alone.
 *
 * THE KEY is `(REWRAP(clause, X), keep_functional)` (§1): the wrap node
 * carries the ordered `X`, and the flag switches what `DISCHARGE` emits. The
 * key names everything an elimination reads, so the table is GLOBAL — across
 * blocks, components and runs. It is taint-aware: a computation that hit a
 * budget is returned but not recorded (§1, cache scope). It is a cache and
 * does not exist in Debug builds, where this is a plain call.
 */
template <NodeType node>
tref eliminate_block(tref clause, const block& X, ctx<node>& c);

/**
 * @brief §7 `ELIMINATE_BLOCK_UNCACHED(clause, X, ctx)`: the two pre-steps and
 * the dispatch.
 *
 * 1. THE WITNESS PASS, once per block variable: a witness beats any method,
 *    being cheaper and exact (invariant 8). A variable it eliminates leaves
 *    the block, and `T`/`F` ends the pass.
 * 2. THE STRIP. The witness substitutes into every conjunct, the pinning one
 *    included, so a conjunct can lose its last block variable — the pin's
 *    residual, or a sibling. Those ride outside the block,
 *    `∃X(indep ∧ dep) = indep ∧ ∃X.dep`, and `X` is narrowed to what the
 *    dependent conjuncts still hold. After it §7's method contract holds:
 *    every remaining conjunct mentions some `x ∈ X`.
 * 3. THE DISPATCH, on `METHOD(ctx.type)`. A type with no method re-wraps the
 *    dependent part (invariant 3).
 *
 * Both pre-steps are pure identities and are run here, once, rather than in
 * every method.
 */
template <NodeType node>
tref eliminate_block_uncached(tref clause, const block& X, ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

#include "eliminate_block.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ELIMINATE_BLOCK_H__
