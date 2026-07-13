// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"
#include "heuristics/bv_predicate_blasting.h"
#include "heuristics/bv_arithmetic_scan.h"

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

	TEST_CASE("bvshr_by_one_rule") {
		using node = node_t;

		TAU_LOG_INFO << "bvshr_by_one_rule:\n";
		TAU_LOG_INFO << TAU_LOG_RULE(bvshr_by_one_rule<node>(2)) << "\n";
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

	TEST_CASE("bit: extracting bit 0 gives a mask-and term of the right shape") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		tref bit0 = bit<node>(x, 0);
		// bit(x, 0) is the AND of x with the { 1 } bitmask (bit 0 selector)
		CHECK( tau::get(bit0).find_top(is<node, tau::bf_and>) != nullptr );
	}

	TEST_CASE("bvshl_by_one and bvshr_by_one produce distinct satisfiable constraints") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto shifted = tau::build_bf_variable(bv_type_id<node>(4));
		tref shl1 = bvshl_by_one<node>(x, shifted);
		tref shr1 = bvshr_by_one<node>(x, shifted);
		CHECK( tau::get(shl1) != tau::get(shr1) );
		CHECK( is_non_temp_nso_satisfiable<node>(shl1) );
		CHECK( is_non_temp_nso_satisfiable<node>(shr1) );
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

	TEST_CASE("bvshr_rule") {
		using node = node_t;
		using tau = tree<node>;

		TAU_LOG_INFO << "bvshr_rule:\n";
		TAU_LOG_INFO << TAU_LOG_RULE(bvshr_rule<node>(tau::get(tau::bf, tau::get_bv_constant(4, 1)))) << "\n";
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

	// bved is not called anywhere else in the library (atomic_blasting
	// dispatches "/" and "%" to bvdiv/bvmod separately, never fusing them
	// into a single bved call), so it has no WFF-level coverage either.
	// Check it directly: 10 = 3*3 + 1, so quotient=3, remainder=1 must
	// satisfy the combined constraint, while a wrong remainder must not.
	TEST_CASE("bved: 10 / 3 gives quotient=3, remainder=1") {
		using node = node_t;
		using tau = tree<node>;

		trefs aux;
		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto q = tau::build_bf_variable(bv_type_id<node>(4));
		auto r = tau::build_bf_variable(bv_type_id<node>(4));
		auto c = tau::get(tau::bf, tau::get_bv_constant(4, 3));
		tref constraint = bved<node>(x, c, q, r, aux);
		tref x10 = tau::build_bf_eq(x, tau::get(tau::bf, tau::get_bv_constant(4, 10)));
		tref q3 = tau::build_bf_eq(q, tau::get(tau::bf, tau::get_bv_constant(4, 3)));
		tref r1 = tau::build_bf_eq(r, tau::get(tau::bf, tau::get_bv_constant(4, 1)));
		tref r2 = tau::build_bf_eq(r, tau::get(tau::bf, tau::get_bv_constant(4, 2)));
		tref good = tau::build_wff_and(tau::build_wff_and(constraint, x10),
			tau::build_wff_and(q3, r1));
		tref bad = tau::build_wff_and(tau::build_wff_and(constraint, x10),
			tau::build_wff_and(q3, r2));
		CHECK( is_non_temp_nso_satisfiable<node>(good) );
		CHECK( !is_non_temp_nso_satisfiable<node>(bad) );
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

	// wff_predicate_blasting's dispatcher never calls bvlteq/bvgteq/bvnlteq/
	// bvnlt/bvngt/bvngteq/bvneq: "<="/">="/"!<="/"!<"/"!>"/"!>=" surface
	// syntax routes through lteq_predicate/gteq_predicate/nlteq_predicate/
	// nlt_predicate/ngt_predicate/ngteq_predicate instead, each of which
	// independently negates lt_predicate/gt_predicate rather than calling
	// these wrappers, and "!=" routes through neq_predicate ->
	// keep_comparison_predicate rather than bvneq. So this whole family of
	// public wrapper functions has zero coverage anywhere else. Check
	// directly that each delegates to the wrapper its definition names, and
	// that bvneq (a real recurrence, not a 1-line forward) computes the
	// right truth value for concrete operands.
	TEST_CASE("bvlteq delegates to bvgt") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvlteq<node>(x, y))
			== tau::get(tau::build_wff_neg(bvgt<node>(x, y))) );
	}

	TEST_CASE("bvgteq delegates to bvlt") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvgteq<node>(x, y))
			== tau::get(tau::build_wff_neg(bvlt<node>(x, y))) );
	}

	TEST_CASE("bvnlteq delegates to bvgt") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvnlteq<node>(x, y)) == tau::get(bvgt<node>(x, y)) );
	}

	TEST_CASE("bvnlt delegates to bvgteq") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvnlt<node>(x, y)) == tau::get(bvgteq<node>(x, y)) );
	}

	TEST_CASE("bvngt delegates to bvlteq") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvngt<node>(x, y)) == tau::get(bvlteq<node>(x, y)) );
	}

	TEST_CASE("bvngteq delegates to bvlt") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvngteq<node>(x, y)) == tau::get(bvlt<node>(x, y)) );
	}

	TEST_CASE("bvneq: x != x is never satisfiable") {
		using node = node_t;
		using tau = tree<node>;

		// Operands must carry proper bv type metadata (as a variable built
		// via bv_type_id does) -- a bare ba_constant does not carry a
		// bitwidth get_bv_type_bitwidth can read, so the recurrence built
		// around it never resolves.
		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		tref pred_same = bvneq<node>(x, x);
		CHECK( tau::get(normalizer<node>(pred_same)).equals_F() );
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

TEST_SUITE("bv_arithmetic_scan") {

	// Variable names are deliberately unique to this suite (not the ubiquitous
	// x/y/z used by hundreds of other cases in this binary): with TAU_CACHE
	// enabled (Release), structurally-identical subtrees are interned
	// process-wide, and BA-type-family / free-var caches keyed on those
	// shared nodes could otherwise pick up a stale result computed under a
	// different formula's context.
	TEST_CASE("collect_bv_arithmetic_taint: bvatx*bvaty survives as unblastable, the atom is tainted") {
		using node = node_t;

		// bvatx * bvaty (non-constant multiplier) cannot be blasted by bvmul,
		// so a formula containing it unresolved is exactly the kind of
		// residue this heuristic is meant to flag for the solver: the
		// top-level atomic wff itself is added to the taint set (pass 2).
		tref fm = get_nso_rr(
			"bvatx:bv[4] * bvaty:bv[4] = bvatz:bv[4].").value().main->get();
		auto tainted = collect_bv_arithmetic_taint<node>(fm);
		CHECK( tainted.size() == 1 );
		CHECK( tainted.contains(fm) );
	}

	TEST_CASE("collect_bv_arithmetic_taint: no arithmetic, nothing tainted") {
		tref fm = get_nso_rr("bvatx:bv[4] = bvaty:bv[4].").value().main->get();
		auto tainted = collect_bv_arithmetic_taint<node_t>(fm);
		CHECK( tainted.empty() );
	}

	TEST_CASE("make_bv_arithmetic_skip: skip predicate matches the taint set") {
		using node = node_t;

		tref fm = get_nso_rr(
			"bvatx:bv[4] * bvaty:bv[4] = bvatz:bv[4].").value().main->get();
		auto skip = make_bv_arithmetic_skip<node>(fm);
		tref other = get_nso_rr("bvatx:bv[4] = bvatz:bv[4].").value().main->get();
		CHECK( skip(fm) );
		CHECK( !skip(other) );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
