// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file conditions.h
 * @brief §7 "Shared helpers": the five procedures a leaf method assembles its
 * answer out of — the freeze that puts an unusable component beyond the
 * method's reach, the squeeze that normalises the clause and groups its
 * positives, and the three conditions that discharge one component, one
 * negative literal and one negative tree.
 *
 * Shared by the atomless and the finite method: the mathematics down to here
 * holds in ANY Boolean algebra, and what separates the two engines — which
 * conjunct shapes are opaque, whether one condition per negative is licensed —
 * stays with the method.
 *
 * A CLAUSE TRAVELS AS ITS CONJUNCT LIST. The member view of the clause is
 * taken once, by the caller, and a formula is assembled again only where one
 * is emitted, through the result joins (joins.h); the spec's `⋀ { … }`
 * groupings are lists here. `ctx.order` is the live order of the component
 * (§5), which every term operation needs, so `SQUEEZE` — which has no ctx —
 * takes it as its own parameter.
 *
 * INVARIANT 4 holds throughout: an equation is rewritten in `SQUEEZE` and
 * nowhere else, an order atom is never rewritten at all, and a negative atom
 * that goes out is `¬(h = 0)`, never a fused `h ≠ 0`.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__CONDITIONS_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__CONDITIONS_H__

#include <vector>

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/terms.h"
#include "../foundations/prims.h"
#include "../shared/parts.h"
#include "../shared/cofactors.h"
#include "../normalisers/joins.h"

namespace idni::tau_lang::anti_prenexing {

// --- the freeze -------------------------------------------------------------------

/**
 * @brief §7 `FREEZE_OPAQUE_COMPONENTS`' result: what the method gives up on,
 * and the clause and block it goes on with.
 *
 * `frozen` is the re-wrapped conjunction of the frozen components — `T` when
 * nothing is frozen, so that a caller can conjoin it unconditionally.
 * `conjuncts` are the remaining conjuncts in input order and `X` is what is
 * left of the block in `X`'s order. The two are disjoint in every sense: a
 * part is closed under shared block variables, so no variable bound in
 * `frozen` occurs in `conjuncts` and none of the returned `X` occurs in
 * `frozen`.
 */
template <NodeType node>
struct freeze_result {
	tref frozen;
	trefs conjuncts;
	block X;
};

/**
 * @brief §7 `FREEZE_OPAQUE_COMPONENTS(clause, X, opaque?)`: every connected
 * component (§6 `PARTS`) holding a conjunct the method cannot use is re-wrapped
 * whole and taken out of the clause.
 *
 * WHY PER COMPONENT: `∃` does not distribute over `∧` across shared variables.
 * A transparent conjunct sharing an `x` with an opaque one cannot keep its own
 * `∃x` — re-wrapping around the opaque conjunct alone would leave that
 * occurrence outside the wrap — and its other block variables bind further
 * conjuncts the same way. Freezing the whole component is exactly that
 * closure.
 *
 * @param opaque a predicate `bool(tref conjunct)`, the METHOD's own: which
 *        shapes are unusable is method-specific — an order atom blocks the
 *        atomless squeeze yet is native to a solver, a surviving binder is
 *        opaque to a BDD engine yet swallowed whole by a query-based one.
 */
template <NodeType node, typename Opaque>
freeze_result<node> freeze_opaque_components(const trefs& conjuncts,
	const block& X, Opaque&& opaque);

// --- the squeeze ------------------------------------------------------------------

/// §7 `SQUEEZE`'s COMPONENT `(F_k, X_k)`: the union `F` of one part's positive
/// terms, and the block variables `vars` that part holds, in `X`'s order. The
/// `vars` of two components are disjoint, which is what makes every condition
/// below exact over one component alone.
template <NodeType node>
struct component {
	tref F;
	block vars;
};

/**
 * @brief §7 `SQUEEZE`'s result: the clause in zero form, its positives
 * squeezed and grouped, and its negatives.
 *
 * `f` is `⋃_k F_k` — the whole positive part, `0` when the clause has no
 * positive — which only the finite method reads; `comps` are the parts of the
 * POSITIVES alone, negatives ignored; `negatives` are the negative conjuncts,
 * literals and trees alike, in the clause's order; `conjuncts` is the whole
 * clause as the rewrite left it.
 *
 * `decided_false` says a conjunct folded to `F` while it was being rewritten,
 * so the clause is unsatisfiable and `∃X.clause` is `F`. Nothing else is
 * filled in that case: the caller returns `F` at once.
 */
template <NodeType node>
struct squeeze_result {
	tref f = nullptr;
	std::vector<component<node>> comps;
	trefs negatives;
	trefs conjuncts;
	bool decided_false = false;
};

/**
 * @brief §7 `SQUEEZE(clause, X)`: the clause in zero form, with its positives
 * squeezed into one term per connected component.
 *
 * THE ONE PLACE AN EQUATION IS REWRITTEN (invariant 4): every (¬)equation
 * becomes `l + r = 0` — a negative tree's literals with it, the tree's own
 * `∧`/`∨` structure left as it stands — and each rewritten atom is simplified
 * again, because the ring sum builds terms the path simplifier has not seen
 * (invariant 6). An atom that folds to `T` in the process drops out of the
 * clause and one that folds to `F` decides it. A conjunct that is no atom —
 * a swallowed binder unit, a conversion emission of the finite method — rides
 * along untouched.
 *
 * The grouping is FINER than the push's own incidence pass, which counts a
 * negative as a connection: quantifiers over disjoint variable sets distribute
 * over `∪` and `·`, so nothing of one component ever enters another's term,
 * and that is what every condition below is exact over.
 *
 * @param order the component's live order, which `TERM_OF` and the term
 *        simplifier both require
 */
template <NodeType node>
squeeze_result<node> squeeze(const trefs& conjuncts, const block& X,
	const var_order<node>& order);

// --- the three conditions ---------------------------------------------------------

/**
 * @brief §7 `POSITIVE_CONDITION(comps, ctx)`: `pos`, ONE atom per component.
 *
 * The meet distributes over a union of terms with disjoint decision supports,
 * `∀_X f = ⋃_k ∀_{X_k} F_k`, so `∀_X f = 0` is one discharged atom per
 * component — each over its own variables, on its own term. `T` when there is
 * no component: with no positives `f = 0` holds vacuously.
 *
 * `F` means some component has no common zero, so `∃X.clause` is `F`; the
 * builder stops at that operand and the later components are never
 * discharged (invariant 7).
 */
template <NodeType node>
tref positive_condition(const std::vector<component<node>>& comps, ctx<node>& c);

/**
 * @brief §7 `NEGATIVE_CONDITION(g, comps, X, ctx)`: the condition for ONE
 * negative literal `¬(g = 0)`, modulo the sibling `pos`.
 *
 * Exact in any Boolean algebra, and it absorbs against the components `g`
 * TOUCHES only: with `f = ⋃_k F_k` over disjoint `X_k`, `K` the components
 * sharing a block variable with `g`, `F_K = ⋃_{k∈K} F_k` and `X_g = X ∩ FV(g)`,
 *
 *     ∃_X f′·g  =  ∃_{X_K ∪ X_g}(F_K′·g) · ∏_{k∉K} (∀_{X_k} F_k)′
 *
 * where every factor of the product is 1 under `pos`. The rest of the clause
 * never enters the absorption term.
 *
 * Two shortcuts rest on `∃_X f′ = (∀_X f)′` and on `∃_X(f′·g) = g · ∃_X f′`
 * for a `g` independent of `X`: a `g` that is 1 makes the condition `T` under
 * `pos`, and a `g` no component touches is discharged on its own. An `X`-free
 * `g` is the latter with `X_g = ∅`, which emits `¬(g = 0)` itself.
 *
 * @param g the term of the literal, `TERM_OF(¬(g = 0))`
 */
template <NodeType node>
tref negative_condition(tref g, const std::vector<component<node>>& comps,
	const block& X, ctx<node>& c);

/**
 * @brief §7 `TREE_CONDITION(t, comps, X, ctx)`: the condition for a negative
 * tree conjunct (§1), modulo `pos` — the tree with every literal replaced by
 * its `NEGATIVE_CONDITION`, its `∨` nodes re-assembled by the ∨-join and its
 * `∧` nodes by the ∧-join, bottom-up.
 *
 * THE TREE IS NEVER FLATTENED: distributing `∃X` over its disjunctions and
 * applying the clause identity to each DNF clause is the PROOF, not the
 * computation. Every condition depends on its own literal and the positives
 * alone, and `pos` is the one conjunct every DNF clause shares, so a condition
 * exact modulo `pos` stays exact inside the joins.
 *
 * `comps` and `X` are fixed for the call, so the rebuild is memoised per node
 * WITHIN it and a tree's repeated subtrees and literals are paid for once.
 * What comes back is whatever the joins leave: a negative tree over the free
 * variables, one condition, or a constant.
 */
template <NodeType node>
tref tree_condition(tref t, const std::vector<component<node>>& comps,
	const block& X, ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

#include "conditions.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__CONDITIONS_H__
