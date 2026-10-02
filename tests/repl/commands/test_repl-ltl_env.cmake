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
# counter-strategy (the negated, role-swapped game is realizable).
add_repl_test(ltl_env-witness
	"ltl F (i1[t] = 1)"
	"=== ENV COUNTER-STRATEGY \\(UNREAL witness\\) ===" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_WITNESS=1 REQUIRES ltlsynt)

# A bad TAU_LTL_SIMPLIFICATION value is an ltlsynt usage error (exit 2):
# no verdict, reported as UNKNOWN -- never REALIZABLE or UNREALIZABLE.
add_repl_test(ltl_env-bad_simplification_is_unknown
	"ltl F (o1[t] = 1)"
	"UNKNOWN" NO_TRACE
	FAIL_REGEX "[^N]REALIZABLE|UNREALIZABLE"
	ENV TAU_LTL_SIMPLIFICATION=definitely-not-a-level REQUIRES ltlsynt)

# TAU_LTL_TIMEOUT_SEC garbage keeps the default and says so.
add_repl_test(ltl_env-timeout_garbage_warns
	"ltl F (o1[t] = 1)"
	"TAU_LTL_TIMEOUT_SEC='abc' is not a non-negative number" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_TIMEOUT_SEC=abc REQUIRES ltlsynt)

# The environment variables are fallbacks of the runtime parameters: the
# variable shows through `get` when the option is unset, and the CLI flag
# wins when both are given.
add_repl_test(ltl_env-timeout_env_is_the_fallback
	"get ltltimeout" "ltltimeout: *5s" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_TIMEOUT_SEC=5)
add_repl_test(ltl_env-timeout_flag_beats_env
	"get ltltimeout" "ltltimeout: *9s" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-timeout 9 ENV TAU_LTL_TIMEOUT_SEC=5)
add_repl_test(ltl_env-timeout_flag_rejects_garbage
	"get ltltimeout" "expects a non-negative number" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-timeout abc)
add_repl_test(ltl_env-alg_env_is_the_fallback
	"get ltlalg" "ltlalg: *D" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_ALG=D)
add_repl_test(ltl_env-alg_flag_beats_env
	"get ltlalg" "ltlalg: *A" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-alg A ENV TAU_LTL_ALG=D)
add_repl_test(ltl_env-alg_garbage_reads_as_auto
	"get ltlalg" "ltlalg: *auto" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_ALG=C)
add_repl_test(ltl_env-qe_env_is_validated
	"get ltlqemaxvars" "keeping the default 2" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_OMCAT_QE_MAX_VARS=abc)
add_repl_test(ltl_env-qe_flag_beats_env
	"get ltlqemaxvars" "ltlqemaxvars: *3" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-qe-max-vars 3 ENV TAU_LTL_OMCAT_QE_MAX_VARS=4)
# BA-declared knobs promoted from header constants. The option exists only
# when its algebra is in the pack, so each is gated on its own BA.
add_repl_test(ltl_env-nlang_http_timeout_option
	"set nlang-http-timeout 3" "nlang-http-timeout: *3" NO_FAIL_REGEX NO_TRACE)
add_repl_test(ltl_env-refinement_rounds_flag
	"get ltlrefinementrounds" "ltlrefinementrounds: *5" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-refinement-rounds 5)
add_repl_test(ltl_env-window_max_paths_flag
	"get ltlwindowmaxpaths" "ltlwindowmaxpaths: *unlimited" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-window-max-paths 0)
add_repl_test(ltl_env-closed_regions_timeout_flag
	"get ltlclosedregionstimeout" "ltlclosedregionstimeout: *7 s" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-closed-regions-timeout 7)
add_repl_test(ltl_env-closed_regions_timeout_default
	"get ltlclosedregionstimeout" "ltlclosedregionstimeout: *20 s" NO_FAIL_REGEX NO_TRACE)
add_repl_test(ltl_env-closed_regions_timeout_set
	"set ltlclosedregionstimeout 0. get ltlclosedregionstimeout"
	"ltlclosedregionstimeout: *off" NO_FAIL_REGEX NO_TRACE)
add_repl_test(ltl_env-closed_regions_timeout_env_is_the_fallback
	"get ltlclosedregionstimeout" "ltlclosedregionstimeout: *5 s" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_CLOSED_REGIONS_TIMEOUT=5)
add_repl_test(ltl_env-closed_regions_timeout_flag_beats_env
	"get ltlclosedregionstimeout" "ltlclosedregionstimeout: *9 s" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_CLOSED_REGIONS_TIMEOUT=5 FLAGS --ltl-closed-regions-timeout 9)
add_repl_test(ltl_env-pwr_semantic_flag
	"get pwrsemantic" "pwrsemantic: *on" NO_FAIL_REGEX NO_TRACE
	FLAGS --pwr-semantic)
# The -K short flag belongs to --ba-component-factoring; --ltl-qe-max-vars is -k.
add_repl_test(ltl_env-qe_short_flag_is_lowercase_k
	"get ltlqemaxvars" "ltlqemaxvars: *3" NO_FAIL_REGEX NO_TRACE
	FLAGS -k 3)

# Every runtime limit of the pipeline carries all three surfaces: a CLI flag,
# a REPL option and a TAU_* environment fallback. The variable shows through
# `get` when the flag is absent, the flag wins when both are given, and a
# garbage value keeps the default and says so.
add_repl_test(ltl_env-hoa_max_states_env_is_the_fallback
	"get ltlhoamaxstates" "ltlhoamaxstates: *7" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_HOA_MAX_STATES=7)
add_repl_test(ltl_env-hoa_max_states_flag_beats_env
	"get ltlhoamaxstates" "ltlhoamaxstates: *9" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_HOA_MAX_STATES=7 FLAGS --ltl-hoa-max-states 9)
add_repl_test(ltl_env-hoa_max_states_garbage_warns
	"get ltlhoamaxstates" "TAU_LTL_HOA_MAX_STATES='abc' is not a non-negative number" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_HOA_MAX_STATES=abc)
add_repl_test(ltl_env-hoa_max_states_flag_rejects_garbage
	"get ltlhoamaxstates" "expects a non-negative number" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-hoa-max-states abc)
add_repl_test(ltl_env-guard_max_cubes_env_is_the_fallback
	"get ltlguardmaxcubes" "ltlguardmaxcubes: *6" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_GUARD_MAX_CUBES=6)
add_repl_test(ltl_env-refinement_rounds_env_is_the_fallback
	"get ltlrefinementrounds" "ltlrefinementrounds: *5" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_REFINEMENT_ROUNDS=5)
add_repl_test(ltl_env-refinement_rounds_flag_beats_env
	"get ltlrefinementrounds" "ltlrefinementrounds: *8" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_REFINEMENT_ROUNDS=5 FLAGS --ltl-refinement-rounds 8)
# 0 is a value, not an absence: it means unlimited, and the variable can say so.
add_repl_test(ltl_env-window_max_paths_env_is_the_fallback
	"get ltlwindowmaxpaths" "ltlwindowmaxpaths: *unlimited" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_WINDOW_MAX_PATHS=0)
add_repl_test(ltl_env-window_max_paths_flag_beats_env
	"get ltlwindowmaxpaths" "ltlwindowmaxpaths: *12" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_WINDOW_MAX_PATHS=0 FLAGS --ltl-window-max-paths 12)
add_repl_test(ltl_env-data_game_max_nodes_flag
	"get ltldatagamemaxnodes" "ltldatagamemaxnodes: *unlimited" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-data-game-max-nodes 0)
add_repl_test(ltl_env-data_game_max_nodes_env_is_the_fallback
	"get ltldatagamemaxnodes" "ltldatagamemaxnodes: *4096" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_DATA_GAME_MAX_NODES=4096)
add_repl_test(ltl_env-data_game_max_nodes_flag_beats_env
	"get ltldatagamemaxnodes" "ltldatagamemaxnodes: *12" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_DATA_GAME_MAX_NODES=4096 FLAGS --ltl-data-game-max-nodes 12)
add_repl_test(ltl_env-data_game_max_nodes_flag_rejects_garbage
	"get ltldatagamemaxnodes" "expects a non-negative number" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-data-game-max-nodes abc)
add_repl_test(ltl_env-data_game_max_memo_flag
	"get ltldatagamemaxmemo" "ltldatagamemaxmemo: *unlimited" NO_FAIL_REGEX NO_TRACE
	FLAGS --ltl-data-game-max-memo 0)
add_repl_test(ltl_env-data_game_max_memo_env_is_the_fallback
	"get ltldatagamemaxmemo" "ltldatagamemaxmemo: *4096" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_DATA_GAME_MAX_MEMO=4096)
add_repl_test(ltl_env-data_game_max_memo_flag_beats_env
	"get ltldatagamemaxmemo" "ltldatagamemaxmemo: *12" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_DATA_GAME_MAX_MEMO=4096 FLAGS --ltl-data-game-max-memo 12)
add_repl_test(ltl_env-data_game_max_memo_garbage_warns
	"get ltldatagamemaxmemo"
	"TAU_LTL_DATA_GAME_MAX_MEMO='abc' is not a non-negative number"
	NO_FAIL_REGEX NO_TRACE ENV TAU_LTL_DATA_GAME_MAX_MEMO=abc)
# The consistency-subset and coverage-product caps read the environment too.
add_repl_test(ltl_env-consistency_subsets_env_is_the_fallback
	"get maxsubsets" "maxsubsets: *7" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_MAX_CONSISTENCY_SUBSETS=7)
add_repl_test(ltl_env-consistency_subsets_flag_beats_env
	"get maxsubsets" "maxsubsets: *9" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_MAX_CONSISTENCY_SUBSETS=7 FLAGS --max-consistency-subsets 9)
add_repl_test(ltl_env-consistency_subsets_flag_rejects_garbage
	"get maxsubsets" "expects a non-negative number" NO_FAIL_REGEX NO_TRACE
	FLAGS --max-consistency-subsets abc)
add_repl_test(ltl_env-cover_products_env_is_the_fallback
	"get maxcoverproducts" "maxcoverproducts: *unlimited" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_MAX_COVER_PRODUCTS=0)
add_repl_test(ltl_env-cover_products_flag_beats_env
	"get maxcoverproducts" "maxcoverproducts: *9" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_MAX_COVER_PRODUCTS=0 FLAGS --max-cover-products 9)
# The same three surfaces for the caps an algebra declares about itself; each
# exists only when its algebra is in the pack, so the family gates each one.
add_repl_test(ltl_env-nlang_http_timeout_env_is_the_fallback
	"get nlang-http-timeout" "nlang-http-timeout: *9" NO_FAIL_REGEX NO_TRACE
	ENV TAU_NLANG_HTTP_TIMEOUT=9)
add_repl_test(ltl_env-nlang_http_timeout_flag_beats_env
	"get nlang-http-timeout" "nlang-http-timeout: *3" NO_FAIL_REGEX NO_TRACE
	ENV TAU_NLANG_HTTP_TIMEOUT=9 FLAGS --nlang-http-timeout 3)
add_repl_test(ltl_env-nlang_http_timeout_garbage_warns
	"get nlang-http-timeout" "TAU_NLANG_HTTP_TIMEOUT='abc' is not a non-negative number" NO_FAIL_REGEX NO_TRACE
	ENV TAU_NLANG_HTTP_TIMEOUT=abc)
# A BA-declared count option left unset keeps the algebra's own default: the
# CLI passes nothing, so an algebra's environment fallback is not shadowed.
add_repl_test(ltl_env-nlang_http_timeout_default_without_flag
	"get nlang-http-timeout" "nlang-http-timeout: *15" NO_FAIL_REGEX NO_TRACE)

# TAU_TREF_BUDGET is the memory budget's environment form: it shows through
# `get` when the flag is unset, and the flag wins when both are given.
add_repl_test(tref_budget_env_is_the_fallback
	"get trefbudget" "trefbudget: *4096" NO_FAIL_REGEX NO_TRACE
	ENV TAU_TREF_BUDGET=4096)
add_repl_test(tref_budget_flag_beats_env
	"get trefbudget" "trefbudget: *8192" NO_FAIL_REGEX NO_TRACE
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
