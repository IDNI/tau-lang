# Build the pinned raw images from source

The reference build ran on Linux x86-64 with GNU tools, Python 3.12.14,
CMake 4.4.4, Ninja 1.13.2, Clang 19.1.7 as the host C/C++ toolchain,
and Node 24.22.3 on PATH.
The qualification used Boost 1.86 headers. A successful build is not
enough: each recipe checks the produced bytes before the product can
enter an image directory. The output of a different compiler version
or build recipe belongs to a separately qualified profile.

## Inputs

- [Emscripten SDK 6.0.12 x86-64 Linux archive](https://storage.googleapis.com/webassembly/emscripten-releases-builds/linux/6e9f6885cb17a3d6c8fd488a5cff730f50aba61d/wasm-binaries.tar.xz), SHA-256
  `7b073a102b07eca5a0fe344037681e53162639f3540432456874c0446a5e8fa7`.
  Use the official archive selected by the [EMS SDK manifest](https://github.com/emscripten-core/emsdk/blob/main/emscripten-releases-tags.json).
  Its `install/` directory contains `bin/` and `emscripten/`.
- [cvc5 source](https://codeload.github.com/cvc5/cvc5/tar.gz/ea1b484fa54bfe56c0f8b3ac90a6e3e2f46441e7)
  at the exact cvc5 1.3.1 commit. The GMP 6.3.0, CaDiCaL and SymFPU
  URLs and digests are those in Tau's `scripts/dep/common/cvc5.sh`.
- [Spot 2.16](https://www.lrde.epita.fr/dload/spot/spot-2.16.tar.gz)
  and the [WASI SDK 34.0](https://github.com/WebAssembly/wasi-sdk/releases/tag/wasi-sdk-34)
  Linux x86-64 archive. Do not substitute the system Spot executable.
- This Tau tree, with its parser submodule initialized. The parser trees
  at `0bb513fbe3fba41aaa898b5a4b7acc7e051723e8` and
  `9070ff9576e9658a9bdfe78f5f130cbbefe32b14` are identical
  (`e0d6b64404d3be6c76b28d69d8ed4fcc31d7074a`).
  The normal Tau dependency store resolves FTXUI, the parser and tgf.
  An existing store may be supplied; none of its binaries is committed.

## Build

From a separate working directory, define `tools` as the absolute path to
this README's parent directory (`extras/tau-metered/tools`). Verify the
SDK archive digest before extraction. Export `SDK` as the absolute path
to its `install/` directory and configure the SDK with an external cache:

```sh
export EM_CONFIG="$SDK/profile-config.py"
export EM_CACHE="$PWD/em-cache"
cat > "$EM_CONFIG" <<'PY'
import os
ROOT_DIR = os.path.dirname(__file__)
BINARYEN_ROOT = ROOT_DIR
LLVM_ROOT = os.path.join(ROOT_DIR, 'bin')
NODE_JS = os.environ['PROFILE_NODE']
PY
export PROFILE_NODE="$(command -v node)"
bash "$tools/rebuild/build-cvc-closure.sh" \
  ./solver ./cvc5.tar.gz ./gmp-6.3.0.tar.bz2 ./cadical.tar.gz ./symfpu.tar.gz
bash "$tools/rebuild/build-tau-worker.sh" \
  /path/to/tau ./worker ./solver /path/to/boost/include ./tau-deps
bash "$tools/rebuild/build-spot-commands.sh" \
  ./spot ./spot-2.16.tar.gz ./wasi-sdk-34.0-x86_64-linux.tar.gz
python3 "$tools/provision-profile.py" ./images \
  --objdump ./spot/wasi-sdk-34.0-x86_64-linux/bin/llvm-objdump \
  --tau ./worker/tau_work_budget.wasm \
  --ltlsynt ./spot/spot-2.16/bin/ltlsynt \
  --ltlfilt ./spot/spot-2.16/bin/ltlfilt \
  --autfilt ./spot/spot-2.16/bin/autfilt
python3 "$tools/provision-profile.py" --check ./images
```

The source packages remain under their respective licenses. These scripts
build locally and do not copy any dependency image into Tau's source tree
or installed C++ SDK.
