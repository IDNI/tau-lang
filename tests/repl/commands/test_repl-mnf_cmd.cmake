#
# mnf command
#

#
# wff
#

# TODO (HIGH) fixme it's not working properly, returns Y & X |Y & X | Z & X
add_repl_test(normal_forms-mnf_wff "mnf T" ": T")
add_repl_test(normal_forms-mnf_wff_mem_abs "T. mnf %1" "%1[^:]*: T.*%[0-9]+[^:]*: T")
add_repl_test(normal_forms-mnf_wff_mem_rel "T. mnf %-0" "%1[^:]*: T.*%[0-9]+[^:]*: T")

#
# bf
#

add_repl_test(normal_forms-mnf_bf "mnf 1" ": 1")
add_repl_test(normal_forms-mnf_bf_mem_abs "1. mnf %1" "%1[^:]*: 1.*%[0-9]+[^:]*: 1")
add_repl_test(normal_forms-mnf_bf_mem_rel "1. mnf %-0" "%1[^:]*: 1.*%[0-9]+[^:]*: 1")
