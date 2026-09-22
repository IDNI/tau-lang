// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file dag.h
 * @brief Anti-prenexing foundations: the formula-DAG facets of §1 and §10
 * that the tree library does not already provide — the formula size `|φ|`,
 * the free-variable intersections, the member view of a connective chain,
 * the raw canonical chain constructors, the `neg(φ)` slot, the literal and
 * tree shapes of §1, and the binder accessors.
 *
 * A §1 term that IS one library call is not re-declared here. The VOCABULARY
 * block below lists those terms and says which call each one is.
 *
 * Everything is a facet of the existing hash-consed tree: a `tref` IS
 * structural identity, `hash_lcrs_tref` is the content hash, the side tables
 * are ctx.h's tables, and free variables are `get_free_vars`.
 *
 * The three cached facets — `formula_size`, `neg_of`, `is_negative_tree` —
 * live in ctx.h's unconditional structural tables and are filled LAZILY: the
 * accessor computes on the first query and stores the result, and nothing is
 * written at construction time. `formula_size` and `is_negative_tree` are the
 * only writers of their tables; `neg_memo` is written by nnf.h through
 * `set_neg`. After the first query a facet is O(1) in every build type (§10:
 * "never recomputed").
 *
 * GC HAZARD: a reference into a table — `get_free_vars`' included — is
 * invalidated by a `bintree<node>::gc()` sweep, which rebuilds the tables.
 * Copy such a reference before any call that can construct nodes.
 *
 * WRAPPER CONVENTION: every predicate and accessor here, and every library
 * call the VOCABULARY block names, takes the `wff` WRAPPER node
 * (`tau::get(n).is(tau::wff)`) — the shape every `build_wff_*` returns and
 * every `tref` in this module is. The wrapper-form helpers of the codebase
 * are the `is_child_*` family (tau_tree_queries.tmpl.h); the bare
 * `is_quantifier` and `is_temporal_quantifier` test the INNER node and must
 * not be used here.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_H__

#include "fwd.h"

namespace idni::tau_lang::anti_prenexing {

// --- §1 vocabulary with a library counterpart -----------------------------------
//
// Not re-declared here — used directly, on a wrapper node `n`:
//
//   FV(φ)                  get_free_vars<node>(n)
//   h(φ)                   hash_lcrs_tref<node>{}(n)
//   content order          tree<node>::subtree_less(a, b); the functor
//                          subtree_less<node>{} for std::sort and containers
//   atom                   is_atomic_fm<node>(n)
//   equation f = g         is_child<node>(n, tau::bf_eq)
//   ∧-node / ∨-node        is_child<node>(n, tau::wff_and) / (n, tau::wff_or)
//   binder Qx.ψ            is_child_quantifier<node>(n)
//   reference              is_child<node>(n, tau::wff_ref)
//   temporal operator      is_child_temporal_quantifier<node>(n)
//   T / F                  tau::get(n).equals_T() / .equals_F()
//   PLAIN conjunct         !is_child<node>(n, tau::wff_or)
//   DISJUNCTIVE conjunct   is_child<node>(n, tau::wff_or)
//
// FV(φ) (§1), the free variables of a formula: leaf occurrences included —
// those inside `BDD_ID` terms and inside reference arguments — and quantified
// subscripts excluded, both those of formula binders and those of functional
// quantifiers. The result is sorted by `subtree_less<node>`, so
// `std::binary_search` applies. It is a reference into `get_free_vars`' table
// and is subject to the GC hazard above.
//
// h(φ) (§1), the structural hash: a pure function of CONTENT — the binder ids
// phase 0 assigns, the source names of free variables, the BDD-canonical
// terms — hence identical across runs, across components and across inputs
// that share subtrees. It is the node's value hash combined with its child
// list's hash, with the right sibling EXCLUDED, the functor the tree's own
// hashed maps use. Two neighbours are NOT `h(φ)`: `node::hash` is the value
// alone without children, and `bintree::hash` includes the right sibling
// chain.
//
// CONTENT ORDER (§1): the canonical total order on nodes, and the tie-break
// of last resort everywhere (result-join emission, `EXPAND`'s disjunct key),
// so that no order anywhere depends on construction history. It compares the
// node value first (hash, then kind and payload), then the child list, and
// ignores right siblings. It is deterministic and content-derived. It is a
// strict weak ordering:
// `subtree_less(a, b)` and `subtree_less(b, a)` are both false iff `a` and
// `b` are the same node.
//
// The remaining entries, in words. ATOM: the shapes `is_atomic_fm` lists —
// `f = g`, `f ≤ g`, `f < g` and, before phase 3, the fused negated and
// mirrored operators. EQUATION: after phase 3 never `f ≠ g` (invariant 4).
// PLAIN conjunct: a literal, a binder or a reference. DISJUNCTIVE conjunct:
// an ∨-node, a negative tree included.

// --- size and free-variable intersections ---------------------------------------

/**
 * @brief §1 `|φ|`: the node count of a formula, `1 + Σ|children|` over the
 * FORMULA children.
 *
 * What counts as 1: an atom, whatever its terms are (plain or `BDD_ID`), so
 * the count does not change when terms move into their BDD representation; a
 * `wff` wrapper together with its operator node; a reference and a temporal
 * operator, both opaque like an atom, their bodies never entered. For the
 * recursive shapes, `|¬ψ| = 1 + |ψ|` and `|Qx.ψ| = 1 + |ψ|`. A connective
 * chain is a BINARY spine whose every node counts, so a chain of `k` members
 * costs `k−1 + Σ|members|` whatever its nesting direction. It is a TREE count
 * over a hash-consed DAG: a shared subtree counts once per place it appears.
 *
 * Not `node_count` (tau_tree_queries.tmpl.h), which counts every LCRS node —
 * wrapper and operator separately, and an atom's whole term subtree — and is
 * `TAU_CACHE`-gated.
 *
 * Lazy, and O(1) once computed: stored in `size_memo`. The first query on an
 * unmeasured subtree measures every wrapper below it in one walk.
 */
template <NodeType node>
size_t formula_size(tref n);

/// `FV(n) ∩ X ≠ ∅` — an intersection of cached sets, never a walk (§1).
template <NodeType node>
bool fv_meets(tref n, const block& X);

/// `X ∩ FV(n)`, KEEPING X's order: how a block is narrowed to what a node
/// still holds.
template <NodeType node>
block fv_intersect(tref n, const block& X);

// --- member view and raw canonical chains ---------------------------------------

/**
 * @brief MEMBER VIEW of an ∧-node or an ∨-node: its members under the FULL
 * same-connective flattening — associativity applied in both nesting
 * directions, as `get_leaves` does — in the order they occur, duplicates
 * included. A node that is neither connective has the one-element view `{n}`.
 *
 * A chain built by `canonical_and` / `canonical_or` or by the joins (joins.h)
 * has deduplicated members in content order by construction; an input node
 * the module has not rebuilt may hold duplicates in any order.
 *
 * NOT cached: one linear walk of the chain per call, returned by value.
 */
template <NodeType node>
trefs members(tref n);

/// `m` is a member of the connective node `n`: a linear scan of the member
/// view, O(chain length). A convenience for one-off tests; a step that tests
/// membership repeatedly builds its own set from `members`.
template <NodeType node>
bool is_member(tref n, tref m);

/**
 * @brief RAW constructor of a canonical ∧-chain: the members sorted in
 * content order, deduplicated, then folded LEFT-NESTED through the BINARY
 * `build_wff_and(l, r)`. Not the n-ary overload, whose `_T()` seed only
 * disappears while the construction hooks are on.
 *
 * NO simplification rules: T/F, complement and absorption belong to the joins
 * (joins.h). The construction hooks still fold what they fold, so the result
 * may have fewer members than the input list, or be a single member, or a
 * constant.
 *
 * Nothing is published: `|·|` and `FV` are computed lazily on the first
 * query, from the RETURNED node. A one-element list returns that member with
 * its right sibling trimmed. `members` must not be empty — the empty join is
 * the joins' business: Debug asserts, Release returns the neutral constant.
 */
template <NodeType node>
tref canonical_and(trefs members);

/// RAW constructor of a canonical ∨-chain; dual of `canonical_and`.
template <NodeType node>
tref canonical_or(trefs members);

/// The complementary literal of `l`: `¬a` for an atom `a`, and `a` for `¬a`.
/// A `¬` is stripped rather than doubled, so applying this twice gives `l`
/// back. O(1), one node construction; not `NEG` (nnf.h), which normalises.
template <NodeType node>
tref complement_of(tref l);

// --- neg(φ) slot ----------------------------------------------------------------

/// §1 `neg(φ)`: the cached NNF of `¬φ`. It is a pure function of the node,
/// computed on first demand by `canonically_factored_neg` (nnf.h) and stored
/// in `neg_memo` through `set_neg`. `nullptr` on a miss.
template <NodeType node>
tref neg_of(tref n);

/// Record `neg(n) = negated`; nnf.h is the writer.
template <NodeType node>
void set_neg(tref n, tref negated);

// --- classification (§1 literals and trees) -------------------------------------

/// `f ≤ g` or `f < g`. The module classifies order atoms and never builds
/// one, which keeps it clear of the order-operator construction hooks.
template <NodeType node> bool is_order_atom(tref n);
/// One `¬` over an atom.
template <NodeType node> bool is_negated_atom(tref n);
/// An atom or a negated atom. NOT the same as `is_atomic_fm`: after phase 3
/// a negative leaf is `¬(f = g)`, a `wff_neg` node (invariant 4), which
/// `is_atomic_fm` rejects.
template <NodeType node> bool is_literal(tref n);
/// `¬(f = g)`. After phase 3 a "negated equation" is every negative leaf (§1).
template <NodeType node> bool is_negated_equation(tref n);
/// The atom under a literal (the literal itself if it is not negated).
template <NodeType node> tref atom_of(tref literal);
/**
 * @brief §1 NEGATIVE TREE: an ∨-node all of whose leaves are negated
 * equations; ∧-nodes are allowed inside. Lazily cached in
 * `negative_tree_memo`.
 */
template <NodeType node> bool is_negative_tree(tref n);
/// §1 FLAT: a negative tree whose members are all literals; NESTED otherwise.
template <NodeType node> bool is_flat_tree(tref n);

// --- binder accessors -----------------------------------------------------------

/// Kind of a formula binder node (`wff_ex` → ex, `wff_all` → all).
template <NodeType node> quantifier<node> binder_kind(tref n);
/// The bound variable of a formula binder node (child 0, a `variable`).
template <NodeType node> tref binder_var(tref n);
/// The body of a formula binder node (child 1, a `wff`).
template <NodeType node> tref binder_body(tref n);

} // namespace idni::tau_lang::anti_prenexing

#include "dag.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_H__
