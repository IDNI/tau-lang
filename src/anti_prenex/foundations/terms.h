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
 * The primitives declared here are §3's `PREPARE_TERMS`,
 * `RESOLVE_FUNCTIONAL_PLAIN`, `RESOLVE_FUNCTIONAL_BDD`, `SIMPLIFY_TERM`,
 * `SIMPLIFY_ATOM`, `TERM_OF` and `NORM_EQUATION`, plus the in-memory term size
 * `‖·‖` (§1, §10).
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
//   RESOLVE_FUNCTIONAL_*   resolve_functional_quantifiers_plain / _bdd (below)
//   φ[x ← t]  /  f[x ← t]  tree<node>::substitute(φ, bf(x), t, order, hook)
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
// kind-homogeneous, so a block quantification never needs the library's
// mixed-prefix `bdd_quant` — only an alternating functional-quantifier chain
// does (the resolvers below) — and a caller that holds a kind (`DISCHARGE`)
// passes it through.
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

/// The term carries a functional quantifier, a BDD-backed term's leaves
/// included: §7's opacity test, which freezes what the solver cannot read.
template <NodeType node>
bool carries_functional_quantifier(tref f);

/**
 * @brief The MAXIMAL functional-quantifier chain hanging off the `bf` node
 * @p n (§1, §3): its prefix OUTERMOST FIRST with the kinds, and the body it
 * sits on. An empty prefix — and @p n itself — when @p n is no chain. The
 * whole nest is taken, kinds mixed freely, because that is the ONE unit of
 * work of the two resolvers (below).
 *
 * It is also how a `keep_functional_fn` (fwd.h) reads the chain NODE it is
 * handed: prefix and body come off the node with this one call.
 */
template <NodeType node>
std::pair<typename tau_term_bdd<node>::quants, tref> strip_chain(tref n);

// --- the two resolvers (RESOLVE_FUNCTIONAL_PLAIN, RESOLVE_FUNCTIONAL_BDD) --------
//
// THE UNIT OF WORK IS THE MAXIMAL CHAIN, any mix of `∀` and `∃`, taken at its
// TOP and quantified in ONE pass. Both resolvers canonicalise its prefix
// through the library's constructor — which drops a degenerate or shadowed
// subscript, sorts each same-kind run into content order, merges an adjoining
// run of the body and folds a closed plain chain — and then ask `keep` ONCE, on
// that canonical chain's NODE: the prefix outermost first with the kinds, and
// the body below it, both read off with `strip_chain` (above). A yes keeps the
// whole chain, and nothing inside it is looked at again.
//
// THE STUCK RULE says what a chain resolves into. A subscript sitting inside a
// LEAF — in a reference argument, in a foreign-typed subterm — is reached by no
// quantification (§1 LEAF HAZARD) and stays. The rest are decided INNERMOST
// FIRST: ahead of the first hidden subscript every subscript is quantified; the
// first hidden one opens a stuck run of ITS KIND, into which an outer subscript
// of that kind still commutes — quantified when it is not hidden, joining the
// run when it is — and the first subscript of the OTHER kind blocks, staying
// along with everything outward of it. So `∃x ∀y (x·r(y))` resolves nothing,
// `∀y ∃x (x·r(y))` gives `∀y r(y)`, and `∃x ∃y (x·y·r(y))` gives
// `∃y (y·r(y))`. What is taken is ONE quantification, and the prefix left over
// is re-attached over the result through the constructor. A choice of SIZE, not
// of soundness: crossing an alternation by cofactors is legal, and duplicates
// the stuck chain.
//
// Neither resolver has a memo of its own: the entry pass runs once, and the
// close's quantifications are covered by the library's memos.

/**
 * @brief §3 `RESOLVE_FUNCTIONAL_PLAIN(φ, kf)`: resolve the
 * functional-quantifier chains of `n` that `keep` does not keep, at ENTRY
 * (phase 1), before any order is live.
 *
 * `n` may be a `bf` term or a `wff` formula, and nothing in it is BDD-backed
 * (Debug-asserted). Chains are found everywhere — under binders, in reference
 * arguments, in chain bodies — and what is resolved before a chain is what sits
 * NESTED inside it, never an adjacent binder.
 *
 * The body of a chain to resolve is built as a BDD over the CHAIN'S OWN order,
 * its innermost subscript lowest, and what comes back is spelled PLAIN before
 * the pass moves on: a node minted under a chain's order belongs to no live
 * order. A body whose BDD does not branch holds every subscript in a leaf, and
 * that chain stays as it is.
 *
 * After this pass a chain is a kept one or a stuck one, and both stay: no later
 * step resolves a chain it finds in a leaf.
 */
template <NodeType node>
tref resolve_functional_quantifiers_plain(tref n,
	const keep_functional_fn<node>& keep = keep_no_functional<node>);

/**
 * @brief §3 `RESOLVE_FUNCTIONAL_BDD(φ, order, kf)`: the same for a component's
 * CLOSE (§5), where `order` is the LIVE order and every BDD-backed term of `n`
 * belongs to it (Debug-asserted).
 *
 * A chain met here sits over a BDD-backed body with its subscripts among the
 * decision variables — keep mode's whole-block emission `Q_X (bf(BDD_ID))`,
 * `X ⊆ P` (§7 `DISCHARGE`) — so ONE quantification of the STORED BDD under the
 * live order settles it where it lies, and the result is emitted like every
 * other term: plain when nothing branches, which is the usual whole-block
 * outcome. A chain over a PLAIN body is one whose BDD did not branch, every
 * subscript of it in a leaf, and it stays as it is.
 *
 * LEAVES ARE NEVER ENTERED: a chain in a leaf is an inner block's kept emission
 * or a stuck one, and both are decided.
 */
template <NodeType node>
tref resolve_functional_quantifiers_bdd(tref n, const var_order<node>& order,
	const keep_functional_fn<node>& keep = keep_no_functional<node>);

// --- the two aggressive normalisers of invariant 6 --------------------------------

/**
 * @brief §3 `SIMPLIFY_TERM(t, order = ∅)`: constant folding, complement
 * laws, per-path contradiction, the cofactor check, and reduction to the
 * canonical form of the BDD backing `t`.
 *
 * Per-path contradiction is the rule `prop:xfx` — `x·f(x) = x·f(1)` and
 * `x′·f(x) = x′·f(0)` — so a subterm under a literal is reduced by that
 * literal's assignment. Absorption is its literal instance (`x ∪ x·c = x`);
 * a compound one (`ab ∪ ab·c`) stays as written.
 *
 * The first three laws are THE SWEEP, the existing
 * `syntactic_path_simplification` (heuristics/). A plain term goes through it
 * whole. For a term under the live order the representation is re-established
 * first: a BDD-backed term is already canonical over `P`, while a plain
 * combination of BDD-backed subterms (`f₀·f₁`, `f₀ ∪ f₁`, `f₁′`, the way
 * `COF` forms its products and complements) is built over `P`, absorbing
 * them. Then every leaf goes through the path simplifier and the BDD is
 * rebuilt, so leaves that became equal merge.
 *
 * THE COFACTOR CHECK is one-level Shannon reduction of the WHOLE term over
 * every free variable of `t` that is no variable of `order`: an order
 * variable is one the BDD has decided already, and one still free in a leaf
 * sits where the expansion is not licensed (§1's leaf hazard). For each such
 * `y`, `f₀ = t[y←0]` and `f₁ = t[y←1]`, the substitution reaching BOOLEAN
 * POSITIONS only — under `∪ · ′ +` and the functional quantifiers, never
 * under an arithmetic operator, a reference or a foreign-typed subterm, where
 * no Shannon expansion holds — with constants folding through the
 * construction hooks; `f₀ = f₁`, a constant included, makes `t ← f₀`. It is
 * sound for a variable that also sits inside a reference: the reference is an
 * opaque element the expansion is taken around. It is what stops nested Boole
 * normal forms from compounding as substitutions stack terms inside terms.
 *
 * TWO PHASES, no interplay: the sweep first, then the check ONCE over the
 * swept term, one pass over its variables on the running term. A cofactor is
 * what the substitution hands back, folded by the hooks and simplified no
 * further, and the check's result is emitted as it stands. So this is NOT
 * idempotent: a second call may simplify further.
 *
 * A term CARRYING a stored BDD is in the BDD regime too, whether or not it is
 * backed itself: `Q_P (bf(BDD_ID))`, keep mode's emission of a discharged
 * block (§7 `DISCHARGE`), is `P`-free and stays one leaf (§1 — a stored BDD
 * is never spelled out before the component's close). Such a leaf is
 * simplified over its plain structure, every `bf(BDD_ID)` in it an opaque
 * leaf of the sweep, and each stored BDD it holds is simplified from the
 * inside instead. So the emission comes back as it stands, with the same
 * stored BDD, its own leaves simplified.
 *
 * Canonical up to LEAF EQUALITY only (§3): that is the single source of
 * incompleteness behind every syntactic test of the spec. A term that
 * neither touches `P` nor carries a stored BDD is in the plain regime
 * whatever the order says.
 */
template <NodeType node>
tref simplify_term(tref t, const var_order<node>& order = {});

/**
 * @brief §3 `SIMPLIFY_ATOM(a, order = ∅)`: ONE recipe in both regimes.
 * `simplify_term` on both sides, the atom rebuilt through the construction
 * hooks, which fold a constant-only atom and equal sides. `a` is a `wff`
 * atom, optionally under one `¬`, which is folded through.
 *
 * For a (¬)equation the joint ring sum `l + r` — `term_of`'s, a BDD
 * operation once a side is BDD-backed — then goes through `simplify_term` as
 * a DECISION alone: `0` decides `=` as `T`, a nonzero constant as `F`, dually
 * for `≠` (phase 1 meets one before `NORMALIZE_OPERATORS`). It is what sees
 * two sides no hook can compare, two `BDD_ID` nodes above all. Anything else
 * leaves the atom as its sides stand: the atom is NEVER reshaped, and
 * `norm_equation` (`SQUEEZE`) stays the one rewrite of an equation's shape.
 * An order atom is side-wise alone.
 *
 * NOT IDEMPOTENT: a second call may simplify further, since the cofactor
 * check's result is not swept again (`simplify_term`). No memo key drifts
 * even so, because an atom is simplified ONCE, where it is built, and again
 * only when a substitution rewrote it.
 */
template <NodeType node>
tref simplify_atom(tref a, const var_order<node>& order = {});

// --- reading and rewriting equations ---------------------------------------------

/// §3 `TERM_OF(atom)`: for an equation `l = r` — or `l ≠ r`, which phase 1
/// meets before `NORMALIZE_OPERATORS` — seen through one optional `¬`, the
/// term `l + r`. Read off without touching the atom, since equations stay as
/// written (invariant 4). `order` is the live order: the ring sum of two
/// BDD-backed sides is a BDD operation, and a plain side is a leaf under it.
/// `nullptr` on anything else, which is a caller bug and is Debug-asserted;
/// every caller guards.
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
