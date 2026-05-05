#
# whatis command
#

include(add_repl_test)

# WFF expressions
add_repl_test(whatis_cmd-wff_T   "whatis T"       "wff")
add_repl_test(whatis_cmd-wff_F   "whatis F"       "wff")
add_repl_test(whatis_cmd-wff_eq  "whatis X = 0"   "wff")

# BF expressions
add_repl_test(whatis_cmd-bf_1  "whatis 1"     "bf")
add_repl_test(whatis_cmd-bf_0  "whatis 0"     "bf")
add_repl_test(whatis_cmd-bf_xr "whatis X & Y" "bf")

# history references
add_repl_test(whatis_cmd-mem_wff  "T.      whatis %"  "wff")
add_repl_test(whatis_cmd-mem_bf   "1.      whatis %"  "bf")
add_repl_test(whatis_cmd-mem_abs  "X = 0.  whatis %1" "wff")
