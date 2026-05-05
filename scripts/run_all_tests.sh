#!/bin/bash
# Run all unit + integration tests with per-test watchdog.
# Every test runs inside safe_run_test.sh — kills the full process group on timeout.
# Usage: run_all_tests.sh [build_dir]
#
# Exit 0 if all pass/skip, non-zero if any fail or timeout.

BUILDDIR="${1:-./build}"
SAFE="$(dirname "$0")/safe_run_test.sh"
cd "$BUILDDIR" || { echo "build dir not found: $BUILDDIR"; exit 1; }

PASS=0; FAIL=0
declare -a FAILURES

run() {
    local name="$1" binary="$2" timeout="${3:-120}"
    if [ ! -f "$binary" ]; then
        echo "[SKIP] $name (binary not found)"
        return
    fi
    SAFE_RUN_TIMEOUT="$timeout" "$SAFE" "$binary" 2>&1
    local rc=$?
    if [ $rc -eq 0 ]; then
        echo "[PASS] $name"
        ((PASS++)) || true
    else
        echo "[FAIL] $name (exit $rc)"
        FAILURES+=("$name")
        ((FAIL++)) || true
    fi
}

# ── unit tests ────────────────────────────────────────────────────────────────
echo "=== unit tests ==="
run algorithm_a        ./test_algorithm_a
run algorithm_c        ./test_algorithm_c
run algorithm_d        ./test_algorithm_d
run algorithm_picker   ./test_algorithm_picker
run ba_constants       ./test_ba_constants
run ba_types           ./test_ba_types
run ba_types_inference ./test_ba_types_inference
run bool               ./test_bool
run bv_ba-simplification ./test_bv_ba-simplification
run bv_ba-solver1      ./test_bv_ba-solver1
run bv_ba-solver2      ./test_bv_ba-solver2
run by_grammar         ./test_by_grammar
run cpp_codegen        ./test_cpp_codegen
run cpp_codegen_bench  ./test_cpp_codegen_bench 300
run cpp_codegen_data_atoms ./test_cpp_codegen_data_atoms
run cpp_codegen_roundtrip  ./test_cpp_codegen_roundtrip
run cvc5               ./test_cvc5
run gc                 ./test_gc
run gr1_detect         ./test_gr1_detect
run hbdd               ./test_hbdd
run interpreter        ./test_interpreter
run liveness_decomp    ./test_liveness_decomp
run ltl_aba            ./test_ltl_aba           300
run ltl_adversarial    ./test_ltl_adversarial    300
run ltl_adversarial2   ./test_ltl_adversarial2   300
run ltl_benchmarks     ./test_ltl_benchmarks     300
run ltl_correctness    ./test_ltl_correctness    300
run ltl_deepseek2      ./test_ltl_deepseek2      300
run ltl_deepseek_gen   ./test_ltl_deepseek_generated 300
run ltl_fuzz           ./test_ltl_fuzz           300
run ltl_fuzz100        ./test_ltl_fuzz100        600
run ltl_multi_type     ./test_ltl_multi_type_oracle 300
run ltl_negative       ./test_ltl_negative       300
run ltl_perf           ./test_ltl_perf           300
run ltl_qlt_bv         ./test_ltl_qlt_bv         300
run ltl_soundness_reg  ./test_ltl_soundness_regression 300
run ltl_strategy_exec  ./test_ltl_strategy_exec  300
run normal_forms       ./test_normal_forms
run omcat_types        ./test_omcat_types
run product_ba         ./test_product_ba
run qlt_oracle         ./test_qlt_oracle         300
run sbf_ba_parsing     ./test_sbf_ba_parsing
run sbf_oracle         ./test_sbf_oracle          300
run spec_decompose     ./test_spec_decompose
run tau_spec           ./test_tau_spec
run tau_tree_from_parser ./test_tau_tree_from_parser
run tau_tree_printers  ./test_tau_tree_printers
run traversal          ./test_traversal
run union_find         ./test_union_find

# nlang: 5 parallel shards (~80-100s each when run in parallel)
echo "=== nlang parallel ==="
SAFE_RUN_TIMEOUT=180 "$SAFE" ./test_ltl_nlang_deepseek_1 &
P1=$!
SAFE_RUN_TIMEOUT=180 "$SAFE" ./test_ltl_nlang_deepseek_2 &
P2=$!
SAFE_RUN_TIMEOUT=180 "$SAFE" ./test_ltl_nlang_deepseek_3 &
P3=$!
SAFE_RUN_TIMEOUT=180 "$SAFE" ./test_ltl_nlang_deepseek_4 &
P4=$!
SAFE_RUN_TIMEOUT=180 "$SAFE" ./test_ltl_nlang_deepseek_5 &
P5=$!
for p in $P1 $P2 $P3 $P4 $P5; do
    wait "$p" && { echo "[PASS] nlang shard $p"; ((PASS++)) || true; } \
              || { echo "[FAIL] nlang shard $p"; FAILURES+=("nlang:shard:$p"); ((FAIL++)) || true; }
done

# ── integration tests ─────────────────────────────────────────────────────────
echo "=== integration tests ==="
IDIR="./tests/integration"
[ -d "$IDIR" ] || IDIR="."
for t in \
    test_integration-ba_types_inference \
    test_integration-bf_fixed_point \
    test_integration-bf_hooks \
    test_integration-bf_normalization \
    test_integration-heuristics-bv_ba_custom_simplification \
    test_integration-heuristics-bv_ba_cvc5_simplification \
    test_integration-heuristics-ex_subs_based_elimination \
    test_integration-interpreter \
    test_integration-interpreter_ltl_multiio \
    test_integration-ltl_aba_execution \
    test_integration-nso_rr_execution \
    test_integration-nso_rr_fixed_point \
    test_integration-nso_rr_partial_eval \
    test_integration-quantifiers \
    test_integration-satisfiability1 \
    test_integration-satisfiability2 \
    test_integration-satisfiability3 \
    test_integration-satisfiability4 \
    test_integration-satisfiability5 \
    test_integration-sbf1 \
    test_integration-sbf2 \
    test_integration-semantic \
    test_integration-splitter \
    test_integration-splitter2 \
    test_integration-tau_ba1 \
    test_integration-tau_ba3 \
    test_integration-type_scoped_resolver \
    test_integration-wff_hooks \
    test_integration-wff_normalization; do
    run "$t" "./$t" 180
done

# ── summary ───────────────────────────────────────────────────────────────────
echo ""
echo "=== RESULTS: $PASS passed, $FAIL failed ==="
if [ ${#FAILURES[@]} -gt 0 ]; then
    echo "FAILED:"
    for f in "${FAILURES[@]}"; do echo "  $f"; done
fi
[ $FAIL -eq 0 ]
