// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ctx.h
 * @brief Anti-prenexing foundations (layer 0), package E: `ctx` (the §1 ctx
 * table), the memo tables — the six §1 result tables and the structural
 * per-node facet tables — the memo wrapper with its taint rule (§1 cache
 * scope, §6 `PUSH_BLOCK`), and the flush.
 *
 * Every table is an entry of `enum class table` with a `table_traits`
 * specialisation and a static GC-registered `create_cache` instance. A
 * `gated` table — one of the six §1 result tables — exists under
 * `#ifdef TAU_CACHE` only, which is off in Debug, and no result may depend on
 * a hit. An unconditional table — a structural facet — always exists, so its
 * accessors may return references: `store` and `memoised` return a reference
 * for an unconditional table and a value for a gated one, split by a
 * `requires` clause.
 *
 * THE `#ifdef` LIVES IN ctx.tmpl.h's `table_ptr` ALONE. Consumers reach a
 * table only through `find`, `lookup`, `store`, `memoised` and
 * `flush_solver_dependent`, each of which sees a missing table as a miss, a
 * no-op, or plain computation.
 *
 * Taint and flush are the §1 policies for knobs that sit in no key. TAINT: a
 * computation that hit a budget returns its result but writes no entry.
 * FLUSH: when the solver configuration changes, every table whose entries
 * could embed a solver verdict is cleared. `qbf_memo` is exempt from every
 * flush.
 *
 * Every function taking a `ctx` takes it by MUTABLE REFERENCE, because
 * `EXPAND` and `DECOMPOSE_ARMS` write `expand_count`. Single-threaded, like
 * every knob and cache in the library.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_H__

#include <array>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <utility>

#include "fwd.h"
#include "options.h"

/// The reference-returning `store` and `memoised` hand back a reference into
/// a table, never to an argument. GCC's `-Wdangling-reference` heuristic,
/// however, flags any call that returns a reference and takes a CLASS-TYPE
/// temporary, however that temporary is passed. The `atoms` fill is exactly
/// that shape (`store(n, tref_set{…})`), so without this attribute such a
/// call would be an error under `-Werror`. Measured on GCC 16: the scalar
/// facets (`size_memo`, `negative_tree_memo`) never trip it, the `tref_set`
/// one always does. The attribute (GCC 14+) states what the heuristic cannot
/// see; elsewhere it expands to nothing.
#if defined(__has_cpp_attribute) && __has_cpp_attribute(gnu::no_dangling)
#	define TAU_ANTI_PRENEX_NO_DANGLING [[gnu::no_dangling]]
#else
#	define TAU_ANTI_PRENEX_NO_DANGLING
#endif

namespace idni::tau_lang::anti_prenexing {

// --- ctx ------------------------------------------------------------------------

/**
 * @brief §1 `ctx`, the per-component context set up by §5 `PUSH_EX_BLOCK`.
 *
 * The GLOBAL members of the spec's table — `taint_count` and the memo tables
 * — are not stored here. They are reached through `taint_count<node>()` and
 * the table functions below (§1: "the global tables, cross-run, never
 * reset"). Everything stored here is component-scoped.
 */
template <NodeType node>
struct ctx {
	/// §1 `type`: the block's Boolean-algebra type `τ`. A single type, by
	/// invariant 2: components are type-homogeneous. `0`, the untyped id, means
	/// "not set up".
	ba_type_id type = 0;
	/// §1 `order`: BDD variable order, inner → LOWER rank; §5 `P[i] ↦ |P| - i`.
	var_order<node> order;
	/// §1 `prio`: variable priority, inner → HIGHER rank; §5 `P[i] ↦ i + 1`.
	/// Read by `EXPAND`'s disjunct key, `DECOMPOSE_ARMS`'s pinned-variable
	/// choice, and `FOLD_DECIDED`'s probe variable. The two conventions are
	/// opposite and cannot be merged (§1).
	var_order<node> prio;
	/// The six knobs (§1 ctx table), copied from options.h at setup.
	size_t subsume_max   = anti_prenexing::subsume_max;
	size_t qbf_node_max  = anti_prenexing::qbf_node_max;
	size_t case_max      = anti_prenexing::case_max;
	size_t expand_max    = anti_prenexing::expand_max;
	size_t accept_growth = anti_prenexing::accept_growth;
	size_t accept_floor  = anti_prenexing::accept_floor;
	/// §1 `keep_functional`: emit `∀_X`/`∃_X` symbolically instead of
	/// discharging them. Part of the `push_memo` / `elim_memo` keys.
	bool keep_functional = false;
	/// §1 `expand_count`: the cases `EXPAND` has built so far — the one
	/// COMPONENT-scoped counter, reset to 0 at setup and shared with
	/// `DECOMPOSE_ARMS` through the single `ctx&` threaded down.
	size_t expand_count = 0;

	/**
	 * @brief §5 setup for one component: `type`, the two rank maps over `P`
	 * (`P[0]` outermost), the knobs read from options.h AT THIS CALL,
	 * `keep_functional`, and `expand_count = 0`.
	 */
	static ctx for_component(const block& P, ba_type_id type, bool keep_functional);
};

// --- taint ----------------------------------------------------------------------

/// §1 `taint_count`: the number of budget hits so far. GLOBAL, never reset,
/// never gated. It is incremented by every source of taint (`EXPAND` and
/// `DECOMPOSE_ARMS` exhaustion, a `DECIDE_FINITE` sweep abandoned to an `ASK`
/// that ends `unknown`) and read by the memo wrapper, which caches only
/// across an unchanged count. `taint()` is the only writer.
template <NodeType node>
size_t taint_count();

/// The one increment every source of taint calls. The counter is per `node`
/// type, so a taint source in code templated on the BDD node type (the
/// `qbf_node_max` sweep of `DECIDE_FINITE`) must name the FORMULA node type
/// here, the one whose tables the wrapper guards.
template <NodeType node>
void taint();

// --- the tables -------------------------------------------------------------------

/// Every table of the module: the six §1 result tables (gated caches) and the
/// structural per-node facet tables (unconditional).
enum class table {
	// §1 result tables — gated
	push_memo,     ///< `(REWRAP(φ, X), keep_functional) → formula`; also EXPAND's state memo
	elim_memo,     ///< `(REWRAP(clause, X), keep_functional) → formula`
	quant_memo,    ///< functional-quantifier term → term; the key IS the query
	cof_memo,      ///< `(settled term, x) → cof_entry`; filled and read by COF
	solver_memo,   ///< canonical closed query → sat / unsat / unknown
	qbf_memo,      ///< canonical closed pure-Boolean query → T / F; never flushed
	// structural per-node facets — unconditional
	atoms_memo,    ///< §1: formula node → the atoms occurring in it, units opaque (subst.h)
	size_memo,     ///< §1 `|φ|` (dag.h `formula_size`, its only writer)
	neg_memo,      ///< §1 `neg(φ)` (dag.h `neg_of`/`set_neg`; filled by layer 1)
	negative_tree_memo, ///< §1 NEGATIVE TREE flag (dag.h `is_negative_tree`)
	count_         ///< NOT a table: the enumerator count, so `all_tables` sizes itself
};

/// Every table, in enum order. `flush_solver_dependent` walks this list and
/// acts on the `solver_flushed` flag rather than on a hand-written set, so a
/// new table cannot be forgotten there. The array is sized by `count_` and
/// the assertion below checks it position by position, so a table added
/// ANYWHERE in the enum — appended included — and not listed here is a
/// compile error rather than a table the flush silently skips.
inline constexpr std::array<table, static_cast<size_t>(table::count_)>
all_tables = {
	table::push_memo, table::elim_memo, table::quant_memo, table::cof_memo,
	table::solver_memo, table::qbf_memo, table::atoms_memo,
	table::size_memo, table::neg_memo, table::negative_tree_memo
};
static_assert([] {
		for (size_t i = 0; i < all_tables.size(); ++i)
			if (static_cast<size_t>(all_tables[i]) != i) return false;
		return true;
	}(),
	"`all_tables` must list every table exactly once, in enum order");

/// Structural hash of a `(tref, tref)` key, consistent with
/// `subtree_pair_equal<node, tref>`; both ignore right siblings. The two
/// CONTENT hashes are mixed by `idni::hash_combine`. Never hand a `tref`
/// itself to `hash_combine` or to `std::hash<std::pair<…>>`: those route it
/// through `portable_hash`, i.e. hash it by POINTER, so equal keys would get
/// unequal hashes and the entry would be lost.
template <NodeType node>
struct tref_pair_hash {
	size_t operator()(const std::pair<tref, tref>& k) const {
		std::uint64_t seed = hash_lcrs_tref<node>{}(k.first);
		idni::hash_combine(seed, hash_lcrs_tref<node>{}(k.second));
		return static_cast<size_t>(seed);
	}
};

/// Structural hash of a `(tref, bool)` key, consistent with
/// `subtree_pair_equal<node, bool>`. Same warning as above.
template <NodeType node>
struct tref_bool_hash {
	size_t operator()(const std::pair<tref, bool>& k) const {
		std::uint64_t seed = hash_lcrs_tref<node>{}(k.first);
		idni::hash_combine(seed, k.second);
		return static_cast<size_t>(seed);
	}
};

/**
 * @brief Key, value and map type of each table, plus its three policies:
 * `gated` (exists under `TAU_CACHE` only), `taint_aware` (the wrapper writes
 * only across an unchanged `taint_count`), and `solver_flushed` (cleared by
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

/**
 * @brief The table instance, or `nullptr` when it does not exist (a gated
 * table with `TAU_CACHE` off). The ONLY place the module spells
 * `#ifdef TAU_CACHE` is this function's definition in ctx.tmpl.h. An
 * unconditional table is a function-local static `create_cache` reference, as
 * `get_free_vars`' is.
 */
template <NodeType node, table T>
typename table_traits<node, T>::map_t* table_ptr();

// --- reads, writes, the wrapper, the flush -------------------------------------------
// Template parameters are `<node, table>` throughout, the order of
// `table_traits`, and both are always spelled explicitly, since a `key_t`
// parameter is a non-deduced context: `find<node, table::push_memo>(key)`.
//
// LIFETIME of a pointer or reference INTO a table: it stays valid across
// every later insertion, because the maps are node-based `unordered_map`s,
// which keep mapped values put across a rehash — so a recursive fill may hold
// a parent's reference while its children insert. It is invalidated only by a
// `bintree<node>::gc()` sweep, which REBUILDS every registered table. Nothing
// in this module sweeps (the callers are the interpreter's `maybe_gc` and the
// tests), so a facet reference is good for the length of a run; copy it
// before any call that can sweep. dag.h states the same rule for
// `get_free_vars`.
//
// `store` and `memoised` come in two forms, chosen by the table's `gated`
// flag: an unconditional table always has storage and hands out a REFERENCE,
// while a gated table has none with `TAU_CACHE` off and can only return a
// VALUE. The split is a `requires` clause rather than a second name, so a
// caller that asks a gated table for a reference fails to compile in both
// build types instead of in Release alone.

/// Pointer to the entry for `key`, or `nullptr` on a miss or a missing table.
/// A plain read; the lazy fills go through `memoised` below.
template <NodeType node, table T>
const typename table_traits<node, T>::value_t*
find(const typename table_traits<node, T>::key_t& key);

/// Read table `T`: a copy of the entry for `key`, or nothing.
template <NodeType node, table T>
std::optional<typename table_traits<node, T>::value_t>
lookup(const typename table_traits<node, T>::key_t& key);

/// Write an UNCONDITIONAL table and return a reference to the stored value.
/// Insert or assign: an entry is a pure function of its key, so a rewrite
/// stores the same value and any outstanding reference stays valid.
template <NodeType node, table T>
	requires (!table_traits<node, T>::gated)
TAU_ANTI_PRENEX_NO_DANGLING
const typename table_traits<node, T>::value_t&
store(const typename table_traits<node, T>::key_t& key,
	typename table_traits<node, T>::value_t value);

/// Write a GATED table, or do nothing when it does not exist. No return
/// value: with `TAU_CACHE` off there is nothing to hand back a reference to.
template <NodeType node, table T>
	requires (table_traits<node, T>::gated)
void store(const typename table_traits<node, T>::key_t& key,
	typename table_traits<node, T>::value_t value);

/**
 * @brief §1/§6 memo wrapper for a GATED table: return the entry for `key`,
 * and on a miss compute `compute()` and record it.
 *
 * For a `taint_aware` table the result is recorded only if `taint_count` is
 * unchanged across the computation; a hit inside the computation is a hit
 * inside every enclosing one, so transitivity comes for free. For every other
 * table it is recorded unconditionally. The result is always returned; a
 * tainted result is returned but never cached. When the table does not exist
 * this is a plain `compute()` and nothing else.
 */
template <NodeType node, table T, typename Compute>
	requires (table_traits<node, T>::gated)
typename table_traits<node, T>::value_t
memoised(const typename table_traits<node, T>::key_t& key, Compute&& compute);

/**
 * @brief The same wrapper for an UNCONDITIONAL table: the lazy fill of a
 * structural facet (`formula_size`, `neg_of`, `is_negative_tree`, `atoms`).
 * Compute on the first query, store, and hand back a reference into the
 * table, so that an accessor can return `const trefs&`. The accessor is the
 * table's only writer (§10: computed once, never recomputed).
 *
 * No structural table is `taint_aware`: a facet is a pure function of its
 * node, and a budget hit cannot change it.
 *
 * `compute()` may recurse into `memoised` on the same table — the reference
 * this call returns survives those insertions (see LIFETIME above) — but
 * recursion costs one frame per level, so a whole-tree fill belongs in a
 * post-order walk (`pre_order`'s `up` callback) that calls `find` / `store`
 * per node, not in a recursive `compute`.
 */
template <NodeType node, table T, typename Compute>
	requires (!table_traits<node, T>::gated)
TAU_ANTI_PRENEX_NO_DANGLING
const typename table_traits<node, T>::value_t&
memoised(const typename table_traits<node, T>::key_t& key, Compute&& compute);

/// §1 FLUSH: when the solver configuration changes, clear every
/// `solver_flushed` table (`solver_memo`, `push_memo`, `elim_memo`) as a
/// whole, since an entry carries no record of whether it embedded a solver
/// verdict. `qbf_memo` is exempt, its entries being mathematical truths, and
/// so is every structural table. The caller is the api layer on a solver
/// option change (layer 4 and later); until then this is a test-only entry
/// point.
template <NodeType node>
void flush_solver_dependent();

} // namespace idni::tau_lang::anti_prenexing

#include "ctx.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_H__
