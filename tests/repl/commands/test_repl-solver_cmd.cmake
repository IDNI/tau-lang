#
# solve / lgrs commands
#

include(add_repl_test)

# solve cvc5 sample with mod
add_repl_test(solver_cvc5_cmd "solve x : bv[16] % { 2 } = { 1 }"
	"x := \\{ 1 \\}:bv\\[16\\]")

# ---------------------------------------------------------------------------

# hsb -- tests/integration/solver/test_integration-solver-hsb.cpp

# find_solution (:17-30) -- test_find_solution, no pinned value (helper.h:18-24)
add_repl_test(solver_cmd-hsb-find_solution-one_var
	"solve {x[0] < 0}:hsb a = 0." "solution: \\{") # hsb.cpp:19-21
add_repl_test(solver_cmd-hsb-find_solution-two_vars
	"solve {x[0] < 0}:hsb a | {x[1] < 0}:hsb b = 0." "solution: \\{") # hsb.cpp:23-25
add_repl_test(solver_cmd-hsb-find_solution-complement
	"solve {x[0] < 0}:hsb a | {x[0] < 0}:hsb' b = 0." "solution: \\{") # hsb.cpp:27-29

# solve_inequality_system (:32-58) -- conjoined with && into one solve call
add_repl_test(solver_cmd-hsb-ineq_system-complement
	"solve {x[0] < 0}:hsb a != 0 && {x[0] < 0}:hsb' a != 0." "solution: \\{") # hsb.cpp:38-43
add_repl_test(solver_cmd-hsb-ineq_system-two_vars
	"solve {x[0] < 0}:hsb a != 0 && {x[1] < 0}:hsb b != 0." "solution: \\{") # hsb.cpp:45-50
add_repl_test(solver_cmd-hsb-ineq_system-generic_complement
	"solve a : hsb != 0 && (a : hsb)' != 0." "solution: \\{") # hsb.cpp:52-57

# solve_system (:60-88) -- equality && inequalities in one solve call
add_repl_test(solver_cmd-hsb-solve_system-basic
	"solve {x[0] < 0}:hsb a = 0 && {x[0] < 0}:hsb' a != 0." "solution: \\{") # hsb.cpp:67-72
add_repl_test(solver_cmd-hsb-solve_system-shared_coeff
	"solve {x[0] < 0}:hsb a | {x[0] < 0}:hsb b = 0 && {x[1] < 0}:hsb b != 0." "solution: \\{") # hsb.cpp:75-80
add_repl_test(solver_cmd-hsb-solve_system-relational
	"solve a : hsb & (b : hsb)' = 0 && (a : hsb & (b : hsb)' | (a : hsb)' & b : hsb != 0)." "solution: \\{") # hsb.cpp:82-87

# solve (:90-128)
add_repl_test(solver_cmd-hsb-solve-neq0
	"solve a : hsb != 0." "solution: \\{") # hsb.cpp:105-107
add_repl_test(solver_cmd-hsb-solve-neq0-min
	"solve --min (a : hsb != 0)." "no solution") # hsb.cpp:108
add_repl_test(solver_cmd-hsb-solve-neq0-max
	"solve --max (a : hsb != 0)." "solution: \\{") # hsb.cpp:109
add_repl_test(solver_cmd-hsb-solve-neq0_neq1
	"solve a : hsb != 0 && a : hsb != 1." "solution: \\{") # hsb.cpp:112-114
add_repl_test(solver_cmd-hsb-solve-neq0_neq1-min
	"solve --min (a : hsb != 0 && a : hsb != 1)." "no solution") # hsb.cpp:115
add_repl_test(solver_cmd-hsb-solve-neq0_neq1-max
	"solve --max (a : hsb != 0 && a : hsb != 1)." "no solution") # hsb.cpp:116
add_repl_test(solver_cmd-hsb-solve-two_vars
	"solve {x[0] < 0}:hsb a != 0 && {x[1] < 0}:hsb b != 0." "solution: \\{") # hsb.cpp:119-121
add_repl_test(solver_cmd-hsb-solve-unsatisfiable
	"solve a : hsb = 0 && a : hsb != 0." "no solution") # hsb.cpp:124-126

# qint -- tests/integration/solver/test_integration-solver-qint.cpp

add_repl_test(solver_cmd-qint-find_solution-one_var
	"solve {[0, 1)}:qint x = 0." "solution: \\{") # qint.cpp:19-21
add_repl_test(solver_cmd-qint-find_solution-two_vars
	"solve {[0, 1)}:qint x | {[1, 2)}:qint y = 0." "solution: \\{") # qint.cpp:23-25
add_repl_test(solver_cmd-qint-find_solution-complement
	"solve {[0, 1)}:qint x | {[0, 1)}:qint' y = 0." "solution: \\{") # qint.cpp:27-29

add_repl_test(solver_cmd-qint-ineq_system-complement
	"solve {[0, 1)}:qint x != 0 && {[0, 1)}:qint' x != 0." "solution: \\{") # qint.cpp:38-43
add_repl_test(solver_cmd-qint-ineq_system-two_vars
	"solve {[0, 1)}:qint x != 0 && {[1, 2)}:qint y != 0." "solution: \\{") # qint.cpp:45-50
add_repl_test(solver_cmd-qint-ineq_system-generic_complement
	"solve x : qint != 0 && (x : qint)' != 0." "solution: \\{") # qint.cpp:52-57

add_repl_test(solver_cmd-qint-solve_system-basic
	"solve {[0, 1)}:qint x = 0 && {[0, 1)}:qint' x != 0." "solution: \\{") # qint.cpp:67-72
add_repl_test(solver_cmd-qint-solve_system-shared_coeff
	"solve {[0, 1)}:qint x | {[0, 1)}:qint y = 0 && {[1, 2)}:qint y != 0." "solution: \\{") # qint.cpp:74-79
add_repl_test(solver_cmd-qint-solve_system-relational
	"solve x : qint & (y : qint)' = 0 && (x : qint & (y : qint)' | (x : qint)' & y : qint != 0)." "solution: \\{") # qint.cpp:81-86

add_repl_test(solver_cmd-qint-solve-neq0
	"solve x : qint != 0." "solution: \\{") # qint.cpp:104-106
add_repl_test(solver_cmd-qint-solve-neq0-min
	"solve --min (x : qint != 0)." "no solution") # qint.cpp:107
add_repl_test(solver_cmd-qint-solve-neq0-max
	"solve --max (x : qint != 0)." "solution: \\{") # qint.cpp:108
add_repl_test(solver_cmd-qint-solve-neq0_neq1
	"solve x : qint != 0 && x : qint != 1." "solution: \\{") # qint.cpp:111-113
add_repl_test(solver_cmd-qint-solve-neq0_neq1-min
	"solve --min (x : qint != 0 && x : qint != 1)." "no solution") # qint.cpp:114
add_repl_test(solver_cmd-qint-solve-neq0_neq1-max
	"solve --max (x : qint != 0 && x : qint != 1)." "no solution") # qint.cpp:115
add_repl_test(solver_cmd-qint-solve-two_vars
	"solve {[0, 1)}:qint x != 0 && {[1, 2)}:qint y != 0." "solution: \\{") # qint.cpp:118-120
add_repl_test(solver_cmd-qint-solve-unsatisfiable
	"solve x : qint = 0 && x : qint != 0." "no solution") # qint.cpp:123-125

# qlt -- tests/integration/solver/test_integration-solver-qlt.cpp

add_repl_test(solver_cmd-qlt-find_solution-one_var
	"solve {(0, 1)}:qlt x = 0." "solution: \\{") # qlt.cpp:19-21
add_repl_test(solver_cmd-qlt-find_solution-two_vars
	"solve {(0, 1)}:qlt x | {[1, 2)}:qlt y = 0." "solution: \\{") # qlt.cpp:23-25
add_repl_test(solver_cmd-qlt-find_solution-complement
	"solve {(0, 1)}:qlt x | {(0, 1)}:qlt' y = 0." "solution: \\{") # qlt.cpp:27-29

add_repl_test(solver_cmd-qlt-ineq_system-complement
	"solve {(0, 1)}:qlt x != 0 && {(0, 1)}:qlt' x != 0." "solution: \\{") # qlt.cpp:38-43
add_repl_test(solver_cmd-qlt-ineq_system-two_vars
	"solve {(0, 1)}:qlt x != 0 && {[1, 2)}:qlt y != 0." "solution: \\{") # qlt.cpp:45-50
add_repl_test(solver_cmd-qlt-ineq_system-generic_complement
	"solve x : qlt != 0 && (x : qlt)' != 0." "solution: \\{") # qlt.cpp:52-57

add_repl_test(solver_cmd-qlt-solve_system-basic
	"solve {(0, 1)}:qlt x = 0 && {(0, 1)}:qlt' x != 0." "solution: \\{") # qlt.cpp:67-72
add_repl_test(solver_cmd-qlt-solve_system-shared_coeff
	"solve {(0, 1)}:qlt x | {(0, 1)}:qlt y = 0 && {[1, 2)}:qlt y != 0." "solution: \\{") # qlt.cpp:75-80
add_repl_test(solver_cmd-qlt-solve_system-relational
	"solve x : qlt & (y : qlt)' = 0 && (x : qlt & (y : qlt)' | (x : qlt)' & y : qlt != 0)." "solution: \\{") # qlt.cpp:82-87

add_repl_test(solver_cmd-qlt-solve-neq0
	"solve x : qlt != 0." "solution: \\{") # qlt.cpp:105-107
add_repl_test(solver_cmd-qlt-solve-neq0-min
	"solve --min (x : qlt != 0)." "no solution") # qlt.cpp:108
add_repl_test(solver_cmd-qlt-solve-neq0-max
	"solve --max (x : qlt != 0)." "solution: \\{") # qlt.cpp:109
add_repl_test(solver_cmd-qlt-solve-neq0_neq1
	"solve x : qlt != 0 && x : qlt != 1." "solution: \\{") # qlt.cpp:112-114
add_repl_test(solver_cmd-qlt-solve-neq0_neq1-min
	"solve --min (x : qlt != 0 && x : qlt != 1)." "no solution") # qlt.cpp:115
add_repl_test(solver_cmd-qlt-solve-neq0_neq1-max
	"solve --max (x : qlt != 0 && x : qlt != 1)." "no solution") # qlt.cpp:116
add_repl_test(solver_cmd-qlt-solve-two_vars
	"solve {(0, 1)}:qlt x != 0 && {[1, 2)}:qlt y != 0." "solution: \\{") # qlt.cpp:119-121
add_repl_test(solver_cmd-qlt-solve-unsatisfiable
	"solve x : qlt = 0 && x : qlt != 0." "no solution") # qlt.cpp:124-126

# Regression: <= normalizes to a negated ordering atom (bf_nlteq); the omcat
# gate must still route it to qlt's own solver instead of falling through to
# the ABA solve_inequality_system, which cannot handle ordering atoms and
# used to abort the process.
add_repl_test(solver_cmd-qlt-solve-interval_via_lteq
	"solve {0}:qlt <= x:qlt && x:qlt <= {1}:qlt." "solution: \\{") # qlt.cpp:130-133

# mixed -- tests/integration/solver/test_integration-solver-mixed.cpp

add_repl_test(solver_cmd-mixed-qint_qlt
	"solve {[0, 1)}:qint x != 0 && {(0, 1)}:qlt y != 0." "solution: \\{") # mixed.cpp:32-35
add_repl_test(solver_cmd-mixed-hsb_qint
	"solve {x[0] < 0}:hsb a != 0 && {[0, 1)}:qint b != 0." "solution: \\{") # mixed.cpp:37-40
add_repl_test(solver_cmd-mixed-qlt_hsb
	"solve {(0, 1)}:qlt x != 0 && {x[0] < 0}:hsb a != 0." "solution: \\{") # mixed.cpp:42-45
add_repl_test(solver_cmd-mixed-three_way
	"solve {[0, 1)}:qint x != 0 && {(0, 1)}:qlt y != 0 && {x[0] < 0}:hsb a != 0." "solution: \\{") # mixed.cpp:47-50
add_repl_test(solver_cmd-mixed-unsatisfiable
	"solve {[0, 1)}:qint x = 0 && {(0, 1)}:qlt y != 0 && {[0, 1)}:qint x != 0." "no solution") # mixed.cpp:52-55

# NOT migrated: mixed.cpp asserts semantics, so it cannot see a type LABEL.
add_repl_test(solver_cmd-mixed-per_binding_type
	"solve {(0, 1)}:qlt x != 0 && {x[0] < 0}:hsb a != 0." "a := \\{top\\}:hsb[^0-9A-Za-z]")

# sbf -- tests/integration/solver/test_integration-solver-sbf.cpp

# find_solution (:23-64)
add_repl_test(solver_cmd-sbf-find_solution-one_var
	"solve x = 0." "solution: \\{") # sbf.cpp:29-32
add_repl_test(solver_cmd-sbf-find_solution-or
	"solve x | y = 0." "solution: \\{") # sbf.cpp:34-37
add_repl_test(solver_cmd-sbf-find_solution-or_neg
	"solve x | y' = 0." "solution: \\{") # sbf.cpp:39-42
add_repl_test(solver_cmd-sbf-find_solution-xor
	"solve x ^ y = 1." "solution: \\{") # sbf.cpp:44-47
add_repl_test(solver_cmd-sbf-find_solution-and
	"solve x & y = 0." "solution: \\{") # sbf.cpp:49-52
add_repl_test(solver_cmd-sbf-find_solution-and_neg
	"solve x & y' = 0." "solution: \\{") # sbf.cpp:54-57
add_repl_test(solver_cmd-sbf-find_solution-labelled_vars
	"solve <:a> z ^ <:b> z' = 0." "solution: \\{") # sbf.cpp:60-63

# lgrs (:66-86) -- direct match to the `lgrs` REPL command
add_repl_test(lgrs_cmd-sbf-or
	"lgrs x | y = 0." "solution: \\{") # sbf.cpp:77-80
add_repl_test(lgrs_cmd-sbf-and
	"lgrs x & y = 0." "solution: \\{") # sbf.cpp:82-85

# solve_minterm_system (:88-120) -- conjoined with && into one solve call
add_repl_test(solver_cmd-sbf-minterm_system-disjoint_coeff
	"solve {a}:sbf x != 0 && {a}:sbf' x != 0." "solution: \\{") # sbf.cpp:109-113
add_repl_test(solver_cmd-sbf-minterm_system-two_vars
	"solve {a}:sbf x != 0 && {b}:sbf y != 0." "solution: \\{") # sbf.cpp:115-119

# solve_inequality_system (:122-188)
add_repl_test(solver_cmd-sbf-ineq_system-same_exponent
	"solve {a}:sbf x != 0 && {b}:sbf x != 0." "solution: \\{") # sbf.cpp:130-134
add_repl_test(solver_cmd-sbf-ineq_system-splitter_of_sbf
	"solve ({a}:sbf|{b}:sbf) x != 0 && {a}:sbf x' != 0." "solution: \\{") # sbf.cpp:139-144
add_repl_test(solver_cmd-sbf-ineq_system-splitter_of_a_sbf
	"solve {a}:sbf x != 0 && ({a}:sbf|{b}:sbf) x' != 0." "solution: \\{") # sbf.cpp:149-153
add_repl_test(solver_cmd-sbf-ineq_system-splitter_case4
	"solve x : sbf != 0 && (x : sbf)' != 0." "solution: \\{") # sbf.cpp:158-163
add_repl_test(solver_cmd-sbf-ineq_system-splitter_of_one
	"solve {a}:sbf&{b}:sbf x != 0 && ({a}:sbf|{b}:sbf) x' != 0." "solution: \\{") # sbf.cpp:169-173
add_repl_test(solver_cmd-sbf-ineq_system-splitter_case5
	"solve {a}:sbf x != 0 && {a}:sbf x' != 0." "solution: \\{") # sbf.cpp:176-180
add_repl_test(solver_cmd-sbf-ineq_system-monotonicity
	"solve x:sbf y' != 0 && (x : sbf)' != 0." "solution: \\{") # sbf.cpp:183-187

# solve_system (:190-288)
add_repl_test(solver_cmd-sbf-solve_system-one_var
	"solve {a}:sbf x = 0 && {a}:sbf x' != 0." "solution: \\{") # sbf.cpp:197-202
add_repl_test(solver_cmd-sbf-solve_system-two_var
	"solve {a}:sbf x y = 0 && {a}:sbf y != 0." "solution: \\{") # sbf.cpp:204-209
add_repl_test(solver_cmd-sbf-solve_system-two_var_or
	"solve {a}:sbf x | {a}:sbf y = 0 && {b}:sbf y x != 0." "solution: \\{") # sbf.cpp:211-220
add_repl_test(solver_cmd-sbf-solve_system-x_lt_y
	"solve x:sbf & y' = 0 && (x' & y:sbf | x & y' != 0)." "solution: \\{") # sbf.cpp:222-227
add_repl_test(solver_cmd-sbf-solve_system-y_eq_a_x_lt_y
	"solve y' & {a}:sbf | y & {a}:sbf' | x y' = 0 && (x' & y | x & y' != 0)." "solution: \\{") # sbf.cpp:229-234
add_repl_test(solver_cmd-sbf-solve_system-y_lt_x_z_lt_y
	"solve y:sbf & x' | z & y' = 0 && (y:sbf & x' | x & y' != 0) && (y:sbf & z' | z & y' != 0)." "solution: \\{") # sbf.cpp:236-242
add_repl_test(solver_cmd-sbf-solve_system-chain
	"solve x'y:sbf | y'z | wz' = 0 && ( w'z|wz' != 0:sbf) && (xy' != 0:sbf) && (yz' != 0:sbf)." "solution: \\{") # sbf.cpp:244-249
add_repl_test(solver_cmd-sbf-solve_system-monotonicity3
	"solve x:sbf | z y' = 0 && (y:sbf z' | y' z != 0) && (y' != 0:sbf)." "solution: \\{") # sbf.cpp:274-279
add_repl_test(solver_cmd-sbf-solve_system-monotonicity3_y1
	"solve x:sbf | x y' = 0 && (y x' | y'x != 0:sbf) && (y' != 0:sbf)." "solution: \\{") # sbf.cpp:282-287

# solve (:290-441)
add_repl_test(solver_cmd-sbf-solve-monotonicity1
	"solve o1[t] > {<:split1> != 0} && o1[t] != 1." "solution: \\{") # sbf.cpp:345-348
add_repl_test(solver_cmd-sbf-solve-y_lt_x_y_eq_a
	"solve y < x && y = {a}:sbf && x' != 0." "solution: \\{") # sbf.cpp:350-353
add_repl_test(solver_cmd-sbf-solve-monotonicity2
	"solve x = {a}:sbf && x < y && y != 1." "solution: \\{") # sbf.cpp:359-362
add_repl_test(solver_cmd-sbf-solve-monotonicity2_y2
	"solve x = {a}:sbf {b}:sbf && x < y && y != 1." "solution: \\{") # sbf.cpp:365-368
add_repl_test(solver_cmd-sbf-solve-neq0
	"solve x != 0." "solution: \\{") # sbf.cpp:370-373
add_repl_test(solver_cmd-sbf-solve-neq0-min
	"solve --min (x != 0)." "no solution") # sbf.cpp:373
add_repl_test(solver_cmd-sbf-solve-neq0-max
	"solve --max (x != 0)." "solution: \\{") # sbf.cpp:374
add_repl_test(solver_cmd-sbf-solve-neq0_neq1
	"solve x != 0 && x != 1." "solution: \\{") # sbf.cpp:377-380
add_repl_test(solver_cmd-sbf-solve-neq0_neq1-min
	"solve --min (x != 0 && x != 1)." "no solution") # sbf.cpp:380
add_repl_test(solver_cmd-sbf-solve-neq0_neq1-max
	"solve --max (x != 0 && x != 1)." "no solution") # sbf.cpp:381
add_repl_test(solver_cmd-sbf-solve-x_neq0_y_neq0
	"solve x != 0 && y != 0." "solution: \\{") # sbf.cpp:384-387
add_repl_test(solver_cmd-sbf-solve-x_neq0_y_neq0-min
	"solve --min (x != 0 && y != 0)." "no solution") # sbf.cpp:387
add_repl_test(solver_cmd-sbf-solve-x_neq0_y_neq0-max
	"solve --max (x != 0 && y != 0)." "solution: \\{") # sbf.cpp:388
add_repl_test(solver_cmd-sbf-solve-lucca_interpreter
	"solve {<:z> = 0 && <:w> = 0} = o1[t] && o2[2] = { <:x> = 0 } && o3[1] = { <:y> = 0 }." "solution: \\{") # sbf.cpp:392-395
add_repl_test(solver_cmd-sbf-solve-lucca_interpreter-min
	"solve --min ({<:z> = 0 && <:w> = 0} = o1[t] && o2[2] = { <:x> = 0 } && o3[1] = { <:y> = 0 })." "solution: \\{") # sbf.cpp:395
add_repl_test(solver_cmd-sbf-solve-lucca_interpreter-max
	"solve --max ({<:z> = 0 && <:w> = 0} = o1[t] && o2[2] = { <:x> = 0 } && o3[1] = { <:y> = 0 })." "solution: \\{") # sbf.cpp:396
add_repl_test(solver_cmd-sbf-solve-lucca_interpreter_sbf
	"solve {a & b}:sbf = o1[t] && o2[2] = { c }:sbf && o3[1] = { d }:sbf." "solution: \\{") # sbf.cpp:400-403
add_repl_test(solver_cmd-sbf-solve-lucca_interpreter_sbf-min
	"solve --min ({a & b}:sbf = o1[t] && o2[2] = { c }:sbf && o3[1] = { d }:sbf)." "solution: \\{") # sbf.cpp:403
add_repl_test(solver_cmd-sbf-solve-lucca_interpreter_sbf-max
	"solve --max ({a & b}:sbf = o1[t] && o2[2] = { c }:sbf && o3[1] = { d }:sbf)." "solution: \\{") # sbf.cpp:404
add_repl_test(solver_cmd-sbf-solve-x_eq_a_y_eq_b
	"solve x = {a}:sbf && y = {b}:sbf." "solution: \\{") # sbf.cpp:408-411
add_repl_test(solver_cmd-sbf-solve-x_eq_a_y_eq_b-min
	"solve --min (x = {a}:sbf && y = {b}:sbf)." "solution: \\{") # sbf.cpp:411
add_repl_test(solver_cmd-sbf-solve-x_eq_a_y_eq_b-max
	"solve --max (x = {a}:sbf && y = {b}:sbf)." "solution: \\{") # sbf.cpp:412
add_repl_test(solver_cmd-sbf-solve-bv_single_var_eq
	"solve x:bv[8] = {0}:bv[8]." "solution: \\{") # sbf.cpp:416-419
add_repl_test(solver_cmd-sbf-solve-bv_single_var_eq-min
	"solve --min (x:bv[8] = {0}:bv[8])." "solution: \\{") # sbf.cpp:419
add_repl_test(solver_cmd-sbf-solve-bv_single_var_eq-max
	"solve --max (x:bv[8] = {0}:bv[8])." "solution: \\{") # sbf.cpp:420
add_repl_test(solver_cmd-sbf-solve-bv_two_var_eq
	"solve x:bv[8] = y:bv[8]." "solution: \\{") # sbf.cpp:423-426
add_repl_test(solver_cmd-sbf-solve-bv_two_var_eq-min
	"solve --min (x:bv[8] = y:bv[8])." "solution: \\{") # sbf.cpp:426
add_repl_test(solver_cmd-sbf-solve-bv_two_var_eq-max
	"solve --max (x:bv[8] = y:bv[8])." "solution: \\{") # sbf.cpp:427
add_repl_test(solver_cmd-sbf-solve-bv_conjunction
	"solve x:bv[8] = {0}:bv[8] && y:bv[8] = {0}:bv[8]." "solution: \\{") # sbf.cpp:430-433
add_repl_test(solver_cmd-sbf-solve-bv_conjunction-min
	"solve --min (x:bv[8] = {0}:bv[8] && y:bv[8] = {0}:bv[8])." "solution: \\{") # sbf.cpp:433
add_repl_test(solver_cmd-sbf-solve-bv_conjunction-max
	"solve --max (x:bv[8] = {0}:bv[8] && y:bv[8] = {0}:bv[8])." "solution: \\{") # sbf.cpp:434
add_repl_test(solver_cmd-sbf-solve-bv_unsatisfiable
	"solve x:bv[8] = {0}:bv[8] && x:bv[8] = {1}:bv[8]." "no solution") # sbf.cpp:437-439

# ---------------------------------------------------------------------------
