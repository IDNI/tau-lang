#!/bin/bash
# install-ccache-windows.sh <x86_64|aarch64>
#
# Installs the pinned ccache release for Windows on a GitHub runner: puts
# ccache.exe on PATH for the later steps and sets CCACHE_DIR, as a Windows
# path, to ~/.ccache, the directory the workflow caches. Run it from Git Bash.
set -euo pipefail

version=4.14.3
arch="${1:?usage: install-ccache-windows.sh <x86_64|aarch64>}"
case "$arch" in
	x86_64)  sha=c845c3d65be81954c75659145a522a793a27586bf940c5dcfe18af33fa1a67dd ;;
	aarch64) sha=4c12ee652936c83b7730ff16f93c86e9e905a91aed9549ce1304185f33c8b3e4 ;;
	*) echo "unknown architecture: $arch" >&2; exit 1 ;;
esac

name="ccache-${version}-windows-${arch}"
base="$(cygpath -u "${RUNNER_TEMP:-$HOME}")"
dest="${base}/${name}"
zip="${dest}.zip"
curl -fsSL -o "$zip" \
	"https://github.com/ccache/ccache/releases/download/v${version}/${name}.zip"
echo "${sha}  ${zip}" | sha256sum -c -
unzip -q -o "$zip" -d "$(dirname "$dest")"
"${dest}/ccache.exe" --version | head -n 1

# ccache.exe is a native program, so it reads a Windows path.
mkdir -p "$HOME/.ccache"
cygpath -w "$dest" >> "$GITHUB_PATH"
echo "CCACHE_DIR=$(cygpath -w "$HOME/.ccache")" >> "$GITHUB_ENV"
