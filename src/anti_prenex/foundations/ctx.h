// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ctx.h
 * @brief Anti-prenexing foundations (layer 0), package E: `ctx` (the §1 ctx
 * table), the seven GLOBAL memo tables, the memo wrapper with the taint rule,
 * and the flush.
 *
 * CACHE GATING (ruling 2026-09-07, fwd.h): every table is a static
 * GC-registered `create_cache` instance under `#ifdef TAU_CACHE` and does not
 * exist otherwise. THE `#ifdef` LIVES IN THIS FILE ALONE: consumers reach a
 * table only through `lookup` / `store` / `memoised` / `flush_solver_dependent`,
 * which are passthroughs (a miss, a no-op, plain computation) when the tables
 * do not exist, so no algorithm code sees the macro and no result may depend
 * on a hit. `TAU_CACHE` is OFF in Debug builds.
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
 * The GLOBAL members of the spec's table — `taint_count` and the seven memo
 * tables — are not stored here: they are reached through `taint_count<node>()`
 * and the table functions below (§1: "the global tables, cross-run, never
 * reset"). Everything stored here is component-scoped.
 */
template <NodeType node>
struct ctx {
	/// §1 `type`: the block's BA type `τ`, a `ba_types<node>` id. Single, by
	/// invariant 2 (components are type-homogeneous).
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
	/// COMPONENT-scoped counter, reset to 0 at setup.
	size_t expand_count = 0;

	/**
	 * @brief §5 setup for one component: `type`, the two rank maps over `P`
	 * (`P[0]` outermost), the knobs from options.h, `keep_functional`,
	 * `expand_count = 0`.
	 */
	static ctx for_component(const block& P, size_t type, bool keep_functional);
};

// --- taint ----------------------------------------------------------------------

/// §1 `taint_count`: budget hits so far, GLOBAL, never reset. Incremented by
/// every source of taint (EXPAND and DECOMPOSE_ARMS exhaustion, a
/// DECIDE_FINITE sweep abandoned to an ASK that ends `unknown`); read by the
/// memo wrapper, which caches only across an unchanged count.
template <NodeType node>
size_t& taint_count();

/// The one increment every source of taint calls.
template <NodeType node>
void taint();

// --- the seven tables -----------------------------------------------------------

/// The GLOBAL tables of the §1 ctx table.
enum class table {
	push_memo,   ///< `(REWRAP(φ, X), keep_functional) → formula` (D3); also EXPAND's state memo
	elim_memo,   ///< `(REWRAP(clause, X), keep_functional) → formula` (D3)
	quant_memo,  ///< functional-quantifier term → term; the key IS the query
	cof_memo,    ///< `(settled term, x) → cof_entry`; filled and read by COF
	atoms_memo,  ///< formula node → the atoms occurring in it, units opaque (§4)
	solver_memo, ///< canonical closed query → sat / unsat / unknown
	qbf_memo     ///< canonical closed pure-Boolean query → T / F; never flushed
};

/// Structural hash of a `(tref, tref)` key, consistent with `tref_pair_equal`.
template <NodeType node>
struct tref_pair_hash {
	size_t operator()(const std::pair<tref, tref>& k) const {
		const size_t a = hash_lcrs_tref<node>{}(k.first);
		const size_t b = hash_lcrs_tref<node>{}(k.second);
		return a ^ (b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2));
	}
};

/// Structural equality of a `(tref, tref)` key.
template <NodeType node>
struct tref_pair_equal {
	bool operator()(const std::pair<tref, tref>& a,
		const std::pair<tref, tref>& b) const
	{
		return subtree_equality<node>{}(a.first, b.first)
			&& subtree_equality<node>{}(a.second, b.second);
	}
};

/// Structural hash of a `(tref, bool)` key, consistent with
/// `subtree_pair_equal<node, bool>`.
template <NodeType node>
struct tref_bool_hash {
	size_t operator()(const std::pair<tref, bool>& k) const {
		return hash_lcrs_tref<node>{}(k.first)
			^ (k.second ? size_t(0x9e3779b97f4a7c15ULL) : size_t(0));
	}
};

/**
 * @brief Key, value and map type of each table, and its two policies:
 * `taint_aware` (the wrapper writes only across an unchanged `taint_count`)
 * and `solver_flushed` (cleared by `flush_solver_dependent`).
 */
template <NodeType node, table T>
struct table_traits;

template <NodeType node>
struct table_traits<node, table::push_memo> {
	using key_t   = std::pair<tref, bool>;
	using value_t = tref;
	using map_t   = std::unordered_map<key_t, value_t, tref_bool_hash<node>,
		subtree_pair_equal<node, bool>>;
	static constexpr bool taint_aware    = true;
	static constexpr bool solver_flushed = true;
};

template <NodeType node>
struct table_traits<node, table::elim_memo> {
	using key_t   = std::pair<tref, bool>;
	using value_t = tref;
	using map_t   = std::unordered_map<key_t, value_t, tref_bool_hash<node>,
		subtree_pair_equal<node, bool>>;
	static constexpr bool taint_aware    = true;
	static constexpr bool solver_flushed = true;
};

template <NodeType node>
struct table_traits<node, table::quant_memo> {
	using key_t   = tref;
	using value_t = tref;
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false;
};

template <NodeType node>
struct table_traits<node, table::cof_memo> {
	using key_t   = std::pair<tref, tref>;
	using value_t = cof_entry;
	using map_t   = std::unordered_map<key_t, value_t, tref_pair_hash<node>,
		tref_pair_equal<node>>;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false;
};

template <NodeType node>
struct table_traits<node, table::atoms_memo> {
	using key_t   = tref;
	using value_t = trefs; ///< sorted by `subtree_less<node>` (subst.h `atoms`)
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false;
};

template <NodeType node>
struct table_traits<node, table::solver_memo> {
	using key_t   = tref;
	using value_t = answer;
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = true;
};

template <NodeType node>
struct table_traits<node, table::qbf_memo> {
	using key_t   = tref;
	using value_t = bool;
	using map_t   = subtree_unordered_map<node, value_t>;
	static constexpr bool taint_aware    = false;
	static constexpr bool solver_flushed = false; ///< exempt from every flush (§1)
};

#ifdef TAU_CACHE
/// The static GC-registered instance of table `T` (`create_cache`). Exists
/// only under `TAU_CACHE`; nothing outside ctx.tmpl.h calls it.
template <NodeType node, table T>
typename table_traits<node, T>::map_t& table_instance();
#endif // TAU_CACHE

// --- the wrapper and the flush -----------------------------------------------------

/// Read table `T`: the entry for `key`, or nothing. Always nothing when the
/// tables do not exist.
template <table T, NodeType node>
std::optional<typename table_traits<node, T>::value_t>
lookup(const typename table_traits<node, T>::key_t& key);

/// Write table `T` unconditionally (for entries that are pure functions of
/// the key). A no-op when the tables do not exist.
template <table T, NodeType node>
void store(const typename table_traits<node, T>::key_t& key,
	typename table_traits<node, T>::value_t value);

/**
 * @brief §1/§6 memo wrapper: return the entry for `key`; on a miss compute
 * `compute()`, and record it — for a `taint_aware` table only if
 * `taint_count` is unchanged across the computation (a hit inside the
 * computation is a hit inside every enclosing one: transitivity for free),
 * for every other table unconditionally. The result is always returned;
 * a tainted result is returned, never cached. Plain `compute()` when the
 * tables do not exist.
 */
template <table T, NodeType node, typename Compute>
typename table_traits<node, T>::value_t
memoised(const typename table_traits<node, T>::key_t& key, Compute&& compute);

/// §1 FLUSH: the solver configuration changed — clear every `solver_flushed`
/// table (`solver_memo`, `push_memo`, `elim_memo`). `qbf_memo` is exempt.
template <NodeType node>
void flush_solver_dependent();

} // namespace idni::tau_lang::anti_prenexing

#include "ctx.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_H__
