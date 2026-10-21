#
# Paths of the execution engine that the other run suites leave out: the
# routes make_interpreter takes for a fixed step or a disjunction the safety
# pipeline cannot play, an update that joins two parts of the running
# specification, a revision of a data-game run that adds a stream of another
# type, a run with a step budget that receives a value it cannot parse, and
# the run loop of a specification file with its idle prompt and a tuple
# console input. Cases marked WILL_FAIL pin the behaviour the specification
# asks for where the run does something else today.
#

include(add_repl_test)
include(tau_repl_pack)

# Marks a case that expects the correct behaviour of an open defect.
function(interpreter_paths_expect_failure name)
	if(TEST test_repl-interpreter_paths-${name})
		set_tests_properties(test_repl-interpreter_paths-${name}
			PROPERTIES WILL_FAIL TRUE)
	endif()
endfunction()

# --- routes of make_interpreter ----------------------------------------------
# A fixed step the input decides: the environment can make i1[2] = 0, so the
# step-counter game finds no strategy and the run refuses the specification.
add_multiline_repl_test(interpreter_paths-fixed_step_read_by_input_unsat
	"Tau specification is unsat"
	STDIN "run 3 steps (always o1[t]:bv[1] = i1[t]:bv[1]) && (o1[2]:bv[1] = 1).\\n1\\n1\\n1\\nq\\nq\\n"
	FAIL_REGEX ":= " REQUIRES ltlsynt)

# A disjunction of models over an input is played as a game: one disjunct is
# kept for the whole run, so one output mirrors every input.
add_multiline_repl_test(interpreter_paths-disjunction_played_as_game
	"o1\\[0\\] := 1.*o1\\[1\\] := 0.*o1\\[2\\] := 1|o2\\[0\\] := 1.*o2\\[1\\] := 0.*o2\\[2\\] := 1"
	STDIN "run 3 steps (always o1[t]:bv[1] = i1[t]:bv[1]) || (always o2[t]:bv[1] = i1[t]:bv[1]).\\n1\\n0\\n1\\nq\\nq\\n"
	FAIL_REGEX "unsat|Error" REQUIRES ltlsynt)

# --- updates -----------------------------------------------------------------
# An update relating o1 and o2 joins their two parts into one: the revised
# specification keeps the update, and from the next step o1 equals o2.
add_multiline_repl_test(interpreter_paths-update_merges_spec_parts
	"Updated specification[^\n]*o2\\[t\\]:tau = o1\\[t\\]:tau.*(o1\\[2\\] := T.*o2\\[2\\] := T|o1\\[2\\] := F.*o2\\[2\\] := F)"
	STDIN "run 4 steps u[t] = i1[t] && o1[t] = 0 && o2[t] = 1.\\nF.\\nalways o1[t] = o2[t].\\nF.\\nF.\\nq\\nq\\n"
	FAIL_REGEX "Error|no update was performed")

# A revision of a data-game run that adds a qlt stream holds from the step
# after the update: the revised specification asks o3 = 5 at every step, and
# step 4 prints 6.
add_multiline_repl_test(interpreter_paths-data_game_revision_new_qlt_stream
	"o3\\[4\\] := 5\n.*o3\\[5\\] := 5"
	STDIN "run 6 steps (always u[t] = i1[t] && o1[t]:bv[1] != o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = 1).\\nF.\\nF.\\nalways o3[t]:qlt = {5}:qlt.\\nF.\\nF.\\nq\\nq\\n"
	NO_FAIL_REGEX REQUIRES ltlsynt)
interpreter_paths_expect_failure(data_game_revision_new_qlt_stream)

# u[t] = i1[t] holds for every value of i1, a temporal formula included: the
# step outputs it and the run goes on, with or without the update. Today the
# step fails ("Specification part has no solvable alternative") and the run
# ends at step 1.
add_multiline_repl_test(interpreter_paths-temporal_update_value_keeps_run
	"o2\\[2\\] := "
	STDIN "run 4 steps u[t] = i1[t] && o2[t] = 0.\\nF.\\nsometimes (always o2[t]:tau = 1).\\nF.\\nF.\\nq\\nq\\n"
	NO_FAIL_REGEX)
interpreter_paths_expect_failure(temporal_update_value_keeps_run)

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
