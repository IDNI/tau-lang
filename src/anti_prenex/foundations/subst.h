// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file subst.h
 * @brief Anti-prenexing foundations: the formula-level atom substitution
 * `φ[atm ↦ T/F]` of §3 with the occurrence guard of §10, and §1's
 * `atoms_memo` behind it.
 *
 * `φ[x ← t]`, the variable substitution, is not here: it is the library's
 * `tree<node>::substitute` (tau_tree.h), called directly with the live order
 * and a re-simplifying argument hook.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_H__

#include "fwd.h"
#include "dag.h"
#include "terms.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 `φ[atm ↦ T/F]`: erase every REACHABLE occurrence of the atom
 * `atm`, compared by content as it occurs in the formula — never re-spelled
 * — with units, references and temporal operators OPAQUE (§4): the
 * substitution reaches through ∧, ∨ and ¬ only.
 *
 * `value = true` puts `T`, `false` puts `F`; a negated occurrence `¬atm`
 * receives the complement, because the `¬` folds through the construction
 * hooks. `T`/`F` fold along a chain through the hooks too, so members only
 * VANISH: a canonical chain stays canonical, and any other input keeps its
 * nesting minus the erased occurrences. No hook exists for a binder, so
 * `∃x.T` stands until `fold_degenerate_binders` or the caller's `simplify`.
 * `φ` itself comes back when `atm` is not reachable in it.
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
