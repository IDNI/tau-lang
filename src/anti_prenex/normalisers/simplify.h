// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file simplify.h
 * @brief §3 `SIMPLIFY` — equality propagation, then the path sweep — and the
 * plain-regime pin match it shares with `TRY_WITNESS`.
 *
 * TWO PASSES over an NNF formula, in this order (§3): EQUALITY PROPAGATION,
 * then the PATH SWEEP. Neither re-spells an atom (invariant 4), and neither
 * re-assembles a chain: absorption and canonical assembly belong to the
 * result joins at the construction sites, so a conjunction keeps its nesting
 * with its members rewritten in place (§10).
 *
 * WHAT PROPAGATION IS FOR: a conjunct pinning a FREE variable licenses
 * substituting the pin's witness for that variable in its sibling conjuncts.
 * It is the only step that carries an assumption from one conjunct into
 * another's TERMS, and it is what lets the syntactic tests fire on shapes
 * like `f = xy ∪ x′a`, where `f₀ = a` and `f₁ = y` compare unequal until
 * `y := a` turns `f` into `a`. A heuristic, not a monotone gain, hence the
 * cap (`propagate_growth`, options.h).
 *
 * WHAT IT MAY TOUCH (§4): the substitution descends through a sibling's whole
 * ∧/∨ structure and INTO a binder unit's body, with a pin SUSPENDED under a
 * binder over its variable or over a variable of its witness. Temporal
 * operators are opaque to both passes. A reference never matches; a pin's
 * substitution reaches its arguments like any other occurrence and
 * re-simplifies the argument it changed through `SIMPLIFY_TERM` (invariant
 * 6); under `ref_args` every argument goes through `SIMPLIFY_TERM`, changed
 * or not. The pinning conjunct STAYS, rewritten by every pin but its OWN: it
 * still constrains its variable, and for a weak pin it is what keeps the
 * residual `p = 0`, while its own pin would fold it to `T`.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__NORMALISERS__SIMPLIFY_H__
#define __IDNI__TAU__ANTI_PRENEX__NORMALISERS__SIMPLIFY_H__

#include <optional>

#include "../foundations/fwd.h"
#include "../foundations/dag.h"
#include "../foundations/ctx.h"
#include "../foundations/terms.h"
#include "../foundations/options.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 `TRY_WITNESS`'s match on a plain term: the variable an equation
 * pins, the witness to substitute for it, and whether the pin is STRICT.
 *
 * `var` is a free variable of the equation's term, `witness` is `f₁′` — the
 * lower end of the interval the one-point rule leaves — and `strict` says
 * that the residual `p = f₀f₁` folds to `0`, i.e. that the pin is the spelled
 * shape `y + t = 0`. A WEAK pin keeps its residual in the pinning conjunct,
 * which is why that conjunct is never dropped here.
 */
template <NodeType node>
struct pin {
	tref var = nullptr;
	tref witness = nullptr;
	bool strict = false;
};

/**
 * @brief §3 `TRY_WITNESS`'s PIN TEST as ONE recipe over a pair of cofactors:
 * what `f₀` and `f₁` say about the variable `y` they were taken on, recorded
 * in a `cof_entry` (fwd.h).
 *
 * `usable` holds iff `y ∉ FV(f₀) ∪ FV(f₁)`; where it does not, `y` still
 * hides inside a subterm no cofactor reached (§1, the LEAF HAZARD) and the
 * record must not be used. `p`, the residual `SIMPLIFY_TERM(f₀·f₁)`, is built
 * when `usable` and is `nullptr` otherwise. `pin` holds iff `usable` and
 * `SIMPLIFY_TERM(f₀ ∪ f₁) = 1`, the test that collapses Boole's interval
 * `f₀ ≤ y ≤ f₁′` to a point. A pin gives the witness `f₁′` — which the
 * consumer builds itself, there being no slot for it — and the residual
 * equation `p = 0`, and is STRICT when `p` folds to `0`.
 *
 * The one pin recipe: `find_pin` / `find_pin_for` take the cofactors by
 * SUBSTITUTION on a plain term, `cof` (cofactors.h) by CHILD SELECTION in
 * the term's BDD, and both record the result in `cof_memo` (§1). `order` is
 * the live order, handed to every `SIMPLIFY_TERM` here; the plain regime
 * passes the empty one.
 */
template <NodeType node>
cof_entry pin_from_cofactors(tref f0, tref f1, tref y,
	const var_order<node>& order);

/**
 * @brief §3 `TRY_WITNESS`'s pin match, on a plain term: the best pin of
 * `atom`, or `nullopt`.
 *
 * `atom` must be a POSITIVE equation — an equation under `¬` pins nothing —
 * and must be free of every variable of `X`, which by §1 makes both sides
 * plain: pushing a block variable into a leaf would break the
 * child-is-cofactor identity, so an `X`-free conjunct is the guard. On
 * `f = TERM_OF(atom)` a free `y` is pinned iff `usable ∧ f₀ ∪ f₁ = 1` on the
 * cofactors `f₀ = SIMPLIFY_TERM(f[y←0])` and `f₁ = SIMPLIFY_TERM(f[y←1])`,
 * where `usable` is `y ∉ FV(f₀) ∪ FV(f₁)`. Among several pins of one atom: a
 * STRICT one first, else the smallest `‖f₁′‖` (`mem_size`).
 *
 * `order` is the live order; the plain regime (phases 1, 2 and 5) passes the
 * empty one, where nothing is BDD-backed and the `X` guard is vacuous.
 */
template <NodeType node>
std::optional<pin<node>> find_pin(tref atom, const block& X,
	const var_order<node>& order = {});

/**
 * @brief §3 `TRY_WITNESS`'s pin match asked about ONE variable: the pin `x`
 * has in `atom`, or `nullopt`.
 *
 * The same match as `find_pin`, asked about a GIVEN variable instead of
 * scanning the atom's free variables. There is nothing to choose: an atom
 * pins a given variable at most one way. `atom` must be a POSITIVE equation.
 * The block guard of `find_pin` is not repeated: a caller with a block hands
 * in an atom its own guard already found block-free. `nullopt` when the atom
 * is no positive equation, when `x` is not free in its term, and when `x` is
 * not pinned.
 */
template <NodeType node>
std::optional<pin<node>> find_pin_for(tref atom, tref x,
	const var_order<node>& order = {});

/**
 * @brief §3 `SIMPLIFY(φ)`: equality propagation, then the spelling-preserving
 * path sweep.
 *
 * @param phi      a `wff` node (a `bf` term is handed straight to the sweep,
 *                 which has nothing to propagate)
 * @param order    the live order (§1 `ctx.order`); its key set IS the block
 *                 `X` in scope. Empty in phases 1, 2 and 5, which makes the
 *                 propagation guard vacuous and every term plain
 * @param ref_args phase 1's entry state, which ESTABLISHES invariant 6: every
 *                 atom goes through `SIMPLIFY_ATOM` whether the environment
 *                 changed it or not, and every reference argument through
 *                 `SIMPLIFY_TERM`
 * @return the simplified formula; an untouched formula comes back as the same
 *         node
 */
template <NodeType node>
tref simplify(tref phi, const var_order<node>& order = {},
	bool ref_args = false);

} // namespace idni::tau_lang::anti_prenexing

#include "simplify.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__NORMALISERS__SIMPLIFY_H__
