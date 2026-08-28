# Scripts

Helpers for building and testing Tau Language.

Run any script from the project root via `./dev`:

```bash
./dev <SCRIPT> [SCRIPT_OPTIONS...]
```

`./dev help` lists available scripts (everything in `scripts/*.sh`).

Shared build helpers ([`devrc`](../../external/parser/scripts/devrc) with
`normalize_args`, `dev_entry`, `build_entry`, `preset_entry`, `test_entry`)
live in the parser submodule at `external/parser/scripts/`.

## How `./dev` handles arguments

[`dev`](../dev) runs `scripts/*.sh`. CMake drivers [`build.sh`](build.sh) and
[`preset.sh`](preset.sh) call `build_entry` / `preset_entry`, which invoke
`dev_entry` → `normalize_args` (single parse) and `resolve_jobs`.

All of the following may appear **in any order** on the command line:

| Token | Effect |
|-------|--------|
| `-D…` | CMake definition (passed to configure/build via `DEV_CMAKE`) |
| `Debug` / `Release` / `RelWithDebInfo` / `Coverage` | legacy build type |
| `-v` | verbose build |
| `--target NAME` | build only this target |
| `-G NAME` | sets `GENERATOR` (not `DEV_CMAKE`); legacy build defaults to Ninja; preset uses preset generator unless `-G` is passed |
| preset name (e.g. `release-tests`) | preset to configure |
| `run` | after preset build: run tests or `tau` |
| `--` | start of program args (e.g. for `tau`) |

Examples:

```bash
./dev preset release-tests run -DTAU_BUILD_TESTS=ON
./dev preset -DTAU_BUILD_TESTS=ON release-tests run
./dev build -v Debug --target test_bool -DTAU_BUILD_UNIT_TESTS=ON
./dev debug --target test_bool -DTAU_BUILD_UNIT_TESTS=ON
```

Use `--` to pass arguments to `tau` when using presets:

```bash
./dev preset release-tau run -- --help
```

## Parallel build jobs (`TAU_BUILD_JOBS`)

Resolution order:

1. `-DTAU_BUILD_JOBS=N` on the command line (any position)
2. `TAU_BUILD_JOBS` already in the environment
3. Half of detected logical CPU cores (auto)

`-DTAU_BUILD_JOBS` is stripped before `cmake --preset`; the value is applied via
the exported environment. CMake also reads `$ENV{TAU_BUILD_JOBS}` when the cache
value is `0` (see [`CMakeLists.txt`](../CMakeLists.txt)).

## Dual build directories

| Path | Used by | Example |
|------|---------|---------|
| `build-${BUILD_TYPE}` | [`build.sh`](build.sh), `debug`, `release`, `packages`, … | `build-Release`, `build-Debug` |
| `build/<lowercase>` | [`preset.sh`](preset.sh), `cmake --preset` | `build/release`, `build/debug` |

Legacy wrappers are unchanged. Prefer presets for new work.

## Cleaning

- `clean [all]` — remove stray artifacts (`tau-config.cmake`, `Testing`); with
  `all`, also the build trees (`build/` and `build-*`). A bare `clean` never
  destroys a build tree, matching `external/parser/scripts/clean.sh`.

## Regenerating parsers

- `regen` — regenerate parsers from grammar files in `parser/`

## Building

- `build [<BUILD_TYPE>] [-v] [--target NAME] [-G GENERATOR] [<CMAKE_OPTIONS>]`
- `debug`, `release`, `relwithdebinfo`, `coverage` — shorthand for `build`
- `w64-debug`, `w64-release` — Windows cross-build (MinGW toolchain from parser)
- `clang <SCRIPT> …` — prefix any build script with clang compilers
- `dep-boost`, `dep-cvc5` — build dependencies into `~/.tau`
- `dep-emsdk` — Emscripten SDK into `$TAU_SHARED_PREFIX/emsdk`; a wrapper around
  the parser's own script, so one install serves both repos
- `dep-chrome`, `dep-js-test-deps` — pinned Chrome for Testing and
  `puppeteer-core`, for the browser test suite. A configure with
  `-DTAU_BUILD_BROWSER_TESTS=ON` runs both itself, so they are rarely called by
  hand. Neither needs a system node: emsdk bundles node/npm/npx.
- `dep-boost --emscripten [-DTAU_BOOST_PTHREAD=ON]` — the wasm Boost dists. Two
  exist because the REPL links `-pthread` and the library must not:
  `dist-wasm` (`threading=single`) and `dist-wasm-pthread`
- `binding <BIND_LANG>` — build bindings (currently `python`)

Build flags for legacy `build.sh`: `-v` (verbose), `--target NAME`, `-G GENERATOR`.

### CMake presets

`preset [<PRESET>] [run] [<CMAKE_OPTIONS>]` — configure (fresh), build, and
optionally test or run `tau` via [`CMakePresets.json`](../CMakePresets.json).

```bash
./dev preset release-tests run
./dev preset release-all run
./dev preset release-tau run -- --help
./dev preset release-packages-deb
./dev preset release-packages-rpm
./dev preset release-mingw-packages
./dev preset debug-asan
./dev preset coverage
./dev preset emscripten                 # wasm library (tau.js/.wasm/.esm.mjs)
./dev preset debug-emscripten-tests     # tau's own suite, run under node
./dev preset emscripten-pthread         # wasm REPL (tau_repl.js)
```

Default preset name is `release` if omitted.

Presets whose name contains **`package`** run `cpack -C Release` after build.
**`run`** runs `ctest` for test/all presets, otherwise runs `tau` (args after `--`).

## Building release packages

- `packages` — legacy: DEB then RPM in `build-Release/packages`
- `w64-packages` — legacy: Windows NSIS and ZIP
- Preset: `./dev preset release-packages-deb`, `./dev preset release-packages-rpm`,
  `./dev preset release-mingw-packages`

## Testing

- `test <TEST_NAME>` — compile and run one test (auto-selects test type)
- `test-debug`, `test-release`, `test-relwithdebinfo` — build all tests + ctest
  (pass `-DTAU_BUILD_TESTS=ON` via each script; extra `-D` flags forwarded)
- `test-wine` — cross-build and run tests under Wine
- WebAssembly: configure the `debug-emscripten-tests` preset, then
  `ctest --test-dir build/debug-emscripten-tests -j 8` runs every test under
  node. Adding `-DTAU_BUILD_BROWSER_TESTS=ON` registers `browser_suite`, which
  drives the same binaries in headless Chrome. See the WebAssembly section of
  [`AGENTS.md`](../AGENTS.md) for the constraints.

## Benchmarking

- `benchmark`, `bench`, `save-benchmarks`

## Serving the browser REPL

- `tau-repl-serve [port] [build-dir]` — static server with the COOP/COEP headers
  `SharedArrayBuffer` needs and the right `application/wasm` MIME type. Defaults
  to `build/emscripten-pthread`, which the `emscripten-pthread` preset produces.

## Debugging

- `gdb-tau`, `gdb`, `debug-tau`

## Docker

See [`docker.sh`](docker.sh) — `docker tau`, `docker packages`, `docker w64-*`, …

## Distributed builds (icecream)

[`icecc-terminal-log`](../../external/parser/scripts/icecc-terminal-log) wraps a
command and tails icecc logs to stderr. Enable icecream via
[`CMakeLocalLists.txt`](../CMakeLocalLists.txt) (`use-icecream.cmake`).

```bash
external/parser/scripts/icecc-terminal-log ./dev release
```
