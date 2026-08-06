#
# ADT type definitions (repl_evaluator::def_type_cmd) and their effect on
# normalize/sat/unsat/solve/run, once a `type` statement has been accepted
# by the REPL.
#
# `tau -e` is single-valued (only the last -e survives), so every non-`run`
# case below chains its commands with '.' inside ONE -e string -- exactly
# how test_repl-normalize_cmd.cmake/test_repl-sat_cmd.cmake/
# test_repl-solver_cmd.cmake chain a definition and its use. This also keeps
# the `type Point = ...` and the command that references `Point` in the SAME
# parse, which is what actually lets the ADT flattener (src/adt/
# adt_flatten.tmpl.h) see the type: it builds its registry fresh from
# whatever tree it is handed, so a type declared on an earlier, separately
# parsed REPL line is not visible to a later command's parse (each `-X`
# stdin line is its own parse). def_type_cmd's own `type_defs` storage
# (repl_evaluator.h/.tmpl.h) mirrors rr_defs/io_defs for get_applied()'s
# spec assembly, but that assembly runs on already-parsed/flattened trees,
# so it does not retroactively fix a later line's parse either -- hence
# every case here keeps the definition and its use on one line.
#
# Member-path/annotation syntax notes (verified against build-Debug/tau
# before being written here):
# - `ex x:Point (x.a = 0)` is the valid form for "annotate the quantified
#   variable, then use a member of it" -- WITHOUT the parens the quantifier
#   body swallows differently and the flattener rejects the bare tuple-typed
#   `x` ("used outside an equality or quantifier context").
# - a variable's ADT type can also be fixed outside any quantifier by
#   comparing an annotated occurrence against the plain one in an equality,
#   e.g. `x:Point = x`, which is what the solve case below uses to keep `x`
#   free (unlike `ex x:Point ...`, which would bind it).
#
include(add_repl_test)

# type_def is accepted and echoed back with its position, mirroring how
# def_input_cmd/def_output_cmd echo "[<n>] ...". Round 1 flagged this as
# untested and the echo turned out to print "[1] Pointa:sbfb:sbf" -- no
# regression here, `to_str()`/tau_tree_printers.tmpl.h simply never had a
# case for type_def/type_parents/type_body/tuple (every other tree printed
# anywhere in the codebase has these erased by adt_flatten before printing;
# def_type_cmd's own echo is the first path that prints one pre-erasure).
# Fixed with on_enter/on_between/on_leave cases for those four node kinds
# ("type ", " of (" + ", "-separated parents + ")", " = ", "{" + ", "-
# separated members + "}"); member_name/typed need no new case, they already
# print "a" then ":sbf" back to back. Renders to "type Point = {a:sbf,
# b:sbf}" (colon, no space -- matching `typed`'s existing print convention
# used everywhere else, e.g. `solve x:sbf = 1`, not a space-separated
# "a: sbf"). Re-parses per parser/tau.tgf: `typed => _ ':' _ type ...` and
# `tuple => ... (_ ',' _ member)* ...` both allow zero surrounding
# whitespace, so this is valid input either way; verified by hand-tracing
# the grammar (no rebuild available yet -- planned manual round-trip check
# is `type Point = {a:sbf, b:sbf}` fed back into `./build-Debug/tau -e` once
# rebuilt).
add_repl_test(adt-type_def
	"type Point = {a: sbf, b: sbf}"
	"\\[1\\] type Point")

# normalize over an ADT-typed existential quantifier.
add_repl_test(adt-normalize
	"type Point = {a: sbf, b: sbf}. n ex x:Point (x = 0)"
	": T")

# sat: a Point can be made non-zero.
add_repl_test(adt-sat
	"type Point = {a: sbf, b: sbf}. sat ex x:Point (x != 0)"
	": T")

# unsat: every Point is unequal to 0 is unsatisfiable (x = 0 is reachable).
add_repl_test(adt-unsat
	"type Point = {a: sbf, b: sbf}. unsat all x:Point (x != 0)"
	": T")

# solve with an ADT-typed free variable: the solution is reported per
# flattened component (x.a, x.b), not as a single tuple value.
add_repl_test(adt-solve
	"type Point = {a: sbf, b: sbf}. solve x:Point = x && x.a = 1 && x.b = 0"
	"x\\.a := \\{ 1 \\}:sbf")

# unknown member access on a registered ADT type is rejected.
add_repl_test_fail(adt-unknown_member
	"type Point = {a: sbf, b: sbf}. n ex x:Point (x.c = 0)"
	"ADT")

# --- run over ADT-typed console streams -------------------------------------
# Declaring an ADT-typed input/output stream through the REPL's def_input_cmd/
# def_output_cmd (as opposed to a `definitions`/spec-file-level input_def)
# used to NOT reach adt_flatten_rewrite_io_def (src/adt/adt_flatten.tmpl.h),
# which is what groups a tuple's members behind ONE physical stream/prompt
# (design doc sec. 4, Task 8): adt_flatten_rewrite's generic dispatch only
# special-cased an input_def/output_def reached through `definitions`/
# `spec_multiline` (the spec-file grammar), never one reached through a
# def_input_cmd/def_output_cmd (the REPL's `cli` grammar) -- so a REPL-
# declared tuple stream's members never got registered into ctx->adt_streams,
# and `run` prompted for `i.a`/`i.b` as two independent plain-sbf streams
# instead of one combined tuple-wire prompt. Fixed by adding a
# `case tau::def_input_cmd: case tau::def_output_cmd:` to
# adt_flatten_rewrite's switch that also calls adt_flatten_rewrite_io_def
# for its ctx-mutating registration side effect, while (unlike the
# definitions/spec_multiline case) keeping the def's own child intact
# afterward -- def_input_cmd()/def_output_cmd() (repl_evaluator.tmpl.h)
# still need it to store/echo. See task-9-report.md for the full root-cause
# trace and the fix's diff. UNVERIFIED AGAINST A REBUILT BINARY (this test's
# expectation follows directly from continue_running's/adt_tuple_writer's
# documented grouped-prompt format -- lbl << root_name << "[" << tp << "]
# := " << adt_wire_hint(...) for the input prompt,
# console_prompt_output_stream::put(value, tp) printing "<root>[<tp>] :=
# <value>" for the grouped output -- not from an actual run of this fix).
add_test(NAME "test_repl-adt-run"
	COMMAND bash -c "printf 'type Point = {a: sbf, b: sbf}. i:Point := in console. o:Point := out console. run o[t] = i[t].\\n{ a: \"1\", b: \"0\" }\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-adt-run" PROPERTIES
	PASS_REGULAR_EXPRESSION "o\\[0\\] := \\{ a: \"1\", b: \"0\" \\}")
