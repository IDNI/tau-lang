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

	/** @brief Build a BDD for Tau formula @p f using variable order @p o. */
	static ref build_bdd(tref f, const order& o);
	/** @brief Build a single-bit BDD for variable @p v. */
	static ref from_bit(tref v);

	/** @brief AND of @p x and a leaf (Tau formula) @p y. */
	static ref bdd_and(ref x, tref y);
	/** @brief AND of two BDD references @p x and @p y under order @p o. */
	static ref bdd_and(ref x, ref y, const order& o);
	/** @brief OR of @p x and @p y under @p o. */
	static ref bdd_or(ref x, ref y, const order& o);
	/** @brief NOT of @p x (flips the output inverter). */
	static ref bdd_not(ref x);

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

	/** @brief Convert BDD reference @p x to a Tau term of type @p term_type. */
	static tref to_tau_term(ref x, size_t term_type);

private:
	static constexpr auto am_cmp = [](ref x, ref y) {
		bool s = x < y;
		x.inv = false, y.inv = false;
		return x < y ? true : x == y ? s : false;
	};
	static size_t bdd_and_many_iter(const refs& v, refs& h, refs& l, ref& res,
		tref& m, const order& o);
	static void am_sort(refs& b);
	static bool am_simplify(refs& v, const std::unordered_map<refs, ref>& memo);
	static bool subset(const refs& small, const refs& big);
	static ref abs(ref x);
#ifdef TAU_CACHE
	static ref bdd_ex(ref x, const trefs& v, size_t i, const order& o, auto& memo);
	static ref bdd_quant(ref x, const quants& v, size_t i, const order& o, auto& memo);
#else
	static ref bdd_ex(ref x, const trefs& v, size_t i, const order& o);
	static ref bdd_quant(ref x, const quants& v, size_t i, const order& o);
#endif
	static ref bdd_compose_impl(ref x, tref xi, ref g, const order& o,
		std::unordered_map<ref, ref>& memo);
	static ref bdd_compose_impl(ref x, const subs_t& subs, size_t i,
		const order& o, std::unordered_map<ref, ref>& memo);

};

/**
 * @brief Reference-counted handle wrapping a `tau_term_bdd` reference.
 *
 * Acts as the user-facing view of a BDD node.  Holds an `htref` (hash-consed
 * tree reference) to the underlying `tau_term_bdd` node plus an output
 * inverter flag.  A global universe map `U` links Tau tree refs to handles,
 * enabling GC of BDD nodes when the originating Tau term is collected.
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

	/** @brief Universe map: Tau tree ref → BDD handle (non-unique). */
	static universe_t& U;

	/** @brief Construct a handle directly from a BDD @p ref. */
	explicit tau_term_bdd_handle(ref x);
	/** @brief Construct from a Tau tree ref @p b and inversion flag. */
	tau_term_bdd_handle(tref b, const bool _inv) : h(tbdd::geth(b)), inv(_inv) {}
	/** @brief Construct from an existing `htref` and inversion flag. */
	tau_term_bdd_handle(htref _h, const bool _inv) : h(std::move(_h)), inv(_inv) {}

	/** @brief Build a BDD handle from Tau formula @p term using variable order @p o. */
	static term_handle build(tref term, const order& o);
	/** @brief Convert a handle back to a Tau tree node with type @p term_type; result cached in @p U. */
	static tref convert_to_tau_node(term_handle handle, size_t term_type);
	/** @brief Build a Tau node from @p term using @p o; result cached in @p U. */
	static tref convert_to_tau_node(tref term, const order& o);
	/** @brief Retrieve the BDD handle for an existing Tau BDD node @p tau_node. */
	static term_handle convert_to_handle(tref tau_node);
	/** @brief Convert this handle to a Tau term of type @p term_type. */
	tref to_tau_term(size_t term_type) const;

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
	/** @brief Existentially quantify @p v from this handle under @p o. */
	term_handle bdd_ex(const trefs& v, const order& o) const;
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

	/** @brief Equality comparison. */
	bool operator==(const tau_term_bdd_handle& other) const;
	/** @brief Inequality comparison. */
	bool operator!=(const tau_term_bdd_handle& other) const;

	htref h;       ///< @brief Underlying hash-consed BDD tree reference.
	bool inv = false; ///< @brief Output inverter flag.

private:
#ifdef TAU_CACHE
	using bdd_fv_cache_t = std::unordered_map<tref, size_t>;
	static void get_free_tau_vars_impl(tref bdd_tref, subtree_set<node>& merged,
		bdd_fv_cache_t& cache);
#else
	static void get_free_tau_vars_impl(tref bdd_tref, subtree_set<node>& merged);
#endif
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

#include "tau_bdd.tmpl.h"

#endif //TAU_TAU_BDD_H
