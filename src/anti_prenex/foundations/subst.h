// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file subst.h
 * @brief Anti-prenexing foundations: the two substitutions of §3 — the
 * formula-level `φ[atm ↦ T/F]` with the occurrence guard of §10 and §1's
 * `atoms_memo` behind it, and the variable substitution `φ[x ← t]`.
 *
 * `φ[x ← t]` IS the library's `tree<node>::substitute` (tau_tree.h) under the
 * live order, wrapped here with the re-simplifying ARGUMENT HOOK invariant 6
 * asks for: that hook is what makes the library call the spec's primitive.
 * `SIMPLIFY`'s propagation substitutes with the same hook and takes it from
 * here.
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

/**
 * @brief §3 `φ[x ← t]`: the library's substitution under the live @p order —
 * empty in the plain regime — with the argument hook of invariant 6, which
 * re-simplifies once every reference argument the rewrite changed.
 *
 * @p x is the VARIABLE node; the key the library matches is its `bf` term,
 * the form every occurrence takes inside a term, and it is formed here so
 * that no caller spells it. An occurrence under a binder over @p x is bound,
 * not free, and the library leaves it where it is.
 *
 * @param phi a `wff` formula or a `bf` term
 */
template <NodeType node>
tref subst_var(tref phi, tref x, tref t, const var_order<node>& order = {});

} // namespace idni::tau_lang::anti_prenexing

#include "subst.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_H__
