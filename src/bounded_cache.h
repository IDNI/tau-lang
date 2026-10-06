// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// bounded_cache — std::map with a configurable max-size bound.
//
// Used for the string-keyed synthesis cache of `call_ltlsynt_game`
// (ltl_aba_synthesis.tmpl.h), in runtime-bound mode with the bound
// `cache_bound` below. The tref-keyed caches made with `create_cache`
// (e.g. in src/normal_forms.tmpl.h and src/satisfiability.tmpl.h) still
// use a plain `std::map`, unbounded apart from the tree GC pruning their
// stale keys and values.
//
// It can replace `std::map<K, V, Cmp>` at a site following the
// convention:
//
//     using cache_t = std::map<K, V, Cmp>;
//     static cache_t& cache = tree<node>::template create_cache<cache_t>();
//
// Why this exists
// ---------------
// The parser tree library's GC pass prunes entries whose tref keys or
// values were collected, but in a long-running process the entries that
// survive every GC accumulate without limit, and a string-keyed cache gets
// no pruning at all. Memory grows with the number of distinct formulas
// seen.
//
// `bounded_cache<K, V, Cmp, Max>` adds a hard upper bound: when an
// insert would push size past the bound, the oldest entry (by FIFO insert
// order) is evicted first.  The eviction queue is kept as a
// `std::list<iterator>`, so insert and eviction are amortised O(1).
//
// FIFO rather than LRU: the bookkeeping is cheaper (no list splice on
// every `find` hit), and on the synthesis workloads hot formulas keep
// being re-inserted, so insertion order approximates access recency for
// the hot set. Splicing the queue on `find` hits would turn it into LRU.
//
// Conformance
// -----------
// Provides the subset of `std::map` the cache sites use:
//   - operator[],   find,    contains
//   - emplace, insert (the copy and move overloads)
//   - begin / end  (returning the underlying map iterators)
//   - size,    empty,   clear,    erase
//
// Iterator validity matches std::map's exactly EXCEPT that `emplace`
// / `insert` / `operator[]` erase the evicted entries, invalidating any
// iterator to them (including one obtained from `begin()`). Do not hold
// an iterator across a mutating call.
//
// GC integration
// --------------
// The parser's `bintree<T>::create_cache<cache_t>()` registers a
// gc_callback that walks the cache's `(key, value)` entries looking for
// stale `tref`s and erases them. bounded_cache is iterable (begin/end
// over the map) and supports erase(), which is all that callback needs.

#ifndef __IDNI__TAU__BOUNDED_CACHE_H__
#define __IDNI__TAU__BOUNDED_CACHE_H__

#include <cstddef>
#include <list>
#include "env_limits.h"
#include <map>
#include <utility>

namespace idni::tau_lang {

/**
 * @brief The runtime bound consulted by the string-keyed synthesis caches
 * (0 = unbounded).
 *
 * Read by the cache of `call_ltlsynt_game`. A cache constructed with
 * `&cache_bound` reads the current value on every insert, so
 * `--cache-bound` / REPL `set cachebound` / `api::set_cache_bound` take
 * effect immediately. Default 4096; environment fallback `TAU_CACHE_BOUND`.
 */
inline env_limit<std::size_t> cache_bound{ "TAU_CACHE_BOUND", 4096 };

/**
 * @brief std::map with a configurable max-size bound and FIFO eviction
 * (see the file header for the rationale and the std::map subset offered).
 *
 * Two bounding modes. Default construction uses the compile-time `Max`
 * template parameter. Constructing with a pointer to a runtime bound
 * (or to an env_limit) switches to runtime mode: the pointee is read on
 * every insert, so
 * a `set cachebound N` tightens or loosens a live cache. In both
 * modes a bound of 0 means unbounded; in runtime mode the FIFO
 * queue is maintained even while the bound is 0, so a later
 * non-zero bound still knows the insertion order.
 * @tparam K Key type.
 * @tparam V Mapped type.
 * @tparam Cmp Key comparator.
 * @tparam Max Compile-time bound; 0 means unbounded -- same as std::map.
 */
template <typename K, typename V,
          typename Cmp = std::less<K>,
          std::size_t Max = 0> // Max == 0 means unbounded — same as std::map
struct bounded_cache {
	using map_t = std::map<K, V, Cmp>;
	using key_type    = K;
	using mapped_type = V;
	using value_type  = typename map_t::value_type;
	using iterator    = typename map_t::iterator;
	using const_iterator = typename map_t::const_iterator;
	using key_compare = Cmp;

	/// @brief Compile-time mode: the bound is the `Max` template parameter.
	bounded_cache() = default;
	/// @brief Runtime mode: the bound is read from @p runtime_bound on
	/// every insert (see the class comment).
	/// @param runtime_bound Bound to read; must outlive the cache. A null
	/// pointer selects compile-time mode.
	explicit bounded_cache(const std::size_t* runtime_bound)
		: runtime_bound_(runtime_bound) {}
	/// @brief Runtime mode over a limit with an environment fallback: the
	/// limit's effective value (see env_limit) is read on every insert.
	/// @param runtime_limit Limit to read; must outlive the cache. A null
	/// pointer selects compile-time mode.
	explicit bounded_cache(const env_limit<std::size_t>* runtime_limit)
		: runtime_limit_(runtime_limit) {}

	// --- queries ----------------------------------------------------

	/// @brief True iff the cache holds no entry.
	bool empty()       const noexcept { return map_.empty(); }
	/// @brief Number of entries currently held.
	std::size_t size() const noexcept { return map_.size(); }
	/// @brief The compile-time bound `Max`.
	static constexpr std::size_t max_size() noexcept { return Max; }
	/// @brief The bound in effect: the runtime pointee, else `Max`.
	std::size_t bound() const noexcept {
		return runtime_limit_ ? runtime_limit_->get()
			: runtime_bound_ ? *runtime_bound_ : Max;
	}

	/// @brief Lookup by key, as std::map::find.
	iterator       find(const K& k)       { return map_.find(k); }
	/// @brief Lookup by key, as std::map::find.
	const_iterator find(const K& k) const { return map_.find(k); }

	/// @brief True iff @p k is present.
	bool contains(const K& k) const { return map_.contains(k); }

	/// @brief Iterator to the first entry of the underlying map.
	iterator       begin()       { return map_.begin(); }
	/// @brief Iterator to the first entry of the underlying map.
	const_iterator begin() const { return map_.begin(); }
	/// @brief Past-the-end iterator of the underlying map.
	iterator       end()         { return map_.end(); }
	/// @brief Past-the-end iterator of the underlying map.
	const_iterator end()   const { return map_.end(); }

	// --- mutations --------------------------------------------------

	/// @brief Access or default-insert the value for @p k; an insert may
	/// evict the oldest entry.
	V& operator[](const K& k) {
		auto it = map_.find(k);
		if (it != map_.end()) return it->second;
		auto [ins, _] = map_.emplace(k, V{});
		on_insert(ins);
		return ins->second;
	}

	/// @brief Construct an entry from @p args, as std::map::emplace; a new
	/// entry may evict the oldest one.
	/// @return {iterator to the entry, `true` if it was inserted}.
	template <typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args) {
		auto r = map_.emplace(std::forward<Args>(args)...);
		if (r.second) on_insert(r.first);
		return r;
	}

	/// @brief Insert a copy of @p v, as std::map::insert; a new entry may
	/// evict the oldest one.
	std::pair<iterator, bool> insert(const value_type& v) {
		auto r = map_.insert(v);
		if (r.second) on_insert(r.first);
		return r;
	}

	/// @brief Insert @p v by move, as std::map::insert; a new entry may
	/// evict the oldest one.
	std::pair<iterator, bool> insert(value_type&& v) {
		auto r = map_.insert(std::move(v));
		if (r.second) on_insert(r.first);
		return r;
	}

	/// @brief Erase the entry for @p k; returns the number erased (0 or 1).
	/// O(n) scan of the FIFO queue when tracked.
	std::size_t erase(const K& k) {
		auto it = map_.find(k);
		if (it == map_.end()) return 0;
		// Drop from FIFO queue too if we track it.
		if (tracked()) {
			for (auto qit = order_.begin(); qit != order_.end(); ++qit) {
				if (*qit == it) { order_.erase(qit); break; }
			}
		}
		map_.erase(it);
		return 1;
	}

	/// @brief Erase the entry at @p it and return the following iterator.
	/// O(n) scan of the FIFO queue when tracked.
	iterator erase(iterator it) {
		if (tracked()) {
			for (auto qit = order_.begin(); qit != order_.end(); ++qit) {
				if (*qit == it) { order_.erase(qit); break; }
			}
		}
		return map_.erase(it);
	}

	/// @brief Remove every entry (and the FIFO queue when tracked).
	void clear() {
		map_.clear();
		if (tracked()) order_.clear();
	}

	// --- introspection (used by tests/unit/test_bounded_cache.cpp) --

	/// @brief Number of entries evicted so far.
	std::size_t evictions() const noexcept { return evictions_; }

private:
	// Whether the FIFO queue is maintained at all: always in runtime
	// mode (the bound can become non-zero later), only for Max != 0 in
	// compile-time mode.
	bool tracked() const noexcept { return Max != 0 || runtime_bound_
		|| runtime_limit_; }

	// Enqueue the new entry @p it and evict the oldest entries while the
	// size exceeds a non-zero bound.
	void on_insert(iterator it) {
		if (!tracked()) return; // unbounded, compile-time mode
		order_.push_back(it);
		const std::size_t b = bound();
		if (b == 0) return; // unbounded right now; keep the queue
		while (map_.size() > b && !order_.empty()) {
			iterator victim = order_.front();
			order_.pop_front();
			map_.erase(victim);
			++evictions_;
		}
	}

	map_t map_;

	// Iterator-stable FIFO queue.  std::map iterators are stable
	// across insert/erase (only the erased iterator is invalidated),
	// so this is sound.
	std::list<iterator> order_;

	const std::size_t* runtime_bound_ = nullptr;
	const env_limit<std::size_t>* runtime_limit_ = nullptr;

	std::size_t evictions_ = 0;
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOUNDED_CACHE_H__
