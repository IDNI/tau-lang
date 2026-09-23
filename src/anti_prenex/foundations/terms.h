// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file terms.h
 * @brief Anti-prenexing foundations: TERM BDDs and the term-level primitives
 * of §3.
 *
 * THE §1 TERM REPRESENTATION, one per component: the decision variables of
 * the BDD are exactly the component's block `P`, and everything else sits in
 * the leaves. Cofactoring is child selection; `∀_P` and `∃_P` are the meet
 * and the join of the leaves; substitution is a BDD compose plus a rewrite of
 * the leaves; functional quantifiers are transparent. What the BDD cannot see
 * is the LEAF HAZARD: a variable of `P` may also occur inside a leaf, where
 * no quantification reaches it.
 *
 * THE LIBRARY UNDERNEATH is `tau_term_bdd_handle<node>` (aliased
 * `term_handle`) and `tau_term_bdd<node>` below it (tau_bdd.h).
 * `build(term, order)` treats exactly the order's keys as decision variables
 * and everything else as a leaf, which IS this representation. A BDD-backed
 * term is stored as a `bf(BDD_ID)` node, interned per type, so the same BDD
 * is the same Tau node everywhere.
 *
 * THE LIVE ORDER IS THE CALLER'S. Every BDD primitive here takes the order as
 * an explicit parameter — the component's `ctx.order` (§5) — and nothing in
 * this file stores one. A `BDD_ID` node belongs to ONE order, so a component
 * converts its terms back (`convert_to_tau_terms`) before another
 * `prepare_terms`. The two simplifiers default to the empty order, the plain
 * regime of phases 1, 2 and 5, where nothing is BDD-backed (§3). Debug
 * builds check at every BDD primitive's entry that the reference passed in
 * is a reduced ordered BDD under `order` (`tau_term_bdd::is_ordered`).
 *
 * WHEN A TERM IS BDD-BACKED: exactly when it has a decision variable, that
 * is, when it touches `P`. A BDD that has become a single leaf or a terminal
 * comes back as its plain term, so a `P`-free term never carries a `BDD_ID`
 * — with one exception: a functional-quantifier chain over a stored BDD,
 * which `discharge` (cofactors.h) emits in keep mode. It is `P`-free, keeps
 * its BDD body until the component's close, and the simplifiers treat it as
 * BDD-backed.
 *
 * `term_of` and `norm_equation` live here because both are term operations
 * (`l + r` is a ring sum). NAME CLASH: inside this namespace `norm_equation`
 * hides `tau_lang::norm_equation` (normal_forms_transformations.h), which
 * does NOT descend through a formula negation. Call that one fully qualified.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_H__

#include "fwd.h"
#include "dag.h"

namespace idni::tau_lang::anti_prenexing {

// --- §1 vocabulary with a library counterpart -----------------------------------
//
// Not declared here — used directly, on a `bf` term `t` or a formula `phi`:
//
//   BDD-backed term        term_handle<node>::is_bdd_backed(t)
//   the library's spelling  term_handle<node>::convert_to_tau_terms(phi)
//   ∀_X f  /  ∃_X f        quantify_over(tbdd::all / tbdd::ex, f, X, order)
//   f[x ← bit]             tau_term_bdd<node>::bdd_cofactor (behind `cofactor`)
//   symbolic ∀_Y f / ∃_Y f tau_term_bdd<node>::build_functional_quantifiers
//   RESOLVE_FUNCTIONAL_*   resolve_functional_quantifiers_plain / _bdd (below)
//   φ[x ← t]  /  f[x ← t]  tree<node>::substitute(φ, bf(x), t, order, hook)
//
// BDD-BACKED: the term is a `bf(BDD_ID)` node — backed by a BDD under the
// live order, with at least one decision variable. The `bf` WRAPPER is the
// form that counts; a bare `BDD_ID` node is not one.
//
// THE LIBRARY'S SPELLING: every `BDD_ID` term of `phi` converted back to a
// plain `bf`, wherever it sits. One memoised walk with no node kind
// special-cased, and the inverse of `prepare_terms` up to term normal form:
// `prepare_terms(convert_to_tau_terms(prepared))` yields the same BDD and,
// by interning, the same `BDD_ID` node. It is the library HALF of §3's
// `FINISH_TERMS` (below), which spells an atom out with it and then puts the
// result into the plain regime's normal form.
//
// SYMBOLIC ∀_Y / ∃_Y: the library's chain constructor takes `quants`
// outermost first. It is canonical — absent subscripts dropped, each
// same-kind run in content order, an adjoining run of the body merged — and
// it folds a chain that binds every free variable of a plain body to its
// constant.

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
 * A formula-level binder, a reference and a temporal operator are opaque and
 * not entered. A term-level functional quantifier `∀_Y` / `∃_Y` is slid onto
 * its body's leaves by the LIBRARY's own chain case (§1): the chain is
 * collected whole, its body built over the live order minus the subscripts,
 * and the chain wrapped onto every leaf; a chain touching no decision
 * variable stays one leaf as it is.
 *
 * Memoised per term within the call. Idempotent: an already prepared term is
 * returned as it is.
 *
 * @param body  the component's body, a `wff`
 * @param P     the component's block, in binder order
 * @param order `ctx.order`: `P[i] ↦ |P| - i` (inner → LOWER, §5)
 */
template <NodeType node>
tref prepare_terms(tref body, const block& P, const var_order<node>& order);

/**
 * @brief §3 primitive `FINISH_TERMS(φ)`: `PREPARE_TERMS`' inverse, run at a
 * component's close (§5) — every BDD-backed term of @p phi spelled back out
 * as a plain term, wherever it sits.
 *
 * ONE WALK over the formula skeleton. An ATOM carrying a stored BDD anywhere
 * — on a side, or in the body of a functional-quantifier chain `DISCHARGE`
 * emitted in keep mode — is spelled out by the library and then put through
 * `simplify_atom` in the PLAIN REGIME, the empty order: exactly what phase 1
 * does to every atom. A junction the walk closes is re-emitted canonically,
 * because a spelled atom may sort elsewhere among its siblings or fold to a
 * constant, which then decides the junction. A `¬` over an atom is rebuilt
 * over the spelled one, the hooks folding `¬T` and `¬F`. Nothing else is
 * touched, so a formula holding no stored BDD comes back as the SAME node.
 *
 * THE WALK MAY STOP AT A TERM, an atom's own sides apart: a stored BDD never
 * sits in a REFERENCE ARGUMENT. `PREPARE_TERMS` transports a reference
 * opaque, and the library's substitute spells a backed witness out before it
 * enters an argument, so there is nothing below a reference to spell.
 *
 * Why the plain normal form and not the library's raw spelling: every
 * spelled term goes through the plain-regime simplification phase 1 applies
 * to every atom, a kept chain's body included, so the terms leaving a
 * component are normalised the way the phases around the push normalise
 * theirs — ONE normal form on both sides of the module.
 *
 * @param phi a `wff` node, at the close of one component
 * @return the same formula with every term plain
 */
template <NodeType node>
tref finish_terms(tref phi);

// --- cofactors and quantification -----------------------------------------------

/**
 * @brief §1 cofactor by CHILD SELECTION: `f[x ← bit]` for a BDD-backed `f`
 * and a DECISION variable `x` at any depth. It is the library's
 * `bdd_cofactor`, with the result emitted through
 * `convert_to_tau_node_or_term`. Canonical over the decision variables, and
 * NOT run through `simplify_term`; `cof` (cofactors.h) does that. Returns `f`
 * itself when `x` is not one of its decision variables, or when `f` is plain.
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

/// Does the term carry a functional quantifier anywhere, a BDD-backed term's
/// leaves included?
template <NodeType node>
bool carries_functional_quantifier(tref f);

/**
 * @brief The MAXIMAL functional-quantifier chain hanging off the `bf` node
 * @p n: its prefix OUTERMOST FIRST with the kinds, and the body it sits on.
 * An empty prefix — and @p n itself — when @p n is no chain. Kinds mix
 * freely: the whole nest is one chain. This is also how a
 * `keep_functional_fn` (fwd.h) reads the chain node it is handed.
 */
template <NodeType node>
std::pair<typename tau_term_bdd<node>::quants, tref> strip_chain(tref n);

// --- the two resolvers (RESOLVE_FUNCTIONAL_PLAIN, RESOLVE_FUNCTIONAL_BDD) --------
//
// THE UNIT OF WORK IS THE MAXIMAL CHAIN, any mix of `∀` and `∃`, taken at its
// top and quantified in ONE pass. Both resolvers canonicalise its prefix
// through the library's constructor and then ask `keep` ONCE, on that
// canonical chain's node (`strip_chain` reads prefix and body off it). A yes
// keeps the whole chain, and nothing inside it is looked at again.
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
// `∃y (y·r(y))`. The prefix left over is re-attached over the result through
// the constructor. A choice of SIZE, not of soundness: crossing an alternation
// by cofactors is legal, and duplicates the stuck chain.
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
 * @brief §3 `RESOLVE_FUNCTIONAL_BDD(φ, order, kf)`: the same at a component's
 * CLOSE (§5), where `order` is the LIVE order and every BDD-backed term of `n`
 * belongs to it (Debug-asserted).
 *
 * A chain met here sits over a BDD-backed body with its subscripts among the
 * decision variables (a keep-mode emission of `discharge`), so ONE
 * quantification of the STORED BDD under the live order settles it where it
 * lies, and the result is emitted like every other term: plain when nothing
 * branches. A chain over a PLAIN body has every subscript in a leaf and stays
 * as it is. Leaves are never entered: a chain in a leaf is decided already.
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
 * whole. A term in the BDD regime — BDD-backed, a plain combination of
 * BDD-backed subterms, or a term carrying a stored BDD anywhere — is first
 * (re)built over the order, then every leaf goes through the sweep and the
 * BDD is rebuilt, so leaves that became equal merge. A term that neither
 * touches the order nor carries a stored BDD is in the plain regime whatever
 * the order says.
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
 * what stops nested Boole normal forms from compounding as substitutions
 * stack terms inside terms.
 *
 * TWO PHASES, no interplay: the sweep first, then the check ONCE over the
 * swept term. A cofactor is what the substitution hands back, folded by the
 * hooks and simplified no further. So this is NOT idempotent: a second call
 * may simplify further.
 *
 * Canonical up to LEAF EQUALITY only (§3): that is the single source of
 * incompleteness behind every syntactic test of the spec.
 */
template <NodeType node>
tref simplify_term(tref t, const var_order<node>& order = {});

/**
 * @brief §3 `SIMPLIFY_ATOM(a, order = ∅)`: ONE recipe in both regimes.
 * `simplify_term` on both sides, the atom rebuilt through the construction
 * hooks, which fold a constant-only atom and equal sides. `a` is a `wff`
 * atom, optionally under one `¬`, which is folded through.
 *
 * For a (¬)equation the joint ring sum `l + r` (`term_of`) then goes through
 * `simplify_term` as a DECISION alone: `0` decides `=` as `T`, a nonzero
 * constant as `F`, dually for `≠`. It is what sees two sides no hook can
 * compare, two `BDD_ID` nodes above all. Anything else leaves the atom as
 * its sides stand: the atom is NEVER reshaped, and `norm_equation` stays the
 * one rewrite of an equation's shape. An order atom is simplified side-wise
 * alone.
 *
 * NOT IDEMPOTENT, like `simplify_term`.
 */
template <NodeType node>
tref simplify_atom(tref a, const var_order<node>& order = {});

// --- reading and rewriting equations ---------------------------------------------

/// §3 `TERM_OF(atom)`: for an equation `l = r` or `l ≠ r`, seen through one
/// optional `¬`, the term `l + r`. Read off without touching the atom
/// (invariant 4). `order` is the live order: the ring sum of two BDD-backed
/// sides is a BDD operation, and a plain side is a leaf under it. `nullptr`
/// on anything else, which is a caller bug and is Debug-asserted.
template <NodeType node>
tref term_of(tref atom, const var_order<node>& order);

/// §3 `NORM_EQUATION`: `l = r ↦ l + r = 0`, descending through one `¬`.
/// Anything that is no equation comes back as it is.
template <NodeType node>
tref norm_equation(tref atom, const var_order<node>& order);

// --- in-memory size and the leaf hazard -------------------------------------------

/**
 * @brief §1 `‖t‖`: the size of a TERM as it is held IN MEMORY (§10), the
 * cost every later compose or replace of the term pays.
 *
 * When `t` is BDD-backed, this is the unique node count of its BDD, leaves
 * included and the terminals counted as zero. When `t` is plain it is the
 * number of distinct term-structure nodes: each operator, functional
 * quantifier and leaf (a variable, constant or reference) counts once, the
 * `bf` wrappers are not counted, and a leaf's own nodes are not entered.
 * There is no formula overload; the formula measure is `formula_size`.
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
 * a leaf.
 *
 * The BDD case is the library's `get_free_leaf_vars`, returned BY VALUE and
 * not stored: the per-leaf sets are `get_free_vars`' own cached entries, and
 * their union is one walk over the BDD's distinct nodes per call.
 */
template <NodeType node>
trefs leaf_fv(tref f);

// --- the PURE shapes --------------------------------------------------------------

/**
 * @brief §1 PURE term: built from variables, constants, the BA operations
 * `∪ · ′ +` and functional quantifiers alone — no reference, no arithmetic
 * operator (a width cast is one), no subterm of another type.
 *
 * @p type is the BA type the term is read under; a node carrying the untyped
 * id `0` carries no type of its own and is of it. A BDD-backed term is read
 * LEAF BY LEAF: its decision variables are variables, so everything a stored
 * BDD could hide sits in a leaf.
 *
 * The shape has a name because it is the literal shape §7's engines decide
 * without a solver; §6's 2a takes only these as its top-level positives, and
 * so does the consistency check.
 */
template <NodeType node>
bool is_pure_term(tref t, ba_type_id type);

/**
 * @brief §1 PURE (¬)equation: an equation, seen through one optional `¬`,
 * both of whose sides are pure terms of @p type. An ORDER ATOM is never one,
 * and neither is anything that is no equation — a reference, a unit, a
 * temporal operator.
 *
 * A `bf_neq` atom is no equation here: after phase 3 a negative leaf is
 * `¬(l = r)` (invariant 4), so none is left where this is asked.
 */
template <NodeType node>
bool is_pure_equation(tref literal, ba_type_id type);

} // namespace idni::tau_lang::anti_prenexing

#include "terms.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_H__
