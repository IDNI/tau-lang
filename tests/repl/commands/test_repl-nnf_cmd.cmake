#
# order normal form tests
#

#
# wff
#

add_repl_test(normal_forms-nnf_wff "nnf T" ": T")
add_repl_test(normal_forms-nnf_wff_mem_abs "T. nnf %1" "T")
add_repl_test(normal_forms-nnf_wff_mem_rel "T. nnf %-0" "T")

#
# bf
#

add_repl_test(normal_forms-nnf_bf "nnf 1" ": 1")
add_repl_test(normal_forms-nnf_bf_mem_abs "1. nnf %1" "1")
add_repl_test(normal_forms-nnf_bf_mem_rel "1. nnf %-0" "1")
