// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <set>
#include <string>
#include <type_traits>

using tau_api = api<node_t>;

TEST_SUITE("Tau API - runtime limits") {

	// Loads the environment with @p var set to @p text, then removes the
	// variable. True when every option takes its variable.
	static bool load_env_with(const char* var, const char* text) {
		setenv(var, text, 1);
		const bool ok = idni::options().load_env("TAU_").has_value();
		unsetenv(var);
		return ok;
	}

	// IN-M3: the two SO-1-exposed temporal caps ship finite; an unlimited
	// default turns a non-converging spec into a hang. Keep this case
	// first so it observes the shipped values, not another case's leftovers.
	TEST_CASE("temporal caps ship finite defaults") {
		CHECK( tau_api::get_max_fixpoint_steps() == 500 );
		CHECK( tau_api::get_max_flag_search_steps() == 500 );
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
		};
		for (const row& r : rows) {
			const size_t saved = *r.global;
			r.set(77);
			CHECK( *r.global == 77 );
			r.set(0);
			CHECK( *r.global == 0 );
			*r.global = saved;
		}
		const size_t saved_pins = ba_decision_pins;
		tau_api::set_ba_decision_pins(77);
		CHECK( ba_decision_pins == 77 );
		tau_api::set_ba_decision_pins(0);
		CHECK( ba_decision_pins == 0 );
		ba_decision_pins = saved_pins;
	}

	// max_blast_reentry_depth (antiprenexing/antiprenexing.h) is bv's
	// bv-blastdepth CLI/REPL option, and the api reaches it in every pack.
	TEST_CASE("max_blast_reentry_depth writes its global verbatim") {
		const size_t saved = max_blast_reentry_depth;
		tau_api::set_max_blast_reentry_depth(77);
		CHECK( tau_api::get_max_blast_reentry_depth() == 77 );
		tau_api::set_max_blast_reentry_depth(0);
		CHECK( tau_api::get_max_blast_reentry_depth() == 0 );
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
		CHECK( tau_api::get_block_max_splits() == 0 );
		tau_api::set_block_max_rounds(33);
		CHECK( block_max_rounds == 33 );
		tau_api::set_block_max_rounds(0);
		CHECK( block_max_rounds
			== std::numeric_limits<size_t>::max() );
		CHECK( tau_api::get_block_max_rounds() == 0 );
		block_boole_max_splits = s1;
		block_max_rounds = s2;
	}

	// The block budgets read and write 0 by name too, as their setters do.
	TEST_CASE("block budgets map 0 to SIZE_MAX by name") {
		const size_t saved = block_boole_max_splits;
		CHECK( idni::options().set_text("block-max-splits", "0")
			.has_value() );
		CHECK( block_boole_max_splits
			== std::numeric_limits<size_t>::max() );
		CHECK( idni::options().get_text("block-max-splits").value()
			== "0" );
		CHECK( idni::options().set_text("block-max-splits", "9")
			.has_value() );
		CHECK( block_boole_max_splits == 9 );
		block_boole_max_splits = saved;
	}

	// The LTL(ABA) knobs: each setter writes its parameter, and the
	// environment writes the same parameter once, when it is loaded.
	TEST_CASE("ltl timeout: environment and setter write one value") {
		const size_t saved = ltl_timeout_sec_param;
		CHECK( load_env_with("TAU_LTL_TIMEOUT", "17") );
		CHECK( ltl_timeout_sec() == 17 );
		tau_api::set_ltl_timeout_sec(9);
		CHECK( ltl_timeout_sec() == 9 );
		tau_api::set_ltl_timeout_sec(0);
		CHECK( ltl_timeout_sec() == 0 );
		// A negative value restores the default.
		tau_api::set_ltl_timeout_sec(-1);
		CHECK( ltl_timeout_sec() == 60 );
		// Garbage in the environment is an error and changes nothing.
		CHECK_FALSE( load_env_with("TAU_LTL_TIMEOUT", "abc") );
		CHECK( ltl_timeout_sec() == 60 );
		// Values above one day clamp (SY-R5).
		tau_api::set_ltl_timeout_sec(1000000);
		CHECK( (size_t) ltl_timeout_sec() == ltl_timeout_sec_max );
		CHECK( load_env_with("TAU_LTL_TIMEOUT", "1000000") );
		CHECK( (size_t) ltl_timeout_sec() == ltl_timeout_sec_max );
		ltl_timeout_sec_param = saved;
	}

	TEST_CASE("ltl algorithm: an unknown word is an error by name") {
		const std::string saved = ltl_algorithm_param;
		tau_api::set_ltl_algorithm("");
		CHECK( ltl_algorithm_choice() == "" );
		CHECK( load_env_with("TAU_LTL_ALG", "D") );
		CHECK( ltl_algorithm_choice() == "D" );
		tau_api::set_ltl_algorithm("b");
		CHECK( ltl_algorithm_choice() == "B" );
		tau_api::set_ltl_algorithm("auto");
		CHECK( ltl_algorithm_choice() == "" );
		// The setter takes any word and reads an unknown one as auto; the
		// option refuses it.
		tau_api::set_ltl_algorithm("C");
		CHECK( ltl_algorithm_choice() == "" );
		CHECK_FALSE( load_env_with("TAU_LTL_ALG", "C") );
		CHECK_FALSE( idni::options().set_text("ltl-alg", "C")
			.has_value() );
		CHECK( ltl_algorithm_param == "C" );
		ltl_algorithm_param = saved;
	}

	TEST_CASE("ltl QE cap: environment and setter write one value") {
		const size_t saved = ltl_qe_max_vars_param;
		CHECK( ltl_qe_max_vars() == saved );
		CHECK( load_env_with("TAU_LTL_QE_MAX_VARS", "4") );
		CHECK( ltl_qe_max_vars() == 4 );
		CHECK_FALSE( load_env_with("TAU_LTL_QE_MAX_VARS", "abc") );
		CHECK( ltl_qe_max_vars() == 4 );
		tau_api::set_ltl_qe_max_vars(3);
		CHECK( ltl_qe_max_vars() == 3 );
		// 0 turns the fast path off.
		tau_api::set_ltl_qe_max_vars(0);
		CHECK( ltl_qe_max_vars() == 0 );
		ltl_qe_max_vars_param = saved;
	}

	TEST_CASE("ltl game caps write their parameters verbatim") {
		const size_t s1 = ltl_hoa_max_states_param;
		const size_t s2 = ltl_guard_max_cubes_param;
		const size_t s3 = ltl_max_refinement_rounds_param;
		const size_t s4 = ltl_window_max_paths_param;
		const size_t s5 = ltl_closed_regions_timeout_param;
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
		tau_api::set_ltl_closed_regions_timeout(3);
		CHECK( ltl_closed_regions_timeout() == 3 );
		tau_api::set_ltl_closed_regions_timeout(0);
		CHECK( ltl_closed_regions_timeout() == 0 );
		ltl_closed_regions_timeout_param = s5;
		ltl_hoa_max_states_param = s1;
		ltl_guard_max_cubes_param = s2;
		ltl_max_refinement_rounds_param = s3;
		ltl_window_max_paths_param = s4;
	}

	TEST_CASE("data game and consistency caps write their parameters "
	          "verbatim") {
		const size_t s1 = ltl_data_game_max_nodes_param;
		const size_t s2 = ltl_data_game_max_memo_param;
		const size_t s3 = max_consistency_subsets_param;
		const size_t s4 = max_cover_products_param;
		tau_api::set_ltl_data_game_max_nodes(1234);
		CHECK( ltl_data_game_max_nodes() == 1234 );
		tau_api::set_ltl_data_game_max_nodes(0);
		CHECK( ltl_data_game_max_nodes() == 0 );
		tau_api::set_ltl_data_game_max_memo(4321);
		CHECK( ltl_data_game_max_memo() == 4321 );
		tau_api::set_ltl_data_game_max_memo(0);
		CHECK( ltl_data_game_max_memo() == 0 );
		tau_api::set_max_consistency_subsets(13);
		CHECK( max_consistency_subsets() == 13 );
		tau_api::set_max_consistency_subsets(0);
		CHECK( max_consistency_subsets() == 0 );
		tau_api::set_max_cover_products(17);
		CHECK( max_cover_products() == 17 );
		tau_api::set_max_cover_products(0);
		CHECK( max_cover_products() == 0 );
		ltl_data_game_max_nodes_param = s1;
		ltl_data_game_max_memo_param = s2;
		max_consistency_subsets_param = s3;
		max_cover_products_param = s4;
	}

	// Each of these caps takes its environment variable when the
	// environment is loaded, and a later setter call writes over it.
	TEST_CASE("ltl game caps: environment and setter write one value") {
		struct cap {
			const char* var;
			size_t* param;
			size_t (*effective)();
		};
		const cap caps[] = {
			{ "TAU_LTL_HOA_MAX_STATES", &ltl_hoa_max_states_param,
				&ltl_hoa_max_states },
			{ "TAU_LTL_GUARD_MAX_CUBES", &ltl_guard_max_cubes_param,
				&ltl_guard_max_cubes },
			{ "TAU_LTL_REFINEMENT_ROUNDS",
				&ltl_max_refinement_rounds_param,
				&ltl_max_refinement_rounds },
			{ "TAU_LTL_WINDOW_MAX_PATHS",
				&ltl_window_max_paths_param,
				&ltl_window_max_paths },
			{ "TAU_LTL_CLOSED_REGIONS_TIMEOUT",
				&ltl_closed_regions_timeout_param,
				&ltl_closed_regions_timeout },
			{ "TAU_LTL_DATA_GAME_MAX_NODES",
				&ltl_data_game_max_nodes_param,
				&ltl_data_game_max_nodes },
			{ "TAU_LTL_DATA_GAME_MAX_MEMO",
				&ltl_data_game_max_memo_param,
				&ltl_data_game_max_memo },
			{ "TAU_MAX_CONSISTENCY_SUBSETS",
				&max_consistency_subsets_param,
				&max_consistency_subsets },
			{ "TAU_MAX_COVER_PRODUCTS",
				&max_cover_products_param,
				&max_cover_products }
		};
		for (const auto& c : caps) {
			CAPTURE( c.var );
			const size_t saved = *c.param;
			CHECK( load_env_with(c.var, "7") );
			CHECK( c.effective() == 7 );
			// 0 is a value: it means unlimited (no attempt for the
			// closed regions).
			CHECK( load_env_with(c.var, "0") );
			CHECK( c.effective() == 0 );
			CHECK_FALSE( load_env_with(c.var, "-3") );
			CHECK_FALSE( load_env_with(c.var, "abc") );
			CHECK( c.effective() == 0 );
			*c.param = 11;
			CHECK( c.effective() == 11 );
			*c.param = saved;
		}
	}

	// Both new caps can change a verdict (decided vs UNKNOWN), so the memos
	// must see them move.
	TEST_CASE("refinement and window caps are part of the budget fingerprint") {
		const size_t base = verdict_budget_fingerprint<node_t>();
		const size_t s3 = ltl_max_refinement_rounds_param;
		const size_t s4 = ltl_window_max_paths_param;
		tau_api::set_ltl_max_refinement_rounds(
			ltl_max_refinement_rounds() + 1);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		ltl_max_refinement_rounds_param = s3;
		tau_api::set_ltl_window_max_paths(ltl_window_max_paths() + 1);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		ltl_window_max_paths_param = s4;
		CHECK( verdict_budget_fingerprint<node_t>() == base );
	}

	// A full node table leaves a data game undecided, so the node cap can
	// change a verdict; the memo cap is in the fingerprint as a budget of
	// the same game.
	TEST_CASE("data game caps are part of the budget fingerprint") {
		const size_t base = verdict_budget_fingerprint<node_t>();
		const size_t s1 = ltl_data_game_max_nodes_param;
		const size_t s2 = ltl_data_game_max_memo_param;
		tau_api::set_ltl_data_game_max_nodes(
			ltl_data_game_max_nodes() + 1);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		ltl_data_game_max_nodes_param = s1;
		tau_api::set_ltl_data_game_max_memo(
			ltl_data_game_max_memo() + 1);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		ltl_data_game_max_memo_param = s2;
		CHECK( verdict_budget_fingerprint<node_t>() == base );
	}

	// A value from the environment is part of the same fingerprint: a memo
	// made under one budget must not answer a query made under another,
	// however the budget was set.
	TEST_CASE("a loaded environment moves the budget fingerprint") {
		const char* vars[] = { "TAU_LTL_WINDOW_MAX_PATHS",
			"TAU_LTL_DATA_GAME_MAX_NODES",
			"TAU_LTL_DATA_GAME_MAX_MEMO",
			"TAU_MAX_CONSISTENCY_SUBSETS",
			"TAU_MAX_COVER_PRODUCTS" };
		size_t* params[] = { &ltl_window_max_paths_param,
			&ltl_data_game_max_nodes_param,
			&ltl_data_game_max_memo_param,
			&max_consistency_subsets_param,
			&max_cover_products_param };
		for (size_t i = 0; i < 5; ++i) {
			CAPTURE( vars[i] );
			const size_t saved = *params[i];
			*params[i] = 12;
			const size_t base = verdict_budget_fingerprint<node_t>();
			CHECK( load_env_with(vars[i], "13") );
			CHECK( verdict_budget_fingerprint<node_t>() != base );
			*params[i] = 12;
			CHECK( verdict_budget_fingerprint<node_t>() == base );
			*params[i] = saved;
		}
	}

	// The verdict memos are keyed on the formula; the budget fingerprint
	// is what tells them a runtime budget moved in between.
	TEST_CASE("verdict budget fingerprint moves with every budget") {
		const size_t base = verdict_budget_fingerprint<node_t>();
		const size_t saved_fp = max_fixpoint_steps;
		const size_t saved_fl = max_flag_search_steps;
		const size_t saved_cs = max_consistency_subsets_param;
		const size_t saved_to = ltl_timeout_sec_param;
		const std::string saved_alg = ltl_algorithm_param;
		max_fixpoint_steps = saved_fp + 1;
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		max_fixpoint_steps = saved_fp;
		CHECK( verdict_budget_fingerprint<node_t>() == base );
		max_flag_search_steps = saved_fl + 1;
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		max_flag_search_steps = saved_fl;
		tau_api::set_max_consistency_subsets(
			max_consistency_subsets() + 1);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		max_consistency_subsets_param = saved_cs;
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
			if (!got.has_value()) {
				// a text option: nothing but nlang-provider is
				// sure to take a word that moves it
				if (name != "nlang-provider") continue;
				auto was = tau_api::get_ba_text_option(name);
				REQUIRE( was.has_value() );
				REQUIRE( tau_api::set_ba_text_option(name,
					was.value() == "openai" ? "anthropic"
						: "openai").has_value() );
				CHECK( verdict_budget_fingerprint<node_t>() != base );
				REQUIRE( tau_api::set_ba_text_option(name, "")
					.has_value() );
				CHECK( verdict_budget_fingerprint<node_t>() == base );
				continue;
			}
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
		for (const std::string& name : tau_api::ba_option_names()) {
			CAPTURE(name);
			// every option is a number or a text, never both
			CHECK( tau_api::get_ba_option(name).has_value()
				!= tau_api::get_ba_text_option(name).has_value() );
		}
		for (const char* name : { "nope-nothing", "nothing", "-x" }) {
			CAPTURE(name);
			CHECK_FALSE( tau_api::set_ba_text_option(name, "x")
				.has_value() );
			CHECK_FALSE( tau_api::get_ba_text_option(name)
				.has_value() );
		}
	}

#ifdef TAU_PACK_HAS_BA_NLANG
	TEST_CASE("BA text options round-trip through the api") {
		const auto names = tau_api::ba_option_names();
		for (const char* name : { "nlang-provider", "nlang-endpoint",
			"nlang-model", "nlang-api-key", "nlang-effort",
			"nlang-max-tokens", "nlang-fallback", "nlang-http-timeout" })
				CHECK( std::ranges::find(names, name) != names.end() );
		const llm_options saved = nlang_llm_options();
		nlang_llm_options() = {};

		auto model = tau_api::set_ba_text_option("nlang-model", "my-model-1");
		REQUIRE( model.has_value() );
		CHECK( model.value() == "my-model-1" );
		CHECK( tau_api::get_ba_text_option("nlang-model").value()
			== "my-model-1" );
		CHECK( nlang_llm_options().model == "my-model-1" );

		auto url = tau_api::set_ba_text_option("nlang-endpoint",
			"http://localhost:8080/v1");
		REQUIRE( url.has_value() );
		CHECK( url.value() == "http://localhost:8080/v1" );

		// a closed set of words: another one is refused and changes nothing
		REQUIRE( tau_api::set_ba_text_option("nlang-provider", "anthropic")
			.has_value() );
		auto bad = tau_api::set_ba_text_option("nlang-provider", "nobody");
		CHECK_FALSE( bad.has_value() );
		CHECK( report_has_code(bad.report(), code::invalid_argument) );
		CHECK( tau_api::get_ba_text_option("nlang-provider").value()
			== "anthropic" );
		CHECK_FALSE( tau_api::set_ba_text_option("nlang-effort", "extreme")
			.has_value() );
		CHECK( tau_api::set_ba_text_option("nlang-effort", "high").value()
			== "high" );

		// the empty text clears the option
		CHECK( tau_api::set_ba_text_option("nlang-model", "").has_value() );
		CHECK( nlang_llm_options().model.empty() );

		nlang_llm_options() = saved;
	}

	TEST_CASE("the api never reads the nlang key back") {
		const llm_options saved = nlang_llm_options();
		auto set = tau_api::set_ba_text_option("nlang-api-key", "sk-secret");
		REQUIRE( set.has_value() );
		CHECK( set.value() == "set" );
		CHECK( tau_api::get_ba_text_option("nlang-api-key").value() == "set" );
		CHECK( nlang_llm_options().api_key == "sk-secret" );
		nlang_llm_options() = saved;
	}

	TEST_CASE("a text option and a numeric one refuse each other's call") {
		for (const char* name : { "nlang-model", "nlang-api-key" }) {
			CAPTURE(name);
			auto set = tau_api::set_ba_option(name, 1);
			CHECK_FALSE( set.has_value() );
			CHECK( report_has_code(set.report(), code::invalid_argument) );
			CHECK_FALSE( tau_api::get_ba_option(name).has_value() );
		}
		for (const char* name : { "nlang-max-tokens", "nlang-fallback" }) {
			CAPTURE(name);
			auto set = tau_api::set_ba_text_option(name, "x");
			CHECK_FALSE( set.has_value() );
			CHECK( report_has_code(set.report(), code::invalid_argument) );
			CHECK_FALSE( tau_api::get_ba_text_option(name).has_value() );
		}
	}

	TEST_CASE("nlang-max-tokens and nlang-fallback round-trip through the api") {
		const llm_options saved = nlang_llm_options();
		CHECK( tau_api::set_ba_option("nlang-max-tokens", 512).value() == 512 );
		CHECK( nlang_llm_options().max_tokens == 512 );
		// 0 is the default, and the value in force is reported back
		CHECK( tau_api::set_ba_option("nlang-max-tokens", 0).value()
			== llm_default_max_tokens );
		REQUIRE( tau_api::set_ba_text_option("nlang-provider", "anthropic")
			.has_value() );
		CHECK( tau_api::set_ba_option("nlang-fallback", 0).value() == 0 );
		CHECK( tau_api::set_ba_option("nlang-fallback", 1).value() == 1 );
		nlang_llm_options() = saved;
	}
#endif

	// A family the pack owns, asked for an option it does not declare,
	// is a different miss from a family the pack lacks; both are not_found.
	TEST_CASE("BA options tell an unknown option from an unknown family") {
		const auto names = tau_api::ba_option_names();
		if (names.empty()) return;
		const std::string family = names.front().substr(0,
			names.front().find('-'));
		auto no_option = tau_api::get_ba_option(family + "-no-such-option");
		CHECK_FALSE( no_option.has_value() );
		CHECK( report_has_code(no_option.report(), code::not_found) );
		auto no_family = tau_api::get_ba_option("nosuchfamily-x");
		CHECK_FALSE( no_family.has_value() );
		CHECK( report_has_code(no_family.report(), code::not_found) );
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
		CHECK( interpreter<node_t>::gc_growth_factor
			== doctest::Approx(2.5) );
		CHECK( tau_api::get_gc_growth_factor() == doctest::Approx(2.5) );
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
	// The normalizer and tree caches are keyed on the formula alone: a
	// setter that changes a semantic option empties them, one that leaves
	// the options as they were keeps them.
	TEST_CASE("a semantic option change empties the tree caches") {
		using cache_t = subtree_unordered_map<node_t, tref>;
		static cache_t& cache = tau::template create_cache<cache_t>();
		tref key = tau::_T();
		auto fill = [&] { cache.clear(); cache.emplace(key, key); };
		const size_t saved_sr = max_simplify_rounds;
		const size_t saved_fp = max_fixpoint_steps;

		fill();
		tau_api::set_max_simplify_rounds(saved_sr);
		CHECK( cache.contains(key) );
		tau_api::set_max_simplify_rounds(saved_sr + 1);
		CHECK_FALSE( cache.contains(key) );
		tau_api::set_max_simplify_rounds(saved_sr);

		fill();
		tau_api::set_max_fixpoint_steps(saved_fp + 1);
		CHECK_FALSE( cache.contains(key) );
		tau_api::set_max_fixpoint_steps(saved_fp);

		fill();
		const auto names = tau_api::ba_option_names();
		if (!names.empty()) {
			auto before = tau_api::get_ba_option(names.front());
			REQUIRE( before.has_value() );
			const size_t v = before.value();
			CHECK( tau_api::set_ba_option(names.front(), v).has_value() );
			CHECK( cache.contains(key) );
			CHECK( tau_api::set_ba_option(names.front(),
				v == 0 ? 1 : 0).has_value() );
			CHECK_FALSE( cache.contains(key) );
			CHECK( tau_api::set_ba_option(names.front(), v).has_value() );
		}

		// a display option is not semantic
		fill();
		tau_api::set_indenting(!pretty_printer_indenting);
		tau_api::set_indenting(!pretty_printer_indenting);
		CHECK( cache.contains(key) );
		cache.clear();
	}

	// Every count limit the bindings enumerate reads back what its setter
	// wrote, through the getter of the same name.
	TEST_CASE("every count limit round-trips through its getter") {
		const auto limits = tau_api::count_limits();
		CHECK( limits.size() >= 37 );
		for (const auto& l : limits) {
			CAPTURE( l.name );
			const size_t saved = l.get();
			l.set(7);
			CHECK( l.get() == 7 );
			l.set(saved);
			CHECK( l.get() == saved );
		}
	}

	// The names follow the setters, so a binding can build set_<name> and
	// get_<name> from them.
	TEST_CASE("count limit names are unique and name a setter") {
		std::set<std::string> seen;
		for (const auto& l : tau_api::count_limits())
			CHECK( seen.insert(l.name).second );
		for (const char* n : { "max_fixpoint_steps", "ltl_mealy_max_states",
			"ltl_mealy_max_edges", "compile_max_table_edges",
			"ltl_max_observations", "ltl_data_game_max_combinations",
			"bf_dependence_max_nodes", "max_blast_reentry_depth" })
		{
			CAPTURE( n );
			CHECK( seen.contains(n) );
		}
	}

	TEST_CASE("the limits that are not a count read back") {
		const size_t to = ltl_timeout_sec_param;
		const std::string alg = ltl_algorithm_param;
		tau_api::set_ltl_timeout_sec(30);
		CHECK( tau_api::get_ltl_timeout_sec() == 30 );
		tau_api::set_ltl_timeout_sec(0);
		CHECK( tau_api::get_ltl_timeout_sec() == 0 );
		tau_api::set_ltl_algorithm("d");
		CHECK( tau_api::get_ltl_algorithm() == "D" );
		tau_api::set_ltl_algorithm("auto");
		CHECK( tau_api::get_ltl_algorithm() == "auto" );
		ltl_timeout_sec_param = to;
		ltl_algorithm_param = alg;
	}

	// The observation cap is at most its hard bound, and 0 means that bound.
	TEST_CASE("ltl observation cap clamps to its hard bound") {
		const size_t saved = ltl_max_observations_param;
		tau_api::set_ltl_max_observations(5);
		CHECK( tau_api::get_ltl_max_observations() == 5 );
		tau_api::set_ltl_max_observations(0);
		CHECK( tau_api::get_ltl_max_observations()
			== ltl_max_observations_hard );
		tau_api::set_ltl_max_observations(1000);
		CHECK( tau_api::get_ltl_max_observations()
			== ltl_max_observations_hard );
		ltl_max_observations_param = saved;
	}

	TEST_CASE("the new verdict caps are part of the budget fingerprint") {
		const size_t s1 = ltl_max_observations_param;
		const size_t s2 = ltl_data_game_max_combinations_param;
		const size_t base = verdict_budget_fingerprint<node_t>();
		tau_api::set_ltl_max_observations(3);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		ltl_max_observations_param = s1;
		CHECK( verdict_budget_fingerprint<node_t>() == base );
		tau_api::set_ltl_data_game_max_combinations(5);
		CHECK( verdict_budget_fingerprint<node_t>() != base );
		ltl_data_game_max_combinations_param = s2;
		CHECK( verdict_budget_fingerprint<node_t>() == base );
	}

	TEST_CASE("an env_limit resolves option, environment, default") {
		setenv("TAU_TEST_ENV_LIMIT_A", "17", 1);
		env_limit<size_t> a{ "TAU_TEST_ENV_LIMIT_A", 5 };
		CHECK( a.get() == 17 );
		a = 3;
		CHECK( a.get() == 3 );
		a.unset();
		CHECK( a.get() == 17 );
		unsetenv("TAU_TEST_ENV_LIMIT_A");
		env_limit<size_t> b{ "TAU_TEST_ENV_LIMIT_B", 5 };
		CHECK( b.get() == 5 );
		setenv("TAU_TEST_ENV_LIMIT_C", "abc", 1);
		env_limit<size_t> c{ "TAU_TEST_ENV_LIMIT_C", 5 };
		CHECK( c.get() == 5 );
		setenv("TAU_TEST_ENV_LIMIT_D", "0", 1);
		env_limit<size_t> d{ "TAU_TEST_ENV_LIMIT_D", 5,
			env_zero::unlimited };
		CHECK( d.get() == std::numeric_limits<size_t>::max() );
		env_limit<size_t> e{ "TAU_TEST_ENV_LIMIT_D", 5,
			env_zero::keep_default };
		CHECK( e.get() == 5 );
		env_limit<size_t> f{ "TAU_TEST_ENV_LIMIT_D", 5 };
		CHECK( f.get() == 0 );
		setenv("TAU_TEST_ENV_LIMIT_E", "2.5", 1);
		env_limit<double> g{ "TAU_TEST_ENV_LIMIT_E", 1.5 };
		CHECK( g.get() == doctest::Approx(2.5) );
		for (const char* v : { "TAU_TEST_ENV_LIMIT_C",
			"TAU_TEST_ENV_LIMIT_D", "TAU_TEST_ENV_LIMIT_E" })
				unsetenv(v);
	}
	// Every count setter keeps the count a valid long.
	TEST_CASE("count setters saturate a value above LONG_MAX") {
		constexpr long lmax = std::numeric_limits<long>::max();
		auto check = [](void (*set)(size_t), auto* param) {
			using T = std::remove_pointer_t<decltype(param)>;
			const T saved = *param;
			set(std::numeric_limits<size_t>::max());
			CHECK( *param == static_cast<T>(lmax) );
			set((size_t) lmax + 1);
			CHECK( *param == static_cast<T>(lmax) );
			set(42);
			CHECK( *param == 42 );
			*param = saved;
		};
		check(&tau_api::set_tref_budget, &tref_budget_param);
		check(&tau_api::set_tref_budget_soft_percent,
			&tref_budget_soft_param);
		check(&tau_api::set_max_consistency_subsets,
			&max_consistency_subsets_param);
		check(&tau_api::set_max_cover_products, &max_cover_products_param);
		check(&tau_api::set_ltl_hoa_max_states, &ltl_hoa_max_states_param);
		check(&tau_api::set_ltl_guard_max_cubes, &ltl_guard_max_cubes_param);
		check(&tau_api::set_ltl_max_refinement_rounds,
			&ltl_max_refinement_rounds_param);
		check(&tau_api::set_ltl_window_max_paths,
			&ltl_window_max_paths_param);
		check(&tau_api::set_ltl_closed_regions_timeout,
			&ltl_closed_regions_timeout_param);
		check(&tau_api::set_ltl_data_game_max_nodes,
			&ltl_data_game_max_nodes_param);
		check(&tau_api::set_ltl_data_game_max_memo,
			&ltl_data_game_max_memo_param);
		check(&tau_api::set_ltl_data_game_max_combinations,
			&ltl_data_game_max_combinations_param);
		check(&tau_api::set_ltl_max_observations,
			&ltl_max_observations_param);
	}
}
