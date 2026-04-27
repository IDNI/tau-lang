# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Tau is an expressive, decidable, and executable formal software specification language. It supports satisfiability checking and program synthesis from logical specifications. The codebase is C++23, built with CMake, and targets Linux/Windows.

## Build Commands

Scripts are invoked via `./dev <script> [cmake-options]` from the project root (or directly from `scripts/`). All extra args are forwarded to CMake.

```bash
./dev debug [<CMAKE_OPTIONS>]          # Debug build → build-Debug/
./dev release [<CMAKE_OPTIONS>]        # Release build → build-Release/
./dev relwithdebinfo [<CMAKE_OPTIONS>] # Release+debug info → build-RelWithDebInfo/
./dev clean                            # Remove build directories
./dev regen                            # Regenerate parsers from .tgf grammar files
./dev clang release                    # Build with Clang instead of GCC
```

Key CMake options:
- `-DTAU_BUILD_TESTS=ON` — build all test types
- `-DTAU_BUILD_UNIT_TESTS=ON` — unit tests only
- `-DTAU_BUILD_INTEGRATION=ON` — integration tests only
- `-DTAU_BUILD_EXECUTABLE=ON` — build the `tau` CLI
- `-DTAU_LOG_CHANNELS=ON` — enable debug/trace logging
- `-DTAU_BUILD_JOBS=N` — set parallel build jobs (default: half of CPU cores)

## Running Tests

```bash
./dev test-debug                       # Run all tests in build-Debug/
./dev test-release                     # Run all tests in build-Release/

# Run tests matching a pattern (from build dir)
cd build-Debug && ctest -R <pattern> --output-on-failure

# Run a single compiled test binary directly
./build-Debug/tests/unit/test_bool
./build-Debug/tests/integration/test_integration-satisfiability1
```

Tests use the **doctest** framework (`src/doctest.h`). Test organization uses `TEST_SUITE` / `TEST_CASE` macros. Test helper headers live in `tests/` (e.g., `test_helpers.h`, `test_tau_helpers.h`, `test_integration-satisfiability_helper.h`).

## Architecture

### Boolean Algebras (`src/boolean_algebras/`)

The core abstraction. Multiple BA implementations are supported:
- `bool_ba.h` — classical Boolean algebra
- `sbf_ba.h` — simple Boolean formulas
- `nso_ba.h` — non-deterministic string operations
- `bv_ba.h` — bitvectors (uses CVC5 and/or bitblasting)
- `tau_ba.h` — Tau BA (embedding Tau specs as a BA)
- `bdds/` — BDD-based operations underlying several BAs
- `cvc5/` — CVC5 SMT solver integration for bitvector theory

### Tree Representation (`src/tau_tree*.h`)

`tau_tree<node_t>` is the central AST type. Key files:
- `tau_tree.h` / `tau_tree.tmpl.h` — tree structure
- `tau_tree_builders.h` — constructing trees
- `tau_tree_queries.tmpl.h` — querying/traversing trees
- `tau_tree_printers.tmpl.h` — pretty-printing
- `tau_tree_from_parser.tmpl.h` — parsing → tree conversion

### Satisfiability & Solving (`src/satisfiability.h`, `src/solver.h`)

Checks whether a Tau specification is satisfiable. `solver.h` handles the underlying decision procedures. `normal_forms.h` / `normalizer.h` transform specs into normal forms required by the solver.

### Heuristics (`src/heuristics/`)

Optimization passes applied before/during solving:
- `bv_bitblasting.h` — bitblast bitvector operations to Boolean formulas
- `bv_bitblasting_predicates.tmpl.h` — predicates used during bitblasting
- `bv_ba_simplification.h`, `bv_ba_cvc5_simplification.tmpl.h`, `bv_ba_custom_simplification.tmpl.h` — simplification passes
- `ex_subs_based_elimination.h` — existential substitution elimination

### Parsers (`parser/`)

Grammars are written in `.tgf` format and compiled to `*_parser.generated.h` headers via `./dev regen`. Don't hand-edit generated parser files.

### Public API (`src/api.h`)

The external C++ API. Template specializations live in `api.tmpl.h`, `api.tmpl.string.h`, `api.tmpl.htref.h`.

### Execution & Interpreter (`src/interpreter.h`, `src/repl_evaluator.h`)

`interpreter.h` executes Tau specs and manages the REPL. `repl_evaluator.h` is the execution engine for synthesized programs.

## Code Conventions

- `.tmpl.h` files contain template implementations (included by their corresponding `.h`).
- Task annotations: `TODO`, `DOING`, `IDEA`, `FIXME`, `REVIEW`, `DOCUMENTATION`, `MARK` with priority tags `(IMPORTANT)`, `(HIGH)`, `(MEDIUM)`, `(LOW)`, `(VERY LOW)`.
- External dependencies (CVC5, Boost) are installed to `~/.tau/` by `./dev dep-cvc5.sh` and `./dev dep-boost.sh`.
- The parser library is a git submodule at `external/parser/`.
