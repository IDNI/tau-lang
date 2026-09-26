#!/bin/bash
# Build and cache the native Linux cvc5 package in the LOCAL store.
#
#   ./dev dep-cvc5-package -DTAU_DEP_CC=clang -DTAU_DEP_CXX=clang++ \
#       -DTAU_BUILD_JOBS=5
#   ./dev dep-cvc5-package -DTAU_DEP_MODE=consumer -DTAU_BUILD_JOBS=5
#
# The source is pinned to one immutable commit; the recipe tag is resolved to it
# and never checked out as a tag. Producer mode builds into a staging entry and
# publishes it. Consumer mode only looks up an existing entry.
#
# The native, w64, macOS and MSVC tuples are produced. MSVC builds with Ninja,
# the production build type and the preset's cl; GMP is the known risk and
# CVC5_CMAKE_PREFIX can point at an MSVC-compatible one.

set -u

DEV_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${DEV_ROOT}/scripts/devrc"

DEP_CVC5_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
CVC5_DEFAULT_REPO="https://github.com/cvc5/cvc5.git"
# cvc5-1.3.1, resolved to its commit. Never build the moving tag.
CVC5_DEFAULT_COMMIT="ea1b484fa54bfe56c0f8b3ac90a6e3e2f46441e7"

# The --auto-download closure of cvc5-1.3.1 with the configure arguments below.
# Each entry is "<name>|<version or commit>|<url>|<sha256>". The producer hashes
# every fetched archive and fails if it differs; the hash recorded in the
# identity is the verified one, not a copy of this table on faith. GMP is
# host-dependent: it is a system dependency when the host provides it, and a
# download otherwise (a different tuple).
CVC5_EXPECTED_CLOSURE=(
	"CaDiCaL|rel-2.1.3-elevate|https://github.com/arminbiere/cadical/archive/rel-2.1.3-elevate.tar.gz|15e1e82f7f9a9da0e97070cb8ac41d5b32139f65d54f72d2ff84849b0466ef92"
	"SymFPU|e6ac3af9c2c574498ea171c957425b407625448b|https://github.com/cvc5/symfpu/archive/e6ac3af9c2c574498ea171c957425b407625448b.tar.gz|823aa663fcc2f6844ae5e9ea83ceda4ed393cdb3dadefce9b3c7c41cd0f4f702"
	"libpoly|0.2.0|https://github.com/SRI-CSL/libpoly/archive/refs/tags/v0.2.0.tar.gz|146adc0d3f6fe8038adb6b8b69dd16114a4be12f520d5c1fb333f3746d233abe"
	"GMP|6.3.0|https://github.com/cvc5/cvc5-deps/blob/main/gmp-6.3.0.tar.bz2?raw=true|ac28211a7cfb609bae2e2c8d6058d66c8fe96434f740cf6fe2e47b000d1c20cb"
)

_dep_cvc5_compiler_id() {
	dep_compiler_id "$DEP_CVC5_CXX"
}

# URL basename with any query string removed: the archive name cvc5 fetches.
_dep_cvc5_url_archive() {
	local url="$1"
	url="${url%%\?*}"
	printf '%s' "${url##*/}"
}

# Resolve the system GMP, if the host provides it, before the build. cvc5's
# FindGMP uses the system GMP when gmp.h is on the include path and only then
# falls back to the download, so this is the same choice the configure makes.
# The system copy is a Linux host library, so w64 and macOS always download.
# MSVC can point at an MSVC-compatible GMP with -DCVC5_CMAKE_PREFIX; without
# one cvc5 auto-downloads (and then has to build) GMP.
_dep_cvc5_gmp() {
	if [ -n "${CVC5_CMAKE_PREFIX:-}" ]; then
		printf 'external|%s' "$CVC5_CMAKE_PREFIX"
		return 0
	fi
	if [ "$(dep_host_os)" = linux ] \
			&& [ "${DEP_CVC5_TARGET:-native}" != "w64" ] \
			&& [ -f /usr/include/gmp.h ]; then
		local major minor patch
		major="$(awk '/#define __GNU_MP_VERSION /{print $3}' /usr/include/gmp.h)"
		minor="$(awk '/#define __GNU_MP_VERSION_MINOR /{print $3}' /usr/include/gmp.h)"
		patch="$(awk '/#define __GNU_MP_VERSION_PATCHLEVEL /{print $3}' /usr/include/gmp.h)"
		if [ -n "$major" ] && [ -n "$minor" ]; then
			printf 'system|%s.%s.%s' "$major" "$minor" "${patch:-0}"
			return 0
		fi
	fi
	printf 'download|6.3.0'
}

# A real python for cvc5's FindPython: Git Bash PATH can carry the
# WindowsApps store stub, which imports nothing useful.
_dep_cvc5_python() {
	local c d
	for d in python3 python; do
		c="$(command -v "$d" 2>/dev/null || true)"
		[ -n "$c" ] || continue
		case "$c" in *WindowsApps*) continue ;; esac
		if "$c" -c "import sys" > /dev/null 2>&1; then
			if command -v cygpath > /dev/null 2>&1; then
				cygpath -m "$c"
			else
				printf '%s' "$c"
			fi
			return 0
		fi
	done
	return 1
}

_dep_cvc5_field_block() {
	local build_helper publish_helper manifest store
	local recipe_hash build_hash publish_hash manifest_hash store_hash
	local gmp closure=""
	build_helper="${__devrc_dir}/dep-build"
	publish_helper="${__devrc_dir}/devrc"
	manifest="${__devrc_dir}/../cmake/tau-manifest.cmake"
	store="${__devrc_dir}/../cmake/tau-store.cmake"
	recipe_hash="$(dep_sha256 "$DEP_CVC5_RECIPE")" || return 1
	build_hash="$(dep_sha256 "$build_helper")" || return 1
	publish_hash="$(dep_sha256 "$publish_helper")" || return 1
	manifest_hash="$(dep_sha256 "$manifest")" || return 1
	store_hash="$(dep_sha256 "$store")" || return 1
	gmp="${DEP_CVC5_GMP_SOURCE}|${DEP_CVC5_GMP_VERSION}"
	local entry name version url sha
	for entry in "${CVC5_EXPECTED_CLOSURE[@]}"; do
		IFS='|' read -r name version url sha <<< "$entry"
		if [ "$name" = "GMP" ] && [ "${gmp%%|*}" != "download" ]; then
			continue
		fi
		closure="${closure}${closure:+;}${name}|${version}|${url}|${sha}"
	done
	printf '%s\n' \
		"dep=cvc5" \
		"target=${DEP_CVC5_TARGET:-native}" \
		"repo=${CVC5_REPO}" \
		"commit=${CVC5_COMMIT}" \
		"recipe_hash=${recipe_hash}" \
		"helper_build_hash=${build_hash}" \
		"provenance.publish_helper_hash=${publish_hash}" \
		"provenance.manifest_writer_hash=${manifest_hash}" \
		"provenance.store_writer_hash=${store_hash}" \
		"configure_args=${_DEP_CVC5_CONFIGURE_ARGS[*]}" \
		"closure=${closure}" \
		"gmp_source=${gmp%%|*}" \
		"gmp_version=${gmp##*|}" \
		"cflags=${DEP_CVC5_CFLAGS}" \
		"cxxflags=${DEP_CVC5_CXXFLAGS}" \
		"cmake_path=${DEP_CVC5_CMAKE}" \
		"cmake_version=$("$DEP_CVC5_CMAKE" --version | head -n 1)" \
		"generator=${DEP_CVC5_GENERATOR}" \
		"compiler_id=$(_dep_cvc5_compiler_id)" \
		"compiler_version=$(dep_compiler_version "$DEP_CVC5_CXX")" \
		"target_triple=$(dep_compiler_triple "$DEP_CVC5_CXX" "$DEP_CVC5_TARGET")" \
		"os=$(uname -s)" \
		"arch=$(uname -m)" \
		"os_release_hash=$(dep_os_release_hash)" \
		"libc_version=$(dep_libc_version)" \
		"build_type=Release" \
		"pic=ON" \
		"lto=OFF" \
		"sanitizer=OFF" \
		"file_prefix_map=cvc5-src;cvc5-build;staging;store"
}

# Verify the fetched auto-download archives against the expected closure, and
# print "<name>|<version>|<url>|<actual sha256>" for each, plus a GMP line when
# cvc5 downloaded GMP instead of using the system copy.
_dep_cvc5_verify_closure() {
	local build="$1" entry name version url sha
	local archive found actual seen=""
	for entry in "${CVC5_EXPECTED_CLOSURE[@]}"; do
		IFS='|' read -r name version url sha <<< "$entry"
		if [ "$name" = "GMP" ] && [ "${DEP_CVC5_GMP_SOURCE}" != "download" ]; then
			continue
		fi
		archive="$(_dep_cvc5_url_archive "$url")"
		found="$(find "$build" -type f -name "$archive" 2>/dev/null | head -n1)"
		if [ -z "$found" ] || [ ! -f "$found" ]; then
			echo "dep-cvc5: expected downloaded archive '${archive}' for ${name} is missing" >&2
			echo "dep-cvc5: archives found under the build tree:" >&2
			find "$build" -type f \( -name '*.tar.*' -o -name '*.zip' \) 2>/dev/null | sed 's/^/  /' >&2
			return 1
		fi
		actual="$(dep_sha256 "$found")" || return 1
		if [ "$actual" != "$sha" ]; then
			echo "dep-cvc5: ${name} archive ${archive} hashes ${actual}, expected ${sha}" >&2
			return 1
		fi
		seen="${seen}${seen:+ }${archive}"
		printf '%s|%s|%s|%s\n' "$name" "$version" "$url" "$actual"
	done
	# Any other downloaded archive is an undeclared closure entry.
	local extra
	while IFS= read -r extra; do
		[ -n "$extra" ] || continue
		case " ${seen} " in *" $(basename "$extra") "*) ;;
		*)
			echo "dep-cvc5: undeclared downloaded archive $(basename "$extra")" >&2
			return 1
			;;
		esac
	done < <(find "$build" -type f \( -name '*.tar.*' -o -name '*.zip' \) 2>/dev/null)
	return 0
}

# Print the enabled/disabled feature set from the build's own CMake cache.
_dep_cvc5_features() {
	local cache="$1"
	grep -E '^(USE_|ENABLE_)[A-Z0-9_]+:BOOL=' "$cache" 2>/dev/null | LC_ALL=C sort
}

# Producer callback. $1 is the staging prefix. Source and build live beside it,
# outside the package prefix, and are removed before success.
_dep_cvc5_producer() {
	local staging_prefix="$1"
	local staging work build prefix_map
	staging="$(dirname "$staging_prefix")"
	work="${staging}/work"
	build="${work}/build"
	# The staging and store paths are random per build, so they cannot enter the
	# identity. The real paths go into the compiler flags; the identity records
	# the fixed placeholder each role maps to. GCC takes the last matching map,
	# so the least specific root comes first and the staging token is always
	# consumed. cl.exe has no flag-encoded path map; its install-time rewrite
	# below covers it.
	prefix_map=""
	if [ "$DEP_CVC5_TARGET" != "win-msvc-x64" ]; then
		prefix_map="-ffile-prefix-map=${staging}=staging -ffile-prefix-map=${staging_prefix}=staging -ffile-prefix-map=${work}=cvc5-src -ffile-prefix-map=${build}=cvc5-build"
	fi
	rm -rf "$work"
	mkdir -p "$work" "$staging_prefix"
	git init -q "$work" || { rm -rf "$work"; return 1; }
	git -C "$work" remote add origin "$CVC5_REPO" || { rm -rf "$work"; return 1; }
	git -C "$work" fetch -q --depth 1 origin "$CVC5_COMMIT" \
		|| { rm -rf "$work"; return 1; }
	git -C "$work" checkout -q FETCH_HEAD || { rm -rf "$work"; return 1; }
	local head
	head="$(git -C "$work" rev-parse HEAD)"
	if [ "$head" != "$CVC5_COMMIT" ]; then
		echo "dep-cvc5: checkout is ${head}, expected ${CVC5_COMMIT}" >&2
		rm -rf "$work"
		return 1
	fi
	# configure.sh must run from the cvc5 base directory. The prefix is added
	# here, not part of the recorded args.
	( cd "$work" && env -u CPPFLAGS -u LDFLAGS \
		"${_DEP_CVC5_COMPILER_ENV[@]}" \
		CXXFLAGS="${DEP_CVC5_CXXFLAGS} ${prefix_map}" \
		CFLAGS="${DEP_CVC5_CFLAGS} ${prefix_map}" \
		./configure.sh --no-gpl --auto-download \
			"${_DEP_CVC5_TARGET_ARGS[@]}" \
			-DSKIP_SET_RPATH=ON \
			-DCMAKE_INSTALL_RPATH="$DEP_CVC5_INSTALL_RPATH" \
			-DCMAKE_BUILD_RPATH="$DEP_CVC5_BUILD_RPATH" \
			-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON \
			--name=build --prefix="$staging_prefix" ) \
		> "${work}/configure.log" 2>&1 \
		|| { echo "dep-cvc5: configure failed" >&2; tail -20 "${work}/configure.log" >&2; rm -rf "$work"; return 1; }
	echo "dep-cvc5: feature set (CMakeCache USE_/ENABLE_):"
	_dep_cvc5_features "${build}/CMakeCache.txt" | sed 's/^/  /' >&2
	env -u CPPFLAGS -u CXXFLAGS -u CFLAGS -u LDFLAGS \
		"$DEP_CVC5_CMAKE" --build "$build" -- -j "$CVC5_JOBS" \
		|| { echo "dep-cvc5: build failed" >&2; rm -rf "$work"; return 1; }
	echo "dep-cvc5: verified closure:"
	_dep_cvc5_verify_closure "$build" || { rm -rf "$work"; return 1; }
	env -u CPPFLAGS -u CXXFLAGS -u CFLAGS -u LDFLAGS \
		"$DEP_CVC5_CMAKE" --install "$build" \
		|| { echo "dep-cvc5: install failed" >&2; rm -rf "$work"; return 1; }
	# The cross GMP is built under the staging tree and its library, import
	# library, dll, .la and .pc files bake those paths in. Rewrite each to an
	# equal-length placeholder so no build path enters the published package.
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
		echo "dep-cvc5: cannot rewrite baked build paths" >&2
		rm -rf "$work"
		return 1
	fi
	if [ ! -f "$staging_prefix/share/licenses/cvc5/COPYING" ]; then
		if [ -f "${work}/COPYING" ]; then
			mkdir -p "$staging_prefix/share/licenses/cvc5"
			cp "${work}/COPYING" "$staging_prefix/share/licenses/cvc5/COPYING"
		else
			echo "dep-cvc5: no COPYING in the cvc5 source" >&2
			rm -rf "$work"
			return 1
		fi
	fi
	rm -rf "$work"
	return 0
}

dep_entry "$@"

case "${DEP_TARGET:-native}" in
	native|w64|darwin-arm64|darwin-x86_64|win-msvc-x64) ;;
	*)
		echo "dep-cvc5: unsupported target '${DEP_TARGET}'" >&2
		exit 2
		;;
esac
dep_require_target_host dep-cvc5 "${DEP_TARGET:-native}"

mode="$(dep_var TAU_DEP_MODE producer)"
case "$mode" in
	producer|consumer) ;;
	*)
		echo "dep-cvc5: -DTAU_DEP_MODE must be producer or consumer, got '${mode}'" >&2
		exit 2
		;;
esac

CVC5_REPO="$(dep_var CVC5_REPO "$CVC5_DEFAULT_REPO")"
CVC5_COMMIT="$(dep_var CVC5_COMMIT "$CVC5_DEFAULT_COMMIT")"
case "$CVC5_COMMIT" in
	*[!0-9a-f]*|"")
		echo "dep-cvc5: CVC5_COMMIT must be 40 lowercase hex characters, got '${CVC5_COMMIT}'" >&2
		exit 2
		;;
esac
if [ "${#CVC5_COMMIT}" -ne 40 ]; then
	echo "dep-cvc5: CVC5_COMMIT must be 40 lowercase hex characters, got '${CVC5_COMMIT}'" >&2
	exit 2
fi
CVC5_JOBS="$(dep_jobs)"
DEP_CVC5_TARGET="${DEP_TARGET:-native}"
CVC5_CMAKE_PREFIX="$(dep_var CVC5_CMAKE_PREFIX "")"
DEP_CVC5_GENERATOR="Unix Makefiles"
[ "$DEP_CVC5_TARGET" = "win-msvc-x64" ] && DEP_CVC5_GENERATOR="Ninja"

# The system GMP, detected once, is both an identity input and the closure
# choice the configure will make.
DEP_CVC5_GMP="$(_dep_cvc5_gmp)"
DEP_CVC5_GMP_SOURCE="${DEP_CVC5_GMP%%|*}"
DEP_CVC5_GMP_VERSION="${DEP_CVC5_GMP##*|}"

DEP_CVC5_CMAKE="$(command -v "${CMAKE:-cmake}")" \
	|| { echo "dep-cvc5: cmake not found" >&2; exit 2; }
DEP_CVC5_CC="$(dep_var TAU_DEP_CC "")"
DEP_CVC5_CXX="$(dep_var TAU_DEP_CXX "")"
if [ -z "$DEP_CVC5_CC" ] || [ -z "$DEP_CVC5_CXX" ]; then
	echo "dep-cvc5: no compiler; pass -DTAU_DEP_CC and -DTAU_DEP_CXX" >&2
	exit 2
fi
DEP_CVC5_CFLAGS="$(dep_var TAU_DEP_CFLAGS "")"
DEP_CVC5_CXXFLAGS="$(dep_var TAU_DEP_CXXFLAGS "")"
_DEP_CVC5_TARGET_ARGS=()
_DEP_CVC5_COMPILER_ENV=()
DEP_CVC5_INSTALL_RPATH='${ORIGIN}:${ORIGIN}/../lib'
DEP_CVC5_BUILD_RPATH='${ORIGIN}'
case "$DEP_CVC5_TARGET" in
	w64)
		# cvc5's own mingw64 toolchain picks the cross compilers; --win64 enables it.
		_DEP_CVC5_TARGET_ARGS=(--win64)
		;;
	win-msvc-x64)
		# Native MSVC: Ninja + the production build type. cl comes from the
		# preset through -DTAU_DEP_C*; CMake must not fall back to another
		# compiler. GMP is the known risk -- point CVC5_CMAKE_PREFIX at an
		# MSVC-compatible GMP when --auto-download cannot supply one.
		_DEP_CVC5_TARGET_ARGS=(--ninja production
			-DCMAKE_C_COMPILER="$DEP_CVC5_CC"
			-DCMAKE_CXX_COMPILER="$DEP_CVC5_CXX")
		_py="$(_dep_cvc5_python || true)"
		if [ -n "$_py" ]; then
			_DEP_CVC5_TARGET_ARGS+=("-DPython_EXECUTABLE=$_py"
				"-DPython3_EXECUTABLE=$_py")
		fi
		if [ -n "$CVC5_CMAKE_PREFIX" ]; then
			_DEP_CVC5_TARGET_ARGS+=("-DCMAKE_PREFIX_PATH=$CVC5_CMAKE_PREFIX")
		fi
		_DEP_CVC5_COMPILER_ENV=(CC="$DEP_CVC5_CC" CXX="$DEP_CVC5_CXX")
		;;
	darwin-*)
		# macOS has no $ORIGIN and no absolute install_name: a relocatable
		# package resolves through @rpath, matching Tau's own install rpath.
		DEP_CVC5_INSTALL_RPATH='@loader_path:@loader_path/../lib'
		DEP_CVC5_BUILD_RPATH='@loader_path'
		_DEP_CVC5_TARGET_ARGS=(-DCMAKE_INSTALL_NAME_DIR=@rpath)
		_DEP_CVC5_COMPILER_ENV=(CC="$DEP_CVC5_CC" CXX="$DEP_CVC5_CXX")
		;;
	*)
		_DEP_CVC5_COMPILER_ENV=(CC="$DEP_CVC5_CC" CXX="$DEP_CVC5_CXX")
		;;
esac

_DEP_CVC5_CONFIGURE_ARGS=(
	--no-gpl
	--auto-download
	-DSKIP_SET_RPATH=ON
	-DCMAKE_INSTALL_RPATH="$DEP_CVC5_INSTALL_RPATH"
	-DCMAKE_BUILD_RPATH="$DEP_CVC5_BUILD_RPATH"
	-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON
	"${_DEP_CVC5_TARGET_ARGS[@]}"
)

block="$(_dep_cvc5_field_block)" || {
	echo "dep-cvc5: cannot build the identity field block" >&2
	exit 1
}

out=""
if ! dep_ensure out "$mode" cvc5 "$block" _dep_cvc5_producer; then
	echo "dep-cvc5: ${mode} failed" >&2
	exit 1
fi

echo "dep-cvc5: package prefix: ${out}"
