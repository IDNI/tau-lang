// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file terms.h
 * @brief Anti-prenexing foundations (layer 0), package B: term BDDs — the §1
 * term representation per component (decision variables = the component
 * `P`, everything else in the leaves; cofactor = child selection; `∀_P`/`∃_P`
 * = meet/join of the leaves; substitution = compose plus leaf rewrite;
 * functional quantifiers transparent; the LEAF HAZARD) and the term-level
 * primitives of §3 (`SIMPLIFY_TERM`, `SIMPLIFY_ATOM`, `TERM_OF`,
 * `NORM_EQUATION`, `PREPARE_TERMS`) plus `‖·‖` (§1, §10).
 *
 * Built on the existing `tau_term_bdd_handle<node>` (`term_handle`): its
 * `build(term, order)` treats exactly the order's keys as decision variables
 * and everything else as a leaf, which IS this representation; `bdd_ex` /
 * `bdd_all` quantify over a set, `bdd_compose` substitutes, `to_tau_term`
 * converts back to a plain `bf`, `convert_to_tau_node` mints the `BDD_ID`
 * node a BDD-backed term is stored as. The library's operation memos are
 * keyed by BDD refs and valid under ONE order (`sync_order_cache`).
 *
 * THE LIVE ORDER IS THE CALLER'S: every BDD primitive here takes the order
 * as an explicit parameter (the component's `ctx.order`, §5); nothing in
 * this file stores one. Only one order may be live at a time, so a
 * component's `finish_terms` (D2) runs before the next `prepare_terms`.
 * The two simplifiers default to the empty order — phases 1, 2 and 5, where
 * nothing is BDD-backed (§3). Debug builds check, at every BDD primitive's
 * entry, that the ref passed in is a reduced ordered BDD under `order`
 * (every decision variable a key, ranks strictly increasing down every
 * path): a mixed-order operand or a stale ref breaks canonicity silently
 * otherwise.
 *
 * A term is BDD-backed exactly when it has a decision variable, i.e. when
 * it touches `P`: a BDD that became a single leaf or a terminal (a cofactor,
 * a quantification over all of `P`) comes back as its plain term, so a
 * `P`-free term never carries a `BDD_ID`. `convert_to_tau_node` interns
 * (tau_bdd.tmpl.h), so the same BDD under the same type is the same node in
 * every component: the D2 round trip and hash-consed identity hold.
 *
 * `term_of` and `norm_equation` live here because both are term operations
 * (`l + r` is a ring sum). Inside this namespace `norm_equation` hides
 * `tau_lang::norm_equation` (normal_forms_transformations), which does NOT
 * descend through a formula negation; call the old one fully qualified, and
 * a test pins that the two disagree on `¬(l = r)` so neither is "fixed"
 * into the other.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_H__

#include "fwd.h"

namespace idni::tau_lang::anti_prenexing {

// --- the representation boundary (PREPARE_TERMS, D2) -----------------------------

/**
 * @brief §3 primitive `PREPARE_TERMS(body, P, order)` — §1's term
 * representation over `P`: both sides of every EQUATION touching `P` (one
 * cached FV test each) rebuilt as BDD-backed terms whose decision variables
 * are `P` in the given order, everything else in the leaves; an atom not
 * touching `P` untouched; an ORDER ATOM left plain whatever it touches (it
 * is never cofactored — `cof_memo` keys are `TERM_OF` of equations — and the
 * solver path consumes it as written); a FORMULA-level binder, a reference
 * and a temporal operator transported opaque (§7 translates a unit at query
 * time; the push never enters the other two); a TERM-level functional
 * quantifier `∀_Y`/`∃_Y` SLID onto its body's leaves — `∀_Y(x·b₁ ∪ x′·b₀) =
 * x·∀_Y b₁ ∪ x′·∀_Y b₀`, dually for `∃_Y`, innermost first, linear in the
 * body's BDD. Memoised per atom and per term within the call. The caller
 * (§5) owns `order` and passes it to every later term operation.
 *
 * @param body  the component's body, a `wff`
 * @param P     the component, `X`'s order
 * @param order `ctx.order`: `P[i] ↦ |P| - i` (inner → LOWER, §5)
 */
template <NodeType node>
tref prepare_terms(tref body, const block& P, const var_order<node>& order);

/**
 * @brief D2: every `BDD_ID` term in `phi` back to a plain `bf`, wherever it
 * sits — under a binder REWRAP re-attached, inside a symbolic functional
 * quantifier's body, inside a reference argument, under a temporal
 * operator — one memoised walk with no node kind special-cased; the inverse
 * of `prepare_terms` up to term normal form. Round trip requirement:
 * `prepare_terms(finish_terms(prepared))` yields the same BDD, hence (by
 * interning) the same `BDD_ID` node.
 *
 * A SOLVER QUERY is built on finished terms: a `BDD_ID` is a node of one
 * order, and a query keyed on it in `solver_memo` would never hit across
 * components. Layer 3's query builders call this on the query formula.
 */
template <NodeType node>
tref finish_terms(tref phi);

/// The term is a `BDD_ID` node: BDD-backed under the live order, with at
/// least one decision variable.
template <NodeType node>
bool is_bdd_backed(tref term);

// --- cofactors and quantification -----------------------------------------------

/**
 * @brief §1 cofactor by CHILD SELECTION: `f[x ← bit]` for a BDD-backed `f`
 * and a DECISION variable `x` at any depth — a memoised restrict traversal
 * (the node of `x` is its child by `bit`, everything else rebuilt as it is;
 * `bdd_compose(x ↦ T/F)` is the tests' oracle). Result canonical over the
 * decision variables; NOT run through `simplify_term` — `COF` (layer 3)
 * does that. `f` itself when `x` is not one of its decision variables, or
 * `f` is plain. Nothing else is ever cofactored on.
 */
template <NodeType node>
tref cofactor(tref f, tref x, bool bit, const var_order<node>& order);

/// `forall_over` or `exists_over` by `kind` — for the callers that hold a
/// `binder` (SETTLE_FUNCTIONAL, DISCHARGE). A block is kind-homogeneous, so
/// the library's mixed-prefix `bdd_quant` is never needed.
template <NodeType node>
tref quantify_over(binder kind, tref f, const block& X,
	const var_order<node>& order);

/// §1 `∀_X f = f₀·f₁` over a block: ONE BDD quantification, never expanded
/// to 2^|X| terms; when `X` covers all decision variables this is the meet of
/// the leaves in one traversal. `order` is the live order.
template <NodeType node>
tref forall_over(tref f, const block& X, const var_order<node>& order);

/// §1 `∃_X f = f₀ ∪ f₁` over a block; dual of `forall_over` (join of the leaves).
template <NodeType node>
tref exists_over(tref f, const block& X, const var_order<node>& order);

/**
 * @brief The SYMBOLIC functional-quantifier term `∀_Y f` / `∃_Y f`: a nested
 * chain of single-variable `bf_fall` / `bf_fex` nodes (the builders and the
 * parser allow no other shape), the subscripts in CONTENT ORDER
 * (`subtree_less`, the order `get_free_vars` returns), outermost first, so
 * permutations of `Y` give one node; a variable of `Y` not free in `f` is
 * dropped (the term-level FOLD_DEGENERATE_BINDERS), a constant `f` is
 * returned as it is. `Y` is excluded from its `FV`. The key of §1
 * `quant_memo` ("the key IS the query") and DISCHARGE's `keep_functional`
 * emission (layer 3).
 */
template <NodeType node>
tref functional_quantifier(binder kind, const block& Y, tref f);

/// The term carries a functional quantifier (`SETTLE_FUNCTIONAL`'s test, §6).
template <NodeType node>
bool carries_functional_quantifier(tref f);

// --- substitution inside a term -----------------------------------------------

/**
 * @brief §1 term-level `f[x ← t]`: for a BDD-backed `f`, a BDD compose on the
 * decision variable `x` PLUS rewriting `x` inside every leaf (`r(x) ↦ r(t)`)
 * — sound for any term, reaches every occurrence, needs no leaf guard. Every
 * reference argument it touches is re-emitted through `simplify_term` (a
 * formula argument through `simplify_formula`, §1), so arguments stay
 * simplified (inv. 6). For a plain (non-BDD) term: ordinary replace with the
 * same re-simplification. Returns `f` itself when `x ∉ FV(f)`. A BDD-backed
 * `t` enters a leaf as its plain term, so no `BDD_ID` ever nests inside a
 * leaf (the compose on the decision variable uses `t`'s BDD directly).
 */
template <NodeType node>
tref subst_term(tref f, tref x, tref t, const var_order<node>& order,
	const simplify_formula_fn& simplify_formula = identity_formula);

// --- the two aggressive normalisers of invariant 6 --------------------------------

/**
 * @brief §3 `SIMPLIFY_TERM(t)`: constant folding, absorption and complement
 * laws, per-path contradiction (`prop:xfx` — `x·f(x) = x·f(1)`,
 * `x′·f(x) = x′·f(0)`, so a sub-term under a literal is reduced by that
 * literal's assignment), and reduction to the canonical form of the BDD
 * backing `t`. All four laws come from the existing
 * `syntactic_path_simplification` (heuristics/): a plain term goes through
 * it whole; for a term under the live order the representation is first
 * re-established — a BDD-backed term is canonical over `P` already, a plain
 * combination of BDD-backed subterms (`f₀·f₁`, `f₀ ∪ f₁`, `f₁′`, the way
 * `COF` forms its products and complements) is built over `P` absorbing
 * them — then every leaf goes through the path simplifier and the BDD is
 * rebuilt so leaves that became equal merge. Canonical up to LEAF EQUALITY
 * only (§3): the single source of incompleteness behind every syntactic
 * test of the spec. A term that does not touch `P` is in the plain regime
 * whatever the order.
 */
template <NodeType node>
tref simplify_term(tref t, const var_order<node>& order = {});

/**
 * @brief §3 `SIMPLIFY_ATOM(a)`: `simplify_term` on both sides, then a
 * constant-only atom folds to `T`/`F` through the construction hooks (an
 * equation by the hooks' constant rules, a bitvector order atom by the
 * bitvector hook). `a` is a `wff` atom, optionally under one `¬` (folded
 * through). Two regimes: with no BDD-backed side (the initial phase-1
 * simplification, phases 2 and 5, every plain atom of the push — all order
 * atoms among them) the full existing `syntactic_atomic_formula_simplification`
 * runs, the `norm_equation`/`denorm_equation` round trip on the joint
 * `l + r` and the per-variable `0`/`1` pass included, which folds and
 * shrinks before any BDD exists; with a BDD-backed side, side-wise and
 * shallow, the atom never reshaped (the joint fold is `TERM_OF`'s,
 * FOLD_DECIDED). The plain regime is IDEMPOTENT (a test pins it), so an
 * atom's shape as written is its simplified shape and no memo key drifts
 * (spec: equations stay as written outside SQUEEZE).
 */
template <NodeType node>
tref simplify_atom(tref a, const var_order<node>& order = {});

// --- reading and rewriting equations ---------------------------------------------

/// §3 `TERM_OF(atom)`: for `l = r` (through one `¬`), the term `l + r` —
/// read off without touching the atom (equations stay as written, inv. 4).
/// `order` is the live order (the ring sum of two BDD-backed sides is a BDD
/// operation; a plain side is a leaf under it). `nullptr` on anything but
/// an equation — a caller bug, Debug-asserted; every caller guards.
template <NodeType node>
tref term_of(tref atom, const var_order<node>& order);

/// §3 `NORM_EQUATION`: `l = r ↦ l + r = 0`, descending through one `¬`.
/// Called in exactly one place, `SQUEEZE` step 1 (§7).
template <NodeType node>
tref norm_equation(tref atom, const var_order<node>& order);

// --- in-memory size and the leaf hazard -------------------------------------------

/**
 * @brief §1 `‖t‖`, the count IN MEMORY (§10) of a TERM: the unique node
 * count of its BDD, leaves included and the terminals as zero, when
 * BDD-backed; when plain (phase 2's spelled witnesses are plain), its
 * distinct term-structure nodes — each operator, functional quantifier and
 * leaf (a variable, constant or reference) once, the `bf` wrappers not
 * counted and a leaf's own nodes not entered. The one consumer is
 * `TRY_WITNESS`'s tie-break among
 * a variable's pins, the smallest `‖f₁′‖`, which is the cost every later
 * compose or replace of the witness pays. No formula overload: nothing in
 * the spec reads `‖φ‖` (the decomposition licence uses `|·|`,
 * `formula_size`). The visited-set scheme of
 * `heuristics/bv_predicate_blasting.tmpl.h`'s `bdd_node_count`, with the
 * leaf guard its own BDDs do not need.
 */
template <NodeType node>
size_t mem_size(tref t);

/**
 * @brief §1 LEAF HAZARD: the free variables contributed by the LEAVES of a
 * BDD-backed term alone (a block variable here is hidden from cofactoring:
 * inside a reference argument or a foreign-typed subterm, including one
 * inside a functional quantifier's body); for a plain term, its `FV`.
 * Sorted like `get_free_vars`. It differs from it exactly in the hazard case, a
 * block variable that is both a decision variable and hidden in a leaf,
 * which is what ELIMINATE_BLOCK's `opaque?` test (§7) asks before any
 * quantification. Returned BY VALUE, not stored: the per-leaf sets are
 * `get_free_vars`' own cached entries and their union is one walk over the
 * BDD's distinct nodes per call.
 */
template <NodeType node>
trefs leaf_fv(tref f);

} // namespace idni::tau_lang::anti_prenexing

#include "terms.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_H__
