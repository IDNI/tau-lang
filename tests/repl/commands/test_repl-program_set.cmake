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
