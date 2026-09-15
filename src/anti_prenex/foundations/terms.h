// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file terms.h
 * @brief Anti-prenexing foundations (layer 0), package B: TERM BDDs and the
 * term-level primitives of §3.
 *
 * THE §1 TERM REPRESENTATION, one per component: the decision variables of
 * the BDD are exactly the component's block `P`, and everything else sits in
 * the leaves. Cofactoring is child selection; `∀_P` and `∃_P` are the meet
 * and the join of the leaves; substitution is a BDD compose plus a rewrite of
 * the leaves; functional quantifiers are transparent. What the BDD cannot see
 * is the LEAF HAZARD: a variable of `P` may also occur inside a leaf, where
 * no quantification reaches it.
 *
 * The primitives declared here are §3's `PREPARE_TERMS`, `RESOLVE_FUNCTIONAL`,
 * `SIMPLIFY_TERM`, `SIMPLIFY_ATOM`, `TERM_OF` and `NORM_EQUATION`, plus the
 * in-memory term size `‖·‖` (§1, §10).
 *
 * THE LIBRARY UNDERNEATH is `tau_term_bdd_handle<node>` (aliased
 * `term_handle`) and the `tau_term_bdd<node>` below it. `build(term, order)`
 * treats exactly the order's keys as decision variables and everything else
 * as a leaf, which IS this representation. `has_bdd_var` asks whether a term
 * touches the order at all; `bdd_ex` / `bdd_all` quantify over a set;
 * `bdd_compose` substitutes a decision variable; `bdd_cofactor` selects a
 * child at any depth; `bdd_xor` is the ring sum; `map_leaves` rewrites the
 * leaves; `visit_nodes` and `node_count` walk the distinct nodes;
 * `is_ordered` checks a BDD reference against an order; `get_free_leaf_vars`
 * reads the leaves' free variables. `to_tau_term` converts back to a plain
 * `bf`; `convert_to_tau_node_or_term` mints the `BDD_ID` node a BDD-backed
 * term is stored as, and hands back a plain term when the BDD does not
 * branch; `convert_to_tau_terms` is the formula-wide inverse and
 * `is_bdd_backed` the test for one. The library's operation memos are keyed
 * by BDD references and stay valid under every order that agrees with the
 * one they were computed under on the common keys (`sync_order_cache`).
 *
 * THE LIVE ORDER IS THE CALLER'S. Every BDD primitive here takes the order as
 * an explicit parameter — the component's `ctx.order` (§5) — and nothing in
 * this file stores one. Orders that are alive together AGREE on their common
 * keys: a component's `P`, the sub-order the library's chain case builds by
 * dropping a chain's subscripts, and the tiny order a chain resolution builds
 * over its own subscripts. The library's cache sync merges them instead of
 * clearing them. A component still converts its terms back
 * (`convert_to_tau_terms`) before the next `prepare_terms`, because a
 * `BDD_ID` node belongs to ONE order. The two simplifiers default to the
 * empty order, which is the plain regime of phases 1, 2 and 5, where nothing
 * is BDD-backed (§3).
 *
 * Debug builds check at every BDD primitive's entry that the reference passed
 * in is a reduced ordered BDD under `order` (`tau_term_bdd::is_ordered`:
 * every decision variable is a key of the order, and ranks increase strictly
 * down every path). A mixed-order operand or a stale reference would break
 * canonicity silently.
 *
 * WHEN A TERM IS BDD-BACKED: exactly when it has a decision variable, that
 * is, when it touches `P`. A BDD that has become a single leaf or a terminal
 * — after a cofactor, or after quantifying over all of `P` — comes back as
 * its plain term, so a `P`-free term never carries a `BDD_ID`. That is what
 * `convert_to_tau_node_or_term`, the one way a term is emitted here, does.
 *
 * ONE EXCEPTION, in keep mode (§7 `DISCHARGE`): a WHOLE-BLOCK emission
 * `Q_X (bf(BDD_ID))` is `X`-free and still keeps its BDD body, which nothing
 * rewrites mid-block. The component's close resolves it with one
 * quantification on the stored BDD, and the finish spells out what is kept.
 *
 * `BDD_ID` nodes are interned (tau_bdd.tmpl.h), so the same BDD under the
 * same type is the same Tau node in every component: the round trip out of
 * and back into this representation preserves hash-consed identity.
 *
 * `term_of` and `norm_equation` live here because both are term operations
 * (`l + r` is a ring sum). NAME CLASH: inside this namespace `norm_equation`
 * hides `tau_lang::norm_equation` (normal_forms_transformations), which does
 * NOT descend through a formula negation. Call that one fully qualified. A
 * test pins that the two disagree on `¬(l = r)`, so that neither is "fixed"
 * into the other.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_H__

#include "fwd.h"

namespace idni::tau_lang::anti_prenexing {

// --- §1 vocabulary with a library counterpart -----------------------------------
//
// Not declared here — used directly, on a `bf` term `t` or a formula `phi`:
//
//   BDD-backed term        term_handle<node>::is_bdd_backed(t)
//   the finish             term_handle<node>::convert_to_tau_terms(phi)
//   ∀_X f  /  ∃_X f        quantify_over(tbdd::all / tbdd::ex, f, X, order)
//   f[x ← bit]             tau_term_bdd<node>::bdd_cofactor (behind `cofactor`)
//   symbolic ∀_Y f / ∃_Y f tau_term_bdd<node>::build_functional_quantifiers
//   RESOLVE_FUNCTIONAL     resolve_functional_quantifiers (below)
//
// BDD-BACKED: the term is a `bf(BDD_ID)` node, i.e. it is backed by a BDD
// under the live order and has at least one decision variable. The `bf`
// WRAPPER is the form that counts — a bare `BDD_ID` node is not one — and the
// test holds in any spelling of the wrapper, with or without a right sibling.
//
// THE FINISH: every `BDD_ID` term of `phi` converted back to a plain `bf`,
// wherever it sits — under a binder that `REWRAP` re-attached, inside a
// symbolic functional quantifier's body, inside a reference argument, under a
// temporal operator. It is one memoised walk with no node kind special-cased,
// and the inverse of `prepare_terms` up to term normal form. Round trip
// requirement: `prepare_terms(convert_to_tau_terms(prepared))` yields the
// same BDD and hence, by interning, the same `BDD_ID` node. A SOLVER QUERY is
// built on finished terms, because a `BDD_ID` is a node of one order and a
// query keyed on one in `solver_memo` would never hit across components.
// Layer 3's query builders call the finish on the query formula.
//
// ∀_X / ∃_X: `quantify_over` is the one entry point. A block is
// kind-homogeneous, so the library's mixed-prefix `bdd_quant` is never needed
// here, and callers that hold a kind (`SETTLE_FUNCTIONAL`, `DISCHARGE`) pass
// it through.
//
// SYMBOLIC ∀_Y / ∃_Y: the library's chain CONSTRUCTOR is the one builder; it
// takes `quants` outermost first, and a block `Y` of one kind gives all pairs
// with that kind. It is canonical — absent subscripts dropped, each same-kind
// run in content order, an adjoining run of the body merged — and it folds a
// chain that binds every free variable of a plain body to its constant, which
// is the term-level `FOLD_DEGENERATE_BINDERS` and `DISCHARGE`'s keep-mode
// emission in one call.

// --- the representation boundary (PREPARE_TERMS) ---------------------------------

/**
 * @brief §3 primitive `PREPARE_TERMS(body, P, order)`: put `body` into §1's
 * term representation over `P`.
 *
 * Both sides of every EQUATION that touches `P` — one cached free-variable
 * test each — are rebuilt as BDD-backed terms whose decision variables are
 * `P` in the given order, everything else going into the leaves. An atom that
 * does not touch `P` is left untouched. An ORDER ATOM is left plain whatever
 * it touches: it is never cofactored (`cof_memo` keys are `TERM_OF` of
 * equations) and the solver path consumes it as written.
 *
 * A formula-level binder, a reference and a temporal operator are transported
 * opaque; §7 translates a unit at query time, and the push never enters the
 * other two. A term-level functional quantifier `∀_Y` / `∃_Y` is slid onto
 * its body's leaves by the LIBRARY's own chain case (§1): the chain is
 * collected whole, its body built over the live order minus the subscripts,
 * and the chain wrapped onto every leaf; a chain touching no decision
 * variable stays one leaf as it is. Nothing here prepares a term for that.
 *
 * Memoised per atom and per term within the call. The caller (§5) owns
 * `order` and passes it to every later term operation.
 *
 * @param body  the component's body, a `wff`
 * @param P     the component, `X`'s order
 * @param order `ctx.order`: `P[i] ↦ |P| - i` (inner → LOWER, §5)
 */
template <NodeType node>
tref prepare_terms(tref body, const block& P, const var_order<node>& order);

// --- cofactors and quantification -----------------------------------------------

/**
 * @brief §1 cofactor by CHILD SELECTION: `f[x ← bit]` for a BDD-backed `f`
 * and a DECISION variable `x` at any depth. It is the library's
 * `bdd_cofactor`, with the result emitted through
 * `convert_to_tau_node_or_term`. Canonical over the decision variables, and
 * NOT run through `simplify_term` — `COF` (layer 3) does that. Returns `f`
 * itself when `x` is not one of its decision variables, or when `f` is plain.
 * Nothing else is ever cofactored on.
 */
template <NodeType node>
tref cofactor(tref f, tref x, bool bit, const var_order<node>& order);

/**
 * @brief §1 `∀_X f = f₀·f₁` (`kind == tbdd::all`) and `∃_X f = f₀ ∪ f₁`
 * (`tbdd::ex`) over a whole block: ONE BDD quantification, never expanded to
 * 2^|X| terms. When `X` covers all decision variables this is the meet (the
 * join) of the leaves in a single traversal. `order` is the live order.
 * Returns `f` itself for a plain `f` or an empty `X`.
 */
template <NodeType node>
tref quantify_over(quantifier<node> kind, tref f, const block& X,
	const var_order<node>& order);

/// The term carries a functional quantifier (`SETTLE_FUNCTIONAL`'s test, §6).
template <NodeType node>
bool carries_functional_quantifier(tref f);

/**
 * @brief §3 `RESOLVE_FUNCTIONAL(φ, kf)`: resolve every functional-quantifier
 * chain of `n`, INNERMOST FIRST. This is the engine behind both
 * `RESOLVE_FUNCTIONAL` and `SETTLE_FUNCTIONAL` (§3, §6).
 *
 * `n` may be a `bf` term or a `wff` formula. Chains are found everywhere:
 * under binders, in reference arguments, in chain bodies and, for a
 * BDD-backed term, in its LEAVES. `order` is the LIVE order, empty when none
 * is live; every `BDD_ID` that `n` carries must belong to it (Debug-asserted).
 *
 * Per chain: canonicalise it through the library's constructor, which drops
 * degenerate and shadowed subscripts, merges runs, and already folds a closed
 * chain. Then, unless `keep` accepts the canonical prefix, quantify.
 *
 * A chain over a BDD-BACKED body is keep mode's whole-block emission
 * `Q_X (bf(BDD_ID))` (§7 `DISCHARGE`). It is settled where it lies, with one
 * quantification of the stored reference under the LIVE order, whenever the
 * subscripts are keys of that order and the prefix's nesting follows its
 * ranks. A prefix of one kind always qualifies, since a set quantification
 * commutes. The result is a BDD under the live order, emitted like every
 * other term — plain when nothing branches, which is the usual whole-block
 * outcome.
 *
 * Otherwise the GENERAL path: build the body as a BDD over an order that
 * ranks the chain's INNERMOST subscript lowest, quantify by the prefix and
 * spell the result with `to_tau_term`. A stored BDD that is not legal under
 * that order is spelled out first, a `BDD_ID` being a node of ONE order. The
 * result of this path is PLAIN, because a `BDD_ID` minted under the chain's
 * order would be a node of the wrong one.
 *
 * LEAF HAZARD (§1): a subscript hiding inside a leaf — in a reference
 * argument, or in a foreign-typed subterm — is not reached by the
 * quantification, so such a chain cannot be resolved and STAYS, rebuilt over
 * the body its own inner chains already resolved. That is the spec's
 * "partial" resolution.
 *
 * A chain left standing, whether kept or hazardous, is never swallowed by an
 * enclosing chain of the OTHER kind, so a `keep` decision taken inside holds
 * across a block boundary. A same-kind run above it belongs to the SAME
 * block, which merges into one chain and is decided once.
 *
 * Memoised per node by the traversal, within the call. Layer 3 wires the
 * cross-call `quant_memo` (§1: the key IS the chain over its plain body).
 */
template <NodeType node>
tref resolve_functional_quantifiers(tref n, const var_order<node>& order,
	const keep_functional_fn<node>& keep = keep_no_functional<node>);

// --- substitution inside a term -----------------------------------------------

/**
 * @brief §1 term-level `f[x ← t]`.
 *
 * For a BDD-backed `f` this is a BDD compose on the decision variable `x`
 * PLUS a rewrite of `x` inside every leaf (`r(x) ↦ r(t)`). The pair is sound
 * for any term, reaches every occurrence, and needs no leaf guard. For a
 * plain (non-BDD) term it is an ordinary replace. Either way, every reference
 * argument it touches is re-emitted through `simplify_term` — a formula
 * argument through `simplify_formula` (§1) — so arguments stay simplified
 * (invariant 6). Returns `f` itself when `x ∉ FV(f)`.
 *
 * A WITNESS IS PLAIN (§3): `t` carries no `BDD_ID` anywhere, so nothing is
 * spelled out here — the leaf rewrite puts `t` in as it stands and the
 * compose builds its BDD under `order`. A caller holding a BDD-backed term
 * spells it ONCE, before the substitution. Debug asserts this.
 *
 * Nothing is RENAMED here either: no binder of `f` may share a bound variable
 * with `t` (Debug asserts it). `subst_var` establishes that once, by renaming
 * the witness apart before its walk.
 */
template <NodeType node>
tref subst_term(tref f, tref x, tref t, const var_order<node>& order,
	const simplify_formula_fn& simplify_formula = identity_formula);

// --- the two aggressive normalisers of invariant 6 --------------------------------

/**
 * @brief §3 `SIMPLIFY_TERM(t)`: constant folding, absorption and complement
 * laws, per-path contradiction, and reduction to the canonical form of the
 * BDD backing `t`.
 *
 * Per-path contradiction is the rule `prop:xfx` — `x·f(x) = x·f(1)` and
 * `x′·f(x) = x′·f(0)` — so a subterm under a literal is reduced by that
 * literal's assignment.
 *
 * All four laws come from the existing `syntactic_path_simplification`
 * (heuristics/). A plain term goes through it whole. For a term under the
 * live order the representation is re-established first: a BDD-backed term is
 * already canonical over `P`, while a plain combination of BDD-backed
 * subterms (`f₀·f₁`, `f₀ ∪ f₁`, `f₁′`, the way `COF` forms its products and
 * complements) is built over `P`, absorbing them. Then every leaf goes
 * through the path simplifier and the BDD is rebuilt, so leaves that became
 * equal merge.
 *
 * Canonical up to LEAF EQUALITY only (§3): that is the single source of
 * incompleteness behind every syntactic test of the spec. A term that does
 * not touch `P` is in the plain regime whatever the order says.
 */
template <NodeType node>
tref simplify_term(tref t, const var_order<node>& order = {});

/**
 * @brief §3 `SIMPLIFY_ATOM(a)`: `simplify_term` on both sides, after which a
 * constant-only atom folds to `T` / `F` through the construction hooks — an
 * equation by the hooks' constant rules, a bitvector order atom by the
 * bitvector hook. `a` is a `wff` atom, optionally under one `¬`, which is
 * folded through.
 *
 * Two regimes. With no BDD-backed side — the initial phase-1 simplification,
 * phases 2 and 5, and every plain atom of the push, all order atoms among
 * them — the full existing `syntactic_atomic_formula_simplification` runs,
 * including the `norm_equation` / `denorm_equation` round trip on the joint
 * `l + r` and the per-variable `0`/`1` pass, which folds and shrinks before
 * any BDD exists. With a BDD-backed side the work is side-wise and shallow
 * and the atom is never reshaped; the joint fold belongs to `TERM_OF` and
 * `FOLD_DECIDED`.
 *
 * The plain regime is IDEMPOTENT (a test pins this), so an atom's shape as
 * written is its simplified shape and no memo key drifts — the spec's
 * "equations stay as written outside SQUEEZE".
 */
template <NodeType node>
tref simplify_atom(tref a, const var_order<node>& order = {});

// --- reading and rewriting equations ---------------------------------------------

/// §3 `TERM_OF(atom)`: for an equation `l = r`, seen through one optional
/// `¬`, the term `l + r`. Read off without touching the atom, since equations
/// stay as written (invariant 4). `order` is the live order: the ring sum of
/// two BDD-backed sides is a BDD operation, and a plain side is a leaf under
/// it. `nullptr` on anything but an equation, which is a caller bug and is
/// Debug-asserted; every caller guards.
template <NodeType node>
tref term_of(tref atom, const var_order<node>& order);

/// §3 `NORM_EQUATION`: `l = r ↦ l + r = 0`, descending through one `¬`.
/// Called in exactly one place, `SQUEEZE` step 1 (§7).
template <NodeType node>
tref norm_equation(tref atom, const var_order<node>& order);

// --- in-memory size and the leaf hazard -------------------------------------------

/**
 * @brief §1 `‖t‖`: the size of a TERM as it is held IN MEMORY (§10).
 *
 * When `t` is BDD-backed, this is the unique node count of its BDD, leaves
 * included and the terminals counted as zero. When `t` is plain — phase 2's
 * spelled-out witnesses are plain — it is the number of distinct
 * term-structure nodes: each operator, functional quantifier and leaf (a
 * variable, constant or reference) counts once, the `bf` wrappers are not
 * counted, and a leaf's own nodes are not entered.
 *
 * The one consumer is `TRY_WITNESS`'s tie-break among a variable's pins, the
 * smallest `‖f₁′‖`, which is the cost every later compose or replace of the
 * witness pays. There is no formula overload: nothing in the spec reads
 * `‖φ‖`, the decomposition licence uses `|·|` (`formula_size`). The BDD case
 * is the library's `node_count`.
 */
template <NodeType node>
size_t mem_size(tref t);

/**
 * @brief §1 LEAF HAZARD: the free variables contributed by the LEAVES of a
 * BDD-backed term alone; for a plain term, simply its `FV`. Sorted like
 * `get_free_vars`.
 *
 * A block variable that shows up here is hidden from cofactoring: it sits
 * inside a reference argument or a foreign-typed subterm, including one
 * inside a functional quantifier's body. This differs from `FV` in exactly
 * that case — a block variable that is both a decision variable and hidden in
 * a leaf — which is what `ELIMINATE_BLOCK`'s `opaque?` test (§7) asks before
 * any quantification.
 *
 * The BDD case is the library's `get_free_leaf_vars`, returned BY VALUE and
 * not stored: the per-leaf sets are `get_free_vars`' own cached entries, and
 * their union is one walk over the BDD's distinct nodes per call.
 */
template <NodeType node>
trefs leaf_fv(tref f);

} // namespace idni::tau_lang::anti_prenexing

#include "terms.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_H__
