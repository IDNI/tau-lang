#!/bin/bash
# Build and cache the cvc5 package in the LOCAL store. Each
# scripts/dep/<target>/cvc5.sh sources this file for its own target.
#
#   ./dev dep-cvc5 -DTAU_DEP_CC=clang -DTAU_DEP_CXX=clang++ \
#       -DTAU_BUILD_JOBS=5
#   ./dev dep-cvc5 -DTAU_DEP_MODE=consumer -DTAU_BUILD_JOBS=5
#
# The source is pinned to one immutable commit; the recipe tag is resolved to it
# and never checked out as a tag. Producer mode builds into a staging entry and
# publishes it. Consumer mode only looks up an existing entry.
#
# The target file defines _dep_cvc5_gmp_header, which prints the system gmp.h
# or nothing, and _dep_cvc5_target_setup, which sets the target arguments. It
# may define _dep_cvc5_target_fields, which prints extra id fields,
# _dep_cvc5_target_prebuild <work> <build>, which runs before the configure,
# _dep_cvc5_target_install <build> <prefix>, which replaces the install of every
# cvc5 target, _dep_cvc5_target_postinstall <prefix> <work>, which runs after
# the install, and _dep_cvc5_target_gmp_licenses <dir> <work>, which copies the
# license of a GMP that cvc5 did not download. _dep_cvc5_target_setup may name
# the cvc5 targets to build in _DEP_CVC5_BUILD_TARGETS; it builds all otherwise.

set -u

DEV_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
source "${DEV_ROOT}/scripts/devrc"

DEP_RECIPE_COMMON="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
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
	"GMP|6.3.0|https://github.com/cvc5/cvc5-deps/blob/main/gmp-6.3.0.tar.bz2?raw=true|ac28211a7cfb609bae2e2c8d6058d66c8fe96434f740cf6fe2e47b000d1c20cb"
)

declare -F _dep_cvc5_target_fields > /dev/null || _dep_cvc5_target_fields() { :; }
declare -F _dep_cvc5_target_prebuild > /dev/null || _dep_cvc5_target_prebuild() { :; }
declare -F _dep_cvc5_target_install > /dev/null || _dep_cvc5_target_install() {
	"$DEP_CVC5_CMAKE" --install "$1"
}
declare -F _dep_cvc5_target_postinstall > /dev/null || _dep_cvc5_target_postinstall() { :; }

# A system GMP is not in the package. Its license comes from the distro package
# when the distro keeps one, else from the copy of the GMP 6.3.0 source in tau.
declare -F _dep_cvc5_target_gmp_licenses > /dev/null || _dep_cvc5_target_gmp_licenses() {
	local dir="$1" f found=0
	for f in /usr/share/doc/libgmp10/copyright /usr/share/licenses/gmp/*; do
		[ -f "$f" ] || continue
		mkdir -p "$dir" && cp "$f" "$dir/" || return 1
		found=1
	done
	[ "$found" = 1 ] || _dep_cvc5_license_copy "$dir" "${DEV_ROOT}"/licenses/gmp/*
}

# Copy each existing <file> into <dir>. No existing file is an error.
_dep_cvc5_license_copy() {
	local dir="$1" f found=0
	shift
	for f in "$@"; do
		[ -f "$f" ] || continue
		mkdir -p "$dir" && cp "$f" "$dir/" || return 1
		found=1
	done
	[ "$found" = 1 ] || { echo "dep-cvc5: no license file for ${dir##*/}: $*" >&2; return 1; }
}

# The licenses of cvc5 and of each library in its package, from the sources of
# this build.
_dep_cvc5_licenses() {
	local prefix="$1" work="$2" build="$3" lic="${1}/share/licenses"
	if [ ! -f "${lic}/cvc5/COPYING" ]; then
		_dep_cvc5_license_copy "${lic}/cvc5" "${work}/COPYING" || return 1
	fi
	_dep_cvc5_license_copy "${lic}/cadical" \
		"${build}/deps/src/CaDiCaL-EP/LICENSE" "${work}"/msvc-src/cadical-*/LICENSE \
		&& _dep_cvc5_license_copy "${lic}/symfpu" "${build}/deps/src/SymFPU-EP/LICENSE" \
		|| return 1
	if [ "$DEP_CVC5_GMP_SOURCE" = download ]; then
		_dep_cvc5_license_copy "${lic}/gmp" "${build}"/deps/src/GMP-EP/COPYING*
	else
		_dep_cvc5_target_gmp_licenses "${lic}/gmp" "$work"
	fi
}

_dep_cvc5_compiler_id() {
	dep_compiler_id "$DEP_CVC5_CXX"
}

# URL basename with any query string removed: the archive name cvc5 fetches.
_dep_cvc5_url_archive() {
	local url="$1"
	url="${url%%\?*}"
	printf '%s' "${url##*/}"
}

# Print the "<major>.<minor>.<patch>" version from a gmp.h.
_dep_cvc5_gmp_version() {
	local header="$1" major minor patch
	major="$(awk '/#define __GNU_MP_VERSION /{print $3}' "$header")"
	minor="$(awk '/#define __GNU_MP_VERSION_MINOR /{print $3}' "$header")"
	patch="$(awk '/#define __GNU_MP_VERSION_PATCHLEVEL /{print $3}' "$header")"
	[ -n "$major" ] && [ -n "$minor" ] || return 1
	printf '%s.%s.%s' "$major" "$minor" "${patch:-0}"
}

# Resolve the system GMP, if the host provides it, before the build. cvc5's
# FindGMP uses the system GMP when gmp.h is on the include path and only then
# falls back to the download, so this is the same choice the configure makes.
# A target whose _dep_cvc5_gmp_header finds no gmp.h reaches the download,
# unless -DCVC5_CMAKE_PREFIX points at a GMP.
_dep_cvc5_gmp() {
	if [ -n "${CVC5_CMAKE_PREFIX:-}" ]; then
		printf 'external|%s' "$CVC5_CMAKE_PREFIX"
		return 0
	fi
	local header version
	header="$(_dep_cvc5_gmp_header)"
	if [ -n "$header" ]; then
		version="$(_dep_cvc5_gmp_version "$header")" || version=""
		if [ -n "$version" ]; then
			printf 'system|%s' "$version"
			return 0
		fi
	fi
	printf 'download|6.3.0'
}

# One hash over the name and the content of each file in licenses/gmp.
_dep_cvc5_gmp_licenses_hash() {
	local f sum list=""
	for f in "${DEV_ROOT}"/licenses/gmp/*; do
		[ -f "$f" ] || { echo "dep-cvc5: no GMP license file in ${DEV_ROOT}/licenses/gmp" >&2; return 1; }
		sum="$(dep_sha256 "$f")" || return 1
		list="${list}${f##*/} ${sum}"$'\n'
	done
	printf '%s' "$list" | dep_sha256_stdin
}

_dep_cvc5_field_block() {
	local build_helper publish_helper manifest store
	local recipe_hash recipe_common_hash build_hash publish_hash manifest_hash store_hash
	local gmp closure="" gmp_licenses_hash
	build_helper="${__devrc_dir}/dep-build"
	publish_helper="${__devrc_dir}/devrc"
	manifest="${__devrc_dir}/../cmake/tau-manifest.cmake"
	store="${__devrc_dir}/../cmake/tau-store.cmake"
	recipe_hash="$(dep_sha256 "$DEP_RECIPE")" || return 1
	recipe_common_hash="$(dep_sha256 "$DEP_RECIPE_COMMON")" || return 1
	build_hash="$(dep_sha256 "$build_helper")" || return 1
	publish_hash="$(dep_sha256 "$publish_helper")" || return 1
	manifest_hash="$(dep_sha256 "$manifest")" || return 1
	store_hash="$(dep_sha256 "$store")" || return 1
	# The repo GMP licenses enter a package whose system GMP has none.
	gmp_licenses_hash="$(_dep_cvc5_gmp_licenses_hash)" || return 1
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
		"target=${DEP_CVC5_TARGET:-$(dep_host_target)}" \
		"repo=${CVC5_REPO}" \
		"commit=${CVC5_COMMIT}" \
		"recipe_hash=${recipe_hash}" \
		"recipe_common_hash=${recipe_common_hash}" \
		"helper_build_hash=${build_hash}" \
		"provenance.publish_helper_hash=${publish_hash}" \
		"provenance.manifest_writer_hash=${manifest_hash}" \
		"provenance.store_writer_hash=${store_hash}" \
		"configure_args=${_DEP_CVC5_CONFIGURE_ARGS[*]}" \
		"closure=${closure}" \
		"gmp_source=${gmp%%|*}" \
		"gmp_version=${gmp##*|}" \
		"gmp_licenses_hash=${gmp_licenses_hash}" \
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
	_dep_cvc5_target_fields
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
# Print the CMake logs of <build> that no command writes to the output: the
# configure checks and every ExternalProject step.
_dep_cvc5_print_logs() {
	local build="$1" f
	for f in "${build}/CMakeFiles/CMakeConfigureLog.yaml" \
			"${build}"/deps/src/*-stamp/*.log; do
		[ -f "$f" ] || continue
		echo "dep-cvc5: ---- begin ${f} ----"
		cat "$f"
		echo "dep-cvc5: ---- end ${f} ----"
	done
}

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
	# consumed. A compiler with no flag-encoded path map (cl.exe) runs with
	# DEP_CVC5_PREFIX_MAP=OFF; the install-time rewrite below covers it.
	prefix_map=""
	if [ "$DEP_CVC5_PREFIX_MAP" = ON ]; then
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
	# The LOG_* options hide each dependency build in a stamp log. Without them
	# the build streams. Another count means a changed file, so stop.
	local _log_opts="${work}/cmake/deps-helper.cmake" _log_count
	_log_count="$(grep -cE '^[[:space:]]*LOG_[A-Z_]+[[:space:]]+ON[[:space:]]*$' "$_log_opts")"
	if [ "$_log_count" != 8 ]; then
		echo "dep-cvc5: expected 8 LOG_* ON lines in ${_log_opts}, found ${_log_count}" >&2
		rm -rf "$work"
		return 1
	fi
	# cvc5 adds "-modified" to its version when the checkout is dirty.
	sed -E '/^[[:space:]]*LOG_[A-Z_]+[[:space:]]+ON[[:space:]]*$/d' "$_log_opts" \
		> "${_log_opts}.tmp" && mv "${_log_opts}.tmp" "$_log_opts" \
		&& git -C "$work" update-index --assume-unchanged cmake/deps-helper.cmake \
		&& git -C "$work" diff-index --quiet HEAD \
		|| { echo "dep-cvc5: cannot remove the LOG_* options cleanly" >&2; rm -rf "$work"; return 1; }
	# configure.sh joins its options into one string and splits it again, so a
	# value with a space would reach CMake in pieces.
	local _a
	for _a in ${_DEP_CVC5_TARGET_ARGS[@]+"${_DEP_CVC5_TARGET_ARGS[@]}"} \
			"--prefix=${staging_prefix}"; do
		case "$_a" in
			*" "*)
				echo "dep-cvc5: cvc5's configure.sh cannot pass an option with a space: '${_a}'" >&2
				rm -rf "$work"
				return 1
				;;
		esac
	done
	_dep_cvc5_target_prebuild "$work" "$build" || { rm -rf "$work"; return 1; }
	# configure.sh must run from the cvc5 base directory. The prefix is added
	# here, not part of the recorded args.
	( cd "$work" && env -u CPPFLAGS -u LDFLAGS \
		${_DEP_CVC5_COMPILER_ENV[@]+"${_DEP_CVC5_COMPILER_ENV[@]}"} \
		CXXFLAGS="${DEP_CVC5_CXXFLAGS} ${prefix_map}" \
		CFLAGS="${DEP_CVC5_CFLAGS} ${prefix_map}" \
		./configure.sh --no-gpl --auto-download --no-poly -DUSE_DEFAULT_LINKER=ON \
			${_DEP_CVC5_TARGET_ARGS[@]+"${_DEP_CVC5_TARGET_ARGS[@]}"} \
			-DSKIP_SET_RPATH=ON \
			-DCMAKE_INSTALL_RPATH="$DEP_CVC5_INSTALL_RPATH" \
			-DCMAKE_BUILD_RPATH="$DEP_CVC5_BUILD_RPATH" \
			-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON \
			--name=build --prefix="$staging_prefix" ) \
		|| { echo "dep-cvc5: configure failed" >&2; _dep_cvc5_print_logs "$build" >&2
			rm -rf "$work"; return 1; }
	_dep_cvc5_print_logs "$build" >&2
	echo "dep-cvc5: feature set (CMakeCache USE_/ENABLE_):"
	_dep_cvc5_features "${build}/CMakeCache.txt" | sed 's/^/  /' >&2
	env -u CPPFLAGS -u CXXFLAGS -u CFLAGS -u LDFLAGS \
		${_DEP_CVC5_BUILD_ENV[@]+"${_DEP_CVC5_BUILD_ENV[@]}"} \
		"$DEP_CVC5_CMAKE" --build "$build" \
		${_DEP_CVC5_BUILD_TARGETS[@]+--target "${_DEP_CVC5_BUILD_TARGETS[@]}"} \
		-- -j "$CVC5_JOBS" \
		|| { echo "dep-cvc5: build failed" >&2; _dep_cvc5_print_logs "$build" >&2
			rm -rf "$work"; return 1; }
	_dep_cvc5_print_logs "$build" >&2
	echo "dep-cvc5: verified closure:"
	_dep_cvc5_verify_closure "$build" || { rm -rf "$work"; return 1; }
	( unset CPPFLAGS CXXFLAGS CFLAGS LDFLAGS
		_dep_cvc5_target_install "$build" "$staging_prefix" ) \
		|| { echo "dep-cvc5: install failed" >&2; rm -rf "$work"; return 1; }
	_dep_cvc5_target_postinstall "$staging_prefix" "$work" \
		|| { echo "dep-cvc5: post-install step failed" >&2; rm -rf "$work"; return 1; }
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
	_dep_cvc5_licenses "$staging_prefix" "$work" "$build" \
		|| { echo "dep-cvc5: cannot copy the license files" >&2; rm -rf "$work"; return 1; }
	rm -rf "$work"
	return 0
}

dep_entry "$@"

dep_require_file_target cvc5
for _hook in _dep_cvc5_gmp_header _dep_cvc5_target_setup; do
	if ! declare -F "$_hook" > /dev/null; then
		echo "dep-cvc5: ${DEP_RECIPE} defines no ${_hook}" >&2
		exit 2
	fi
done
dep_require_target_host dep-cvc5 "${DEP_TARGET:-$(dep_host_target)}"

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
DEP_CVC5_TARGET="${DEP_TARGET:-$(dep_host_target)}"
CVC5_CMAKE_PREFIX="$(dep_var CVC5_CMAKE_PREFIX "")"
DEP_CVC5_GENERATOR="Unix Makefiles"
DEP_CVC5_PREFIX_MAP=ON

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
DEP_CVC5_TOOLCHAIN="$(dep_var TAU_DEP_TOOLCHAIN "")"
_DEP_CVC5_TARGET_ARGS=()
_DEP_CVC5_COMPILER_ENV=()
_DEP_CVC5_BUILD_ENV=()
_DEP_CVC5_BUILD_TARGETS=()
DEP_CVC5_INSTALL_RPATH='${ORIGIN}:${ORIGIN}/../lib'
DEP_CVC5_BUILD_RPATH='${ORIGIN}'
_dep_cvc5_target_setup

# tau asks cvc5 only for linear logics, and libpoly serves only nonlinear
# arithmetic.
# cvc5 otherwise links with mold or gold when the host has one.
_DEP_CVC5_CONFIGURE_ARGS=(
	--no-gpl
	--auto-download
	--no-poly
	-DUSE_DEFAULT_LINKER=ON
	-DSKIP_SET_RPATH=ON
	-DCMAKE_INSTALL_RPATH="$DEP_CVC5_INSTALL_RPATH"
	-DCMAKE_BUILD_RPATH="$DEP_CVC5_BUILD_RPATH"
	-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON
	${_DEP_CVC5_TARGET_ARGS[@]+"${_DEP_CVC5_TARGET_ARGS[@]}"}
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
