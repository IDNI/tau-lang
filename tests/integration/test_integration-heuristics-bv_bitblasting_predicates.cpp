// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"
//#include "heuristics/bv_bitblasting_predicates.tmpl.h"

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

/*TEST_SUITE("bitblasting") {

	TEST_CASE("bvmul_rec_rule") {
	}

	TEST_CASE("bvlt_rule(s)") {
		using node = node_t;

		TAU_LOG_INFO << "bvlt_rules:\n";
		auto rules = bvlt_rules<node>(4);
		for (const auto& rule : rules)
			TAU_LOG_INFO << LOG_RULE(rule) << '\n';
		TAU_LOG_INFO << "---" << '\n';
		TAU_LOG_INFO << LOG_RULE(bvlt_rule<node>(4)) << "\n\n";
	}

	TEST_CASE("bvgt_rule") {
		using node = node_t;

		TAU_LOG_INFO << "bvgt_rules:\n";
		auto rules = bvgt_rules<node>(4);
		for (const auto& rule : rules)
			TAU_LOG_INFO << LOG_RULE(rule) << '\n';
		TAU_LOG_INFO << "---" << '\n';
		TAU_LOG_INFO << LOG_RULE(bvgt_rule<node>(4)) << "\n\n";
	}

	TEST_CASE("bvshl_by_one_rule") {
		using node = node_t;

		TAU_LOG_INFO << "bvshl_by_one_rules:\n";
		TAU_LOG_INFO << LOG_RULE(bvshl_by_one_rule<node>(4)) << "\n\n";
	}

	TEST_CASE("bvrhl_by_one_rule") {
		using node = node_t;

		TAU_LOG_INFO << "bvrhl_by_one_rule:\n";
		TAU_LOG_INFO << LOG_RULE(bvrhl_by_one_rule<node>(4)) << "\n\n";
	}

	TEST_CASE("is_bit_zero_rule") {
		using node = node_t;

		TAU_LOG_INFO << "is_bit_zero_rule:\n";
		TAU_LOG_INFO << LOG_RULE(is_bit_zero_rule<node>(2, 4)) << "\n\n";
	}

	TEST_CASE("is_bit_one_rule") {
		using node = node_t;

		TAU_LOG_INFO << "is_bit_one_rule:\n";
		TAU_LOG_INFO << LOG_RULE(is_bit_one_rule<node>(2, 4)) << "\n\n";
	}

	TEST_CASE("bit_rule") {
		using node = node_t;

		TAU_LOG_INFO << "bit_rule:\n";
		TAU_LOG_INFO << LOG_RULE(bit_rule<node>(1, 2)) << "\n\n";
	}

	TEST_CASE("bvadd_rule") {
		//using node = node_t;

		//TAU_LOG_INFO << LOG_RULE(bvadd_rule<node>(4));
	}

	TEST_CASE("bvsub_rule") {
		//using node = node_t;

		//TAU_LOG_INFO << LOG_RULE(bvsub_rule<node>(4));
	}

	TEST_CASE("bvdiv_rule") {
		//using node = node_t;

		//TAU_LOG_INFO << LOG_RULE(bvdiv_rule<node>(2, 2));
	}

	TEST_CASE("bvmod_rule") {
		//using node = node_t;

		//TAU_LOG_INFO << LOG_RULE(bvmod_rule<node>(2, 2));
	}

	TEST_CASE("bvshl_rule") {
		//using node = node_t;

		//TAU_LOG_INFO << LOG_RULE(bvshl_rule<node>(2, 2));
	}

	TEST_CASE("bvrhl_rule") {
		//using node = node_t;

		//TAU_LOG_INFO << LOG_RULE(bvrhl_rule<node>(2, 2));
	}
}*/

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
