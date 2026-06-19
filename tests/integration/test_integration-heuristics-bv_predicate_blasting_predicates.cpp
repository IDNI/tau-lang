// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"
#include "heuristics/bv_predicate_blasting.h"

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

	TEST_CASE("bvlt_rule(s)") {
		using node = node_t;

		TAU_LOG_INFO << "bvlt_rules:\n";
		auto rules = bvlt_rules<node>(2);
		for (const auto& rule : rules)
			TAU_LOG_INFO << TAU_LOG_RULE(rule) << "\n";
		TAU_LOG_INFO << "---" << "\n";
		TAU_LOG_INFO << TAU_LOG_RULE(bvlt_rule<node>(2)) << "\n";
		TAU_LOG_INFO << "------" << "\n";

	}

	TEST_CASE("bvgt_rule") {
		using node = node_t;

		TAU_LOG_INFO << "bvgt_rules:\n";
		auto rules = bvgt_rules<node>(2);
		for (const auto& rule : rules)
			TAU_LOG_INFO << TAU_LOG_RULE(rule) << "\n";
		TAU_LOG_INFO << "---" << "\n";
		TAU_LOG_INFO << TAU_LOG_RULE(bvgt_rule<node>(2)) << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bvshl_by_one_rule") {
		using node = node_t;

		TAU_LOG_INFO << "bvshl_by_one_rules:\n";
		TAU_LOG_INFO << TAU_LOG_RULE(bvshl_by_one_rule<node>(2)) << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bvrhl_by_one_rule") {
		using node = node_t;

		TAU_LOG_INFO << "bvrhl_by_one_rule:\n";
		TAU_LOG_INFO << TAU_LOG_RULE(bvrhl_by_one_rule<node>(2)) << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("is_bit_zero_rule") {
		using node = node_t;

		TAU_LOG_INFO << "is_bit_zero_rule:\n";
		TAU_LOG_INFO << TAU_LOG_RULE(is_bit_zero_rule<node>(1, 2)) << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("is_bit_one_rule") {
		using node = node_t;

		TAU_LOG_INFO << "is_bit_one_rule:\n";
		TAU_LOG_INFO << TAU_LOG_RULE(is_bit_one_rule<node>(1, 2)) << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bit_rule") {
		using node = node_t;

		TAU_LOG_INFO << "bit_rule:\n";
		TAU_LOG_INFO << TAU_LOG_RULE(bit_rule<node>(1, 2)) << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bvadd") {
		using node = node_t;
		using tau = tree<node>;

		trefs aux;
		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		auto z = tau::build_bf_variable(bv_type_id<node>(4));
		TAU_LOG_INFO << "bvadd:\n";
		TAU_LOG_INFO << tau::get(bvadd<node>(x, y, z, aux)).to_str() << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bvsub") {
		using node = node_t;
		using tau = tree<node>;

		trefs aux;
		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		auto z = tau::build_bf_variable(bv_type_id<node>(4));
		TAU_LOG_INFO << "bvsub:\n";
		TAU_LOG_INFO << tau::get(bvsub<node>(x, y, z, aux)).to_str() << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bvshl_rule") {
		using node = node_t;
		using tau = tree<node>;

		TAU_LOG_INFO << "bvshl_rule:\n";
		TAU_LOG_INFO << TAU_LOG_RULE(bvshl_rule<node>(tau::get(tau::bf, tau::get_bv_constant(4, 1)))) << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bvrhl_rule") {
		using node = node_t;
		using tau = tree<node>;

		TAU_LOG_INFO << "bvrhl_rule:\n";
		TAU_LOG_INFO << TAU_LOG_RULE(bvrhl_rule<node>(tau::get(tau::bf, tau::get_bv_constant(4, 1)))) << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bvmul") {
		using node = node_t;
		using tau = tree<node>;

		trefs aux;
		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto z = tau::build_bf_variable(bv_type_id<node>(4));
		auto c = tau::get(tau::bf, tau::get_bv_constant(4, 2));
		TAU_LOG_INFO << "bvmul:\n";
		TAU_LOG_INFO << tau::get(bvmul<node>(x, c, z, aux)).to_str() << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bvdiv") {
		using node = node_t;
		using tau = tree<node>;

		trefs aux;
		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto q = tau::build_bf_variable(bv_type_id<node>(4));
		auto c = tau::get(tau::bf, tau::get_bv_constant(4, 2));
		TAU_LOG_INFO << "bvdiv:\n";
		TAU_LOG_INFO << tau::get(bvdiv<node>(x, c, q, aux)).to_str() << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bvmod") {
		using node = node_t;
		using tau = tree<node>;

		trefs aux;
		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto r = tau::build_bf_variable(bv_type_id<node>(4));
		auto c = tau::get(tau::bf, tau::get_bv_constant(4, 2));
		TAU_LOG_INFO << "bvmod:\n";
		TAU_LOG_INFO << tau::get(bvmod<node>(x, c, r, aux)).to_str() << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bved") {
		using node = node_t;
		using tau = tree<node>;

		trefs aux;
		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto q = tau::build_bf_variable(bv_type_id<node>(4));
		auto r = tau::build_bf_variable(bv_type_id<node>(4));
		auto c = tau::get(tau::bf, tau::get_bv_constant(4, 2));
		TAU_LOG_INFO << "bved:\n";
		TAU_LOG_INFO << tau::get(bved<node>(x, c, q, r, aux)).to_str() << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}

	TEST_CASE("bvneq_rule") {
		using node = node_t;

		TAU_LOG_INFO << "bvneq_rules:\n";
		auto rules = bvneq_rules<node>(2);
		for (const auto& rule : rules)
			TAU_LOG_INFO << TAU_LOG_RULE(rule) << "\n";
		TAU_LOG_INFO << "---" << "\n";
		TAU_LOG_INFO << TAU_LOG_RULE(bvneq_rule<node>(2)) << "\n";
		TAU_LOG_INFO << "------" << "\n";
	}
}

TEST_SUITE("bvcast") {

	TEST_CASE("bvcast: zero-extension (bv[2] -> bv[4])") {
		using node = node_t;
		using tau = tree<node>;
		auto src    = tau::build_bf_variable(bv_type_id<node>(2));
		auto result = tau::build_bf_variable(bv_type_id<node>(4));
		// Expanded bit-equality + zero predicates for a 2->4 zero-extension
		auto src_b0_zero = tau::build_bf_eq_0(bit<node>(src, 0));
		auto res_b0_zero = tau::build_bf_eq_0(bit<node>(result, 0));
		auto src_b1_zero = tau::build_bf_eq_0(bit<node>(src, 1));
		auto res_b1_zero = tau::build_bf_eq_0(bit<node>(result, 1));
		TAU_LOG_INFO << "bvcast zext bv[2]->bv[4]: bit-equality predicates\n";
		TAU_LOG_INFO << "  bit0 src=0  : " << tau::get(src_b0_zero).to_str()  << "\n";
		TAU_LOG_INFO << "  bit0 res=0  : " << tau::get(res_b0_zero).to_str()  << "\n";
		TAU_LOG_INFO << "  bit1 src=0  : " << tau::get(src_b1_zero).to_str()  << "\n";
		TAU_LOG_INFO << "  bit1 res=0  : " << tau::get(res_b1_zero).to_str()  << "\n";
		TAU_LOG_INFO << "  high bit2=0 : " << tau::get(tau::build_bf_eq_0(bit<node>(result, 2))).to_str() << "\n";
		TAU_LOG_INFO << "  high bit3=0 : " << tau::get(tau::build_bf_eq_0(bit<node>(result, 3))).to_str() << "\n";
		TAU_LOG_INFO << "------\n";
	}

	TEST_CASE("bvcast: truncation (bv[4] -> bv[2])") {
		using node = node_t;
		using tau = tree<node>;
		auto src    = tau::build_bf_variable(bv_type_id<node>(4));
		auto result = tau::build_bf_variable(bv_type_id<node>(2));
		// Expanded bit-equality predicates for a 4->2 truncation (low 2 bits only)
		auto src_b0_zero = tau::build_bf_eq_0(bit<node>(src, 0));
		auto res_b0_zero = tau::build_bf_eq_0(bit<node>(result, 0));
		auto src_b1_zero = tau::build_bf_eq_0(bit<node>(src, 1));
		auto res_b1_zero = tau::build_bf_eq_0(bit<node>(result, 1));
		TAU_LOG_INFO << "bvcast trunc bv[4]->bv[2]: bit-equality predicates\n";
		TAU_LOG_INFO << "  bit0 src=0  : " << tau::get(src_b0_zero).to_str()  << "\n";
		TAU_LOG_INFO << "  bit0 res=0  : " << tau::get(res_b0_zero).to_str()  << "\n";
		TAU_LOG_INFO << "  bit1 src=0  : " << tau::get(src_b1_zero).to_str()  << "\n";
		TAU_LOG_INFO << "  bit1 res=0  : " << tau::get(res_b1_zero).to_str()  << "\n";
		TAU_LOG_INFO << "------\n";
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
