#!/bin/bash
# Install a pinned oras release, verified by sha256, for the CI jobs that
# read the remote store.
#
#   ./dev dep-oras
#
# The binary lands in <TAU_SHARED_PREFIX>/oras/bin. When GITHUB_PATH is set
# (a GitHub Actions runner) that directory is appended to it, so later steps
# see oras; a writable /usr/local/bin also gets a symlink, which is how the
# container stages see it. The version and every digest are pinned below, so
# a re-run is a no-op and a changed tarball fails the checksum.
set -u

source "$(dirname "${BASH_SOURCE[0]}")/env"

ORAS_VERSION="1.2.2"
ORAS_BASE_URL="https://github.com/oras-project/oras/releases/download/v${ORAS_VERSION}"

case "$(uname -s)" in
	Linux) ORAS_OS=linux ;;
	Darwin) ORAS_OS=darwin ;;
	MINGW*|MSYS*|CYGWIN*) ORAS_OS=windows ;;
	*) echo "dep-oras: unsupported host $(uname -s)" >&2; exit 2 ;;
esac
case "$(uname -m)" in
	x86_64|amd64) ORAS_ARCH=amd64 ;;
	aarch64|arm64) ORAS_ARCH=arm64 ;;
	*) echo "dep-oras: unsupported arch $(uname -m)" >&2; exit 2 ;;
esac

if [ "$ORAS_OS" = windows ]; then
	ORAS_ASSET="oras_${ORAS_VERSION}_${ORAS_OS}_${ORAS_ARCH}.zip"
else
	ORAS_ASSET="oras_${ORAS_VERSION}_${ORAS_OS}_${ORAS_ARCH}.tar.gz"
fi

# From oras_1.2.2_checksums.txt. The asset above must have an entry here, so
# a new platform cannot be added without its digest.
case "$ORAS_ASSET" in
	oras_1.2.2_linux_amd64.tar.gz)   ORAS_SHA="bff970346470e5ef888e9f2c0bf7f8ee47283f5a45207d6e7a037da1fb0eae0d" ;;
	oras_1.2.2_linux_arm64.tar.gz)   ORAS_SHA="edd7195cbb8ba56c29ede413eefa10c8026201d63326017cd315841b4063aa56" ;;
	oras_1.2.2_darwin_amd64.tar.gz)  ORAS_SHA="e953c3c5580a317ba30871ee77f39a37e5bcb030edf01c5e9a83ebddcec7cf8a" ;;
	oras_1.2.2_darwin_arm64.tar.gz)  ORAS_SHA="fcac22adaf1cfe7854738859d0f4705d1cd75a23edaef5ee2e9aba66cfff2315" ;;
	oras_1.2.2_windows_amd64.zip)    ORAS_SHA="25110d69d220bf55469a14c243f759305737dac6672fb47b4af4e43b000f55f5" ;;
	*) echo "dep-oras: no pinned digest for ${ORAS_ASSET}" >&2; exit 2 ;;
esac

prefix="$(dep_shared_prefix)" || exit 1
bin_dir="${prefix}/oras/bin"
binary="${bin_dir}/oras"
if [ "$ORAS_OS" = windows ]; then
	binary="${bin_dir}/oras.exe"
fi
stamp="${prefix}/oras/.version"
expected="${ORAS_VERSION} ${ORAS_ASSET} ${ORAS_SHA}"

if [ -x "$binary" ] && [ "$(cat "$stamp" 2>/dev/null)" = "$expected" ]; then
	echo "dep-oras: ${ORAS_VERSION} already installed at ${binary}"
else
	work="$(mktemp -d "${TMPDIR:-/tmp}/tau-oras.XXXXXX")" || exit 1
	# shellcheck disable=SC2064
	trap "rm -rf '${work}'" EXIT
	archive="${work}/${ORAS_ASSET}"
	echo "dep-oras: downloading ${ORAS_ASSET}"
	curl -fsSL --retry 3 -o "$archive" "${ORAS_BASE_URL}/${ORAS_ASSET}" || {
		echo "dep-oras: cannot download ${ORAS_ASSET}" >&2; exit 1; }
	actual="$(dep_sha256 "$archive")" || exit 1
	if [ "$actual" != "$ORAS_SHA" ]; then
		echo "dep-oras: ${ORAS_ASSET} hashes ${actual}, expected ${ORAS_SHA}" >&2
		exit 1
	fi
	mkdir -p "$bin_dir"
	# A .tar.gz carries the binary at its root; a .zip has the same layout.
	(cd "$work" && "${CMAKE:-cmake}" -E tar xf "$archive") || {
		echo "dep-oras: cannot extract ${ORAS_ASSET}" >&2; exit 1; }
	if [ ! -f "${work}/oras" ] && [ ! -f "${work}/oras.exe" ]; then
		echo "dep-oras: ${ORAS_ASSET} holds no oras binary" >&2
		exit 1
	fi
	if [ -f "${work}/oras.exe" ]; then
		mv "${work}/oras.exe" "$binary"
	else
		mv "${work}/oras" "$binary"
	fi
	chmod +x "$binary"
	printf '%s' "$expected" > "$stamp"
	echo "dep-oras: installed ${ORAS_VERSION} at ${binary}"
fi

if [ -n "${GITHUB_PATH:-}" ] && [ -w "${GITHUB_PATH}" ]; then
	printf '%s\n' "$bin_dir" >> "$GITHUB_PATH"
fi
if [ -d /usr/local/bin ] && [ -w /usr/local/bin ]; then
	ln -sf "$binary" /usr/local/bin/oras
fi
