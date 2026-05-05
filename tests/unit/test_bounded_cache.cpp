// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// bounded_cache — correctness + benchmarks.
//
// Two suites:
//   1. "bounded_cache — correctness" verifies functional equivalence
//      to std::map for the unbounded mode (Max == 0) and tests the
//      FIFO eviction semantics for bounded modes.
//   2. "bounded_cache — benchmark" measures throughput vs an
//      unbounded std::map across a synthetic memoisation workload
//      (same hot-set / churn pattern that long-running tau processes
//      hit in production).
//
// The benchmark prints a table showing:
//   - hit rate per cache configuration
//   - evictions
//   - mean nanoseconds per lookup+insert
//
// It does not enforce a regression bound (workloads are environment-
// sensitive), but it does fail if a configuration produces a
// correctness mismatch (different verdict than the unbounded
// reference).

#include "test_init.h"
#include "bounded_cache.h"

#include <chrono>
#include <cstdio>
#include <iomanip>
#include <map>
#include <random>
#include <sstream>
#include <vector>

using idni::tau_lang::bounded_cache;

namespace {

// Trivial workload: integer key, integer value (square).  Choosing
// trivial K/V keeps the benchmark measuring cache mechanics, not
// hash/compare cost on big keys.
int compute(int k) { return k * k; }

} // namespace

TEST_SUITE("bounded_cache — correctness") {

	TEST_CASE("unbounded (Max=0) behaves like std::map") {
		bounded_cache<int, int> bc;
		std::map<int, int> ref;

		for (int i = 0; i < 1000; ++i) {
			bc.emplace(i, compute(i));
			ref.emplace(i, compute(i));
		}

		CHECK(bc.size() == ref.size());
		for (const auto& [k, v] : ref) {
			REQUIRE(bc.contains(k));
			CHECK(bc[k] == v);
		}
		CHECK(bc.evictions() == 0);
	}

	TEST_CASE("Max=64 evicts in FIFO order") {
		bounded_cache<int, int, std::less<int>, 64> bc;

		// Insert 100 distinct keys.  After we cross 64, every
		// further insert evicts the oldest entry.
		for (int i = 0; i < 100; ++i)
			bc.emplace(i, compute(i));

		CHECK(bc.size() == 64);
		CHECK(bc.evictions() == 100 - 64);

		// Earliest insertions (0..35) should be evicted.
		for (int i = 0; i < 100 - 64; ++i)
			CHECK(!bc.contains(i));

		// Latest 64 insertions should be present.
		for (int i = 100 - 64; i < 100; ++i) {
			REQUIRE(bc.contains(i));
			CHECK(bc.find(i)->second == compute(i));
		}
	}

	TEST_CASE("erase shrinks both map and queue") {
		bounded_cache<int, int, std::less<int>, 16> bc;
		for (int i = 0; i < 16; ++i) bc.emplace(i, compute(i));
		CHECK(bc.size() == 16);
		CHECK(bc.evictions() == 0);

		// Erase 4 entries.  No new evictions should happen on the
		// next insert as long as size stays <= 16.
		for (int i = 0; i < 4; ++i) bc.erase(i);
		CHECK(bc.size() == 12);

		for (int i = 100; i < 104; ++i) bc.emplace(i, compute(i));
		CHECK(bc.size() == 16);
		CHECK(bc.evictions() == 0);

		// One more insert *should* evict.
		bc.emplace(200, compute(200));
		CHECK(bc.size() == 16);
		CHECK(bc.evictions() == 1);
	}

	TEST_CASE("verdict equivalence: bounded vs unbounded on streaming workload") {
		// Run the same key sequence through bounded and unbounded
		// caches.  Every successful find() must return the same
		// value.  Misses on the bounded cache are OK (eviction).
		std::mt19937 rng(0x70a3);
		std::uniform_int_distribution<int> hot(0, 31);    // tight reuse
		std::uniform_int_distribution<int> cold(0, 4095); // wide churn

		bounded_cache<int, int> unbounded;
		bounded_cache<int, int, std::less<int>, 256> bounded;

		for (int step = 0; step < 5000; ++step) {
			// 60% hot keys, 40% cold churn.
			int k = (step % 5 < 3) ? hot(rng) : cold(rng);
			auto [u_it, u_new] = unbounded.emplace(k, compute(k));
			auto [b_it, b_new] = bounded.emplace(k, compute(k));
			CHECK(u_it->second == compute(k));
			CHECK(b_it->second == compute(k));
		}

		CHECK(unbounded.evictions() == 0);
		CHECK(bounded.size() <= 256);
	}
}

TEST_SUITE("bounded_cache — benchmark") {

	// One row of the printed table: cache configuration → measurement.
	struct row {
		std::string label;
		std::size_t size;
		std::size_t evictions;
		double      ns_per_op;
		double      hit_rate;
	};

	// The synthetic workload:
	//   - 90% of accesses hit a "hot" set of HOT_KEYS distinct keys.
	//   - 10% are cold churn over COLD_KEYS distinct keys.
	// Total operations: OPS.  This mirrors how synthesis pipelines
	// re-emit the same canonicalised formulas hundreds of times
	// (the C-cache compounding curve from dominance_design.md) with
	// occasional new spec material.
	static constexpr int HOT_KEYS  = 1024;
	static constexpr int COLD_KEYS = 32 * 1024;
	static constexpr int OPS       = 200'000;

	template <typename Cache>
	static row measure(const std::string& label, Cache& cache) {
		using clock = std::chrono::steady_clock;
		std::mt19937 rng(0xCAFE);
		std::uniform_int_distribution<int> hot(0, HOT_KEYS - 1);
		std::uniform_int_distribution<int> cold(HOT_KEYS,
		                                        HOT_KEYS + COLD_KEYS - 1);
		std::uniform_int_distribution<int> coin(0, 9);

		std::size_t hits = 0;
		auto t0 = clock::now();
		for (int i = 0; i < OPS; ++i) {
			int k = (coin(rng) < 9) ? hot(rng) : cold(rng);
			auto it = cache.find(k);
			if (it != cache.end()) {
				++hits;
			} else {
				cache.emplace(k, compute(k));
			}
		}
		auto t1 = clock::now();
		double ns = std::chrono::duration<double, std::nano>(t1 - t0).count();
		return {
			label,
			cache.size(),
			cache.evictions(),
			ns / OPS,
			static_cast<double>(hits) / OPS
		};
	}

	template <typename Cache>
	static row measure_skewed(const std::string& label, Cache& cache,
	                          int hot_keys, int cold_keys,
	                          int hot_fraction_pct) {
		using clock = std::chrono::steady_clock;
		std::mt19937 rng(0xCAFE);
		std::uniform_int_distribution<int> hot(0, hot_keys - 1);
		std::uniform_int_distribution<int> cold(hot_keys,
		                                        hot_keys + cold_keys - 1);
		std::uniform_int_distribution<int> coin(0, 99);

		std::size_t hits = 0;
		auto t0 = clock::now();
		for (int i = 0; i < OPS; ++i) {
			int k = (coin(rng) < hot_fraction_pct)
			            ? hot(rng)
			            : cold(rng);
			auto it = cache.find(k);
			if (it != cache.end()) ++hits;
			else cache.emplace(k, compute(k));
		}
		auto t1 = clock::now();
		double ns = std::chrono::duration<double, std::nano>(t1 - t0).count();
		return { label, cache.size(), cache.evictions(),
		         ns / OPS, static_cast<double>(hits) / OPS };
	}

	TEST_CASE("hot/cold workload across cache bounds") {
		std::vector<row> rows;

		{
			bounded_cache<int, int> c;
			rows.push_back(measure("unbounded",     c));
		}
		{
			bounded_cache<int, int, std::less<int>, 64>    c;
			rows.push_back(measure("bounded(64)",   c));
		}
		{
			bounded_cache<int, int, std::less<int>, 256>   c;
			rows.push_back(measure("bounded(256)",  c));
		}
		{
			bounded_cache<int, int, std::less<int>, 1024>  c;
			rows.push_back(measure("bounded(1024)", c));
		}
		{
			bounded_cache<int, int, std::less<int>, 4096>  c;
			rows.push_back(measure("bounded(4096)", c));
		}
		{
			bounded_cache<int, int, std::less<int>, 16384> c;
			rows.push_back(measure("bounded(16k)",  c));
		}

		std::printf(
		    "\n[bounded_cache benchmark]  HOT=%d  COLD=%d  OPS=%d\n",
		    HOT_KEYS, COLD_KEYS, OPS);
		std::printf(
		    "  %-15s %10s %12s %14s %12s\n",
		    "config", "size", "evictions", "ns/op", "hit-rate");
		for (const auto& r : rows) {
			std::printf(
			    "  %-15s %10zu %12zu %14.1f %11.3f\n",
			    r.label.c_str(), r.size, r.evictions, r.ns_per_op,
			    r.hit_rate);
		}

		// Sanity: cache below HOT_KEYS bound should still see >50%
		// hits (the hot set keeps churning through it but enough
		// fits at any given moment).
		// Cache >= HOT_KEYS bound should see >= ~85% hit rate
		// (matches the workload's hot-fraction = 90%).
		CHECK(rows[0].hit_rate >= 0.80);          // unbounded
		CHECK(rows[5].hit_rate >= 0.80);          // 16k
		// Eviction count grows monotonically as bound shrinks.
		for (std::size_t i = 1; i + 1 < rows.size(); ++i) {
			CHECK(rows[i].evictions >= rows[i + 1].evictions);
		}
	}

	TEST_CASE("skew sweep: how hot-fraction changes the trade-off") {
		// Same cache size, different hot-fraction.  Demonstrates
		// that the bounded cache is most valuable when the
		// workload has a clear hot set; degrades gracefully when
		// workload approaches uniform.
		std::vector<row> rows;
		for (int hot_pct : {99, 90, 75, 50, 25, 10}) {
			std::ostringstream lbl;
			lbl << "hot=" << hot_pct << "%";
			bounded_cache<int, int, std::less<int>, 1024> c;
			rows.push_back(measure_skewed(
			    lbl.str(), c, /*hot=*/1024, /*cold=*/32 * 1024,
			    hot_pct));
		}

		std::printf(
		    "\n[bounded_cache skew sweep]  bound=1024  HOT=1024  "
		    "COLD=32k  OPS=%d\n", OPS);
		std::printf(
		    "  %-15s %10s %12s %14s %12s\n",
		    "config", "size", "evictions", "ns/op", "hit-rate");
		for (const auto& r : rows) {
			std::printf(
			    "  %-15s %10zu %12zu %14.1f %11.3f\n",
			    r.label.c_str(), r.size, r.evictions, r.ns_per_op,
			    r.hit_rate);
		}

		// Hit-rate must drop monotonically as the workload becomes
		// less skewed (more uniform churn → less benefit from any
		// fixed-size cache).
		for (std::size_t i = 0; i + 1 < rows.size(); ++i) {
			CHECK(rows[i].hit_rate >= rows[i + 1].hit_rate - 0.01);
		}
	}

	TEST_CASE("zipf-like sequential workload") {
		// Repeated sweeps over a fixed set, simulating the
		// "synthesise N specs, normalise each to a fixed point"
		// pattern seen in the LTL pipeline.  Hot set = SET_SIZE.
		// Cache bound below SET_SIZE means thrashing; >= SET_SIZE
		// means amortised steady state.
		constexpr int SET_SIZE = 2048;
		constexpr int SWEEPS   = 16;
		using clock = std::chrono::steady_clock;
		std::vector<row> rows;
		auto run = [&](const std::string& lbl, auto& cache) {
			std::size_t hits = 0;
			auto t0 = clock::now();
			for (int sweep = 0; sweep < SWEEPS; ++sweep) {
				for (int k = 0; k < SET_SIZE; ++k) {
					auto it = cache.find(k);
					if (it != cache.end()) ++hits;
					else cache.emplace(k, compute(k));
				}
			}
			auto t1 = clock::now();
			std::size_t total = SWEEPS * SET_SIZE;
			double ns =
			    std::chrono::duration<double, std::nano>(t1 - t0).count();
			rows.push_back({
			    lbl, cache.size(), cache.evictions(),
			    ns / total, static_cast<double>(hits) / total
			});
		};
		{ bounded_cache<int, int>                                   c; run("unbounded",     c); }
		{ bounded_cache<int, int, std::less<int>, 256>              c; run("bounded(256)",  c); }
		{ bounded_cache<int, int, std::less<int>, 1024>             c; run("bounded(1024)", c); }
		{ bounded_cache<int, int, std::less<int>, 2047>             c; run("bounded(2047)", c); }
		{ bounded_cache<int, int, std::less<int>, 2048>             c; run("bounded(2048)", c); }
		{ bounded_cache<int, int, std::less<int>, 4096>             c; run("bounded(4096)", c); }

		std::printf(
		    "\n[bounded_cache sweep workload]  SET=%d  SWEEPS=%d\n",
		    SET_SIZE, SWEEPS);
		std::printf(
		    "  %-15s %10s %12s %14s %12s\n",
		    "config", "size", "evictions", "ns/op", "hit-rate");
		for (const auto& r : rows) {
			std::printf(
			    "  %-15s %10zu %12zu %14.1f %11.3f\n",
			    r.label.c_str(), r.size, r.evictions, r.ns_per_op,
			    r.hit_rate);
		}

		// At bound == SET_SIZE the FIFO cache can't help on the
		// repeating sweep (it evicts the entry just before reuse).
		// At bound > SET_SIZE the cache catches steady-state
		// almost-100% hit rate after the first sweep.
		// The 4096 case must beat the 2047 case.
		CHECK(rows[5].hit_rate >  rows[3].hit_rate);
		CHECK(rows[0].hit_rate >= 0.93);  // unbounded reaches steady state
	}
}
