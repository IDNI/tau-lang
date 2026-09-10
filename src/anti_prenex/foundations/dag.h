// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file dag.h
 * @brief Anti-prenexing foundations (layer 0), package A: the formula DAG
 * facets of §1 and §10 — `|φ|`, `FV(φ)`, `h(φ)`, the content order, the
 * member view (D1), the raw canonical chain constructors, the `neg(φ)` slot,
 * and the node classification of §1.
 *
 * Built on the existing hash-consed tree: a `tref` IS structural identity,
 * `hash_lcrs_tref` is the content hash, side tables are ctx.h's tables.
 * Free variables come from the existing `get_free_vars`, wrapped, not
 * reimplemented.
 *
 * Cached facets (`formula_size`, `neg_of`, `is_negative_tree`) live in
 * ctx.h's unconditional structural tables and are filled LAZILY: the
 * accessor computes on the first query and stores; nothing is written at
 * construction. `formula_size` and `is_negative_tree` are the only writers of
 * their tables; `neg_memo` is the exception — nothing here derives it, layer 1
 * fills it through `set_neg`. After the first
 * query a facet is O(1) in every build type (§10: "never recomputed"). A
 * reference into a table — including `fv`'s, which points into
 * `get_free_vars`' table — is invalidated by a `bintree<node>::gc()` sweep,
 * which rebuilds the tables: copy it before any call that can construct
 * nodes.
 *
 * WRAPPER CONVENTION: every predicate and accessor here takes the `wff`
 * WRAPPER node (`tau::get(n).is(tau::wff)`), the shape every `build_wff_*`
 * returns and every `tref` in this module is. The wrapper-form helpers of the
 * codebase are the `is_child_*` family (tau_tree_queries.tmpl.h); the bare
 * `is_quantifier` / `is_temporal_quantifier` test the INNER node and must
 * not be used here.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_H__

#include "fwd.h"

namespace idni::tau_lang::anti_prenexing {

// --- sizes, free variables, hash ---------------------------------------------

/**
 * @brief §1 `|φ|`: the node count — `1 + Σ|children|` over the FORMULA
 * children; an atom counts 1 whatever its terms (plain or `BDD_ID`), so the
 * count is invariant across D2's boundary; the `wff` wrapper and its
 * operator node together count 1; a reference and a temporal operator count
 * 1, opaque like an atom, their bodies not descended into; `|¬ψ| = 1 + |ψ|`;
 * a binder `|Qx.ψ| = 1 + |ψ|`. Every node of a D1 chain's binary spine
 * counts like any other node (D1), so a k-member chain costs
 * `k−1 + Σ|members|` — the deliberate deviation from the n-ary reading of
 * the spec's `|φ|` recorded in D1, which keeps this a plain `1 + Σ|children|`
 * count and stays independent of the nesting direction. A tree count over the
 * hash-consed DAG: a shared subtree counts once per place it appears. The
 * sort convention of 2d, the case witness and EXPAND, and the metric of
 * §5's size acceptance.
 *
 * Not `node_count` (tau_tree_queries.tmpl.h): that counts every LCRS node —
 * wrapper and operator separately, and an atom's whole term subtree with them
 * — and it is `TAU_CACHE`-gated; a different number under a different gating
 * rule (ground rule 9).
 *
 * Lazy: computed on the first query from the children's (cached) sizes and
 * stored in `size_memo`; O(1) after that. The first query on an unmeasured
 * subtree is one `pre_order` walk of it, accumulating in the traversal's
 * post-order `up` callback; every wrapper it passes is measured, not only
 * the queried one.
 */
template <NodeType node>
size_t formula_size(tref n);

/**
 * @brief §1 `FV(φ)`: the cached free-variable set — leaf occurrences (inside
 * `BDD_ID` terms and reference arguments) included, quantified subscripts
 * (formula binders AND functional quantifiers) excluded. Sorted by
 * `subtree_less<node>`, as `get_free_vars` returns it, so `std::binary_search`
 * applies. A thin wrapper over `get_free_vars`; the reference points into
 * its table.
 */
template <NodeType node>
const trefs& fv(tref n);

/// `FV(n) ∩ X ≠ ∅` — an intersection of cached sets, never a walk (§1).
template <NodeType node>
bool fv_meets(tref n, const block& X);

/// `X ∩ FV(n)`, KEEPING X's order (the key narrowing of §6 `PUSH_BLOCK` and
/// §7 `ELIMINATE_BLOCK`, the re-wrap of §5's size acceptance).
template <NodeType node>
block fv_intersect(tref n, const block& X);

/**
 * @brief §1 `h(φ)`: the structural hash, a pure function of CONTENT
 * (phase-0 binder ids, source names for free variables, BDD-canonical
 * terms), identical across runs, components and inputs sharing subtrees.
 * Returns `hash_lcrs_tref<node>{}(n)`: the node's value hash combined with
 * its child list's hash, right sibling EXCLUDED — the functor the tree's own
 * hashed maps use. Neither `node::hash` (the value alone, no children) nor
 * `bintree::hash` (which includes the right sibling chain) is `h(φ)`.
 */
template <NodeType node>
size_t content_hash(tref n);

// --- content order --------------------------------------------------------------

/**
 * @brief §1 CONTENT ORDER: the canonical total order on nodes, the tie-break
 * of last resort everywhere (result-join emission, EXPAND's disjunct key),
 * so no order anywhere depends on construction history. Implemented by
 * `subtree_less<node>`: node value first (hash, then kind and payload), then
 * the child list, right siblings ignored — deterministic and content-derived
 * within a build (it bottoms out in `std::hash`, so it is not stable across
 * toolchains, which no use in the spec needs). Strict weak ordering;
 * `content_less(a, b)` and `content_less(b, a)` both false iff `a` and `b`
 * are the same node.
 */
template <NodeType node>
bool content_less(tref a, tref b);

/// Comparator form of `content_less`, for `std::sort` and ordered containers.
template <NodeType node>
struct content_order {
	bool operator()(tref a, tref b) const { return content_less<node>(a, b); }
};

// --- member view and raw canonical chains (D1) ---------------------------------

/**
 * @brief D1 member view: the members of an ∧-node or ∨-node — the FULL
 * same-connective flattening (associativity; both nesting sides, as
 * `get_leaves` does), in the order they occur, duplicates included. A
 * canonical chain built by `canonical_and`/`canonical_or` or by the joins
 * (layer 1) has its members deduplicated and in content order by
 * construction; an input node not yet rebuilt may hold duplicates and any
 * order. A node that is not a connective has the one-element view `{n}`.
 * NOT cached: one linear walk of the chain per call, returned by value —
 * every consumer processes every member anyway, and the joins' O(1)
 * membership test during assembly is their own local set, not this list.
 */
template <NodeType node>
trefs members(tref n);

/// `m` is a member of the connective node `n` (D1): a linear scan of the
/// member view, O(chain length). A convenience for one-off tests; a step
/// that tests membership repeatedly builds its own set from `members`.
template <NodeType node>
bool is_member(tref n, tref m);

/**
 * @brief D1 RAW constructor of a canonical ∧-chain: members sorted in content
 * order, deduplicated, folded LEFT-NESTED through the BINARY
 * `build_wff_and(l, r)` — not the n-ary overload, whose `_T()` seed only
 * disappears while the construction hooks are on. NO simplification rules —
 * T/F, complement, absorption belong to the joins (layer 1); the hooks (D4)
 * still fold what they fold, so the returned node may have fewer members
 * than the input list, or be a single member, or a constant. Nothing is
 * published: sizes and FV are computed lazily on first query, from the
 * RETURNED node. One member returns it (right sibling trimmed); `members`
 * must not be empty (the empty join is the joins' business) — in Debug an
 * empty list asserts, in Release it returns the neutral constant.
 * Substitution rebuilds through these.
 */
template <NodeType node>
tref canonical_and(trefs members);

/// D1 RAW constructor of a canonical ∨-chain; dual of `canonical_and`.
template <NodeType node>
tref canonical_or(trefs members);

/// The complementary literal of `l`: `¬a` for an atom `a`, `a` for `¬a` (any
/// `¬` is stripped rather than doubled, so the result of two calls is `l`).
/// O(1), one node construction — never `NEG` (layer 1). The joins' unit-
/// elimination test (§3) is a membership test against `complement_of(l)`.
template <NodeType node>
tref complement_of(tref l);

// --- neg(φ) slot ----------------------------------------------------------------

/// §1 `neg(φ)`: the cached NNF of `¬φ`, a pure function of the node computed
/// on first demand by `NEG` (layer 1) and stored in `neg_memo` through
/// `set_neg`. `nullptr` on a miss.
template <NodeType node>
tref neg_of(tref n);

/// Record `neg(n) = negated` (layer 1 writes).
template <NodeType node>
void set_neg(tref n, tref negated);

// --- classification (§1 atom shapes, literals, binders, trees) -------------------

/// `wff` node of the shapes `is_atomic_fm` lists (`f = g`, `f ≤ g`, `f < g`
/// and, before phase 3, the fused negated/mirrored operators).
template <NodeType node> bool is_atom(tref n);
/// `f = g` (post phase 3: never `f ≠ g` — inv. 4).
template <NodeType node> bool is_equation(tref n);
/// `f ≤ g` or `f < g` — never split by the push, consumed only by the
/// bitvector router's solver path (§1). Classifies what the input holds:
/// the module never CONSTRUCTS one, which is what keeps it clear of the
/// order-operator hooks (D4 amendment, ruling of Sep 8 2026).
template <NodeType node> bool is_order_atom(tref n);
/// One `¬` over an atom.
template <NodeType node> bool is_negated_atom(tref n);
/// An atom or a negated atom.
template <NodeType node> bool is_literal(tref n);
/// `¬(f = g)` — post phase 3 "negated equation" names every negative leaf (§1).
template <NodeType node> bool is_negated_equation(tref n);
/// The atom under a literal (the literal itself if not negated).
template <NodeType node> tref atom_of(tref literal);
/// `T` / `F`.
template <NodeType node> bool is_true(tref n);
template <NodeType node> bool is_false(tref n);
/// ∧-node / ∨-node (a D1 chain or an input node).
template <NodeType node> bool is_conjunction(tref n);
template <NodeType node> bool is_disjunction(tref n);
/// A formula binder node, `wff_ex`/`wff_all`. (The spec calls one a UNIT once
/// its run is final, §4 — a property of the pass, not of the node.)
template <NodeType node> bool is_binder(tref n);
/// `wff_ref` — a reference: opaque, connects nothing (inv. 2), frozen by
/// every method (§7).
template <NodeType node> bool is_reference(tref n);
/// A temporal operator (`always`/`sometimes`): opaque to the push (§6 floor).
template <NodeType node> bool is_temporal(tref n);
/**
 * @brief §1 NEGATIVE TREE: an ∨-node all of whose leaves are negated
 * equations — ∧-nodes allowed inside. FINAL for scope narrowing (§6), taken
 * whole by the leaf methods (§7), a disjunctive conjunct for EXPAND.
 * Lazily cached in `negative_tree_memo`.
 */
template <NodeType node> bool is_negative_tree(tref n);
/// §1 FLAT: a negative tree whose members are all literals; NESTED otherwise.
template <NodeType node> bool is_flat_tree(tref n);
/// §1 PLAIN conjunct: not an ∨-node — a literal, a binder, a reference.
template <NodeType node> bool is_plain_conjunct(tref n);
/// §1 DISJUNCTIVE conjunct: an ∨-node, a negative tree included.
template <NodeType node> bool is_disjunctive_conjunct(tref n);

// --- binder accessors -----------------------------------------------------------

/// Kind of a formula binder node (`wff_ex` → ex, `wff_all` → all).
template <NodeType node> binder binder_kind(tref n);
/// The bound variable of a formula binder node (child 0, a `variable`).
template <NodeType node> tref binder_var(tref n);
/// The body of a formula binder node (child 1, a `wff`).
template <NodeType node> tref binder_body(tref n);

} // namespace idni::tau_lang::anti_prenexing

#include "dag.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_H__
