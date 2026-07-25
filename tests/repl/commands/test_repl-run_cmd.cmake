# run / stop commands
include(add_repl_test)

set(TF "${CMAKE_SOURCE_DIR}/tests/integration/test_files")
set(BOT "${TF}/tau-bottom-length_8.in")
set(TOP "${TF}/tau-top-length_8.in")
set(ALT "${TF}/tau-alternating_zeros_and_ones-length_10.in")
set(NAM "${TF}/tau-named-length_6.in")
set(A2J "${TF}/tau-named_a_to_j-length_10.in")
set(P2Y "${TF}/tau-named_p_to_y-length_10.in")
set(XYZWVU "${TF}/tau-named_xyzwvu-length_6.in")
set(ABA_XYZXY5 "${TF}/tau-aba-xyzxy-length_5.in")
set(ABA_WXVY4 "${TF}/tau-aba-wxvy-length_4.in")
set(ABA_WXVZYU6 "${TF}/tau-aba-wxvzyu-length_6.in")
set(ABA_XYZXYZXY8 "${TF}/tau-aba-xyzxyzxy-length_8.in")
set(ABA_XYZX4 "${TF}/tau-aba-xyzx-length_4.in")
set(ABA_WXY3 "${TF}/tau-aba-wxy-length_3.in")
set(ABA_XYZXW5 "${TF}/tau-aba-xyzxw-length_5.in")
set(ABA_XYZXYW6 "${TF}/tau-aba-xyzxyw-length_6.in")
set(ABA_XYZWXYZ7 "${TF}/tau-aba-xyzwxyz-length_7.in")
set(ABA_WXVZ4 "${TF}/tau-aba-wxvz-length_4.in")
set(ABA_XYZWX5 "${TF}/tau-aba-xyzwx-length_5.in")
set(SBFONES "${TF}/sbf-ones-length_10.in")
set(BVONES "${TF}/bv-ones-length_10.in")

# ── ADV-G: G(o=c) -- every output is c, regardless of adversarial input ───────
add_repl_test(run_cmd-ltl_correctness-adv_g_01_o_eq_0_bottom
	"i1:tau := in file(\\\"${BOT}\\\"). o1:tau := out console. run 8 steps G (o1[t] = 0)."
	"o1\\[0\\] := F.*o1\\[1\\] := F.*o1\\[2\\] := F.*o1\\[3\\] := F.*o1\\[4\\] := F.*o1\\[5\\] := F.*o1\\[6\\] := F.*o1\\[7\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_g_02_o_eq_0_top
	"i1:tau := in file(\\\"${TOP}\\\"). o1:tau := out console. run 8 steps G (o1[t] = 0)."
	"o1\\[0\\] := F.*o1\\[1\\] := F.*o1\\[2\\] := F.*o1\\[3\\] := F.*o1\\[4\\] := F.*o1\\[5\\] := F.*o1\\[6\\] := F.*o1\\[7\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_g_03_o_eq_0_alt
	"i1:tau := in file(\\\"${ALT}\\\"). o1:tau := out console. run 8 steps G (o1[t] = 0)."
	"o1\\[0\\] := F.*o1\\[1\\] := F.*o1\\[2\\] := F.*o1\\[3\\] := F.*o1\\[4\\] := F.*o1\\[5\\] := F.*o1\\[6\\] := F.*o1\\[7\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_g_04_o_eq_1_bottom
	"i1:tau := in file(\\\"${BOT}\\\"). o1:tau := out console. run 8 steps G (o1[t] = 1)."
	"o1\\[0\\] := T.*o1\\[1\\] := T.*o1\\[2\\] := T.*o1\\[3\\] := T.*o1\\[4\\] := T.*o1\\[5\\] := T.*o1\\[6\\] := T.*o1\\[7\\] := T")
add_repl_test(run_cmd-ltl_correctness-adv_g_05_o_eq_1_top
	"i1:tau := in file(\\\"${TOP}\\\"). o1:tau := out console. run 8 steps G (o1[t] = 1)."
	"o1\\[0\\] := T.*o1\\[1\\] := T.*o1\\[2\\] := T.*o1\\[3\\] := T.*o1\\[4\\] := T.*o1\\[5\\] := T.*o1\\[6\\] := T.*o1\\[7\\] := T")
add_repl_test(run_cmd-ltl_correctness-adv_g_06_o_eq_1_alt
	"i1:tau := in file(\\\"${ALT}\\\"). o1:tau := out console. run 8 steps G (o1[t] = 1)."
	"o1\\[0\\] := T.*o1\\[1\\] := T.*o1\\[2\\] := T.*o1\\[3\\] := T.*o1\\[4\\] := T.*o1\\[5\\] := T.*o1\\[6\\] := T.*o1\\[7\\] := T")

# ── ADV-MIR: G(o=i) -- output mirrors the input stream ────────────────────────
add_repl_test(run_cmd-ltl_correctness-adv_mir_01_bottom_all_f
	"i1:tau := in file(\\\"${BOT}\\\"). o1:tau := out console. run 6 steps G (o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := F.*o1\\[1\\] := F.*o1\\[2\\] := F.*o1\\[3\\] := F.*o1\\[4\\] := F.*o1\\[5\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_mir_02_top_all_t
	"i1:tau := in file(\\\"${TOP}\\\"). o1:tau := out console. run 6 steps G (o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := T.*o1\\[1\\] := T.*o1\\[2\\] := T.*o1\\[3\\] := T.*o1\\[4\\] := T.*o1\\[5\\] := T")
add_repl_test(run_cmd-ltl_correctness-adv_mir_03_alt_mirror
	"i1:tau := in file(\\\"${ALT}\\\"). o1:tau := out console. run 8 steps G (o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := F.*o1\\[1\\] := T.*o1\\[2\\] := F.*o1\\[3\\] := T.*o1\\[4\\] := F.*o1\\[5\\] := T.*o1\\[6\\] := F.*o1\\[7\\] := T")
add_repl_test(run_cmd-ltl_correctness-adv_mir_04_named_mirror
	"i1:tau := in file(\\\"${NAM}\\\"). o1:tau := out console. run 6 steps G (o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := <:a> = 0.*o1\\[1\\] := <:a> = 0.*o1\\[2\\] := <:a> = 0.*o1\\[3\\] := <:a> = 0.*o1\\[4\\] := <:a> = 0.*o1\\[5\\] := <:a> = 0")

# ── ADV-LB: G(o=i[t-1]) -- step 0 is warmup, steps 1+ equal the prior input ────
add_repl_test(run_cmd-ltl_correctness-adv_lb_01_bottom
	"i1:tau := in file(\\\"${BOT}\\\"). o1:tau := out console. run 6 steps G (o1[t]:tau = i1[t-1]:tau)."
	"o1\\[1\\] := F.*o1\\[2\\] := F.*o1\\[3\\] := F.*o1\\[4\\] := F.*o1\\[5\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_lb_02_top
	"i1:tau := in file(\\\"${TOP}\\\"). o1:tau := out console. run 6 steps G (o1[t]:tau = i1[t-1]:tau)."
	"o1\\[1\\] := T.*o1\\[2\\] := T.*o1\\[3\\] := T.*o1\\[4\\] := T.*o1\\[5\\] := T")
add_repl_test(run_cmd-ltl_correctness-adv_lb_03_alt_shifted
	"i1:tau := in file(\\\"${ALT}\\\"). o1:tau := out console. run 7 steps G (o1[t]:tau = i1[t-1]:tau)."
	"o1\\[1\\] := F.*o1\\[2\\] := T.*o1\\[3\\] := F.*o1\\[4\\] := T.*o1\\[5\\] := F.*o1\\[6\\] := T")

# ── ADV-F: F(o=c) -- c eventually appears (check_F) ───────────────────────────
add_repl_test(run_cmd-ltl_correctness-adv_f_01_o_eq_0_bottom
	"i1:tau := in file(\\\"${BOT}\\\"). o1:tau := out console. run 4 steps F (o1[t] = 0)."
	"o1\\[[0-9]+\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_f_02_o_eq_0_top
	"i1:tau := in file(\\\"${TOP}\\\"). o1:tau := out console. run 4 steps F (o1[t] = 0)."
	"o1\\[[0-9]+\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_f_03_o_eq_1_alt
	"i1:tau := in file(\\\"${ALT}\\\"). o1:tau := out console. run 4 steps F (o1[t] = 1)."
	"o1\\[[0-9]+\\] := T")

# ── ADV-GF: G(F(o=c)) -- c appears (liveness; check_GF checks every 2-window) ──
add_repl_test(run_cmd-ltl_correctness-adv_gf_01_o_eq_0_bottom
	"i1:tau := in file(\\\"${BOT}\\\"). o1:tau := out console. run 8 steps G (F (o1[t] = 0))."
	"o1\\[[0-9]+\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_gf_02_o_eq_0_top
	"i1:tau := in file(\\\"${TOP}\\\"). o1:tau := out console. run 8 steps G (F (o1[t] = 0))."
	"o1\\[[0-9]+\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_gf_03_o_eq_0_alt
	"i1:tau := in file(\\\"${ALT}\\\"). o1:tau := out console. run 8 steps G (F (o1[t] = 0))."
	"o1\\[[0-9]+\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_gf_04_o_eq_1_bottom
	"i1:tau := in file(\\\"${BOT}\\\"). o1:tau := out console. run 8 steps G (F (o1[t] = 1))."
	"o1\\[[0-9]+\\] := T")
add_repl_test(run_cmd-ltl_correctness-adv_gf_05_o_eq_1_named
	"i1:tau := in file(\\\"${NAM}\\\"). o1:tau := out console. run 6 steps G (F (o1[t] = 1))."
	"o1\\[[0-9]+\\] := T")

# ── ADV-U: (o=1) U (o=0) -- right (F) eventually appears (check_U) ─────────────
add_repl_test(run_cmd-ltl_correctness-adv_u_01_bottom
	"i1:tau := in file(\\\"${BOT}\\\"). o1:tau := out console. run 6 steps (o1[t] = 1) U (o1[t] = 0)."
	"o1\\[[0-9]+\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_u_02_top
	"i1:tau := in file(\\\"${TOP}\\\"). o1:tau := out console. run 6 steps (o1[t] = 1) U (o1[t] = 0)."
	"o1\\[[0-9]+\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_u_03_alt
	"i1:tau := in file(\\\"${ALT}\\\"). o1:tau := out console. run 6 steps (o1[t] = 1) U (o1[t] = 0)."
	"o1\\[[0-9]+\\] := F")

# ── ADV-W: weak until -- right value appears (check_W) ────────────────────────
add_repl_test(run_cmd-ltl_correctness-adv_w_01_o1_until_o0_bottom
	"i1:tau := in file(\\\"${BOT}\\\"). o1:tau := out console. run 6 steps (o1[t] = 1) W (o1[t] = 0)."
	"o1\\[[0-9]+\\] := F")
add_repl_test(run_cmd-ltl_correctness-adv_w_02_o0_until_o1_alt
	"i1:tau := in file(\\\"${ALT}\\\"). o1:tau := out console. run 6 steps (o1[t] = 0) W (o1[t] = 1)."
	"o1\\[[0-9]+\\] := T")

# ── ADV-SBF / ADV-BV: no input; every output is non-empty ─────────────────────
add_repl_test(run_cmd-ltl_correctness-adv_sbf_01_g_x_and_y
	"o1:sbf := out console. run 6 steps G (o1[t]:sbf = {X & Y}:sbf)."
	"o1\\[[0-9]+\\] := X Y")
add_repl_test(run_cmd-ltl_correctness-adv_sbf_02_f_x_or_z
	"o1:sbf := out console. run 4 steps F (o1[t]:sbf = {X | Z}:sbf)."
	"o1\\[[0-9]+\\] := X ")
add_repl_test(run_cmd-ltl_correctness-adv_bv_01_g_b10110101
	"o1:bv[8] := out console. run 6 steps G (o1[t]:bv[8] = {#b10110101}:bv[8])."
	"o1\\[[0-9]+\\] := 181")

# ── stop command ──────────────────────────────────────────────────────────────
add_repl_test(stop_cmd-no_run_in_progress "stop" "no run in progress")

# multi-io: two inputs, two outputs (test_integration-interpreter_ltl_multiio.cpp)
add_repl_test(run_cmd-multiio-t01_dual_mirror_8steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 8 steps G (o1[t]:tau = i1[t]:tau && o2[t]:tau = i2[t]:tau)."
	"o1\\[0\\] := <:a> = 0[^0-9A-Za-z].*o2\\[0\\] := <:p> = 0[^0-9A-Za-z].*o1\\[1\\] := <:b> = 0[^0-9A-Za-z].*o2\\[1\\] := <:q> = 0[^0-9A-Za-z].*o1\\[2\\] := <:c> = 0[^0-9A-Za-z].*o2\\[2\\] := <:r> = 0[^0-9A-Za-z].*o1\\[3\\] := <:d> = 0[^0-9A-Za-z].*o2\\[3\\] := <:s> = 0[^0-9A-Za-z].*o1\\[4\\] := <:e> = 0[^0-9A-Za-z].*o2\\[4\\] := <:t> = 0[^0-9A-Za-z].*o1\\[5\\] := <:f> = 0[^0-9A-Za-z].*o2\\[5\\] := <:u> = 0[^0-9A-Za-z].*o1\\[6\\] := <:g> = 0[^0-9A-Za-z].*o2\\[6\\] := <:v> = 0[^0-9A-Za-z].*o1\\[7\\] := <:h> = 0[^0-9A-Za-z].*o2\\[7\\] := <:w> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t02_swapped_mirror_7steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 7 steps G (o1[t]:tau = i2[t]:tau && o2[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := <:p> = 0[^0-9A-Za-z].*o2\\[0\\] := <:a> = 0[^0-9A-Za-z].*o1\\[1\\] := <:q> = 0[^0-9A-Za-z].*o2\\[1\\] := <:b> = 0[^0-9A-Za-z].*o1\\[2\\] := <:r> = 0[^0-9A-Za-z].*o2\\[2\\] := <:c> = 0[^0-9A-Za-z].*o1\\[3\\] := <:s> = 0[^0-9A-Za-z].*o2\\[3\\] := <:d> = 0[^0-9A-Za-z].*o1\\[4\\] := <:t> = 0[^0-9A-Za-z].*o2\\[4\\] := <:e> = 0[^0-9A-Za-z].*o1\\[5\\] := <:u> = 0[^0-9A-Za-z].*o2\\[5\\] := <:f> = 0[^0-9A-Za-z].*o1\\[6\\] := <:v> = 0[^0-9A-Za-z].*o2\\[6\\] := <:g> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t03_mirror_and_const_false_6steps
	"i1:tau := in file(\\\"${A2J}\\\"). o1:tau := out console. o2:tau := out console. run 6 steps G (o1[t]:tau = i1[t]:tau && o2[t]:tau = 0)."
	"o1\\[0\\] := <:a> = 0[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := <:b> = 0[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:c> = 0[^0-9A-Za-z].*o2\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := <:d> = 0[^0-9A-Za-z].*o2\\[3\\] := F[^0-9A-Za-z].*o1\\[4\\] := <:e> = 0[^0-9A-Za-z].*o2\\[4\\] := F[^0-9A-Za-z].*o1\\[5\\] := <:f> = 0[^0-9A-Za-z].*o2\\[5\\] := F[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t04_mirror_and_const_true_6steps
	"i1:tau := in file(\\\"${A2J}\\\"). o1:tau := out console. o2:tau := out console. run 6 steps G (o1[t]:tau = i1[t]:tau && o2[t]:tau = 1)."
	"o1\\[0\\] := <:a> = 0[^0-9A-Za-z].*o2\\[0\\] := T[^0-9A-Za-z].*o1\\[1\\] := <:b> = 0[^0-9A-Za-z].*o2\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := <:c> = 0[^0-9A-Za-z].*o2\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := <:d> = 0[^0-9A-Za-z].*o2\\[3\\] := T[^0-9A-Za-z].*o1\\[4\\] := <:e> = 0[^0-9A-Za-z].*o2\\[4\\] := T[^0-9A-Za-z].*o1\\[5\\] := <:f> = 0[^0-9A-Za-z].*o2\\[5\\] := T[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t05_two_mirrors_one_g_9steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 9 steps G (o1[t]:tau = i1[t]:tau && o2[t]:tau = i2[t]:tau)."
	"o1\\[0\\] := <:a> = 0[^0-9A-Za-z].*o2\\[0\\] := <:p> = 0[^0-9A-Za-z].*o1\\[1\\] := <:b> = 0[^0-9A-Za-z].*o2\\[1\\] := <:q> = 0[^0-9A-Za-z].*o1\\[2\\] := <:c> = 0[^0-9A-Za-z].*o2\\[2\\] := <:r> = 0[^0-9A-Za-z].*o1\\[3\\] := <:d> = 0[^0-9A-Za-z].*o2\\[3\\] := <:s> = 0[^0-9A-Za-z].*o1\\[4\\] := <:e> = 0[^0-9A-Za-z].*o2\\[4\\] := <:t> = 0[^0-9A-Za-z].*o1\\[5\\] := <:f> = 0[^0-9A-Za-z].*o2\\[5\\] := <:u> = 0[^0-9A-Za-z].*o1\\[6\\] := <:g> = 0[^0-9A-Za-z].*o2\\[6\\] := <:v> = 0[^0-9A-Za-z].*o1\\[7\\] := <:h> = 0[^0-9A-Za-z].*o2\\[7\\] := <:w> = 0[^0-9A-Za-z].*o1\\[8\\] := <:i> = 0[^0-9A-Za-z].*o2\\[8\\] := <:x> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t06_both_const_false_7steps
	"o1:tau := out console. o2:tau := out console. run 7 steps G (o1[t]:tau = 0 && o2[t]:tau = 0)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o2\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := F[^0-9A-Za-z].*o2\\[3\\] := F[^0-9A-Za-z].*o1\\[4\\] := F[^0-9A-Za-z].*o2\\[4\\] := F[^0-9A-Za-z].*o1\\[5\\] := F[^0-9A-Za-z].*o2\\[5\\] := F[^0-9A-Za-z].*o1\\[6\\] := F[^0-9A-Za-z].*o2\\[6\\] := F[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t07_both_const_true_6steps
	"o1:tau := out console. o2:tau := out console. run 6 steps G (o1[t]:tau = 1 && o2[t]:tau = 1)."
	"o1\\[0\\] := T[^0-9A-Za-z].*o2\\[0\\] := T[^0-9A-Za-z].*o1\\[1\\] := T[^0-9A-Za-z].*o2\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := T[^0-9A-Za-z].*o2\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := T[^0-9A-Za-z].*o2\\[3\\] := T[^0-9A-Za-z].*o1\\[4\\] := T[^0-9A-Za-z].*o2\\[4\\] := T[^0-9A-Za-z].*o1\\[5\\] := T[^0-9A-Za-z].*o2\\[5\\] := T[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t08_o1_copies_i2_o2_const_false_8steps
	"i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 8 steps G (o1[t]:tau = i2[t]:tau && o2[t]:tau = 0)."
	"o1\\[0\\] := <:p> = 0[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := <:q> = 0[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:r> = 0[^0-9A-Za-z].*o2\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := <:s> = 0[^0-9A-Za-z].*o2\\[3\\] := F[^0-9A-Za-z].*o1\\[4\\] := <:t> = 0[^0-9A-Za-z].*o2\\[4\\] := F[^0-9A-Za-z].*o1\\[5\\] := <:u> = 0[^0-9A-Za-z].*o2\\[5\\] := F[^0-9A-Za-z].*o1\\[6\\] := <:v> = 0[^0-9A-Za-z].*o2\\[6\\] := F[^0-9A-Za-z].*o1\\[7\\] := <:w> = 0[^0-9A-Za-z].*o2\\[7\\] := F[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t09_swapped_mirrors_one_g_10steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 10 steps G (o1[t]:tau = i2[t]:tau && o2[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := <:p> = 0[^0-9A-Za-z].*o2\\[0\\] := <:a> = 0[^0-9A-Za-z].*o1\\[1\\] := <:q> = 0[^0-9A-Za-z].*o2\\[1\\] := <:b> = 0[^0-9A-Za-z].*o1\\[2\\] := <:r> = 0[^0-9A-Za-z].*o2\\[2\\] := <:c> = 0[^0-9A-Za-z].*o1\\[3\\] := <:s> = 0[^0-9A-Za-z].*o2\\[3\\] := <:d> = 0[^0-9A-Za-z].*o1\\[4\\] := <:t> = 0[^0-9A-Za-z].*o2\\[4\\] := <:e> = 0[^0-9A-Za-z].*o1\\[5\\] := <:u> = 0[^0-9A-Za-z].*o2\\[5\\] := <:f> = 0[^0-9A-Za-z].*o1\\[6\\] := <:v> = 0[^0-9A-Za-z].*o2\\[6\\] := <:g> = 0[^0-9A-Za-z].*o1\\[7\\] := <:w> = 0[^0-9A-Za-z].*o2\\[7\\] := <:h> = 0[^0-9A-Za-z].*o1\\[8\\] := <:x> = 0[^0-9A-Za-z].*o2\\[8\\] := <:i> = 0[^0-9A-Za-z].*o1\\[9\\] := <:y> = 0[^0-9A-Za-z].*o2\\[9\\] := <:j> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t10_mirror_and_const_true_one_g_8steps
	"i1:tau := in file(\\\"${A2J}\\\"). o1:tau := out console. o2:tau := out console. run 8 steps G (o1[t]:tau = i1[t]:tau && o2[t]:tau = 1)."
	"o1\\[0\\] := <:a> = 0[^0-9A-Za-z].*o2\\[0\\] := T[^0-9A-Za-z].*o1\\[1\\] := <:b> = 0[^0-9A-Za-z].*o2\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := <:c> = 0[^0-9A-Za-z].*o2\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := <:d> = 0[^0-9A-Za-z].*o2\\[3\\] := T[^0-9A-Za-z].*o1\\[4\\] := <:e> = 0[^0-9A-Za-z].*o2\\[4\\] := T[^0-9A-Za-z].*o1\\[5\\] := <:f> = 0[^0-9A-Za-z].*o2\\[5\\] := T[^0-9A-Za-z].*o1\\[6\\] := <:g> = 0[^0-9A-Za-z].*o2\\[6\\] := T[^0-9A-Za-z].*o1\\[7\\] := <:h> = 0[^0-9A-Za-z].*o2\\[7\\] := T[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t11_1step_delay_7steps
	"i1:tau := in file(\\\"${A2J}\\\"). o1:tau := out console. run 7 steps G (o1[t]:tau = i1[t-1]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := <:a> = 0[^0-9A-Za-z].*o1\\[2\\] := <:b> = 0[^0-9A-Za-z].*o1\\[3\\] := <:c> = 0[^0-9A-Za-z].*o1\\[4\\] := <:d> = 0[^0-9A-Za-z].*o1\\[5\\] := <:e> = 0[^0-9A-Za-z].*o1\\[6\\] := <:f> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t12_2step_delay_8steps
	"i1:tau := in file(\\\"${A2J}\\\"). o1:tau := out console. run 8 steps G (o1[t]:tau = i1[t-2]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:a> = 0[^0-9A-Za-z].*o1\\[3\\] := <:b> = 0[^0-9A-Za-z].*o1\\[4\\] := <:c> = 0[^0-9A-Za-z].*o1\\[5\\] := <:d> = 0[^0-9A-Za-z].*o1\\[6\\] := <:e> = 0[^0-9A-Za-z].*o1\\[7\\] := <:f> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t13_3step_delay_8steps
	"i1:tau := in file(\\\"${A2J}\\\"). o1:tau := out console. run 8 steps G (o1[t]:tau = i1[t-3]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := <:a> = 0[^0-9A-Za-z].*o1\\[4\\] := <:b> = 0[^0-9A-Za-z].*o1\\[5\\] := <:c> = 0[^0-9A-Za-z].*o1\\[6\\] := <:d> = 0[^0-9A-Za-z].*o1\\[7\\] := <:e> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t14_crossed_1step_delay_7steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 7 steps G (o1[t]:tau = i2[t-1]:tau && o2[t]:tau = i1[t-1]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := <:p> = 0[^0-9A-Za-z].*o2\\[1\\] := <:a> = 0[^0-9A-Za-z].*o1\\[2\\] := <:q> = 0[^0-9A-Za-z].*o2\\[2\\] := <:b> = 0[^0-9A-Za-z].*o1\\[3\\] := <:r> = 0[^0-9A-Za-z].*o2\\[3\\] := <:c> = 0[^0-9A-Za-z].*o1\\[4\\] := <:s> = 0[^0-9A-Za-z].*o2\\[4\\] := <:d> = 0[^0-9A-Za-z].*o1\\[5\\] := <:t> = 0[^0-9A-Za-z].*o2\\[5\\] := <:e> = 0[^0-9A-Za-z].*o1\\[6\\] := <:u> = 0[^0-9A-Za-z].*o2\\[6\\] := <:f> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t15_mixed_delays_8steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 8 steps G (o1[t]:tau = i1[t-1]:tau && o2[t]:tau = i2[t-2]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:a> = 0[^0-9A-Za-z].*o2\\[2\\] := <:p> = 0[^0-9A-Za-z].*o1\\[3\\] := <:b> = 0[^0-9A-Za-z].*o2\\[3\\] := <:q> = 0[^0-9A-Za-z].*o1\\[4\\] := <:c> = 0[^0-9A-Za-z].*o2\\[4\\] := <:r> = 0[^0-9A-Za-z].*o1\\[5\\] := <:d> = 0[^0-9A-Za-z].*o2\\[5\\] := <:s> = 0[^0-9A-Za-z].*o1\\[6\\] := <:e> = 0[^0-9A-Za-z].*o2\\[6\\] := <:t> = 0[^0-9A-Za-z].*o1\\[7\\] := <:f> = 0[^0-9A-Za-z].*o2\\[7\\] := <:u> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t16_reversed_mixed_delays_7steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 7 steps G (o1[t]:tau = i1[t-2]:tau && o2[t]:tau = i2[t-1]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:a> = 0[^0-9A-Za-z].*o2\\[2\\] := <:p> = 0[^0-9A-Za-z].*o1\\[3\\] := <:b> = 0[^0-9A-Za-z].*o2\\[3\\] := <:q> = 0[^0-9A-Za-z].*o1\\[4\\] := <:c> = 0[^0-9A-Za-z].*o2\\[4\\] := <:r> = 0[^0-9A-Za-z].*o1\\[5\\] := <:d> = 0[^0-9A-Za-z].*o2\\[5\\] := <:s> = 0[^0-9A-Za-z].*o1\\[6\\] := <:e> = 0[^0-9A-Za-z].*o2\\[6\\] := <:t> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t17_3step_delay_6steps
	"i1:tau := in file(\\\"${A2J}\\\"). o1:tau := out console. run 6 steps G (o1[t]:tau = i1[t-3]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := <:a> = 0[^0-9A-Za-z].*o1\\[4\\] := <:b> = 0[^0-9A-Za-z].*o1\\[5\\] := <:c> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t18_crossed_2step_delay_8steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 8 steps G (o1[t]:tau = i2[t-2]:tau && o2[t]:tau = i1[t-2]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:p> = 0[^0-9A-Za-z].*o2\\[2\\] := <:a> = 0[^0-9A-Za-z].*o1\\[3\\] := <:q> = 0[^0-9A-Za-z].*o2\\[3\\] := <:b> = 0[^0-9A-Za-z].*o1\\[4\\] := <:r> = 0[^0-9A-Za-z].*o2\\[4\\] := <:c> = 0[^0-9A-Za-z].*o1\\[5\\] := <:s> = 0[^0-9A-Za-z].*o2\\[5\\] := <:d> = 0[^0-9A-Za-z].*o1\\[6\\] := <:t> = 0[^0-9A-Za-z].*o2\\[6\\] := <:e> = 0[^0-9A-Za-z].*o1\\[7\\] := <:u> = 0[^0-9A-Za-z].*o2\\[7\\] := <:f> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t19_1step_delay_xyzwvu_6steps
	"i1:tau := in file(\\\"${XYZWVU}\\\"). o1:tau := out console. run 6 steps G (o1[t]:tau = i1[t-1]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := <:x> = 0[^0-9A-Za-z].*o1\\[2\\] := <:y> = 0[^0-9A-Za-z].*o1\\[3\\] := <:z> = 0[^0-9A-Za-z].*o1\\[4\\] := <:w> = 0[^0-9A-Za-z].*o1\\[5\\] := <:v> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t20_cross_variable_3step_delay_8steps
	"i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. run 8 steps G (o1[t]:tau = i2[t-3]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := <:p> = 0[^0-9A-Za-z].*o1\\[4\\] := <:q> = 0[^0-9A-Za-z].*o1\\[5\\] := <:r> = 0[^0-9A-Za-z].*o1\\[6\\] := <:s> = 0[^0-9A-Za-z].*o1\\[7\\] := <:t> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t21_delay_plus_const_false_7steps
	"i1:tau := in file(\\\"${A2J}\\\"). o1:tau := out console. o2:tau := out console. run 7 steps G (o1[t]:tau = i1[t-1]:tau && o2[t]:tau = 0)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := <:a> = 0[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:b> = 0[^0-9A-Za-z].*o2\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := <:c> = 0[^0-9A-Za-z].*o2\\[3\\] := F[^0-9A-Za-z].*o1\\[4\\] := <:d> = 0[^0-9A-Za-z].*o2\\[4\\] := F[^0-9A-Za-z].*o1\\[5\\] := <:e> = 0[^0-9A-Za-z].*o2\\[5\\] := F[^0-9A-Za-z].*o1\\[6\\] := <:f> = 0[^0-9A-Za-z].*o2\\[6\\] := F[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t22_delay_plus_const_true_7steps
	"i1:tau := in file(\\\"${A2J}\\\"). o1:tau := out console. o2:tau := out console. run 7 steps G (o1[t]:tau = i1[t-1]:tau && o2[t]:tau = 1)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := <:a> = 0[^0-9A-Za-z].*o2\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := <:b> = 0[^0-9A-Za-z].*o2\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := <:c> = 0[^0-9A-Za-z].*o2\\[3\\] := T[^0-9A-Za-z].*o1\\[4\\] := <:d> = 0[^0-9A-Za-z].*o2\\[4\\] := T[^0-9A-Za-z].*o1\\[5\\] := <:e> = 0[^0-9A-Za-z].*o2\\[5\\] := T[^0-9A-Za-z].*o1\\[6\\] := <:f> = 0[^0-9A-Za-z].*o2\\[6\\] := T[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t23_current_plus_2step_delay_8steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 8 steps G (o1[t]:tau = i1[t]:tau && o2[t]:tau = i2[t-2]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:a> = 0[^0-9A-Za-z].*o2\\[2\\] := <:p> = 0[^0-9A-Za-z].*o1\\[3\\] := <:b> = 0[^0-9A-Za-z].*o2\\[3\\] := <:q> = 0[^0-9A-Za-z].*o1\\[4\\] := <:c> = 0[^0-9A-Za-z].*o2\\[4\\] := <:r> = 0[^0-9A-Za-z].*o1\\[5\\] := <:d> = 0[^0-9A-Za-z].*o2\\[5\\] := <:s> = 0[^0-9A-Za-z].*o1\\[6\\] := <:e> = 0[^0-9A-Za-z].*o2\\[6\\] := <:t> = 0[^0-9A-Za-z].*o1\\[7\\] := <:f> = 0[^0-9A-Za-z].*o2\\[7\\] := <:u> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t24_both_constant_false_true_10steps
	"o1:tau := out console. o2:tau := out console. run 10 steps G (o1[t]:tau = 0 && o2[t]:tau = 1)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := T[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o2\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := F[^0-9A-Za-z].*o2\\[3\\] := T[^0-9A-Za-z].*o1\\[4\\] := F[^0-9A-Za-z].*o2\\[4\\] := T[^0-9A-Za-z].*o1\\[5\\] := F[^0-9A-Za-z].*o2\\[5\\] := T[^0-9A-Za-z].*o1\\[6\\] := F[^0-9A-Za-z].*o2\\[6\\] := T[^0-9A-Za-z].*o1\\[7\\] := F[^0-9A-Za-z].*o2\\[7\\] := T[^0-9A-Za-z].*o1\\[8\\] := F[^0-9A-Za-z].*o2\\[8\\] := T[^0-9A-Za-z].*o1\\[9\\] := F[^0-9A-Za-z].*o2\\[9\\] := T[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t25_current_and_1step_delayed_same_input_8steps
	"i1:tau := in file(\\\"${A2J}\\\"). o1:tau := out console. o2:tau := out console. run 8 steps G (o1[t]:tau = i1[t]:tau && o2[t]:tau = i1[t-1]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := <:b> = 0[^0-9A-Za-z].*o2\\[1\\] := <:a> = 0[^0-9A-Za-z].*o1\\[2\\] := <:c> = 0[^0-9A-Za-z].*o2\\[2\\] := <:b> = 0[^0-9A-Za-z].*o1\\[3\\] := <:d> = 0[^0-9A-Za-z].*o2\\[3\\] := <:c> = 0[^0-9A-Za-z].*o1\\[4\\] := <:e> = 0[^0-9A-Za-z].*o2\\[4\\] := <:d> = 0[^0-9A-Za-z].*o1\\[5\\] := <:f> = 0[^0-9A-Za-z].*o2\\[5\\] := <:e> = 0[^0-9A-Za-z].*o1\\[6\\] := <:g> = 0[^0-9A-Za-z].*o2\\[6\\] := <:f> = 0[^0-9A-Za-z].*o1\\[7\\] := <:h> = 0[^0-9A-Za-z].*o2\\[7\\] := <:g> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t26_2step_delay_plus_const_true_8steps
	"i1:tau := in file(\\\"${A2J}\\\"). o1:tau := out console. o2:tau := out console. run 8 steps G (o1[t]:tau = i1[t-2]:tau && o2[t]:tau = 1)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:a> = 0[^0-9A-Za-z].*o2\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := <:b> = 0[^0-9A-Za-z].*o2\\[3\\] := T[^0-9A-Za-z].*o1\\[4\\] := <:c> = 0[^0-9A-Za-z].*o2\\[4\\] := T[^0-9A-Za-z].*o1\\[5\\] := <:d> = 0[^0-9A-Za-z].*o2\\[5\\] := T[^0-9A-Za-z].*o1\\[6\\] := <:e> = 0[^0-9A-Za-z].*o2\\[6\\] := T[^0-9A-Za-z].*o1\\[7\\] := <:f> = 0[^0-9A-Za-z].*o2\\[7\\] := T[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t27_cross_delayed_plus_current_7steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 7 steps G (o1[t]:tau = i2[t-1]:tau && o2[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := <:p> = 0[^0-9A-Za-z].*o2\\[1\\] := <:a> = 0[^0-9A-Za-z].*o1\\[2\\] := <:q> = 0[^0-9A-Za-z].*o2\\[2\\] := <:b> = 0[^0-9A-Za-z].*o1\\[3\\] := <:r> = 0[^0-9A-Za-z].*o2\\[3\\] := <:c> = 0[^0-9A-Za-z].*o1\\[4\\] := <:s> = 0[^0-9A-Za-z].*o2\\[4\\] := <:d> = 0[^0-9A-Za-z].*o1\\[5\\] := <:t> = 0[^0-9A-Za-z].*o2\\[5\\] := <:e> = 0[^0-9A-Za-z].*o1\\[6\\] := <:u> = 0[^0-9A-Za-z].*o2\\[6\\] := <:f> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t28_3step_delay_plus_current_9steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 9 steps G (o1[t]:tau = i1[t-3]:tau && o2[t]:tau = i2[t]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o2\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := <:a> = 0[^0-9A-Za-z].*o2\\[3\\] := <:p> = 0[^0-9A-Za-z].*o1\\[4\\] := <:b> = 0[^0-9A-Za-z].*o2\\[4\\] := <:q> = 0[^0-9A-Za-z].*o1\\[5\\] := <:c> = 0[^0-9A-Za-z].*o2\\[5\\] := <:r> = 0[^0-9A-Za-z].*o1\\[6\\] := <:d> = 0[^0-9A-Za-z].*o2\\[6\\] := <:s> = 0[^0-9A-Za-z].*o1\\[7\\] := <:e> = 0[^0-9A-Za-z].*o2\\[7\\] := <:t> = 0[^0-9A-Za-z].*o1\\[8\\] := <:f> = 0[^0-9A-Za-z].*o2\\[8\\] := <:u> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t29_both_delayed_9steps
	"i1:tau := in file(\\\"${A2J}\\\"). i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 9 steps G (o1[t]:tau = i1[t-1]:tau && o2[t]:tau = i2[t-2]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:a> = 0[^0-9A-Za-z].*o2\\[2\\] := <:p> = 0[^0-9A-Za-z].*o1\\[3\\] := <:b> = 0[^0-9A-Za-z].*o2\\[3\\] := <:q> = 0[^0-9A-Za-z].*o1\\[4\\] := <:c> = 0[^0-9A-Za-z].*o2\\[4\\] := <:r> = 0[^0-9A-Za-z].*o1\\[5\\] := <:d> = 0[^0-9A-Za-z].*o2\\[5\\] := <:s> = 0[^0-9A-Za-z].*o1\\[6\\] := <:e> = 0[^0-9A-Za-z].*o2\\[6\\] := <:t> = 0[^0-9A-Za-z].*o1\\[7\\] := <:f> = 0[^0-9A-Za-z].*o2\\[7\\] := <:u> = 0[^0-9A-Za-z].*o1\\[8\\] := <:g> = 0[^0-9A-Za-z].*o2\\[8\\] := <:v> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-multiio-t30_cross_2step_delay_plus_const_false_8steps
	"i2:tau := in file(\\\"${P2Y}\\\"). o1:tau := out console. o2:tau := out console. run 8 steps G (o1[t]:tau = i2[t-2]:tau && o2[t]:tau = 0)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:p> = 0[^0-9A-Za-z].*o2\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := <:q> = 0[^0-9A-Za-z].*o2\\[3\\] := F[^0-9A-Za-z].*o1\\[4\\] := <:r> = 0[^0-9A-Za-z].*o2\\[4\\] := F[^0-9A-Za-z].*o1\\[5\\] := <:s> = 0[^0-9A-Za-z].*o2\\[5\\] := F[^0-9A-Za-z].*o1\\[6\\] := <:t> = 0[^0-9A-Za-z].*o2\\[6\\] := F[^0-9A-Za-z].*o1\\[7\\] := <:u> = 0[^0-9A-Za-z].*o2\\[7\\] := F[^0-9A-Za-z]")

# ── ltl_aba_execution (tests/integration/test_integration-ltl_aba_execution.cpp)

# ── Group 1: G constant outputs (tau) ──────────────────────────────────────
add_repl_test(run_cmd-ltl_aba_execution-g1_01_g_o_eq0_4steps
	"o1:tau := out console. run 4 steps G(o1[t]:tau = 0)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := F[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g1_02_g_o_eq1_4steps
	"o1:tau := out console. run 4 steps G(o1[t]:tau = 1)."
	"o1\\[0\\] := T[^0-9A-Za-z].*o1\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := T[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g1_03_g_o_eq0_6steps
	"o1:tau := out console. run 6 steps G(o1[t]:tau = 0)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := F[^0-9A-Za-z].*o1\\[4\\] := F[^0-9A-Za-z].*o1\\[5\\] := F[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g1_04_g_o_eq1_8steps
	"o1:tau := out console. run 8 steps G(o1[t]:tau = 1)."
	"o1\\[0\\] := T[^0-9A-Za-z].*o1\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := T[^0-9A-Za-z].*o1\\[4\\] := T[^0-9A-Za-z].*o1\\[5\\] := T[^0-9A-Za-z].*o1\\[6\\] := T[^0-9A-Za-z].*o1\\[7\\] := T[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g1_05_two_outputs_5steps
	"o1:tau := out console. o2:tau := out console. run 5 steps G(o1[t]:tau = 0 && o2[t]:tau = 1)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := T[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o2\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := F[^0-9A-Za-z].*o2\\[3\\] := T[^0-9A-Za-z].*o1\\[4\\] := F[^0-9A-Za-z].*o2\\[4\\] := T[^0-9A-Za-z]")

# ── Group 2: G input mirroring (tau) ───────────────────────────────────────
add_repl_test(run_cmd-ltl_aba_execution-g2_01_mirror_3vals
	"i1:tau := in file(\\\"${XYZWVU}\\\"). o1:tau := out console. run 3 steps G(o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := <:x> = 0[^0-9A-Za-z].*o1\\[1\\] := <:y> = 0[^0-9A-Za-z].*o1\\[2\\] := <:z> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g2_02_mirror_compound_4vals
	"i1:tau := in file(\\\"${ABA_WXVY4}\\\"). o1:tau := out console. run 4 steps G(o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := <:w> = 0[^0-9A-Za-z].*o1\\[1\\] := <:x> = 0[^0-9A-Za-z].*o1\\[2\\] := <:v> = 0[^0-9A-Za-z].*o1\\[3\\] := <:y> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g2_03_mirror_5vals_mixed
	"i1:tau := in file(\\\"${ABA_XYZXY5}\\\"). o1:tau := out console. run 5 steps G(o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := <:x> = 0[^0-9A-Za-z].*o1\\[1\\] := <:y> = 0[^0-9A-Za-z].*o1\\[2\\] := <:z> = 0[^0-9A-Za-z].*o1\\[3\\] := <:x> = 0[^0-9A-Za-z].*o1\\[4\\] := <:y> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g2_04_mirror_6vals_repeating
	"i1:tau := in file(\\\"${ABA_WXVZYU6}\\\"). o1:tau := out console. run 6 steps G(o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := <:w> = 0[^0-9A-Za-z].*o1\\[1\\] := <:x> = 0[^0-9A-Za-z].*o1\\[2\\] := <:v> = 0[^0-9A-Za-z].*o1\\[3\\] := <:z> = 0[^0-9A-Za-z].*o1\\[4\\] := <:y> = 0[^0-9A-Za-z].*o1\\[5\\] := <:u> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g2_05_mirror_8vals
	"i1:tau := in file(\\\"${ABA_XYZXYZXY8}\\\"). o1:tau := out console. run 8 steps G(o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := <:x> = 0[^0-9A-Za-z].*o1\\[1\\] := <:y> = 0[^0-9A-Za-z].*o1\\[2\\] := <:z> = 0[^0-9A-Za-z].*o1\\[3\\] := <:x> = 0[^0-9A-Za-z].*o1\\[4\\] := <:y> = 0[^0-9A-Za-z].*o1\\[5\\] := <:z> = 0[^0-9A-Za-z].*o1\\[6\\] := <:x> = 0[^0-9A-Za-z].*o1\\[7\\] := <:y> = 0[^0-9A-Za-z]")

# ── Group 3: G two outputs (tau) ───────────────────────────────────────────
add_repl_test(run_cmd-ltl_aba_execution-g3_01_mirror_and_const0_3steps
	"i1:tau := in file(\\\"${XYZWVU}\\\"). o1:tau := out console. o2:tau := out console. run 3 steps G(o1[t]:tau = i1[t]:tau && o2[t]:tau = 0)."
	"o1\\[0\\] := <:x> = 0[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := <:y> = 0[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:z> = 0[^0-9A-Za-z].*o2\\[2\\] := F[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g3_02_mirror_and_const1_4steps
	"i1:tau := in file(\\\"${XYZWVU}\\\"). o1:tau := out console. o2:tau := out console. run 4 steps G(o1[t]:tau = i1[t]:tau && o2[t]:tau = 1)."
	"o1\\[0\\] := <:x> = 0[^0-9A-Za-z].*o2\\[0\\] := T[^0-9A-Za-z].*o1\\[1\\] := <:y> = 0[^0-9A-Za-z].*o2\\[1\\] := T[^0-9A-Za-z].*o1\\[2\\] := <:z> = 0[^0-9A-Za-z].*o2\\[2\\] := T[^0-9A-Za-z].*o1\\[3\\] := <:w> = 0[^0-9A-Za-z].*o2\\[3\\] := T[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g3_03_const0_and_mirror_to_o2_4steps
	"i1:tau := in file(\\\"${ABA_XYZX4}\\\"). o1:tau := out console. o2:tau := out console. run 4 steps G(o1[t]:tau = 0 && o2[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := F[^0-9A-Za-z].*o2\\[0\\] := <:x> = 0[^0-9A-Za-z].*o1\\[1\\] := F[^0-9A-Za-z].*o2\\[1\\] := <:y> = 0[^0-9A-Za-z].*o1\\[2\\] := F[^0-9A-Za-z].*o2\\[2\\] := <:z> = 0[^0-9A-Za-z].*o1\\[3\\] := F[^0-9A-Za-z].*o2\\[3\\] := <:x> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g3_04_two_inputs_3steps
	"i1:tau := in file(\\\"${XYZWVU}\\\"). i2:tau := in file(\\\"${ABA_WXY3}\\\"). o1:tau := out console. o2:tau := out console. run 3 steps G(o1[t]:tau = i1[t]:tau && o2[t]:tau = i2[t]:tau)."
	"o1\\[0\\] := <:x> = 0[^0-9A-Za-z].*o2\\[0\\] := <:w> = 0[^0-9A-Za-z].*o1\\[1\\] := <:y> = 0[^0-9A-Za-z].*o2\\[1\\] := <:x> = 0[^0-9A-Za-z].*o1\\[2\\] := <:z> = 0[^0-9A-Za-z].*o2\\[2\\] := <:y> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g3_05_const_pair_5steps
	"o1:tau := out console. o2:tau := out console. run 5 steps G(o1[t]:tau = 1 && o2[t]:tau = 0)."
	"o1\\[0\\] := T[^0-9A-Za-z].*o2\\[0\\] := F[^0-9A-Za-z].*o1\\[1\\] := T[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := T[^0-9A-Za-z].*o2\\[2\\] := F[^0-9A-Za-z].*o1\\[3\\] := T[^0-9A-Za-z].*o2\\[3\\] := F[^0-9A-Za-z].*o1\\[4\\] := T[^0-9A-Za-z].*o2\\[4\\] := F[^0-9A-Za-z]")

# ── Group 4: G lookback inputs (tau) ───────────────────────────────────────
add_repl_test(run_cmd-ltl_aba_execution-g4_01_lb1_5steps
	"i1:tau := in file(\\\"${ABA_XYZXW5}\\\"). o1:tau := out console. run 5 steps G(o1[t]:tau = i1[t-1]:tau)."
	"o1\\[1\\] := <:x> = 0[^0-9A-Za-z].*o1\\[2\\] := <:y> = 0[^0-9A-Za-z].*o1\\[3\\] := <:z> = 0[^0-9A-Za-z].*o1\\[4\\] := <:x> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g4_02_lb2_6steps
	"i1:tau := in file(\\\"${ABA_XYZXYW6}\\\"). o1:tau := out console. run 6 steps G(o1[t]:tau = i1[t-2]:tau)."
	"o1\\[2\\] := <:x> = 0[^0-9A-Za-z].*o1\\[3\\] := <:y> = 0[^0-9A-Za-z].*o1\\[4\\] := <:z> = 0[^0-9A-Za-z].*o1\\[5\\] := <:x> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g4_03_lb3_7steps
	"i1:tau := in file(\\\"${ABA_XYZWXYZ7}\\\"). o1:tau := out console. run 7 steps G(o1[t]:tau = i1[t-3]:tau)."
	"o1\\[3\\] := <:x> = 0[^0-9A-Za-z].*o1\\[4\\] := <:y> = 0[^0-9A-Za-z].*o1\\[5\\] := <:z> = 0[^0-9A-Za-z].*o1\\[6\\] := <:w> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g4_04_lb1_4steps_compound
	"i1:tau := in file(\\\"${ABA_WXVZ4}\\\"). o1:tau := out console. run 4 steps G(o1[t]:tau = i1[t-1]:tau)."
	"o1\\[1\\] := <:w> = 0[^0-9A-Za-z].*o1\\[2\\] := <:x> = 0[^0-9A-Za-z].*o1\\[3\\] := <:v> = 0[^0-9A-Za-z]")
add_repl_test(run_cmd-ltl_aba_execution-g4_05_two_lookbacks_5steps
	"i1:tau := in file(\\\"${ABA_XYZWX5}\\\"). o1:tau := out console. o2:tau := out console. run 5 steps G(o1[t]:tau = i1[t-2]:tau && o2[t]:tau = i1[t-1]:tau)."
	"o2\\[0\\] := F[^0-9A-Za-z].*o2\\[1\\] := F[^0-9A-Za-z].*o1\\[2\\] := <:x> = 0[^0-9A-Za-z].*o2\\[2\\] := <:y> = 0[^0-9A-Za-z].*o1\\[3\\] := <:y> = 0[^0-9A-Za-z].*o2\\[3\\] := <:z> = 0[^0-9A-Za-z].*o1\\[4\\] := <:z> = 0[^0-9A-Za-z].*o2\\[4\\] := <:w> = 0[^0-9A-Za-z]")

# ── Group 5: F formulas (tau) ──────────────────────────────────────────────
add_repl_test(run_cmd-ltl_aba_execution-g5_01_f_o_eq0_4steps
	"o1:tau := out console. run 4 steps F(o1[t]:tau = 0)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g5_02_f_o_eq1_4steps
	"o1:tau := out console. run 4 steps F(o1[t]:tau = 1)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g5_03_f_o_eq_i1_5steps
	"i1:tau := in file(\\\"${NAM}\\\"). o1:tau := out console. run 5 steps F(o1[t]:tau = i1[t]:tau)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g5_04_f_two_outputs_4steps
	"o1:tau := out console. o2:tau := out console. run 4 steps F(o1[t]:tau = 0 && o2[t]:tau = 1)."
	"o1\\[0\\] := .*o2\\[0\\] := .*o1\\[1\\] := .*o2\\[1\\] := .*o1\\[2\\] := .*o2\\[2\\] := .*o1\\[3\\] := .*o2\\[3\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g5_05_g_mirror_and_f_const_5steps
	"i1:tau := in file(\\\"${ABA_XYZXY5}\\\"). o1:tau := out console. o2:tau := out console. run 5 steps G(o1[t]:tau = i1[t]:tau) && F(o2[t]:tau = 0)."
	"o1\\[0\\] := <:x> = 0[^0-9A-Za-z].*o2\\[0\\] := .*o1\\[1\\] := <:y> = 0[^0-9A-Za-z].*o2\\[1\\] := .*o1\\[2\\] := <:z> = 0[^0-9A-Za-z].*o2\\[2\\] := .*o1\\[3\\] := <:x> = 0[^0-9A-Za-z].*o2\\[3\\] := .*o1\\[4\\] := <:y> = 0[^0-9A-Za-z].*o2\\[4\\] := ")

# ── Group 6: U R W formulas (tau, no input) ────────────────────────────────
add_repl_test(run_cmd-ltl_aba_execution-g6_01_u_4steps
	"o1:tau := out console. run 4 steps (o1[t]:tau = 1) U (o1[t]:tau = 0)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g6_02_u_5steps
	"o1:tau := out console. run 5 steps (o1[t]:tau = 1) U (o1[t]:tau = 0)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g6_03_r_4steps
	"o1:tau := out console. run 4 steps (o1[t]:tau = 0) R (o1[t]:tau = 1)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g6_04_w_5steps
	"o1:tau := out console. run 5 steps (o1[t]:tau = 1) W (o1[t]:tau = 0)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g6_05_w_5steps_variant
	"o1:tau := out console. run 5 steps (o1[t]:tau = 0) W (o1[t]:tau = 1)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := ")

# ── Group 7: sbf G constant outputs (no input) ─────────────────────────────
add_repl_test(run_cmd-ltl_aba_execution-g7_01_sbf_g_xandy_4steps
	"o1:sbf := out console. run 4 steps G (o1[t]:sbf = {X & Y}:sbf)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g7_02_sbf_g_xorz_5steps
	"o1:sbf := out console. run 5 steps G (o1[t]:sbf = {X | Z}:sbf)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g7_03_sbf_g_compound_6steps
	"o1:sbf := out console. run 6 steps G (o1[t]:sbf = {X | (Y & Z)}:sbf)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := .*o1\\[5\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g7_04_sbf_g_xandy_7steps
	"o1:sbf := out console. run 7 steps G (o1[t]:sbf = {X & Y}:sbf)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := .*o1\\[5\\] := .*o1\\[6\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g7_05_sbf_g_compound_4steps
	"o1:sbf := out console. run 4 steps G (o1[t]:sbf = {X | (Y & Z)}:sbf)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ")

# ── Group 8: sbf G input mirroring (input value not asserted -> reuse ${SBFONES}) ──
add_repl_test(run_cmd-ltl_aba_execution-g8_01_sbf_mirror_3steps
	"i1:sbf := in file(\\\"${SBFONES}\\\"). o1:sbf := out console. run 3 steps G (o1[t]:sbf = i1[t]:sbf)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g8_02_sbf_mirror_4steps
	"i1:sbf := in file(\\\"${SBFONES}\\\"). o1:sbf := out console. run 4 steps G (o1[t]:sbf = i1[t]:sbf)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g8_03_sbf_mirror_5steps
	"i1:sbf := in file(\\\"${SBFONES}\\\"). o1:sbf := out console. run 5 steps G (o1[t]:sbf = i1[t]:sbf)."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g8_04_sbf_complement_mirror_4steps
	"i1:sbf := in file(\\\"${SBFONES}\\\"). o1:sbf := out console. run 4 steps G (o1[t]:sbf = i1[t]:sbf')."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g8_05_sbf_union_top_5steps
	"i1:sbf := in file(\\\"${SBFONES}\\\"). o1:sbf := out console. run 5 steps G (o1[t]:sbf | i1[t]:sbf = 1)."
	"o1\\[0\\] := ")

# ── Group 9: sbf G with lookback -- SHAPE-MINIMAL, individually probed live ──
add_repl_test(run_cmd-ltl_aba_execution-g9_01_sbf_lb1_4steps
	"i1:sbf := in file(\\\"${SBFONES}\\\"). o1:sbf := out console. run 4 steps G (o1[t]:sbf = i1[t-1]:sbf)."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g9_02_sbf_lb1_5steps
	"i1:sbf := in file(\\\"${SBFONES}\\\"). o1:sbf := out console. run 5 steps G (o1[t]:sbf = i1[t-1]:sbf)."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g9_03_sbf_lb2_5steps
	"i1:sbf := in file(\\\"${SBFONES}\\\"). o1:sbf := out console. run 5 steps G (o1[t]:sbf = i1[t-2]:sbf)."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g9_04_sbf_lb3_6steps
	"i1:sbf := in file(\\\"${SBFONES}\\\"). o1:sbf := out console. run 6 steps G (o1[t]:sbf = i1[t-3]:sbf)."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g9_05_sbf_self_lookback_5steps
	"o1:sbf := out console. run 5 steps G (o1[t]:sbf = o1[t-1]:sbf)."
	"o1\\[0\\] := ")

# ── Group 10: sbf F and U/W operators (no input) ───────────────────────────
add_repl_test(run_cmd-ltl_aba_execution-g10_01_sbf_f_xandy_4steps
	"o1:sbf := out console. run 4 steps F (o1[t]:sbf = {X & Y}:sbf)."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g10_02_sbf_f_compound_5steps
	"o1:sbf := out console. run 5 steps F (o1[t]:sbf = {X | (Y & Z)}:sbf)."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g10_03_sbf_u_4steps
	"o1:sbf := out console. run 4 steps (o1[t]:sbf = {X & Y}:sbf) U (o1[t]:sbf = {X | Z}:sbf)."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g10_04_sbf_w_5steps
	"o1:sbf := out console. run 5 steps (o1[t]:sbf = {X | (Y & Z)}:sbf) W (o1[t]:sbf = {X & Y}:sbf)."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g10_05_sbf_gf_6steps
	"o1:sbf := out console. run 6 steps G (F (o1[t]:sbf = {X & Y}:sbf))."
	"o1\\[0\\] := ")

# Group 11: bv G constant outputs (g11_01 is size-only; do NOT assert 181)
#   source -- see the "do NOT assert 181" note in the section comment above) ──
add_repl_test(run_cmd-ltl_aba_execution-g11_01_bv_g_b10110101_4steps
	"o1:bv[8] := out console. run 4 steps G (o1[t]:bv[8] = {#b10110101}:bv[8])."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g11_02_bv_g_b00001111_5steps
	"o1:bv[8] := out console. run 5 steps G (o1[t]:bv[8] = {#b00001111}:bv[8])."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g11_03_bv_g_b11110000_6steps
	"o1:bv[8] := out console. run 6 steps G (o1[t]:bv[8] = {#b11110000}:bv[8])."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := .*o1\\[5\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g11_04_bv_g_dec5_5steps
	"o1:bv[8] := out console. run 5 steps G (o1[t]:bv[8] = {5}:bv[8])."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g11_05_bv_g_dec255_7steps
	"o1:bv[8] := out console. run 7 steps G (o1[t]:bv[8] = {255}:bv[8])."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := .*o1\\[5\\] := .*o1\\[6\\] := ")

# Group 12: bv G input mirroring + LTL operators (g12_04 is shape-minimal)
#   for 01..04 -> reuse ${BVONES}; g12_04 is SHAPE-MINIMAL, probed live) ────
add_repl_test(run_cmd-ltl_aba_execution-g12_01_bv_mirror_3steps
	"i1:bv[8] := in file(\\\"${BVONES}\\\"). o1:bv[8] := out console. run 3 steps G (o1[t]:bv[8] = i1[t]:bv[8])."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g12_02_bv_mirror_4steps
	"i1:bv[8] := in file(\\\"${BVONES}\\\"). o1:bv[8] := out console. run 4 steps G (o1[t]:bv[8] = i1[t]:bv[8])."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g12_03_bv_mirror_5steps
	"i1:bv[8] := in file(\\\"${BVONES}\\\"). o1:bv[8] := out console. run 5 steps G (o1[t]:bv[8] = i1[t]:bv[8])."
	"o1\\[0\\] := .*o1\\[1\\] := .*o1\\[2\\] := .*o1\\[3\\] := .*o1\\[4\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g12_04_bv_lb1_mirror_5steps
	"i1:bv[8] := in file(\\\"${BVONES}\\\"). o1:bv[8] := out console. run 5 steps G (o1[t]:bv[8] = i1[t-1]:bv[8])."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g12_05_bv_f_b10110101_4steps
	"o1:bv[8] := out console. run 4 steps F (o1[t]:bv[8] = {#b10110101}:bv[8])."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g12_06_bv_u_5steps
	"o1:bv[8] := out console. run 5 steps (o1[t]:bv[8] = {#b00001111}:bv[8]) U (o1[t]:bv[8] = {#b11110000}:bv[8])."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g12_07_bv_gf_255_6steps
	"o1:bv[8] := out console. run 6 steps G (F (o1[t]:bv[8] = {255}:bv[8]))."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g12_08_bv_w_4steps
	"o1:bv[8] := out console. run 4 steps (o1[t]:bv[8] = {5}:bv[8]) W (o1[t]:bv[8] = {#b10110101}:bv[8])."
	"o1\\[0\\] := ")
add_repl_test(run_cmd-ltl_aba_execution-g12_09_bv_r_5steps
	"o1:bv[8] := out console. run 5 steps (o1[t]:bv[8] = {#b00001111}:bv[8]) R (o1[t]:bv[8] = {#b11110000}:bv[8])."
	"o1\\[0\\] := ")
# G12.10: unblocked by the scoped joint solve in step(); shape-nonempty.
add_repl_test(run_cmd-ltl_aba_execution-g12_10_bv_alternating_6steps
	"o1:bv[8] := out console. run 6 steps G (F (o1[t]:bv[8] = {#b10110101}:bv[8])) && G (F (!(o1[t]:bv[8] = {#b10110101}:bv[8])))."
	"o1\\[0\\] := ")
