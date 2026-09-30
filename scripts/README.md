# Scripts

Helpers for building and testing Tau Language.

Run any script from the project root via `./dev`:

```bash
./dev <SCRIPT> [SCRIPT_OPTIONS...]
```

`./dev help` lists available scripts (everything in `scripts/*.sh`).

Shared build helpers ([`devrc`](../external/parser/scripts/devrc) with
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
| `--keep-cache` | preset only: keep the CMake cache of the build directory instead of a fresh configure |
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
the exported environment. The typed spelling `-DTAU_BUILD_JOBS:STRING=N` is not
stripped, so it also lands in the configure cache. CMake reads
`$ENV{TAU_BUILD_JOBS}` when the cache value is `0`
(see [`CMakeLists.txt`](../CMakeLists.txt)).

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
- `dep-oras` — the pinned oras client, verified by sha256, for reading the remote
  store. CI runs it; a local build needs it only with `TAU_STORE_REMOTE` set.
- `dep-python-venv [-DTAU_PYTHON_ARCH=<arch>]` — the shared Python 3.12.14
  venv the nanobind binding builds against, on Linux, macOS and Windows (Git
  Bash). uv fetches the interpreter, the venv lands in
  `$TAU_SHARED_PREFIX/py312`, and nanobind plus the platform's wheel repair
  tool (auditwheel, delocate or delvewheel) install into it with the venv's
  own pip. The script exports the interpreter as `TAU_PYTHON` to a CI job
  through `$GITHUB_ENV`. A target arch needs binfmt for that arch, which the
  arm64 cross job registers first.
- `dep-*-package` — the store producers configure runs itself (`dep-cvc5-package`,
  `dep-boost-package`, `dep-curl-package`, `dep-spot-package`); called by hand
  only to prefetch. `dep-spot-package` builds the Spot CLI Tau execs, never
  links: MSYS2 UCRT64 g++ on `windows-x86_64-msvc`, the preset's compiler elsewhere, and
  a host with `ltlsynt` on `PATH` skips it entirely.
- `dep-emsdk` — Emscripten SDK into `$TAU_SHARED_PREFIX/emsdk`; a wrapper around
  the parser's own script, so one install serves both repos
- `dep-chrome`, `dep-js-test-deps` — pinned Chrome for Testing and
  `puppeteer-core`, for the browser test suite. A configure with
  `-DTAU_BUILD_BROWSER_TESTS=ON` runs both itself, so they are rarely called by
  hand. Neither needs a system node: emsdk bundles node/npm/npx.
- `binding <BIND_LANG>` — build bindings (currently `python`)
- `compile <artifact-dir> [--preset <name>] [-D NAME=VALUE]... [-G <generator>]`
  — configure, build and copy an artifact directory that `tau gen` emitted,
  through the same `cmake/tau-compile.cmake` script `tau compile` runs. The
  artifact dir is the first argument. Without `--preset` the build is native
  and uses `build/release/sdk`, cmake's compiler and a Release build type.
  `--preset` names a platform or tau preset: the SDK is `build/<platform>/sdk`,
  or the installed box at `lib/tau/sdk/<platform>/lib/cmake/Tau`. `-D` and `-G`
  reach the emitted project configure, and a `-D` value wins over the preset.
  `TAU_SDK_DIR` names the SDK in both cases.

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
./dev preset release-w64-packages
./dev preset release-w64-sdk-packages-deb   # SDK box only, DEB
./dev preset release-w64-sdk-packages-rpm
./dev preset release-wasm-sdk-packages-deb  # the wasm box, DEB
./dev preset release-wasm-sdk-packages-rpm
./dev preset release-arm64-sdk-packages-deb  # the Linux arm64 box, DEB
./dev preset release-arm64-sdk-packages-rpm
./dev preset release-arm64-tests run        # arm64 cross tests, under qemu
./dev preset release-msvc-all-clang-cl run  # clang-cl on the MSVC ABI
./dev preset debug-asan
./dev preset coverage
./dev preset release-wasm                        # wasm library (tau.js/.wasm/.esm.mjs)
./dev preset release-wasm-all-tests run          # C++ suite, REPL suite and js parity, under node
./dev preset release-wasm-all-tests-browser run  # the compiled suite in headless Chrome
./dev preset release-wasm-repl-tests run         # the REPL suite, run under node
./dev preset release-wasm-repl-browser           # wasm browser REPL (tau_repl_web.js)
./dev preset release-wasm-repl-tests-browser run # the REPL suite inside the browser REPL, in Chrome
./dev preset release-wasm-nothreads              # the same library without -pthread
./dev preset devel-wasm-all-tests run            # every wasm preset also has a devel- and a debug- twin
```

Default preset name is `release` if omitted.

Presets whose name contains **`package`** run `cpack -C Release` after build.
**`run`** runs `ctest` for test/all presets, otherwise runs `tau` (args after `--`).

## Building release packages

- `packages` — legacy: DEB then RPM in `build-Release/packages`
- `w64-packages` — legacy: Windows NSIS and ZIP
- Preset: `./dev preset release-packages-deb`, `./dev preset release-packages-rpm`,
  `./dev preset release-w64-packages`

The deb and rpm packages split in two. `tau` runs and interprets specs.
`tau-sdk` compiles specs into programs and links against Tau.

A cross platform has its own SDK package: `tau-sdk-windows-x86_64-mingw`,
`tau-sdk-wasm32-emscripten` and `tau-sdk-linux-arm64`, one box each, no
executable. The presets
`release-w64-sdk-packages-deb`, `release-w64-sdk-packages-rpm`,
`release-wasm-sdk-packages-deb`, `release-wasm-sdk-packages-rpm`,
`release-arm64-sdk-packages-deb` and `release-arm64-sdk-packages-rpm` build and
package those boxes. The name carries `package`, so `./dev preset` runs `cpack`
after the build. The `arm64` box is the Linux arm64 target, cross-compiled from
x86 with clang; the `release-arm64-*` presets need `g++-aarch64-linux-gnu` (and
`qemu-user` to run their tests).

## Testing

- `test <TEST_NAME>` — compile and run one test (auto-selects test type)
- `test-debug`, `test-release`, `test-relwithdebinfo` — build all tests + ctest
  (pass `-DTAU_BUILD_TESTS=ON` via each script; extra `-D` flags forwarded)
- `test-wine` — cross-build and run tests under Wine
- WebAssembly: configure a wasm preset, then `ctest --test-dir build/<folder>
  -j 8` runs every test under node (Emscripten's toolchain sets node as the
  test emulator). The browser presets run their suite in headless Chrome
  instead; see `AGENTS.md`'s WebAssembly section for the full preset table.
- `test-with-tau-testnet [<PRESET>] [<CMAKE_OPTIONS>] [-- <PYTEST_ARGS>]` —
  clone [tau-testnet](https://github.com/IDNI/tau-testnet), build the Python
  binding from the current source, and run tau-testnet's pytest suite against
  it. Downstream mirror of the parser's `test-with-tau`. Defaults to the
  `release-binding-python` preset; any of the `*-binding-python*` presets
  work, and one without the binding is rejected after the build.

  `TAU_TESTNET_DIR` reuses an existing checkout instead of cloning into
  `./tau-testnet`. `TAU_TESTNET_PYTHON` picks the interpreter the venv is
  built from — tau-testnet pins exact dependency versions, and several pin
  no wheel past cp312, so the newest interpreter on the box may not work.

  The image runs the same script with `--build-arg TEST_TAU_TESTNET=yes`,
  after its own test suite. The release workflow sets it for the default
  pack.

  ```bash
  ./dev test-with-tau-testnet
  ./dev test-with-tau-testnet devel-binding-python-tests -DTAU_BUILD_JOBS=10
  ./dev test-with-tau-testnet -- -k test_consensus_time
  TAU_TESTNET_PYTHON=$(uv python find 3.12) ./dev test-with-tau-testnet
  ```

## Benchmarking

- `benchmark`, `bench`, `save-benchmarks`

## Serving the browser REPL

- `tau-repl-serve [port] [build-dir]` — static server with the COOP/COEP headers
  `SharedArrayBuffer` needs and the right `application/wasm` MIME type. Defaults
  to `build/release-wasm-repl-browser`, which the `release-wasm-repl-browser` preset produces.

## Debugging

- `gdb-tau`, `gdb`, `debug-tau`

## Dependency store

Configure resolves each dependency by a content id. The id is the SHA-256 of
the recipe fields, so a changed recipe gives a new id and never a stale hit.

The local store keeps one folder for each entry:

```text
$TAU_SHARED_PREFIX/store/<dep>/<id>/manifest.json
$TAU_SHARED_PREFIX/store/<dep>/<id>/prefix/
$TAU_SHARED_PREFIX/store/<dep>/<id>.lock
```

The store has two tiers:

- The local tier is the folder above. A hit is used as it is.
- The remote tier is the OCI registry that `TAU_STORE_REMOTE` names. It holds
  one private artifact for each entry, tagged `<dep>-<id>`. Configure reads a
  local miss from the remote. When the remote also misses, or when
  `TAU_STORE_REMOTE` is not set, configure builds the dependency.

With `TAU_STORE_PUBLISH=ON` and `TAU_STORE_REMOTE` set, configure pushes each
entry it builds to the remote right after the build. A failed push stops the
configure. An entry from a local hit or a remote pull is not pushed.

`store-publish` sends the local entries to the remote. `TAU_STORE_KEEP` sets
how many entries of each dependency stay in the local tier.

Two files are the single definitions. Do not copy their rules into another
file:

- [`tau-store.cmake`](../external/parser/cmake/tau-store.cmake) defines the
  layout, the lookup and the publication of a local entry.
- [`tau-manifest.cmake`](../external/parser/cmake/tau-manifest.cmake) defines
  the input id, the manifest schema and the manifest check.

## Docker

See [`docker.sh`](docker.sh) — `docker tau`, `docker packages`, `docker w64-*`, …

[`with-gh-token`](with-gh-token) authenticates the cvc5/boost clones and the
oras pull from the remote store when the image build is passed
`--secret id=gh_token`. It is extensionless because only `scripts/*.sh` are
`./dev` subcommands.

## Distributed builds (icecream)

[`icecc-terminal-log`](../external/parser/cmake/use-icecream/icecc-terminal-log) wraps a
command and tails icecc logs to stderr. Enable icecream via
[`CMakeLocalLists.txt`](../CMakeLocalLists.txt) (`use-icecream.cmake`).

```bash
external/parser/cmake/use-icecream/icecc-terminal-log ./dev release
```
