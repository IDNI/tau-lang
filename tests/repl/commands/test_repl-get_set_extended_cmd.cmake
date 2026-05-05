#
# get and set — extended coverage
#

include(add_repl_test)

# get individual options
add_repl_test(get_cmd-severity      "get severity"      "severity:")
add_repl_test(get_cmd-charvar       "get charvar"       "charvar:")
add_repl_test(get_cmd-highlighting  "get highlighting"  "highlighting:")
add_repl_test(get_cmd-indenting     "get indenting"     "indenting:")
add_repl_test(get_cmd-benchmarks    "get benchmarks"    "benchmarks:")

# set severity
add_repl_test(set_cmd-severity_info  "set severity info"  "info")
add_repl_test(set_cmd-severity_error "set severity error" "error")

# set charvar
add_repl_test(set_cmd-charvar_on  "set charvar on"  "on")
add_repl_test(set_cmd-charvar_off "set charvar off" "off")

# set highlighting
add_repl_test(set_cmd-highlighting_on  "set highlighting on"  "on")
add_repl_test(set_cmd-highlighting_off "set highlighting off" "off")

# set indenting
add_repl_test(set_cmd-indenting_on  "set indenting on"  "on")
add_repl_test(set_cmd-indenting_off "set indenting off" "off")

# set benchmarks
add_repl_test(set_cmd-benchmarks_on  "set benchmarks on"  "on")
add_repl_test(set_cmd-benchmarks_off "set benchmarks off" "off")

# invalid severity value
add_repl_test_fail(set_cmd-severity_invalid "set severity foobar" "Invalid severity value")

# get with equals-sign syntax
add_repl_test(set_cmd-status_equals "set status = off" "off")
