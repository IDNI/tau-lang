#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "$0")/cvc-environment.sh"
cd "$D"
(
 cd src/gmp-6.3.0
 CC=emcc CXX=em++ AR=emar RANLIB=emranlib CC_FOR_BUILD=cc CFLAGS="-O1 -std=gnu99 $CPPFLAGS" CXXFLAGS="-O1 $EH_FLAGS $CPPFLAGS" MAKEINFO=true \
  emconfigure ./configure --prefix="$D/prefix" --disable-assembly --enable-cxx --disable-shared --enable-static --host=wasm32-unknown-emscripten --build=x86_64-pc-linux-gnu
 make -j1
 make install
)
(
 cd src/cadical-rel-2.1.3-elevate
 mkdir -p build
 sed -e "s,@CXX@,$SDK/emscripten/em++," -e "s,@CXXFLAGS@,-O1 -DNDEBUG -DQUIET -DNUNLOCKED -DNCLOSEFROM -DNTRACING -std=c++11 $EH_FLAGS $CPPFLAGS," \
 -e "s,@ROOT@,$PWD," -e "s,@CONTRIB@,no," -e "s,ar rc,$SDK/emscripten/emar rc," makefile.in > build/makefile
 make -C build -j1 libcadical.a
 cp build/libcadical.a "$D/prefix/lib/"
 cp src/cadical.hpp src/tracer.hpp "$D/prefix/include/cadical/"
)
