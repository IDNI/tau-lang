// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

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
			{ &tau_api::set_max_rewrite_rounds,  &max_rewrite_rounds },
			{ &tau_api::set_max_simplify_rounds, &max_simplify_rounds },
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
