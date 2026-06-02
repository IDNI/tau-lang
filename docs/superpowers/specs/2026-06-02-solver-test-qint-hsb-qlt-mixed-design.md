# Design: Integration Solver Tests for qint, hsb, qlt, and mixed

**Date:** 2026-06-02
**Branch:** feature/test-solver

## Goal

Add four integration solver test files to `tests/integration/solver/`, one per BA (qint, hsb, qlt) and one for mixed-BA formulas. All follow the existing sbf_ba style: test the general solver framework (find_solution, solve_inequality_system, solve_system, solve) using Tau wff formulas with BA-typed constants.

---

## Files

| Action | Path |
|--------|------|
| Create | `tests/integration/solver/test_integration-solver-qint.cpp` |
| Create | `tests/integration/solver/test_integration-solver-hsb.cpp` |
| Create | `tests/integration/solver/test_integration-solver-qlt.cpp` |
| Create | `tests/integration/solver/test_integration-solver-mixed.cpp` |
| Modify | `tests/integration/solver/CMakeLists.txt` |

---

## CMake

Add `qint hsb qlt mixed` to the `TESTS` list in `tests/integration/solver/CMakeLists.txt`. No other CMake changes needed — the existing `target_include_directories` pattern applies to each new target.

---

## Test Suite Structure

### Per-BA files (qint, hsb, qlt)

Each file has five test suites:

1. **`configuration`** — `bdd_init<Bool>()`
2. **`find_solution`** — 3–4 cases: one var, two vars, complement, xor-style
3. **`solve_inequality_system`** — 3–4 cases: same-exponent, different-exponent, splitter cases
4. **`solve_system`** — 3–4 cases: equality + inequality combinations
5. **`solve`** — 3–4 cases: general/min/max modes + one unsatisfiable case

All suites use `node_t` from `test_tau_helpers.h` (the full bas_pack including all BAs).

### Mixed file

Has four test suites:

1. **`configuration`** — `bdd_init<Bool>()`
2. **`find_solution`** — 2–3 cases with two BA types in one formula (qint+qlt, qint+hsb, qlt+hsb)
3. **`solve_system`** — 2–3 cases: mixed-BA equality + inequality
4. **`solve`** — 2–3 cases general mode + one unsatisfiable

---

## Formula Patterns

### qint constants
```
{[0,1)}:qint          # proper interval (non-trivial element)
{[1,2)}:qint          # another interval
{[0,1)}:qint'         # complement of [0,1) — covers (-inf,0) | [1,+inf)
```

### hsb constants
```
{x[0] < 0}:hsb        # halfspace (strict, x[0] negative)
{x[0] <= 0}:hsb       # halfspace (non-strict)
{x[0] < 0}:hsb'       # complement halfspace
```

### qlt constants
```
{(0,1)}:qlt           # open interval
{[0,1)}:qlt           # half-open interval
{(0,1)}:qlt'          # complement
```

### Mixed constants (two BAs in one formula)
```
{[0,1)}:qint x | {(0,1)}:qlt y = 0.       # qint + qlt
{x[0] < 0}:hsb x | {[0,1)}:qint y = 0.   # hsb + qint
{(0,1)}:qlt x | {x[0] < 0}:hsb y = 0.    # qlt + hsb
```

---

## Solver Options

For suites using `solve_minterm_system`, `solve_inequality_system`, and `solve_system`:
```cpp
solver_options options = {
    .splitter_one = splitter_one_<ba>(),   // BA-specific splitter_one tref
    .mode = solver_mode::general
};
```

For the `solve` suite, `splitter_one` is obtained via:
```cpp
node_t::ba::splitter_one(qint_type<node_t>())   // for qint
node_t::ba::splitter_one(hsb_type<node_t>())    // for hsb
node_t::ba::splitter_one(qlt_type<node_t>())    // for qlt
```

For the mixed file, `tau_type<node_t>()` is used as the type argument.

---

## Excluded Suites

`minterm_iterator`, `minterm_range`, `minterm_inequality_system_iterator`, `minterm_inequality_system_range` — these test the framework internals, not BA-specific behavior, and are already covered by `test_integration-solver-sbf_ba.cpp`.

---

## Verification

Build: `./dev debug -DTAU_BUILD_TESTS=ON`
Run: `ctest -R "test_integration-solver-(qint|hsb|qlt|mixed)" --output-on-failure`
Expected: 4/4 pass.
