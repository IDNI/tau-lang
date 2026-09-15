// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file subst.h
 * @brief Anti-prenexing foundations (layer 0), package C: formula-level
 * substitution — §3's `φ[x ← t]` and `φ[atm ↦ T/F]` with the occurrence
 * guards of §10 — and §1's `atoms_memo`.
 *
 * §10: "Substitution needs occurrence guards. `[x ← t]` must return an
 * untouched subtree by one cached-FV test; `[atm ↦ T/F]` needs the analogous
 * guard — `atoms_memo`, a per-node atom vocabulary with exactly the
 * substitution's own reach (unit-opaque, §4). One memo per rewrite, shared
 * across every conjunct of the site: the copies overlap."
 *
 * REACH (§4). `[x ← t]` is the ONE rewrite that reaches inside a unit's body
 * and inside a reference's arguments (§1). It descends into binders, which is
 * capture-safe because the depth-derived ids of phase 0 put an enclosing
 * binder strictly above every id inside it, so `FV(t)` — free variables, or
 * block variables bound above the site — meets no binder on the path. It also
 * descends into a `wff_ref`'s arguments: a term argument through `subst_term`
 * and then `SIMPLIFY_TERM`, a formula argument through this same substitution
 * and then `simplify_formula`, once per touched argument (invariant 6).
 *
 * `[atm ↦ T/F]` reaches through ∧, ∨ and ¬ only: a unit and a reference are
 * opaque leaves. A TEMPORAL operator (`always` / `sometimes`) is opaque to
 * BOTH substitutions and its body is not in the `atoms` vocabulary; the
 * caveat is that an `x` inside a temporal body is not rewritten, a site the
 * spec's callers never form. Any other `wff` shape (the pre-NNF connectives)
 * is rebuilt generically, with its formula children substituted and its term
 * children put through `subst_term`.
 *
 * REBUILD. Neither substitution runs `SIMPLIFY`; the caller does (§3
 * `TRY_WITNESS`, §6 arm edges), which is invariant 6. `[x ← t]` rebuilds a
 * chain top from its FULL member view through the raw canonical constructors
 * (dag.h), so the result is a canonical chain whatever the input's nesting.
 * `[atm ↦ T/F]` erases members in place, so a canonical chain stays canonical
 * and any other input keeps its shape. Everything else goes through the
 * hooked constructors: `T`/`F` fold along a chain, `¬T` / `¬F` / `¬¬ψ` fold,
 * a constant-only atom folds, and the TERM hooks fold what they fold inside a
 * rewritten side (`y′·y` is `0` before its atom is rebuilt).
 *
 * No hook exists for a binder, so `∃x.T` stands until
 * `FOLD_DEGENERATE_BINDERS` or the caller's `SIMPLIFY`. A chain none of whose
 * members changed is returned as it stands. An atom is rebuilt exactly as
 * `simplify_atom` rebuilds one, so on a non-bitvector type a rebuilt order
 * atom is the hooks' equation — the one place the module constructs an order
 * atom.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_H__

#include "fwd.h"
#include "dag.h"
#include "terms.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 `φ[x ← t]`, capture-aware.
 *
 * A node with `x ∉ FV(node)` is returned UNTOUCHED — the same `tref` — after
 * one cached test, and `φ` itself comes back when `x` is not free in it. The
 * walk descends into units and reference arguments (§4, §1); temporal
 * operators stay opaque. Atoms are rewritten through `subst_term` (a BDD
 * compose plus a leaf rewrite for BDD-backed terms, an ordinary replace
 * otherwise), which re-simplifies the reference arguments it touches (§1).
 *
 * One memo per call, shared by every conjunct of the site (§10): a caller
 * substitutes into a whole site with ONE call. The substitution is
 * SIMULTANEOUS — only the original occurrences of `x` are replaced, and no
 * result is re-entered — so `x ∈ FV(t)` is allowed. Debug builds assert that
 * `FV(t)` meets no binder on the path.
 *
 * A WITNESS IS PLAIN (§3). This is the BOUNDARY where the spec's "a caller
 * spells one once per substitution" happens: a `t` carrying a `BDD_ID` is
 * spelled out here, ONCE, before the walk, rather than once per atom side
 * inside `subst_term`, which takes the plain contract and Debug-asserts it.
 *
 * RENAME APART (§3): when `t` carries a functional quantifier, its BOUND
 * variables are moved above every id in sight once, before the walk
 * (`terms_detail::rename_apart`), so that no binder of `φ` can equal a
 * subscript inside `t` on any path. No SHADOWING PAIR can form, and no
 * substitution below has to rename; phase 5's `CANONICALISE_BINDER_IDS`
 * restores the depth ids.
 *
 * @param order            the live BDD order (`ctx.order`), for `subst_term`;
 *                         empty at phases 1, 2 and 5
 * @param simplify_formula re-simplifier for FORMULA arguments of touched
 *                         references (`identity_formula` by default,
 *                         `simplify` from layer 1)
 */
template <NodeType node>
tref subst_var(tref phi, tref x, tref t, const var_order<node>& order,
	const simplify_formula_fn& simplify_formula = identity_formula);

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
