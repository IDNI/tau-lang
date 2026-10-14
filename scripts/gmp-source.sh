#!/bin/bash
# Write the Corresponding Source (LGPLv3, through GPLv3 section 6) of the GMP
# library that the tau packages and wheels ship beside cvc5: the upstream
# tarball, the vcpkg port that builds the Windows GMP, and the scripts that
# build both.
#
#   scripts/gmp-source.sh <out-dir>
#
# The version, the URL, the digest and the vcpkg commit come from the cvc5
# producer recipes, so a release attaches the source that the store packages
# were built from.

set -euo pipefail

if [ $# -ne 1 ]; then
	echo "usage: scripts/gmp-source.sh <out-dir>" >&2
	exit 2
fi

TOP="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RECIPE="$TOP/scripts/dep/common/cvc5.sh"
RECIPE_MSVC="$TOP/scripts/dep/common/cvc5-msvc.sh"
mkdir -p "$1"
OUT="$(cd "$1" && pwd)"

entry="$(grep -o '"GMP|[^"]*"' "$RECIPE" | tr -d '"' | head -n 1)"
IFS='|' read -r _ version url sha256 <<< "$entry"
vcpkg_repo="$(sed -n 's/^CVC5_MSVC_VCPKG_REPO="\(.*\)"$/\1/p' "$RECIPE_MSVC")"
vcpkg_commit="$(sed -n 's/^CVC5_MSVC_VCPKG_COMMIT="\([0-9a-f]\{40\}\)"$/\1/p' "$RECIPE_MSVC")"
if [ -z "$version" ] || [ -z "$url" ] || [ -z "$sha256" ] \
		|| [ -z "$vcpkg_repo" ] || [ -z "$vcpkg_commit" ]; then
	echo "gmp-source: cannot read the GMP pins from $RECIPE and $RECIPE_MSVC" >&2
	exit 1
fi

# The cvc5 mirror first, the GMP server second; both hold the same file.
tarball="$OUT/gmp-${version}.tar.bz2"
if ! curl -fsSL "$url" -o "$tarball"; then
	echo "gmp-source: the cvc5 mirror failed, trying gmplib.org"
	curl -fsSL "https://gmplib.org/download/gmp/gmp-${version}.tar.bz2" -o "$tarball"
fi
echo "${sha256}  ${tarball}" | sha256sum -c --quiet -

# The vcpkg port: its portfile and the patches it applies to that release.
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT
git -C "$work" init -q vcpkg
git -C "$work/vcpkg" remote add origin "$vcpkg_repo"
git -C "$work/vcpkg" sparse-checkout set --no-cone /ports/gmp/
git -C "$work/vcpkg" fetch -q --depth 1 --filter=blob:none origin "$vcpkg_commit"
git -C "$work/vcpkg" checkout -q FETCH_HEAD
port="gmp-vcpkg-port-${vcpkg_commit:0:12}"
tar --sort=name --mtime=@0 --owner=0 --group=0 --numeric-owner \
	-C "$work/vcpkg/ports" --transform "s,^gmp,${port}," \
	-czf "$OUT/${port}.tar.gz" gmp

# The recipes, and every helper that they source or run with cmake -P.
cd "$TOP"
files=(
	dev
	scripts/devrc
	scripts/dep/common/cvc5-msvc.sh
	scripts/dep/*/cvc5.sh
	cmake/cvc5-msvc/triplets/*.cmake
	external/parser/scripts/devrc
	external/parser/scripts/dep-build
	external/parser/scripts/store-remote.sh
	external/parser/scripts/store-transport.sh
	external/parser/scripts/audit-store-paths.sh
	external/parser/cmake/tau-manifest.cmake
	external/parser/cmake/tau-resolve.cmake
	external/parser/cmake/tau-store.cmake
)
for f in "${files[@]}"; do
	[ -f "$f" ] || { echo "gmp-source: missing $f" >&2; exit 1; }
done
tar --sort=name --mtime=@0 --owner=0 --group=0 --numeric-owner \
	--transform "s,^,gmp-build-scripts-${version}/," \
	-czf "$OUT/gmp-build-scripts-${version}.tar.gz" "${files[@]}"

echo "gmp-source: $tarball"
echo "gmp-source: $OUT/${port}.tar.gz"
echo "gmp-source: $OUT/gmp-build-scripts-${version}.tar.gz"
