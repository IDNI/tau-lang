#
# Paths of the execution engine that the other run suites leave out: the
# routes make_interpreter takes for a fixed step or a disjunction the safety
# pipeline cannot play, a CTL* specification over finite traces, the
# atomless decode of a warm-up step before outputs that must differ, streams a
# run cannot open or write, a compositional part without a data-game
# strategy, an update that joins two parts of the running specification,
# revisions of a data-game or compositional run (a new stream, a nested
# temporal clause, a second revision reading values of the first or an
# input the run never read, a fixed input step, a stream that cannot be
# opened), the largest value of the update stream, the this stream after a
# conditional update, a run with a step budget that receives a value it
# cannot parse, the run loop of a specification file with its idle prompt
# and a tuple console input,
# updates whose values are temporal formulas, and expected failures for an
# uninterpreted constant, a realizable disjunctive update value, and
# revisions that start with a warm-up step.
#

include(add_repl_test)
include(tau_repl_pack)

# --- routes of make_interpreter ----------------------------------------------
# A fixed step the input decides: the environment can make i1[2] = 0, so the
# step-counter game finds no strategy and the run refuses the specification.
add_multiline_repl_test(interpreter_paths-fixed_step_read_by_input_unsat
	"Tau specification is unsat"
	STDIN "run 3 steps (always o1[t]:bv[1] = i1[t]:bv[1]) && (o1[2]:bv[1] = 1).\\n1\\n1\\n1\\nq\\nq\\n"
	FAIL_REGEX ":= " REQUIRES ltlsynt)

# Over finite traces a CTL* specification has no reduction to LTL, so the
# run refuses it.
add_multiline_repl_test(interpreter_paths-ctl_star_reduction_refused_finite
	"no path quantifiers A and E.*not executable \\(CTL\\* reduction failed\\)"
	STDIN "fragment ctl_star\\nset ltl-semantics finite\\nrun 2 steps A (G (o1[t] = 1)).\\nq\\nq\\n"
	NO_FAIL_REGEX)

# A disjunction of models over an input is played as a game: one disjunct is
# kept for the whole run, so one output mirrors every input.
add_multiline_repl_test(interpreter_paths-disjunction_played_as_game
	"o1\\[0\\] := 1.*o1\\[1\\] := 0.*o1\\[2\\] := 1|o2\\[0\\] := 1.*o2\\[1\\] := 0.*o2\\[2\\] := 1"
	STDIN "run 3 steps (always o1[t]:bv[1] = i1[t]:bv[1]) || (always o2[t]:bv[1] = i1[t]:bv[1]).\\n1\\n0\\n1\\nq\\nq\\n"
	FAIL_REGEX "unsat|Error" REQUIRES ltlsynt)

# --- atomless decode of a warm-up step ----------------------------------------
# The clause starts at its lookback, step 1: step 0, the warm-up step the
# atomless decode completes, may take any values, and from step 1 on every
# disequality holds.
set(TAU_DISTINCT_CHECKER "${CMAKE_SOURCE_DIR}/tests/repl/check_distinct_outputs.sh")
add_raw_repl_test(interpreter_paths-atomless_decode_mutual_tau
	"printf 'run 4 steps always o1[t]:tau != o2[t]:tau && o1[t]:tau != o1[t-1]:tau.\\nq\\nq\\n' | ${TAU_RUN} -X | bash ${TAU_DISTINCT_CHECKER} 1 o1:o2 o1:o1-1"
	"o1\\[0\\] := .*o2\\[0\\] := .*distinct outputs at 3 steps"
	FAIL_REGEX "Error|unsat|EQUAL OUTPUTS")
add_raw_repl_test(interpreter_paths-atomless_decode_mutual_sbf
	"printf 'run 4 steps always o1[t]:sbf != o2[t]:sbf && o1[t]:sbf != o1[t-1]:sbf.\\nq\\nq\\n' | ${TAU_RUN} -X | bash ${TAU_DISTINCT_CHECKER} 1 o1:o2 o1:o1-1"
	"o1\\[0\\] := .*o2\\[0\\] := .*distinct outputs at 3 steps"
	FAIL_REGEX "Error|unsat|EQUAL OUTPUTS")
# three outputs pairwise distinct
add_raw_repl_test(interpreter_paths-atomless_decode_three_distinct_sbf
	"printf 'run 4 steps always o1[t]:sbf != o2[t]:sbf && o2[t]:sbf != o3[t]:sbf && o1[t]:sbf != o3[t]:sbf && o1[t]:sbf != o1[t-1]:sbf.\\nq\\nq\\n' | ${TAU_RUN} -X | bash ${TAU_DISTINCT_CHECKER} 1 o1:o2 o2:o3 o1:o3 o1:o1-1"
	"o3\\[0\\] := .*distinct outputs at 3 steps"
	FAIL_REGEX "Error|unsat|EQUAL OUTPUTS")

# --- updates -----------------------------------------------------------------
# The minimal example of the README's pointwise revision: an unsatisfiable
# update is not applied, and a conflicting one replaces the o1 part while the
# part u[t] = i1[t], which shares no output with it, is kept.
add_multiline_repl_test(interpreter_paths-readme_pwr_minimal_example
	"Updated specification[^\n]*always \\(u\\[t\\]:tau = i1\\[t\\]:tau && o1\\[t\\]:tau' = 0\\).*u\\[1\\] *:= F.*u\\[2\\] *:= always o1\\[t\\]:tau = 0\nUpdated specification[^\n]*always \\(u\\[t\\]:tau = i1\\[t\\]:tau && o1\\[t\\]:tau = 0\\).*o1\\[3\\] := F.*o1\\[4\\] := F"
	STDIN "run 5 steps u[t] = i1[t].\\no1[t] = 1.\\no2[t] = 0 && o2[t] = 1.\\no1[t] = 0.\\nF.\\nF.\\nq\\nq\\n")
# An update relating o1 and o2 joins their two parts into one: the revised
# specification keeps the update, and from the next step o1 equals o2.
add_multiline_repl_test(interpreter_paths-update_merges_spec_parts
	"Updated specification[^\n]*o2\\[t\\]:tau = o1\\[t\\]:tau.*(o1\\[2\\] := T.*o2\\[2\\] := T|o1\\[2\\] := F.*o2\\[2\\] := F)"
	STDIN "run 4 steps u[t] = i1[t] && o1[t] = 0 && o2[t] = 1.\\nF.\\nalways o1[t] = o2[t].\\nF.\\nF.\\nq\\nq\\n"
	FAIL_REGEX "Error|no update was performed")

# A revision of a data-game run that adds a qlt stream holds from the step
# after the update, its lookback reading the values already played: o1 keeps
# alternating and o3 is 5 at every step.
add_multiline_repl_test(interpreter_paths-data_game_revision_new_qlt_stream
	"o1\\[3\\] := 1.*o1\\[4\\] := 0.*o3\\[4\\] := 5\n.*o1\\[5\\] := 1.*o3\\[5\\] := 5"
	STDIN "run 6 steps (always u[t] = i1[t] && o1[t]:bv[1] != o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = 1).\\nF.\\nF.\\nalways o3[t]:qlt = {5}:qlt.\\nF.\\nF.\\nq\\nq\\n"
	NO_FAIL_REGEX REQUIRES ltlsynt)

# u[t] = i1[t] holds for every value of i1, a temporal formula included: the
# step outputs it and the run goes on. The update sometimes (always o2 = 1)
# contradicts o2[t] = 0 and replaces it, as a pointwise revision does; only
# the data game plays it, from step 2 on.
add_multiline_repl_test(interpreter_paths-temporal_update_value_keeps_run
	"u\\[1\\] *:= F always o2.*Updated specification[^\n]*F always o2\\[t\\]:tau' = 0.*o2\\[2\\] := T.*o2\\[3\\] := T"
	STDIN "run 4 steps u[t] = i1[t] && o2[t] = 0.\\nF.\\nsometimes (always o2[t]:tau = 1).\\nF.\\nF.\\nq\\nq\\n"
	FAIL_REGEX "%[0-9]+: " REQUIRES ltlsynt)
# an until on the update stream is refused the same way, and the run goes on
add_multiline_repl_test(interpreter_paths-until_update_value_keeps_run
	"u\\[1\\] *:= .* U .*o2\\[3\\] := F"
	STDIN "run 4 steps u[t] = i1[t] && o2[t] = 0.\\nF.\\n(o1[t] = 1) until (o2[t] = 1).\\nF.\\nF.\\nq\\nq\\n"
	FAIL_REGEX "%[0-9]+: " REQUIRES ltlsynt)
# an update `sometimes (always ...)` of a run the data game plays is applied,
# and o1 keeps alternating across it
add_multiline_repl_test(interpreter_paths-data_game_update_sometimes_always
	"Updated specification.*F always o2\\[t\\]:bv\\[1\\] = 0.*o1\\[3\\] := 1.*o1\\[4\\] := 0"
	STDIN "run 5 steps (always u[t] = i1[t] && o1[t]:bv[1] != o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = 1).\\nF.\\nsometimes (always o2[t]:bv[1] = 0).\\nF.\\nF.\\nq\\nq\\n"
	FAIL_REGEX "Assertion|Aborted|nested in an always" REQUIRES ltlsynt)

# --- step inputs ---------------------------------------------------------------
# A value that does not parse is reported and the same step is asked again,
# with a step budget as without one.
add_multiline_repl_test(interpreter_paths-step_budget_rejected_value_reasks
	"failed to parse the input value for the stream.*o\\[1\\] := F"
	STDIN "run 3 steps o[t] = i[t].\\nT.\\nzzz.\\nF.\\nT.\\nq\\nq\\n"
	NO_FAIL_REGEX)

# --- specification files ---------------------------------------------------------
# With no input to read, the run of a specification file asks to continue
# after each step; q ends it.
add_raw_repl_test(interpreter_paths-spec_file_idle_quit
	"d=$(mktemp -d) && printf 'o1[t] = 1.\\n' > $d/s.tau && printf '\\n\\nq\\n' | ${TAU_RUN} $d/s.tau; r=$?; rm -rf $d; exit $r"
	"o1\\[2\\] := T"
	FAIL_REGEX "Execution step: 3|Error" REQUIRES hostfs)

# A tuple-typed console input of a specification file is read as one wire
# literal from stdin and echoed back through the tuple output.
add_raw_repl_test(interpreter_paths-spec_file_tuple_console_input
	"d=$(mktemp -d) && printf 'type Point = {a: sbf, b: sbf}.\\ni:Point := in console.\\no:Point := out console.\\no[t] = i[t].\\n' > $d/p.tau && printf '{ a: \"1\", b: \"0\" }\\n' | ${TAU_RUN} $d/p.tau -q; r=$?; rm -rf $d; exit $r"
	"o\\[0\\] := \\{ a: \"1\", b: \"0\" \\}"
	REQUIRES hostfs)

# --- compositional parts ---------------------------------------------------------
# A part whose since clause runs as a safety formula has no data-game
# strategy, so the specification runs as a whole: o2 follows the since.
add_multiline_repl_test(interpreter_paths-compositional_part_without_data_game
	"o2\\[1\\] := 1\n.*o2\\[2\\] := 0\n"
	STDIN "run 3 steps (G (F (o1[t]:bv[1] = 1))) && (G((o2[t]:bv[1] = 1) <-> ((o2[t-1]:bv[1] = 0) since (i2[t]:bv[1] = 1)))).\\n1\\n\\n0\\nq\\n"
	FAIL_REGEX "Error|unsat|not executable"
	ENV TAU_LTL_COMPOSITIONAL=on REQUIRES ltlsynt)

# --- revisions of a data-game run --------------------------------------------------
# The weak until the update touches is revised as a whole formula: o3 is 1
# from step 3, which discharges the weak until at once.
add_multiline_repl_test(interpreter_paths-data_game_revision_nested_alternative
	"Updated specification[^\n]* W \\(o2\\[t\\]:bv\\[1\\] = 1 && o3\\[t\\]:bv\\[1\\] = 1\\).*o2\\[3\\] := 1\n.*o3\\[3\\] := 1\n.*o3\\[4\\] := 1\n"
	STDIN "run 5 steps (always u[t] = i1[t] && o1[t]:bv[1] != o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = 1) && ((o3[t]:bv[1] = 0) weak_until (o2[t]:bv[1] = 1 && o3[t]:bv[1] = 1)).\\nF.\\nalways o3[t]:bv[1] = 1.\\nF.\\nF.\\nF.\\nF.\\nq\\nq\\n"
	FAIL_REGEX "Error|no update was performed" REQUIRES ltlsynt)

# A second revision one step after the first reads, two steps back, a value
# the first revised strategy chose before it started: o3 follows each update.
add_multiline_repl_test(interpreter_paths-data_game_second_revision_reads_first
	"Updated specification.*o3\\[5\\] := 1\n.*Updated specification.*o3\\[6\\] := 0\n"
	STDIN "run 7 steps (always u[t] = i1[t] && o1[t]:bv[2] != o1[t-2]:bv[2]) && (sometimes o2[t]:bv[2] = {1}:bv[2]).\\nF.\\nF.\\nalways o3[t]:bv[1] = 1.\\nalways o3[t]:bv[1] = 0.\\nF.\\nF.\\nF.\\nq\\nq\\n"
	FAIL_REGEX "Error|no update was performed" REQUIRES ltlsynt)

# A fixed step of the update counts from the revision step, and its input is
# read there, and only there: o3[5] takes the value of i3[5].
add_multiline_repl_test(interpreter_paths-data_game_update_fixed_input_step
	"Updated specification[^\n]*o3\\[5\\]:bv\\[1\\] = i3\\[5\\]:bv\\[1\\].*o3\\[5\\] := 1\n"
	STDIN "run 6 steps (always u[t] = i1[t] && o1[t]:bv[1] != o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = 1).\\nF.\\nF.\\no3[1]:bv[1] = i3[1]:bv[1].\\nF.\\nF.\\n1\\nq\\nq\\n"
	FAIL_REGEX "Error|no update was performed" REQUIRES ltlsynt)

# A new output stream of the update whose file cannot be opened skips each
# candidate, so the update is undecided and the run goes on.
add_multiline_repl_test(interpreter_paths-data_game_update_output_file_cannot_open
	"failed to open file.*the updated specification could not be decided; no update was performed.*o1\\[4\\] := 0\n"
	STDIN "o5:bv[1] := out file(\"/nonexistent-tau-test-dir/out.txt\").\\nrun 5 steps (always u[t] = i1[t] && o1[t]:bv[1] != o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = 1).\\nF.\\nalways o5[t]:bv[1] = 1.\\nF.\\nF.\\nF.\\nq\\nq\\n"
	REQUIRES ltlsynt hostfs)

# Under a node cap the game leaves the revision with bv[4] arithmetic
# undecided, and another decider's strategy starts at step 0, which a
# revision does not replay: no update, and o1 keeps alternating.
add_multiline_repl_test(interpreter_paths-data_game_revision_undecided_under_node_cap
	"the updated specification could not be decided; no update was performed.*o1\\[3\\] := 1\n.*o1\\[4\\] := 0\n"
	STDIN "set ltl-data-game-max-nodes 128\\nrun 5 steps (always u[t] = i1[t] && o1[t]:bv[1] != o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = 1).\\nF.\\nalways (o4[t]:bv[4] << {1}:bv[4]) = {8}:bv[4] && max(o5[t]:bv[4], i2[t]:bv[4]) = {15}:bv[4].\\nF.\\nF.\\nq\\nq\\n"
	REQUIRES ltlsynt)

# --- revisions of a compositional run ------------------------------------------------
# The second revision reads o2 two steps back, before the first revised
# part started: o2 differs from o2[t-2] after it.
add_multiline_repl_test(interpreter_paths-compositional_second_revision_reads_part
	"Updated specification.*Updated specification[^\n]*o2\\[t-2\\].*o2\\[5\\] := 0\n.*o2\\[6\\] := 1\n"
	STDIN "run 7 steps (always u[t] = i1[t]) && (G (F (o2[t]:bv[1] = 1))) && (G (F (o3[t]:bv[1] = 1 && o3[t-1]:bv[1] = 0))).\\nF.\\nF.\\nalways (o2[t]:bv[1] = 0 || o2[t-1]:bv[1] = 0).\\nalways o2[t]:bv[1] != o2[t-2]:bv[1].\\nF.\\nF.\\nF.\\nq\\nq\\n"
	FAIL_REGEX "Error|no update was performed|not accepted"
	ENV TAU_LTL_COMPOSITIONAL=on REQUIRES ltlsynt)
# a new output no part plays reads 0 before the revision: o5 starts at 1
add_multiline_repl_test(interpreter_paths-compositional_revision_new_lookback_output
	"Updated specification.*o5\\[4\\] := 1\n.*o5\\[5\\] := 0\n"
	STDIN "run 6 steps (always u[t] = i1[t]) && (G (F (o2[t]:bv[1] = 1))) && (G (F (o3[t]:bv[1] = 1 && o3[t-1]:bv[1] = 0))).\\nF.\\nF.\\nalways o5[t]:bv[1] != o5[t-1]:bv[1].\\nF.\\nF.\\nq\\nq\\n"
	FAIL_REGEX "Error|no update was performed|not accepted"
	ENV TAU_LTL_COMPOSITIONAL=on REQUIRES ltlsynt)

# The second revision reads i2 two steps back, before the first revised
# part started and before the run read i2: the values that part started
# from stand in for them, and o4 follows i2 two steps later.
add_multiline_repl_test(interpreter_paths-compositional_second_revision_reads_input
	"Updated specification[^\n]*i2\\[t-2\\].*o4\\[6\\] := 1\n"
	STDIN "run 7 steps (always u[t] = i1[t]) && (G (F (o2[t]:bv[1] = 1))) && (G (F (o3[t]:bv[1] = 1 && o3[t-1]:bv[1] = 0))).\\nF.\\nF.\\nalways o4[t]:bv[1] = i2[t-1]:bv[1].\\nalways o4[t]:bv[1] = i2[t-2]:bv[1].\\n1\\nF.\\n1\\nF.\\n0\\nF.\\n1\\nq\\nq\\n"
	FAIL_REGEX "Error|reads values the run does not keep"
	ENV TAU_LTL_COMPOSITIONAL=on REQUIRES ltlsynt)

# --- revisions that open streams ---------------------------------------------------------
# A new input or output stream of the update whose file cannot be opened
# rejects the update, and the run goes on.
add_multiline_repl_test(interpreter_paths-update_input_file_cannot_open
	"failed to open file.*no update candidate was accepted; no update was performed.*o1\\[3\\] := F"
	STDIN "i2:tau := in file(\"/nonexistent-tau-test-dir/in.txt\").\\nrun 4 steps u[t] = i1[t] && o1[t] = 0.\\nF.\\nalways o2[t] = i2[t].\\nF.\\nF.\\nq\\nq\\n"
	REQUIRES hostfs)
add_multiline_repl_test(interpreter_paths-update_output_file_cannot_open
	"failed to open file.*no update candidate was accepted; no update was performed.*o1\\[3\\] := F"
	STDIN "o5:tau := out file(\"/nonexistent-tau-test-dir/out.txt\").\\nrun 4 steps u[t] = i1[t] && o1[t] = 0.\\nF.\\nalways o5[t] = 1.\\nF.\\nF.\\nq\\nq\\n"
	REQUIRES hostfs)

# --- streams of a run ----------------------------------------------------------------------
# A data-game run whose input or output file cannot be opened is refused
# before its first step.
add_multiline_repl_test(interpreter_paths-data_game_input_file_cannot_open
	"failed to open file \\(name=/nonexistent-tau-test-dir/in.txt\\)"
	STDIN "i2:bv[1] := in file(\"/nonexistent-tau-test-dir/in.txt\").\\nrun 3 steps (always o1[t]:bv[1] != o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = i2[t]:bv[1]).\\nq\\nq\\n"
	FAIL_REGEX "o1\\[0\\] :=" REQUIRES ltlsynt hostfs)
add_multiline_repl_test(interpreter_paths-data_game_output_file_cannot_open
	"failed to open file \\(name=/nonexistent-tau-test-dir/out.txt\\)"
	STDIN "o2:bv[1] := out file(\"/nonexistent-tau-test-dir/out.txt\").\\nrun 3 steps (always o1[t]:bv[1] != o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = 1).\\nq\\nq\\n"
	FAIL_REGEX "o1\\[0\\] :=" REQUIRES ltlsynt hostfs)

# A run of a specification file stops when an output cannot be written. The
# node mount of a wasm build does not pass the refusal of /dev/full on.
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" AND NOT EMSCRIPTEN)
	add_raw_repl_test(interpreter_paths-spec_file_output_write_fails
		"d=$(mktemp -d) && printf 'o1:tau := out file(\"/dev/full\").\\no1[t] = 1.\\n' > $d/s.tau && printf '\\n\\nq\\n' | ${TAU_RUN} $d/s.tau; rm -rf $d; exit 0"
		"failed to write to the output stream \\(name=o1\\).*Execution stopped on a failed step"
		NO_FAIL_REGEX REQUIRES hostfs)
endif()

# The update stream takes the largest value the step allows: u & o2 = 0
# with o2 = 0 gives u = T.
add_multiline_repl_test(interpreter_paths-update_stream_takes_max_value
	"o2\\[0\\] := F\n.*u\\[0\\] +:= T\n.*u\\[1\\] +:= T\n"
	STDIN "run 2 steps u[t] & o2[t] = 0 && o1[t] = 0.\\n\\n\\nq\\nq\\n")

# The this stream after an update whose part keeps two alternatives: the
# conditional alternative i2 = 0 is the one the step follows.
add_multiline_repl_test(interpreter_paths-this_stream_after_conditional_update
	"Updated specification[^\n]*always \\(i2\\[t\\]:tau = 0 && o2\\[t\\]:tau = 0\\) \\|\\| always o2\\[t\\]:tau = 0.*o2\\[2\\] := F\n.*o2\\[3\\] := F\n"
	STDIN "run 4 steps u[t] = i1[t] && this[t] = o1[t] && (i2[t] & o2[t]) = i2[t].\\nF.\\nF.\\nalways o2[t] = 0.\\nT.\\nF.\\nF.\\nF.\\nF.\\nq\\nq\\n")

# --- uninterpreted constants and revisions ------------------------------------------------
# The constraint on an uninterpreted constant is solved with the part that
# outputs it: o1 is nonzero, as <:a> is.
add_multiline_repl_test(interpreter_paths-uconst_constraint_holds_in_outputs
	"o1\\[0\\] := "
	STDIN "run 2 steps always o1[t] = <:a> && <:a> != 0.\\nq\\nq\\n"
	FAIL_REGEX "o1\\[0\\] := F\n")

# Keeping the running goal o2 = 1 beside the update o2 = 3 loses from the
# values already played, so the goal gives way and o2 is 3 from step 4.
add_multiline_repl_test(interpreter_paths-data_game_revision_drops_conflicting_goal
	"Updated specification.*o2\\[4\\] := 3\n.*o2\\[5\\] := 3\n"
	STDIN "run 6 steps (always u[t] = i1[t] && o1[t]:bv[2] != o1[t-1]:bv[2]) && (sometimes o2[t]:bv[2] = {1}:bv[2]).\\nF.\\nF.\\nalways o2[t]:bv[2] = {3}:bv[2].\\nF.\\nF.\\nF.\\nq\\nq\\n"
	REQUIRES ltlsynt)

# An update over a new input reads it from the revision step: o2[4] is the
# input 2.
add_multiline_repl_test(interpreter_paths-data_game_revision_reads_new_input
	"o2\\[4\\] := 2\n"
	STDIN "run 6 steps (always u[t] = i1[t] && o1[t]:bv[2] != o1[t-1]:bv[2]) && (sometimes o2[t]:bv[2] = {1}:bv[2]).\\nF.\\nF.\\nalways o2[t]:bv[2] = i2[t]:bv[2].\\nF.\\n{2}:bv[2]\\nF.\\n{2}:bv[2]\\nq\\nq\\n"
	REQUIRES ltlsynt)

# Under a node cap the strategy runs as a two-state Mealy safety encoding,
# whose revision restarts the lookback window of the revised specification
# (README, pointwise revision): its clause looks back one step, so o4 is free
# at step 2 and the clause holds again from step 3, o1 alternating and o4 = 1.
add_multiline_repl_test(interpreter_paths-mealy_revision_restarts_its_lookback
	"Updated specification[^\n]*\n.*o4\\[2\\] := 0\n.*o1\\[3\\] := 1\n.*o4\\[3\\] := 1\n.*o1\\[4\\] := 0\n.*o4\\[4\\] := 1\n.*o1\\[5\\] := 1\n.*o4\\[5\\] := 1\n|Updated specification[^\n]*\n.*o4\\[2\\] := 0\n.*o1\\[3\\] := 0\n.*o4\\[3\\] := 1\n.*o1\\[4\\] := 1\n.*o4\\[4\\] := 1\n.*o1\\[5\\] := 0\n.*o4\\[5\\] := 1\n"
	STDIN "set ltl-data-game-max-nodes 16\\nrun 6 steps (always u[t] = i1[t] && o1[t]:bv[1] != o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = 1).\\nF.\\nalways o4[t]:bv[1] = 1.\\nF.\\nF.\\nF.\\nF.\\nq\\nq\\n"
	REQUIRES ltlsynt)
# the same restart in a safety run: o1[3] and o4[3] are free, step 4 holds
add_multiline_repl_test(interpreter_paths-safety_revision_restarts_its_lookback
	"Updated specification.*o1\\[4\\] := 1\n.*o4\\[4\\] := 1\n.*o1\\[5\\] := 0\n.*o4\\[5\\] := 1\n"
	STDIN "run 6 steps always u[t] = i1[t] && o1[t]:bv[1] != o1[t-1]:bv[1].\\nF.\\nalways o4[t]:bv[1] = 1.\\nF.\\nF.\\nF.\\nF.\\nq\\nq\\n")
# The restart does not read the values played before the revision (README,
# pointwise revision details: "updating a specification in step s with
# o1[t] = i1[t-1] means that in the next step o1[s+1] is unspecified"): the
# revision u[t] = i[t] | i[t-1] at step 3 leaves u[4] at the bottom value,
# although i[3] is not 0, and u[t] = i[t] | i[t-1] holds from step 5 on. The
# Python update tests pin the same values through the bindings.
add_multiline_repl_test(interpreter_paths-revision_lookback_skips_the_revision_step
	"Updated specification[^\n]*\n.*u\\[4\\] *:= F\n.*u\\[5\\] *:= F\n.*u\\[6\\] *:= T\n.*u\\[7\\] *:= T\n"
	STDIN "run 8 steps u[t] = i[t].\\nT.\\nF.\\nT.\\nu[t] = i[t] | i[t-1].\\nF.\\nF.\\nT.\\nF.\\nq\\nq\\n")
# A revised clause starts at its own lookback, counted from the revision: the
# disequalities added at step 1 leave step 2 free (o2[2] may equal o3[2]) and
# hold at every step from 3 on (README, pointwise revision details: "an always
# clause that looks back k steps holds only from k steps after the revision").
add_raw_repl_test(interpreter_paths-revised_disequalities_start_after_warm_up
	"printf 'run 6 steps always u[t] = i1[t] && o1[t]:sbf = 0.\\nF.\\nalways o2[t]:sbf != o3[t]:sbf && o2[t]:sbf != o2[t-1]:sbf.\\nF.\\nF.\\nF.\\nF.\\nq\\nq\\n' | ${TAU_RUN} -X | bash ${TAU_DISTINCT_CHECKER} 3 o2:o3 o2:o2-1"
	"Updated specification.*o2\\[2\\] := .*o3\\[2\\] := .*distinct outputs at 3 steps"
	FAIL_REGEX "Error|unsat|EQUAL OUTPUTS")

# A realizable update whose goal is a disjunction keeps its value: the tau
# constant is not 0, although neither disjunct is realizable alone.
add_multiline_repl_test(interpreter_paths-update_disjunctive_goal_keeps_its_value
	"u\\[1\\] *:= always o2\\[t\\]:bv\\[1\\] = i2\\[t\\]:bv\\[1\\] && F o2"
	STDIN "run 4 steps u[t] = i1[t] && o1[t] = 0.\\nF.\\n(always o2[t]:bv[1] = i2[t]:bv[1]) && ((sometimes o2[t]:bv[1] = 1) || (always o2[t]:bv[1] = 0)).\\nF.\\n0\\nF.\\n0\\nq\\nq\\n"
	FAIL_REGEX "solved to 0|u\\[1\\] *:= F\n" REQUIRES ltlsynt)
# Such an update needs a game to run: the safety run hands the revised
# specification to the data game, whose strategy copies i2 into o2.
add_multiline_repl_test(interpreter_paths-update_realizable_disjunctive_goal
	"Updated specification.*o2\\[2\\] := 0\n.*o2\\[3\\] := 1\n"
	STDIN "run 4 steps u[t] = i1[t] && o1[t] = 0.\\nF.\\n(always o2[t]:bv[1] = i2[t]:bv[1]) && ((sometimes o2[t]:bv[1] = 1) || (always o2[t]:bv[1] = 0)).\\nF.\\n0\\nF.\\n1\\nq\\nq\\n"
	FAIL_REGEX "no update was performed" REQUIRES ltlsynt)
