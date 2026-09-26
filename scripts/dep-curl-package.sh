#!/bin/bash
# Build and cache the static curl package for the Windows targets in the LOCAL
# store.
#
#   ./dev dep-curl -DTAU_DEP_TARGET=w64 -DTAU_BUILD_JOBS=8
#   ./dev dep-curl -DTAU_DEP_TARGET=win-msvc-x64 -DTAU_BUILD_JOBS=8
#
# A Windows target has no system curl, and nlang's HTTP oracle links
# CURL::libcurl. curl is built static with the Windows schannel TLS backend and
# every optional dependency off, so the archive needs only Windows system
# libraries. Native Linux and macOS keep the system curl; wasm never needs one.
#
# The source is one pinned release tarball; nothing here moves a tag.

set -u

DEV_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${DEV_ROOT}/scripts/devrc"

DEP_CURL_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
CURL_VERSION="8.11.1"
CURL_URL="https://curl.se/download/curl-8.11.1.tar.gz"
CURL_SHA256="a889ac9dbba3644271bd9d1302b5c22a088893719b72be3487bc3d401e5c4e80"

_dep_curl_compiler_id() {
	dep_compiler_id "$DEP_CURL_CXX"
}

_dep_curl_field_block() {
	local build_helper publish_helper manifest store
	local recipe_hash build_hash publish_hash manifest_hash store_hash
	build_helper="${__devrc_dir}/dep-build"
	publish_helper="${__devrc_dir}/devrc"
	manifest="${__devrc_dir}/../cmake/tau-manifest.cmake"
	store="${__devrc_dir}/../cmake/tau-store.cmake"
	recipe_hash="$(dep_sha256 "$DEP_CURL_RECIPE")" || return 1
	build_hash="$(dep_sha256 "$build_helper")" || return 1
	publish_hash="$(dep_sha256 "$publish_helper")" || return 1
	manifest_hash="$(dep_sha256 "$manifest")" || return 1
	store_hash="$(dep_sha256 "$store")" || return 1
	printf '%s\n' \
		"dep=curl" \
		"target=${DEP_CURL_TARGET:-w64}" \
		"version=${CURL_VERSION}" \
		"url=${CURL_URL}" \
		"sha256=${CURL_SHA256}" \
		"recipe_hash=${recipe_hash}" \
		"helper_build_hash=${build_hash}" \
		"provenance.publish_helper_hash=${publish_hash}" \
		"provenance.manifest_writer_hash=${manifest_hash}" \
		"provenance.store_writer_hash=${store_hash}" \
		"configure_args=${_DEP_CURL_CONFIGURE_ARGS[*]}" \
		"toolchain=${DEP_CURL_TOOLCHAIN}" \
		"cmake_path=${DEP_CURL_CMAKE}" \
		"cmake_version=$("$DEP_CURL_CMAKE" --version | head -n 1)" \
		"generator=Ninja" \
		"ninja_path=${DEP_CURL_NINJA}" \
		"ninja_version=$("$DEP_CURL_NINJA" --version)" \
		"compiler_id=$(_dep_curl_compiler_id)" \
		"compiler_version=$(dep_compiler_version "$DEP_CURL_CXX")" \
		"target_triple=$(dep_compiler_triple "$DEP_CURL_CXX" "${DEP_CURL_TARGET:-w64}")" \
		"os=$(uname -s)" \
		"arch=$(uname -m)" \
		"build_type=Release" \
		"shared=OFF" \
		"tls=schannel" \
		"lto=OFF" \
		"file_prefix_map=curl-src;curl-build;staging"
}

# Producer callback. $1 is the staging prefix. Source and build live beside it,
# outside the package prefix, and are removed before success.
_dep_curl_producer() {
	local staging_prefix="$1"
	local staging work build src got prefix_map
	staging="$(dirname "$staging_prefix")"
	work="${staging}/work"
	build="${work}/build"
	src="${work}/curl-${CURL_VERSION}"
	rm -rf "$work"
	mkdir -p "$work" "$staging_prefix"
	"$DEP_CURL_CURL" -fsSL "$CURL_URL" -o "${work}/curl.tar.gz" \
		|| { echo "dep-curl: download failed" >&2; rm -rf "$work"; return 1; }
	got="$(dep_sha256 "${work}/curl.tar.gz")" \
		|| { rm -rf "$work"; return 1; }
	if [ "$got" != "$CURL_SHA256" ]; then
		echo "dep-curl: archive sha256 ${got}, expected ${CURL_SHA256}" >&2
		rm -rf "$work"
		return 1
	fi
	tar -xzf "${work}/curl.tar.gz" -C "$work" \
		|| { echo "dep-curl: extract failed" >&2; rm -rf "$work"; return 1; }
	if [ ! -d "$src" ]; then
		echo "dep-curl: extracted tree not found: '$src'" >&2
		rm -rf "$work"
		return 1
	fi
	# Build and staging paths would otherwise bake into the archive. MSVC has
	# no flag-encoded path map; the install-time rewrite below covers it.
	prefix_map=""
	if [ "$(dep_compiler_id "$DEP_CURL_CXX")" != "MSVC" ]; then
		prefix_map="-ffile-prefix-map=${src}=curl-src -ffile-prefix-map=${build}=curl-build -ffile-prefix-map=${work}=curl-src -ffile-prefix-map=${staging_prefix}=staging -ffile-prefix-map=${staging}=staging"
	fi
	env -u CPPFLAGS -u CXXFLAGS -u CFLAGS -u LDFLAGS \
		"$DEP_CURL_CMAKE" -S "$src" -B "$build" \
		"${_DEP_CURL_CONFIGURE_ARGS[@]}" \
		"${_DEP_CURL_TOOLCHAIN_ARGS[@]}" \
		-DCMAKE_C_COMPILER="$DEP_CURL_CC" \
		-DCMAKE_CXX_COMPILER="$DEP_CURL_CXX" \
		"-DCMAKE_C_FLAGS=${DEP_CURL_CFLAGS} ${prefix_map}" \
		"-DCMAKE_CXX_FLAGS=${DEP_CURL_CXXFLAGS} ${prefix_map}" \
		-DCMAKE_INSTALL_PREFIX="$staging_prefix" \
		|| { echo "dep-curl: configure failed" >&2; rm -rf "$work"; return 1; }
	env -u CPPFLAGS -u CXXFLAGS -u CFLAGS -u LDFLAGS \
		"$DEP_CURL_CMAKE" --build "$build" -- -j "$DEP_CURL_JOBS" \
		|| { echo "dep-curl: build failed" >&2; rm -rf "$work"; return 1; }
	env -u CPPFLAGS -u CXXFLAGS -u CFLAGS -u LDFLAGS \
		"$DEP_CURL_CMAKE" --install "$build" \
		|| { echo "dep-curl: install failed" >&2; rm -rf "$work"; return 1; }
	# curl-config and libcurl.pc bake the staging prefix; rewrite every baked
	# path to an equal-length placeholder so none is published.
	if ! python3 - "$staging_prefix" "$staging" "$work" "$build" <<'PY'
import os
import sys
root, *old_paths = sys.argv[1:]
olds = sorted({p.encode() for p in old_paths if p}, key=len, reverse=True)
for dirpath, _dirs, files in os.walk(root):
	for name in files:
		path = os.path.join(dirpath, name)
		if os.path.islink(path):
			continue
		try:
			with open(path, 'rb') as fh:
				data = fh.read()
		except OSError:
			continue
		if not any(old in data for old in olds):
			continue
		for old in olds:
			if old in data:
				data = data.replace(old, b'@' * len(old))
		with open(path, 'wb') as fh:
			fh.write(data)
PY
	then
		echo "dep-curl: cannot rewrite baked build paths" >&2
		rm -rf "$work"
		return 1
	fi
	if [ ! -f "$staging_prefix/lib/libcurl.a" ] \
			&& [ ! -f "$staging_prefix/lib/libcurl.lib" ]; then
		echo "dep-curl: no static curl archive under ${staging_prefix}/lib" >&2
		rm -rf "$work"
		return 1
	fi
	if [ -f "${src}/COPYING" ]; then
		mkdir -p "$staging_prefix/share/licenses/curl"
		cp "${src}/COPYING" "$staging_prefix/share/licenses/curl/COPYING"
	else
		echo "dep-curl: no COPYING in the source" >&2
		rm -rf "$work"
		return 1
	fi
	rm -rf "$work"
	return 0
}

dep_entry "$@"

case "${DEP_TARGET:-native}" in
	w64|win-msvc-x64) ;;
	*)
		echo "dep-curl: only the Windows targets are supported, got '${DEP_TARGET}'" >&2
		exit 2
		;;
esac
dep_require_target_host dep-curl "${DEP_TARGET:-w64}"

mode="$(dep_var TAU_DEP_MODE producer)"
case "$mode" in
	producer|consumer) ;;
	*)
		echo "dep-curl: -DTAU_DEP_MODE must be producer or consumer, got '${mode}'" >&2
		exit 2
		;;
esac

DEP_CURL_TARGET="${DEP_TARGET:-w64}"
DEP_CURL_JOBS="$(dep_jobs)"

DEP_CURL_CMAKE="$(command -v "${CMAKE:-cmake}")" \
	|| { echo "dep-curl: cmake not found" >&2; exit 2; }
DEP_CURL_NINJA="$(command -v ninja)" \
	|| { echo "dep-curl: ninja not found" >&2; exit 2; }
DEP_CURL_CURL="$(command -v curl)" \
	|| { echo "dep-curl: curl not found" >&2; exit 2; }
DEP_CURL_CC="$(dep_var TAU_DEP_CC "")"
DEP_CURL_CXX="$(dep_var TAU_DEP_CXX "")"
if [ -z "$DEP_CURL_CC" ] || [ -z "$DEP_CURL_CXX" ]; then
	echo "dep-curl: no compiler; pass -DTAU_DEP_CC and -DTAU_DEP_CXX" >&2
	exit 2
fi
DEP_CURL_CFLAGS="$(dep_var TAU_DEP_CFLAGS "")"
DEP_CURL_CXXFLAGS="$(dep_var TAU_DEP_CXXFLAGS "")"
DEP_CURL_TOOLCHAIN="$(dep_var TAU_DEP_TOOLCHAIN "")"
_DEP_CURL_TOOLCHAIN_ARGS=()
if dep_target_needs_toolchain "$DEP_CURL_TARGET"; then
	if [ -z "$DEP_CURL_TOOLCHAIN" ]; then
		echo "dep-curl: ${DEP_CURL_TARGET} needs -DTAU_DEP_TOOLCHAIN" >&2
		exit 2
	fi
	_DEP_CURL_TOOLCHAIN_ARGS=(-DCMAKE_TOOLCHAIN_FILE="$DEP_CURL_TOOLCHAIN")
fi

_DEP_CURL_CONFIGURE_ARGS=(
	-G Ninja
	-DCMAKE_BUILD_TYPE=Release
	-DBUILD_SHARED_LIBS=OFF
	-DBUILD_STATIC_LIBS=ON
	-DBUILD_CURL_EXE=OFF
	-DBUILD_EXAMPLES=OFF
	-DBUILD_TESTING=OFF
	-DBUILD_LIBCURL_DOCS=OFF
	-DBUILD_MISC_DOCS=OFF
	-DENABLE_CURL_MANUAL=OFF
	-DHTTP_ONLY=ON
	-DCURL_USE_SCHANNEL=ON
	-DCURL_USE_OPENSSL=OFF
	-DCURL_USE_GNUTLS=OFF
	-DCURL_USE_MBEDTLS=OFF
	-DCURL_USE_WOLFSSL=OFF
	-DCURL_USE_BEARSSL=OFF
	-DCURL_USE_RUSTLS=OFF
	-DCURL_USE_LIBPSL=OFF
	-DCURL_USE_LIBSSH2=OFF
	-DCURL_USE_LIBSSH=OFF
	-DCURL_USE_LIBIDN2=OFF
	-DCURL_USE_GSSAPI=OFF
	-DCURL_USE_LDAP=OFF
	-DCURL_ZLIB=OFF
	-DCURL_ZSTD=OFF
	-DCURL_BROTLI=OFF
	-DCURL_USE_NGHTTP2=OFF
	-DUSE_NGTCP2=OFF
	-DUSE_QUICHE=OFF
	-DUSE_MSH3=OFF
	-DENABLE_THREADED_RESOLVER=OFF
	-DPICKY_COMPILER=OFF
	-DCMAKE_C_FLAGS="$DEP_CURL_CFLAGS"
	-DCMAKE_CXX_FLAGS="$DEP_CURL_CXXFLAGS"
)

block="$(_dep_curl_field_block)" || {
	echo "dep-curl: cannot build the identity field block" >&2
	exit 1
}

out=""
if ! dep_ensure out "$mode" curl "$block" _dep_curl_producer; then
	echo "dep-curl: ${mode} failed" >&2
	exit 1
fi

echo "dep-curl: package prefix: ${out}"
