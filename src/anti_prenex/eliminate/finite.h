// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file finite.h
 * @brief §7 "Finite (pure Boolean)": `ELIMINATE_FINITE_CLAUSE`, the leaf
 * method of a finite type whose term language is pure, with its `opaque?`,
 * `SUBSUME_NEGATIVES`, `FINITE_TIER2`, and the closing that `FINITE_QUERY`
 * and `SOLVE_ARITHMETIC` share.
 *
 * THROUGH ONE NEGATIVE IT IS THE ATOMLESS MATHEMATICS, exact in every
 * Boolean algebra — the tier-1 lemma
 *
 *     ∃X.( f = 0 ∧ g ≠ 0 )  ≡  ∀_X f = 0 ∧ ∃_X f′·g ≠ 0
 *
 * — and a flat negative tree is one such clause per literal. Finiteness is
 * consulted at TIER 2 alone, where two negatives compete for the atoms of
 * the algebra: over `bv[1]`, `∃x(x ≠ 0 ∧ x′ ≠ 0)` is `F` while both single
 * conditions hold. There the clause is decided as a QUERY by
 * `DECIDE_FINITE` (blast.h), one per variable-disjoint part.
 *
 * A QUERY IS BUILT TO BE DECIDED, NEVER EMITTED: it is the finished spelling
 * of the prepared clause, and a query that stays open emits the re-wrap of
 * the prepared clause (invariant 3).
 *
 * WHAT IS SWALLOWED: a binder unit with a pure body and a conversion
 * emission are no atoms, so the squeeze cannot read them and the clause does
 * not factor around them. They force tier 2 and enter its query whole.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__FINITE_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__FINITE_H__

#include <optional>

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/terms.h"
#include "../foundations/prims.h"
#include "../shared/parts.h"
#include "../normalisers/joins.h"
#include "conditions.h"
#include "atomless.h"
#include "blast.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §7 the finite method's `opaque?`: the atomless condition plus
 * anything arithmetic, MINUS a binder unit with a recursively pure body and
 * MINUS a conversion emission, both swallowed into the tier-2 query.
 *
 * A (¬)EQUATION is opaque when it is impure (`is_pure_equation`) or when its
 * term hides a block variable in a leaf (`atomless_opaque`). A NEGATIVE TREE
 * is read literal-wise, opaque iff one of its literals is. A BINDER UNIT and
 * a CONVERSION EMISSION — an ∨-node that is no negative tree — are opaque
 * iff impure (`is_pure_formula`). Anything else — a reference, an order
 * atom, a temporal operator — is opaque.
 *
 * @param type  the block's type, which purity is read under
 * @param order the component's live order, which `TERM_OF` requires
 */
template <NodeType node>
bool finite_opaque(tref conjunct, const block& X, ba_type_id type,
	const var_order<node>& order);

/// §7 `SUBSUME_NEGATIVES`' result: the surviving negatives in clause order,
/// the clause without the dropped ones, and the block narrowed to what still
/// occurs.
template <NodeType node>
struct subsume_result {
	trefs negatives;
	trefs conjuncts;
	block X;
};

/**
 * @brief §7 `SUBSUME_NEGATIVES(f, clause, X, ctx)`: term-order subsumption
 * among the single negatives and the flat trees, modulo the positive part
 * @p f; skipped above `ctx.subsume_max` literals. Costs no precision.
 *
 * `gᵢ ⊑ gⱼ` is `f′·gᵢ·gⱼ′ = 0`: under `f = 0` it is `gᵢ ≤ gⱼ`, so
 * `gᵢ ≠ 0` implies `gⱼ ≠ 0`. In the CONJUNCTION of singles the larger goes;
 * inside a flat tree, a DISJUNCTION, the smaller goes; across, a tree implied
 * by a single goes, a single implied by a tree goes, and a tree implied by
 * another tree goes. Each rule tests against the survivors in clause order
 * alone, so of an equivalent pair the first stays. A flat tree reduced to
 * one literal is a single from then on; a nested tree takes part in none of
 * this.
 *
 * `X` is narrowed whether the scan ran or not: a variable whose last
 * occurrence went with a dropped negative sat in no positive.
 *
 * @param f the squeeze's `f`, the union of the clause's positive terms
 * @param conjuncts the squeezed clause, in zero form
 */
template <NodeType node>
subsume_result<node> subsume_negatives(tref f, const trefs& conjuncts,
	const block& X, ctx<node>& c);

/**
 * @brief §7 the closing shared by `FINITE_QUERY` and `SOLVE_ARITHMETIC`:
 * `∃X.clause` as ONE query on finished terms, decided closed as it is, or
 * open by closing its free variables both ways — valid gives `T`,
 * unsatisfiable gives `F` — else re-wrapped over the prepared clause
 * (invariant 3).
 *
 * @param decide answers a closed query `true`, `false`, or nothing, as a
 *        `std::optional<bool>(tref)`
 */
template <NodeType node, typename Decide>
tref query_or_rewrap(const trefs& conjuncts, const block& X,
	Decide&& decide);

/**
 * @brief §7 `FINITE_TIER2(clause, X, ctx)`: one query per variable-disjoint
 * part holding a non-positive conjunct, smallest first, then one joint
 * closing over the re-wraps that share a free variable.
 *
 * `∃X` distributes over the parts, and a part of positives alone is its
 * own `pos`, which the caller emits, so it is left out. The per-part
 * closings lose no validity, but UNSATISFIABILITY does not distribute: two
 * open parts may share a free variable satisfiable only under incompatible
 * assignments, and the one joint closing catches that. A part decided `F`
 * decides the clause, and the later parts are never queried (invariant 7).
 *
 * A conjunct the squeeze left free of the block — `x + b = x + c` gives
 * `b + c = 0` — is a part of its own over no variable, and its query is the
 * conjunct itself.
 */
template <NodeType node>
tref finite_tier2(const trefs& conjuncts, const block& X, ctx<node>& c);

/**
 * @brief §7 `ELIMINATE_FINITE_CLAUSE(clause, X, ctx)`: a formula equivalent
 * to `∃X.⋀conjuncts` over a finite type whose term language is pure — the
 * atomless mathematics through one negative, the query engine from two.
 *
 * The method contract (§7) holds on entry, as for the atomless method.
 * FREEZE what the engine cannot read; SQUEEZE and SUBSUME the rest; `pos`,
 * one atom per component, implied by the clause, so its `F` decides. Then by
 * the clause's shape: with no unit and no emission, at most one negative
 * that is a literal or a flat tree is TIER 0–1, its conditions exact; one
 * NESTED tree with literal members at its root is the MIDDLE TIER, those
 * literals decided exactly and the rest one query; everything else is
 * TIER 2.
 *
 * `F` comes out only where the clause is unsatisfiable; what no query
 * decides is re-wrapped, never answered `F` (invariant 3).
 */
template <NodeType node>
tref eliminate_finite_clause(const trefs& conjuncts, const block& X,
	ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

#include "finite.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__FINITE_H__
