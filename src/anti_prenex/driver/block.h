// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file block.h
 * @brief §5 ONE BLOCK: the atom-connectivity partition of a block, the ∃ push
 * over the components it gives, and the ∀ run's dualisation onto that push.
 *
 * WHAT ARRIVES: one quantifier run — a kind, its variables outermost first,
 * and the matrix below them, which is already final (§4).
 *
 * WHAT GOES OUT: a formula equivalent to the run, pushed as far inward as §6
 * reaches, in §1's term representation still: the close does nothing to
 * terms, and the one spelling waits for phase 5 (§3 `FINISH_TERMS`).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__DRIVER__BLOCK_H__
#define __IDNI__TAU__ANTI_PRENEX__DRIVER__BLOCK_H__

#include <vector>

#include "../../ba_types.h"
#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/prims.h"
#include "../foundations/subst.h"
#include "../normalisers/joins.h"
#include "../normalisers/nnf.h"
#include "../normalisers/simplify.h"
#include "../push/push_block.h"
#include "../shared/cofactors.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §5 `CONNECTED_COMPONENTS(X, body)`: `X` partitioned by ATOM
 * CONNECTIVITY — two block variables land in one component when some ATOM of
 * @p body mentions both, an atom inside a nested binder or under a temporal
 * operator included.
 *
 * A REFERENCE merges nothing: it is no atom and its arguments are terms, so
 * one mentioning two components straddles them harmlessly — a block variable
 * inside a reference is frozen by every method (§7) and never resolved.
 *
 * Every variable handed over gets a component, a SINGLETON when no atom
 * mentions it; narrowing the block to what the body mentions is the caller's
 * business (§5). Each component keeps `X`'s order, and the components come in
 * the order of their first variable.
 *
 * Components share no atom, so they push one after the other without
 * interference and may reuse the ranks `1..|P|`; and no atom mentions
 * variables of two BA types, so every component is type-homogeneous, which is
 * what makes `ctx.type` well defined (invariant 2).
 */
template <NodeType node>
std::vector<block> connected_components(const block& X, tref body);

/**
 * @brief §5 `DELETE_UNATE_BINDERS(body, X)`: every variable of `X` that moves
 * the truth of @p body one way only, substituted by the end of the order that
 * witnesses `∃x` — `0` when growing it can only make each literal mentioning
 * it false, `1` when only true — in one simultaneous `φ[x ← t]`, then
 * `SIMPLIFY` with `X` in scope, both in the backed regime.
 *
 * The polarity is the COFACTOR TEST on each (¬)equation's `TERM_OF`: `COF`'s
 * restricts `f₀, f₁`, monotone when the node test finds `f₀·f₁′` zero,
 * antitone when it finds `f₁·f₀′` zero. An unusable record — the variable
 * hidden in a leaf — and an order atom, a unit, a reference or a temporal
 * operator make every variable of `X` they mention binate, and a binate
 * variable stays. @p body comes back as it is when nothing is deleted.
 */
template <NodeType node>
tref delete_unate_binders(tref body, const block& X);

/**
 * @brief §5 `PUSH_EX_BLOCK(body, X, kf)`: `∃X.body` pushed inward, one
 * component at a time.
 *
 * @p body arrives in §1's term representation (`PREPARE_TERMS`, run once at
 * phase 4's start). Its unate variables leave first (`delete_unate_binders`),
 * before the partition and the callback. `X` is then narrowed to
 * `X ∩ FV(body)` — a variable the body does not mention binds nothing and
 * gets no component. Every component of what is left is then set up (ctx.h
 * `for_component`), pushed (§6) and size-checked, and the next component
 * starts from what the previous one left.
 *
 * THE CALLBACK is asked PER COMPONENT, on `REWRAP(body, X)`: the ∃ wrap of
 * the whole narrowed block over the body as that component's setup finds it —
 * its terms backed, the dualised matrix for a ∀ run, the earlier components'
 * results already in place (§1). Two components of one block may be answered
 * differently, which is sound: every memo key carries its component's flag.
 * In keep mode it is asked again on every chain `DISCHARGE` emits (`ctx.kf`).
 *
 * SIZE ACCEPTANCE (§5) is one test per component on the COMPLETED push: a
 * result whose tree count sits past both `accept_growth · |input|` and
 * `accept_floor` is discarded for the input re-wrapped over the component
 * (invariant 3), the component-free conjuncts staying outside the wrap. Not a
 * taint event — the verdict is a pure function of the completed pair, and the
 * entries the discarded push wrote remain correct answers to their own keys.
 *
 * WHAT GOES OUT IS STILL BACKED: every chain was decided at its emission (§7
 * `DISCHARGE`), so the close does nothing to terms; they cross into the next
 * component and the enclosing blocks as they are, and phase 5 spells them
 * (§3 `FINISH_TERMS`).
 */
template <NodeType node>
tref push_ex_block(tref body, const block& X,
	const keep_functional_fn<node>& kf);

/// §4 `COLLECT_RUN`'s result: one maximal same-kind quantifier run — its
/// kind, its variables OUTERMOST FIRST, and the matrix below them.
template <NodeType node>
struct run {
	quantifier<node> kind;
	block vars;
	tref matrix;
};

/**
 * @brief §5 `PROCESS_BLOCK(blk, kf)`: the ∃ push, and a ∀ run by DUALISATION
 * — `∀X.ψ` is `¬∃X.¬ψ`, so the matrix is negated into NNF, the block pushed
 * as an ∃ one, and what comes back negated again.
 *
 * The two negations flip an already pushed inner unit's quantifiers in and
 * back out; the ∃ machinery treats such a unit as opaque and transports it,
 * and free-variable substitution is the one operation that reaches inside it
 * (§4). The OUTBOUND negation is where §3's factored negation pays: the
 * pushed result is an OR-join of arms and cases that share material by
 * construction, and factoring hands the enclosing block a flat disjunction
 * instead of a conjunction of one disjunction per member.
 */
template <NodeType node>
tref process_block(const run<node>& blk, const keep_functional_fn<node>& kf);

} // namespace idni::tau_lang::anti_prenexing

#include "block.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__DRIVER__BLOCK_H__
