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

The core abstraction. Every BA is a plugin in its own directory:
- `sbf/` — simple Boolean formulas (BDD-backed)
- `tau/` — Tau BA (embedding Tau specs as a BA)
- `qint/` — rational intervals · `qlt/` — dense linear order (non-aba omcat)
- `hsb/` — half-open polyhedra in R^d · `nlang/` — natural-language BA
- `bv/` — bitvectors (CVC5 and/or bitblasting); the only *parameterized* type
  (`bv[8]` and `bv[16]` are distinct types of one family)
- `bool_ba.h` + `bool_descriptor.tmpl.h` — classical Boolean algebra; described
  like the rest but not a `TAU_BAS` entry, since packs such as
  `node<bv, Bool>` still need it
- `nso_ba.h` — non-deterministic string operations
- `bdds/` — BDD-based operations underlying several BAs
- `cvc5/` — CVC5 SMT solver integration for bitvector theory

What remains flat in `src/boolean_algebras/` is not a BA plugin: the descriptor
interface (`ba_descriptor.h`, `ba_pack_traits.h`), `bool_ba.h` and its
descriptor, and the `nso_ba` / `variant_ba` / `product_ba` combinators. Include
a BA as `boolean_algebras/<id>/<id>[_ba].h` — the flat `<id>.h` paths are gone.

The BAs of a build are its **pack**, chosen at configure time with
`-DTAU_BAS=` (default `tau,qint,qlt,nlang,bv,sbf,hsb`). `cmake/tau_bas.cmake`
globs `src/boolean_algebras/*/ba.cmake` manifests and generates `tau_pack.h`
into the build tree, providing `tau_pack::node_t`, `TAU_PACK_BASE_BAS` and
`TAU_PACK_FULL_BAS` — use those instead of spelling a pack literally.

A BA reaches core only through `ba_descriptor<BA, node>` (`ba_descriptor.h`);
`ba_descriptor_complete` lists the mandatory members. **There are no
hand-written per-pack dispatchers left** — one generic descriptor-driven
`base_ba_dispatcher` serves every pack, default or reduced.

Beyond the mandatory surface, a BA may declare **optional capabilities**, which
core probes with `requires` and never by BA name. Those in use today live in
`ba_pack_traits.h` as `pack_*` folds: `solve`, `is_sat`, `can_solve`,
`sat_status`, `preprocess`/`set_preprocessing`, `zero_constant`,
`value_constant`, `arith_ops`, `non_aba_omcat`, `literal_incomplete`,
`can_host_bool`/`bool_carrier_type`, `print_constant`. The carrier pair feeds
`pack_bool_carrier_type`, the type core builds a plain 0/1 in (an LTL state bit,
a CTL* witness): `can_host_bool` marks a candidate (bv, sbf, Bool),
`bool_carrier_type` names *which* of its types when that is not `type_tree()`
(bv answers `bv[1]`), and the winner is resolved per pack from the
`-DTAU_BOOL_CARRIERS=bv,sbf,bool` preference order. The same carrier renders a
type no BA owns, asked about its own type — do not add a second capability for
that, `bv[1]`'s literals already are `1`/`0`. `print_constant` is how a BA whose
own `operator<<` formats a constant unhelpfully (bv prints SMT-LIB) says how Tau
should render it. Each fold's empty case is chosen deliberately — `pack_solve`
static_asserts (reaching it means a gate drifted), while `pack_zero_constant`
and `pack_type_has_arith_ops` return nullptr/false because "no BA owns this
type" is ordinary. When writing a fold, use `if constexpr` inside a per-element
lambda: a `?:` in a fold expression instantiates both arms for every BA.

**Rewrite hooks are a second, separate mechanism.** `ba_descriptor.h` also
declares `ba_wff_hooks<BA, Node>` and `ba_term_hooks<BA, Node>` — *defined and
empty*, unlike `ba_descriptor`, so a BA specializes neither, one, or both, in
its own `<ba>_ba_hooks_ext.tmpl.h`. Capabilities answer questions; hooks rewrite
trees, so their routing lives with the hooks (`try_wff_*` in `hooks_wff.tmpl.h`,
`try_term_cast` in `hooks_bf.tmpl.h`) rather than in the traits. A comparison
hook needs **two** folds: `try_wff_lt` asks "did some BA fold this", and
`pack_ba_type_has_wff_lt_hook` asks "does the owner define this operator at
all" — when the owner declines, the atom must be preserved with `get_raw`
instead of falling through to the generic Boolean definition.

To add a BA, copy `src/boolean_algebras/_template/` and follow
`docs/adding_base_bas.md`. `scripts/test-external-ba.sh` proves the out-of-tree
path, and `tests/unit/test_ba_descriptor_pack.cpp` is where each BA joins a pack
so its descriptor is type-checked.

Verify BA work against three configurations, not one: the default pack
(`build/devel`, `build/release`), a reduced pack containing bv
(`-DTAU_BAS=sbf,tau,bv`), and one *without* it (`-DTAU_BAS=sbf,tau,qint`) —
the last is the only thing exercising a capability fold's empty case.

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

Optimization passes applied before/during solving. Everything here works on
generic formula trees and names no BA — a pass belonging to one algebra lives
in that plugin instead (see below):
- `ex_subs_based_elimination.h` — existential substitution elimination
- `simplify_using_equality.h` — congruence-closure simplification
- `syntactic_path_simplification.h` — path-wise syntactic simplification
- `trivial_skolem.h` — trivially skolemizable existential blocks

A BA's own passes live under its plugin, e.g.
`src/boolean_algebras/bv/heuristics/`:
- `bv_predicate_blasting.h` — blast bitvector operations to Boolean formulas with predicates (implementation split into `bv_predicate_blasting{,_logic,_comparisons,_arithmetic,_helpers}.tmpl.h`)
- `bv_ba_simplification.h`, `bv_ba_cvc5_simplification.tmpl.h`, `bv_ba_custom_simplification.tmpl.h` — simplification passes

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
