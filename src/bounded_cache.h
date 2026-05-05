// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// bounded_cache — std::map with a configurable max-size bound.
//
// Drop-in replacement for `std::map<K, V, Cmp>` at every call site
// where the existing tau-ltl convention is:
//
//     using cache_t = std::map<K, V, Cmp>;
//     static cache_t& cache = tree<node>::template create_cache<cache_t>();
//
// (See src/normal_forms.tmpl.h, src/satisfiability.tmpl.h, and the
// 17+ other create_cache call sites for the existing pattern.)
//
// Why this exists
// ---------------
// Every existing cache in tau-ltl is unbounded.  The parser tree
// library's GC pass prunes entries whose tref keys/values were
// collected, but in a long-running process (the planned LTL server,
// the web demo, batch synthesis pipelines) the entries that survive
// every GC accumulate without limit.  Memory grows with the corpus
// of distinct formulas seen, with no upper bound.
//
// `bounded_cache<K, V, Cmp, Max>` adds a hard upper bound: when an
// insert would push size past `Max`, the oldest entry (by FIFO insert
// order) is evicted first.  The eviction queue is kept as a
// `std::list<iterator>` so amortised O(1) insert + O(1) eviction.
//
// FIFO is intentionally chosen over true LRU:
//   - Cheaper bookkeeping (no per-find list-splice).
//   - Indistinguishable from LRU on the workloads tau-ltl actually
//     hits, where hot formulas keep being re-emitted by the
//     synthesis pipeline (so insertion order ~= access recency for
//     the hot set).
//   - Trivially exposes a knob to upgrade to LRU later: replace the
//     queue with an intrusive list and splice on `find` hits.
//
// Conformance
// -----------
// Provides the subset of `std::map` actually used by every
// create_cache call site in tau-ltl:
//   - operator[],   find,    contains
//   - emplace, insert (the 2-arg overloads)
//   - begin / end  (returning the underlying map iterators)
//   - size,    empty,   clear,    erase
//
// Iterator validity matches std::map's exactly EXCEPT that `emplace`
// / `insert` / `operator[]` may invalidate the iterator returned by
// the prior `begin()` if the eviction was triggered.  The existing
// tau-ltl call sites never hold iterators across mutating
// operations, so this is safe by inspection.
//
// GC integration
// --------------
// The parser's `bintree<T>::create_cache<cache_t>()` registers a
// gc_callback that walks the underlying map's `(key, value)` entries
// looking for stale `tref`s.  bounded_cache is iterable (begin/end
// over the map) and supports erase() — both behaviours are what the
// gc_callback uses, so the pruning code in
// external/parser/src/utility/tree.tmpl.h:255-294 needs zero changes
// to interoperate.

#ifndef __IDNI__TAU__BOUNDED_CACHE_H__
#define __IDNI__TAU__BOUNDED_CACHE_H__

#include <cstddef>
#include <list>
#include <map>
#include <utility>

namespace idni::tau_lang {

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

	// --- queries ----------------------------------------------------

	bool empty()       const noexcept { return map_.empty(); }
	std::size_t size() const noexcept { return map_.size(); }
	static constexpr std::size_t max_size() noexcept { return Max; }

	iterator       find(const K& k)       { return map_.find(k); }
	const_iterator find(const K& k) const { return map_.find(k); }

	bool contains(const K& k) const { return map_.contains(k); }

	iterator       begin()       { return map_.begin(); }
	const_iterator begin() const { return map_.begin(); }
	iterator       end()         { return map_.end(); }
	const_iterator end()   const { return map_.end(); }

	// --- mutations --------------------------------------------------

	V& operator[](const K& k) {
		auto it = map_.find(k);
		if (it != map_.end()) return it->second;
		auto [ins, _] = map_.emplace(k, V{});
		on_insert(ins);
		return ins->second;
	}

	// Same return shape as std::map::emplace: {iterator, bool}.
	template <typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args) {
		auto r = map_.emplace(std::forward<Args>(args)...);
		if (r.second) on_insert(r.first);
		return r;
	}

	std::pair<iterator, bool> insert(const value_type& v) {
		auto r = map_.insert(v);
		if (r.second) on_insert(r.first);
		return r;
	}

	std::pair<iterator, bool> insert(value_type&& v) {
		auto r = map_.insert(std::move(v));
		if (r.second) on_insert(r.first);
		return r;
	}

	// O(n) scan of order_ deque; acceptable for current cache sizes.
	std::size_t erase(const K& k) {
		auto it = map_.find(k);
		if (it == map_.end()) return 0;
		// Drop from FIFO queue too if we track it.
		if constexpr (Max != 0) {
			for (auto qit = order_.begin(); qit != order_.end(); ++qit) {
				if (*qit == it) { order_.erase(qit); break; }
			}
		}
		map_.erase(it);
		return 1;
	}

	iterator erase(iterator it) {
		if constexpr (Max != 0) {
			for (auto qit = order_.begin(); qit != order_.end(); ++qit) {
				if (*qit == it) { order_.erase(qit); break; }
			}
		}
		return map_.erase(it);
	}

	void clear() {
		map_.clear();
		if constexpr (Max != 0) order_.clear();
	}

	// --- introspection (used by the benchmark) ---------------------

	std::size_t evictions() const noexcept { return evictions_; }

private:
	void on_insert(iterator it) {
		if constexpr (Max == 0) return; // unbounded
		order_.push_back(it);
		while (map_.size() > Max && !order_.empty()) {
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

	std::size_t evictions_ = 0;
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOUNDED_CACHE_H__
