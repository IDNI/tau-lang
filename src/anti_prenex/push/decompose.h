// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file decompose.h
 * @brief §6 `TRY_DECOMPOSE`, one LICENSED Boole decomposition on an atom whose
 * two signs span sibling disjunctive conjuncts, and `DECOMPOSE_ARMS`, its two
 * arms — the step of the conjunction's ladder above the expansion.
 *
 * WHAT ARRIVES is what the ladder leaves below the case witness: a
 * conjunction under a non-empty `X` narrowed to `X ∩ FV(ψ)`, every conjunct
 * touching `X`, and no cheaper step having taken it.
 *
 * THE IDENTITY, exact for any atom `a` of `ψ` in any BA (invariant 5):
 * `∃X ψ = ∃X (a ∧ ψ[a ↦ T]) ∨ ∃X (¬a ∧ ψ[a ↦ F])`. Both cofactors erase
 * every reachable occurrence of `a`, and an arm keeps at most one
 * one-signed guard literal, so the census, which asks for both signs, never
 * selects the same atom again.
 *
 * `push/push_block.h` is the file that includes this one: every arm goes
 * back through `push_block`.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__PUSH__DECOMPOSE_H__
#define __IDNI__TAU__ANTI_PRENEX__PUSH__DECOMPOSE_H__

#include <optional>

#include "../foundations/fwd.h"
#include "../foundations/ctx.h"
#include "../foundations/dag.h"
#include "../foundations/terms.h"
#include "../foundations/subst.h"
#include "../normalisers/joins.h"
#include "../normalisers/nnf.h"
#include "../normalisers/simplify.h"
#include "../witness/witness.h"
#include "expand.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §6 `TRY_DECOMPOSE(ψ, X, ctx)`: one LICENSED Boole decomposition on a
 * census atom whose two signs sit at member tops of different disjunctive
 * conjuncts; a candidate the probe proves `F` is folded instead. With no
 * candidate, or none licensed, `try_guard_split` takes over. `nullopt` when
 * that finds no unit either.
 *
 * THE CENSUS reads MEMBER TOPS only: a literal member of an ∨-conjunct of
 * `ψ`, a negative tree's included, or a literal conjunct of an ∧-member of
 * one, one level deep, counts one occurrence of its atom in its sign. A unit
 * and a reference are no member tops. A candidate occurs in both signs; the
 * candidates are ranked by total coverage, then by the weaker side, then
 * content order, and the first three are tried.
 *
 * PROBE FIRST: a candidate `FOLD_DECIDED` proves `F` is folded — `ψ[a ↦ F]`
 * pushed, no guard, no arm — and the smaller body re-enters the ladder
 * (invariant 8).
 *
 * THE LICENCE, on the BUILT cofactors of the conjuncts carrying the atom:
 * their sizes over both cofactors exceed those conjuncts' by a quarter at
 * most. A licensed atom partitions that region between the arms; one that
 * fails would copy shared bulk into both. The first licensed candidate wins,
 * and its cofactors are the arms' material.
 */
template <NodeType node>
std::optional<tref> try_decompose(tref psi, const block& X, ctx<node>& c);

/**
 * @brief §6 `TRY_GUARD_SPLIT(ψ, X, ctx)` on the conjuncts @p cs of `ψ`: the
 * same separation on a COMPOUND guard `g`,
 * `∃X ψ = ∃X (g ∧ ψ[g ↦ T]) ∨ ∃X (neg(g) ∧ ψ[g ↦ F])`, reached when the atom
 * census has no candidate or licenses none. The census reads literals one ∧
 * deep, so a guard whose atoms meet both signs only deeper — under an ∨
 * inside an ∧-member — is split here, as a whole.
 *
 * JUNCTIONS are the disjunctive conjuncts and their ∧-members. UNITS are
 * X-free compound guards: a non-literal member of a junction, the X-free
 * members of a disjunctive conjunct as one ∨, the X-free conjuncts of an
 * ∧-member as one ∧. A unit occurs in a junction as a member or, spelled
 * with the junction's connective, as a subset of its members. A candidate
 * occurs in one junction and its `neg` in another; the best covers the most
 * junctions, ties going to the larger weaker side, then to content order.
 * `ψ[g ↦ v]` turns a junction holding `g` or `neg(g)` into a
 * constant or drops the group from it.
 *
 * THE LICENCE is `TRY_DECOMPOSE`'s on the disjunctive conjuncts, each one the
 * split leaves unchanged counted once per arm, since both arms copy it. A
 * split that changes every disjunctive conjunct is not tested. `nullopt` when
 * no unit qualifies or the licence fails.
 */
template <NodeType node>
std::optional<tref> try_guard_split(const trefs& cs, const block& X,
	ctx<node>& c);

/**
 * @brief §6 `DECOMPOSE_ARMS(a, C_T, C_F, X, ctx)`:
 * `∃X (a ∧ C_T) ∨ ∃X (¬a ∧ C_F)`, smaller arm first; a pin atom's T-arm
 * substitutes its binder away.
 *
 * THE PIN ARM: when `a` is an equation whose term pins a block variable by
 * `COF` — the witness step's own test (§3) — the innermost such `x` equals
 * the witness `f₁′` under `a`. The T-arm is then `(a ∧ C_T)[x ← f₁′]` under
 * `X ∖ {x}`: the guard becomes its residual `p = 0`, `T` for a strict pin.
 *
 * An arm that simplifies to a constant is no case and is inserted as it
 * stands; an arm deciding `T` leaves the other unpushed. Each arm pushed
 * reads the clock first (§3 time limit).
 *
 * @param na the F-arm's guard, `¬a` when absent: a compound guard passes its
 *        `neg`, and is no equation, so it has no pin arm
 */
template <NodeType node>
tref decompose_arms(tref a, tref C_T, tref C_F, const block& X, ctx<node>& c,
	tref na = nullptr);

} // namespace idni::tau_lang::anti_prenexing

#include "decompose.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__DECOMPOSE_H__
