#!/usr/bin/env bash
# Produce the three pinned uninstrumented Spot 2.16 WASI commands.
set -euo pipefail
if [ "$#" != 3 ]; then
    echo "Usage: bash $0 NEW_BUILD_ROOT SPOT_TARBALL WASI_SDK_TARBALL" >&2
    exit 2
fi
tools=$(cd "$(dirname "$0")" && pwd)
root=$(realpath -m "$1") spot=$(realpath "$2") sdk_archive=$(realpath "$3")
verify() { printf '%s  %s\n' "$1" "$2" | sha256sum -c -; }
verify 688463cb2fa393c51d9cf938fb01a716b91e4c8122aeb52fd116a3bbfddab869 "$spot"
verify b761e3a0721dbae9c09a0059e5fdb2bf917d1b4a8a7b430fb3b5aafb0984b2c4 "$sdk_archive"
mkdir "$root"
tar -xzf "$spot" -C "$root"
tar -xzf "$sdk_archive" -C "$root"
src="$root/spot-2.16"
sdk="$root/wasi-sdk-34.0-x86_64-linux"
b="$sdk/bin"
patch -d "$src" -p1 < "$tools/spot-lib-wasi.patch"
patch -d "$src" -p1 < "$tools/spot-cli-wasi.patch"
cflags="-O1 -mllvm -wasm-enable-sjlj -mllvm -wasm-use-legacy-eh=false -D_WASI_EMULATED_SIGNAL -D_WASI_EMULATED_PROCESS_CLOCKS"
cx="$cflags -fwasm-exceptions"
libs="-Wl,--max-memory=67108864 -lwasi-emulated-signal -lwasi-emulated-process-clocks -lwasi-emulated-getpid -lsetjmp"
cd "$src"
CC="$b/clang" CXX="$b/clang++" AR="$b/llvm-ar" RANLIB="$b/llvm-ranlib" \
 CFLAGS="$cflags" CXXFLAGS="$cx" LDFLAGS="$libs" \
 ./configure --host=wasm32-unknown-wasi --build=x86_64-pc-linux-gnu --disable-shared --disable-python --disable-devel --disable-pthread
for part in picosat buddy lib spot/misc spot/priv spot/tl spot/twa spot/kripke spot/twaalgos spot/twacube spot/twacube_algos spot/mc spot/parseaut spot/parsetl; do
 make -j1 -C "$part" CFLAGS="$cflags" CXXFLAGS="$cx -std=c++20"
done
make -j1 -C spot libspot.la CFLAGS="$cflags" CXXFLAGS="$cx -std=c++20"
libs="$libs -L$sdk/share/wasi-sysroot/lib/wasm32-wasip1/eh -lunwind"
make -j1 -C bin ltlfilt autfilt CFLAGS="$cflags" CXXFLAGS="$cx -std=c++20" LDFLAGS="$libs"
make -j1 -C bin ltlsynt CFLAGS="$cflags" CXXFLAGS="$cx -std=c++20" LDFLAGS="$libs"
verify 4ce584bf26375ccc4fe28032280b79e6f64ec8d032c7dd3ab95c719cfeb39d59 bin/ltlfilt
verify 95e7268afb04c9cab21f1c983f32ba2ce78ebc180ca82adf0319c9f8b81cdaed bin/autfilt
verify 3667deef99d4204eaff946489f10287f14915dff0cae4a508d1f6c1467e7f4cc bin/ltlsynt
