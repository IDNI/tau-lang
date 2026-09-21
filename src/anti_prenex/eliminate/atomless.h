// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file atomless.h
 * @brief §7 `ELIMINATE_ATOMLESS_CLAUSE`: the leaf method of the ATOMLESS
 * Boolean algebras, and the `opaque?` predicate it hands to the freeze.
 *
 * ONE IDENTITY, applied once per clause (`cor:qelim-main`):
 *
 *     ∃X.( f = 0 ∧ ⋀ⱼ gⱼ ≠ 0 )  ≡  ∀_X f = 0 ∧ ⋀ⱼ ∃_X f′·gⱼ ≠ 0
 *
 * — the positive part squeezed into one term `f` and discharged as `pos`, and
 * ONE condition per negative literal, each absorbing against `f` alone. A
 * negative tree conjunct (§1) becomes its `TREE_CONDITION`, the same
 * condition taken at every literal and re-assembled in the tree's own shape.
 * Everything the method needs is in conditions.h; this file is the order the
 * spec puts those helpers in, plus the one predicate that is the method's
 * own.
 *
 * WHERE ATOMLESSNESS IS USED, twice. `EX_DISTRIBUTES_OVER_NEGATIVES`
 * (eliminate_block.h) licenses conjoining ONE condition per negative
 * (invariant 1) — in a finite algebra the negatives interact and a single
 * condition each would be too weak. The conditions themselves hold in ANY
 * Boolean algebra, which is why they live in the shared file.
 *
 * THE ENGINE ANSWERS NO QUERIES. It reads a conjunct through its term and its
 * BDD, and nothing else: a reference, an order atom, a surviving binder and a
 * temporal operator are all opaque to it, and so is an equation whose term
 * hides a block variable in a leaf. Each of them freezes its whole connected
 * component, which is then re-wrapped and left undecided (invariant 3).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ATOMLESS_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ATOMLESS_H__

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/terms.h"
#include "../normalisers/joins.h"
#include "conditions.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §7 the atomless method's `opaque?`: can this engine use the conjunct
 * @p conjunct at all?
 *
 * A (¬)EQUATION is usable unless its term hides a block variable in a LEAF —
 * `LEAF_FV(t) ∩ X ≠ ∅` (§1 leaf hazard). Quantification here is cofactoring,
 * and a leaf is opaque to it, so `DISCHARGE` would emit such an `x` free.
 * Both a reference argument and a kept functional quantifier's body hide one
 * that way.
 *
 * A NEGATIVE TREE (§1) is read LITERAL-WISE: opaque iff one of its literals
 * is, since the tree is taken whole and one unusable literal blocks the
 * conditions of all of them.
 *
 * ANYTHING ELSE is opaque — a reference, an ORDER ATOM, EVERY binder unit, a
 * temporal operator: this engine answers no queries, so it can swallow none
 * of them.
 *
 * @param order the component's live order, which `TERM_OF` requires
 */
template <NodeType node>
bool atomless_opaque(tref conjunct, const block& X,
	const var_order<node>& order);

/**
 * @brief §7 `ELIMINATE_ATOMLESS_CLAUSE(clause, X, ctx)`: a formula equivalent
 * to `∃X.⋀conjuncts` over an atomless type.
 *
 * The method contract (§7) holds on entry: `X` is non-empty, every `x ∈ X`
 * occurs in the clause and every conjunct mentions some `x ∈ X`. Variables of
 * the enclosing component that are not in `X` may occur and are read as free.
 *
 * Three steps. FREEZE what the engine cannot use, per connected component.
 * SQUEEZE the rest — the zero form, the positives grouped into components.
 * Then `pos`, one discharged atom per component, and one condition per
 * negative conjunct, the whole conjoined with the frozen part.
 *
 * `F` comes out only where the clause is genuinely unsatisfiable: a conjunct
 * that folded to `F` in the zero form, a component with no common zero, or a
 * negative whose condition is `F`. What the engine cannot decide is
 * RE-WRAPPED inside `frozen`, never answered `F` (invariant 3).
 */
template <NodeType node>
tref eliminate_atomless_clause(const trefs& conjuncts, const block& X,
	ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

#include "atomless.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__ATOMLESS_H__
