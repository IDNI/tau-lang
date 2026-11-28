# Sourced by the solver-closure build scripts.
: "${D:?missing solver build root}" "${SDK:?missing Emscripten SDK root}" "${EM_CONFIG:?missing Emscripten configuration}" "${EM_CACHE:?missing Emscripten cache}"
export PATH="$SDK/emscripten:$SDK/bin:$PATH"
export EH_FLAGS="-fwasm-exceptions -sWASM_LEGACY_EXCEPTIONS=0"
export TZ=UTC LC_ALL=C PYTHONHASHSEED=0 SOURCE_DATE_EPOCH=1791450180
export CVC_SOURCE_ROOT="$D"
export CPPFLAGS="-ffile-prefix-map=$D=source-root"
