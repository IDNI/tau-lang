// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

using cache_t = std::map<std::pair<tref, tref>, bool,
	subtree_pair_less<node_t, tref>>;
using tuple_cache_t = std::map<std::tuple<tref, bool, bool>, tref,
	subtree_bool_bool_tuple_less<node_t>>;

struct nodes_fixture {
	using node = node_t;
	tref a, b, c, d;
	std::pair<tref, tref> a_pair, b_pair, c_pair, d_pair;
	std::tuple<tref, bool, bool> a_tuple, b_tuple, c_tuple, d_tuple;
	nodes_fixture() {
		a = tau::build_bf_eq(tau::build_variable("a", 0));
		b = tau::build_bf_eq(tau::build_variable("b", 0));
		c = tau::build_bf_eq(tau::build_variable("c", 0));
		d = tau::build_bf_eq(tau::build_variable("d", 0));
		a_pair = std::make_pair(a, tau::_T());
		b_pair = std::make_pair(b, tau::_F());
		c_pair = std::make_pair(c, tau::_T());
		d_pair = std::make_pair(d, tau::_F());
		a_tuple = std::make_tuple(a, false, true);
		b_tuple = std::make_tuple(b, true, false);
		c_tuple = std::make_tuple(c, false, true);
		d_tuple = std::make_tuple(d, true, false);
	}
	void emplace_fixtures(cache_t& cache) {
		cache.emplace(a_pair, true);
		cache.emplace(b_pair, false);
		cache.emplace(c_pair, false);
		cache.emplace(d_pair, true);
	}
	void emplace_fixtures(tuple_cache_t& cache) {
		cache.emplace(a_tuple, tau::_T());
		cache.emplace(b_tuple, tau::_F());
		cache.emplace(c_tuple, tau::_T());
		cache.emplace(d_tuple, tau::_F());
	}
	void print(const cache_t& cache) {
		for (const auto& [k, v] : cache) {
			TAU_LOG_INFO << "pair cache: [" << TAU_LOG_FM(k.first)
				<< ", " << TAU_LOG_FM(k.second)
				<< "] = " << v;
		}
	};
	void print(const tuple_cache_t& cache) {
		for (const auto& [k, v] : cache) {
			TAU_LOG_INFO << "tuple cache: [" << TAU_LOG_FM(get<0>(k))
				<< ", " << get<1>(k)
				<< ", " << get<2>(k)
				<< "] = " << TAU_LOG_FM(v);
		}
	};
};

TEST_SUITE("gc") {

	TEST_CASE_FIXTURE(nodes_fixture, "check fixtures") {
		static cache_t cache;
		emplace_fixtures(cache);
		print(cache);
		CHECK(cache.size() == 4);
		CHECK(cache.contains(a_pair));
		CHECK(cache.contains(b_pair));
		CHECK(cache.contains(c_pair));
		CHECK(cache.contains(d_pair));

		static tuple_cache_t tuple_cache;
		emplace_fixtures(tuple_cache);
		print(tuple_cache);
		CHECK(tuple_cache.size() == 4);
		CHECK(tuple_cache.contains(a_tuple));
		CHECK(tuple_cache.contains(b_tuple));
		CHECK(tuple_cache.contains(c_tuple));
		CHECK(tuple_cache.contains(d_tuple));
	}

	TEST_CASE_FIXTURE(nodes_fixture, "cache rebuild - proof of concept") {
		std::unordered_set<tref> keep{};
		static cache_t cache;

		emplace_fixtures(cache);

		TAU_LOG_INFO << "Keep a by creating a handle";
		htref ha = tau::geth(a);

		TAU_LOG_INFO << "Keep b by inserting it into the keep set";
		keep.insert(b);

		TAU_LOG_INFO << "GC", tau::gc(keep);

		TAU_LOG_INFO << "Rebuild cache";
		auto rebuild_cache = [&]() {
			std::map<std::pair<tref, tref>, bool,
				subtree_pair_less<node_t, tref>> new_cache;
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
		CHECK(cache.contains(a_pair));
		CHECK(cache.contains(b_pair));
		CHECK(!cache.contains(c_pair));
		CHECK(!cache.contains(d_pair));
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

		emplace_fixtures(cache);

		TAU_LOG_INFO << "Keep a by creating a handle";
		htref ha = tau::geth(a);

		TAU_LOG_INFO << "Keep b by inserting it into the keep set";
		keep.insert(b);

		TAU_LOG_INFO << "GC", tau::gc(keep);

		print(cache);

		CHECK(cache.size() == 2);
		CHECK(cache.contains(a_pair));
		CHECK(cache.contains(b_pair));
		CHECK(!cache.contains(c_pair));
		CHECK(!cache.contains(d_pair));
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

		emplace_fixtures(cache);

		print(cache);

		TAU_LOG_INFO << "Keep a by creating a handle";
		htref ha = tau::geth(a);

		TAU_LOG_INFO << "Keep b by inserting it into the keep set";
		keep.insert(b);

		TAU_LOG_INFO << "GC", tau::gc(keep);

		print(cache);

		CHECK(cache.size() == 2);
		CHECK(cache.contains(a_pair));
		CHECK(cache.contains(b_pair));
		CHECK(!cache.contains(c_pair));
		CHECK(!cache.contains(d_pair));
	}

	TEST_CASE_FIXTURE(nodes_fixture, "cache rebuild - tau::create_cache<cache_t>()") {
		static cache_t& cache = tau::create_cache<cache_t>();

		emplace_fixtures(cache);

		TAU_LOG_INFO << "Keep a by creating a handle";
		htref ha = tau::geth(a);

		TAU_LOG_INFO << "Keep b by inserting it into the keep set";
		std::unordered_set<tref> keep{ b };

		TAU_LOG_INFO << "GC", tau::gc(keep);

		print(cache);

		CHECK(cache.size() == 2);
		CHECK(cache.contains(a_pair));
		CHECK(cache.contains(b_pair));
		CHECK(!cache.contains(c_pair));
		CHECK(!cache.contains(d_pair));
	}

	TEST_CASE_FIXTURE(nodes_fixture, "cache rebuild - tau::create_cache<tuple_cache_t>()") {
		static tuple_cache_t& cache = tau::create_cache<tuple_cache_t>();

		emplace_fixtures(cache);

		TAU_LOG_INFO << "Keep a by creating a handle";
		htref ha = tau::geth(a);

		TAU_LOG_INFO << "Keep b by inserting it into the keep set";
		std::unordered_set<tref> keep{ b };

		TAU_LOG_INFO << "GC", tau::gc(keep);

		print(cache);

		CHECK(cache.size() == 2);
		CHECK( cache.contains(a_tuple));
		CHECK( cache.contains(b_tuple));
		CHECK(!cache.contains(c_tuple));
		CHECK(!cache.contains(d_tuple));
	}

	TEST_CASE_FIXTURE(nodes_fixture, "cache rebuild - multiple caches of same type") {
		static cache_t& cache = tau::create_cache<cache_t>();
		static cache_t& cache2 = tau::create_cache<cache_t>();
		static cache_t& cache3 = tau::create_cache<cache_t>();

		emplace_fixtures(cache);
		emplace_fixtures(cache2);
		emplace_fixtures(cache3);

		TAU_LOG_INFO << "Keep a by creating a handle";
		htref ha = tau::geth(a);

		TAU_LOG_INFO << "Keep b by inserting it into the keep set";
		std::unordered_set<tref> keep{ b };

		TAU_LOG_INFO << "GC", tau::gc(keep);

		print(cache);
		print(cache2);
		print(cache3);

		CHECK( cache.size() == 2);
		CHECK( cache.contains(a_pair));
		CHECK( cache.contains(b_pair));
		CHECK(!cache.contains(c_pair));
		CHECK(!cache.contains(d_pair));
		CHECK( cache2.size() == 2);
		CHECK( cache2.contains(a_pair));
		CHECK( cache2.contains(b_pair));
		CHECK(!cache2.contains(c_pair));
		CHECK(!cache2.contains(d_pair));
		CHECK( cache3.size() == 2);
		CHECK( cache3.contains(a_pair));
		CHECK( cache3.contains(b_pair));
		CHECK(!cache3.contains(c_pair));
		CHECK(!cache3.contains(d_pair));
	}

}