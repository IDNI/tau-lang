// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"
#include "heuristics/bv_bitblasting_predicates.tmpl.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}

	TEST_CASE("logging") {
		logging::trace();
	}
}

tref parse_wff(const std::string& sample) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::wff }};
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

TEST_SUITE("bitblasting") {

	/*TEST_CASE("print bvshl predicates") {
		using node = node_t;

		for(auto rule : bv_bitblasting_rules<node_t>::bvshl_by_one(2)) {
			TAU_LOG_INFO << LOG_RULE(rule);
		}

		CHECK(true);
	}

	TEST_CASE("print bvadd predicates") {
		using node = node_t;
		for(auto rule : bv_bitblasting_rules<node_t>::bvadd(2)) {
			TAU_LOG_INFO << LOG_RULE(rule);
		}

		CHECK(true);
	}

	TEST_CASE("print bvsub predicates") {
		using node = node_t;
		for(auto rule : bv_bitblasting_rules<node_t>::bvsub(2)) {
			TAU_LOG_INFO << LOG_RULE(rule);
		}

		CHECK(true);
	}*/

}
TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
