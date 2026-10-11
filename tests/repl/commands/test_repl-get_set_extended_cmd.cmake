#
# get and set — extended coverage
#

include(add_repl_test)

# get individual options
add_repl_test(get_cmd-severity      "get severity"      "severity:")
add_repl_test(get_cmd-charvar       "get charvar"       "charvar:")
add_repl_test(get_cmd-highlighting  "get highlighting"  "highlighting:")
add_repl_test(get_cmd-indenting     "get indenting"     "indenting:")
add_repl_test(get_cmd-benchmarks    "get benchmarks"    "benchmarks:")

# set severity
add_repl_test(set_cmd-severity_info  "set severity info"  "severity: *info")
add_repl_test(set_cmd-severity_error "set severity error" "severity: *error")

# set charvar
add_repl_test(set_cmd-charvar_on  "set charvar on"  "charvar: *true")
add_repl_test(set_cmd-charvar_off "set charvar off" "charvar: *false")

# set highlighting
add_repl_test(set_cmd-highlighting_on  "set highlighting on"  "highlighting: *true")
add_repl_test(set_cmd-highlighting_off "set highlighting off" "highlighting: *false")

# set indenting
add_repl_test(set_cmd-indenting_on  "set indenting on"  "indenting: *true")
add_repl_test(set_cmd-indenting_off "set indenting off" "indenting: *false")

# set benchmarks
add_repl_test(set_cmd-benchmarks_on  "set benchmarks on"  "benchmarks: *true")
add_repl_test(set_cmd-benchmarks_off "set benchmarks off" "benchmarks: *false")

# invalid severity value
add_repl_test(set_cmd-severity_invalid "set severity foobar" "does not take the value.*name=severity" NO_FAIL_REGEX)

# get with equals-sign syntax
add_repl_test(set_cmd-status_equals "set status = off" "status: *false")

# LTL(ABA) synthesis knobs: get shows the effective value, set round-trips
# through the api setter, an unknown algorithm letter is rejected.
add_repl_test(get_cmd-ltl_timeout        "get ltl-timeout"        "ltl-timeout: *60")
add_repl_test(set_cmd-ltl_timeout        "set ltl-timeout 7"      "ltl-timeout: *7")
add_repl_test(set_cmd-ltl_timeout_off    "set ltl-timeout 0"      "ltl-timeout: *0")
add_repl_test(get_cmd-ltl_alg            "get ltl-alg"            "ltl-alg: *auto")
add_repl_test(set_cmd-ltl_alg_b          "set ltl-alg B"          "ltl-alg: *B")
add_repl_test(set_cmd-ltl_alg_lowercase  "set ltl-alg d"          "ltl-alg: *D")
add_repl_test(set_cmd-ltl_alg_auto       "set ltl-alg B. set ltl-alg auto. get ltl-alg" "ltl-alg: *auto")
add_repl_test(set_cmd-ltl_alg_invalid "set ltl-alg C"      "does not take the value.*name=ltl-alg" NO_FAIL_REGEX)
add_repl_test(get_cmd-ltl_qe_max_vars      "get ltl-qe-max-vars"      "ltl-qe-max-vars: *2")
add_repl_test(set_cmd-ltl_qe_max_vars      "set ltl-qe-max-vars 3"    "ltl-qe-max-vars: *3")
add_repl_test(get_cmd-ltl_hoa_max_states   "get ltl-hoa-max-states"   "ltl-hoa-max-states: *4194304")
add_repl_test(set_cmd-ltl_hoa_max_states   "set ltl-hoa-max-states 0" "ltl-hoa-max-states: *0")
add_repl_test(get_cmd-ltl_guard_max_cubes  "get ltl-guard-max-cubes"  "ltl-guard-max-cubes: *512")
add_repl_test(set_cmd-ltl_guard_max_cubes  "set ltl-guard-max-cubes 9" "ltl-guard-max-cubes: *9")
add_repl_test(help_set_lists_ltl_options "help set" "ltl-timeout")

# The semantic pointwise-revision fallback is a Boolean option like factoring:
# get/set/enable/disable/toggle all reach the same library switch.
add_repl_test(get_cmd-pwr_semantic          "get pwr-semantic"            "pwr-semantic: *false")
add_repl_test(set_cmd-pwr_semantic_on       "set pwr-semantic on"         "pwr-semantic: *true")
add_repl_test(enable_cmd-pwr_semantic       "enable pwr-semantic. get pwr-semantic"  "pwr-semantic: *true")
add_repl_test(disable_cmd-pwr_semantic      "enable pwr-semantic. disable pwr-semantic. get pwr-semantic" "pwr-semantic: *false")
add_repl_test(toggle_cmd-pwr_semantic       "toggle pwr-semantic. get pwr-semantic" "pwr-semantic: *true")
add_repl_test(help_set_lists_pwr_semantic   "help set" "pwr-semantic")
# The two synthesis caps promoted from header constants.
add_repl_test(get_cmd-ltl_refinement_rounds  "get ltl-refinement-rounds"    "ltl-refinement-rounds: *64")
add_repl_test(set_cmd-ltl_refinement_rounds  "set ltl-refinement-rounds 3"  "ltl-refinement-rounds: *3")
add_repl_test(set_cmd-ltl_refinement_rounds_unlimited "set ltl-refinement-rounds 0" "ltl-refinement-rounds: *0")
add_repl_test(get_cmd-ltl_window_max_paths    "get ltl-window-max-paths"      "ltl-window-max-paths: *4096")
add_repl_test(set_cmd-ltl_window_max_paths    "set ltl-window-max-paths 12"   "ltl-window-max-paths: *12")
add_repl_test(get_cmd-ltl_data_game_max_nodes  "get ltl-data-game-max-nodes"    "ltl-data-game-max-nodes: *8388608")
add_repl_test(set_cmd-ltl_data_game_max_nodes  "set ltl-data-game-max-nodes 12" "ltl-data-game-max-nodes: *12")
add_repl_test(set_cmd-ltl_data_game_max_nodes_unlimited "set ltl-data-game-max-nodes 0" "ltl-data-game-max-nodes: *0")
add_repl_test(get_cmd-ltl_data_game_max_memo   "get ltl-data-game-max-memo"     "ltl-data-game-max-memo: *33554432")
add_repl_test(set_cmd-ltl_data_game_max_memo   "set ltl-data-game-max-memo 12"  "ltl-data-game-max-memo: *12")
add_repl_test(set_cmd-ltl_data_game_max_memo_unlimited "set ltl-data-game-max-memo 0" "ltl-data-game-max-memo: *0")
add_repl_test(help_set_lists_ltl_data_game_max_nodes "help set" "ltl-data-game-max-nodes")
add_repl_test(help_set_lists_ltl_data_game_max_memo  "help set" "ltl-data-game-max-memo")
# Promoted from header constants: the default each shipped with, a set that
# reads back, and the listing in `help set`.
add_repl_test(get_cmd-ltl_data_game_max_combinations "get ltl-data-game-max-combinations" "ltl-data-game-max-combinations: *4096")
add_repl_test(set_cmd-ltl_data_game_max_combinations "set ltl-data-game-max-combinations 12" "ltl-data-game-max-combinations: *12")
add_repl_test(set_cmd-ltl_data_game_max_combinations_unlimited "set ltl-data-game-max-combinations 0" "ltl-data-game-max-combinations: *0")
add_repl_test(get_cmd-ltl_max_observations "get ltl-max-observations" "ltl-max-observations: *8")
add_repl_test(set_cmd-ltl_max_observations "set ltl-max-observations 3" "ltl-max-observations: *3")
add_repl_test(set_cmd-ltl_max_observations_clamps "set ltl-max-observations 99" "ltl-max-observations: *30")
add_repl_test(get_cmd-ltl_mealy_max_states "get ltl-mealy-max-states" "ltl-mealy-max-states: *4096")
add_repl_test(set_cmd-ltl_mealy_max_states "set ltl-mealy-max-states 0" "ltl-mealy-max-states: *0")
add_repl_test(get_cmd-ltl_mealy_max_edges "get ltl-mealy-max-edges" "ltl-mealy-max-edges: *65536")
add_repl_test(set_cmd-ltl_mealy_max_edges "set ltl-mealy-max-edges 12" "ltl-mealy-max-edges: *12")
add_repl_test(get_cmd-compile_max_table_edges "get compile-max-table-edges" "compile-max-table-edges: *400")
add_repl_test(set_cmd-compile_max_table_edges "set compile-max-table-edges 12" "compile-max-table-edges: *12")
add_repl_test(get_cmd-compile_build_timeout "get compile-build-timeout" "compile-build-timeout: *3600")
add_repl_test(set_cmd-compile_build_timeout "set compile-build-timeout 12" "compile-build-timeout: *12")
add_repl_test(set_cmd-compile_build_timeout_off "set compile-build-timeout 0" "compile-build-timeout: *0")
add_repl_test(get_cmd-bf_dependence_max_nodes "get bf-dependence-max-nodes" "bf-dependence-max-nodes: *65536")
add_repl_test(set_cmd-bf_dependence_max_nodes "set bf-dependence-max-nodes 0" "bf-dependence-max-nodes: *0")
add_repl_test(get_cmd-bv_blasting_max_nodes "get bv-blasting-max-nodes" "bv-blasting-max-nodes: *500000")
add_repl_test(set_cmd-bv_blasting_max_nodes "set bv-blasting-max-nodes 12" "bv-blasting-max-nodes: *12")
add_repl_test(get_cmd-bv_bitblast_max_width "get bv-bitblast-max-width" "bv-bitblast-max-width: *16")
add_repl_test(set_cmd-bv_bitblast_max_width "set bv-bitblast-max-width 8" "bv-bitblast-max-width: *8")
foreach(opt ltl-data-game-max-combinations ltl-max-observations
		ltl-mealy-max-states ltl-mealy-max-edges compile-max-table-edges
		compile-build-timeout bf-dependence-max-nodes)
	add_repl_test(help_set_lists_${opt} "help set" "${opt}")
endforeach()
add_repl_test(get_cmd-qlt_const_output_max "get qlt-const-output-max"   "qlt-const-output-max: *100")
add_repl_test(set_cmd-qlt_const_output_max "set qlt-const-output-max 7" "qlt-const-output-max: *7")
add_repl_test(get_cmd-qlt_cells_budget "get qlt-cells-budget"   "qlt-cells-budget: *65536")
add_repl_test(set_cmd-qlt_cells_budget "set qlt-cells-budget 9" "qlt-cells-budget: *9")
add_repl_test(get_cmd-qlt_cells_max_params "get qlt-cells-max-params"   "qlt-cells-max-params: *2")
add_repl_test(set_cmd-qlt_cells_max_params "set qlt-cells-max-params 3" "qlt-cells-max-params: *3")

# GitHub #121: the solver's lgrs-route variable cap is a count option.
add_repl_test(get_cmd-lgrs_max_vars           "get lgrs-max-vars"        "lgrs-max-vars: *8")
add_repl_test(set_cmd-lgrs_max_vars           "set lgrs-max-vars 3"      "lgrs-max-vars: *3")
add_repl_test(set_cmd-lgrs_max_vars_unlimited "set lgrs-max-vars 0"      "lgrs-max-vars: *0")
add_repl_test(help_set_lists_lgrs_max_vars    "help set"               "lgrs-max-vars")
# GitHub #126: the step's definitional propagation is a Boolean option.
add_repl_test(get_cmd-step_definitional_propagation              "get step-definitional-propagation"           "step-definitional-propagation: *true")
add_repl_test(set_cmd-step_definitional_propagation_off          "set step-definitional-propagation off"       "step-definitional-propagation: *false")
add_repl_test(enable_cmd-step_definitional_propagation           "enable step-definitional-propagation. get step-definitional-propagation" "step-definitional-propagation: *true")
add_repl_test(disable_cmd-step_definitional_propagation          "enable step-definitional-propagation. disable step-definitional-propagation. get step-definitional-propagation" "step-definitional-propagation: *false")
add_repl_test(toggle_cmd-step_definitional_propagation           "toggle step-definitional-propagation. get step-definitional-propagation" "step-definitional-propagation: *false")
add_repl_test(help_set_lists_step_definitional_propagation       "help set"               "step-definitional-propagation")
