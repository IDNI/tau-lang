// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file subst.h
 * @brief Anti-prenexing foundations (layer 0), package C: the formula-level
 * atom substitution `φ[atm ↦ T/F]` of §3 with the occurrence guard of §10,
 * and §1's `atoms_memo` behind it.
 *
 * `φ[x ← t]` is the library's `term_handle<node>::substitute` (tau_bdd.h),
 * called directly with the live order and a re-simplifying argument hook.
 *
 * §10: "Substitution needs occurrence guards. `[atm ↦ T/F]` needs one —
 * `atoms_memo`, a per-node atom vocabulary with exactly the substitution's own
 * reach (unit-opaque, §4)."
 *
 * REACH (§4). `[atm ↦ T/F]` reaches through ∧, ∨ and ¬ only: a unit, a
 * reference and a TEMPORAL operator are opaque leaves, and a temporal body is
 * not in the `atoms` vocabulary.
 *
 * REBUILD. The substitution does not run `SIMPLIFY`; the caller does (§6, the
 * decomposition arms), which is invariant 6. Its replacement is a constant, so
 * members only VANISH: a canonical chain stays canonical — sorted,
 * deduplicated, left-nested — with nothing re-canonicalised, and any other
 * input keeps its nesting minus the erased occurrences. Everything else goes
 * through the hooked constructors: `T`/`F` fold along a chain and `¬T` / `¬F`
 * / `¬¬ψ` fold.
 *
 * No hook exists for a binder, so `∃x.T` stands until
 * `FOLD_DEGENERATE_BINDERS` or the caller's `SIMPLIFY`. A chain none of whose
 * members changed is returned as it stands.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_H__

#include "fwd.h"
#include "dag.h"
#include "terms.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 `φ[atm ↦ T/F]`: erase every REACHABLE occurrence of the atom
 * `atm`, taken as it occurs in the formula — compared by content, never
 * re-spelled — with units, references and temporal operators OPAQUE (§4).
 *
 * A node whose atom vocabulary (`atoms`) does not contain `atm` is returned
 * untouched, and `φ` itself comes back when `atm` is not reachable in it.
 * `value = true` puts `T`, `false` puts `F`; a negated occurrence `¬atm`
 * receives the complement, because the `¬` folds through the construction
 * hooks. The constants fold through the hooks too; the deep folding is the
 * caller's `SIMPLIFY` (§6, the decomposition arms).
 *
 * The implementation is the library's `rewriter::replace_if` with `atoms` as
 * its descent predicate. Members only vanish, so a canonical chain stays
 * canonical — sorted, deduplicated, left-nested — with nothing
 * re-canonicalised, and a non-canonical input keeps its nesting minus the
 * erased occurrences.
 */
template <NodeType node>
tref subst_atom(tref phi, tref atm, bool value);

/**
 * @brief §1 `atoms_memo`: formula node → the atoms occurring in it, units
 * opaque (§4). Purely structural, and exactly `subst_atom`'s reach (∧, ∨, ¬).
 * The result is trimmed, deduplicated and sorted by `subtree_less<node>`,
 * hence binary-searchable like `get_free_vars`' result.
 *
 * The rows live in the UNCONDITIONAL structural table `atoms_memo` of ctx.h
 * (value type `tref_set`, fwd.h), so the returned reference is valid in every
 * build type; a GC sweep invalidates it.
 *
 * Lazy: the first query fills a row for every wrapper in the reach below `n`
 * in one post-order walk. The exception is the same-connective continuation
 * nodes of a chain — its "spine" — which get no row, the rule `get_free_vars`
 * follows as well: a chain of k members would otherwise store k sets of size
 * O(k). A spine node asked about directly is a root and does get its row.
 */
template <NodeType node>
const trefs& atoms(tref n);

/// `atm ∈ atoms(n)` — the occurrence guard of `subst_atom`.
template <NodeType node>
bool has_atom(tref n, tref atm);

} // namespace idni::tau_lang::anti_prenexing

#include "subst.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_H__
