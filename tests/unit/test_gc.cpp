// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"


TEST_SUITE("gc") {
	TEST_CASE("cache rebuild - proof of concept") {
		using node = node_t;

		tref a, b, c, d;
		std::unordered_set<tref> keep{};
		static std::map<std::pair<tref, tref>, bool,
				subtree_pair_less<node, tref>> cache;

		a = tau::build_bf_eq(tau::build_variable("a", 0));
		b = tau::build_bf_eq(tau::build_variable("b", 0));
		c = tau::build_bf_eq(tau::build_variable("c", 0));
		d = tau::build_bf_eq(tau::build_variable("d", 0));

		TAU_LOG_INFO << "a: " << TAU_LOG_FM(a);
		TAU_LOG_INFO << "b: " << TAU_LOG_FM(b);
		TAU_LOG_INFO << "c: " << TAU_LOG_FM(c);
		TAU_LOG_INFO << "d: " << TAU_LOG_FM(d);

		cache.emplace(std::make_pair(a, tau::_T()), true);
		cache.emplace(std::make_pair(b, tau::_F()), false);
		cache.emplace(std::make_pair(c, tau::_T()), false);
		cache.emplace(std::make_pair(d, tau::_F()), true);

		auto print_cache = [&]() {
			for (auto& [k, v] : cache) {
				TAU_LOG_INFO << "cache: " << TAU_LOG_FM(k.first)
					<< " / " << TAU_LOG_FM(k.second)
					<< " = " << v;
			}
		};
		print_cache();

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

		print_cache();

		CHECK(cache.size() == 2);
		CHECK(cache.contains(std::make_pair(a, tau::_T())));
		CHECK(cache.contains(std::make_pair(b, tau::_F())));
		CHECK(!cache.contains(std::make_pair(c, tau::_T())));
		CHECK(!cache.contains(std::make_pair(d, tau::_F())));
	}
}