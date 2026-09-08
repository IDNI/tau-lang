// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file subst.h
 * @brief Anti-prenexing foundations (layer 0), package C: formula-level
 * substitution — §3's `φ[x ← t]` and `φ[atm ↦ T/F]` with the occurrence
 * guards of §10, and §1's `atoms_memo`.
 *
 * §10: "Substitution needs occurrence guards. `[x ← t]` must return an
 * untouched subtree by one cached-FV test; `[atm ↦ T/F]` needs the analogous
 * guard — `atoms_memo`, a per-node atom vocabulary with exactly the
 * substitution's own reach (unit-opaque, §4). One memo per rewrite, shared
 * across every conjunct of the site: the copies overlap."
 *
 * §4, what may touch a unit: substitution of a free variable is the ONE
 * rewrite that reaches inside a unit's body (`[x ← t]` descends); everything
 * else treats a unit as an opaque leaf (`[atm ↦ T/F]` does not descend).
 * Binder-id canonicalisation (§3 phase 0) makes the descent capture-safe:
 * ids are depth-derived, so an outer binder's id is strictly greater than any
 * binder inside it, and `FV(t)` (free variables or block variables of an
 * enclosing run) meets no binder on the path.
 *
 * Neither substitution runs `SIMPLIFY`: the caller does (§3 `TRY_WITNESS`,
 * §6 arm edges), invariant 6. Connectives are rebuilt through the raw
 * canonical constructors (dag.h) so the chain shape (D1) is kept; `T`/`F`
 * fold through the construction hooks (D4).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_H__

#include "fwd.h"
#include "dag.h"
#include "terms.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 `φ[x ← t]`, capture-aware. A node with `x ∉ FV(node)` is returned
 * UNTOUCHED (the same `tref`) by one cached test. Descends into units (§4).
 * Atoms are rewritten through `subst_term` (BDD compose plus leaf rewrite for
 * BDD-backed terms, ordinary replace otherwise), which re-simplifies what it
 * touches (§1). One memo per call, shared by every conjunct of the site
 * (§10). Debug builds assert that `FV(t)` meets no binder on the path.
 *
 * @param order            the live BDD order (`ctx.order`), for `subst_term`
 * @param simplify_formula re-simplifier for FORMULA arguments of touched
 *                         references (identity at layer 0, `simplify` from
 *                         layer 1)
 */
template <NodeType node>
tref subst_var(tref phi, tref x, tref t, const var_order<node>& order,
	const simplify_formula_fn& simplify_formula = {});

/**
 * @brief §3 `φ[atm ↦ T/F]`: erases every REACHABLE occurrence of the atom
 * `atm` (as it occurs in the formula — never re-spelled), units OPAQUE (§4).
 * A node whose atom vocabulary (`atoms`) does not contain `atm` is returned
 * untouched. `value = true` puts `T`, `false` puts `F`; a negated occurrence
 * `¬atm` receives the complement. The constants fold through the hooks;
 * the deep folding is the caller's `SIMPLIFY` (§6, the decomposition arms).
 */
template <NodeType node>
tref subst_atom(tref phi, tref atm, bool value);

/**
 * @brief §1 `atoms_memo`: `formula node → the atoms occurring in it`, units
 * opaque (§4), purely structural — exactly `subst_atom`'s reach. Sorted by
 * `subtree_less<node>` (binary-searchable, like `fv`). GLOBAL table under
 * `TAU_CACHE` (ctx.h); recomputed otherwise.
 */
template <NodeType node>
const trefs& atoms(tref n);

/// `atm ∈ atoms(n)` — the occurrence guard of `subst_atom`.
template <NodeType node>
bool has_atom(tref n, tref atm);

} // namespace idni::tau_lang::anti_prenexing

#include "subst.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_H__
