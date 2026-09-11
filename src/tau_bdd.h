/**
 * @file tau_bdd.h
 * @brief Tau term BDD (Binary Decision Diagram) with output-inverter support.
 *
 * Provides `tau_bdd_node`, `tau_bdd_ref`, `tau_term_bdd`, and
 * `tau_term_bdd_handle` — a BDD data structure whose variables are Tau tree
 * references rather than integer ids.  Supports output inverters to halve
 * node counts and caching (enabled with `TAU_CACHE`).
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef TAU_TAU_BDD_H
#define TAU_TAU_BDD_H

#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @brief BDD node keyed by a Tau tree reference with per-edge inversion flags.
 *
 * Stores the variable (@p v) and three output-inverter bits: one for the
 * node itself (`inv_v`) and one each for the high (`inv_h`) and low (`inv_l`)
 * child edges.
 *
 * @tparam node Tree node type.
 */
template <NodeType node>
struct tau_bdd_node {
	tref v = nullptr;     ///< Variable (Tau tree reference).
	bool inv_v : 1 = false; ///< Invert the variable polarity.
	bool inv_h : 1 = false; ///< Invert the high child.
	bool inv_l : 1 = false; ///< Invert the low child.

	/** @brief Construct a non-inverted leaf node for @p _v. */
	explicit tau_bdd_node(tref _v) : v(_v) {}
	/** @brief Construct with all inversion flags explicit. */
	tau_bdd_node(tref _v, bool _inv_v, bool _inv_h, bool _inv_l)
		: v(_v), inv_v(_inv_v), inv_h(_inv_h), inv_l(_inv_l) {}

	/** @brief Equality comparison. */
	bool operator==(const tau_bdd_node& other) const;
	/** @brief Inequality comparison. */
	bool operator!=(const tau_bdd_node& other) const;
	/** @brief Ordering (for use in maps/sets). */
	bool operator<(const tau_bdd_node& other) const;
};

/**
 * @brief Reference into the Tau BDD universe with an optional output inverter.
 *
 * Pairs a `tref` (the BDD node in the hash-cons store) with an `inv` bit.
 * Setting `inv = true` is equivalent to prepending a NOT gate on the output.
 *
 * @tparam node Tree node type.
 */
template <NodeType node>
struct tau_bdd_ref {
	tref b = nullptr; ///< BDD node reference.
	bool inv = false; ///< Output-inversion flag.

	/** @brief Default-construct a null reference. */
	tau_bdd_ref() {}
	/** @brief Construct from node @p _b with inversion @p _inv. */
	tau_bdd_ref(tref _b, bool _inv) : b(_b), inv(_inv) {}

	/** @brief Equality comparison. */
	bool operator==(const tau_bdd_ref& other) const;
	/** @brief Inequality comparison. */
	bool operator!=(const tau_bdd_ref& other) const;
	/** @brief Ordering (for use in maps/sets). */
	bool operator<(const tau_bdd_ref& other) const;
};

/**
 * @brief BDD whose variables are Tau tree references.
 *
 * Each node stores a `tau_bdd_node` (variable + three inversion bits) and
 * two child tree references (`high` / `low`) inside a `bintree`.  The variable
 * ordering is explicit: each `tref` variable maps to an integer rank via an
 * `order` map; smaller rank = higher in the diagram.
 *
 * @tparam node Tree node type.
 */
template <NodeType node>
struct tau_term_bdd : bintree<tau_bdd_node<node>> {
	/** @brief Variable ordering: each Tau-tree variable → integer rank. */
	using order = subtree_unordered_map<node, int_t>;
	using ref = tau_bdd_ref<node>;              ///< @brief BDD reference (node + inversion).
	using refs = std::vector<ref>;              ///< @brief Vector of BDD references.
	using bdd_node = tau_bdd_node<node>;        ///< @brief BDD node type.

	/** @brief Construct from a BDD node and high/low child tree references. */
	tau_term_bdd(bdd_node v, tref h, tref l) : bintree<bdd_node>(v, h, l) {}

	enum Quantifier {ex, all}; ///< @brief Quantifier kinds for `bdd_quant`.
	static ref T, F; ///< @brief Canonical true and false references.

	/**
	 * @brief Insert every Tau tree reference held by the BDD universe
	 * into @p keep.
	 *
	 * Each interned `tau_bdd_node` stores its variable as a raw Tau
	 * `tref` (`v`), and this BDD store is never swept by
	 * `bintree<node>::gc()` — so those variables (and, transitively,
	 * their subtrees, e.g. the cached free-vars vectors) must be pinned
	 * whenever the Tau tree is collected.
	 * @param keep Set of Tau tree nodes to preserve across gc.
	 */
	static void collect_live_refs(std::unordered_set<tref>& keep);

	using quants  = std::vector<std::pair<tref, Quantifier>>; ///< @brief Quantifier list.
	using subs_t  = std::vector<std::pair<tref, ref>>;        ///< @brief Substitution list.

#ifdef TAU_CACHE
	using cache_and_t      = std::unordered_map<std::array<ref, 2>, ref>;
	using cache_and_many_t = std::unordered_map<refs, ref>;
	using cache_ex_t       = std::map<trefs, std::unordered_map<ref, ref>>;
	using cache_quant_t    = std::map<quants, std::unordered_map<ref, ref>>;
	using cache_ite_t      = std::unordered_map<std::array<ref, 3>, ref>;

	static cache_and_t      and_memo;       ///< @brief Memoisation table for AND.
	static cache_and_many_t and_many_memo;  ///< @brief Memoisation table for AND-many.
	static cache_ex_t       ex_memo;        ///< @brief Memoisation table for existential quantification.
	static cache_quant_t    quant_memo;     ///< @brief Memoisation table for general quantification.
	static cache_ite_t      ite_memo;       ///< @brief Memoisation table for if-then-else.

	/** @brief Clear all memoisation caches. */
	static void clear_caches();

	// The memo tables are keyed by BDD refs alone, not by order, so a
	// cached entry is only valid under the order it was computed with.
	// Every public entry point calls sync_order_cache() first, which
	// clears all five tables when @p o differs from last_order.
	static order last_order;      ///< @brief Order the memo caches were last populated under.
	static bool  has_last_order;  ///< @brief Whether `last_order` holds a valid previous order.

	/** @brief Clear all memoisation caches if @p o differs from the order last seen at a public entry point. */
	static void sync_order_cache(const order& o);
#endif

	/** @brief Canonicalise the pair @p x, @p y so @p x ≤ @p y. */
	static void make_canonical(ref& x, ref& y);
	/** @brief Return `true` if variable @p x has lower rank than @p y under @p o. */
	static bool less_then(tref x, tref y, const order& o);

	/** @brief Canonically insert a non-leaf BDD node into the universe. */
	static ref add(tref v, ref h, ref l);
	/** @brief Canonically insert a leaf BDD node into the universe. */
	static ref add(tref leaf);
	/** @brief Retrieve the BDD node for reference @p x. */
	static tau_term_bdd get_node(ref x);
	/** @brief Return the variable of reference @p x. */
	static tref get_var(ref x);
	/** @brief Return the high child of reference @p x. */
	static ref get_high(ref x);
	/** @brief Return the low child of reference @p x. */
	static ref get_low(ref x);
	/** @brief Return `true` if @p l is a leaf reference. */
	static bool leaf(ref l);
	/**
	 * @brief The variable of reference @p x as a `bf` term: the node's
	 * variable under a `bf` wrapper; for a leaf that is its Tau term, with
	 * the output inverter folded in as `bf_neg` (see `get_var`).
	 */
	static tref get_var_term(ref x);

	/**
	 * @brief Visit every NON-terminal node of @p x once, BY NODE: a node
	 * reached under both inverters is visited once, `T` and `F` never.
	 *
	 * @param fn Called as `fn(ref, bool is_leaf)`; returning `false` stops
	 * the walk.
	 * @return `false` iff @p fn stopped the walk.
	 */
	template <typename Fn>
	static bool visit_nodes(ref x, Fn& fn);
	/**
	 * @brief The number of distinct non-terminal nodes reachable from @p x,
	 * leaves included; zero for a terminal, and a node shared through an
	 * inverter counted once.
	 */
	static size_t node_count(ref x);
	/**
	 * @brief `true` iff @p x is a reduced ordered BDD under @p o: every
	 * decision variable is a key of @p o and ranks strictly increase down
	 * every path. Catches a mixed-order operand and a ref built under
	 * another order. Linear in the BDD.
	 */
	static bool is_ordered(ref x, const order& o);

	/** @brief Build a BDD for Tau formula @p f using variable order @p o. */
	static ref build_bdd(tref f, const order& o);
	/** @brief Build a single-bit BDD for variable @p v. */
	static ref from_bit(tref v);
	/**
	 * @brief `true` iff some free variable of the Tau term @p term is a key
	 * of @p o, i.e. @p term would have a decision variable under it. Always
	 * `false` for an empty order.
	 */
	static bool has_bdd_var(tref term, const order& o);

	/** @brief AND of @p x and a leaf (Tau formula) @p y. */
	static ref bdd_and(ref x, tref y);
	/** @brief AND of two BDD references @p x and @p y under order @p o. */
	static ref bdd_and(ref x, ref y, const order& o);
	/** @brief OR of @p x and @p y under @p o. */
	static ref bdd_or(ref x, ref y, const order& o);
	/** @brief NOT of @p x (flips the output inverter). */
	static ref bdd_not(ref x);
	/** @brief XOR of @p x and @p y under @p o. */
	static ref bdd_xor(ref x, ref y, const order& o);

	/** @brief Existentially quantify variables @p v from @p x under @p o. */
	static ref bdd_ex(ref x, trefs& v, const order& o);
	/** @brief Universally quantify variables @p v from @p x under @p o. */
	static ref bdd_all(ref x, trefs v, const order& o);
	/** @brief Apply a sequence of mixed quantifiers @p v to @p x under @p o. */
	static ref bdd_quant(ref x, const quants& v, const order& o);

	/** @brief AND of all references in @p v under @p o. */
	static ref bdd_and_many(refs v, const order& o);
	/** @brief OR of all references in @p v under @p o. */
	static ref bdd_or_many(refs v, const order& o);

	/** @brief If-then-else of three BDD references under @p o. */
	static ref bdd_ite(ref f, ref g, ref h, const order& o);

	/** @brief Replace variable @p xi with @p g in @p x. */
	static ref bdd_compose(ref x, tref xi, ref g, const order& o);
	/** @brief Apply multiple simultaneous variable substitutions @p subs. */
	static ref bdd_compose(ref x, subs_t subs, const order& o);
	/**
	 * @brief Cofactor `x[xi ← bit]` by CHILD SELECTION, at any depth: the
	 * node of @p xi is its child by @p bit, everything else rebuilt as it
	 * is. @p x itself when @p xi is not a key of @p o. Cheaper than
	 * `bdd_compose(x, xi, T/F, o)`, which routes every node through
	 * `bdd_ite`; that entry point delegates here for a terminal @p g.
	 * @p xi is taken as the caller spells it, untrimmed (as in
	 * `bdd_compose`).
	 */
	static ref bdd_cofactor(ref x, tref xi, bool bit, const order& o);
	/**
	 * @brief Rebuild @p x with every non-terminal leaf `ℓ` replaced by
	 * `fn(ℓ)` (both as `bf` terms), the decision structure kept.
	 *
	 * A changed leaf with no decision variable of @p o is re-interned
	 * through `add`, so leaves that became equal are one node and `h == l`
	 * folds; a changed leaf that came to hold one is built as a BDD. A
	 * decision node is rebuilt through `add` while both children stay
	 * ordered below it, and through `bdd_ite` once a rebuilt leaf brought a
	 * variable of equal or higher rank up, which re-canonicalises.
	 * Memoised per call over the BDD's nodes.
	 */
	template <typename Fn>
	static ref map_leaves(ref x, Fn& fn, const order& o);

	/** @brief Convert BDD reference @p x to a Tau term of type @p term_type. */
	static tref to_tau_term(ref x, size_t term_type);

private:
	/**
	 * @brief Ordering for and-many worklists: compares with the inverter
	 * flag stripped, so a reference and its complement sort adjacently
	 * (letting am_sort spot x AND x' = F); ties on the underlying node
	 * fall back to the full comparison including the flag.
	 */
	static constexpr auto am_cmp = [](ref x, ref y) {
		bool s = x < y;
		x.inv = false, y.inv = false;
		return x < y ? true : x == y ? s : false;
	};
	// Memoised worker for bdd_and_many(refs, const order&): always
	// memoizes the recursion within one top-level call, same pattern as
	// bdd_and/bdd_ite/bdd_ex/bdd_quant's workers above.
	static ref bdd_and_many(refs v, const order& o,
		std::unordered_map<refs, ref>& memo);
	/**
	 * @brief One Shannon-expansion step of the and-many recursion.
	 *
	 * Picks @p m, the order-smallest decision variable among the
	 * non-leaf conjuncts of @p v, and cofactors every conjunct on it
	 * into the high worklist @p h and low worklist @p l (both am_sort
	 * canonicalised; conjuncts common to both are ANDed once via the
	 * memoised recursion and re-inserted).
	 *
	 * @return Code telling the caller how to finish:
	 * - 0: recurse on both @p h and @p l, result is add(m, high, low);
	 * - 1: done, the final result is already in @p res (F on a
	 *   contradiction, or the AND of the leaf values when no conjunct
	 *   has a decision variable);
	 * - 2: the low cofactor collapsed to F, recurse on @p h only,
	 *   result is add(m, high, F);
	 * - 3: the high cofactor collapsed to F, recurse on @p l only,
	 *   result is add(m, F, low).
	 */
	static size_t bdd_and_many_iter(const refs& v, refs& h, refs& l, ref& res,
		tref& m, const order& o, std::unordered_map<refs, ref>& memo);
	/**
	 * @brief Canonicalise an and-many worklist: sort by am_cmp, drop T,
	 * dedupe, AND adjacent leaves together, and collapse the whole list
	 * to {F} when it contains F or a complementary pair.
	 */
	static void am_sort(refs& b);
	/**
	 * @brief Rewrite @p v using one @p memo hit: if some memoised
	 * argument set is a subset of @p v, replace that subset by its
	 * memoised result ({F} collapses the list). Returns true iff @p v
	 * changed; callers iterate it to a fixpoint.
	 */
	static bool am_simplify(refs& v, const std::unordered_map<refs, ref>& memo);
	/**
	 * @brief True iff every element of @p small occurs in @p big; both
	 * must be non-empty and am_sort-ed (binary search plus range/size
	 * quick rejects rely on that).
	 */
	static bool subset(const refs& small, const refs& big);
	/** @brief Copy of @p x with the output-inverter flag cleared. */
	static ref abs(ref x);
	// Memoised recursive workers, independent of TAU_CACHE: the public
	// entry points thread through a static map when TAU_CACHE is on, or
	// a fresh local one otherwise.
	static ref bdd_and(ref x, ref y, const order& o,
		std::unordered_map<std::array<ref, 2>, ref>& memo);
	static ref bdd_ite(ref f, ref g, ref h, const order& o,
		std::unordered_map<std::array<ref, 3>, ref>& memo);
	static ref bdd_ex(ref x, const trefs& v, size_t i, const order& o, auto& memo);
	static ref bdd_quant(ref x, const quants& v, size_t i, const order& o, auto& memo);
	static ref bdd_compose_impl(ref x, tref xi, ref g, const order& o,
		std::unordered_map<ref, ref>& memo);
	static ref bdd_compose_impl(ref x, const subs_t& subs, size_t i,
		const order& o, std::unordered_map<ref, ref>& memo);
	static ref bdd_cofactor_impl(ref x, tref xi, bool bit, const order& o,
		std::unordered_map<ref, ref>& memo);
	// Workers of the walks above, carrying the per-call visited set / memo:
	// recursive over BDD refs, the shape of bdd_compose_impl.
	template <typename Fn>
	static bool visit_nodes(ref x, Fn& fn, std::unordered_set<tref>& seen);
	static bool is_ordered(ref x, const order& o,
		std::unordered_set<tref>& seen);
	template <typename Fn>
	static ref map_leaves(ref x, Fn& fn, const order& o,
		std::unordered_map<ref, ref>& memo);
	// Memoised worker for to_tau_term(ref, size_t): shared BDD nodes are
	// rebuilt once per top-level call instead of once per path.
	static tref to_tau_term(ref x, size_t term_type,
		std::unordered_map<ref, tref>& memo);

};

/**
 * @brief Reference-counted handle wrapping a `tau_term_bdd` reference.
 *
 * Acts as the user-facing view of a BDD node.  Holds an `htref` (hash-consed
 * tree reference) to the underlying `tau_term_bdd` node plus an output
 * inverter flag.  A global universe map `U` links the `BDD_ID` node of a
 * converted Tau term to its handle, enabling GC of BDD nodes when the
 * originating Tau term is collected.
 *
 * @tparam node Tree node type.
 */
template <NodeType node>
struct tau_term_bdd_handle {
	using order        = subtree_unordered_map<node, int_t>; ///< @brief Variable ordering map.
	using ref          = tau_bdd_ref<node>;                  ///< @brief BDD reference type.
	using refs         = std::vector<ref>;                   ///< @brief Vector of BDD references.
	using term_handle  = tau_term_bdd_handle;                ///< @brief Self alias.
	using term_handles = std::vector<term_handle>;           ///< @brief Vector of handles.
	using tbdd         = tau_term_bdd<node>;                 ///< @brief Underlying BDD type.
	using quants       = std::vector<std::pair<tref, typename tbdd::Quantifier>>; ///< @brief Quantifier list.
	using universe_t   = std::unordered_map<tref, term_handle>; ///< @brief Universe type.
	/** @brief Interning key of a converted node: the handle and the BA type
	 * the `BDD_ID` node was minted with (the type is part of the node). */
	using intern_key_t = std::pair<term_handle, size_t>;
	using intern_t     = std::unordered_map<intern_key_t, tref>; ///< @brief Interning map type.

	/**
	 * @brief Universe map: the `BDD_ID` node of a converted term → its BDD
	 * handle. A bijection with `I`: `convert_to_tau_node` interns, so one
	 * BDD (per type) has exactly one `BDD_ID` node, and equal BDDs give the
	 * same hash-consed Tau node.
	 *
	 * KEY: the inner `BDD_ID` node, never the `bf` wrapper around it. In
	 * the LCRS tree the wrapper has one spelling per right sibling (alone,
	 * or followed by the other side inside an equation) and the tree's gc
	 * marks by pointer, so a key on the wrapper would die with the one
	 * spelling nothing holds while the term lives on in another; the
	 * `BDD_ID` child is the same node in every spelling. `key_of` takes a
	 * term to its key.
	 */
	static universe_t& U;
	/**
	 * @brief Interning map: (handle, type) → the `BDD_ID` node minted for
	 * it, the inverse of `U` (the same node as `U`'s key). Swept with the
	 * tree: an entry whose node did not survive a `bintree<node>::gc()` is
	 * dropped, as `U`'s is, so a node nothing references is collectable
	 * and a later conversion of the same BDD mints afresh. Interning holds
	 * among live nodes, which is all hash-consed identity needs.
	 */
	static intern_t& I;

	/** @brief Construct a handle directly from a BDD @p ref. */
	explicit tau_term_bdd_handle(ref x);
	/** @brief Construct from a Tau tree ref @p b and inversion flag. */
	tau_term_bdd_handle(tref b, const bool _inv) : h(tbdd::geth(b)), inv(_inv) {}
	/** @brief Construct from an existing `htref` and inversion flag. */
	tau_term_bdd_handle(htref _h, const bool _inv) : h(std::move(_h)), inv(_inv) {}

	/** @brief Build a BDD handle from Tau formula @p term using variable order @p o. */
	static term_handle build(tref term, const order& o);
	/**
	 * @brief The `bf(BDD_ID)` term of @p handle with type @p term_type,
	 * without a right sibling: the `bf` wrapper, re-derived through the
	 * hash-consed typed constructor, of the `BDD_ID` node already minted
	 * for this (handle, type) in @p I, else of a fresh one recorded in
	 * @p U and @p I. Two calls with equal handles return the same node.
	 */
	static tref convert_to_tau_node(term_handle handle, size_t term_type);
	/** @brief Build a BDD from @p term using @p o and return its interned `bf(BDD_ID)` term. */
	static tref convert_to_tau_node(tref term, const order& o);
	/**
	 * @brief The term of @p handle with type @p term_type, minting a
	 * `BDD_ID` only for a BDD that actually BRANCHES: a BDD with no
	 * decision node (a terminal or a single leaf) comes back as its plain
	 * term through `to_tau_term` — `_1`/`_0` of the type for a terminal.
	 */
	static tref convert_to_tau_node_or_term(term_handle handle,
		size_t term_type);
	/**
	 * @brief Retrieve the BDD handle behind a BDD-backed term @p tau_node:
	 * a `bf(BDD_ID)` wrapper in any spelling, or the `BDD_ID` node itself.
	 */
	static term_handle convert_to_handle(tref tau_node);
	/**
	 * @brief @p term is a BDD-backed term: the `bf(BDD_ID)` wrapper, the
	 * form `convert_to_tau_node` returns (the bare `BDD_ID` node is not
	 * one). `false` for `nullptr`.
	 */
	static bool is_bdd_backed(tref term);
	/**
	 * @brief The store key of a BDD-backed term: its `BDD_ID` node, the
	 * same tref whatever the right sibling of the `bf` wrapper (see `U`).
	 * Accepts the `bf(BDD_ID)` wrapper in any spelling, or the `BDD_ID`
	 * node itself.
	 */
	static tref key_of(tref tau_node);
	/** @brief Convert this handle to a Tau term of type @p term_type. */
	tref to_tau_term(size_t term_type) const;
	/**
	 * @brief The formula-wide inverse of `convert_to_tau_node`: every
	 * `bf(BDD_ID)` node of @p formula, wherever it sits — under a binder,
	 * inside a reference argument, in a functional quantifier's body, under
	 * a temporal operator — replaced by the plain term of its BDD.
	 *
	 * TOTAL: a `BDD_ID` nested inside a produced term (a leaf may hold one,
	 * and `build_bdd` absorbs it again) is converted too, so no `BDD_ID`
	 * remains anywhere in the result.
	 */
	static tref convert_to_tau_terms(tref formula);

	/** @brief AND with @p other under @p o. */
	term_handle bdd_and(term_handle other, const order& o) const;
	/** @brief OR with @p other under @p o. */
	term_handle bdd_or(term_handle other, const order& o) const;
	/** @brief NOT (flips `inv`). */
	term_handle bdd_not() const;
	/** @brief AND of all @p bdds under @p o. */
	static term_handle bdd_and_many(const term_handles& bdds, const order& o);
	/** @brief OR of all @p bdds under @p o. */
	static term_handle bdd_or_many(const term_handles& bdds, const order& o);
	/** @brief Existentially quantify @p v from this handle under @p o.
	 * @p v comes back sorted by @p o, as the static `tbdd::bdd_ex` leaves it. */
	term_handle bdd_ex(trefs& v, const order& o) const;
	/** @brief Universally quantify @p v from this handle under @p o. */
	term_handle bdd_all(const trefs& v, const order& o) const;
	/** @brief Apply quantifier sequence @p q from this handle under @p o. */
	term_handle bdd_quant(const quants& q, const order& o) const;
	/** @brief If-then-else of this (condition), @p g (then), @p h (else) under @p o. */
	term_handle bdd_ite(term_handle g, term_handle h, const order& o) const;

	/** @brief Substitute variable @p xi with @p g under @p o. */
	term_handle bdd_compose(tref xi, term_handle g, const order& o) const;
	/** @brief Apply simultaneous substitutions @p subs under @p o. */
	term_handle bdd_compose(const std::vector<std::pair<tref, term_handle>>& subs, const order& o) const;

	/** @brief Substitute @p var with @p with in formula @p formula under @p o. */
	static tref substitute(tref formula, tref var, term_handle with, const order& o);

	/** @brief Return the underlying BDD reference. */
	ref get() const;

	/** @brief Return the free Tau variables referenced by BDD node @p bdd_tref. */
	static const trefs& get_free_tau_vars(tref bdd_tref);
	/**
	 * @brief The free Tau variables contributed by the LEAVES alone of the
	 * BDD rooted at @p bdd_tref (the decision variables left out), sorted
	 * as its sibling `get_free_tau_vars` sorts.
	 *
	 * BY VALUE, and not cached, where the sibling returns a cached
	 * reference: the per-leaf sets are `get_free_vars`' own cached entries,
	 * so their union is one linear walk over the BDD's distinct nodes per
	 * call.
	 */
	static trefs get_free_leaf_vars(tref bdd_tref);

	/** @brief Equality comparison. */
	bool operator==(const tau_term_bdd_handle& other) const;
	/** @brief Inequality comparison. */
	bool operator!=(const tau_term_bdd_handle& other) const;

	htref h;       ///< @brief Underlying hash-consed BDD tree reference.
	bool inv = false; ///< @brief Output inverter flag.

private:
	using bdd_fv_cache_t = std::unordered_map<tref, trefs>;
};

template<NodeType node>
using term_handle = tau_term_bdd_handle<node>;

} // namespace idni::tau_lang

/// @brief `std::hash` specialisation for `tau_bdd_node<T>`.
template<typename T>
struct std::hash<idni::tau_lang::tau_bdd_node<T>> {
	size_t operator()(auto& n) const;
};

/// @brief `std::hash` specialisation for `tau_bdd_ref<T>`.
template<typename T>
struct std::hash<idni::tau_lang::tau_bdd_ref<T>> {
	size_t operator()(auto& r) const;
};

/// @brief `std::hash` specialisation for a pair of `tau_bdd_ref<T>` (AND cache key).
template<typename T>
struct std::hash<std::array<idni::tau_lang::tau_bdd_ref<T>, 2>> {
	size_t operator()(auto& a) const;
};

/// @brief `std::hash` specialisation for a triple of `tau_bdd_ref<T>` (ITE cache key).
template<typename T>
struct std::hash<std::array<idni::tau_lang::tau_bdd_ref<T>, 3>> {
	size_t operator()(auto& a) const;
};

/// @brief `std::hash` specialisation for `term_handle<T>`.
template<typename T>
struct std::hash<idni::tau_lang::term_handle<T>> {
	size_t operator()(auto& th) const;
};

/// @brief `std::hash` specialisation for the interning key (handle, type).
template<typename T>
struct std::hash<std::pair<idni::tau_lang::term_handle<T>, size_t>> {
	size_t operator()(auto& k) const;
};

#include "tau_bdd.tmpl.h"

#endif //TAU_TAU_BDD_H
