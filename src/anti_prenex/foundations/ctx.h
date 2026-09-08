// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ctx.h
 * @brief Anti-prenexing foundations (layer 0), package E: `ctx` (the §1 ctx
 * table), the memo tables — the six §1 result tables and the structural
 * per-node facet tables — the memo wrapper with the taint rule, and the
 * flush.
 *
 * CACHE GATING (rulings 2026-09-07, fwd.h): every table is an entry of
 * `enum class table` with a `table_traits` specialisation and is a static
 * GC-registered `create_cache` instance. A table with `gated == true` (the
 * six §1 result tables) exists under `#ifdef TAU_CACHE` only — `TAU_CACHE`
 * is OFF in Debug — and no result may depend on a hit; a table with
 * `gated == false` (the structural per-node facets) is UNCONDITIONAL, so
 * its accessors may return references into it. THE `#ifdef` LIVES IN
 * ctx.tmpl.h's `table_ptr` ALONE: consumers reach a table only through
 * `find` / `lookup` / `store` / `memoised` / `flush_solver_dependent`, which
 * see a missing table as a miss, a no-op, or plain computation.
 *
 * §1 CACHE SCOPE, the one rule: a table is GLOBAL — outliving the call,
 * shared across `ANTI_PRENEX` runs, components and blocks — exactly when its
 * key names everything its entries depend on. Three policies cover the knobs
 * that sit in no key: TAINT (a computation that hit a budget returns its
 * result but writes no `push_memo`/`elim_memo` entry; carried by the GLOBAL
 * counter `taint_count`, read by the wrapper before and after), FLUSH
 * (`solver_memo` is valid per solver configuration and is flushed when it
 * changes, `push_memo` and `elim_memo` with it; `qbf_memo` is exempt: its
 * entries are mathematical truths), NEITHER (the acceptance pair).
 *
 * Every function taking a `ctx` takes it by MUTABLE REFERENCE: EXPAND and
 * DECOMPOSE_ARMS write `expand_count`.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_H__

#include <unordered_map>

#include "fwd.h"
#include "options.h"

namespace idni::tau_lang::anti_prenexing {

// --- ctx ------------------------------------------------------------------------

/**
 * @brief §1 `ctx`, the per-component context set up by §5 `PUSH_EX_BLOCK`.
 *
 * The GLOBAL members of the spec's table — `taint_count` and the memo
 * tables — are not stored here: they are reached through `taint_count<node>()`
 * and the table functions below (§1: "the global tables, cross-run, never
 * reset"). Everything stored here is component-scoped.
 */
template <NodeType node>
struct ctx {
	/// §1 `type`: the block's BA type `τ`, a `ba_types<node>` id. Single, by
	/// invariant 2 (components are type-homogeneous). `0` is the codebase's
	/// untyped id and means "not set up".
	size_t type = 0;
	/// §1 `order`: BDD variable order, inner → LOWER rank; §5 `P[i] ↦ |P| - i`.
	var_order<node> order;
	/// §1 `prio`: variable priority, inner → HIGHER rank; §5 `P[i] ↦ i + 1`.
	/// Read by EXPAND's disjunct key, DECOMPOSE_ARMS's pinned-variable choice,
	/// FOLD_DECIDED's probe variable. The two conventions are opposite and
	/// cannot be merged (§1).
	var_order<node> prio;
	/// The six knobs (§1 ctx table), copied from options.h at setup.
	size_t subsume_max   = anti_prenexing::subsume_max;
	size_t qbf_node_max  = anti_prenexing::qbf_node_max;
	size_t case_max      = anti_prenexing::case_max;
	size_t expand_max    = anti_prenexing::expand_max;
	size_t accept_growth = anti_prenexing::accept_growth;
	size_t accept_floor  = anti_prenexing::accept_floor;
	/// §1 `keep_functional`: emit `∀_X`/`∃_X` symbolically instead of
	/// discharging them; in the `push_memo`/`elim_memo` keys (D3).
	bool keep_functional = false;
	/// §1 `expand_count`: cases built by EXPAND so far — the one
	/// COMPONENT-scoped counter, reset to 0 at setup, shared with
	/// DECOMPOSE_ARMS through the one `ctx&` threaded down.
	size_t expand_count = 0;

	/**
	 * @brief §5 setup for one component: `type`, the two rank maps over `P`
	 * (`P[0]` outermost), the knobs read from options.h AT THIS CALL,
	 * `keep_functional`, `expand_count = 0`.
	 */
	static ctx for_component(const block& P, size_t type, bool keep_functional);
};

// --- taint ----------------------------------------------------------------------

/// §1 `taint_count`: budget hits so far, GLOBAL, never reset, never gated.
/// Incremented by every source of taint (EXPAND and DECOMPOSE_ARMS
/// exhaustion, a DECIDE_FINITE sweep abandoned to an ASK that ends
/// `unknown`); read by the memo wrapper, which caches only across an
/// unchanged count. `taint()` is the only writer.
template <NodeType node>
size_t taint_count();

/// The one increment every source of taint calls.
template <NodeType node>
void taint();

// --- the tables -------------------------------------------------------------------

/// Every table of the module: the six §1 result tables (gated caches) and
/// the structural per-node facet tables (unconditional).
enum class table {
	// §1 result tables — gated
	push_memo,     ///< `(REWRAP(φ, X), keep_functional) → formula` (D3); also EXPAND's state memo
	elim_memo,     ///< `(REWRAP(clause, X), keep_functional) → formula` (D3)
	quant_memo,    ///< functional-quantifier term → term; the key IS the query
	cof_memo,      ///< `(settled term, x) → cof_entry`; filled and read by COF
	solver_memo,   ///< canonical closed query → sat / unsat / unknown
	qbf_memo,      ///< canonical closed pure-Boolean query → T / F; never flushed
	// structural per-node facets — unconditional
	atoms_memo,    ///< §1: formula node → the atoms occurring in it, units opaque (subst.h)
	size_memo,     ///< §1 `|φ|` (dag.h `size`)
	members_memo,  ///< D1 member view (dag.h `members`)
	neg_memo,      ///< §1 `neg(φ)` (dag.h `cached_neg`/`cache_neg`; filled by layer 1)
	negative_tree_memo, ///< §1 NEGATIVE TREE flag (dag.h `is_negative_tree`)
	leaf_fv_memo   ///< §1 leaf hazard: FV contributed by a BDD-backed term's leaves (terms.h `leaf_fv`)
};

/// Structural hash of a `(tref, tref)` key, consistent with
/// `subtree_pair_equal<node, tref>` (both ignore right siblings). Never
/// replace with `std::hash<std::pair<…>>`: that hashes a `tref` by pointer
/// and would give equal keys unequal hashes.
template <NodeType node>
struct tref_pair_hash {
	size_t operator()(const std::pair<tref, tref>& k) const {
		const size_t a = hash_lcrs_tref<node>{}(k.first);
		const size_t b = hash_lcrs_tref<node>{}(k.second);
		return a ^ (b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2));
	}
};

/// Structural hash of a `(tref, bool)` key, consistent with
/// `subtree_pair_equal<node, bool>`. Same warning as above.
template <NodeType node>
struct tref_bool_hash {
	size_t operator()(const std::pair<tref, bool>& k) const {
		return hash_lcrs_tref<node>{}(k.first)
			^ (k.second ? size_t(0x9e3779b97f4a7c15ULL) : size_t(0));
	}
};

/**
 * @brief Key, value and map type of each table, and its three policies:
 * `gated` (exists under `TAU_CACHE` only), `taint_aware` (the wrapper writes
 * only across an unchanged `taint_count`), `solver_flushed` (cleared by
 * `flush_solver_dependent`).
 */
template <NodeType node, table T>
struct table_traits;

template <NodeType node>
struct table_traits<node, table::push_memo> {
	using key_t   = std::pair<tref, bool>;
	using value_t = tref;
	using map_t   = std::unordered_map<key_t, value_t, tref_bool_hash<node>,
		subtree_pair_equal<node, bool>>;
	static constexpr bool gated          = true;
	static constexpr bool taint_aware    = true;
	static constexpr bool solver_flushed = true;
};

template <NodeType node>
struct table_traits<node, table::elim_memo> {
	using key_t   = std::pair<tref, bool>;
	using value_t = tref;
	using map_t   = std::unordered_map<key_t, value_t, tref_bool_hash<node>,
		subtree_pair_equal<node, bool>>;
	static constexpr bool gated          = true;
	static constexpr bool taint_aware    = true;
	static constexpr bool solver_flushed = true;
};

template <NodeType node>
struct table_traits<node, table::quant_memo> {
	using key_t   = tref;
	using value_t = tref;
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool gated          = true;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false;
};

template <NodeType node>
struct table_traits<node, table::cof_memo> {
	using key_t   = std::pair<tref, tref>;
	using value_t = cof_entry;
	using map_t   = std::unordered_map<key_t, value_t, tref_pair_hash<node>,
		subtree_pair_equal<node, tref>>;
	static constexpr bool gated          = true;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false;
};

template <NodeType node>
struct table_traits<node, table::solver_memo> {
	using key_t   = tref;
	using value_t = answer;
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool gated          = true;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = true;
};

template <NodeType node>
struct table_traits<node, table::qbf_memo> {
	using key_t   = tref;
	using value_t = bool;
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool gated          = true;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false; ///< exempt from every flush (§1)
};

template <NodeType node>
struct table_traits<node, table::atoms_memo> {
	using key_t   = tref;
	using value_t = tref_set; ///< sorted by `subtree_less<node>`, GC-walked (fwd.h)
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool gated          = false;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false;
};

template <NodeType node>
struct table_traits<node, table::size_memo> {
	using key_t   = tref;
	using value_t = size_t;
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool gated          = false;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false;
};

template <NodeType node>
struct table_traits<node, table::members_memo> {
	using key_t   = tref;
	using value_t = tref_set; ///< D1 member view in occurrence order (dag.h)
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool gated          = false;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false;
};

template <NodeType node>
struct table_traits<node, table::neg_memo> {
	using key_t   = tref;
	using value_t = tref; ///< a bare `tref` value is walked by GC introspection
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool gated          = false;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false;
};

template <NodeType node>
struct table_traits<node, table::negative_tree_memo> {
	using key_t   = tref;
	using value_t = bool;
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool gated          = false;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false;
};

template <NodeType node>
struct table_traits<node, table::leaf_fv_memo> {
	using key_t   = tref;
	using value_t = tref_set; ///< sorted like `fv`
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool gated          = false;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false;
};

/**
 * @brief The table instance, or `nullptr` when it does not exist (a gated
 * table with `TAU_CACHE` off). The ONLY place the module spells
 * `#ifdef TAU_CACHE` is this function's definition in ctx.tmpl.h. An
 * unconditional table is a function-local static `create_cache` reference,
 * as `get_free_vars`' is.
 */
template <NodeType node, table T>
typename table_traits<node, T>::map_t* table_ptr();

// --- reads, writes, the wrapper, the flush -------------------------------------------

/// Pointer to the entry for `key`, or `nullptr` on a miss or a missing
/// table. The reference-returning facet accessors (`members`, `atoms`,
/// `leaf_fv`) are built on this; the pointer is stable until a GC sweep
/// rebuilds the table.
template <table T, NodeType node>
const typename table_traits<node, T>::value_t*
find(const typename table_traits<node, T>::key_t& key);

/// Read table `T`: a copy of the entry for `key`, or nothing.
template <table T, NodeType node>
std::optional<typename table_traits<node, T>::value_t>
lookup(const typename table_traits<node, T>::key_t& key);

/// Write table `T` unconditionally (for entries that are pure functions of
/// the key) and return a reference to the stored value — for a gated table
/// with `TAU_CACHE` off, a reference to a static scratch copy of `value`,
/// valid until the next `store` on that table.
template <table T, NodeType node>
const typename table_traits<node, T>::value_t&
store(const typename table_traits<node, T>::key_t& key,
	typename table_traits<node, T>::value_t value);

/**
 * @brief §1/§6 memo wrapper: return the entry for `key`; on a miss compute
 * `compute()`, and record it — for a `taint_aware` table only if
 * `taint_count` is unchanged across the computation (a hit inside the
 * computation is a hit inside every enclosing one: transitivity for free),
 * for every other table unconditionally. The result is always returned;
 * a tainted result is returned, never cached. Plain `compute()` when the
 * table does not exist — nothing else.
 */
template <table T, NodeType node, typename Compute>
typename table_traits<node, T>::value_t
memoised(const typename table_traits<node, T>::key_t& key, Compute&& compute);

/// §1 FLUSH: the solver configuration changed — clear every `solver_flushed`
/// table (`solver_memo`, `push_memo`, `elim_memo`), whole tables. `qbf_memo`
/// is exempt. Caller: the api layer on a solver option change (layer 4+);
/// until then a test-only entry point.
template <NodeType node>
void flush_solver_dependent();

} // namespace idni::tau_lang::anti_prenexing

#include "ctx.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_H__
