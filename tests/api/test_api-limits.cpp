// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include <algorithm>
#include <cstdlib>

using tau_api = api<node_t>;

TEST_SUITE("Tau API - runtime limits") {

	// IN-M3: the two SO-1-exposed temporal caps ship finite; an unlimited
	// default turns a non-converging spec into a hang. Keep this case
	// first so it observes the shipped values, not another case's leftovers.
	TEST_CASE("temporal caps ship finite defaults") {
		CHECK( max_fixpoint_steps == 500 );
		CHECK( max_flag_search_steps == 500 );
	}

	// Raw-stored size_t caps: 0 means unlimited and is stored as 0.
	TEST_CASE("plain caps write their globals verbatim") {
		struct row { void (*set)(size_t); size_t* global; };
		const row rows[] = {
			{ &tau_api::set_block_squeeze_cap,   &block_squeeze_cap },
			{ &tau_api::set_max_fixpoint_steps,  &max_fixpoint_steps },
			{ &tau_api::set_max_flag_search_steps,
				&max_flag_search_steps },
			{ &tau_api::set_max_def_passes,      &max_def_passes },
			{ &tau_api::set_max_enum_steps,      &max_enum_steps },
			{ &tau_api::set_max_probe_steps,     &max_probe_steps },
			{ &tau_api::set_max_rewrite_rounds,  &max_rewrite_rounds },
			{ &tau_api::set_max_simplify_rounds, &max_simplify_rounds },
			{ &tau_api::set_ba_decision_pins,    &ba_decision_pins },
		};
		for (const row& r : rows) {
			const size_t saved = *r.global;
			r.set(77);
			CHECK( *r.global == 77 );
			r.set(0);
			CHECK( *r.global == 0 );
			*r.global = saved;
		}
	}

	// max_blast_reentry_depth (antiprenexing/antiprenexing.tmpl.h) is now
	// reached only through bv's own bv-blastdepth REPL/CLI option, not
	// through the api -- set_max_blast_reentry_depth was removed. Set the
	// global directly, the same "raw-stored, verbatim" contract the loop
	// above checks for its siblings, the way tests/test_init.h already
	// sets preprocessing/preprocess_placement globals directly.
	TEST_CASE("max_blast_reentry_depth writes its global verbatim") {
		const size_t saved = max_blast_reentry_depth;
		max_blast_reentry_depth = 77;
		CHECK( max_blast_reentry_depth == 77 );
		max_blast_reentry_depth = 0;
		CHECK( max_blast_reentry_depth == 0 );
		max_blast_reentry_depth = saved;
	}

	// The two decrementing block budgets map 0 to SIZE_MAX instead.
	TEST_CASE("block budgets map 0 to SIZE_MAX") {
		const size_t s1 = block_boole_max_splits;
		const size_t s2 = block_max_rounds;
		tau_api::set_block_max_splits(512);
		CHECK( block_boole_max_splits == 512 );
		tau_api::set_block_max_splits(0);
		CHECK( block_boole_max_splits
			== std::numeric_limits<size_t>::max() );
		tau_api::set_block_max_rounds(33);
		CHECK( block_max_rounds == 33 );
		tau_api::set_block_max_rounds(0);
		CHECK( block_max_rounds == std::numeric_limits<size_t>::max() );
		block_boole_max_splits = s1;
		block_max_rounds = s2;
	}

	// The LTL(ABA) knobs promoted from environment variables / header
	// constants: each setter writes its parameter, and the accessors
	// resolve parameter > environment > default.
	TEST_CASE("ltl timeout: parameter beats environment, -1 unsets") {
		const long saved = ltl_timeout_sec_param;
		tau_api::set_ltl_timeout_sec(-1);
		setenv("TAU_LTL_TIMEOUT_SEC", "17", 1);
		CHECK( ltl_timeout_sec() == 17 );
		tau_api::set_ltl_timeout_sec(9);
		CHECK( ltl_timeout_sec() == 9 );
		tau_api::set_ltl_timeout_sec(0);
		CHECK( ltl_timeout_sec() == 0 );
		// Garbage in the environment keeps the default (LS-9).
		tau_api::set_ltl_timeout_sec(-1);
		setenv("TAU_LTL_TIMEOUT_SEC", "abc", 1);
		CHECK( ltl_timeout_sec() == 60 );
		// Values above one day clamp (SY-R5).
		tau_api::set_ltl_timeout_sec(1000000);
		CHECK( ltl_timeout_sec() == ltl_timeout_sec_max );
		unsetenv("TAU_LTL_TIMEOUT_SEC");
		ltl_timeout_sec_param = saved;
	}

	TEST_CASE("ltl algorithm: parameter beats environment, garbage is auto") {
		const std::string saved = ltl_algorithm_param;
		tau_api::set_ltl_algorithm("");
		unsetenv("TAU_LTL_ALG");
		CHECK( ltl_algorithm_choice() == "" );
		setenv("TAU_LTL_ALG", "D", 1);
		CHECK( ltl_algorithm_choice() == "D" );
		tau_api::set_ltl_algorithm("b");
		CHECK( ltl_algorithm_choice() == "B" );
		tau_api::set_ltl_algorithm("auto");
		CHECK( ltl_algorithm_choice() == "" );
		tau_api::set_ltl_algorithm("C");
		CHECK( ltl_algorithm_choice() == "" );
		unsetenv("TAU_LTL_ALG");
		ltl_algorithm_param = saved;
	}

	TEST_CASE("ltl QE cap: parameter beats environment, garbage keeps 2") {
		const size_t saved = ltl_qe_max_vars_param;
		tau_api::set_ltl_qe_max_vars(0);
		unsetenv("TAU_LTL_OMCAT_QE_MAX_VARS");
		CHECK( ltl_qe_max_vars() == 2 );
		setenv("TAU_LTL_OMCAT_QE_MAX_VARS", "4", 1);
		CHECK( ltl_qe_max_vars() == 4 );
		setenv("TAU_LTL_OMCAT_QE_MAX_VARS", "abc", 1);
		CHECK( ltl_qe_max_vars() == 2 );
		tau_api::set_ltl_qe_max_vars(3);
		CHECK( ltl_qe_max_vars() == 3 );
		unsetenv("TAU_LTL_OMCAT_QE_MAX_VARS");
		ltl_qe_max_vars_param = saved;
	}

	TEST_CASE("ltl game caps write their parameters verbatim") {
		const long s1 = ltl_hoa_max_states_param;
		const long s2 = ltl_guard_max_cubes_param;
		const long s3 = ltl_max_refinement_rounds_param;
		const long s4 = ltl_window_max_paths_param;
		tau_api::set_ltl_hoa_max_states(77);
		CHECK( ltl_hoa_max_states() == 77 );
		tau_api::set_ltl_hoa_max_states(0);
		CHECK( ltl_hoa_max_states() == 0 );
		tau_api::set_ltl_guard_max_cubes(5);
		CHECK( ltl_guard_max_cubes() == 5 );
		tau_api::set_ltl_max_refinement_rounds(9);
		CHECK( ltl_max_refinement_rounds() == 9 );
		tau_api::set_ltl_max_refinement_rounds(0);
		CHECK( ltl_max_refinement_rounds() == 0 );
		tau_api::set_ltl_window_max_paths(11);
		CHECK( ltl_window_max_paths() == 11 );
		ltl_hoa_max_states_param = s1;
		ltl_guard_max_cubes_param = s2;
		ltl_max_refinement_rounds_param = s3;
		ltl_window_max_paths_param = s4;
	}

	// Each of the four game caps resolves parameter > environment >
	// default, like the timeout and the QE cap before them, so a script
	// can set one without a flag and a flag always wins over the script.
	TEST_CASE("ltl game caps: parameter beats environment, garbage keeps "
	          "the default") {
		struct cap {
			const char* var;
			long* param;
			size_t (*effective)();
			size_t dflt;
		};
		const cap caps[] = {
			{ "TAU_LTL_HOA_MAX_STATES", &ltl_hoa_max_states_param,
				&ltl_hoa_max_states, size_t(1) << 22 },
			{ "TAU_LTL_GUARD_MAX_CUBES", &ltl_guard_max_cubes_param,
				&ltl_guard_max_cubes, 512 },
			{ "TAU_LTL_REFINEMENT_ROUNDS",
				&ltl_max_refinement_rounds_param,
				&ltl_max_refinement_rounds, 64 },
			{ "TAU_LTL_WINDOW_MAX_PATHS",
				&ltl_window_max_paths_param,
				&ltl_window_max_paths, 4096 }
		};
		for (const auto& c : caps) {
			const long saved = *c.param;
			*c.param = -1;
			unsetenv(c.var);
			CHECK( c.effective() == c.dflt );
			setenv(c.var, "7", 1);
			CHECK( c.effective() == 7 );
			// 0 is a value, not an absence: it means unlimited.
			setenv(c.var, "0", 1);
			CHECK( c.effective() == 0 );
			setenv(c.var, "-3", 1);
			CHECK( c.effective() == c.dflt );
			setenv(c.var, "abc", 1);
			CHECK( c.effective() == c.dflt );
			*c.param = 11;
			CHECK( c.effective() == 11 );
			unsetenv(c.var);
			*c.param = saved;
		}
	}

	// Both new caps can change a verdict (decided vs UNKNOWN), so the memos
	// must see them move.
	TEST_CASE("refinement and window caps are part of the budget fingerprint") {
		const size_t base = verdict_budget_fingerprint<node_t>();
		const long s3 = ltl_max_refinement_rounds_param;
		const long s4 = ltl_window_max_paths_param;
		tau_api::set_ltl_max_refinement_rounds(
			ltl_max_refinement_rounds() + 1);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		ltl_max_refinement_rounds_param = s3;
		tau_api::set_ltl_window_max_paths(ltl_window_max_paths() + 1);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		ltl_window_max_paths_param = s4;
		CHECK( verdict_budget_fingerprint<node_t>() == base );
	}

	// An environment fallback is part of the same fingerprint: a memo made
	// under one budget must not answer a query made under another, however
	// the budget was set.
	TEST_CASE("an environment fallback moves the budget fingerprint") {
		const long saved = ltl_window_max_paths_param;
		ltl_window_max_paths_param = -1;
		unsetenv("TAU_LTL_WINDOW_MAX_PATHS");
		const size_t base = verdict_budget_fingerprint<node_t>();
		setenv("TAU_LTL_WINDOW_MAX_PATHS", "13", 1);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		unsetenv("TAU_LTL_WINDOW_MAX_PATHS");
		CHECK( verdict_budget_fingerprint<node_t>() == base );
		ltl_window_max_paths_param = saved;
	}

	// The verdict memos are keyed on the formula; the budget fingerprint
	// is what tells them a runtime budget moved in between.
	TEST_CASE("verdict budget fingerprint moves with every budget") {
		const size_t base = verdict_budget_fingerprint<node_t>();
		const size_t saved_fp = max_fixpoint_steps;
		const size_t saved_fl = max_flag_search_steps;
		const size_t saved_cs = max_consistency_subsets;
		const long   saved_to = ltl_timeout_sec_param;
		const std::string saved_alg = ltl_algorithm_param;
		max_fixpoint_steps = saved_fp + 1;
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		max_fixpoint_steps = saved_fp;
		CHECK( verdict_budget_fingerprint<node_t>() == base );
		max_flag_search_steps = saved_fl + 1;
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		max_flag_search_steps = saved_fl;
		max_consistency_subsets = saved_cs + 1;
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		max_consistency_subsets = saved_cs;
		tau_api::set_ltl_timeout_sec(ltl_timeout_sec() + 1);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		ltl_timeout_sec_param = saved_to;
		tau_api::set_ltl_algorithm("B");
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		ltl_algorithm_param = saved_alg;
		CHECK( verdict_budget_fingerprint<node_t>() == base );
	}

	// PW-N4: the semantic PWR fallback is a runtime knob, OFF by default.
	TEST_CASE("pwr semantic fallback ships off and round-trips") {
		const bool saved = pwr_semantic_fallback;
		CHECK_FALSE( pwr_semantic_fallback );
		tau_api::set_pwr_semantic_fallback(true);
		CHECK( pwr_semantic_fallback );
		tau_api::set_pwr_semantic_fallback(false);
		CHECK_FALSE( pwr_semantic_fallback );
		pwr_semantic_fallback = saved;
	}

	// An algebra's options steer how its formulas are decided, and the
	// memos are keyed on the formula alone.
	TEST_CASE("BA options and preprocessing are part of the budget fingerprint") {
		const size_t base = verdict_budget_fingerprint<node_t>();
		for (const std::string& name : tau_api::ba_option_names()) {
			CAPTURE(name);
			auto got = tau_api::get_ba_option(name);
			REQUIRE( got.has_value() );
			const size_t v = got.value();
			auto moved = tau_api::set_ba_option(name, v == 1 ? 2 : 1);
			REQUIRE( moved.has_value() );
			if (moved.value() != v)
				CHECK( verdict_budget_fingerprint<node_t>() != base );
			REQUIRE( tau_api::set_ba_option(name, v).has_value() );
			CHECK( verdict_budget_fingerprint<node_t>() == base );
		}
		const bool saved = preprocessing;
		tau_api::set_preprocessing(!saved);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		tau_api::set_preprocessing(saved);
		CHECK( verdict_budget_fingerprint<node_t>() == base );
	}

	TEST_CASE("BA options answer an error for a name no algebra declares") {
		for (const char* name : { "nope-nothing", "nothing", "-x", "bv-" })
		{
			CAPTURE(name);
			auto set = tau_api::set_ba_option(name, 1);
			CHECK_FALSE( set.has_value() );
			CHECK( set.report().has_error() );
			CHECK_FALSE( tau_api::get_ba_option(name).has_value() );
		}
		for (const std::string& name : tau_api::ba_option_names())
			CHECK( tau_api::get_ba_option(name).has_value() );
	}

#ifdef TAU_PACK_HAS_BA_BV
	TEST_CASE("BA options round-trip through the api") {
		const auto names = tau_api::ba_option_names();
		CHECK( std::ranges::find(names, "bv-widening") != names.end() );
		const bool saved_elim = bv_definitional_elimination;
		auto off = tau_api::set_ba_option("bv-definitional-elimination", 0);
		REQUIRE( off.has_value() );
		CHECK( off.value() == 0 );
		CHECK_FALSE( bv_definitional_elimination );
		CHECK( tau_api::get_ba_option("bv-definitional-elimination")
			.value() == 0 );
		CHECK( tau_api::set_ba_option("bv-definitional-elimination", 7)
			.value() == 1 );
		CHECK( bv_definitional_elimination );
		bv_definitional_elimination = saved_elim;

		const size_t saved_atoms = bv_defelim_max_atoms;
		CHECK( tau_api::set_ba_option("bv-defelim-max-atoms", 5)
			.value() == 5 );
		CHECK( bv_defelim_max_atoms == 5 );
		bv_defelim_max_atoms = saved_atoms;

		// bv-max-width ignores 0: the value in force is reported back.
		const size_t saved_width = bv_max_width;
		CHECK( tau_api::set_ba_option("bv-max-width", 0).value()
			== saved_width );
		bv_max_width = saved_width;
	}

	// The case-split cap follows the block budgets: 0 = unlimited = SIZE_MAX.
	// Driven through bv's own `case-split-max-tests` option, not an api
	// setter: only bv's own case-split pass can ever make progress against
	// it, so the option lives on bv's descriptor (bv_descriptor.tmpl.h).
	TEST_CASE("bv case split cap maps 0 to SIZE_MAX") {
		using bv_descriptor = ba_descriptor<bv, node_t>;
		const size_t saved = bv_case_split_max_tests;
		bv_descriptor::set_case_split_max_tests_option(3);
		CHECK( bv_case_split_max_tests == 3 );
		bv_descriptor::set_case_split_max_tests_option(0);
		CHECK( bv_case_split_max_tests
			== std::numeric_limits<size_t>::max() );
		bv_case_split_max_tests = saved;
	}
#endif // TAU_PACK_HAS_BA_BV

	TEST_CASE("interpreter statics") {
		const size_t sw = interpreter<node_t>::spec_size_warn_threshold;
		const size_t ra = interpreter<node_t>::max_revision_alts;
		const size_t gm = interpreter<node_t>::gc_min_size;
		const double gf = interpreter<node_t>::gc_growth_factor;
		tau_api::set_spec_size_warn(4096);
		CHECK( interpreter<node_t>::spec_size_warn_threshold == 4096 );
		tau_api::set_max_revision_alts(3);
		CHECK( interpreter<node_t>::max_revision_alts == 3 );
		tau_api::set_gc_min_size(512);
		CHECK( interpreter<node_t>::gc_min_size == 512 );
		tau_api::set_gc_growth_factor(2.5);
		CHECK( interpreter<node_t>::gc_growth_factor == doctest::Approx(2.5) );
		interpreter<node_t>::spec_size_warn_threshold = sw;
		interpreter<node_t>::max_revision_alts = ra;
		interpreter<node_t>::gc_min_size = gm;
		interpreter<node_t>::gc_growth_factor = gf;
	}

	// Enum setters: in-range casts, out-of-range clamps to the default.
	TEST_CASE("set_preprocess_placement clamps to per_leaf") {
		const preprocess_site saved = preprocess_placement;
		tau_api::set_preprocess_placement(0);
		CHECK( preprocess_placement == preprocess_site::per_leaf );
		tau_api::set_preprocess_placement(1);
		CHECK( preprocess_placement == preprocess_site::per_block );
		tau_api::set_preprocess_placement(2);
		CHECK( preprocess_placement == preprocess_site::per_formula );
		tau_api::set_preprocess_placement(99);
		CHECK( preprocess_placement == preprocess_site::per_leaf );
		tau_api::set_preprocess_placement(-1);
		CHECK( preprocess_placement == preprocess_site::per_leaf );
		preprocess_placement = saved;
	}

	TEST_CASE("set_preprocess_method clamps to anti_prenex_result") {
		const preprocess_mode saved = preprocess_method;
		tau_api::set_preprocess_method(0);
		CHECK( preprocess_method == preprocess_mode::anti_prenex_result );
		tau_api::set_preprocess_method(1);
		CHECK( preprocess_method == preprocess_mode::defer );
		tau_api::set_preprocess_method(5);
		CHECK( preprocess_method == preprocess_mode::anti_prenex_result );
		preprocess_method = saved;
	}

	TEST_CASE("set_solver_placement clamps to eager") {
		const solver_site saved = solver_placement;
		tau_api::set_solver_placement(0);
		CHECK( solver_placement == solver_site::eager );
		tau_api::set_solver_placement(1);
		CHECK( solver_placement == solver_site::per_closed_block );
		tau_api::set_solver_placement(2);
		CHECK( solver_placement == solver_site::per_formula );
		tau_api::set_solver_placement(7);
		CHECK( solver_placement == solver_site::eager );
		solver_placement = saved;
	}

	// NOTE the asymmetric default: out-of-range clamps to
	// ext_rewrite_no_models (the shipped default), NOT to baseline.
	TEST_CASE("set_cvc5_options clamps to ext_rewrite_no_models") {
		const cvc5_option_set saved = cvc5_options;
		tau_api::set_cvc5_options(
			static_cast<int>(cvc5_option_set::baseline));
		CHECK( cvc5_options == cvc5_option_set::baseline );
		tau_api::set_cvc5_options(
			static_cast<int>(cvc5_option_set::combined_best));
		CHECK( cvc5_options == cvc5_option_set::combined_best );
		tau_api::set_cvc5_options(999);
		CHECK( cvc5_options == cvc5_option_set::ext_rewrite_no_models );
		cvc5_options = saved;
	}
}
