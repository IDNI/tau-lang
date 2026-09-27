# `tau compile` verb: emit + build + run the echo spec end-to-end.
include(add_compile_test)

add_compile_test(compile_verb-echo
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	REQUIRES hostfs)

# Period-terminated spec: proves compile_spec parses via get_spec, not just
# the bare-formula get_formula fallback (echo.tau above has no period).
add_compile_test(compile_verb-echo_dot
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo_dot.tau"
	REQUIRES hostfs)

# A lookback atom under an eventuality drives a __step_ge<k> guard
# (ltl_aba_helpers.tmpl.h's step_guard_prop) into the synthesized strategy;
# this is the emit_main/table_step_provider path's own smoke test for it.
add_compile_test(compile_verb-ltl_lookback_under_eventuality
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/ltl_lookback_under_eventuality.tau"
	REQUIRES hostfs)

# --cxx names the compiler the nested cmake configure uses: an unusable
# name must reach that configure (and fail it) instead of being replaced
# by whatever clang++ is on PATH. The spec names no BA so every pack gets
# as far as the configure; it is copied to a temp dir because the build
# tree is placed beside the spec file.
add_compile_test(compile_verb-cxx_override_is_used
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo_untyped.tau"
	CHECKER "${CMAKE_CURRENT_LIST_DIR}/../check_cxx_override.cmake"
	PASS_REGEX "nonexistent/c\\+\\+")

# A tautological literal still gives its clause a two-step warm-up, so the
# spec is realizable and compiles (o2 may be 0 at steps 0 and 1).
add_compile_test(compile_verb-warm_up_tautology
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/warm_up_tautology.tau"
	REQUIRES hostfs)

# The program plays the strategy `run` plays. Over each of these specs the
# data game decides, and the program used to play the abstraction's
# strategy instead: it stopped at step 2 finding no witness for its edge,
add_compile_run_test(compile_verb-plays_run_strategy 6
	"(always o2[t] = o1[t-1] && !(o1[t-1] = 0)) && (sometimes !(o2[t-1] = o1[t]))")
# printed nothing (its edge had no solution at step 0),
add_compile_run_test(compile_verb-plays_run_strategy_from_start 6
	"(always o2[t] = o1[t-1] && o3[t] = o2[t-1] && (!(o2[t-1] = o1[t]) || !(o2[t-1] = 1))) && (sometimes (o1[t] = 0 && o2[t-1] = o1[t]))")
# never met the awaited event,
add_compile_run_test(compile_verb-plays_run_strategy_goal_met 6
	"(always o2[t]:bv[1] = o1[t-1]:bv[1] && o1[t-1]:bv[1] = 0) && (sometimes !(o1[t]:bv[1] = o2[t-1]:bv[1]))")
# or was refused, its bitvector output given no witness.
add_compile_run_test(compile_verb-plays_run_strategy_witness 6
	"(always o1[t-1]:bv[1] = 0) && (sometimes !(o2[t-1]:bv[1] = 1))")
# An output no atom of the strategy reads is printed, as `run` prints it.
add_compile_run_test(compile_verb-prints_every_output 6
	"(always (o2[0]:bv[1] = 1 || o2[0]:bv[1] = 0) && o1[1]:bv[1] = 1 && !((o3[t-1]:bv[1] = 1 && o3[t]:bv[1] = 0))) && (sometimes (o3[t]:bv[1] = o1[t-1]:bv[1] && !(o3[t-2]:bv[1] = 1)))")
# `run` executes each of these specs by solving each step, with no
# strategy, and so does the program. It used to play the abstraction's
# strategy, which broke the specification at step 1,
add_compile_run_test(compile_verb-solves_as_run 6
	"always o2[t]:bv[1] = o1[t-1]:bv[1] && o3[t]:bv[1] = o2[t-1]:bv[1] && o3[t-1]:bv[1] = 0")
# printed nothing,
add_compile_run_test(compile_verb-solves_as_run_from_start 6
	"(always o3[0] = 0 && (!(o2[t-1] = o1[t-1]) || o2[t-1] = 0) && o1[t-1] = o3[t-1]) && (sometimes o3[t-1] = o1[t-1])")
# or was refused, its bitvector output given no witness.
add_compile_run_test(compile_verb-solves_as_run_witness 6
	"always o1[t-1]:bv[1] = 1")
