#
# Every runtime limit falls back to a TAU_* environment variable until its
# flag, its REPL `set` or its api setter gives it a value: the variable shows
# through `get`, the flag wins over it, and garbage keeps the default and says
# so. The limits of the LTL(ABA) route and the tree-node budget have their
# own cases in test_repl-ltl_env.cmake.
#

include(add_repl_test)

# Each row: testname|variable|value|get-option|expected|long flag|flag value.
# A BA-declared option names its algebra in the command, which gates the case
# on that algebra being in the pack.
set(TAU_ENV_LIMIT_ROWS
	"spec_size_warn|TAU_SPEC_SIZE_WARN|99|specsizewarn|99|spec-size-warn|98"
	"max_revision_alts|TAU_MAX_REVISION_ALTS|5|revisionalts|5|max-revision-alts|6"
	"block_max_splits|TAU_BLOCK_MAX_SPLITS|7|maxsplits|7|block-max-splits|9"
	"block_max_rounds|TAU_BLOCK_MAX_ROUNDS|7|maxrounds|7|block-max-rounds|9"
	"ba_decision_pins|TAU_BA_DECISION_PINS|7|decisionpins|7|ba-decision-pins|9"
	"cqe_max_clauses|TAU_CQE_MAX_CLAUSES|7|maxclauses|7|cqe-max-clauses|9"
	"lgrs_max_vars|TAU_LGRS_MAX_VARS|7|lgrsmaxvars|7|lgrs-max-vars|9"
	"max_fixpoint_steps|TAU_MAX_FIXPOINT_STEPS|7|fixpointsteps|7|max-fixpoint-steps|9"
	"max_flag_search_steps|TAU_MAX_FLAG_SEARCH_STEPS|7|flagsteps|7|max-flag-search-steps|9"
	"block_squeeze_cap|TAU_BLOCK_SQUEEZE_CAP|7|squeezecap|7|block-squeeze-cap|9"
	"max_simplify_rounds|TAU_MAX_SIMPLIFY_ROUNDS|7|simplifyrounds|7|max-simplify-rounds|9"
	"max_def_passes|TAU_MAX_DEF_PASSES|7|defpasses|7|max-def-passes|9"
	"max_enum_steps|TAU_MAX_ENUM_STEPS|7|enumsteps|7|max-enum-steps|9"
	"max_probe_steps|TAU_MAX_PROBE_STEPS|7|probesteps|7|max-probe-steps|9"
	"max_rewrite_rounds|TAU_MAX_REWRITE_ROUNDS|7|rewriterounds|7|max-rewrite-rounds|9"
	"max_constant_size|TAU_MAX_CONSTANT_SIZE|7|maxconstantsize|7|max-constant-size|9"
	"cache_bound|TAU_CACHE_BOUND|7|cachebound|7|cache-bound|9"
	"gc_min_size|TAU_GC_MIN_SIZE|7|gcminsize|7|gc-min-size|9"
	"gc_growth_factor|TAU_GC_GROWTH_FACTOR|2.5|gcgrowth|2.5|gc-growth-factor|3.5"
	"ltl_data_game_max_combinations|TAU_LTL_DATA_GAME_MAX_COMBINATIONS|7|ltldatagamemaxcombinations|7|ltl-data-game-max-combinations|9"
	"ltl_max_observations|TAU_LTL_MAX_OBSERVATIONS|7|ltlmaxobservations|7|ltl-max-observations|9"
	"ltl_mealy_max_states|TAU_LTL_MEALY_MAX_STATES|7|ltlmealymaxstates|7|ltl-mealy-max-states|9"
	"ltl_mealy_max_edges|TAU_LTL_MEALY_MAX_EDGES|7|ltlmealymaxedges|7|ltl-mealy-max-edges|9"
	"compile_max_table_edges|TAU_COMPILE_MAX_TABLE_EDGES|7|compilemaxtableedges|7|compile-max-table-edges|9"
	"compile_build_timeout|TAU_COMPILE_BUILD_TIMEOUT|7|compilebuildtimeout|7 s|compile-build-timeout|9"
	"bf_dependence_max_nodes|TAU_BF_DEPENDENCE_MAX_NODES|7|bfdependencemaxnodes|7|bf-dependence-max-nodes|9"
	"bv_blastdepth|TAU_BV_BLASTDEPTH|7|bv-blastdepth|7|bv-blastdepth|9"
	"bv_case_split_max_tests|TAU_BV_CASE_SPLIT_MAX_TESTS|7|bv-case-split-max-tests|7|bv-case-split-max-tests|9"
	"bv_defelim_max_clauses|TAU_BV_DEFELIM_MAX_CLAUSES|7|bv-defelim-max-clauses|7|bv-defelim-max-clauses|9"
	"bv_defelim_max_atoms|TAU_BV_DEFELIM_MAX_ATOMS|7|bv-defelim-max-atoms|7|bv-defelim-max-atoms|9"
	"bv_defelim_max_subset|TAU_BV_DEFELIM_MAX_SUBSET|7|bv-defelim-max-subset|7|bv-defelim-max-subset|9"
	"bv_defelim_max_rounds|TAU_BV_DEFELIM_MAX_ROUNDS|7|bv-defelim-max-rounds|7|bv-defelim-max-rounds|9"
	"bv_blasting_max_nodes|TAU_BV_BLASTING_MAX_NODES|7|bv-blasting-max-nodes|7|bv-blasting-max-nodes|9"
	"bv_bitblast_max_nodes|TAU_BV_BITBLAST_MAX_NODES|7|bv-bitblast-max-nodes|7|bv-bitblast-max-nodes|9"
	"bv_bitblast_max_width|TAU_BV_BITBLAST_MAX_WIDTH|7|bv-bitblast-max-width|7|bv-bitblast-max-width|9"
	"bv_solve_timeout|TAU_BV_SOLVE_TIMEOUT|7|bv-solve-timeout|7|bv-solve-timeout|9"
	"bv_max_width|TAU_BV_MAX_WIDTH|7|bv-max-width|7|bv-max-width|9"
)
foreach(row IN LISTS TAU_ENV_LIMIT_ROWS)
	string(REPLACE "|" ";" f "${row}")
	list(GET f 0 nm)
	list(GET f 1 var)
	list(GET f 2 val)
	list(GET f 3 opt)
	list(GET f 4 expect)
	list(GET f 5 flag)
	list(GET f 6 fval)
	add_repl_test(limit_env-${nm}-env_is_the_fallback
		"get ${opt}" "${opt}: *${expect}" NO_FAIL_REGEX NO_TRACE
		ENV ${var}=${val})
	add_repl_test(limit_env-${nm}-flag_beats_env
		"get ${opt}" "${opt}: *${fval}" NO_FAIL_REGEX NO_TRACE
		ENV ${var}=${val} FLAGS --${flag} ${fval})
	add_repl_test(limit_env-${nm}-set_beats_env
		"set ${opt} ${fval}. get ${opt}" "${opt}: *${fval}" NO_FAIL_REGEX
		NO_TRACE ENV ${var}=${val})
	# --help names no algebra, so a BA-declared row says which it needs
	set(need "")
	if(nm MATCHES "^bv_")
		set(need REQUIRES bv)
	endif()
	add_raw_repl_test(limit_env-${nm}-help_names_the_variable
		"${TAU_RUN} --help"
		"${var}" NO_FAIL_REGEX ${need})
endforeach()

# Garbage in a count variable is an error.
add_env_error_test(limit_env-garbage_count_is_an_error
	TAU_MAX_FIXPOINT_STEPS=abc)
# Garbage keeps the default and says so once.
add_repl_test(limit_env-garbage_real_warns
	"get gcgrowth" "TAU_GC_GROWTH_FACTOR='abc' is not a number"
	NO_FAIL_REGEX NO_TRACE ENV TAU_GC_GROWTH_FACTOR=abc)
add_repl_test(limit_env-garbage_real_keeps_default
	"get gcgrowth" "gcgrowth: *1.5"
	NO_FAIL_REGEX NO_TRACE ENV TAU_GC_GROWTH_FACTOR=abc)
add_repl_test(limit_env-garbage_bv_option_warns
	"get bv-defelim-max-atoms"
	"TAU_BV_DEFELIM_MAX_ATOMS='-3' is not a non-negative number"
	NO_FAIL_REGEX NO_TRACE ENV TAU_BV_DEFELIM_MAX_ATOMS=-3)

# 0 in a variable means what 0 means to the setter.
add_repl_test(limit_env-zero_is_unlimited_for_a_decrementing_budget
	"get maxsplits" "maxsplits: *unlimited"
	NO_FAIL_REGEX NO_TRACE ENV TAU_BLOCK_MAX_SPLITS=0)
add_repl_test(limit_env-zero_is_unlimited_for_lgrs
	"get lgrsmaxvars" "lgrsmaxvars: *unlimited"
	NO_FAIL_REGEX NO_TRACE ENV TAU_LGRS_MAX_VARS=0)
add_repl_test(limit_env-zero_is_unlimited_for_a_cap
	"get fixpointsteps" "fixpointsteps: *unlimited"
	NO_FAIL_REGEX NO_TRACE ENV TAU_MAX_FIXPOINT_STEPS=0)
add_repl_test(limit_env-zero_keeps_the_bv_max_width
	"get bv-max-width" "bv-max-width: *1024"
	NO_FAIL_REGEX NO_TRACE ENV TAU_BV_MAX_WIDTH=0)
add_repl_test(limit_env-zero_observations_is_the_hard_bound
	"get ltlmaxobservations" "ltlmaxobservations: *30"
	NO_FAIL_REGEX NO_TRACE ENV TAU_LTL_MAX_OBSERVATIONS=0)

# A flag that is not given leaves the variable in force; a garbage flag is an
# error rather than atoll's 0, which would mean unlimited.
add_repl_test(limit_env-flag_rejects_garbage
	"get fixpointsteps" "--max-fixpoint-steps expects a non-negative number"
	NO_FAIL_REGEX NO_TRACE FLAGS --max-fixpoint-steps abc)
add_repl_test(limit_env-real_flag_rejects_garbage
	"get gcgrowth" "--gc-growth-factor expects a number"
	NO_FAIL_REGEX NO_TRACE FLAGS --gc-growth-factor abc)
add_repl_test(limit_env-new_flag_rejects_garbage
	"get ltlmealymaxstates" "--ltl-mealy-max-states expects a non-negative number"
	NO_FAIL_REGEX NO_TRACE FLAGS --ltl-mealy-max-states -1)
