#
# mnf command
#

#
# wff
#

add_repl_test(normal_forms-mnf_wff "mnf T" ": T")
# a repeated disjunct, with its conjuncts in either order, is kept once
add_repl_test(normal_forms-mnf_wff_repeated_disjunct
	"mnf x = 0 && y = 0 || y = 0 && x = 0 || x = 0 && z = 0"
	": [xyz] = 0 && [xyz] = 0 [|][|] [xyz] = 0 && [xyz] = 0[^|]*$")
add_repl_test(normal_forms-mnf_wff_distributed
	"mnf x = 0 && (y = 0 || z = 0)"
	": [xyz] = 0 && [xyz] = 0 [|][|] [xyz] = 0 && [xyz] = 0[^|]*$")
add_repl_test(normal_forms-mnf_wff_mem_abs "T. mnf %1" "%1[^:]*: T.*%[0-9]+[^:]*: T")
add_repl_test(normal_forms-mnf_wff_mem_rel "T. mnf %-0" "%1[^:]*: T.*%[0-9]+[^:]*: T")

#
# bf
#

add_repl_test(normal_forms-mnf_bf "mnf 1" ": 1")
add_repl_test(normal_forms-mnf_bf_repeated_disjunct
	"mnf y & x | y & x | z & x" ": (xy[|]xz|xz[|]xy)[^|]*$")
add_repl_test(normal_forms-mnf_bf_in_equation
	"mnf (y & x | y & x | z & x) = 0" ": (xy[|]xz|xz[|]xy) = 0[^|]*$")
add_repl_test(normal_forms-mnf_bf_mem_abs "1. mnf %1" "%1[^:]*: 1.*%[0-9]+[^:]*: 1")
add_repl_test(normal_forms-mnf_bf_mem_rel "1. mnf %-0" "%1[^:]*: 1.*%[0-9]+[^:]*: 1")
