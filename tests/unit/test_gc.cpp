// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

struct nodes_fixture {
	tref a, b, c, d;
	nodes_fixture() {
		a = tau::build_bf_eq(tau::build_variable("a", 0));
		b = tau::build_bf_eq(tau::build_variable("b", 0));
		c = tau::build_bf_eq(tau::build_variable("c", 0));
		d = tau::build_bf_eq(tau::build_variable("d", 0));
	}
};

TEST_SUITE("gc") {
	using node = node_t;
	using cache_t = std::map<std::pair<tref, tref>, bool,
		subtree_pair_less<node, tref>>;

	void print(const cache_t& cache) {
		for (const auto& [k, v] : cache) {
			TAU_LOG_INFO << "cache: " << TAU_LOG_FM(k.first)
				<< " / " << TAU_LOG_FM(k.second)
				<< " = " << v;
		}
	};

	TEST_CASE_FIXTURE(nodes_fixture, "cache rebuild - proof of concept") {
		using node = node_t;

		std::unordered_set<tref> keep{};
		static cache_t cache;

		cache.emplace(std::make_pair(a, tau::_T()), true);
		cache.emplace(std::make_pair(b, tau::_F()), false);
		cache.emplace(std::make_pair(c, tau::_T()), false);
		cache.emplace(std::make_pair(d, tau::_F()), true);

		print(cache);

		TAU_LOG_INFO << "Keep a by creating a handle";
		htref ha = tau::geth(a);

		TAU_LOG_INFO << "Keep b by inserting it into the keep set";
		keep.insert(b);

		TAU_LOG_INFO << "GC", tau::gc(keep);

		TAU_LOG_INFO << "Rebuild cache";
		auto rebuild_cache = [&]() {
			std::map<std::pair<tref, tref>, bool,
				subtree_pair_less<node, tref>> new_cache;
			for (auto it = cache.begin(); it != cache.end(); it++) {
				if (keep.contains(it->first.first)
					&& keep.contains(it->first.second))
						new_cache.emplace(*it);
			}
			cache = std::move(new_cache);
		};
		rebuild_cache();

		print(cache);

		CHECK(cache.size() == 2);
		CHECK(cache.contains(std::make_pair(a, tau::_T())));
		CHECK(cache.contains(std::make_pair(b, tau::_F())));
		CHECK(!cache.contains(std::make_pair(c, tau::_T())));
		CHECK(!cache.contains(std::make_pair(d, tau::_F())));
	}

	TEST_CASE_FIXTURE(nodes_fixture, "cache rebuild - callbacks") {
		std::unordered_set<tref> keep{};
		static cache_t cache;
		cache_t* pcache = &cache;

		tau::gc_callbacks.push_back(
			[pcache](const std::unordered_set<tref>& keep)
		{
			cache_t& cache = *pcache;
			cache_t new_cache;
			for (auto it = cache.begin(); it != cache.end(); it++) {
				if (keep.contains(it->first.first)
					&& keep.contains(it->first.second))
						new_cache.emplace(*it);
			}
			cache = std::move(new_cache);
		});
				
		cache.emplace(std::make_pair(a, tau::_T()), true);
		cache.emplace(std::make_pair(b, tau::_F()), false);
		cache.emplace(std::make_pair(c, tau::_T()), false);
		cache.emplace(std::make_pair(d, tau::_F()), true);

		print(cache);

		TAU_LOG_INFO << "Keep a by creating a handle";
		htref ha = tau::geth(a);

		TAU_LOG_INFO << "Keep b by inserting it into the keep set";
		keep.insert(b);

		TAU_LOG_INFO << "GC", tau::gc(keep);

		print(cache);

		CHECK(cache.size() == 2);
		CHECK(cache.contains(std::make_pair(a, tau::_T())));
		CHECK(cache.contains(std::make_pair(b, tau::_F())));
		CHECK(!cache.contains(std::make_pair(c, tau::_T())));
		CHECK(!cache.contains(std::make_pair(d, tau::_F())));
	}

	cache_t& get_cache() {
		static cache_t cache;
		static bool initialized = false;
		if (initialized) return cache;
		initialized = true;
		tau::gc_callbacks.push_back(
			[](const std::unordered_set<tref>& keep)
		{
			cache_t new_cache;
			cache_t& cache = get_cache();
			for (auto it = cache.begin(); it != cache.end(); it++) {
				if (keep.contains(it->first.first)
					&& keep.contains(it->first.second))
						new_cache.emplace(*it);
			}
			cache = std::move(new_cache);
		});
		return cache;
	}

	TEST_CASE_FIXTURE(nodes_fixture, "cache rebuild - get_cache()") {
		cache_t& cache = get_cache();
		std::unordered_set<tref> keep{};

		cache.emplace(std::make_pair(a, tau::_T()), true);
		cache.emplace(std::make_pair(b, tau::_F()), false);
		cache.emplace(std::make_pair(c, tau::_T()), false);
		cache.emplace(std::make_pair(d, tau::_F()), true);

		print(cache);

		TAU_LOG_INFO << "Keep a by creating a handle";
		htref ha = tau::geth(a);

		TAU_LOG_INFO << "Keep b by inserting it into the keep set";
		keep.insert(b);

		TAU_LOG_INFO << "GC", tau::gc(keep);

		print(cache);

		CHECK(cache.size() == 2);
		CHECK(cache.contains(std::make_pair(a, tau::_T())));
		CHECK(cache.contains(std::make_pair(b, tau::_F())));
		CHECK(!cache.contains(std::make_pair(c, tau::_T())));
		CHECK(!cache.contains(std::make_pair(d, tau::_F())));
	}

}