#
# LT-27 / SY-RT4: the diagnostic environment paths of the ltlsynt call.
# Each one is observed through the REPL `ltl` command with the variable
# set for that single invocation.  NOTE: never use `(.*\n)*` to bridge two
# markers here -- the HOA dump is long enough that ctest's regex engine
# backtracks for minutes; match one marker only.
# The spec must go through ltlsynt (a plain
# `always` takes the safety path and never spawns it).  All need a live
# ltlsynt on PATH (as the
# other `ltl` REPL tests do).
#

include(add_repl_test)
include(tau_repl_pack)

# TAU_LTL_EXPORT_STRATEGY=hoa prints the strategy to stderr.
add_repl_test(ltl_env-export_hoa
	"ltl F (o1[t] = 1)"
	"=== STRATEGY HOA ===" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_EXPORT_STRATEGY=hoa REQUIRES ltlsynt)

# TAU_LTL_EXPORT_STRATEGY=dot prints dot (or falls back to HOA when autfilt
# is missing) -- either way a STRATEGY banner appears.
add_repl_test(ltl_env-export_dot
	"ltl F (o1[t] = 1)"
	"=== STRATEGY (DOT|HOA \\(dot unavailable\\)) ===" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_EXPORT_STRATEGY=dot REQUIRES ltlsynt)

# Without the variable nothing is exported.
add_repl_test(ltl_env-export_off_by_default
	"ltl F (o1[t] = 1)"
	"REALIZABLE" NO_TRACE
	FAIL_REGEX "=== STRATEGY" REQUIRES ltlsynt)

# TAU_LTL_WITNESS=1 on an UNREALIZABLE spec prints the environment's
# counter-strategy (the negated, role-swapped game is realizable): for
# F(i1 = 1), after the atom legend, one state in which the environment,
# which owns p0, never sets it.
add_repl_test(ltl_env-witness
	"ltl F (i1[t] = 1)"
	"=== ENV COUNTER-STRATEGY \\(UNREAL witness\\) ===\n(  p[0-9]+ := [^\n]+\n)*HOA: v1\nStates: 1\nStart: 0\n[^\n]+\n[^\n]+\n[^\n]+\n[^\n]+\ncontrollable-AP: 0\n--BODY--\nState: 0\n\\[!0\\] 0\n--END--"
	NO_FAIL_REGEX NO_TRACE ENV TAU_LTL_WITNESS=1 REQUIRES ltlsynt)

# An ltlsynt older than Spot 2.16 decides the verdict but refuses the
# --semantics=Moore of the counter-strategy game: ltl-witness and explain
# say why there is no counter-strategy instead of printing nothing.
if(NOT CMAKE_HOST_WIN32 AND TAU_REPL_LTLSYNT_USABLE)
	set(_old "${CMAKE_CURRENT_BINARY_DIR}/old_ltlsynt")
	file(WRITE "${_old}/ltlsynt"
		"#!/bin/sh\n"
		"case \"$1\" in --version) echo 'ltlsynt (spot) 2.15.1'; exit 0;; esac\n"
		"for a in \"$@\"; do case \"$a\" in --semantics=Moore)\n"
		"\techo 'ltlsynt: Moore semantics are not supported yet' >&2; exit 2;;\n"
		"esac; done\n"
		"exec \"${TAU_LTLSYNT_PROGRAM}\" \"$@\"\n")
	file(CHMOD "${_old}/ltlsynt" PERMISSIONS OWNER_READ OWNER_WRITE
		OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	set(_no_moore "ltlsynt did not solve the dual game, which runs --semantics=Moore \\(Spot >= 2.16\\)")
	add_raw_repl_test(ltl_env-witness_old_ltlsynt_says_why
		"PATH=${_old}:$PATH TAU_LTL_WITNESS=1 ${TAU_RUN} -e \"ltl F (i1[t] = 1)\""
		"=== ENV COUNTER-STRATEGY \\(UNREAL witness\\) ===\nnone: ${_no_moore}"
		FAIL_REGEX "HOA: v1" REQUIRES ltlsynt)
	add_raw_repl_test(ltl_env-explain_old_ltlsynt_says_why
		"PATH=${_old}:$PATH ${TAU_RUN} -e \"explain (G(o1[t]:bv[1] = 1 -> i1[t]:bv[1] = 0)) && (F(o1[t]:bv[1] = 1))\""
		"unrealizable core:.*no counter-strategy: ${_no_moore}"
		FAIL_REGEX "(^|\n)environment counter-strategy:" REQUIRES ltlsynt bv)

	# The two other reasons: the dual game answered unrealizable, and the dual
	# game outlived ltl-timeout. Each fake answers the verdict game itself.
	foreach(_fake unreal slow)
		if(_fake STREQUAL "unreal")
			set(_moore "\techo UNREALIZABLE; exit 1;;\n")
			set(_why "ltlsynt found the dual game unrealizable")
		else()
			set(_moore "\texec sleep 30;;\n")
			set(_why "ltlsynt did not solve the dual game within ltl-timeout")
		endif()
		set(_dir "${CMAKE_CURRENT_BINARY_DIR}/${_fake}_dual_ltlsynt")
		file(WRITE "${_dir}/ltlsynt"
			"#!/bin/sh\n"
			"for a in \"$@\"; do case \"$a\" in --semantics=Moore)\n"
			"${_moore}"
			"esac; done\n"
			"exec \"${TAU_LTLSYNT_PROGRAM}\" \"$@\"\n")
		file(CHMOD "${_dir}/ltlsynt" PERMISSIONS OWNER_READ OWNER_WRITE
			OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
		add_raw_repl_test(ltl_env-witness_${_fake}_dual_says_why
			"PATH=${_dir}:$PATH TAU_LTL_WITNESS=1 ${TAU_RUN} -e \"set ltl-timeout 2. ltl F (i1[t] = 1)\""
			"=== ENV COUNTER-STRATEGY \\(UNREAL witness\\) ===\nnone: ${_why}"
			FAIL_REGEX "HOA: v1" REQUIRES ltlsynt)
	endforeach()
endif()

# TAU_LTL_SIMPLIFICATION reaches ltlsynt as --simplify=: a valid level keeps
# the verdict, `none` is stored as `no`, and any other word is an error.
add_repl_test(ltl_env-simplification_level_keeps_verdict
	"ltl F (o1[t] = 1)"
	"[^N]REALIZABLE" NO_TRACE
	FAIL_REGEX "UNKNOWN|UNREALIZABLE"
	ENV TAU_LTL_SIMPLIFICATION=bwoa REQUIRES ltlsynt)
add_repl_test(ltl_env-simplification_none_is_no
	"get ltl-simplification" "ltl-simplification: *no" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_SIMPLIFICATION=none)
add_env_error_test(ltl_env-simplification_garbage_is_an_error
	TAU_LTL_SIMPLIFICATION=definitely-not-a-level)

# TAU_LTL_SYNTH_ALGO and TAU_LTL_DECOMPOSE reach ltlsynt as --algo= and
# --decompose=; neither changes a verdict, and a word ltlsynt does not know
# is an error.
add_repl_test(ltl_env-synth_algo_keeps_verdict
	"ltl F (o1[t] = 1)"
	"[^N]REALIZABLE" NO_TRACE
	FAIL_REGEX "UNKNOWN|UNREALIZABLE"
	ENV TAU_LTL_SYNTH_ALGO=ps REQUIRES ltlsynt)
add_repl_test(ltl_env-synth_algo_keeps_unrealizable
	"ltl F (i1[t] = 1)"
	"UNREALIZABLE" NO_TRACE
	FAIL_REGEX "UNKNOWN|[^N]REALIZABLE"
	ENV TAU_LTL_SYNTH_ALGO=lar REQUIRES ltlsynt)
add_env_error_test(ltl_env-synth_algo_garbage_is_an_error
	TAU_LTL_SYNTH_ALGO=fastest)
add_repl_test(ltl_env-decompose_keeps_verdict
	"ltl F (o1[t] = 1)"
	"[^N]REALIZABLE" NO_TRACE
	FAIL_REGEX "UNKNOWN|UNREALIZABLE"
	ENV TAU_LTL_DECOMPOSE=no REQUIRES ltlsynt)
add_repl_test(ltl_env-decompose_set_get
	"set ltl-decompose yes. get ltl-decompose"
	"ltl-decompose: *yes" NO_FAIL_REGEX NO_TRACE)
add_env_error_test(ltl_env-decompose_garbage_is_an_error
	TAU_LTL_DECOMPOSE=maybe)

# A game is cached with the caps it was parsed under: after the state cap
# is lowered below the 4 states of this arena, the cached game is not
# served and the data game refuses the arena.
add_repl_test(ltl_env-game_cache_follows_hoa_state_cap
	"realizable F (o1[t] = 0). set ltl-hoa-max-states 1. realizable F (o1[t] = 0)."
	"REALIZABLE \\(data game\\).*ltl-hoa-max-states: *1.*bad state count"
	NO_TRACE FAIL_REGEX "ltl-hoa-max-states: *1[^[]*\\[ltl_aba\\] REALIZABLE \\(data game\\)"
	REQUIRES ltlsynt)

# TAU_LTL_TIMEOUT garbage is an error.
add_env_error_test(ltl_env-timeout_garbage_is_an_error TAU_LTL_TIMEOUT=abc)

# A variable writes its option when tau starts: the value shows through
# `get`, and the CLI flag, written after it, wins when both are given.
add_repl_test(ltl_env-timeout_env_is_the_fallback
	"get ltl-timeout" "ltl-timeout: *5" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_TIMEOUT=5)
add_repl_test(ltl_env-timeout_flag_beats_env
	"get ltl-timeout" "ltl-timeout: *9" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-timeout 9 ENV TAU_LTL_TIMEOUT=5)
add_repl_test(ltl_env-timeout_flag_rejects_garbage
	"get ltl-timeout" "does not take the value.*name=ltl-timeout" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-timeout abc)
add_repl_test(ltl_env-alg_env_is_the_fallback
	"get ltl-alg" "ltl-alg: *D" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_ALG=D)
add_repl_test(ltl_env-alg_flag_beats_env
	"get ltl-alg" "ltl-alg: *A" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-alg A ENV TAU_LTL_ALG=D)
add_env_error_test(ltl_env-alg_garbage_is_an_error TAU_LTL_ALG=C)
add_env_error_test(ltl_env-qe_garbage_is_an_error TAU_LTL_QE_MAX_VARS=abc)
# A value above one day clamps.
add_repl_test(ltl_env-timeout_env_clamps
	"get ltl-timeout" "ltl-timeout: *86400" NO_TRACE
	ENV TAU_LTL_TIMEOUT=100000)
add_repl_test(ltl_env-qe_env_is_the_fallback
	"get ltl-qe-max-vars" "ltl-qe-max-vars: *4" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_QE_MAX_VARS=4)
add_repl_test(ltl_env-qe_flag_beats_env
	"get ltl-qe-max-vars" "ltl-qe-max-vars: *3" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-qe-max-vars 3 ENV TAU_LTL_QE_MAX_VARS=4)
# BA-declared knobs promoted from header constants. The option exists only
# when its algebra is in the pack, so each is gated on its own BA.
add_repl_test(ltl_env-qlt_t3_cap_option
	"get qlt-t3-cap" "qlt-t3-cap: *12" NO_FAIL_REGEX NO_TRACE
	FLAGS --qlt-t3-cap 12)
add_repl_test(ltl_env-nlang_http_timeout_option
	"set nlang-http-timeout 3" "nlang-http-timeout: *3" NO_FAIL_REGEX NO_TRACE)
add_repl_test(ltl_env-refinement_rounds_flag
	"get ltl-refinement-rounds" "ltl-refinement-rounds: *5" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-refinement-rounds 5)
add_repl_test(ltl_env-window_max_paths_flag
	"get ltl-window-max-paths" "ltl-window-max-paths: *0" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-window-max-paths 0)
add_repl_test(ltl_env-closed_regions_timeout_flag
	"get ltl-closed-regions-timeout" "ltl-closed-regions-timeout: *7" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-closed-regions-timeout 7)
add_repl_test(ltl_env-closed_regions_timeout_default
	"get ltl-closed-regions-timeout" "ltl-closed-regions-timeout: *20" NO_FAIL_REGEX NO_TRACE)
add_repl_test(ltl_env-closed_regions_timeout_set
	"set ltl-closed-regions-timeout 0. get ltl-closed-regions-timeout"
	"ltl-closed-regions-timeout: *0" NO_FAIL_REGEX NO_TRACE)
add_repl_test(ltl_env-closed_regions_timeout_env_is_the_fallback
	"get ltl-closed-regions-timeout" "ltl-closed-regions-timeout: *5" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_CLOSED_REGIONS_TIMEOUT=5)
add_repl_test(ltl_env-closed_regions_timeout_flag_beats_env
	"get ltl-closed-regions-timeout" "ltl-closed-regions-timeout: *9" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_CLOSED_REGIONS_TIMEOUT=5 FLAGS --ltl-closed-regions-timeout 9)
add_repl_test(ltl_env-pwr_semantic_flag
	"get pwr-semantic" "pwr-semantic: *true" NO_FAIL_REGEX NO_TRACE
	FLAGS --pwr-semantic)
add_repl_test(ltl_env-qlt_const_output_max_flag
	"get qlt-const-output-max" "qlt-const-output-max: *3" NO_FAIL_REGEX NO_TRACE
	FLAGS --qlt-const-output-max 3)
# The -K short flag belongs to --ba-component-factoring; --ltl-qe-max-vars is -k.
add_repl_test(ltl_env-qe_short_flag_is_lowercase_k
	"get ltl-qe-max-vars" "ltl-qe-max-vars: *3" NO_FAIL_REGEX NO_TRACE
	FLAGS -k 3)

# Every runtime limit of the pipeline carries all three surfaces: a CLI flag,
# a REPL option and a TAU_* environment variable. The variable shows through
# `get` when the flag is absent, the flag wins when both are given, and a
# garbage value is an error.
add_repl_test(ltl_env-hoa_max_states_env_is_the_fallback
	"get ltl-hoa-max-states" "ltl-hoa-max-states: *7" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_HOA_MAX_STATES=7)
add_repl_test(ltl_env-hoa_max_states_flag_beats_env
	"get ltl-hoa-max-states" "ltl-hoa-max-states: *9" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_HOA_MAX_STATES=7 FLAGS --ltl-hoa-max-states 9)
add_env_error_test(ltl_env-hoa_max_states_garbage_is_an_error
	TAU_LTL_HOA_MAX_STATES=abc)
add_repl_test(ltl_env-ctl_star_tableau_max_states_env_is_the_fallback
	"get ltl-ctl-star-tableau-max-states" "ltl-ctl-star-tableau-max-states: *5" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_CTL_STAR_TABLEAU_MAX_STATES=5)
add_repl_test(ltl_env-ctl_star_tableau_max_states_flag_beats_env
	"get ltl-ctl-star-tableau-max-states" "ltl-ctl-star-tableau-max-states: *9" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_CTL_STAR_TABLEAU_MAX_STATES=5 FLAGS --ltl-ctl-star-tableau-max-states 9)
add_repl_test(ltl_env-hoa_max_states_flag_rejects_garbage
	"get ltl-hoa-max-states" "does not take the value.*name=ltl-hoa-max-states" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-hoa-max-states abc)
add_repl_test(ltl_env-guard_max_cubes_env_is_the_fallback
	"get ltl-guard-max-cubes" "ltl-guard-max-cubes: *6" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_GUARD_MAX_CUBES=6)
add_repl_test(ltl_env-refinement_rounds_env_is_the_fallback
	"get ltl-refinement-rounds" "ltl-refinement-rounds: *5" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_REFINEMENT_ROUNDS=5)
add_repl_test(ltl_env-refinement_rounds_flag_beats_env
	"get ltl-refinement-rounds" "ltl-refinement-rounds: *8" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_REFINEMENT_ROUNDS=5 FLAGS --ltl-refinement-rounds 8)
# 0 is a value, not an absence: it means unlimited, and the variable can say so.
add_repl_test(ltl_env-window_max_paths_env_is_the_fallback
	"get ltl-window-max-paths" "ltl-window-max-paths: *0" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_WINDOW_MAX_PATHS=0)
add_repl_test(ltl_env-window_max_paths_flag_beats_env
	"get ltl-window-max-paths" "ltl-window-max-paths: *12" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_WINDOW_MAX_PATHS=0 FLAGS --ltl-window-max-paths 12)
add_repl_test(ltl_env-data_game_max_nodes_flag
	"get ltl-data-game-max-nodes" "ltl-data-game-max-nodes: *0" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-data-game-max-nodes 0)
add_repl_test(ltl_env-data_game_max_nodes_env_is_the_fallback
	"get ltl-data-game-max-nodes" "ltl-data-game-max-nodes: *4096" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_DATA_GAME_MAX_NODES=4096)
add_repl_test(ltl_env-data_game_max_nodes_flag_beats_env
	"get ltl-data-game-max-nodes" "ltl-data-game-max-nodes: *12" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_DATA_GAME_MAX_NODES=4096 FLAGS --ltl-data-game-max-nodes 12)
add_repl_test(ltl_env-data_game_max_nodes_flag_rejects_garbage
	"get ltl-data-game-max-nodes" "does not take the value.*name=ltl-data-game-max-nodes" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-data-game-max-nodes abc)
add_repl_test(ltl_env-data_game_max_memo_flag
	"get ltl-data-game-max-memo" "ltl-data-game-max-memo: *0" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-data-game-max-memo 0)
add_repl_test(ltl_env-data_game_max_memo_env_is_the_fallback
	"get ltl-data-game-max-memo" "ltl-data-game-max-memo: *4096" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_DATA_GAME_MAX_MEMO=4096)
add_repl_test(ltl_env-data_game_max_memo_flag_beats_env
	"get ltl-data-game-max-memo" "ltl-data-game-max-memo: *12" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_DATA_GAME_MAX_MEMO=4096 FLAGS --ltl-data-game-max-memo 12)
add_env_error_test(ltl_env-data_game_max_memo_garbage_is_an_error
	TAU_LTL_DATA_GAME_MAX_MEMO=abc)
# The consistency-subset and coverage-product caps read the environment too.
add_repl_test(ltl_env-consistency_subsets_env_is_the_fallback
	"get max-consistency-subsets" "max-consistency-subsets: *7" NO_FAIL_REGEX NO_TRACE
	ENV TAU_MAX_CONSISTENCY_SUBSETS=7)
add_repl_test(ltl_env-consistency_subsets_flag_beats_env
	"get max-consistency-subsets" "max-consistency-subsets: *9" NO_FAIL_REGEX NO_TRACE
	ENV TAU_MAX_CONSISTENCY_SUBSETS=7 FLAGS --max-consistency-subsets 9)
add_repl_test(ltl_env-consistency_subsets_flag_rejects_garbage
	"get max-consistency-subsets" "does not take the value.*name=max-consistency-subsets" NO_FAIL_REGEX NO_TRACE
	FLAGS --max-consistency-subsets abc)
add_repl_test(ltl_env-cover_products_env_is_the_fallback
	"get max-cover-products" "max-cover-products: *0" NO_FAIL_REGEX NO_TRACE
	ENV TAU_MAX_COVER_PRODUCTS=0)
add_repl_test(ltl_env-cover_products_flag_beats_env
	"get max-cover-products" "max-cover-products: *9" NO_FAIL_REGEX NO_TRACE
	ENV TAU_MAX_COVER_PRODUCTS=0 FLAGS --max-cover-products 9)
# The same three surfaces for the caps an algebra declares about itself; each
# exists only when its algebra is in the pack, so the family gates each one.
add_repl_test(ltl_env-qlt_t3_cap_env_is_the_fallback
	"get qlt-t3-cap" "qlt-t3-cap: *14" NO_FAIL_REGEX NO_TRACE
	ENV TAU_QLT_T3_CAP=14)
add_repl_test(ltl_env-qlt_t3_cap_flag_beats_env
	"get qlt-t3-cap" "qlt-t3-cap: *12" NO_FAIL_REGEX NO_TRACE
	ENV TAU_QLT_T3_CAP=14 FLAGS --qlt-t3-cap 12)
add_env_error_test(ltl_env-qlt_t3_cap_garbage_is_an_error
	TAU_QLT_T3_CAP=abc REQUIRES qlt)
add_repl_test(ltl_env-qlt_const_output_max_env_is_the_fallback
	"get qlt-const-output-max" "qlt-const-output-max: *4" NO_FAIL_REGEX NO_TRACE
	ENV TAU_QLT_CONST_OUTPUT_MAX=4)
add_repl_test(ltl_env-qlt_const_output_max_flag_beats_env
	"get qlt-const-output-max" "qlt-const-output-max: *3" NO_FAIL_REGEX NO_TRACE
	ENV TAU_QLT_CONST_OUTPUT_MAX=4 FLAGS --qlt-const-output-max 3)
add_repl_test(ltl_env-qlt_cells_budget_env_is_the_fallback
	"get qlt-cells-budget" "qlt-cells-budget: *77" NO_FAIL_REGEX NO_TRACE
	ENV TAU_QLT_CELLS_BUDGET=77)
add_repl_test(ltl_env-qlt_cells_budget_flag_beats_env
	"get qlt-cells-budget" "qlt-cells-budget: *5" NO_FAIL_REGEX NO_TRACE
	ENV TAU_QLT_CELLS_BUDGET=77 FLAGS --qlt-cells-budget 5)
add_repl_test(ltl_env-qlt_cells_max_params_env_is_the_fallback
	"get qlt-cells-max-params" "qlt-cells-max-params: *3" NO_FAIL_REGEX NO_TRACE
	ENV TAU_QLT_CELLS_MAX_PARAMS=3)
add_repl_test(ltl_env-qlt_cells_max_params_flag_beats_env
	"get qlt-cells-max-params" "qlt-cells-max-params: *1" NO_FAIL_REGEX NO_TRACE
	ENV TAU_QLT_CELLS_MAX_PARAMS=3 FLAGS --qlt-cells-max-params 1)
add_repl_test(ltl_env-nlang_http_timeout_env_is_the_fallback
	"get nlang-http-timeout" "nlang-http-timeout: *9" NO_FAIL_REGEX NO_TRACE
	ENV TAU_NLANG_HTTP_TIMEOUT=9)
add_repl_test(ltl_env-nlang_http_timeout_flag_beats_env
	"get nlang-http-timeout" "nlang-http-timeout: *3" NO_FAIL_REGEX NO_TRACE
	ENV TAU_NLANG_HTTP_TIMEOUT=9 FLAGS --nlang-http-timeout 3)
# A BA-declared count option left unset keeps the algebra's own default: the
# CLI passes nothing, so an algebra's environment fallback is not shadowed.
add_repl_test(ltl_env-qlt_t3_cap_default_without_flag
	"get qlt-t3-cap" "qlt-t3-cap: *20" NO_FAIL_REGEX NO_TRACE)

# TAU_TREF_BUDGET is the memory budget's environment form: it shows through
# `get` when the flag is unset, and the flag wins when both are given.
add_repl_test(tref_budget_env_is_the_fallback
	"get tref-budget" "tref-budget: *4096" NO_FAIL_REGEX NO_TRACE
	ENV TAU_TREF_BUDGET=4096)
add_repl_test(tref_budget_flag_beats_env
	"get tref-budget" "tref-budget: *8192" NO_FAIL_REGEX NO_TRACE
	ENV TAU_TREF_BUDGET=4096 FLAGS --tref-budget 8192)

# A budget no session can meet refuses the command instead of answering it,
# and says which knob set the cap.
add_repl_test(tref_budget_refuses_when_exhausted
	"dnf (x & y) | z" "memory budget exhausted.*--tref-budget" NO_FAIL_REGEX NO_TRACE
	FLAGS --tref-budget 1)

# Unlimited is the shipped default: the same command answers normally.
add_repl_test(tref_budget_unlimited_by_default
	"dnf (x & y) | z" "xy\\|z" NO_TRACE
	FAIL_REGEX "memory budget exhausted")
