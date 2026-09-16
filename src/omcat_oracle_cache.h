// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Runtime type-oracle cache for Algorithm D's execution layer (Opt-8).
//
// At runtime, the concrete controller must repeatedly answer:
//   σ = tp(m, x)?     — the 2-type of current (memory, input).
//   ρ ∈ A_{m,x}?      — is a given next-memory-type achievable given (m, x)?
//
// These are theory-level oracle calls that would otherwise cost a
// full-BA satisfiability check each step.  For ω-categorical theories,
// the answer depends ONLY on the atomic query results (e.g., for qlt,
// the orderings of {m, x, c_1, …, c_k}).  These atomic queries are
// few and fast; caching them eliminates redundant work across steps.
//
// This header provides a thread-safe LRU-less map<(atomic-query-key),
// type-index>.  It is a POC for Opt-8 — concrete integration with
// tau-lang's runtime comes when Algorithm D's Phase 6 (execution layer)
// is wired up.

#ifndef __IDNI__TAU__OMCAT_ORACLE_CACHE_H__
#define __IDNI__TAU__OMCAT_ORACLE_CACHE_H__

#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace idni::tau_lang::omcat {

/**
 * @brief Key: a hashable identifier for an atomic-query batch.
 *
 * The discriminator encodes which atomic relations were queried; the
 * result is the type index (int).
 */
struct oracle_key {
	uint64_t hash;
	// BA2-7: keep the queried batch and compare it too -- hash-only
	// equality let FNV collisions alias distinct batches to one cached
	// answer.
	std::vector<int> atomic_results;
	bool operator==(const oracle_key& o) const {
		return hash == o.hash && atomic_results == o.atomic_results;
	}
};
/// @brief Hash functor for `oracle_key` (uses the precomputed FNV hash).
struct oracle_key_hash {
	/// @brief The stored hash of @p k.
	size_t operator()(const oracle_key& k) const noexcept {
		return static_cast<size_t>(k.hash);
	}
};

/// @brief Thread-safe map from atomic-query batches to type indices.
class oracle_cache {
public:
	/// @brief Returns the cached type index for `key`, or -1 if not
	/// present.
	int get(const oracle_key& key) const {
		std::lock_guard<std::mutex> lg(mtx_);
		auto it = map_.find(key);
		return it == map_.end() ? -1 : it->second;
	}
	/// @brief Stores the result; overwrites existing entry.
	void put(const oracle_key& key, int value) {
		std::lock_guard<std::mutex> lg(mtx_);
		map_[key] = value;
	}
	/// @brief Compute-and-cache: calls `compute` only on a miss.
	template <class F>
	int get_or_compute(const oracle_key& key, F&& compute) {
		{
			std::lock_guard<std::mutex> lg(mtx_);
			auto it = map_.find(key);
			if (it != map_.end()) return it->second;
		}
		int v = compute();
		std::lock_guard<std::mutex> lg(mtx_);
		map_.emplace(key, v);
		return v;
	}
	/// @brief Number of cached entries.
	size_t size() const {
		std::lock_guard<std::mutex> lg(mtx_);
		return map_.size();
	}
	/// @brief Remove every cached entry.
	void clear() {
		std::lock_guard<std::mutex> lg(mtx_);
		map_.clear();
	}
private:
	mutable std::mutex mtx_;
	std::unordered_map<oracle_key, int, oracle_key_hash> map_;
};

/**
 * @brief Compute a key by hashing a vector of atomic-query results.
 *
 * Each atomic query is a small integer (e.g., sign of a comparison).
 * @param atomic_results The queried batch.
 * @return The key (FNV-1a hash plus the batch itself).
 */
inline oracle_key make_oracle_key(const std::vector<int>& atomic_results) {
	uint64_t h = 1469598103934665603ull; // FNV-1a offset basis
	for (int r : atomic_results) {
		h ^= static_cast<uint64_t>(r);
		h *= 1099511628211ull; // FNV-1a prime
	}
	return oracle_key{h, atomic_results};
}

} // namespace idni::tau_lang::omcat

#endif // __IDNI__TAU__OMCAT_ORACLE_CACHE_H__
