#
# ltl-semantics finite: the decisions over the finite traces of LTLf, whose
# end the system chooses, against the same specifications over infinite
# traces.
#

include(add_repl_test)
include(tau_repl_pack)

# ltl_semantics_pair(<name> <cmd> <infinite> <finite>): the verdict of cmd
# under each semantics.
function(ltl_semantics_pair name cmd infinite finite)
	add_repl_test(ltl_finite-${name}-infinite "${cmd}" ": ${infinite}"
		REQUIRES ltlsynt bv)
	add_repl_test(ltl_finite-${name}-finite "${cmd}" ": ${finite}"
		ENV TAU_LTL_SEMANTICS=finite REQUIRES ltlsynt bv)
endfunction()

set(_o "o1[t]:bv[1]")
set(_i "i1[t]:bv[1]")

# both values recur only on an infinite trace: on a finite one both would
# hold at the last step
ltl_semantics_pair(sat_both_values_recur
	"sat (G(F(${_o} = 0))) && (G(F(${_o} = 1)))" T F)
ltl_semantics_pair(unsat_both_values_recur
	"unsat (G(F(${_o} = 0))) && (G(F(${_o} = 1)))" F T)
# at the last step of a finite trace G F and F G both read that step
ltl_semantics_pair(valid_recurrence_gives_persistence
	"valid (G(F(${_o} = 1))) -> (F(G(${_o} = 1)))" F T)
# the system ends the trace before it must predict an input
ltl_semantics_pair(realizable_prediction
	"realizable G(${_o} = i1[t+1]:bv[1])" F T)
ltl_semantics_pair(unrealizable_prediction
	"unrealizable G(${_o} = i1[t+1]:bv[1])" T F)
ltl_semantics_pair(realizable_bare_prediction
	"realizable ${_o} = i1[t+1]:bv[1]" F T)
# what the environment controls stays out of reach on a finite trace
ltl_semantics_pair(realizable_input_safety
	"realizable G(${_i} = 0)" F F)
ltl_semantics_pair(realizable_input_liveness
	"realizable G(F(${_i} = 1))" F F)
ltl_semantics_pair(realizable_response
	"realizable G(${_i} = 1 -> F(${_o} = 1))" T T)
ltl_semantics_pair(realizable_since_of_input
	"realizable G((${_o} = 1) S (${_i} = 1))" F F)
ltl_semantics_pair(sat_always_against_sometimes
	"sat (G(${_o} = 1)) && (F(${_o} = 0))" F F)

# the REPL sets it as the CLI and the environment do
add_repl_test(ltl_finite-set_get
	"set ltl-semantics finite. get ltl-semantics"
	"ltl-semantics: *finite" NO_FAIL_REGEX NO_TRACE)
add_repl_test(ltl_finite-set_then_sat
	"set ltl-semantics finite. sat (G(F(${_o} = 0))) && (G(F(${_o} = 1)))"
	": F" REQUIRES ltlsynt bv)
add_env_error_test(ltl_finite-garbage_is_an_error
	TAU_LTL_SEMANTICS=eventually)

# a run follows a strategy over infinite traces
add_repl_test(ltl_finite-run_is_refused
	"run G(${_o} = 1)."
	"over finite traces only" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_SEMANTICS=finite REQUIRES bv)
# a program and a circuit follow a strategy over infinite traces too
add_raw_repl_test(ltl_finite-gen_is_refused
	"d=$(mktemp -d) && printf 'G(o1[t]:bv[1] = 1)\\n' > $d/s.tau && TAU_LTL_SEMANTICS=finite ${TAU_RUN} gen $d/s.tau -o $d/out; r=$?; rm -rf $d; exit $r"
	"over finite traces only" NO_FAIL_REGEX REQUIRES hostfs bv)
add_raw_repl_test(ltl_finite-aiger_is_refused
	"d=$(mktemp -d) && printf 'G(o1[t]:bv[1] = 1)\\n' > $d/s.tau && TAU_LTL_SEMANTICS=finite ${TAU_RUN} gen --aiger $d/s.tau; r=$?; rm -rf $d; exit $r"
	"needs ltl-semantics infinite" NO_FAIL_REGEX REQUIRES hostfs bv)
# CTL* has no LTLf reading
add_repl_test(ltl_finite-ctl_star_is_refused
	"fragment ctl_star. sat E(F(${_o} = 1))"
	"no path quantifiers" NO_FAIL_REGEX NO_TRACE
	ENV TAU_LTL_SEMANTICS=finite REQUIRES bv)
