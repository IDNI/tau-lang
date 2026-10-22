#
# The data game: the synthesis game of the LTL skeleton played on the data,
# and the runs of its strategy. The cases pick the code each stream takes
# (the values of a type with few elements, the classes of a type read only
# against constants, the bits of a modular type, the order of a dense one,
# or formula regions when no code fits), the caps that move a game off its
# codes or its Mealy view, and the revisions that start a new game from the
# values already played, and the games of an ltlsynt stub the data game
# cannot read. Every case needs ltlsynt on PATH.
#

include(add_repl_test)
include(tau_repl_pack)

function(add_data_game_run_test name input pass fail)
	# The input carries printf escapes, so the helper pipes the exact bytes.
	add_multiline_repl_test(ltl_data_game-${name} "${pass}"
		STDIN "${input}" FAIL_REGEX "${fail}" REQUIRES ltlsynt)
endfunction()

set(DG_WIDENED_SPEC "(sometimes o1[t]:bv[2] >= i1[t]:bv[2]) && (always o2[t]:bv[2] > i2[t]:bv[2] || i2[t]:bv[2] = {3}:bv[2])")
set(DG_CLASS_SPEC "(sometimes o1[t]:sbf < {X}:sbf && o1[t]:sbf != 0) && (always (i1[t-1]:sbf = {X}:sbf -> o2[t]:sbf = {Y}:sbf))")
set(DG_FORMULA_SPEC "(always o2[t]:sbf < i1[t-1]:sbf || i1[t-1]:sbf = 0) && (sometimes o1[t]:sbf < i1[t]:sbf && o1[t]:sbf != 0 || i1[t]:sbf = 0)")

# With widening a bv[2] stream is no modular type: a comparison other than
# an equality gives its streams the four values of the type as codes, and o2
# stays above i2 unless i2 is 3.
add_data_game_run_test(widened_bv_values_run
	"set bv-widening on\\nrun 5 steps ${DG_WIDENED_SPEC}.\\n1\\n2\\n0\\n1\\n0\\n2\\nq\\n"
	"o2\\[0\\] := 3\n.*o2\\[1\\] := 2\n.*o2\\[2\\] := 2\n.*o2\\[3\\] := 2\n.*o2\\[4\\] := 3"
	"unrealizable|no strategy|not executable")

# the same game played without a Mealy view: the moves decode the value codes
add_data_game_run_test(widened_bv_values_without_mealy_view
	"set bv-widening on\\nset ltl-mealy-max-states 0\\nrun 3 steps ${DG_WIDENED_SPEC}.\\n1\\n2\\n0\\n1\\n0\\n2\\nq\\n"
	"o2\\[0\\] := 3\n.*o2\\[1\\] := 2\n.*o2\\[2\\] := 3"
	"unrealizable|no strategy|not executable")

# The elements of a type are counted once per type and solver budget: the
# second check reads them back, and a reset drops them, so the third counts
# them again; every check decides the same way.
add_multiline_repl_test(ltl_data_game-widened_values_cache_and_reset
	"REALIZABLE \\(data game\\).*REALIZABLE \\(data game\\).*Session reset.*REALIZABLE \\(data game\\)"
	"set bv-widening on"
	"realizable ${DG_WIDENED_SPEC}"
	"realizable ${DG_WIDENED_SPEC}"
	"reset"
	"realizable ${DG_WIDENED_SPEC}"
	FAIL_REGEX "UNREALIZABLE|UNKNOWN" REQUIRES ltlsynt)

# a cap on the value combinations a comparison tabulates leaves the codes;
# the formula regions still decide the game
add_repl_test(ltl_data_game-combination_cap_leaves_the_codes
	"set bv-widening on. set ltl-data-game-max-combinations 4. realizable ${DG_WIDENED_SPEC}."
	"REALIZABLE \\(data game\\)" NO_TRACE
	FAIL_REGEX "UNREALIZABLE|UNKNOWN" REQUIRES ltlsynt)

# With only equalities and too few values for the slots of a window, a
# widened bv[2] takes its values as codes: o1 alternates and o2 meets its
# goal at step 0 with a value other than 0 and i1.
add_data_game_run_test(widened_equalities_take_values
	"set bv-widening on\\nrun 3 steps (always o1[t]:bv[2] != o1[t-1]:bv[2]) && (sometimes o2[t]:bv[2] != i1[t]:bv[2] && o2[t]:bv[2] != {0}:bv[2]).\\n2\\nq\\n"
	"o1\\[0\\] := 0\n.*o2\\[0\\] := 1\n.*o1\\[1\\] := 2\n.*o1\\[2\\] := 0"
	"unrealizable|no strategy|not executable")

# a widened bv[5] has more elements than a value code holds: its streams,
# each read alone against constants, take class codes
add_data_game_run_test(widened_bv5_class_codes
	"set bv-widening on\\nrun 4 steps (sometimes o1[t]:bv[5] > {20}:bv[5]) && (always (i1[t]:bv[5] < {3}:bv[5] -> o2[t]:bv[5] > {7}:bv[5])).\\n1\\n9\\n2\\n30\\nq\\n"
	"o1\\[0\\] := 21\n.*o2\\[0\\] := 8\n.*o2\\[2\\] := 8"
	"unrealizable|no strategy|not executable")

# The bits of a modular type: shifts, min and max are circuits too.
add_data_game_run_test(bv_shift_min_max
	"run 2 steps (sometimes (o1[t]:bv[4] << {1}:bv[4]) = {8}:bv[4] && (o2[t]:bv[4] >> {1}:bv[4]) = {1}:bv[4] && min(o3[t]:bv[4], i1[t]:bv[4]) = {0}:bv[4] && max(o4[t]:bv[4], i1[t]:bv[4]) = {15}:bv[4]) && (always o5[t]:bv[4] = o5[t-1]:bv[4]).\\n1\\nq\\n"
	"o1\\[0\\] := (4|12)\n.*o2\\[0\\] := [23]\n.*o3\\[0\\] := 0\n.*o4\\[0\\] := 15"
	"unrealizable|no strategy|not executable")

# a small memo of the BDD operations is emptied as it fills: the moves stay
add_data_game_run_test(memo_cap_keeps_the_moves
	"set ltl-data-game-max-memo 16\\nrun 2 steps (sometimes (o1[t]:bv[4] << {1}:bv[4]) = {8}:bv[4] && (o2[t]:bv[4] >> {1}:bv[4]) = {1}:bv[4] && min(o3[t]:bv[4], i1[t]:bv[4]) = {0}:bv[4] && max(o4[t]:bv[4], i1[t]:bv[4]) = {15}:bv[4]) && (always o5[t]:bv[4] = o5[t-1]:bv[4]).\\n1\\nq\\n"
	"o1\\[0\\] := (4|12)\n.*o2\\[0\\] := [23]\n.*o3\\[0\\] := 0\n.*o4\\[0\\] := 15"
	"unrealizable|no strategy|not executable")
# circuits that outgrow the node cap leave the game on codes undecided, and
# another decider gives the verdict
add_repl_test(ltl_data_game-node_cap_leaves_the_code_game
	"set ltl-data-game-max-nodes 16. realizable (sometimes (o1[t]:bv[4] << {1}:bv[4]) = {8}:bv[4] && max(o4[t]:bv[4], i1[t]:bv[4]) = {15}:bv[4]) && (always o5[t]:bv[4] = o5[t-1]:bv[4])."
	"\\[ltl_aba\\] REALIZABLE\n" NO_TRACE
	FAIL_REGEX "REALIZABLE \\(data game\\)|UNREALIZABLE|UNKNOWN" REQUIRES ltlsynt)

# An sbf stream read only against constants takes the classes of the
# comparisons as codes: o1 meets its goal below X at step 0, and o2 is Y
# after each step whose i1 was X.
add_data_game_run_test(class_codes_run
	"run 4 steps ${DG_CLASS_SPEC}.\\n{X}\\n{Y}\\n{X}\\n{X}\\nq\\n"
	"o1\\[0\\] := X Y'\n.*o2\\[1\\] := Y\n.*o2\\[3\\] := Y"
	"unrealizable|no strategy|not executable")
# the same moves without a Mealy view, decoded from the classes
add_data_game_run_test(class_codes_without_mealy_view
	"set ltl-mealy-max-states 0\\nrun 4 steps ${DG_CLASS_SPEC}.\\n{X}\\n{Y}\\n{X}\\n{X}\\nq\\n"
	"o1\\[0\\] := X Y'\n.*o2\\[1\\] := Y\n.*o2\\[3\\] := Y"
	"unrealizable|no strategy|not executable")
# a Mealy view past the edge cap is dropped, and the moves are played directly
add_data_game_run_test(mealy_edge_cap_plays_moves_directly
	"set ltl-mealy-max-edges 1\\nrun 4 steps ${DG_CLASS_SPEC}.\\n{X}\\n{Y}\\n{X}\\n{X}\\nq\\n"
	"o1\\[0\\] := X Y'\n.*o2\\[1\\] := Y\n.*o2\\[3\\] := Y"
	"unrealizable|no strategy|not executable")

# A dense order with two inputs read at one step: o1 is placed between them.
add_data_game_run_test(two_order_inputs
	"run 2 steps (sometimes o1[t]:qlt > i1[t]:qlt && o1[t]:qlt < i2[t]:qlt || i2[t]:qlt <= i1[t]:qlt).\\n{1}\\n{3}\\nq\\n"
	"i2\\[0\\][^\n]*\n.*o1\\[0\\] := 2\n"
	"unrealizable|no strategy|not executable|:= bot|:= top")

# Comparisons between two sbf streams take no code: the game is played on
# formula regions.
add_repl_test(ltl_data_game-formula_regions_decide
	"realizable ${DG_FORMULA_SPEC}."
	"REALIZABLE \\(data game\\)" NO_TRACE
	FAIL_REGEX "UNREALIZABLE|UNKNOWN" REQUIRES ltlsynt)
# a formula fixpoint capped at one round leaves the game undecided, and
# another decider gives the verdict
add_repl_test(ltl_data_game-fixpoint_round_cap_leaves_the_formula_game
	"set ltl-data-fixpoint-rounds 1. realizable ${DG_FORMULA_SPEC}."
	"\\[ltl_aba\\] REALIZABLE\n" NO_TRACE
	FAIL_REGEX "REALIZABLE \\(data game\\)|UNREALIZABLE|UNKNOWN" REQUIRES ltlsynt)

# A revision whose game is won from the values already played starts from
# them: from the revision step on o1 keeps alternating, never 0. The widened
# bv[2] streams carry value codes.
add_data_game_run_test(revision_starts_from_value_codes
	"set bv-widening on\\nrun 8 steps (always u[t] = i1[t] && o1[t]:bv[2] != o1[t-1]:bv[2]) && (sometimes o2[t]:bv[2] > {1}:bv[2]).\\nF.\\nF.\\nalways o1[t]:bv[2] > {0}:bv[2].\\nF.\\nF.\\nF.\\nq\\n"
	"o1\\[3\\] := 2\n.*Updated specification[^\n]*\n.*o1\\[4\\] := 1\n.*o1\\[5\\] := 2\n.*o1\\[6\\] := 1"
	"does not keep|is not won from|o1\\[[4-7]\\] := 0|no strategy|unsat")
# the same over formula regions: the game of the revision is won from the
# values played, and o2 stays below the last i1
add_data_game_run_test(revision_starts_from_formula_regions
	"run 7 steps (always u[t] = i2[t] && (o2[t]:sbf < i1[t-1]:sbf || i1[t-1]:sbf = 0)) && (sometimes o1[t]:sbf < i1[t]:sbf && o1[t]:sbf != 0 || i1[t]:sbf = 0).\\n{X}\\n{Y}\\nF.\\n{X}\\nalways o2[t]:sbf != {Z}:sbf.\\n{Y}\\nF.\\n{X}\\nF.\\n{Y}\\nF.\\n{X}\\nF.\\nq\\n"
	"Updated specification[^\n]*\n.*o2\\[3\\] := X Y Z \\| X Y' Z'\n.*o2\\[4\\] := 0\n.*o2\\[5\\] := 0\n.*o2\\[6\\] := 0"
	"does not keep|is not won from|no strategy|unsat")
# the same over class codes: o2 stays nonzero, and Y after an i1 of X
add_data_game_run_test(revision_starts_from_class_codes
	"run 7 steps (always u[t] = i2[t] && (i1[t-1]:sbf = {X}:sbf -> o2[t]:sbf = {Y}:sbf)) && (sometimes o1[t]:sbf < {X}:sbf && o1[t]:sbf != 0).\\n{X}\\n{Y}\\nF.\\n{X}\\nalways (o2[t]:sbf != 0).\\nF.\\nF.\\nF.\\nq\\n"
	"Updated specification[^\n]*\n.*o2\\[3\\] := Y\n.*o2\\[4\\] := Y"
	"does not keep|is not won from|o2\\[[3-6]\\] := 0|no strategy|unsat")

# A game the data game cannot read leaves it undecided, and the abstraction
# decides: an acceptance that is no parity condition, a state with two
# colours, a start state the system owns, an environment label over an
# output, a label over a prop outside the AP table, and one that does not
# parse (tests/repl/stubs/odd_ltlsynt).
if(NOT WIN32)
	foreach(_game streett multi env_start foreign ap_index bad_label)
		add_raw_repl_test(ltl_data_game-unreadable_game_${_game}
			"TAU_STUB_LTLSYNT=${_game} PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/odd_ltlsynt:$PATH ${TAU_RUN} -e \"realizable (G(i1[t]:bv[1] = 1 -> o1[t]:bv[1] = 1)) && F(o2[t]:bv[1] = 1)\""
			"\\[ltl_aba\\] REALIZABLE\n.*: T"
			FAIL_REGEX "REALIZABLE \\(data game\\)|Error|Aborted|core dumped" REQUIRES ltlsynt)
	endforeach()
endif()
