#
# enable and disable commands
#

include(add_repl_test)

# enable (default state is on, so enable → already on → "on")
add_repl_test(enable_cmd-status       "enable status"        "status: *true")
add_repl_test(enable_cmd-color       "enable color"       "color: *true")
add_repl_test(enable_cmd-highlighting "enable highlighting"  "highlighting: *true")
add_repl_test(enable_cmd-indenting    "enable indenting"     "indenting: *true")
add_repl_test(enable_cmd-benchmarks   "enable benchmarks"    "benchmarks: *true")
add_repl_test(enable_cmd-charvar      "enable charvar"       "charvar: *true")

# disable (flips default on → off)
add_repl_test(disable_cmd-status       "disable status"        "status: *false")
add_repl_test(disable_cmd-color       "disable color"       "color: *false")
add_repl_test(disable_cmd-highlighting "disable highlighting"  "highlighting: *false")
add_repl_test(disable_cmd-indenting    "disable indenting"     "indenting: *false")
add_repl_test(disable_cmd-benchmarks   "disable benchmarks"    "benchmarks: *false")
add_repl_test(disable_cmd-charvar      "disable charvar"       "charvar: *false")

# disable then enable round-trip
add_repl_test(enable_disable_roundtrip "disable status. enable status"  "status: *true")
add_repl_test(disable_enable_roundtrip "enable status. disable status"  "status: *false")

# severity takes a word: the bool-only commands refuse it
add_repl_test(enable_cmd-invalid_severity "enable severity" "takes a value, not a flag" NO_FAIL_REGEX)
add_repl_test(disable_cmd-invalid_severity "disable severity" "takes a value, not a flag" NO_FAIL_REGEX)

add_repl_test(enable_disable_cmd-preprocessing
	"disable preprocessing. get preprocessing. enable preprocessing. get preprocessing"
	"preprocessing: *false.*preprocessing: *true")
add_repl_test(enable_disable_cmd-ba_component_factoring
	"disable ba-component-factoring. get ba-component-factoring. enable ba-component-factoring. get ba-component-factoring"
	"ba-component-factoring: *false.*ba-component-factoring: *true")
