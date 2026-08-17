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
# still need it to store/echo.
#
# Round 4 review caught that the fix above was incomplete: that SAME kept-
# intact, still-unflattened io_defs entry is re-spliced into every later
# get_applied() call (normalize/sat/solve/run all go through it), which
# used to fabricate a SECOND, un-grouped "bare root" stream registration in
# ctx alongside the correct per-member one -- rebuild_inputs then also tried
# to read through that stray registration, producing a live
# "(Error) Failed to read from input stream 'i.a'" during THIS test, which
# passed anyway because a raw add_test with only PASS_REGULAR_EXPRESSION
# (no FAIL_REGULAR_EXPRESSION) can't catch it. Fixed in get_applied()
# (repl_evaluator.tmpl.h): an io_defs entry whose root is already present in
# ctx->adt_streams (i.e. a tuple-typed def, already fully registered by its
# own original parse) is now skipped rather than re-spliced -- see that
# function's own comment for why this loses nothing (an ADT-typed def's
# consumer is always in the SAME parse as the def itself, per the
# cross-line-visibility finding above, so there is no later-line use case
# left needing the splice, unlike an ordinary cross-line io def).
#
# FAIL_REGULAR_EXPRESSION "Error" added below per that review, matching the
# rest of this file's add_repl_test-based cases (which get it for free) --
# this is the one raw add_test in this file and the only one that lacked it.
# The old "T./F.q.q" step-loop termination style used by
# test_repl-run_cmd.cmake legitimately produces a "Failed to parse input
# value" error as part of ending an "always" (needs-input-every-step) run,
# which would fail this stricter check -- so the spec below is deliberately
# a single-time-point run (`o[0] = i[0]`, like
# test_repl-run_cmd-continue_or_quit_prompt/quit_finishes_run), which stops
# needing input after step 0 and reaches the continue-or-quit gate; a
# literal "q" there quits cleanly via finish_running(), with no error at
# any point in the transcript.
#
# Round 5: FAIL_REGULAR_EXPRESSION "Error" (just above) caught a second,
# unrelated defect once round 4's fix was actually rebuilt: the very FIRST
# read attempt on the "i" prompt logged "(Error) Failed to read from input
# stream 'i.a'" before the prompt was even shown, because adt_tuple_reader's
# leaf() (src/io_context.h/.tmpl.h) turned the physical stream's ordinary
# "no value yet" empty read into nullopt, which interpreter::read()
# (interpreter.tmpl.h) treats as a hard failure -- unlike a plain stream,
# whose own get() already returns a PRESENT-but-empty string for the same
# situation, hitting read()'s quiet end-of-input path instead. Fixed by
# making leaf() propagate an empty physical read as an empty string (not
# nullopt); malformed non-empty literals still hard-error as before. See
# tests/unit/test_io_context.cpp's new "reader propagates an empty physical
# read..."/"an empty read is not memoized as a failure" cases and
# task-9-report.md for the fix.
# UNVERIFIED AGAINST A REBUILT BINARY (the get_applied fix, the
# adt_tuple_reader empty-propagation fix, and this test's expectations are
# all read from the relevant code paths, not from an actual run --
# see task-9-report.md).
add_test(NAME "test_repl-adt-run"
	COMMAND bash -c "printf 'type Point = {a: sbf, b: sbf}. i:Point := in console. o:Point := out console. run o[0] = i[0].\\n{ a: \"1\", b: \"0\" }\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-adt-run" PROPERTIES
	PASS_REGULAR_EXPRESSION "o\\[0\\] := \\{ a: \"1\", b: \"0\" \\}"
	FAIL_REGULAR_EXPRESSION "Error")

# Round 6: High review finding, reproduced live -- adt_tuple_reader's
# read_time_point (src/io_context.h/.tmpl.h) used to latch a malformed-line
# failure permanently for a time point: once one bad value was submitted,
# EVERY later call short-circuited to the memoized failure WITHOUT ever
# consulting the physical stream again, so a textually-valid correction
# typed right after was silently discarded and the stream stayed stuck
# ("Failed to read from input stream 'i.a'" forever, no q-escape before the
# continue gate). Fixed to keep re-consulting the physical stream on retry,
# comparing the RAW line against the one the memoized failure came from
# (see tests/unit/test_io_context.cpp's new "a corrected line after a
# failed one..."/"resubmitting the exact same malformed line..." cases and
# task-9-report.md for the fix). This is the REPL-level regression test,
# modeled on test_repl-run_cmd-retry_on_bad_value (repl_evaluator.tmpl.h's
# own retry mechanism: continue_running re-enters the SAME time point when
# a submitted value doesn't parse) -- a malformed wire literal is submitted
# first (expected to log a "(Error) ADT wire: ..." parse error, same as
# retry_on_bad_value's own "Failed to parse input value" expectation), so
# this is the raw add_test form with a PASS regex pinning the CORRECTED o[0]
# output rather than add_repl_test/FAIL_REGULAR_EXPRESSION "Error" (which
# would wrongly fail on that expected mid-transcript error).
add_test(NAME "test_repl-adt-run_retry_on_bad_tuple_value"
	COMMAND bash -c "printf 'type Point = {a: sbf, b: sbf}. i:Point := in console. o:Point := out console. run o[0] = i[0].\\nnot a tuple literal\\n{ a: \"1\", b: \"0\" }\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-adt-run_retry_on_bad_tuple_value" PROPERTIES
	PASS_REGULAR_EXPRESSION "o\\[0\\] := \\{ a: \"1\", b: \"0\" \\}")

# Round 7: Critical demo-extension finding (private/2026-08-06-adt-demo-
# extension-report.md, "Defect B") -- ANY get_applied()-driven REPL command
# after an ADT-typed `run` in the same session used to crash: SIGABRT
# (Debug, `tau_spec.tmpl.h:142`'s `DBG(assert(false))`) / SIGSEGV (Release).
# Root cause (gdb-confirmed against build-Debug/tau, this file's own repro
# below): repl_evaluator's `type_defs`/`rr_defs`/`io_defs` (repl_evaluator.h)
# used to store plain `tref` -- a raw, non-owning pointer into
# bintree<node>'s node storage -- and none of the three are walked by any
# `collect_live_refs` implementation `interpreter::maybe_gc()` consults
# (interpreter.tmpl.h), so a stored entry is unprotected from
# `bintree<node>::gc()`, which `maybe_gc()` runs once accumulated tree size
# crosses a threshold -- something only a `run`'s per-step tree churn
# realistically reaches (normalize/sat/solve never step the interpreter at
# all). Confirmed live: after a sweep, `type_defs[0]` pointed at memory
# reused for an unrelated, tiny leaf node (gdb: the freed slot's node type
# read back as `tau_parser_nonterminals::eof`, not `type_def` -- a classic
# dangling-pointer/memory-reuse signature, not a logic bug in `tau_spec::
# add`'s dispatch itself), so the NEXT get_applied() call (any later
# normalize/sat/solve/run) iterated a corrupted entry and hit the
# `default:` "unknown node" branch. Fixed by storing `htref` (an owning
# `std::shared_ptr<htree>`) instead of `tref` in all three vectors --
# exactly how `history`/`H` (repl_evaluator.h) already protects itself the
# same way, and consistent with every comment in interpreter.tmpl.h
# explaining that a live htref keeps its node reachable through
# `bintree<node>::gc()` via M's own weak_ptr bookkeeping. `rr_defs`/
# `io_defs` (not just `type_defs`) had the identical latent hazard --
# fixed identically, not just the vector this specific demo repro happened
# to hit first (type_defs is iterated first in get_applied(), so it always
# surfaces before rr_defs/io_defs would even be reached).
#
# This is the requested regression test: `run` over an ADT stream, THEN a
# plain `normalize`, THEN a second `run` over a (differently-named) ADT
# stream, all in one session -- mirrors the report's exact crashing repro,
# extended with the second `run` the coordinator asked for. Uses the raw
# add_test form (interactive -X, like the other `run`-driving cases in this
# file) but WITH FAIL_REGULAR_EXPRESSION "Error" (error-intolerant: nothing
# here is expected to fail) -- only the final PASS_REGULAR_EXPRESSION (the
# SECOND run's grouped output) is asserted, since reaching it at all proves
# every earlier step (including the crash-prone normalize right after the
# first run) already completed without crashing or being cut short.
# Coverage round 2026-08-17 (test-coverage plan, Task 7) --------------------

# R1: the tuple console prompt is labeled with the ROOT name and a
# wire-shaped hint (continue_running's ADT branch: find_adt_stream_for_member
# + adt_wire_hint, repl_evaluator.tmpl.h/io_context.tmpl.h) -- the only
# automated assertion on that code path. Transcript verified live 2026-08-17:
# the prompt renders as `i[0] := { a: "", b: "" } `.
add_test(NAME "test_repl-adt-run_prompt_hint"
	COMMAND bash -c "printf 'type Point = {a: sbf, b: sbf}. i:Point := in console. o:Point := out console. run o[0] = i[0].\\n{ a: \"1\", b: \"0\" }\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-adt-run_prompt_hint" PROPERTIES
	PASS_REGULAR_EXPRESSION "i\\[0\\] := \\{ a: \"\", b: \"\" \\}"
	FAIL_REGULAR_EXPRESSION "Error")

# R2: an alias-typed io def stays ONE plain stream through the REPL: the
# def's typed rewrites to bv[8], run prompts per-value (`i[0] : bv[8] :=`,
# no tuple hint) and the output prints as a single bv value (canonical
# decimal), all verified live 2026-08-17.
add_test(NAME "test_repl-adt-run_alias_stream"
	COMMAND bash -c "printf 'type byte = bv[8]. i:byte := in console. o:byte := out console. run o[0] = i[0].\\n#b00000001\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-adt-run_alias_stream" PROPERTIES
	PASS_REGULAR_EXPRESSION "o\\[0\\] := 1"
	FAIL_REGULAR_EXPRESSION "Error")

# R3: type_def echo with parents exercises the printer's type_parents
# on_enter/on_between/on_leave (" of (" + ", "-separated + ")") -- canonical
# "of"/"=" spelling regardless of the "is" the input used.
add_repl_test(adt-type_def_echo_parents
	"type Tagged = {tag: bv[8]}. type Line of (Tagged) is {p: bv[8]}"
	"\\[2\\] type Line of \\(Tagged\\) = \\{p:bv\\[8\\]\\}")

# R4: alias type_def echo exercises the printer's alias type_body (" = "
# followed by type + subtype brackets).
add_repl_test(adt-type_def_echo_alias
	"type byte = bv[8]"
	"\\[1\\] type byte = bv\\[8\\]")

# R5: member access on an indexed stream variable in a non-run command --
# must be parenthesized (the documented REPL quirk: without parens the REPL
# splits the command at the period after "]"). Pins the WORKING form; the
# echo renders the flattened member as `always i[t].a:sbf = 0`.
add_repl_test(adt-normalize_io_member
	"type Point = {a: sbf, b: sbf}. i:Point := in console. n always (i[t].a = 0)"
	"always i\\[t\\]\\.a")

# R6: solve reports only the constrained member; an unconstrained tuple
# member is OMITTED from the solution rather than defaulted (by design --
# contrast with the interpreter's partial-copy defaulting, which emits the
# BA's 0). Verified live 2026-08-17: the solution block lists x.a only.
add_test(NAME "test_repl-adt-solve_omits_unconstrained"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"type Point = {a: sbf, b: sbf}. solve x:Point = x && x.a = 1\" -S trace")
set_tests_properties("test_repl-adt-solve_omits_unconstrained" PROPERTIES
	PASS_REGULAR_EXPRESSION "x\\.a := \\{ 1 \\}:sbf"
	FAIL_REGULAR_EXPRESSION "x\\.b :=;Error")

add_test(NAME "test_repl-adt-run_then_normalize_then_run"
	COMMAND bash -c "printf 'type Point = {a: sbf, b: sbf}. i:Point := in console. o:Point := out console. run o[0] = i[0].\\n{ a: \"1\", b: \"0\" }\\nq\\ntype Point = {a: sbf, b: sbf}. n ex x:Point (x = 0)\\ntype Point = {a: sbf, b: sbf}. i2:Point := in console. o2:Point := out console. run o2[0] = i2[0].\\n{ a: \"0\", b: \"1\" }\\nq\\nquit\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-adt-run_then_normalize_then_run" PROPERTIES
	PASS_REGULAR_EXPRESSION "o2\\[0\\] := \\{ a: \"0\", b: \"1\" \\}"
	FAIL_REGULAR_EXPRESSION "Error")
