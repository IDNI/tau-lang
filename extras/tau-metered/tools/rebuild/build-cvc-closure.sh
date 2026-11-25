#!/usr/bin/env bash
# Build the fixed Emscripten solver closure from caller-supplied source archives.
set -euo pipefail
if [ "$#" != 5 ]; then
    echo "Usage: bash $0 NEW_BUILD_ROOT CVC5_RELEASE_TARBALL GMP_TARBALL CADICAL_TARBALL SYMFPU_TARBALL" >&2
    exit 2
fi
tools=$(cd "$(dirname "$0")" && pwd)
D=$(realpath -m "$1")
cvc=$(realpath "$2") gmp=$(realpath "$3") cadical=$(realpath "$4") symfpu=$(realpath "$5")
verify() { printf '%s  %s\n' "$1" "$2" | sha256sum -c -; }
verify fb3d90b60378ef7cda9d05b7cc239fe721c0c2d79325b24b1a66651512fe48ec "$cvc"
verify ac28211a7cfb609bae2e2c8d6058d66c8fe96434f740cf6fe2e47b000d1c20cb "$gmp"
verify 15e1e82f7f9a9da0e97070cb8ac41d5b32139f65d54f72d2ff84849b0466ef92 "$cadical"
verify 823aa663fcc2f6844ae5e9ea83ceda4ed393cdb3dadefce9b3c7c41cd0f4f702 "$symfpu"
export D
source "$tools/cvc-environment.sh"
mkdir "$D"
mkdir -p "$D/src/cvc5" "$D/prefix/include/"{symfpu,cadical}
tar -xzf "$cvc" --strip-components=1 -C "$D/src/cvc5"
tar -xjf "$gmp" -C "$D/src"
tar -xzf "$cadical" -C "$D/src"
tar -xzf "$symfpu" --strip-components=1 -C "$D/prefix/include/symfpu"
python3 - "$D" <<'PY'
from pathlib import Path
import sys
p=Path(sys.argv[1]) / 'src/cadical-rel-2.1.3-elevate/scripts/make-build-header.sh'
s=p.read_text()
old='DATE="`date 2>/dev/null|sed -e \'s,  *, ,g\'`"\nOS="`uname -srmn 2>/dev/null`"'
new='DATE="$(date -u -d "@$SOURCE_DATE_EPOCH" "+%Y-%m-%dT%H:%M:%SZ")"\nOS="wasm32-unknown-emscripten"'
if s.count(old) != 1:
    raise SystemExit('CaDiCaL build metadata signature changed')
p.write_text(s.replace(old,new))
PY
python3 "$tools/normalize-cadical-flags.py" "$D"
bash "$tools/compile-cvc-deps.sh"
bash "$tools/compile-cvc5.sh" 1
cd "$D"
sha256sum -c <<'SUMS'
4c5a4e912d2230fcd967bdbe26fda317ad63628d0648051fed70bb7cad7cfa97  prefix/lib/libcadical.a
f60811b2d5296c5164c03780dae4db95c2cc4253180dc74ddb0fd8b6a0a79f89  prefix/lib/libgmp.a
0f11e01426df3286d90110785bb94c890d8c459cecbbec38a4a2bba1b5d3da10  prefix/lib/libgmpxx.a
1b2ed5f9be8b3294359d9b8aae5a42b005ead8571ef276ebb16294cd00357374  build/src/libcvc5.a
SUMS
