// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file terms.h
 * @brief Anti-prenexing foundations: TERM BDDs and the term-level primitives
 * of §3.
 *
 * THE §1 TERM REPRESENTATION: every variable met at a Boolean position is a
 * decision variable of the BDD, and everything else sits in the leaves.
 * Cofactoring is child selection; `∀_P` and `∃_P` are the meet and the join
 * of what hangs at `P`'s levels; substitution is a BDD compose plus a rewrite
 * of the leaves. What the BDD cannot see is the LEAF HAZARD: a variable may
 * also occur inside a leaf, where no quantification reaches it.
 *
 * THE LIBRARY UNDERNEATH is `tau_term_bdd_handle<node>` (aliased
 * `term_handle`) and `tau_term_bdd<node>` below it (tau_bdd.h). `build(term)`
 * makes every variable at a Boolean position a decision variable and
 * everything else a leaf, which IS this representation. A BDD-backed term is
 * stored as a `bf(BDD_ID)` node, interned per type, so the same BDD is the
 * same Tau node everywhere.
 *
 * ONE ORDER, THE LIBRARY'S global comparator, for every call (§1): nothing
 * here takes or stores an order. The two simplifiers take the REGIME instead
 * (fwd.h), `plain` by default, the regime of phases 1 and 2, where nothing
 * is BDD-backed (§3). Debug builds check at every BDD primitive's entry that
 * the reference passed in is a reduced ordered BDD
 * (`tau_term_bdd::is_ordered`).
 *
 * WHEN A TERM IS BDD-BACKED: exactly when its BDD branches. A BDD that has
 * become a single leaf or a terminal comes back as its plain term, so a term
 * with no variable at a Boolean position never carries a `BDD_ID`.
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
//   FINISH_TERMS(φ)        term_handle<node>::convert_to_tau_terms(phi)
//   ∀_X f  /  ∃_X f        quantify_over(tbdd::all / tbdd::ex, f, X)
//   f[x ← bit]             tau_term_bdd<node>::bdd_cofactor (behind `cofactor`)
//   symbolic ∀_Y f / ∃_Y f tau_term_bdd<node>::build_functional_quantifiers
//   the slide of a chain   tau_term_bdd<node>::build_bdd of the chain
//   RESOLVE_FUNCTIONAL_PLAIN resolve_functional_quantifiers_plain (below)
//   φ[x ← t]  /  f[x ← t]  tree<node>::substitute(φ, bf(x), t, hook)
//
// BDD-BACKED: the term is a `bf(BDD_ID)` node — backed by a BDD with at
// least one decision variable. The `bf` WRAPPER is the form that counts; a
// bare `BDD_ID` node is not one.
//
// FINISH_TERMS, the call's ONE SPELLING, after phase 5's `SIMPLIFY`: every
// `BDD_ID` term of `phi` converted back to a plain `bf`, wherever it sits,
// by the library's `to_tau_term` — Shannon per node, the OR form where a
// cofactor is 1, the ring sum where the cofactors are complements. One
// memoised walk with no node kind special-cased, and the inverse of
// `prepare_terms` up to term normal form:
// `prepare_terms(convert_to_tau_terms(prepared))` yields the same BDD and,
// by interning, the same `BDD_ID` node.
//
// SYMBOLIC ∀_Y / ∃_Y: the library's chain constructor takes `quants`
// outermost first. It is canonical — absent subscripts dropped, each
// same-kind run in content order, an adjoining run of the body merged — and
// it folds a chain that binds every free variable of its body, plain or
// backed, to its constant, unless a subscript hides in a leaf.
//
// THE SLIDE: building a chain into a BDD slides it down its body to its
// subscripts' levels (§1 transparency). The variables above stay decision
// structure, and each piece over the subscripts' levels becomes one leaf, the
// constructor's chain over that piece, so a closed piece folds.

// --- the representation boundary (PREPARE_TERMS) ---------------------------------

/**
 * @brief §3 primitive `PREPARE_TERMS(φ)`: phase 4's ONE conversion, at its
 * start — the whole formula put into §1's term representation, which it
 * keeps until phase 5's spelling.
 *
 * Both sides of every EQUATION holding a variable — one cached free-variable
 * test each — are rebuilt as terms backed by a BDD under the library's one
 * order, everything not at a Boolean position going into the leaves; a side
 * whose BDD does not branch stays plain. An ORDER ATOM is left plain whatever
 * it touches: it is never cofactored (`cof_memo` keys are `TERM_OF` of
 * equations) and the solver path consumes it as written.
 *
 * The walk ENTERS formula binders and temporal operators and stops at a term,
 * so a reference argument stays plain. A term-level functional-quantifier
 * chain is SLID by the build (§1): the variables above its subscripts' levels
 * are decision variables like any other.
 *
 * One walk, memoised per node and per term. Idempotent: an already prepared
 * term is returned as it is.
 *
 * @param phi a `wff`
 */
template <NodeType node>
tref prepare_terms(tref phi);

// --- cofactors and quantification -----------------------------------------------

/**
 * @brief §1 cofactor by CHILD SELECTION, the RESTRICT: `f[x ← bit]` for a
 * BDD-backed `f` and a variable `x` at any depth. It is the library's
 * `bdd_cofactor`, with the result emitted through
 * `convert_to_tau_node_or_term`. Canonical over the decision variables, and
 * NOT run through `simplify_term`, by `cof` (cofactors.h) either. Returns
 * `f` itself when `x` is not one of its decision variables, or when `f` is
 * plain.
 */
template <NodeType node>
tref cofactor(tref f, tref x, bool bit);

/**
 * @brief §1 `∀_X f = f₀·f₁` (`kind == tbdd::all`) and `∃_X f = f₀ ∪ f₁`
 * (`tbdd::ex`) over a whole block: ONE BDD quantification, never expanded to
 * 2^|X| terms. When `X` covers all decision variables this is the meet (the
 * join) of the leaves in a single traversal. Returns `f` itself for a plain
 * `f` or an empty `X`.
 */
template <NodeType node>
tref quantify_over(quantifier<node> kind, tref f, const block& X);

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

// --- the resolver (RESOLVE_FUNCTIONAL_PLAIN) --------------------------------------
//
// THE UNIT OF WORK IS THE MAXIMAL CHAIN, any mix of `∀` and `∃`, taken at its
// top and quantified by `bdd_quant`, one same-kind run at a time, innermost
// run first. The resolver canonicalises its prefix through the library's
// constructor and then asks `keep` ONCE, on that canonical chain's node
// (`strip_chain` reads prefix and body off it). A yes keeps the whole chain,
// and nothing inside it is looked at again.
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
// The resolver has no memo of its own: the entry pass runs once.

/**
 * @brief §3 `RESOLVE_FUNCTIONAL_PLAIN(φ, kf)`: resolve the
 * functional-quantifier chains of `n` that `keep` does not keep, at ENTRY
 * (phase 1).
 *
 * `n` may be a `bf` term or a `wff` formula, and nothing in it is BDD-backed
 * (Debug-asserted). Chains are found everywhere — under binders, in reference
 * arguments, in chain bodies — and what is resolved before a chain is what sits
 * NESTED inside it, never an adjacent binder.
 *
 * The body of a chain to resolve is built as a BDD under the global order,
 * phase 0's canonical ids putting its subscripts, innermost lowest, at the
 * bottom, and what comes back is spelled PLAIN before the pass moves on, phase
 * 1 being the plain regime. A chain nested in the body that a kept or stuck
 * one left standing is SLID by that build below the outer subscripts' levels,
 * where their quantification crosses it (§1 transparency). A body whose BDD
 * does not branch holds every subscript in a leaf, and that chain stays as it
 * is.
 *
 * After this pass a chain is a kept one or a stuck one, and both stay: no later
 * step resolves a chain it finds in a leaf.
 */
template <NodeType node>
tref resolve_functional_quantifiers_plain(tref n,
	const keep_functional_fn<node>& keep = keep_no_functional<node>);

// --- the two aggressive normalisers of invariant 6 --------------------------------

/**
 * @brief §3 `SIMPLIFY_TERM(t, order = ∅)`: constant folding, complement
 * laws, per-path contradiction, the cofactor check, and reduction to the
 * canonical form of the BDD backing `t`. The spec's empty order is the
 * `plain` regime, its global one the `backed` regime (fwd.h).
 *
 * Per-path contradiction is the rule `prop:xfx` — `x·f(x) = x·f(1)` and
 * `x′·f(x) = x′·f(0)` — so a subterm under a literal is reduced by that
 * literal's assignment. Absorption is its literal instance (`x ∪ x·c = x`);
 * a compound one (`ab ∪ ab·c`) stays as written.
 *
 * The first three laws are THE SWEEP, the existing
 * `syntactic_path_simplification` (heuristics/). In the plain regime a term
 * goes through it whole. In the backed regime a term holding a variable or a
 * stored BDD — BDD-backed, or carrying a stored BDD inside a leaf — is first
 * (re)built, then every leaf goes through the sweep and the BDD is rebuilt,
 * so leaves that became equal merge; a term holding neither is swept whole.
 * A raw tree combining stored BDDs under a Boolean operator is never handed
 * in: terms are combined by the library's operations (Debug-asserted, here
 * and in `simplify_atom`).
 *
 * THE COFACTOR CHECK runs in the plain regime alone: in the backed one every
 * variable is a variable of the order, decided by the BDD already or sitting
 * in a leaf, where the expansion is not licensed (§1's leaf hazard). It is
 * one-level Shannon reduction of the WHOLE term over every free variable of
 * `t`. For each `y`, `f₀ = t[y←0]` and `f₁ = t[y←1]`, the substitution
 * reaching BOOLEAN
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
tref simplify_term(tref t, regime rg = regime::plain);

/**
 * @brief §3 `SIMPLIFY_ATOM(a, order = ∅)`: ONE recipe in both regimes.
 * `simplify_term` on both sides, the atom rebuilt through the construction
 * hooks, which fold a constant-only atom and equal sides. `a` is a `wff`
 * atom, optionally under one `¬`, which is folded through.
 *
 * For a (¬)equation the joint ring sum `l + r` (`term_of`) then goes through
 * `simplify_term` as a DECISION alone, skipped when either simplified side is
 * `0`: `0` decides `=` as `T`, a nonzero constant as `F`, dually for `≠`. It
 * is what sees two sides no hook can compare, two `BDD_ID` nodes above all.
 * Anything else leaves the atom as its sides stand: the atom is NEVER
 * reshaped, and `norm_equation` stays the one rewrite of an equation's
 * shape. An order atom is simplified side-wise alone.
 *
 * NOT IDEMPOTENT, like `simplify_term`.
 */
template <NodeType node>
tref simplify_atom(tref a, regime rg = regime::plain);

// --- reading and rewriting equations ---------------------------------------------

/// §3 `TERM_OF(atom, order)`: for an equation `l = r` or `l ≠ r`, seen
/// through one optional `¬`, the term `l + r`. Read off without touching the
/// atom (invariant 4). In the backed regime the ring sum is ALWAYS a BDD
/// operation, even on sides that are leaves alone; in the plain regime it is
/// the `bf_xor` of the two sides, unless a side is BDD-backed. `nullptr` on
/// anything else, which is a caller bug and is Debug-asserted.
template <NodeType node>
tref term_of(tref atom, regime rg = regime::plain);

/// §3 `NORM_EQUATION(atom, order)`: `l = r ↦ l + r = 0`, descending through
/// one `¬`, the ring sum `term_of`'s in the regime @p rg. Anything that is no
/// equation comes back as it is.
template <NodeType node>
tref norm_equation(tref atom, regime rg = regime::plain);

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
