// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file dag.h
 * @brief Anti-prenexing foundations (layer 0), package A: the formula DAG
 * facets of §1 and §10 — `|φ|`, `FV(φ)`, `h(φ)`, the content order, the
 * member view (D1), the raw canonical chain constructors, the `neg(φ)` slot,
 * and the node classification of §1.
 *
 * Built on the existing hash-consed tree (`tree<node>`, `bintree`): a `tref`
 * IS structural identity, `node::hash` is content-based, side tables are
 * `create_cache` maps (GC-registered). Free variables come from the existing
 * `get_free_vars` (scope-correct, memoised, functional quantifiers treated
 * as binders, `BDD_ID` leaves read); it is wrapped, not reimplemented.
 *
 * Every cached facet below lives in one of ctx.h's UNCONDITIONAL structural
 * tables (`size_memo`, `neg_memo`, `negative_tree_memo`; fwd.h, cache
 * gating), reached through ctx.h's `find`/`store`, so it is O(1) on a
 * second query in every build type: "|φ| and FV(φ) are set at construction,
 * never recomputed" (§10). A reference into such a table (or into
 * `get_free_vars`' table) is invalidated by a `bintree<node>::gc()` sweep,
 * which rebuilds the tables — copy it before any call that can construct
 * nodes. The member view is NOT cached: it is one linear walk of the chain,
 * and every consumer processes every member anyway.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_H__

#include "fwd.h"

namespace idni::tau_lang::anti_prenexing {

// --- sizes, free variables, hash ---------------------------------------------

/**
 * @brief §1 `|φ|`: the cached node count — `1 + Σ|children|` over the FORMULA
 * children; an atom counts 1, its terms are not children; a D1 chain counts
 * as ONE node, `1 + Σ|members|`. A tree count over the hash-consed DAG, so a
 * shared subtree counts once per place it appears. The sort convention of
 * 2d, the case witness and EXPAND, and the metric of §5's size acceptance.
 *
 * Populated by the module's constructors at construction and, for nodes
 * built elsewhere (parsed input), by one memoised post-order fill at first
 * query.
 */
template <NodeType node>
size_t size(tref n);

/**
 * @brief §1 `FV(φ)`: the cached free-variable set — leaf occurrences (inside
 * `BDD_ID` terms and reference arguments) included, quantified subscripts
 * (formula binders AND functional quantifiers) excluded. Sorted by
 * `subtree_less<node>`, as `get_free_vars` returns it, so `std::binary_search`
 * applies.
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
 * @brief §1 `h(φ)`: the node's structural hash, set at construction from the
 * node's kind and its children's hashes. A pure function of CONTENT: every
 * input is canonical (phase-0 binder ids, source names for free variables,
 * BDD-canonical terms), so it is identical across runs, components and
 * inputs sharing subtrees. Returns `hash_lcrs_tref<node>{}(n)`: the node's
 * value hash combined with its child list's hash, right sibling EXCLUDED —
 * the functor the tree's own hashed maps use. Neither `node::hash` (the
 * value alone, no children) nor `bintree::hash` (which includes the right
 * sibling chain) is `h(φ)`.
 */
template <NodeType node>
size_t content_hash(tref n);

// --- content order --------------------------------------------------------------

/**
 * @brief §1 CONTENT ORDER: the canonical total order on nodes — by `h`, ties
 * broken by structural comparison (kind by a fixed rank, children left to
 * right, variables by identifier). A pure function of content: the tie-break
 * of last resort everywhere (result-join emission, EXPAND's disjunct key), so
 * no order anywhere depends on construction history. Strict weak ordering;
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
 * NOT cached: one linear walk of the chain per call, returned by value.
 * Every consumer processes every member anyway, so the walk is not an
 * extra cost; the joins' O(1) membership test during assembly is their own
 * local set, not this list.
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
 * order, deduplicated, folded RIGHT-NESTED, `|·|` and `FV` published. NO
 * simplification rules — T/F, complement, absorption belong to the joins
 * (layer 1); the construction hooks (D4) still fold what they fold. One
 * member returns it; `members` must not be empty (the empty join is the
 * joins' business). Substitution rebuilds through these.
 */
template <NodeType node>
tref canonical_and(trefs members);

/// D1 RAW constructor of a canonical ∨-chain; dual of `canonical_and`.
template <NodeType node>
tref canonical_or(trefs members);

// --- neg(φ) slot ----------------------------------------------------------------

/// §1 `neg(φ)`: the cached NNF of `¬φ`, a pure function of the node computed
/// on first demand by `NEG` (layer 1) and cached here. `nullptr` on a miss.
template <NodeType node>
tref cached_neg(tref n);

/// Record `neg(n) = negated` (layer 1 writes; cache-gated, fwd.h).
template <NodeType node>
void cache_neg(tref n, tref negated);

// --- classification (§1 atom shapes, literals, units, trees) ---------------------

/// `wff` node of the shapes `is_atomic_fm` lists (`f = g`, `f ≤ g`, `f < g`
/// and, before phase 3, the fused negated/mirrored operators).
template <NodeType node> bool is_atom(tref n);
/// `f = g` (post phase 3: never `f ≠ g` — inv. 4).
template <NodeType node> bool is_equation(tref n);
/// `f ≤ g` or `f < g` — never split by the push, consumed only by the
/// bitvector router's solver path (§1).
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
/// A formula binder node, `wff_ex`/`wff_all` — a UNIT once its run is final (§4).
template <NodeType node> bool is_unit(tref n);
/// `wff_ref` — a reference: opaque, connects nothing (inv. 2), frozen by
/// every method (§7).
template <NodeType node> bool is_reference(tref n);
/// A temporal operator (`always`/`sometimes`): opaque to the push (§6 floor).
template <NodeType node> bool is_temporal(tref n);
/**
 * @brief §1 NEGATIVE TREE: an ∨-node all of whose leaves are negated
 * equations — ∧-nodes allowed inside. FINAL for scope narrowing (§6), taken
 * whole by the leaf methods (§7), a disjunctive conjunct for EXPAND.
 * Memoised (cache-gated).
 */
template <NodeType node> bool is_negative_tree(tref n);
/// §1 FLAT: a negative tree whose members are all literals; NESTED otherwise.
template <NodeType node> bool is_flat_tree(tref n);
/// §1 PLAIN conjunct: not an ∨-node — a literal, a unit, a reference.
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
