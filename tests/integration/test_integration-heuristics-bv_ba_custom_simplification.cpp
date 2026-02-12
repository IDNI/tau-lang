// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}

	TEST_CASE("logging") {
		logging::trace();
	}
}

tref parse_bf(const std::string& sample) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::bf }};
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

TEST_SUITE("ba bv custom simplification") {

	TEST_CASE("1 + 2") {
		const char* sample = "{1}:bv[64] + {2}:bv[64]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_custom_simplification<node_t>(src);
		TAU_LOG_TRACE << "simplified: " << tree<node_t>::get(simplified).tree_to_str() << "\n";
		CHECK( true );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
