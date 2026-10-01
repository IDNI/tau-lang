#!/bin/bash
# Write the Corresponding Source (GPLv3 section 6) of the Spot tools that the
# tau packages ship: the upstream tarball and the scripts that build it.
#
#   scripts/spot-source.sh <out-dir>
#
# The version, the URL and the digest come from the producer recipe, so a
# release attaches the source that the store package was built from. The
# tarball comes from a Spot store package, first local, then from
# TAU_STORE_REMOTE. Only when no store package holds it does the script
# download it from the Spot server.

set -euo pipefail

if [ $# -ne 1 ]; then
	echo "usage: scripts/spot-source.sh <out-dir>" >&2
	exit 2
fi

TOP="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RECIPE="$TOP/scripts/dep/common/spot.sh"
mkdir -p "$1"
OUT="$(cd "$1" && pwd)"

version="$(sed -n 's/^SPOT_VERSION="$(dep_var SPOT_VERSION \([^)]*\))"$/\1/p' "$RECIPE")"
sha256="$(sed -n 's/^SPOT_SHA256="\([0-9a-f]\{64\}\)"$/\1/p' "$RECIPE")"
url_template="$(grep -o 'https://[^"]*\.tar\.gz' "$RECIPE" | head -n 1)"
if [ -z "$version" ] || [ -z "$sha256" ] || [ -z "$url_template" ]; then
	echo "spot-source: cannot read the version, digest or URL from $RECIPE" >&2
	exit 1
fi
url="${url_template//\$\{SPOT_VERSION\}/$version}"

tarball="$OUT/spot-${version}.tar.gz"
store_root="${TAU_SHARED_PREFIX:-$HOME/.tau}"
store_remote="$TOP/external/parser/scripts/store-remote.sh"

# Copy the tarball of one local store entry when its digest matches.
take_from_entry() {
	local copy="$1/prefix/src/spot-${version}.tar.gz"
	[ -f "$copy" ] || return 1
	[ "$(sha256sum "$copy" | cut -d ' ' -f 1)" = "$sha256" ] || return 1
	cp "$copy" "$tarball"
	echo "spot-source: tarball from the store entry $1"
}

take_from_local_store() {
	local entry
	for entry in "$store_root"/store/spot/*/; do
		[ -d "$entry" ] || continue
		take_from_entry "${entry%/}" && return 0
	done
	return 1
}

# Every target has its own Spot id, and each id holds the same tarball, so
# any entry of the remote serves. An older entry without the tarball is skipped.
take_from_remote_store() {
	local repo tag id
	[ -n "${TAU_STORE_REMOTE:-}" ] || return 1
	command -v oras > /dev/null 2>&1 || return 1
	case "$TAU_STORE_REMOTE" in
		*/*) repo="${TAU_STORE_REMOTE%%/*}/$(printf '%s' "${TAU_STORE_REMOTE#*/}" | tr '[:upper:]' '[:lower:]')" ;;
		*) repo="$(printf '%s' "$TAU_STORE_REMOTE" | tr '[:upper:]' '[:lower:]')" ;;
	esac
	for tag in $(oras repo tags "$repo" 2>/dev/null | grep -E '^spot-[0-9a-f]{64}$'); do
		id="${tag#spot-}"
		[ -d "$store_root/store/spot/$id" ] \
			|| "$store_remote" pull "$store_root" "spot/$id" || continue
		take_from_entry "$store_root/store/spot/$id" && return 0
	done
	return 1
}

if ! take_from_local_store && ! take_from_remote_store; then
	echo "spot-source: no store package holds the tarball, downloading $url"
	curl -fsSL "$url" -o "$tarball"
fi
echo "${sha256}  ${tarball}" | sha256sum -c --quiet -

# The recipes, and every helper that they source or run with cmake -P.
cd "$TOP"
files=(
	dev
	scripts/devrc
	scripts/dep/*/spot.sh
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
	[ -f "$f" ] || { echo "spot-source: missing $f" >&2; exit 1; }
done
tar --sort=name --mtime=@0 --owner=0 --group=0 --numeric-owner \
	--transform "s,^,spot-build-scripts-${version}/," \
	-czf "$OUT/spot-build-scripts-${version}.tar.gz" "${files[@]}"

echo "spot-source: $tarball"
echo "spot-source: $OUT/spot-build-scripts-${version}.tar.gz"
