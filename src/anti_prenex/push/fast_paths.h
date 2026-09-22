// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file fast_paths.h
 * @brief §6 the two WHOLE-FORMULA fast paths — 2a, which distributes `∃X` to
 * every negated atom in one pass, and 2b, which collapses a positive
 * skeleton on TERMS — the sign census that says which one may fire, and §3's
 * `SQUEEZE_POSITIVES` behind 2b.
 *
 * Both end in SMALL CLAUSES handed to §7 — one atom, or the top-level
 * positives plus one — and neither eliminates anything itself. Both classify
 * per X-TOUCHING LEAF: a subtree no `x ∈ X` touches is a constant under `∃X`
 * and rides along neutral.
 *
 * WHAT ARRIVES is what `PUSH_BLOCK`'s wrapper leaves: `X` non-empty and
 * narrowed to `X ∩ FV(φ)`, and, for a conjunction, every conjunct touching
 * `X` — the strip has hoisted the others outside the block already.
 *
 * `push/push_block.h` is the file that includes this one.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__PUSH__FAST_PATHS_H__
#define __IDNI__TAU__ANTI_PRENEX__PUSH__FAST_PATHS_H__

#include <optional>

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/terms.h"
#include "../normalisers/joins.h"
#include "../eliminate/eliminate_block.h"

namespace idni::tau_lang::anti_prenexing {

/// §6 `SIGN_CENSUS`'s counts: one per X-touching leaf, one per maximal
/// X-free subtree.
struct census {
	size_t pos   = 0;  ///< equations
	size_t neg   = 0;  ///< negated equations
	size_t other = 0;  ///< an order atom, a unit, a reference, a temporal
	                   ///< operator, or `¬` over one of these
	size_t free  = 0;  ///< maximal subtrees `X` does not touch
};

/**
 * @brief §6 `SIGN_CENSUS(φ, X, p)`: ONE walk over the ∧/∨ structure,
 * classifying per X-touching leaf. @p p is the number of top-level positive
 * literals the caller tolerates.
 *
 * A maximal X-free subtree is NEUTRAL: counted once in `free` and not
 * descended. An X-touching ∧/∨ is descended and counts nothing; every other
 * X-touching node is a leaf of the classification and is not descended.
 *
 * EARLY ABORT (invariant 8): the walk stops classifying the moment the counts
 * refute 2a and 2b at once — `other > 0`, or `pos > p` together with
 * `neg > 0` or `free > 0` — so the counts of an aborted census say which
 * refutation fired and nothing more.
 *
 * A node reached twice is classified once (§10), which is the count 2a asks
 * for: a duplicate of a top-level positive IS that top-level literal.
 * Uncached (§9).
 */
template <NodeType node>
census sign_census(tref phi, const block& X, size_t p);

/**
 * @brief §6 `TRY_FAST_PATHS(φ, X, ctx)`: `∃X.φ` by 2a or by 2b, or `nullopt`
 * when neither fires.
 *
 * `P` is the top-level conjunct list of PURE positive equations (§1), empty
 * unless `φ` is a conjunction; an impure positive stays an ordinary positive
 * leaf and refutes 2a. 2b takes any positive, pure or not: the term identity
 * behind it holds in every BA.
 *
 * 2a asks the type table whether `∃` distributes over negatives and gives up
 * where it does not — the one type-specific question the push asks
 * (invariant 1), asked before anything is eliminated.
 */
template <NodeType node>
std::optional<tref> try_fast_paths(tref phi, const block& X, ctx<node>& c);

/**
 * @brief §6 `DISTRIBUTE_TO_ATOMS(φ, X, ctx, P)`: `φ`'s ∧/∨ skeleton rebuilt
 * with every X-touching LITERAL `l` — atom plus its enclosing `¬`, if any —
 * replaced by `ELIMINATE_BLOCK(⋀P ∧ l, X, ctx)`, so that the top-level
 * positives ride into every leaf's clause.
 *
 * X-free subtrees are left verbatim, with no vacuous binder, and the nodes
 * above are reassembled with the result joins, so a decided literal folds
 * through the skeleton.
 *
 * WHAT MAY STAND IN `φ`: ∧-nodes, ∨-nodes, literals and X-free subtrees —
 * 2a's census has refuted everything else.
 */
template <NodeType node>
tref distribute_to_atoms(tref phi, const block& X, ctx<node>& c,
	const trefs& P);

/**
 * @brief §3 `SQUEEZE_POSITIVES(φ)`: terms `t₁ … t_k` with
 * `φ ≡ t₁ = 0 ∨ … ∨ t_k = 0`, for an ∧/∨ skeleton whose leaves are positive
 * equations (Debug-asserted).
 *
 * ∧ is distributed over ∨ on TERMS, a conjunction squeezing to the union
 * (`prop:squeeze-pos`), so the list is as long as the skeleton has DNF
 * clauses — the honest cost of 2b (invariant 8). A shared sub-skeleton is
 * squeezed once.
 *
 * Every term goes through `SIMPLIFY_TERM` once, at the end: the unions are
 * built raw, and the list is NOT deduplicated — the term sweep folds a
 * repeated factor inside a term, and the join over the eliminations merges
 * equal atoms across the list.
 *
 * @param order the live order, under which the terms are read and simplified
 */
template <NodeType node>
trefs squeeze_positives(tref phi, const var_order<node>& order);

} // namespace idni::tau_lang::anti_prenexing

#include "fast_paths.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__FAST_PATHS_H__
