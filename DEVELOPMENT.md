![The TAU logo](/docs/tau_logo.svg)

Tau advances formal methods by removing coding from the process, while expanding its industrial capability, reliability and ease of maintenance. Tau enables you to automatically create the most complex, provably correct software, by simply writing sentences about what you want the software to do.

Tau Language is able to embed and extend the most powerful decidable knowledge representation languages, logics, and Boolean algebras to describe states in the specification.

This document covers general remarks for the development and testing of the Tau Language and the Tau Language API.

# General remarks

Throughout all the code we use the following tags to denote different types of tasks or notes: `TODO`, `DOING`, `IDEA`, `FIXME`, `REVIEW`, `DOCUMENTATION`, and `MARK`. A priority subtag is added in parentheses:

- `(IMPORTANT)` — main tasks of the project
- `(HIGH)` — important missing functionality or improvements
- `(MEDIUM)` — code quality and testing improvements (not missing functionality)
- `(LOW)` — minor quality improvements
- `(VERY LOW)` — lowest priority polish

# Building

Check the `README.md` for full platform-specific instructions. Quick build summary:

## Release build

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DTAU_BUILD_TESTS=ON \
  -DCVC5_DIR="$HOME/.tau/cvc5/dist/lib/cmake/cvc5"
make -j$(nproc)
```

## Debug build

```bash
mkdir -p build-debug && cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DTAU_BUILD_TESTS=ON \
  -DCVC5_DIR="$HOME/.tau/cvc5/dist/lib/cmake/cvc5"
make -j$(nproc)
```

Debug builds enable assertions, disable caching (`TAU_CACHE=OFF`), and
disable benchmarks. The `DEBUG` preprocessor macro is defined, which
activates additional logging, `DBG(assert(...))` checks, and symbol
demangling via `cxxabi.h`.

## Other build types

- **`RelWithDebInfo`** — enables `TAU_CACHE`, builds benchmarks.
- **`Coverage`** — instruments with `--coverage` for `lcov`/`genhtml` reports. Run `make coverage` after `ctest` to generate HTML reports.

## Useful CMake options

| Option | Default | Description |
|--------|---------|-------------|
| `TAU_BUILD_TESTS` | OFF | Build all test targets (unit, integration, API, REPL) |
| `TAU_BUILD_EXECUTABLE` | ON (if nothing else selected) | Build the `tau` REPL/interpreter |
| `TAU_BUILD_STATIC_LIBRARY` | OFF | Build `libTAU.a` |
| `TAU_BUILD_SHARED_LIBRARY` | OFF | Build `libTAU.so` |
| `TAU_BUILD_BINDING_PYTHON_NANOBIND` | OFF | Build the nanobind Python binding |
| `TAU_BUILD_BINDING_PYTHON_CTYPE` | OFF | Build the ctypes Python C ABI shim |
| `TAU_USE_CCACHE` | OFF | Use ccache for faster rebuilds |
| `TAU_ADDRESS_SANITIZER` | OFF | Build with ASan |
| `TAU_LOG_CHANNELS` | OFF | Enable debug/trace logging channels |

# Executables

The following executables are built from `src/CMakeLists.txt`:

- **`tau`** — the main REPL and interpreter. Reads specs from files or stdin, runs the interactive REPL with command history, normalization, satisfiability checking, and specification execution.
- **`tau compile`** — the ahead-of-time compiler is a verb of the `tau` binary, not a separate executable. `tau compile spec.tau [-o exe] [-c c++-compiler]` synthesizes the spec, emits `spec.tau.build/` (a driver `main.cpp` plus a `CMakeLists.txt` linking the emitting build's libTAU) and builds a standalone executable that steps the strategy like `tau spec.tau` does. Emitting a C++ header with the synthesized class is the library API (`build_program_desc` + `emit_program`, `src/cpp_codegen.h`).

# Testing

Run the full test suite:

```bash
cd build && ctest --output-on-failure -j$(nproc)
```

Individual test categories can be enabled separately via `TAU_BUILD_UNIT_TESTS`, `TAU_BUILD_INTEGRATION`, `TAU_BUILD_API_TESTS`, `TAU_BUILD_REPL_TESTS`, and `TAU_BUILD_BENCHMARK`.

The test binary `tau_eval` (in `tests/unit/`) is a reference evaluator used by integration tests for formula evaluation and realizability checks.

# Architecture

See the "Architecture" section of [AGENTS.md](AGENTS.md) for the module map, and `docs/tau_tree.md` for the tree representation.

# Contributing

See `docs/adding_base_bas.md` for how to add a new Boolean algebra, and the "Code Conventions" section of [AGENTS.md](AGENTS.md) for the annotations and file layout conventions.
