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

// Key: a hashable identifier for an atomic-query batch.  The
// discriminator encodes which atomic relations were queried; the
// result is the type index (int).
struct OracleKey {
	uint64_t hash;
	bool operator==(const OracleKey& o) const { return hash == o.hash; }
};
struct OracleKeyHash {
	size_t operator()(const OracleKey& k) const noexcept {
		return static_cast<size_t>(k.hash);
	}
};

class OracleCache {
public:
	// Returns the cached type index for `key`, or -1 if not present.
	int get(const OracleKey& key) const {
		std::lock_guard<std::mutex> lg(mtx_);
		auto it = map_.find(key);
		return it == map_.end() ? -1 : it->second;
	}
	// Stores the result; overwrites existing entry.
	void put(const OracleKey& key, int value) {
		std::lock_guard<std::mutex> lg(mtx_);
		map_[key] = value;
	}
	// Compute-and-cache: calls `compute` only on a miss.
	template <class F>
	int get_or_compute(const OracleKey& key, F&& compute) {
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
	size_t size() const {
		std::lock_guard<std::mutex> lg(mtx_);
		return map_.size();
	}
	void clear() {
		std::lock_guard<std::mutex> lg(mtx_);
		map_.clear();
	}
private:
	mutable std::mutex mtx_;
	std::unordered_map<OracleKey, int, OracleKeyHash> map_;
};

// Compute a key by hashing a vector of atomic-query results.  Each atomic
// query is a small integer (e.g., sign of a comparison).
inline OracleKey make_oracle_key(const std::vector<int>& atomic_results) {
	uint64_t h = 1469598103934665603ull; // FNV-1a offset basis
	for (int r : atomic_results) {
		h ^= static_cast<uint64_t>(r);
		h *= 1099511628211ull; // FNV-1a prime
	}
	return OracleKey{h};
}

} // namespace idni::tau_lang::omcat

#endif // __IDNI__TAU__OMCAT_ORACLE_CACHE_H__
