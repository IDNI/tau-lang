#
# The options that steer the feasibility checks of the LTL(ABA) pipeline
# (phi-delta-crosscheck, ltl-qe-max-vars, ltl-max-observations) leave every
# verdict as it is, and the step-counter encoding of fixed-position atoms
# tells a stream from a longer name ending in it. The AIGER export turns
# each comparison of bv[1] terms into a Boolean formula over the bits of
# its streams: the circuits below are the ones ltlsynt verifies, and an
# ltlsynt stub gives the ones it refuses. Past 22 atoms of one type the
# k-ary consistency pass walks the positive subsets, and an atom false at
# every step forbids only atoms of its own type.
#

include(add_repl_test)
include(tau_repl_pack)

# The shadow check of the closed form of Phi_Delta runs on every matching
# conjunction of sbf atoms and changes no verdict; an atom against 1 is no
# matching shape.
add_repl_test(ltl_normalization-phi_delta_crosscheck_keeps_realizable
	"set phi-delta-crosscheck true. realizable G(F(o1[t]:sbf = 0)) && G(F(o1[t]:sbf != 0))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_normalization-phi_delta_crosscheck_keeps_unrealizable
	"set phi-delta-crosscheck true. realizable G(F(o1[t]:sbf = 0)) && F(G(o1[t]:sbf != 0))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_normalization-phi_delta_crosscheck_ineligible_shape
	"set phi-delta-crosscheck true. realizable G(F(o1[t]:sbf = 1)) && G(F(o1[t]:sbf = 0))"
	": T" REQUIRES ltlsynt)

# With no variable eliminated on its own, the solver decides each check;
# with three, the order chain o1 < o2 < o3 < 0 < 5 < o1 is still refused.
add_repl_test(ltl_normalization-qe_max_vars_zero_until_realizable
	"set ltl-qe-max-vars 0. realizable (o1[t]:qlt > {0}:qlt) U (o1[t]:qlt < {0}:qlt)"
	": T" REQUIRES qlt ltlsynt)
add_repl_test(ltl_normalization-qe_max_vars_zero_order_chain_unrealizable
	"set ltl-qe-max-vars 0. realizable G F(o1[t]:qlt < o2[t]:qlt && o2[t]:qlt < o3[t]:qlt && o3[t]:qlt < {0}:qlt && o1[t]:qlt > {5}:qlt)"
	": F" REQUIRES qlt ltlsynt)
add_repl_test(ltl_normalization-qe_max_vars_three_order_chain_unrealizable
	"set ltl-qe-max-vars 3. realizable G F(o1[t]:qlt < o2[t]:qlt && o2[t]:qlt < o3[t]:qlt && o3[t]:qlt < {0}:qlt && o1[t]:qlt > {5}:qlt)"
	": F" REQUIRES qlt ltlsynt)

# Past the observation cap no combination of observations is assumed away:
# the observed abstraction stays open and the data decides the verdict.
add_repl_test(ltl_normalization-max_observations_cap_keeps_verdict
	"set ltl-max-observations 1. realizable (o1[t]:sbf = i1[t-1]:sbf) until (o1[t]:sbf = {X&Y}:sbf)"
	"observations give the environment choices the data may not allow.*: T" REQUIRES ltlsynt)

# `o1[2]` inside `oo1[2]` is a fixed position of another stream.
add_repl_test(ltl_normalization-fixed_position_inside_longer_name
	"realizable (always o1[2]:sbf = oo1[2]:sbf) && F(o1[t]:sbf != 0)"
	": T" REQUIRES ltlsynt)

# `tau gen --aiger` writes the circuit of `spec`, then the case prints it.
function(add_aiger_circuit_test name spec expected)
	add_raw_repl_test(aiger-${name}
		"d=$(mktemp -d) && printf '${spec}\\n' > $d/s.tau && ${TAU_RUN} gen --aiger $d/s.tau && cat $d/s.aag; r=$?; rm -rf $d; exit $r"
		"${expected}" REQUIRES hostfs bv ltlsynt)
endfunction()

# o1 != (i1 & i2) | i1' is o1 = i1 & !i2: one AND of i1 and !i2.
add_aiger_circuit_test(neq_of_or_and_complement
	"G(o1[t]:bv[1] != (i1[t]:bv[1] & i2[t]:bv[1]) | i1[t]:bv[1]'\\'')"
	"\naag 3 2 0 1 1\n2\n4\n6\n6 (2 5|5 2)\n")
# o1 > i1 & i2 unless i1 = 1: o1 = 1 serves every input.
add_aiger_circuit_test(gt_of_and
	"G(o1[t]:bv[1] > (i1[t]:bv[1] & i2[t]:bv[1]) || i1[t]:bv[1] = 1)"
	"\naag 2 2 0 1 0\n2\n4\n1\n")
add_aiger_circuit_test(gteq
	"G(o1[t]:bv[1] >= i1[t]:bv[1])"
	"\naag 1 1 0 1 0\n2\n1\n")
add_aiger_circuit_test(lteq
	"G(o1[t]:bv[1] <= i1[t]:bv[1])"
	"\naag 1 1 0 1 0\n2\n0\n")
add_aiger_circuit_test(lt_unless_zero
	"G(o1[t]:bv[1] < i1[t]:bv[1] || i1[t]:bv[1] = 0)"
	"\naag 1 1 0 1 0\n2\n0\n")
# o1 = i1 + i2 is the exclusive or of the inputs: no constant output.
add_aiger_circuit_test(xor
	"G(o1[t]:bv[1] = (i1[t]:bv[1] + i2[t]:bv[1]))"
	"\naag [0-9]+ 2 0 1 [1-9][0-9]*\n.*o0 o1\n")

# More than 22 atoms of one type: the k-ary consistency pass walks their
# positive subsets instead of enumerating minimal ones. o1 = o2, o2 = {1}
# and o1 = {2} hold pairwise but not together, which the walk forbids; the
# lookback atom of o6 is skipped. A subset cap stops the walk.
set(_k_ary_walk_atoms "o1[t]:bv[8] = o2[t]:bv[8] || o2[t]:bv[8] = {1}:bv[8] || o1[t]:bv[8] = {2}:bv[8] || o6[t]:bv[8] = o6[t-1]:bv[8] || o5[t]:bv[8] = {10}:bv[8] || o5[t]:bv[8] = {11}:bv[8] || o5[t]:bv[8] = {12}:bv[8] || o5[t]:bv[8] = {13}:bv[8] || o5[t]:bv[8] = {14}:bv[8] || o5[t]:bv[8] = {15}:bv[8] || o5[t]:bv[8] = {16}:bv[8] || o5[t]:bv[8] = {17}:bv[8] || o5[t]:bv[8] = {18}:bv[8] || o5[t]:bv[8] = {19}:bv[8] || o5[t]:bv[8] = {20}:bv[8] || o5[t]:bv[8] = {21}:bv[8] || o5[t]:bv[8] = {22}:bv[8] || o5[t]:bv[8] = {23}:bv[8] || o5[t]:bv[8] = {24}:bv[8] || o5[t]:bv[8] = {25}:bv[8] || o5[t]:bv[8] = {26}:bv[8] || o5[t]:bv[8] = {27}:bv[8] || o5[t]:bv[8] = {28}:bv[8] || o5[t]:bv[8] = {29}:bv[8]")
add_repl_test(ltl_normalization-k_ary_walk_forbids_a_triple
	"ltl sometimes (${_k_ary_walk_atoms})"
	"\n  G\\(!\\(p[0-9]+ && p[0-9]+ && p[0-9]+\\)\\)\n.*\nREALIZABLE"
	NO_TRACE REQUIRES ltlsynt)
add_repl_test(ltl_normalization-k_ary_walk_subset_cap
	"set max-consistency-subsets 3. realizable sometimes (${_k_ary_walk_atoms})"
	"k-ary consistency walk capped after 3 subset checks.*: T|: T.*k-ary consistency walk capped after 3 subset checks"
	NO_TRACE REQUIRES ltlsynt)

# An atom kept false at every step forbids only atoms of its own type: the
# sbf atom beside the bitvector one stays free, and o2 meets the goal.
add_repl_test(ltl_normalization-globally_false_atom_skips_other_types
	"ltl (G(o1[t]:bv[8] != {3}:bv[8])) && (F(o1[t]:bv[8] = {3}:bv[8] || o2[t]:sbf = 0))"
	"LTL skeleton: \\(G\\(!p0\\)\\) & \\(F\\(\\(p1 \\| p0\\)\\)\\).*\nREALIZABLE"
	NO_TRACE REQUIRES ltlsynt)

# An ltlsynt whose circuit or machine `tau gen` cannot take
# (tests/repl/stubs/odd_ltlsynt): no circuit, a truncated one, one ltlsynt
# did not verify, an UNREALIZABLE answer `realizable` contradicts, and an
# unverified machine, after which `--machine` follows `run`.
if(NOT WIN32)
	set(_odd_ltlsynt "PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/odd_ltlsynt:$PATH")
	function(add_odd_aiger_test name expected)
		add_raw_repl_test(aiger-${name}
			"d=$(mktemp -d) && printf 'G(o1[t]:bv[1] >= i1[t]:bv[1])\\n' > $d/s.tau && TAU_STUB_LTLSYNT=${name} ${_odd_ltlsynt} ${TAU_RUN} gen --aiger $d/s.tau; r=$?; ls $d; rm -rf $d; exit $r"
			"${expected}" FAIL_REGEX "s\\.aag|Aborted|core dumped"
			REQUIRES hostfs bv ltlsynt)
	endfunction()
	add_odd_aiger_test(aiger_no_circuit "ltlsynt printed no AIGER circuit")
	add_odd_aiger_test(aiger_truncated "the AIGER circuit of ltlsynt is truncated")
	add_odd_aiger_test(aiger_unverified "ltlsynt did not verify the AIGER circuit")
	add_odd_aiger_test(aiger_unrealizable "the AIGER export and `realizable` disagree on this specification; nothing is emitted \\(value=realizable says T\\)")
	add_raw_repl_test(aiger-machine_unverified_follows_run
		"d=$(mktemp -d) && printf 'G(o1[t]:bv[1] = i1[t-1]:bv[1])\\n' > $d/s.tau && TAU_STUB_LTLSYNT=machine_unverified ${_odd_ltlsynt} ${TAU_RUN} gen --machine $d/s.tau -o $d/out && grep -q get_interpreter $d/out/main.cpp && echo SOLVING; r=$?; rm -rf $d; exit $r"
		"--machine: the spec has no bit machine to play[^\n]*\n[^\n]*ltlsynt did not verify the strategy.*SOLVING|SOLVING.*--machine: the spec has no bit machine to play[^\n]*\n[^\n]*ltlsynt did not verify the strategy"
		FAIL_REGEX "Aborted|core dumped" REQUIRES hostfs bv ltlsynt)
endif()
