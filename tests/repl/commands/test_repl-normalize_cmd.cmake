#
# normalize command
#

include(add_repl_test)

# term
add_repl_test(normalize_cmd_bf "n X & 1" ": X")
add_repl_test(normalize_cmd_nso "n X & X' = 0" ": T")

# formula
add_repl_test(normalize_cmd_wff "n X & X' = 0" ": T")
add_repl_test(normalize_cmd_memory "X & X' = 0. n %" ": T")

# function applications
add_repl_test(normalize_cmd_func_app_1 "f(x) := x'. n f(t)" "t'")

# predicate applications
add_repl_test(normalize_cmd_pred_app_1 "p(x) := x' = 0. n p(t)" "t' = 0")

# 'normalize' command for terms with history reference
add_repl_test(normalize_cmd_bf_nonmem "normalize 1" ": 1")
add_repl_test(normalize_cmd_bf_mem_rel "1. normalize %-0" "1")
add_repl_test(normalize_cmd_bf_mem_abs "1. normalize %1" "1")

# 'normalize' command for normalization for formulas with history reference
add_repl_test(normalize_cmd_wff_nonmem "normalize T" ": T")
add_repl_test(normalize_cmd_wff_mem_rel "T. normalize %-0" "T")
add_repl_test(normalize_cmd_wff_mem_abs "T. normalize %1" "T")

# AP-N3 regression: get_type_and_arg() used to deref a null child.
add_repl_test_fail(normalize_cmd-multiindex_fixed_point_call
	"g[0, 0](Y) := Y = 0. g[n, 0](Y) := g[n - 1, 0](Y). normalize g(Y)"
	"multiindex offset relations is not supported")

# by_grammar parse-only cases (SHAPE-I/K/O): assert parse success only.
add_repl_test(normalize_cmd-by_grammar-shape_i_01_f_f_f_fall_x_o1_and_x_eq_0 "normalize F (F (F ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8])))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_02_g_fall_x_o1_and_x_eq_0 "normalize G ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8])." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_03_f_f_f_fex_x_o1_and_x_eq_o1 "normalize F (F (F ((fex x (o1[t]:bv[8] & x:bv[8])) = o1[t]:bv[8])))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_04_g_fex_x_o1_and_x_eq_o1 "normalize G ((fex x (o1[t]:bv[8] & x:bv[8])) = o1[t]:bv[8])." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_05_fall_x_eq_0_until_fex_y_eq_o2 "normalize ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8]) U ((fex y (o2[t]:bv[8] & y:bv[8])) = o2[t]:bv[8])." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_06_g_f_fall_x_o1_and_x_eq_0 "normalize G (F ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8]))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_k_08_f_f_f "normalize F (F (F))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_01_f_f_f_i1_qlt_gt_0 "normalize F (F (F (i1[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_02_g_f_f_i1_qlt_gt_0 "normalize G (F (F (i1[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_03_f_f_g_i1_qlt_gt_0 "normalize F (F (G (i1[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_04_f_i1_gt_0_until_f_i2_gt_0 "normalize F ((i1[t]:qlt > {0}:qlt) U (F (i2[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_05_g_f_f_i1_bv_eq_b10110101 "normalize G (F (F (i1[t]:bv[8] = {#b10110101}:bv[8])))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_06_i1_gt_0_until_f_f_i2_gt_0 "normalize (i1[t]:qlt > {0}:qlt) U (F (F (i2[t]:qlt > {0}:qlt)))." "%[0-9]+")

# hooks: wff_imply / wff_rimply / wff_equiv / wff_lt / wff_lteq (tests/unit/test_hooks.cpp)
add_repl_test(normalize_cmd-hooks_wff_imply-01_f_imply_x_eq_t    "normalize F -> x = 0"           ": T")
add_repl_test(normalize_cmd-hooks_wff_imply-04_x_imply_t_eq_t    "normalize (x = 0) -> T"         ": T")
add_repl_test(normalize_cmd-hooks_wff_imply-05_x_imply_x_eq_t    "normalize (x = 0) -> (x = 0)"   ": T")
add_repl_test(normalize_cmd-hooks_wff_rimply-04_t_rimply_x_eq_t  "normalize T <- (x = 0)"         ": T")
add_repl_test(normalize_cmd-hooks_wff_rimply-05_x_rimply_f_eq_t  "normalize (x = 0) <- F"         ": T")
add_repl_test(normalize_cmd-hooks_wff_equiv-05_x_equiv_x_eq_t    "normalize (x = 0) <-> (x = 0)"  ": T")
add_repl_test(normalize_cmd-hooks_wff_equiv-06_x_equiv_negx_eq_f "normalize (x = 0) <-> !(x = 0)" ": F")
add_repl_test(normalize_cmd-hooks_wff_equiv-07_negx_equiv_x_eq_f "normalize !(x = 0) <-> (x = 0)" ": F")
add_repl_test(normalize_cmd-hooks_wff_lt_lteq-03_lt_self_eq_f    "normalize x < x"                ": F")
add_repl_test(normalize_cmd-hooks_wff_lt_lteq-04_lteq_self_eq_t  "normalize x <= x"               ": T")

# quantifiers -- normalized verdict (assertion 2; raw form is in history_cmd)
add_repl_test(normalize_cmd-quantifiers-ex_01_ex_x_x_eq_0        "normalize ex x x=0."             ": T")
add_repl_test(normalize_cmd-quantifiers-ex_02_ex_x_y_xy_eq_0     "normalize ex x,y xy=0."          ": T")
add_repl_test(normalize_cmd-quantifiers-ex_03_ex_x_ex_y_xy_eq_0  "normalize ex x ex y xy=0."       ": T")
# "all" TEST_CASE, all_cases (test_integration-quantifiers.cpp:18-22, 67)
add_repl_test(normalize_cmd-quantifiers-all_01_all_x_x_ne_0        "normalize all x x!=0."           ": F")
add_repl_test(normalize_cmd-quantifiers-all_02_all_x_y_xy_ne_0     "normalize all x,y xy!=0."        ": F")
add_repl_test(normalize_cmd-quantifiers-all_03_all_x_all_y_xy_ne_0 "normalize all x all y xy!=0."    ": F")
# "ex all" TEST_CASE, ex_all_cases (test_integration-quantifiers.cpp:24-28, 68)
add_repl_test(normalize_cmd-quantifiers-ex_all_01_ex_x_all_y_x_eq_y "normalize ex x all y x=y."                ": F")
add_repl_test(normalize_cmd-quantifiers-ex_all_02_ex_xy_all_wz_x_eq_w_and_y_eq_z "normalize ex x,y all w,z x=w&&y=z." ": F")
# "all ex" TEST_CASE, all_ex_cases (test_integration-quantifiers.cpp:30-34, 69)
add_repl_test(normalize_cmd-quantifiers-all_ex_01_all_x_ex_y_x_eq_y "normalize all x ex y x=y."                ": T")
add_repl_test(normalize_cmd-quantifiers-all_ex_02_all_xy_ex_wz_x_eq_w_and_y_eq_z "normalize all x,y ex w,z x=w && y=z." ": T")
