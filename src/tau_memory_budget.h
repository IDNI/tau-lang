// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file tau_memory_budget.h
 * @brief Budget on the interned tree store, measured in nodes.
 *
 * Every tree node the process builds is interned in `bintree<node>::M()` and
 * is freed only by a `bintree<node>::gc()` sweep, so the live node count is
 * what a long-running process actually spends. This file caps it.
 *
 * The cap is checked **on entry** to a unit of work, never during it and never
 * after: a call that starts with the store already at its cap is refused
 * before it does anything, and a call that was allowed to start runs to
 * completion and hands back its value even if it ends over the cap. Work that
 * was permitted is never thrown away, and work that cannot be permitted is
 * never paid for.
 *
 * Checking on entry also keeps the node-creation path untouched -- no counter,
 * no branch in `tree<node>::get_raw` -- and means nothing is ever abandoned
 * half-way, so no traversal memo is left holding a partial answer. It needs no
 * state of its own either: the check is a fresh measurement, so a store that
 * drops back under its cap (through `interpreter::maybe_gc`, or because the
 * trees went out of scope) admits the next call with nothing to reset.
 *
 * The consequence, stated plainly: a single call can overshoot the cap without
 * being stopped. What is bounded is the store a call is allowed to *start*
 * from, so growth is caught one call later, not within the call that caused
 * it.
 *
 * Runtime parameter by policy, like every limit in `ltl_aba_limits.h`: the CLI
 * flag, the REPL `set` command and the `api::set_*` setter write the
 * parameter, and the accessor resolves parameter > environment > default.
 */

#ifndef __IDNI__TAU__TAU_MEMORY_BUDGET_H__
#define __IDNI__TAU__TAU_MEMORY_BUDGET_H__

#include "backends/bdds/babdd.h"
#include "env_limits.h"
#include "tau_diagnostics.h"
#include "tau_tree.h"

#include <atomic>
#include <cstddef>
#include <string>

namespace idni::tau_lang {

/**
 * @brief Cap on live interned tree nodes; 0 = unlimited (the default).
 *
 * Runtime parameter by policy (`--tref-budget`, REPL `set trefbudget`,
 * `api::set_tref_budget`). The sentinel -1 means "not set", in which case
 * `TAU_TREF_BUDGET` is consulted and 0 applies when that is absent too. Read
 * through @ref tref_budget.
 */
inline long tref_budget_param = -1;

/**
 * @brief Percentage of @ref tref_budget at which the store counts as
 * "approaching the limit" and a sweep is worth forcing.
 *
 * Runtime parameter by policy (`--tref-budget-soft`, REPL `set
 * trefbudgetsoft`, `api::set_tref_budget_soft_percent`). The sentinel -1 means
 * "not set", in which case `TAU_TREF_BUDGET_SOFT` is consulted and 75 applies
 * when that is absent too. Read through @ref tref_budget_soft_percent.
 */
inline long tref_budget_soft_param = -1;

/// Live node count when the outermost @ref budget_scope was entered.
inline size_t tref_budget_last_before = 0;
/// Live node count when it was left.
inline size_t tref_budget_last_after = 0;

/// Effective node cap (0 = unlimited).
inline size_t tref_budget() {
	if (tref_budget_param >= 0) return (size_t) tref_budget_param;
	return env_limit_count("TAU_TREF_BUDGET", 0);
}

/// Effective soft-mark percentage, clamped to 1..100.
inline size_t tref_budget_soft_percent() {
	size_t pct = tref_budget_soft_param >= 0
		? (size_t) tref_budget_soft_param
		: env_limit_count("TAU_TREF_BUDGET_SOFT", 75);
	if (pct == 0 || pct > 100) {
		static thread_local bool warned = false;
		if (!warned) {
			warned = true;
			TAU_LOG_WARNING << "tref budget soft mark " << pct
				<< " is not a percentage in 1..100; using 75";
		}
		pct = 75;
	}
	return pct;
}

/// Node count at which the store counts as approaching the cap; 0 when
/// unlimited, since there is then nothing to approach.
inline size_t tref_soft_mark() {
	const size_t cap = tref_budget();
	if (!cap) return 0;
	const size_t pct = tref_budget_soft_percent();
	// Multiply first: cap / 100 * pct truncates to 0 for every cap below
	// 100, which would make a small budget never approach itself. Divide
	// first only where multiplying would overflow.
	return cap > (size_t) -1 / pct ? cap / 100 * pct : cap * pct / 100;
}

/// Live interned node count.
template <NodeType node>
size_t tref_count() { return tree<node>::m_size(); }

/// True while the store is at or above the soft mark (never when unlimited).
template <NodeType node>
bool over_tref_soft_mark() {
	const size_t soft = tref_soft_mark();
	return soft && tref_count<node>() >= soft;
}

/// True while the store is at or above the cap (never when unlimited).
template <NodeType node>
bool over_tref_budget() {
	const size_t cap = tref_budget();
	return cap && tref_count<node>() >= cap;
}

/// The message a refusal carries. Built here so every refusal reads alike.
template <NodeType node>
std::string tref_budget_message() {
	return "memory budget exhausted: " + std::to_string(tref_count<node>())
		+ " interned tree nodes against a limit of "
		+ std::to_string(tref_budget())
		+ " (--tref-budget / TAU_TREF_BUDGET)";
}

/**
 * @brief Records what one unit of work did to the store, for @ref residue.
 *
 * Nests, so guarded functions may call each other: the outermost scope owns
 * the before/after pair and an inner one leaves it alone. It decides nothing
 * -- the budget is enforced by @ref with_budget on entry -- and it allocates
 * nothing, reading the store's size twice.
 */
template <NodeType node>
struct budget_scope {
	budget_scope() {
		if (depth()++) return;
		tref_budget_last_before = tref_count<node>();
		tref_budget_last_after = tref_budget_last_before;
	}

	~budget_scope() {
		if (--depth() == 0) tref_budget_last_after = tref_count<node>();
	}

	budget_scope(const budget_scope&) = delete;
	budget_scope& operator=(const budget_scope&) = delete;

	/// Growth the unit of work left behind, valid once its scope has closed.
	static size_t residue() {
		return tref_budget_last_after > tref_budget_last_before
			? tref_budget_last_after - tref_budget_last_before : 0;
	}

private:
	// Not atomic: a scope nests within one thread's call stack.
	static int& depth() { static thread_local int d = 0; return d; }
};

/// Nesting depth of @ref with_budget calls on this thread.
inline int& with_budget_depth() {
	static thread_local int depth = 0;
	return depth;
}

/**
 * @brief Whether a bdd node table filled since the last boundary; if so,
 * readies the process for the next unit of work.
 *
 * The work that filled the table computed with F in place of the nodes that
 * did not fit (see @ref bdd_node_table_exhausted), and its unknown values may
 * sit in any tree cache. The bdd memos, handle tables and constant caches took
 * no write meanwhile, so emptying the tree caches and lowering the flag is
 * enough: the next unit of work runs normally, and fails the same way only if
 * it too needs a new node. Call it only where no reference into a cache is
 * held, that is, between units of work.
 */
template <NodeType node>
bool take_bdd_node_table_exhausted() {
	if (!bdd_node_table_exhausted) return false;
	tree<node>::clear_caches();
	bdd_node_table_exhausted = false;
	return true;
}

/**
 * @brief Refuses @p f when the store is already at its cap, runs it otherwise.
 *
 * @p f must return a `result<T>`. A refusal never runs @p f at all, so no
 * value is computed to be thrown away and no report is lost. A call that is
 * allowed to start keeps its value whatever it does to the store: the budget
 * says which calls may begin, not which results are fit to return.
 *
 * The refusal is deliberately blind to what @p f would have returned. It has
 * to be: an `std::optional<solution>` whose `nullopt` means UNSAT, a `bool`
 * whose `false` means "not equivalent" and a `tref` a caller will dereference
 * are all ordinary returns, so there is no sentinel a budget failure could
 * ride on without being read as an answer.
 *
 * A bdd node table, in contrast, fills during a call: its value is then
 * unknown and is replaced by an error. Only the outermost call recovers
 * (@ref take_bdd_node_table_exhausted); a nested one leaves the flag up, so
 * its caller cannot turn the failure into an answer either. A call that
 * starts with the flag already up, raised by work outside any boundary, is
 * refused, since that work may have left unknown values in the caches.
 */
template <NodeType node, typename F>
std::invoke_result_t<F> with_budget(F&& f) {
	int& depth = with_budget_depth();
	auto exhausted = [] {
		std::invoke_result_t<F> r;
		r.error(code::runtime_error, messages::bdd_node_table_exhausted);
		return r;
	};
	if (depth == 0 && take_bdd_node_table_exhausted<node>())
		return exhausted();
	if (over_tref_budget<node>()) {
		std::invoke_result_t<F> refused;
		refused.error(code::runtime_error, tref_budget_message<node>());
		return refused;
	}
	budget_scope<node> scope;
	++depth;
	auto r = f();
	--depth;
	if (!bdd_node_table_exhausted) return r;
	if (depth == 0) take_bdd_node_table_exhausted<node>();
	r.error(code::runtime_error, messages::bdd_node_table_exhausted);
	return r;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TAU_MEMORY_BUDGET_H__
