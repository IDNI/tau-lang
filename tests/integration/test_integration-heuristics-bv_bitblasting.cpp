// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"
#include "heuristics/bv_bitblasting.tmpl.h"

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

TEST_SUITE("bv blasting") {

	TEST_CASE("x | y = 0") {
		logging::trace();
		tref src = parse_wff("x:bv[64] | y = 0");
		auto solution = bv_bitblasting_solve<node_t>(src);
		if (solution) {
			TAU_LOG_INFO << "Solution found for: " << src;
			for (const auto& [var, val] : solution.value()) {
				TAU_LOG_INFO << "  " << var << " -> " << val;
			}
		} else {
			TAU_LOG_INFO << "No solution found for: " << src;
		}
		logging::info();
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
