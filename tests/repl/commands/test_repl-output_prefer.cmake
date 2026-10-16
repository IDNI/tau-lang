# output-prefer (TAU_OUTPUT_PREFER): soft preferences on the outputs of
# each step. Without them G (o1[t] < {10}) runs 8, 8, 8 and
# G (o1[t] = 1 || o2[t] = 1) runs (0, 1), (0, 1), (1, 0).
include(add_repl_test)

add_repl_test(output_prefer-value
	"o1:bv[8] := out console. run 3 steps G (o1[t] < {10}:bv[8])."
	"o1\\[0\\] := 3[^0-9].*o1\\[1\\] := 3[^0-9].*o1\\[2\\] := 3[^0-9]"
	NO_TRACE ENV TAU_OUTPUT_PREFER=o1=3)
# a preference no output of the step can keep is dropped
add_repl_test(output_prefer-unkeepable_is_dropped
	"o1:bv[8] := out console. run 2 steps G (o1[t] > {2}:bv[8] && o1[t] < {10}:bv[8])."
	"o1\\[0\\] := [3-9][^0-9].*o1\\[1\\] := [3-9][^0-9]"
	NO_TRACE ENV TAU_OUTPUT_PREFER=o1=0)
# when both cannot hold, the earlier preference is kept
add_repl_test(output_prefer-order_o1_first
	"o1:bv[1] := out console. o2:bv[1] := out console. run 3 steps G (o1[t] = 1 || o2[t] = 1)."
	"o1\\[0\\] := 0.*o2\\[0\\] := 1.*o1\\[1\\] := 0.*o2\\[1\\] := 1.*o1\\[2\\] := 0.*o2\\[2\\] := 1"
	NO_TRACE ENV "TAU_OUTPUT_PREFER=o1=0,o2=0")
add_repl_test(output_prefer-order_o2_first
	"o1:bv[1] := out console. o2:bv[1] := out console. run 3 steps G (o1[t] = 1 || o2[t] = 1)."
	"o1\\[0\\] := 1.*o2\\[0\\] := 0.*o1\\[1\\] := 1.*o2\\[1\\] := 0.*o1\\[2\\] := 1.*o2\\[2\\] := 0"
	NO_TRACE ENV "TAU_OUTPUT_PREFER=o2=0,o1=0")
# prev keeps the value of the step before
add_repl_test(output_prefer-prev
	"o1:bv[1] := out console. o2:bv[1] := out console. run 3 steps G (o1[t] = 1 || o2[t] = 1)."
	"o1\\[0\\] := 0.*o2\\[0\\] := 1.*o1\\[1\\] := 0.*o2\\[1\\] := 1.*o1\\[2\\] := 0.*o2\\[2\\] := 1"
	NO_TRACE ENV "TAU_OUTPUT_PREFER=o1=prev,o2=prev")
# the blanks around names and values are removed
add_repl_test(output_prefer-get_trims
	"get output-prefer"
	"output-prefer: *o1=3,o2=prev" NO_FAIL_REGEX NO_TRACE
	ENV "TAU_OUTPUT_PREFER= o1 = 3 , o2=prev ")
add_env_error_test(output_prefer-without_value_is_an_error
	TAU_OUTPUT_PREFER=o1)
add_env_error_test(output_prefer-bad_name_is_an_error
	TAU_OUTPUT_PREFER=1o=0)
# a value the stream's type does not parse is an error of the run
add_repl_test(output_prefer-unparsable_value
	"o1:bv[8] := out console. run 1 steps G (o1[t] < {10}:bv[8])."
	"failed to parse the output-prefer value" NO_FAIL_REGEX NO_TRACE
	ENV TAU_OUTPUT_PREFER=o1=abc)
# the data game's move pins o1 to 0 and leaves o2 below it (-1 without a
# preference): o2 takes the preferred point, o1 cannot
add_repl_test(output_prefer-data_game_move
	"run 3 steps ((o2[t]:qlt = o2[t]:qlt) U (o1[t]:qlt = o2[t]:qlt))."
	"o1\\[1\\] := 0[^0-9].*o2\\[1\\] := -7/2[^0-9].*o1\\[2\\] := 0[^0-9].*o2\\[2\\] := -7/2[^0-9]"
	NO_TRACE ENV "TAU_OUTPUT_PREFER=o1=5,o2=-7/2" REQUIRES qlt ltlsynt)
