// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file expand.h
 * @brief §6 `EXPAND`, the floor of the conjunction's ladder — `∃X` distributed
 * over ONE disjunctive conjunct, one case per member — with `FOLD_DECIDED`,
 * the probe that folds a (¬)equation its cofactors prove `F`.
 *
 * WHAT ARRIVES is what the ladder leaves: a conjunction under a non-empty `X`
 * narrowed to `X ∩ FV(ψ)`, every conjunct touching `X`, at least one of them
 * an ∨-node, and no cheaper step having taken it.
 *
 * `push/push_block.h` is the file that includes this one: every case goes
 * back through `push_block`.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__PUSH__EXPAND_H__
#define __IDNI__TAU__ANTI_PRENEX__PUSH__EXPAND_H__

#include <optional>

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/terms.h"
#include "../foundations/subst.h"
#include "../normalisers/joins.h"
#include "../normalisers/simplify.h"
#include "../shared/cofactors.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §6 `FOLD_DECIDED(C, X, ctx)`: the conjunct list with every
 * (¬)equation proven `F` by its one-variable probe folded out — a positive
 * one makes the list `F` (`nullopt`); a negated one rides out as `T`; a
 * negative tree's literal folds the tree up, and a tree folded to `T` is
 * dropped.
 *
 * THE PROBE, on the innermost block variable `x̂` of the term `f`: `f = 0`
 * forces `f₀·f₁ = 0` on every variable, so a residual `p` that is a nonzero
 * constant proves the atom `F` — the product of two cofactors, which
 * `SIMPLIFY_ATOM` does not see. One `COF` per (¬)equation, memoised there.
 */
template <NodeType node>
std::optional<trefs> fold_decided(const trefs& C, const block& X,
	ctx<node>& c);

/**
 * @brief §6 `EXPAND(ψ, X, ctx)`: `∃X` distributed over ONE disjunctive
 * conjunct `D`, one case per member, each handed back to `PUSH_BLOCK`; the
 * floor of the conjunction's ladder.
 *
 * `E` is the plain conjuncts through `FOLD_DECIDED`, whose `F` is the answer.
 * `D` minimises (the prio of its innermost block variable, its member count,
 * content order), so the expansion order is fixed by content and converging
 * states build equal memo keys. Its members are taken smallest first; each
 * case is `SIMPLIFY`d over `E` and the member alone. The cases need not be
 * disjoint, since `∃X` distributes over `∨`, so none carries the complement
 * of another. A dead case builds nothing; a member proven `T` makes `D` `T`;
 * a case deciding `T` ends the expansion. Each case reads the clock first
 * (§3 time limit).
 */
template <NodeType node>
tref expand(tref psi, const block& X, ctx<node>& c);

} // namespace idni::tau_lang::anti_prenexing

#include "expand.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__EXPAND_H__
