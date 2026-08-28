# AGENTS.md

This file provides guidance to AI coding agents when working with code in this repository.

`CLAUDE.md` always stays a symlink to `AGENTS.md`; `AGENTS.md` is therefore part of each session's context and is the single source of truth for agent instructions.

## Project Overview

Tau is an expressive, decidable, and executable formal software specification language. It supports satisfiability checking and program synthesis from logical specifications. The codebase is C++23, built with CMake, and targets Linux, Windows, macOS, and WebAssembly.

## Build Commands

Prefer building via `./dev preset` from the project root. It configures (fresh),
builds, and optionally tests/runs using `CMakePresets.json`. Build output goes
to `build/<preset>` (e.g. `build/debug`, `build/release`).

```bash
./dev preset <PRESET> [run] [<CMAKE_OPTIONS>]

./dev preset devel                     # Fast optimized build → build/devel/  (use for quick checks)
./dev preset devel-tests               # Fast optimized build with all tests
./dev preset release                   # Release build → build/release/      (use for full verification)
./dev preset release-tests             # Release build with all tests
./dev preset devel-tau                 # Fast optimized build of the tau CLI
./dev preset release-tau               # Release build of the tau CLI
./dev preset relwithdebinfo            # → build/relwithdebinfo/
./dev clean [all]                      # Remove stray artifacts; `all` also removes build/ and build-*
./dev regen [build-dir]                # Build the parser generation target (default build/devel)
```

Debug (`-O0`) presets (`debug`, `debug-tests`, `debug-clang`, `debug-asan`, …)
exist but are ONLY for gdb debugging sessions — never use them for building,
verification, or test runs.

Other presets: `{devel,release}-{ninja,all,measure}`, `relwithdebinfo-{tests,tau,all}`,
`coverage`, `release-packages-{deb,rpm}`, `release-mingw*`. Default preset is
`release` if omitted. The emscripten family is documented under
[WebAssembly](#webassembly) below.

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
- `-DTAU_ARTIFACT_PREINST=ON` — pre-instantiate the artifact pack in libTAU.a for faster `tau compile` (defaults to `TAU_BUILD_EXECUTABLE`, sticky once set)
- `-DTAU_LOG_CHANNELS=ON` — enable debug/trace logging
- `-DTAU_BUILD_JOBS=N` — parallel build jobs (resolution: `-D` flag > env var > half of CPU cores)
- WebAssembly-only options are listed under [WebAssembly](#webassembly).

Hashing is done in `uint64_t` regardless of the platform's `size_t` width
(`external/parser/src/utility/hashing.h`), so the mixing is identical on
32-bit targets (wasm32) and 64-bit ones. Hash order is observable in printed
output — compare with `matches_wff_mod_and_or` / `matches_bf_mod_and_or`
(`tests/test_helpers.h`) rather than pinning a spelling.

## Running Tests

```bash
./dev preset devel-tests               # Fast check: build tests in build/devel/ (run single tests while iterating)
./dev preset release-tests run         # Full verification: build + run all tests in build/release/

# Run tests matching a pattern (test presets work from the project root)
ctest --preset devel-tests -R <pattern> --output-on-failure
ctest --preset release-tests -R <pattern> --output-on-failure

# Compile and run a single test by name (auto-selects test type)
./dev test <TEST_NAME>

# Run a single compiled devel test binary directly
./build/devel/tests/unit/test_bool
./build/devel/tests/integration/test_integration-satisfiability1

# Run a single compiled release test binary directly
./build/release/tests/unit/test_bool
./build/release/tests/integration/test_integration-satisfiability1
```

Tests use the **doctest** framework (`src/doctest.h`). Test organization uses `TEST_SUITE` / `TEST_CASE` macros. Test helper headers live in `tests/` (e.g., `test_helpers.h`, `test_tau_helpers.h`, `test_integration-satisfiability_helper.h`).

Always follow those guidelines using `./dev` and `ctest` to run tests.
Workflow: `devel` is only for a quick build check and for iterating during
development (single tests). When the work is finished, run the FULL test
suite once in `release` before committing — do not run the full suite in
devel first. Never use debug (`-O0`) builds or test runs for verification;
debug exists only for gdb.

## WebAssembly

Tau builds for wasm through Emscripten as three separate artifacts: a **library**
(`tau.js`/`tau.wasm`, an embind wrapper over `src/api.h`), **the test suite**, and
**the REPL** (`tau_repl.js`, FTXUI over xterm.js). They are separate because their
link requirements conflict — see the constraints below.

```bash
./dev dep-emsdk.sh                                  # emsdk → $TAU_SHARED_PREFIX/emsdk

./dev preset emscripten                             # tau.js + tau.wasm + tau.esm.mjs
node build/emscripten/tau.node.js                   # smoke test
node bindings/js/tests/parity.js                    # wasm vs native, 140 checks

./dev preset debug-emscripten-tests                 # tau's own suite for wasm
ctest --test-dir build/debug-emscripten-tests -j 8  # runs each test under node

./dev preset emscripten-pthread                     # tau_repl.js (needs pthreads)
./dev tau-repl-serve [port] [build-dir]             # serve the REPL page
```

Options:
- `-DTAU_BUILD_EMSCRIPTEN=ON` — selects the toolchain; set by the presets. It gates
  `include(use-emscripten)` before `project()`, which is why no preset names a
  `toolchainFile` (a preset-supplied one never loads at that point).
- `-DTAU_BUILD_BINDING_JS=ON` — the embind library, mirroring `TAU_BUILD_BINDING_PYTHON`.
- `-DTAU_BUILD_BROWSER_TESTS=ON` — runs the wasm suite in headless Chrome as the
  `browser_suite` ctest entry. Configure installs Chrome and `puppeteer-core` itself
  via `dep-chrome.sh`/`dep-js-test-deps.sh`. Emscripten-only; fatal otherwise.
- `-DTAU_BUILD_REPL_WASM=ON` — `tau_repl.js`. Emscripten-only; fatal otherwise.

Four constraints, each of which has broken a build here:
- **The pack is `sbf,tau,qint,qlt`.** `bv`/`hsb` need cvc5 and `nlang` needs curl,
  neither of which is ported. So the wasm build is permanently the "pack without
  `bv`" configuration that exercises a capability fold's empty case.
- **The library must stay pthread-free.** pthreads mean `SharedArrayBuffer`, which
  means the embedding page needs COOP/COEP, which would stop `tau.js` being
  droppable on an arbitrary host. Only the REPL is `-pthread`, because FTXUI's
  `ScreenInteractive::Install()` spawns threads with no single-threaded fallback;
  it also needs `-sJSPI`, for the `emscripten_sleep()` in FTXUI's input loop.
- **Every linked object must agree on the exception encoding.** Tau builds with
  `-fwasm-exceptions -sWASM_LEGACY_EXCEPTIONS=0`; a Boost dist built the other way
  fails `wasm-ld` with undefined `__cpp_exception`. `dep-boost.sh` stamps each wasm
  dist with the encoding that produced it and rebuilds on a mismatch — b2 will not
  otherwise notice a flag change, since its dependency tracking is mtime-based.
- **Values that must agree across platforms need a fixed width.** wasm32 is the only
  32-bit target here, and `size_t` is a word size, not a width. `tau_tree.h`'s node
  word and `bintree::hash` are `uint64_t` for this reason; do not "simplify" them.

The suite is **73 of 107** non-REPL tests — the rest need the missing algebras. The
1493 REPL tests are not built for wasm (they would turn on `TAU_BUILD_EXECUTABLE`
and collide with `tau.js`), so the compiled suites that natively defer to them are
built for wasm instead.

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

What remains flat in `src/boolean_algebras/` is not a BA plugin: the descriptor
interface (`ba_descriptor.h`, `ba_pack_traits.h`), `bool_ba.h` and its
descriptor, and the `nso_ba` / `variant_ba` / `product_ba` combinators. Include
a BA as `boolean_algebras/<id>/<id>[_ba].h` — the flat `<id>.h` paths are gone.

### Backends (`src/backends/`)

What algebras stand on, and no algebra itself — no manifest, no descriptor, no
`TAU_BAS` id:
- `bdds/` — the BDD library sbf is built on, plus `var_dict`, its variable
  symbol table; core's `tau_bdd.h` uses it directly, so it is in `tau.h`
- `cvc5/` — the CVC5 wrapper bv and hsb share; deliberately **out** of `tau.h`,
  so an out-of-tree BA needs no CVC5 headers to include it

A backend is compiled and linked only when a pack member asks for it, through
that BA's `TAU_BA_REQUIRES_PACKAGES` / `TAU_BA_LINK_LIBS`.

The BAs of a build are its **pack**, chosen at configure time with
`-DTAU_BAS=` (default `tau,qint,qlt,nlang,bv,sbf,hsb`). `cmake/tau_bas.cmake`
globs `src/boolean_algebras/*/ba.cmake` manifests and generates `tau_pack.h`
into the build tree, providing `tau_pack::node_t`, `TAU_PACK_BASE_BAS` and
`TAU_PACK_FULL_BAS` — use those instead of spelling a pack literally.

Which type names that pack owns is answered by `node::ba::types()` (owning
strings), `type_names()` and `types_joined()` (both `constexpr`, from the
descriptors' `type_name`). Core validates a spec's annotations against them
through `pack_owns_ba_type[_name]` in `ba_types.h`, matching by family name so
`bv[8]` needs `bv` in the pack; the type ids core reserves for itself
(`tau_type_id`, `nat_type_id`, `untyped_type_id`) are exempt, and no BA is
named to do it.

A BA reaches core only through `ba_descriptor<BA, node>` (`ba_descriptor.h`);
`ba_descriptor_complete` lists the mandatory members. **There are no
hand-written per-pack dispatchers left** — one generic descriptor-driven
`base_ba_dispatcher` serves every pack, default or reduced.

Beyond the mandatory surface, a BA may declare **optional capabilities**, which
core probes with `requires` and never by BA name. Those in use today live in
`ba_pack_traits.h` as `pack_*` folds: `solve`, `is_sat`, `can_solve`,
`sat_status`, `preprocess`/`set_preprocessing`, `zero_constant`,
`value_constant`, `arith_ops`, `non_aba_omcat`, `literal_incomplete`,
`can_host_bool`/`bool_carrier_type`, `print_constant`, `uses_oracle`. The last
is declared by nlang alone — deciding a question there leaves the process, so
generic checks that compare two constants sit it out; absent means decided
in-process, which is what every other algebra says by saying nothing. The
carrier pair feeds
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
`docs/adding_base_bas.md`. A manifest declares everything the plugin owns —
sources, its grammar (`TAU_BA_GRAMMAR`) and its suites (`TAU_BA_TESTS`, with
`TAU_BA_TEST_REQUIRES_<target>` for a suite needing another algebra) — so no
central list names an algebra. `tests/unit/test_ba_descriptor_pack.cpp` is where
each BA joins a pack so its descriptor is type-checked, and
`tests/unit/test_ba_conformance.cpp` runs one battery against every algebra of
the configured pack, checking that the descriptor behaves rather than merely
compiles. `scripts/test-external-ba.sh` proves the out-of-tree path end to end:
it builds every suite that pack can run and checks the registered algebra
against the same contract.

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

### Parsers

Grammars are written in `.tgf`: the language grammar in `parser/`, each BA's in
its own plugin (`src/boolean_algebras/<id>/parser/<id>.tgf`, declared by the
manifest's `TAU_BA_GRAMMAR`). The build generates their parsers with the `tgf`
tool it builds itself, into `<build>/generated/` — core's beside the include
path, a BA's under `boolean_algebras/<id>/parser/`. Nothing generated is
committed, so editing a `.tgf` and building is the whole loop; `./dev regen`
only builds the `tau_parsers` target of an existing build directory. Only the
configured pack's grammars are generated.

### Public API (`src/api.h`)

The external C++ API. Template specializations live in `api.tmpl.h`, `api.tmpl.string.h`, `api.tmpl.htref.h`.

### Execution & Interpreter (`src/interpreter.h`, `src/repl_evaluator.h`)

`interpreter.h` executes Tau specs and manages the REPL. `repl_evaluator.h` is the execution engine for synthesized programs.

## Code Conventions

- `.tmpl.h` files contain template implementations (included by their corresponding `.h`).
- Task annotations: `TODO`, `DOING`, `IDEA`, `FIXME`, `REVIEW`, `DOCUMENTATION`, `MARK` with priority tags `(IMPORTANT)`, `(HIGH)`, `(MEDIUM)`, `(LOW)`, `(VERY LOW)`.
- External dependencies (CVC5, Boost) are installed to `~/.tau/` by `./dev dep-cvc5.sh` and `./dev dep-boost.sh`.
- The parser library is a git submodule at `external/parser/`.

### Comments

- Comment only what the code cannot state itself. If the line below already
  says it, delete the comment. One line where one will do.
- Describe the code as it stands — never what it replaced, which alternative
  was rejected, or the debugging that led here.
- Do prefer a comment where a reader could undo something by accident: a flag
  that must not be removed, an ordering requirement, a non-obvious platform or
  tool behaviour.
- **Never cite an identifier that does not exist.** `[SHAPE-Q-NN]` (in
  `tests/unit/parser/`), `AP-N` and `BA-N` are real test-case id conventions;
  do not invent lookalikes for code that has no id. A reference that resolves
  to nothing costs the next reader more than no reference at all.
