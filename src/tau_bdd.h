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

#include <span>

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
 * two child tree references (`high` / `low`) inside a `bintree`. Every
 * `variable` met at a Boolean position is a decision variable, everything
 * else is a leaf, and ONE global comparator (`less_then`) orders the decision
 * variables for every caller and every call.
 *
 * @tparam node Tree node type.
 */
template <NodeType node>
struct tau_term_bdd : bintree<tau_bdd_node<node>> {
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
	using cache_and_many_index_t = std::unordered_map<ref,
		std::vector<const typename cache_and_many_t::value_type*>>;
	using cache_ex_t       = std::map<trefs, std::unordered_map<ref, ref>>;
	using cache_ite_t      = std::unordered_map<std::array<ref, 3>, ref>;

	/// @brief Memoisation table for AND. Its definition also hooks the
	/// operation tables into the tree's collection (`clear_caches`).
	static cache_and_t&     and_memo;
	static cache_and_many_t and_many_memo;  ///< @brief Memoisation table for AND-many.
	/// @brief Entries of `and_many_memo` by their first (am_cmp-least) argument;
	/// they point into the memo and are cleared with it.
	static cache_and_many_index_t and_many_index;
	static cache_ex_t       ex_memo;        ///< @brief Memoisation table for existential quantification.
	static cache_ite_t      ite_memo;       ///< @brief Memoisation table for if-then-else.

	/**
	 * @brief Clear the operation tables above. Every collection of the tree
	 * runs it: the tables are caches, and the BDD node store, which is never
	 * swept, is left alone.
	 */
	static void clear_caches();
#endif

	/** @brief Canonicalise the pair @p x, @p y so @p x ≤ @p y. */
	static void make_canonical(ref& x, ref& y);
	/**
	 * @brief THE GLOBAL ORDER on `variable` nodes, @p x above @p y: the free
	 * variables first, by name; then the binder variables (a decimal name,
	 * the ids canonicalisation mints) by id, highest (outermost) first; ties
	 * by content order, the BA type included. A strict total order.
	 */
	static bool less_then(tref x, tref y);

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
	 * @brief `true` iff @p x is a reduced ordered BDD: every decision
	 * variable is a `variable` node and `less_then` strictly increases down
	 * every path. Linear in the BDD.
	 */
	static bool is_ordered(ref x);

	/**
	 * @brief Build the BDD of the Tau term @p f: every `variable` reached
	 * through `∪ · ′ +` is a decision variable; references, foreign-typed
	 * and arithmetic subterms and constants are leaves.
	 *
	 * A functional-quantifier chain `Q_Y b` goes through the constructor and
	 * then SLIDES down its body to its subscripts' levels: the decision
	 * nodes above them stay, and each PIECE, the sub-BDD from the first
	 * subscript node or leaf on a path, is decided as the constructor
	 * decides a chain: dropped when it holds no subscript, quantified on
	 * its BDD when closed, else ONE LEAF, the chain over its term. Sound for
	 * any order, since `Q_Y` commutes with cofactoring on a variable outside
	 * `Y`; with canonical ids the subscripts are the body's lowest levels,
	 * so a piece holds them alone.
	 */
	static ref build_bdd(tref f);
	/** @brief Build a single-bit BDD for the `variable` node @p v. */
	static ref from_bit(tref v);

	/**
	 * @brief The canonical functional-quantifier chain over @p body (a `bf`
	 * term) with the prefix @p q, OUTERMOST FIRST: a nest of one-variable
	 * `bf_fall` / `bf_fex` binders, the only shape the builders and the
	 * parser allow.
	 *
	 * Canonical, so permutations inside one quantifier block give one node:
	 * a subscript not free in @p body is dropped (the term-level
	 * FOLD_DEGENERATE_BINDERS) and a repeated one kept at its INNERMOST
	 * occurrence, which is the one that binds; each maximal run of one kind
	 * is sorted into content order (`subtree_less`); and a @p body that is
	 * itself a chain whose outermost run has the kind of the new innermost
	 * one joins that run, so the same chain assembled in two steps is one
	 * node.
	 *
	 * FOLD: a chain binding every free variable of @p body, plain or
	 * BDD-backed, is a CONSTANT — the body's BDD quantified by the prefix
	 * with `bdd_quant`, as its constant term (`_1` / `_0` of the body's type
	 * for a terminal) — unless a subscript hides in a leaf (the §1 LEAF
	 * HAZARD), which no quantification reaches.
	 *
	 * An empty @p q, a constant @p body, and a chain left with no subscript
	 * all give @p body itself.
	 *
	 * @p body_bdd, when the caller holds it, is @p body's BDD, which the
	 * fold then quantifies instead of building @p body again.
	 */
	static tref build_functional_quantifiers(const quants& q, tref body,
		const ref* body_bdd = nullptr);

	/**
	 * @brief AND of the leaves @p leaves as one leaf, built once: their
	 * `bf_and` members sorted into content order and deduplicated; `F` when
	 * a member meets its complement, `T` when no member is left. Typed BA
	 * constant members are evaluated, `{c}′` as `{~c}` and all of them into
	 * one constant: `F` when it is 0, dropped when it is 1.
	 */
	static ref bdd_and_leaves(std::span<const ref> leaves);
	/**
	 * @brief AND of @p x and a leaf (Tau formula) @p y; `bdd_and_leaves`
	 * of the two at a leaf @p x.
	 */
	static ref bdd_and(ref x, tref y);
	/** @brief AND of two BDD references @p x and @p y. */
	static ref bdd_and(ref x, ref y);
	/** @brief OR of @p x and @p y. */
	static ref bdd_or(ref x, ref y);
	/** @brief NOT of @p x (flips the output inverter). */
	static ref bdd_not(ref x);
	/** @brief XOR of @p x and @p y. */
	static ref bdd_xor(ref x, ref y);

	/** @brief Existentially quantify the variables @p v from @p x; @p v comes back sorted by `less_then`. */
	static ref bdd_ex(ref x, trefs& v);
	/** @brief Universally quantify the variables @p v from @p x. */
	static ref bdd_all(ref x, trefs v);
	/**
	 * @brief Apply the mixed prefix @p v, OUTERMOST FIRST, to @p x: one
	 * maximal same-kind run at a time, innermost run first, each run one
	 * `bdd_ex` / `bdd_all`.
	 */
	static ref bdd_quant(ref x, const quants& v);

	/** @brief AND of all references in @p v. */
	static ref bdd_and_many(refs v);
	/** @brief OR of all references in @p v. */
	static ref bdd_or_many(refs v);

	/** @brief If-then-else of three BDD references. */
	static ref bdd_ite(ref f, ref g, ref h);

	/** @brief Replace variable @p xi with @p g in @p x. */
	static ref bdd_compose(ref x, tref xi, ref g);
	/** @brief Apply multiple simultaneous variable substitutions @p subs. */
	static ref bdd_compose(ref x, subs_t subs);
	/**
	 * @brief Cofactor `x[xi ← bit]` by CHILD SELECTION, at any depth: the
	 * node of @p xi is its child by @p bit, everything else rebuilt as it
	 * is; @p x itself when @p xi is no decision variable of it. Cheaper than
	 * `bdd_compose(x, xi, T/F)`, which routes every node through `bdd_ite`;
	 * that entry point delegates here for a terminal @p g. @p xi is taken as
	 * the caller spells it, untrimmed (as in `bdd_compose`).
	 */
	static ref bdd_cofactor(ref x, tref xi, bool bit);
	/**
	 * @brief Rebuild @p x with every non-terminal leaf `ℓ` replaced by
	 * `fn(ℓ)` (both as `bf` terms), the decision structure kept.
	 *
	 * A changed leaf is built again through `build_bdd`, so it is spelled
	 * as the constructor spells it (products through the leaf product,
	 * chains through the slide), leaves that became
	 * equal are one node and `h == l` folds, and a leaf that came to hold
	 * a decision variable becomes a BDD. A decision node is rebuilt
	 * through `add` while both children stay ordered below it, and through
	 * `bdd_ite` once a rebuilt leaf brought a variable of equal or higher
	 * rank up, which re-canonicalises.
	 * Memoised per call over the BDD's nodes.
	 */
	template <typename Fn>
	static ref map_leaves(ref x, Fn& fn);

	/**
	 * @brief THE SPELLING of @p x as a Tau term of type @p term_type: per
	 * decision node on `v`, the OR form `v ∪ f₀` where `f₁ = 1` and
	 * `v′ ∪ f₁` where `f₀ = 1`, else the ring sum `v + f₀` where the
	 * cofactors are complements (`f₁ = f₀′`), else `v·f₁ ∪ v′·f₀`; a leaf is
	 * its term, `T` / `F` are `_1` / `_0` of the type. Memoised per
	 * reference, so a shared node is spelled once; the construction hooks
	 * fold constants.
	 */
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
	// Memoised worker for bdd_and_many(refs): always memoizes the
	// recursion within one top-level call, same pattern as
	// bdd_and/bdd_ite/bdd_ex's workers above.
	static ref bdd_and_many(refs v, std::unordered_map<refs, ref>& memo);
	/**
	 * @brief One Shannon-expansion step of the and-many recursion.
	 *
	 * Picks @p m, the `less_then`-least decision variable among the
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
		tref& m, std::unordered_map<refs, ref>& memo);
	/**
	 * @brief Canonicalise an and-many worklist: sort by am_cmp, drop T,
	 * dedupe, AND adjacent leaves together, and collapse the whole list
	 * to {F} when it contains F or a complementary pair.
	 */
	static void am_sort(refs& b);
#ifdef TAU_CACHE
	/**
	 * @brief Rewrite @p v using one `and_many_memo` hit: if some memoised
	 * argument set is a subset of @p v, replace that subset by its
	 * memoised result ({F} collapses the list). Returns true iff @p v
	 * changed; callers iterate it to a fixpoint.
	 */
	static bool am_simplify(refs& v);
#endif
	/**
	 * @brief Memoise @p r as the result for the am_sort-ed argument list
	 * @p v (at least two arguments) in @p memo, which is `and_many_memo`
	 * under TAU_CACHE; there the entry also joins `and_many_index`.
	 * Returns the memoised result.
	 */
	static ref am_memoize(std::unordered_map<refs, ref>& memo, const refs& v,
		ref r);
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
	static ref bdd_and(ref x, ref y,
		std::unordered_map<std::array<ref, 2>, ref>& memo);
	static ref bdd_ite(ref f, ref g, ref h,
		std::unordered_map<std::array<ref, 3>, ref>& memo);
	static ref bdd_ex(ref x, const trefs& v, size_t i, auto& memo);
	static ref bdd_compose_impl(ref x, tref xi, ref g,
		std::unordered_map<ref, ref>& memo);
	static ref bdd_compose_impl(ref x, const subs_t& subs, size_t i,
		std::unordered_map<ref, ref>& memo);
	static ref bdd_cofactor_impl(ref x, tref xi, bool bit,
		std::unordered_map<ref, ref>& memo);
	/**
	 * @brief The slide of `build_bdd`'s chain case over the body's BDD @p x:
	 * the chain's canonical prefix @p q, its subscripts @p ys sorted by
	 * `subtree_less`, and the body's BA type @p type for the spelling of a
	 * piece that stays a chain, which @p spelled memoises across the
	 * pieces. @p memo is keyed per ref, inverters included: a chain does
	 * not commute with complement.
	 */
	static ref slide(ref x, const quants& q, const trefs& ys, size_t type,
		std::unordered_map<ref, ref>& memo,
		std::unordered_map<ref, tref>& spelled);
	// Workers of the walks above, carrying the per-call visited set / memo:
	// recursive over BDD refs, the shape of bdd_compose_impl.
	template <typename Fn>
	static bool visit_nodes(ref x, Fn& fn, std::unordered_set<tref>& seen);
	static bool is_ordered(ref x, std::unordered_set<tref>& seen);
	template <typename Fn>
	static ref map_leaves(ref x, Fn& fn, std::unordered_map<ref, ref>& memo);
	/// What `less_then` compares before content order: a binder variable
	/// (decimal name) carries its id, a free one the dictionary id of its
	/// name.
	struct sort_key {
		bool binder = false;
		std::uint64_t id = 0;
		size_t name = 0;
	};
	/// The sort key of the `variable` node @p v, kept per node under
	/// TAU_CACHE, since `less_then` runs at every apply step.
	static sort_key sort_key_of(tref v);
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
 * inverter flag.  A global universe map `U` links the id the `BDD_ID` node
 * of a converted Tau term carries to its handle, enabling GC of BDD nodes
 * when the originating Tau term is collected.
 *
 * @tparam node Tree node type.
 */
template <NodeType node>
struct tau_term_bdd_handle {
	using ref          = tau_bdd_ref<node>;                  ///< @brief BDD reference type.
	using refs         = std::vector<ref>;                   ///< @brief Vector of BDD references.
	using term_handle  = tau_term_bdd_handle;                ///< @brief Self alias.
	using term_handles = std::vector<term_handle>;           ///< @brief Vector of handles.
	using tbdd         = tau_term_bdd<node>;                 ///< @brief Underlying BDD type.
	using quants       = std::vector<std::pair<tref, typename tbdd::Quantifier>>; ///< @brief Quantifier list.
	using universe_t   = std::unordered_map<size_t, term_handle>; ///< @brief Universe type.
	/** @brief Interning key of a converted node: the handle and the BA type
	 * the `BDD_ID` node was minted with (the type is part of the node). */
	using intern_key_t = std::pair<term_handle, size_t>;
	/** @brief An interned conversion: the `BDD_ID` node and the id it carries. */
	struct interned_t { tref id_node; size_t id; };
	using intern_t     = std::unordered_map<intern_key_t, interned_t>; ///< @brief Interning map type.

	/**
	 * @brief Universe map: the id a `BDD_ID` node carries → its BDD handle.
	 * Filled before the node is built, as the node's hash and order read its
	 * BDD here; `I`'s sweep drops an entry with its node, and no id recurs.
	 */
	static universe_t& U;
	/**
	 * @brief Interning map: (handle, type) → the `BDD_ID` node minted for it.
	 * An entry dies with its node in a `bintree<node>::gc()`, so a later
	 * conversion of the same BDD mints afresh; interning holds among live nodes.
	 */
	static intern_t& I;

	/** @brief Construct a handle directly from a BDD @p ref. */
	explicit tau_term_bdd_handle(ref x);
	/** @brief Construct from a Tau tree ref @p b and inversion flag. */
	tau_term_bdd_handle(tref b, const bool _inv) : h(tbdd::geth(b)), inv(_inv) {}
	/** @brief Construct from an existing `htref` and inversion flag. */
	tau_term_bdd_handle(htref _h, const bool _inv) : h(std::move(_h)), inv(_inv) {}

	/** @brief Build a BDD handle from Tau formula @p term. */
	static term_handle build(tref term);
	/**
	 * @brief The `bf(BDD_ID)` term of @p handle with type @p term_type,
	 * without a right sibling: the `bf` wrapper, re-derived through the
	 * hash-consed typed constructor, of the `BDD_ID` node already minted
	 * for this (handle, type) in @p I, else of a fresh one recorded in
	 * @p U and @p I. Two calls with equal handles return the same node.
	 */
	static tref convert_to_tau_node(term_handle handle, size_t term_type);
	/** @brief Build a BDD from @p term and return its interned `bf(BDD_ID)` term. */
	static tref convert_to_tau_node(tref term);
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
	 * @brief The store key of a BDD-backed term: the id its `BDD_ID` node
	 * carries (see `U`). Accepts the `bf(BDD_ID)` wrapper in any spelling,
	 * or the `BDD_ID` node itself.
	 */
	static size_t key_of(tref tau_node);
	/** @brief Convert this handle to a Tau term of type @p term_type. */
	tref to_tau_term(size_t term_type) const;
	/**
	 * @brief The formula-wide inverse of `convert_to_tau_node`: every
	 * `bf(BDD_ID)` node of @p formula, wherever it sits — under a binder,
	 * inside a reference argument, in a functional quantifier's body, under
	 * a temporal operator — replaced by its BDD's spelling (`to_tau_term`).
	 *
	 * TOTAL: a `BDD_ID` nested inside a produced term (a leaf may hold one,
	 * and `build_bdd` absorbs it again) is converted too, so no `BDD_ID`
	 * remains anywhere in the result.
	 */
	static tref convert_to_tau_terms(tref formula);

	/** @brief AND with @p other. */
	term_handle bdd_and(term_handle other) const;
	/** @brief OR with @p other. */
	term_handle bdd_or(term_handle other) const;
	/** @brief NOT (flips `inv`). */
	term_handle bdd_not() const;
	/** @brief AND of all @p bdds. */
	static term_handle bdd_and_many(const term_handles& bdds);
	/** @brief OR of all @p bdds. */
	static term_handle bdd_or_many(const term_handles& bdds);
	/** @brief Existentially quantify @p v from this handle.
	 * @p v comes back sorted, as the static `tbdd::bdd_ex` leaves it. */
	term_handle bdd_ex(trefs& v) const;
	/** @brief Universally quantify @p v from this handle. */
	term_handle bdd_all(const trefs& v) const;
	/** @brief Apply the quantifier prefix @p q, outermost first, to this handle. */
	term_handle bdd_quant(const quants& q) const;
	/** @brief If-then-else of this (condition), @p g (then), @p h (else). */
	term_handle bdd_ite(term_handle g, term_handle h) const;

	/** @brief Substitute variable @p xi with @p g. */
	term_handle bdd_compose(tref xi, term_handle g) const;
	/** @brief Apply simultaneous substitutions @p subs. */
	term_handle bdd_compose(const std::vector<std::pair<tref, term_handle>>& subs) const;

	/** @brief Return the underlying BDD reference. */
	ref get() const;

	/** @brief Return the free Tau variables referenced by BDD node @p bdd_tref. */
	static const trefs& get_free_tau_vars(tref bdd_tref);
	/**
	 * @brief The free Tau variables of the BDD nodes under @p bdd_tref --
	 * of the leaves alone if @p leaves_only -- sorted by `subtree_less` and
	 * deduplicated.
	 *
	 * The body of `get_free_tau_vars` and `get_free_leaf_vars`, which differ
	 * only in that flag and in whether they cache the answer.
	 */
	static trefs collect_free_tau_vars(tref bdd_tref, bool leaves_only);
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
