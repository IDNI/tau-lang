#!/bin/bash
# Build and cache the Spot CLI package in the LOCAL store. Each
# scripts/dep/<target>/spot.sh sources this file for its own target.
#
#   ./dev dep-spot -DTAU_DEP_TARGET=linux-x86_64 -DTAU_DEP_CC=clang \
#       -DTAU_DEP_CXX=clang++ -DTAU_BUILD_JOBS=8
#   ./dev dep-spot -DTAU_DEP_TARGET=windows-x86_64-msvc -DTAU_BUILD_JOBS=8
#
# Tau never links Spot: it only execs ltlsynt, autfilt and ltlfilt, the way a
# Linux install gets them from the distro `spot` package. This producer builds
# the pinned official tarball for the host that runs those tools.
#
# Native and macOS use the preset's compiler. The windows-x86_64-msvc target is
# a host tool (nothing links it), so it is built with MSYS2 UCRT64 g++ and the
# three runtime DLLs are copied beside the executables; cl.exe cannot build
# Spot.
# SPOT_SHA256 pins the tarball; an archive with a different digest aborts the
# build.

set -u

DEV_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
source "${DEV_ROOT}/scripts/devrc"

DEP_RECIPE_COMMON="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
# Digest of the Spot 2.16 release tarball from the URL below. The package keeps
# that tarball as src/spot-<version>.tar.gz, the copy a release attaches.
SPOT_SHA256="688463cb2fa393c51d9cf938fb01a716b91e4c8122aeb52fd116a3bbfddab869"

# A target file overrides the ones it needs before it sources this file.
# _dep_spot_target_setup sets the builder and its flags.
declare -F _dep_spot_target_setup > /dev/null || _dep_spot_target_setup() { :; }

# _dep_spot_target_build <src> <staging prefix> configures, builds and
# installs the tools with the preset's compiler.
if ! declare -F _dep_spot_target_build > /dev/null; then
	_dep_spot_target_build() {
		local src="$1" staging_prefix="$2"
		( cd "$src" && env -u CPPFLAGS -u LDFLAGS \
			CC="$DEP_SPOT_CC" CXX="$DEP_SPOT_CXX" \
			CFLAGS="$DEP_SPOT_CFLAGS" CXXFLAGS="$DEP_SPOT_CXXFLAGS" \
			./configure --prefix="$staging_prefix" --disable-python \
				--disable-shared --disable-devel ) \
			|| { echo "dep-spot: configure failed" >&2; return 1; }
		( cd "$src" && make -j "$DEP_SPOT_JOBS" ) \
			|| { echo "dep-spot: build failed" >&2; return 1; }
		( cd "$src" && make install-exec ) \
			|| { echo "dep-spot: install failed" >&2; return 1; }
	}
fi

# _dep_spot_target_finish <staging prefix> completes the installed tools.
declare -F _dep_spot_target_finish > /dev/null || _dep_spot_target_finish() { :; }

# _dep_spot_target_compiler_fields prints the compiler_id, compiler_version and
# target_triple fields of the id.
if ! declare -F _dep_spot_target_compiler_fields > /dev/null; then
	_dep_spot_target_compiler_fields() {
		printf '%s\n' \
			"compiler_id=$(dep_compiler_id "$DEP_SPOT_BUILDER_CXX")" \
			"compiler_version=$(dep_compiler_version "$DEP_SPOT_BUILDER_CXX")" \
			"target_triple=$(dep_compiler_triple "$DEP_SPOT_BUILDER_CXX" "${DEP_SPOT_TARGET:-$(dep_host_target)}")"
	}
fi

_dep_spot_field_block() {
	local build_helper publish_helper manifest store
	local recipe_hash recipe_common_hash build_hash publish_hash manifest_hash store_hash
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
	printf '%s\n' \
		"dep=spot" \
		"target=${DEP_SPOT_TARGET:-$(dep_host_target)}" \
		"version=${SPOT_VERSION}" \
		"url=${SPOT_URL}" \
		"sha256=${DEP_SPOT_SHA256}" \
		"recipe_hash=${recipe_hash}" \
		"recipe_common_hash=${recipe_common_hash}" \
		"helper_build_hash=${build_hash}" \
		"provenance.publish_helper_hash=${publish_hash}" \
		"provenance.manifest_writer_hash=${manifest_hash}" \
		"provenance.store_writer_hash=${store_hash}" \
		"configure_args=${_DEP_SPOT_CONFIGURE_ARGS[*]}" \
		"cflags=${DEP_SPOT_CFLAGS}" \
		"cxxflags=${DEP_SPOT_CXXFLAGS}" \
		"builder=${DEP_SPOT_BUILDER}"
	_dep_spot_target_compiler_fields
	printf '%s\n' \
		"os=$(uname -s)" \
		"arch=$(uname -m)" \
		"build_type=Release" \
		"shared=OFF" \
		"python=OFF"
}

# Producer callback. $1 is the staging prefix. Source and build live beside it,
# outside the package prefix, and are removed before success.
_dep_spot_producer() {
	local staging_prefix="$1"
	local staging work src tarball got
	staging="$(dirname "$staging_prefix")"
	work="${staging}/work"
	src="${work}/spot-${SPOT_VERSION}"
	tarball="${work}/spot-${SPOT_VERSION}.tar.gz"
	rm -rf "$work"
	mkdir -p "$work" "$staging_prefix"
	if ! command -v curl > /dev/null 2>&1; then
		echo "dep-spot: curl not found" >&2
		rm -rf "$work"
		return 1
	fi
	curl -fsSL "$SPOT_URL" -o "$tarball" \
		|| { echo "dep-spot: download failed" >&2; rm -rf "$work"; return 1; }
	got="$(dep_sha256 "$tarball")" || { rm -rf "$work"; return 1; }
	if [ "$got" != "$DEP_SPOT_SHA256" ]; then
		echo "dep-spot: archive sha256 ${got}, expected ${DEP_SPOT_SHA256}" >&2
		rm -rf "$work"
		return 1
	fi
	# From stdin: GNU tar takes a file name with a drive letter for a host.
	tar -xzf - -C "$work" < "$tarball" \
		|| { echo "dep-spot: extract failed" >&2; rm -rf "$work"; return 1; }
	if [ ! -d "$src" ]; then
		echo "dep-spot: extracted tree not found: '$src'" >&2
		rm -rf "$work"
		return 1
	fi

	_dep_spot_target_build "$src" "$staging_prefix" || { rm -rf "$work"; return 1; }

	if [ ! -f "${staging_prefix}/bin/${DEP_SPOT_EXE}" ]; then
		echo "dep-spot: ${staging_prefix}/bin/${DEP_SPOT_EXE} missing" >&2
		rm -rf "$work"
		return 1
	fi

	_dep_spot_target_finish "$staging_prefix" || { rm -rf "$work"; return 1; }
	# install-exec also installs the static libraries. Tau only execs the tools.
	rm -rf "${staging_prefix}/lib" "${staging_prefix}/include"

	# The configure prefix is baked into the tools; rewrite every baked build
	# path to an equal-length placeholder so none is published.
	if ! "$DEP_PYTHON" - "$staging_prefix" "$staging" "$work" "$src" <<'PY'
import os
import sys
root, *old_paths = sys.argv[1:]
# A Windows compiler writes the same paths with backslashes, and an MSYS2
# tool writes the drive as a folder: C:/x is /c/x.
def spellings(p):
	out = {p, p.replace('/', '\\')}
	if len(p) > 2 and p[0].isalpha() and p[1:3] == ':/':
		out.add('/' + p[0].lower() + p[2:])
	return out
olds = sorted({s.encode() for p in old_paths if p for s in spellings(p)},
	key=len, reverse=True)
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
		echo "dep-spot: cannot rewrite baked build paths" >&2
		rm -rf "$work"
		return 1
	fi
	if [ -f "${src}/COPYING" ]; then
		mkdir -p "$staging_prefix/share/licenses/spot"
		cp "${src}/COPYING" "$staging_prefix/share/licenses/spot/COPYING"
	else
		echo "dep-spot: no COPYING in the spot source" >&2
		rm -rf "$work"
		return 1
	fi
	# Copied after the path rewrite, which must not touch the verified bytes.
	mkdir -p "$staging_prefix/src"
	cp "$tarball" "$staging_prefix/src/spot-${SPOT_VERSION}.tar.gz" \
		|| { echo "dep-spot: cannot keep the source tarball" >&2; rm -rf "$work"; return 1; }
	rm -rf "$work"
	return 0
}

dep_entry "$@"
DEP_PYTHON="$(dep_python)" || exit 2

SPOT_VERSION="$(dep_var SPOT_VERSION 2.16)"
SPOT_URL="$(dep_var SPOT_URL \
	"https://www.lre.epita.fr/dload/spot/spot-${SPOT_VERSION}.tar.gz")"

dep_require_file_target spot
dep_require_target_host dep-spot "${DEP_TARGET:-$(dep_host_target)}"

mode="$(dep_var TAU_DEP_MODE producer)"
case "$mode" in
	producer|consumer) ;;
	*)
		echo "dep-spot: -DTAU_DEP_MODE must be producer or consumer, got '${mode}'" >&2
		exit 2
		;;
esac

DEP_SPOT_JOBS="$(dep_jobs)"
DEP_SPOT_TARGET="${DEP_TARGET:-$(dep_host_target)}"
DEP_SPOT_SHA256="$(dep_var SPOT_SHA256 "${SPOT_SHA256}")"
DEP_SPOT_CC="$(dep_var TAU_DEP_CC "")"
DEP_SPOT_CXX="$(dep_var TAU_DEP_CXX "")"
if [ -z "$DEP_SPOT_CC" ] || [ -z "$DEP_SPOT_CXX" ]; then
	echo "dep-spot: no compiler; pass -DTAU_DEP_CC and -DTAU_DEP_CXX" >&2
	exit 2
fi
DEP_SPOT_CFLAGS="$(dep_var TAU_DEP_CFLAGS "")"
DEP_SPOT_CXXFLAGS="$(dep_var TAU_DEP_CXXFLAGS "")"
DEP_SPOT_BUILDER="preset"
DEP_SPOT_BUILDER_CXX="$DEP_SPOT_CXX"
DEP_SPOT_EXE="ltlsynt"
_dep_spot_target_setup

_DEP_SPOT_CONFIGURE_ARGS=(
	--disable-python
	--disable-shared
	--disable-devel
)

block="$(_dep_spot_field_block)" || {
	echo "dep-spot: cannot build the identity field block" >&2
	exit 1
}

out=""
if ! dep_ensure out "$mode" spot "$block" _dep_spot_producer; then
	echo "dep-spot: ${mode} failed" >&2
	exit 1
fi

echo "dep-spot: package prefix: ${out}"
