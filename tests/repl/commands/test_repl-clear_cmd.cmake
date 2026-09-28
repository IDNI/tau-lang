#
# clear command
#
# Coverage-driven addition (2026-08-01). The include existed but was commented
# out. `clear` emits terminal control sequences rather than text, so the
# assertion is simply that the REPL survives it and goes on to process the next
# command -- which is what the eval_cmd clear arm has to guarantee.
#

include(add_repl_test)

add_multiline_repl_test(clear_cmd
	"Tau Language Framework"
	STDIN "clear\\nversion\\nq\\n")
