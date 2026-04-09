# Scripts

Helpers for building and testing Tau.

These scripts are also available as `./dev <SCRIPT> <SCRIPT_OPTIONS>` from project's root directory.

## Cleaning

- `clean` removes build directories.

## Regenerating parsers

- `regen` regenerates parsers from grammar files in `parser` directory

## Building

Building scripts run cmake with different options. Additional script options provided to `./dev <script>` are passed as additional arguments to cmake. 

- `build [<BUILD_TYPE> [<CMAKE_OPTIONS>]]` builds Tau Parser Library
- `debug [<CMAKE_OPTIONS>]`, `release [<CMAKE_OPTIONS>]` and `relwithdebinfo [<CMAKE_OPTIONS>]` call `build` script with `Debug`, `Release`, `RelWithDebInfo` BUILD_TYPE respectively
- `w64-debug [<CMAKE_OPTIONS>]` and `w64-release [<CMAKE_OPTIONS>]` build for windows using mingw-w64
- `boost-mingw` builds boost for windows using mingw-w64 (a prerequisite for `w64-debug` and `w64-release`)
- `binding <BIND_LANG>` builds binding for BIND_LANG. Currently, only `python` is supported.
- `dep-boost` and `dep-cvc5` build Boost and CVC5 dependencies.

### Building with clang compiler

- `clang <SCRIPT> <SCRIPT_OPTIONS>` works as a prefix argument for build scripts
  Example: `./dev clang release` would run `./dev release` build using clang compiler 

### Build directories

`debug` and `w64-debug` builds are using `build-Debug` directory.

`release` and `w64-release` builds are using `build-Release` directory.

`relwithdebinfo` build is using `build-RelWithDebInfo` directory.

## Benchmarking

- `benchmark [<CMAKE_OPTIONS>]` calls `build` script with `RelWithDebInfo` BUILD_TYPE
- `save-benchmarks` moves benchmark results to `tests/benchmark/data` directory
- `bench [<OPTIONS>] [<PROFILE>]` benchmarks fixture specs and stores measuring into PROFILE 

## Coverage

- `coverage [<CMAKE_OPTIONS>]` calls `build` script with `Coverage` BUILD_TYPE and outputs the report to build-Coverage

## Building release packages

- `packages [<CMAKE_OPTIONS>]` builds packages for linux
- `w64-packages [<CMAKE_OPTIONS>]` builds packages for windows

Packages are created in `build-Release/packages` directory.

## Testing

- `test <TEST_NAME>` compiles a required type of tests and runs the test TEST_NAME
- `test-debug` compiles all tests with `Debug` BUILD_TYPE and runs ctest
- `test-release` compiles all tests with  `Release` BUILD_TYPE and runs ctest
- `test-relwithdebinfo` runs ctest with `RelWithDebInfo` BUILD_TYPE
- `test-wine` runs tests with wine

## Debugging

- `gdb-tau [<TAU_OPTIONS>]` compiles tau and runs it in gdb debugger
- `gdb <TEST_NAME>` compiles a required type of tests and runs the test TEST_NAME in gdb debugger

## Docker

`docker <ACTION> <DOCKER OPTIONS>`

- `docker tau` builds tau runner image with Tau executable and runs it in interactive mode
- `dockewr tau-rpm` builds tau rpm-based runner image with Tau executable and runs it in interactive mode
- `docker run -t <IMAGE>` runs docker image in interactive mode
- `docker bash -t <IMAGE>` runs bash in docker image in interactive mode
- `docker build` default build (default options with no target)
- `docker base` builds base image
- `docker deps` builds deps image
- `docker build-debug` builds build image with `Debug` BUILD_TYPE and tests
- `docker build-release` builds build image with `Release` BUILD_TYPE and tests
- `docker packages` builds packages image and extracts them to `~/.tau/packages`
- `docker nightly` builds nightly packages image and extracts them to `~/.tau/packages`
- `docker w64-deps` builds Windows deps image
- `docker w64-build` builds Windows build image
- `docker w64-packages` builds Windows packages and extracts them to `~/.tau/packages`
- `docker w64-nightly` builds Windows nightly packages and extracts them to `~/.tau/packages`
