# AGENTS.md

This file provides guidance to AI coding agents when working with code in this repository.

`CLAUDE.md` always stays a symlink to `AGENTS.md`; `AGENTS.md` is therefore part of each session's context and is the single source of truth for agent instructions.

## Project Overview

Tau is an expressive, decidable, and executable formal software specification language. It supports satisfiability checking and program synthesis from logical specifications. The codebase is C++23, built with CMake, and targets Linux, Windows, macOS, and WebAssembly (in progress).

## Build Commands

Prefer building via `./dev preset` from the project root. It configures (fresh),
builds, and optionally tests/runs using `CMakePresets.json`. Build output goes
to `build/<preset>` (e.g. `build/debug`, `build/release`).

```bash
./dev preset <PRESET> [run] [<CMAKE_OPTIONS>]

./dev preset debug                     # Debug build → build/debug/
./dev preset release                   # Release build → build/release/
./dev preset debug-tests               # Debug build with all tests
./dev preset release-tests             # Release build with all tests
./dev preset debug-tau                 # Debug build of the tau CLI
./dev preset release-tau               # Release build of the tau CLI
./dev preset relwithdebinfo            # → build/relwithdebinfo/
./dev preset debug-clang               # Clang build → build/debug-clang/
./dev preset debug-asan                # Debug + AddressSanitizer
./dev clean [all]                      # Remove build dirs (all: also build/ preset trees)
./dev regen                            # Regenerate parsers from .tgf grammar files
```

Other presets: `{debug,release}-{ninja,all,measure}`, `relwithdebinfo-{tests,tau,all}`,
`coverage`, `release-packages-{deb,rpm}`, `release-mingw*`. Default preset is
`release` if omitted.

Notes:
- Arguments may appear in any order: preset name, `run`, `-D…`, `-v`,
  `--target NAME`, `-G GENERATOR`, and `--` (args after `--` go to `tau`).
- Appending `run` runs ctest for `*-tests`/`*-all` presets, otherwise runs `tau`.
- Always use the explicit form `./dev preset <name>` — bare `./dev <preset-name>`
  asks an interactive Y/n question and will hang non-interactive shells.
- Legacy wrappers (`./dev debug`, `./dev release`, `./dev test-debug`, … →
  `build-Debug/`, `build-Release/`) still exist but should not be used for new work.

Key CMake options (forwarded from anywhere on the command line):
- `-DTAU_BUILD_TESTS=ON` — build all test types (already ON in `*-tests`/`*-all` presets)
- `-DTAU_BUILD_UNIT_TESTS=ON` — unit tests only
- `-DTAU_BUILD_INTEGRATION=ON` — integration tests only
- `-DTAU_BUILD_EXECUTABLE=ON` — build the `tau` CLI (already ON in `*-tau`/`*-all` presets)
- `-DTAU_LOG_CHANNELS=ON` — enable debug/trace logging
- `-DTAU_BUILD_JOBS=N` — parallel build jobs (resolution: `-D` flag > env var > half of CPU cores)

## Running Tests

```bash
./dev preset debug-tests run           # Build + run all tests in build/debug/
./dev preset release-tests run         # Build + run all tests in build/release/

# Run tests matching a pattern (test presets work from the project root)
ctest --preset debug-tests -R <pattern> --output-on-failure
ctest --preset release-tests -R <pattern> --output-on-failure

# Compile and run a single test by name (auto-selects test type)
./dev test <TEST_NAME>

# Run a single compiled debug test binary directly
./build/debug/tests/unit/test_bool
./build/debug/tests/integration/test_integration-satisfiability1

# Run a single compiled release test binary directly
./build/release/tests/unit/test_bool
./build/release/tests/integration/test_integration-satisfiability1
```

Tests use the **doctest** framework (`src/doctest.h`). Test organization uses `TEST_SUITE` / `TEST_CASE` macros. Test helper headers live in `tests/` (e.g., `test_helpers.h`, `test_tau_helpers.h`, `test_integration-satisfiability_helper.h`).

Always follow those guidelines using `./dev` and `ctest` run tests.
Always run tests in debug. Always run tests in release before pushing.

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
- `bv_predicate_blasting.h` — blast bitvector operations to Boolean formulas with predicates (implementation split into `bv_predicate_blasting{,_logic,_comparisons,_arithmetic}.tmpl.h`)
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
