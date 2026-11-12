#
# Program-level combination of specifications: a term of type tau is a
# Boolean expression over sets of programs, and `realizable`, `sat` and the
# equalities between tau constants decide it through the compiled expression
# (src/program_set.h). Every case over an input needs ltlsynt on PATH.
#

include(add_repl_test)

set(_copy "{ always o1[t]:sbf = i1[t]:sbf }:tau")
set(_zero "{ always o1[t]:sbf = 0 }:tau")
set(_one "{ always o1[t]:sbf = 1 }:tau")

# program_set_verdict(<name> <command> <verdict>)
function(program_set_verdict name cmd verdict)
	add_repl_test(program_set-${name} "${cmd}" ": ${verdict}\n" NO_TRACE
		REQUIRES ltlsynt sbf)
endfunction()

# realizable of a term: the set has a program
program_set_verdict(complement_of_copier_nonempty
	"realizable ${_copy}'" T)
program_set_verdict(set_and_its_complement_disjoint
	"realizable ${_copy} & ${_copy}'" F)
program_set_verdict(copier_outside_eventually_one
	"realizable ${_copy} & { sometimes o1[t]:sbf = 1 }:tau'" T)
program_set_verdict(copying_entails_implication
	"realizable ${_copy} & { always (i1[t]:sbf = 1 -> o1[t]:sbf = 1) }:tau'" F)
program_set_verdict(union_cannot_fit_both_inputs
	"realizable (${_one} | ${_zero}) & { always ((i1[t]:sbf = 1) <-> (o1[t]:sbf = 1)) }:tau" F)
program_set_verdict(double_complement_is_the_set
	"realizable ${_copy}'' & ${_copy}'" F)

# The Tau BA equalities: a constant over inputs is its set of programs
program_set_verdict(copier_not_equal_to_zero
	"sat ${_copy} = ${_zero}" F)
program_set_verdict(copier_not_included_in_zero
	"sat ${_copy} & ${_zero}' = 0" F)
program_set_verdict(zero_not_included_in_copier
	"sat ${_zero} & ${_copy}' = 0" F)
program_set_verdict(copier_included_in_itself
	"sat ${_copy} & ${_copy}' = 0" T)
program_set_verdict(copier_equal_to_itself
	"sat ${_copy} = ${_copy}" T)
program_set_verdict(copier_implies_the_implication
	"sat ${_copy} & { always (i1[t]:sbf = 1 -> o1[t]:sbf = 1) }:tau' = 0" T)
program_set_verdict(complement_of_copier_is_not_everything
	"sat ${_copy}' = 1" F)

# Input-free constants keep the plain reading
add_repl_test(program_set-input_free_one_not_included_in_zero
	"sat ${_one} & ${_zero}' = 0" ": F\n" NO_TRACE REQUIRES sbf)
add_repl_test(program_set-input_free_complement_of_everything
	"sat { T. }:tau' = 0" ": T\n" NO_TRACE)
add_repl_test(program_set-input_free_complement_of_nothing
	"sat { F. }:tau' = 0" ": F\n" NO_TRACE)

# the previous reading stays reachable
add_repl_test(program_set-trace_reading_on_request
	"set ba-complement trace. sat ${_copy} = ${_zero}." ": T\n" NO_TRACE
	REQUIRES sbf)

# ltl and unrealizable_core read the compiled expression
add_repl_test(program_set-ltl_of_a_term
	"ltl ${_copy} & ${_zero}'" "w_ps0_d_i1" NO_TRACE REQUIRES ltlsynt sbf)
add_repl_test(program_set-unrealizable_core_of_a_term
	"unrealizable_core ${_copy} & ${_copy}'" "w_ps0_d_i1" NO_TRACE
	FAIL_REGEX "^realizable" REQUIRES ltlsynt sbf)

# A run of a term plays the compiled expression; the hidden streams are
# solved and never printed. The input goes through printf in single quotes,
# so the complement's quote is closed and reopened around it.
add_multiline_repl_test(program_set-run_copier_outside_zero
	"o1\\[0\\] := 0\n.*o1\\[1\\] := 1\n.*o1\\[2\\] := 1\n.*o1\\[3\\] := 0"
	STDIN "run 4 steps ${_copy} & ${_zero}'.\\n0\\n1\\n1\\n0\\nq\\n"
	FAIL_REGEX "w_ps[^\n]*:=|unrealizable|not executable" REQUIRES ltlsynt sbf)
add_multiline_repl_test(program_set-run_input_free_union
	"o1\\[0\\] := 1\n.*o1\\[1\\] := 1\n.*o1\\[2\\] := 1"
	STDIN "run 3 steps ${_one} | { always o1[t]:sbf = o1[t]:sbf }:tau'.\\nq\\n"
	FAIL_REGEX "w_ps[^\n]*:=|unrealizable|not executable" REQUIRES ltlsynt sbf)
add_repl_test(program_set-run_of_an_empty_set_is_refused
	"run 2 steps ${_copy} & ${_copy}'" "unrealizable|not executable|unsat|empty"
	NO_TRACE NO_FAIL_REGEX REQUIRES ltlsynt sbf)
add_repl_test(program_set-ltl_names_the_hidden_streams
	"ltl ${_copy} & ${_zero}'" "w_ps0_d_i1: the prediction of i1" NO_TRACE
	REQUIRES ltlsynt sbf)

# A generated program solves the hidden streams of a term and never prints
# them: they get a vector output, no console label. printf spells the
# complement's quote as \x27, since the command sits in single quotes.
add_raw_repl_test(program_set-gen_keeps_the_hidden_streams_silent
	"d=$(mktemp -d) && printf '{ always o1[t]:sbf = i1[t]:sbf }:tau & { always o1[t]:sbf = 0 }:tau\\x27.\\n' > $d/s.tau && ${TAU_RUN} gen $d/s.tau -o $d/g && grep add_output $d/g/main.cpp; r=$?; rm -rf $d; exit $r"
	"ctx.add_output[(]\"w_ps0_d_i1\", [0-9]+, make_shared<vector_output_stream>[(][)][)]"
	FAIL_REGEX "add_output_console[(]\"w_ps" REQUIRES hostfs ltlsynt sbf)

# A positive CTL* leaf is conjoined; a complemented one is refused.
add_repl_test(program_set-positive_E_leaf_is_conjoined
	"fragment ctl_star. realizable { E (sometimes i1[t]:sbf = 1) }:tau & { always o1[t]:sbf = i1[t]:sbf }:tau"
	": T" REQUIRES ltlsynt sbf)
add_repl_test(program_set-positive_E_leaf_keeps_its_verdict
	"fragment ctl_star. realizable { E (always i1[t]:sbf = 1) }:tau & { always o1[t]:sbf = 0 }:tau"
	": F" REQUIRES ltlsynt sbf)
add_repl_test(program_set-complemented_E_leaf_is_refused
	"fragment ctl_star. realizable { E (sometimes i1[t]:sbf = 1) }:tau'"
	"complemented program-set literal is a plain specification" NO_FAIL_REGEX REQUIRES sbf)
# A run plays an E witness and a complement's prediction together; neither
# is printed.
add_multiline_repl_test(program_set-run_mixes_E_witness_and_complement
	"Execution step: 0\n.*Execution step: 1\n.*Execution step: 2"
	STDIN "fragment ctl_star.\\nrun 3 steps { E (sometimes i1[t]:sbf = 1) }:tau & ${_zero}'.\\n0\\n1\\n0\\nq\\n"
	FAIL_REGEX "w_[^\n]*:=|unrealizable|not executable" REQUIRES ltlsynt sbf)

# A constant with no temporal operator is its always: the bare spelling
# combines with a complement or a union as the always spelling does.
add_repl_test(program_set-bare_literal_equality
	"sat { o1[t]:sbf = i1[t]:sbf }:tau = { o1[t]:sbf = 0 }:tau" ": F"
	REQUIRES ltlsynt sbf)
add_repl_test(program_set-bare_literal_not_included
	"sat { o1[t]:sbf = i1[t]:sbf }:tau & { o1[t]:sbf = 0 }:tau' = 0" ": F"
	REQUIRES ltlsynt sbf)
add_repl_test(program_set-bare_literal_not_included_either_way
	"sat { o1[t]:sbf = 0 }:tau & { o1[t]:sbf = i1[t]:sbf }:tau' = 0" ": F"
	REQUIRES ltlsynt sbf)
add_repl_test(program_set-bare_literal_self_inclusion
	"sat { o1[t]:sbf = i1[t]:sbf }:tau & { o1[t]:sbf = i1[t]:sbf }:tau' = 0" ": T"
	REQUIRES ltlsynt sbf)
add_repl_test(program_set-bare_literal_union
	"realizable { o1[t]:sbf = i1[t]:sbf }:tau | { o1[t]:sbf = 0 }:tau" ": T"
	REQUIRES ltlsynt sbf)
add_repl_test(program_set-bare_input_free_complement_is_sometimes
	"sat { o1[t]:sbf = 1 }:tau' = { sometimes o1[t]:sbf != 1 }:tau" ": T"
	REQUIRES sbf)

# sometimes has one reading, the goal reached whatever the inputs do; = 1 is
# validity, every input read as an output; = 0 is unrealizability.
add_repl_test(program_set-sometimes_over_input_is_zero
	"sat { sometimes i1[t]:sbf = 1 }:tau = 0" ": T" REQUIRES sbf)
add_repl_test(program_set-sometimes_over_input_is_not_one
	"sat { sometimes i1[t]:sbf = 1 }:tau = 1" ": F" REQUIRES sbf)
add_repl_test(program_set-complement_of_always_over_input_is_one
	"sat { always i1[t]:sbf = 0 }:tau' = 1" ": T" REQUIRES sbf)
add_repl_test(program_set-one_is_validity
	"sat { always (i1[t]:sbf = 0 || o1[t]:sbf = 1) }:tau = 1" ": F" REQUIRES sbf)
add_repl_test(program_set-unrealizable_recurrence_is_zero
	"sat { always (sometimes i1[t]:sbf = 1) }:tau = 0" ": T" REQUIRES ltlsynt sbf)
