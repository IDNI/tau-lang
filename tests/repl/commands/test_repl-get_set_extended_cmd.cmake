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
add_repl_test(set_cmd-severity_info  "set severity info"  "info")
add_repl_test(set_cmd-severity_error "set severity error" "error")

# set charvar
add_repl_test(set_cmd-charvar_on  "set charvar on"  "on")
add_repl_test(set_cmd-charvar_off "set charvar off" "off")

# set highlighting
add_repl_test(set_cmd-highlighting_on  "set highlighting on"  "on")
add_repl_test(set_cmd-highlighting_off "set highlighting off" "off")

# set indenting
add_repl_test(set_cmd-indenting_on  "set indenting on"  "on")
add_repl_test(set_cmd-indenting_off "set indenting off" "off")

# set benchmarks
add_repl_test(set_cmd-benchmarks_on  "set benchmarks on"  "on")
add_repl_test(set_cmd-benchmarks_off "set benchmarks off" "off")

# invalid severity value
add_repl_test(set_cmd-severity_invalid "set severity foobar" "Invalid severity value" NO_FAIL_REGEX)

# get with equals-sign syntax
add_repl_test(set_cmd-status_equals "set status = off" "off")

# LTL(ABA) synthesis knobs: get shows the effective value, set round-trips
# through the api setter, an unknown algorithm letter is rejected.
add_repl_test(get_cmd-ltltimeout        "get ltltimeout"        "ltltimeout: *60s")
add_repl_test(set_cmd-ltltimeout        "set ltltimeout 7"      "ltltimeout: *7s")
add_repl_test(set_cmd-ltltimeout_off    "set ltltimeout 0"      "ltltimeout: *off")
add_repl_test(get_cmd-ltlalg            "get ltlalg"            "ltlalg: *auto")
add_repl_test(set_cmd-ltlalg_b          "set ltlalg B"          "ltlalg: *B")
add_repl_test(set_cmd-ltlalg_lowercase  "set ltlalg d"          "ltlalg: *D")
add_repl_test(set_cmd-ltlalg_auto       "set ltlalg B. set ltlalg auto. get ltlalg" "ltlalg: *auto")
add_repl_test(set_cmd-ltlalg_invalid "set ltlalg C"      "expected A, B, D or auto" NO_FAIL_REGEX)
add_repl_test(get_cmd-ltlqemaxvars      "get ltlqemaxvars"      "ltlqemaxvars: *2")
add_repl_test(set_cmd-ltlqemaxvars      "set ltlqemaxvars 3"    "ltlqemaxvars: *3")
add_repl_test(get_cmd-ltlhoamaxstates   "get ltlhoamaxstates"   "ltlhoamaxstates: *4194304")
add_repl_test(set_cmd-ltlhoamaxstates   "set ltlhoamaxstates 0" "ltlhoamaxstates: *unlimited")
add_repl_test(get_cmd-ltlguardmaxcubes  "get ltlguardmaxcubes"  "ltlguardmaxcubes: *512")
add_repl_test(set_cmd-ltlguardmaxcubes  "set ltlguardmaxcubes 9" "ltlguardmaxcubes: *9")
add_repl_test(help_set_lists_ltl_options "help set" "ltltimeout")

# The semantic pointwise-revision fallback is a Boolean option like factoring:
# get/set/enable/disable/toggle all reach the same library switch.
add_repl_test(get_cmd-pwrsemantic          "get pwrsemantic"            "pwrsemantic: *off")
add_repl_test(set_cmd-pwrsemantic_on       "set pwrsemantic on"         "pwrsemantic: *on")
add_repl_test(enable_cmd-pwrsemantic       "enable pwrsemantic. get Z"  "pwrsemantic: *on")
add_repl_test(disable_cmd-pwrsemantic      "enable pwrsemantic. disable pwrsemantic. get pwrsemantic" "pwrsemantic: *off")
add_repl_test(toggle_cmd-pwrsemantic       "toggle pwrsemantic. get pwrsemantic" "pwrsemantic: *on")
add_repl_test(help_set_lists_pwrsemantic   "help set" "pwrsemantic")
# The two synthesis caps promoted from header constants.
add_repl_test(get_cmd-ltlrefinementrounds  "get ltlrefinementrounds"    "ltlrefinementrounds: *64")
add_repl_test(set_cmd-ltlrefinementrounds  "set ltlrefinementrounds 3"  "ltlrefinementrounds: *3")
add_repl_test(set_cmd-ltlrefinementrounds_unlimited "set ltlrefinementrounds 0" "ltlrefinementrounds: *unlimited")
add_repl_test(get_cmd-ltlwindowmaxpaths    "get ltlwindowmaxpaths"      "ltlwindowmaxpaths: *4096")
add_repl_test(set_cmd-ltlwindowmaxpaths    "set ltlwindowmaxpaths 12"   "ltlwindowmaxpaths: *12")
add_repl_test(get_cmd-ltldatagamemaxnodes  "get ltldatagamemaxnodes"    "ltldatagamemaxnodes: *8388608")
add_repl_test(set_cmd-ltldatagamemaxnodes  "set ltldatagamemaxnodes 12" "ltldatagamemaxnodes: *12")
add_repl_test(set_cmd-ltldatagamemaxnodes_unlimited "set ltldatagamemaxnodes 0" "ltldatagamemaxnodes: *unlimited")
add_repl_test(get_cmd-ltldatagamemaxmemo   "get ltldatagamemaxmemo"     "ltldatagamemaxmemo: *33554432")
add_repl_test(set_cmd-ltldatagamemaxmemo   "set ltldatagamemaxmemo 12"  "ltldatagamemaxmemo: *12")
add_repl_test(set_cmd-ltldatagamemaxmemo_unlimited "set ltldatagamemaxmemo 0" "ltldatagamemaxmemo: *unlimited")
add_repl_test(help_set_lists_ltldatagamemaxnodes "help set" "ltldatagamemaxnodes")
add_repl_test(help_set_lists_ltldatagamemaxmemo  "help set" "ltldatagamemaxmemo")
# Promoted from header constants: the default each shipped with, a set that
# reads back, and the listing in `help set`.
add_repl_test(get_cmd-ltldatagamemaxcombinations "get ltldatagamemaxcombinations" "ltldatagamemaxcombinations: *4096")
add_repl_test(set_cmd-ltldatagamemaxcombinations "set ltldatagamemaxcombinations 12" "ltldatagamemaxcombinations: *12")
add_repl_test(set_cmd-ltldatagamemaxcombinations_unlimited "set ltldatagamemaxcombinations 0" "ltldatagamemaxcombinations: *unlimited")
add_repl_test(get_cmd-ltlmaxobservations "get ltlmaxobservations" "ltlmaxobservations: *8")
add_repl_test(set_cmd-ltlmaxobservations "set ltlmaxobservations 3" "ltlmaxobservations: *3")
add_repl_test(set_cmd-ltlmaxobservations_clamps "set ltlmaxobservations 99" "ltlmaxobservations: *30")
add_repl_test(get_cmd-ltlmealymaxstates "get ltlmealymaxstates" "ltlmealymaxstates: *4096")
add_repl_test(set_cmd-ltlmealymaxstates "set ltlmealymaxstates 0" "ltlmealymaxstates: *0")
add_repl_test(get_cmd-ltlmealymaxedges "get ltlmealymaxedges" "ltlmealymaxedges: *65536")
add_repl_test(set_cmd-ltlmealymaxedges "set ltlmealymaxedges 12" "ltlmealymaxedges: *12")
add_repl_test(get_cmd-compilemaxtableedges "get compilemaxtableedges" "compilemaxtableedges: *400")
add_repl_test(set_cmd-compilemaxtableedges "set compilemaxtableedges 12" "compilemaxtableedges: *12")
add_repl_test(get_cmd-bfdependencemaxnodes "get bfdependencemaxnodes" "bfdependencemaxnodes: *65536")
add_repl_test(set_cmd-bfdependencemaxnodes "set bfdependencemaxnodes 0" "bfdependencemaxnodes: *unlimited")
add_repl_test(get_cmd-bv_blasting_max_nodes "get bv-blasting-max-nodes" "bv-blasting-max-nodes: *500000")
add_repl_test(set_cmd-bv_blasting_max_nodes "set bv-blasting-max-nodes 12" "bv-blasting-max-nodes: *12")
add_repl_test(get_cmd-bv_bitblast_max_width "get bv-bitblast-max-width" "bv-bitblast-max-width: *16")
add_repl_test(set_cmd-bv_bitblast_max_width "set bv-bitblast-max-width 8" "bv-bitblast-max-width: *8")
foreach(opt ltldatagamemaxcombinations ltlmaxobservations ltlmealymaxstates
		ltlmealymaxedges compilemaxtableedges bfdependencemaxnodes)
	add_repl_test(help_set_lists_${opt} "help set" "${opt}")
endforeach()
add_repl_test(get_cmd-qlt_const_output_max "get qlt-const-output-max"   "qlt-const-output-max: *100")
add_repl_test(set_cmd-qlt_const_output_max "set qlt-const-output-max 7" "qlt-const-output-max: *7")
add_repl_test(get_cmd-qlt_cells_budget "get qlt-cells-budget"   "qlt-cells-budget: *65536")
add_repl_test(set_cmd-qlt_cells_budget "set qlt-cells-budget 9" "qlt-cells-budget: *9")
add_repl_test(get_cmd-qlt_cells_max_params "get qlt-cells-max-params"   "qlt-cells-max-params: *2")
add_repl_test(set_cmd-qlt_cells_max_params "set qlt-cells-max-params 3" "qlt-cells-max-params: *3")

# GitHub #121: the solver's lgrs-route variable cap is a count option.
add_repl_test(get_cmd-lgrsmaxvars           "get lgrsmaxvars"        "lgrsmaxvars: *8")
add_repl_test(set_cmd-lgrsmaxvars           "set lgrsmaxvars 3"      "lgrsmaxvars: *3")
add_repl_test(set_cmd-lgrsmaxvars_unlimited "set lgrsmaxvars 0"      "lgrsmaxvars: *unlimited")
add_repl_test(help_set_lists_lgrsmaxvars    "help set"               "lgrsmaxvars")
# GitHub #126: the step's definitional propagation is a Boolean option.
add_repl_test(get_cmd-stepprop              "get stepprop"           "stepprop: *on")
add_repl_test(set_cmd-stepprop_off          "set stepprop off"       "stepprop: *off")
add_repl_test(enable_cmd-stepprop           "enable stepprop. get stepdefinitionalpropagation" "stepprop: *on")
add_repl_test(disable_cmd-stepprop          "enable stepprop. disable stepprop. get stepprop" "stepprop: *off")
add_repl_test(toggle_cmd-stepprop           "toggle stepprop. get stepprop" "stepprop: *off")
add_repl_test(help_set_lists_stepprop       "help set"               "stepprop")
