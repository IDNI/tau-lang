#
# enable and disable commands
#

include(add_repl_test)

# enable (default state is on, so enable → already on → "on")
add_repl_test(enable_cmd-status       "enable status"       "on")
add_repl_test(enable_cmd-colors       "enable colors"       "on")
add_repl_test(enable_cmd-highlighting "enable highlighting" "on")
add_repl_test(enable_cmd-indenting    "enable indenting"    "on")
add_repl_test(enable_cmd-benchmarks   "enable benchmarks"   "on")
add_repl_test(enable_cmd-charvar      "enable charvar"      "on")

# disable (flips default on → off)
add_repl_test(disable_cmd-status       "disable status"       "off")
add_repl_test(disable_cmd-colors       "disable colors"       "off")
add_repl_test(disable_cmd-highlighting "disable highlighting" "off")
add_repl_test(disable_cmd-indenting    "disable indenting"    "off")
add_repl_test(disable_cmd-benchmarks   "disable benchmarks"   "off")
add_repl_test(disable_cmd-charvar      "disable charvar"      "off")

# disable then enable round-trip
add_repl_test(enable_disable_roundtrip "disable status. enable status" "on")
add_repl_test(disable_enable_roundtrip "enable status. disable status" "off")

# invalid option for bool-only commands
add_repl_test_fail(enable_cmd-invalid_severity "enable severity" "Invalid option")
add_repl_test_fail(disable_cmd-invalid_severity "disable severity" "Invalid option")
