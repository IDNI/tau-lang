# Strategy-execution tests -- migrated from tests/unit/test_ltl_strategy_exec.cpp

include(add_strategy_export_test)

set(TF "${CMAKE_SOURCE_DIR}/tests/integration/test_files")
set(SE_XYZX "${TF}/tau-aba-xyzx-length_4.in")        # <:x>,<:y>,<:z>,<:x>
set(SE_XYZXW "${TF}/tau-aba-xyzxw-length_5.in")      # + <:w>
set(SE_A2J "${TF}/tau-named_a_to_j-length_10.in")    # <:a>,<:b>,<:c>,...
set(SE_FFTT "${TF}/tau-bool_FFTT-length_4.in")       # F.,F.,T.,T.

# EXEC-G: G with constant tau outputs (exact)
add_repl_test(strategy_exec-exec_g_01_g_o1_eq0_6steps
	"o1:tau := out console. run 6 steps G (o1[t] = 0)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := F[^0-9A-Za-z].*o1\\[4\\] := F[^0-9A-Za-z].*o1\\[5\\] := F[^0-9A-Za-z]") # :75
add_repl_test(strategy_exec-exec_g_02_g_o1_eq1_6steps
	"o1:tau := out console. run 6 steps G (o1[t] = 1)."
	"o1\\[0\\] := T[^0-9A-Za-z].*o1\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := T[^0-9A-Za-z].*o1\\[4\\] := T[^0-9A-Za-z].*o1\\[5\\] := T[^0-9A-Za-z]") # :83
add_repl_test(strategy_exec-exec_g_03_g_o1_eq_tau_top_4steps
	"o1:tau := out console. run 4 steps G (o1[t]:tau = {T.}:tau)."
	"o1\\[0\\] := T[^0-9A-Za-z].*o1\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := T[^0-9A-Za-z]") # :91
add_repl_test(strategy_exec-exec_g_04_g_o1_eq_tau_bot_4steps
	"o1:tau := out console. run 4 steps G (o1[t]:tau = {F.}:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := F[^0-9A-Za-z]") # :99

# EXEC-F: F with constant tau outputs
add_repl_test(strategy_exec-exec_f_01_f_o1_eq0_1step
	"o1:tau := out console. run 1 steps F (o1[t] = 0)."
	"o1\\[0\\] := F[^0-9A-Za-z]") # :113
add_repl_test(strategy_exec-exec_f_02_f_o1_eq1_1step
	"o1:tau := out console. run 1 steps F (o1[t] = 1)."
	"o1\\[0\\] := T[^0-9A-Za-z]") # :121
# EXISTENTIAL: source only checks `satisfied` = some step is T.
add_repl_test(strategy_exec-exec_f_03_f_o1_eq_tau_top_3steps
	"o1:tau := out console. run 3 steps F (o1[t]:tau = {T.}:tau)."
	"(o1\\[0\\] := T[^0-9A-Za-z]|o1\\[1\\] := T[^0-9A-Za-z]|o1\\[2\\] := T[^0-9A-Za-z])") # :129

# EXEC-U: Until / Weak-until
add_repl_test(strategy_exec-exec_u_01_o1_eq0_until_o1_eq1_4steps
	"o1:tau := out console. run 4 steps (o1[t] = 0) U (o1[t] = 1)."
	"(o1\\[0\\] := T[^0-9A-Za-z]|o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := T[^0-9A-Za-z]|o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := T[^0-9A-Za-z]|o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := T[^0-9A-Za-z])") # :149
# Weak until: all-F (no T at all) is legal, unlike U-01.
# alternative is why this differs from U-01.
add_repl_test(strategy_exec-exec_u_02_o1_eq0_wuntil_o1_eq1_4steps
	"o1:tau := out console. run 4 steps (o1[t] = 0) W (o1[t] = 1)."
	"(o1\\[0\\] := T[^0-9A-Za-z]|o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := T[^0-9A-Za-z]|o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := T[^0-9A-Za-z]|o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := T[^0-9A-Za-z]|o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := F[^0-9A-Za-z])") # :168
# Source asserts ONLY vals[0] and vals[1]; step 2 onward is explicitly free.
add_repl_test(strategy_exec-exec_u_03_o1_eq0_wuntil_i1_eq1_4steps
	"i1:tau := in file(\\\"${SE_FFTT}\\\"). o1:tau := out console. run 4 steps (o1[t] = 0) W (i1[t] = 1)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z]") # :182

# EXEC-GF: G(F(phi)) liveness -- EXISTENTIAL in the source (found_F / found_T)
add_repl_test(strategy_exec-exec_gf_01_g_f_o1_eq0_6steps
	"o1:tau := out console. run 6 steps G (F (o1[t] = 0))."
	"(o1\\[0\\] := F[^0-9A-Za-z]|o1\\[1\\] := F[^0-9A-Za-z]|o1\\[2\\] := F[^0-9A-Za-z]|o1\\[3\\] := F[^0-9A-Za-z]|o1\\[4\\] := F[^0-9A-Za-z]|o1\\[5\\] := F[^0-9A-Za-z])") # :202
add_repl_test(strategy_exec-exec_gf_02_g_f_o1_eq1_6steps
	"o1:tau := out console. run 6 steps G (F (o1[t] = 1))."
	"(o1\\[0\\] := T[^0-9A-Za-z]|o1\\[1\\] := T[^0-9A-Za-z]|o1\\[2\\] := T[^0-9A-Za-z]|o1\\[3\\] := T[^0-9A-Za-z]|o1\\[4\\] := T[^0-9A-Za-z]|o1\\[5\\] := T[^0-9A-Za-z])") # :212
# GF-03 runs two independent runs, so it migrates as two entries.
# migrates as two entries -- a and b -- not one.
add_repl_test(strategy_exec-exec_gf_03a_g_f_o1_eq0_4steps
	"o1:tau := out console. run 4 steps G (F (o1[t] = 0))."
	"(o1\\[0\\] := F[^0-9A-Za-z]|o1\\[1\\] := F[^0-9A-Za-z]|o1\\[2\\] := F[^0-9A-Za-z]|o1\\[3\\] := F[^0-9A-Za-z])") # :225
add_repl_test(strategy_exec-exec_gf_03b_g_f_o1_eq1_4steps
	"o1:tau := out console. run 4 steps G (F (o1[t] = 1))."
	"(o1\\[0\\] := T[^0-9A-Za-z]|o1\\[1\\] := T[^0-9A-Za-z]|o1\\[2\\] := T[^0-9A-Za-z]|o1\\[3\\] := T[^0-9A-Za-z])") # :225

# EXEC-IO: input mirroring
add_repl_test(strategy_exec-exec_io_01_g_o1_eq_i1_4steps
	"i1:tau := in file(\\\"${SE_XYZX}\\\"). o1:tau := out console. run 4 steps G (o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := <:x> = 0[^0-9A-Za-z].*o1\\[1\\] := <:y> = 0[^0-9A-Za-z].*o1\\[2\\] := <:z> = 0[^0-9A-Za-z].*o1\\[3\\] := <:x> = 0[^0-9A-Za-z]") # :248
# NONEMPTY: source checks only !v.empty() for 3 steps.
add_repl_test(strategy_exec-exec_io_02_g_o1_eq_sbf_x_and_y_3steps
	"o1:sbf := out console. run 3 steps G (o1[t]:sbf = {X & Y}:sbf)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := ") # :258
add_repl_test(strategy_exec-exec_io_03_f_o1_eq_i1_3steps
	"i1:tau := in file(\\\"${SE_A2J}\\\"). o1:tau := out console. run 3 steps F (o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := <:a> = 0[^0-9A-Za-z]") # :274

# EXEC-LB: lookback; only steps with a defined lookback are asserted.
add_repl_test(strategy_exec-exec_lb_01_g_o1_eq_i1_prev_5steps
	"i1:tau := in file(\\\"${SE_XYZXW}\\\"). o1:tau := out console. run 5 steps G (o1[t]:tau = i1[t-1]:tau)."
	"o1\\[1\\] := <:x> = 0[^0-9A-Za-z].*o1\\[2\\] := <:y> = 0[^0-9A-Za-z].*o1\\[3\\] := <:z> = 0[^0-9A-Za-z].*o1\\[4\\] := <:x> = 0[^0-9A-Za-z]") # :291
add_repl_test(strategy_exec-exec_lb_02_g_o1_eq_i1_prev2_6steps
	"i1:tau := in file(\\\"${SE_A2J}\\\"). o1:tau := out console. run 6 steps G (o1[t]:tau = i1[t-2]:tau)."
	"o1\\[2\\] := <:a> = 0[^0-9A-Za-z].*o1\\[3\\] := <:b> = 0[^0-9A-Za-z].*o1\\[4\\] := <:c> = 0[^0-9A-Za-z].*o1\\[5\\] := <:d> = 0[^0-9A-Za-z]") # :304

# EXEC-SBF / EXEC-BV: NONEMPTY only (source checks !empty()).
add_repl_test(strategy_exec-exec_sbf_01_g_o1_eq_sbf_x_and_y_4steps
	"o1:sbf := out console. run 4 steps G (o1[t]:sbf = {X & Y}:sbf)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ") # :321
add_repl_test(strategy_exec-exec_sbf_02_f_o1_eq_sbf_x_or_z_1step
	"o1:sbf := out console. run 1 steps F (o1[t]:sbf = {X | Z}:sbf)."
	"o1\\[0\\] := ") # :336
# EXISTENTIAL over a 3-step run: source sets `found` when a value is nonempty.
add_repl_test(strategy_exec-exec_sbf_03_g_f_o1_eq_sbf_x_and_y_3steps
	"o1:sbf := out console. run 3 steps G (F (o1[t]:sbf = {X & Y}:sbf))."
	"o1\\[0\\] := ") # :351
add_repl_test(strategy_exec-exec_bv_01_g_o1_eq_bv_b10110101_4steps
	"o1:bv[8] := out console. run 4 steps G (o1[t]:bv[8] = {#b10110101}:bv[8])."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ") # :374
add_repl_test(strategy_exec-exec_bv_02_f_o1_eq_bv_5_1step
	"o1:bv[8] := out console. run 1 steps F (o1[t]:bv[8] = {5}:bv[8])."
	"o1\\[0\\] := ") # :389

# EXEC-MS: every disjoint 2-step window must contain the value.
add_repl_test(strategy_exec-exec_ms_01_g_f_o1_eq0_8steps_windows
	"o1:tau := out console. run 8 steps G (F (o1[t] = 0))."
	"(o1\\[0\\] := F[^0-9A-Za-z]|o1\\[1\\] := F[^0-9A-Za-z]).*(o1\\[2\\] := F[^0-9A-Za-z]|o1\\[3\\] := F[^0-9A-Za-z]).*(o1\\[4\\] := F[^0-9A-Za-z]|o1\\[5\\] := F[^0-9A-Za-z]).*(o1\\[6\\] := F[^0-9A-Za-z]|o1\\[7\\] := F[^0-9A-Za-z])") # :412
add_repl_test(strategy_exec-exec_ms_02_g_f_o1_eq1_8steps_windows
	"o1:tau := out console. run 8 steps G (F (o1[t] = 1))."
	"(o1\\[0\\] := T[^0-9A-Za-z]|o1\\[1\\] := T[^0-9A-Za-z]).*(o1\\[2\\] := T[^0-9A-Za-z]|o1\\[3\\] := T[^0-9A-Za-z]).*(o1\\[4\\] := T[^0-9A-Za-z]|o1\\[5\\] := T[^0-9A-Za-z]).*(o1\\[6\\] := T[^0-9A-Za-z]|o1\\[7\\] := T[^0-9A-Za-z])") # :425
# Source asserts ONLY vals[0] == "T" (the `right_held_at_step0` flag).
add_repl_test(strategy_exec-exec_ms_03_o1_eq0_release_o1_eq1_5steps
	"o1:tau := out console. run 5 steps (o1[t] = 0) R (o1[t] = 1)."
	"o1\\[0\\] := T[^0-9A-Za-z]") # :438

# EXEC-VER: semantic self-verification (same shapes as above)
add_repl_test(strategy_exec-exec_ver_01_g_o1_eq0_5steps
	"o1:tau := out console. run 5 steps G (o1[t] = 0)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := F[^0-9A-Za-z].*o1\\[4\\] := F[^0-9A-Za-z]") # :458
add_repl_test(strategy_exec-exec_ver_02_g_o1_eq1_5steps
	"o1:tau := out console. run 5 steps G (o1[t] = 1)."
	"o1\\[0\\] := T[^0-9A-Za-z].*o1\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := T[^0-9A-Za-z].*o1\\[4\\] := T[^0-9A-Za-z]") # :467
add_repl_test(strategy_exec-exec_ver_03_g_o1_eq_i1_4steps
	"i1:tau := in file(\\\"${SE_XYZX}\\\"). o1:tau := out console. run 4 steps G (o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := <:x> = 0[^0-9A-Za-z].*o1\\[1\\] := <:y> = 0[^0-9A-Za-z].*o1\\[2\\] := <:z> = 0[^0-9A-Za-z].*o1\\[3\\] := <:x> = 0[^0-9A-Za-z]") # :477
add_repl_test(strategy_exec-exec_ver_04_f_o1_eq0_4steps
	"o1:tau := out console. run 4 steps F (o1[t] = 0)."
	"(o1\\[0\\] := F[^0-9A-Za-z]|o1\\[1\\] := F[^0-9A-Za-z]|o1\\[2\\] := F[^0-9A-Za-z]|o1\\[3\\] := F[^0-9A-Za-z])") # :488
add_repl_test(strategy_exec-exec_ver_05_f_o1_eq1_4steps
	"o1:tau := out console. run 4 steps F (o1[t] = 1)."
	"(o1\\[0\\] := T[^0-9A-Za-z]|o1\\[1\\] := T[^0-9A-Za-z]|o1\\[2\\] := T[^0-9A-Za-z]|o1\\[3\\] := T[^0-9A-Za-z])") # :498

# SQ1: strategy export
add_strategy_export_test(strategy_exec-sq1_01_f_g_o1_eq0_exports_hoa
	"sat F (G (o1[t] = 0)).") # :529
# SQ1-02 only asserts is_tau_formula_sat, so it is a plain `sat` test.
add_repl_test(strategy_exec-sq1_02_f_g_o1_eq0_realizable
	"sat F (G (o1[t] = 0))." ": T") # :552
