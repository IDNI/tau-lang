#!/bin/bash
# Build and cache the Spot CLI package in the LOCAL store.
#
#   ./dev dep-spot-package -DTAU_DEP_TARGET=native -DTAU_DEP_CC=clang \
#       -DTAU_DEP_CXX=clang++ -DTAU_BUILD_JOBS=8
#   ./dev dep-spot-package -DTAU_DEP_TARGET=win-msvc-x64 -DTAU_BUILD_JOBS=8
#
# Tau never links Spot: it only execs ltlsynt, autfilt and ltlfilt, the way a
# Linux install gets them from the distro `spot` package. This producer builds
# the pinned official tarball for the host that runs those tools.
#
# Native and macOS use the preset's compiler. The win-msvc-x64 target is a host
# tool (nothing links it), so it is built with MSYS2 UCRT64 g++ and the three
# runtime DLLs are copied beside the executables; cl.exe cannot build Spot.
# SPOT_SHA256 pins the tarball; an archive with a different digest aborts the
# build.

set -u

DEV_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${DEV_ROOT}/scripts/devrc"

DEP_SPOT_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
# Spot 2.16 release tarball digest, fetched from the URL below.
SPOT_SHA256="688463cb2fa393c51d9cf938fb01a716b91e4c8122aeb52fd116a3bbfddab869"

# The MSYS2 UCRT64 toolchain that builds the Windows host tool.
_dep_spot_msys_root() {
	local d
	for d in \
		"/c/msys64" \
		"/c/tools/msys64" \
		"${HOME:-}/msys64" \
		"/c/Program Files/msys64"
	do
		if [ -x "${d}/ucrt64/bin/g++.exe" ]; then
			printf '%s' "$d"
			return 0
		fi
	done
	return 1
}

_dep_spot_field_block() {
	local build_helper publish_helper manifest store
	local recipe_hash build_hash publish_hash manifest_hash store_hash
	build_helper="${__devrc_dir}/dep-build"
	publish_helper="${__devrc_dir}/devrc"
	manifest="${__devrc_dir}/../cmake/tau-manifest.cmake"
	store="${__devrc_dir}/../cmake/tau-store.cmake"
	recipe_hash="$(dep_sha256 "$DEP_SPOT_RECIPE")" || return 1
	build_hash="$(dep_sha256 "$build_helper")" || return 1
	publish_hash="$(dep_sha256 "$publish_helper")" || return 1
	manifest_hash="$(dep_sha256 "$manifest")" || return 1
	store_hash="$(dep_sha256 "$store")" || return 1
	printf '%s\n' \
		"dep=spot" \
		"target=${DEP_SPOT_TARGET:-native}" \
		"version=${SPOT_VERSION}" \
		"url=${SPOT_URL}" \
		"sha256=${DEP_SPOT_SHA256}" \
		"recipe_hash=${recipe_hash}" \
		"helper_build_hash=${build_hash}" \
		"provenance.publish_helper_hash=${publish_hash}" \
		"provenance.manifest_writer_hash=${manifest_hash}" \
		"provenance.store_writer_hash=${store_hash}" \
		"configure_args=${_DEP_SPOT_CONFIGURE_ARGS[*]}" \
		"builder=${DEP_SPOT_BUILDER}" \
		"compiler_id=$(dep_compiler_id "$DEP_SPOT_BUILDER_CXX")" \
		"compiler_version=$(dep_compiler_version "$DEP_SPOT_BUILDER_CXX")" \
		"target_triple=$(dep_compiler_triple "$DEP_SPOT_BUILDER_CXX" "${DEP_SPOT_TARGET:-native}")" \
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
	tar -xzf "$tarball" -C "$work" \
		|| { echo "dep-spot: extract failed" >&2; rm -rf "$work"; return 1; }
	if [ ! -d "$src" ]; then
		echo "dep-spot: extracted tree not found: '$src'" >&2
		rm -rf "$work"
		return 1
	fi

	local build_script="${work}/build.sh"
	if [ "$DEP_SPOT_BUILDER" = msys2 ]; then
		# Git Bash rewrites `/c` and `/d` on cmd/bash command lines as
		# filesystem paths; keep the MSYS2 invocation in one login shell and
		# let it set UCRT64 up itself.
		cat > "$build_script" <<EOF
#!/bin/bash
set -euo pipefail
export PATH=/ucrt64/bin:/usr/bin:\$PATH
cd "${src}"
./configure --prefix="${staging_prefix}" --disable-python --disable-shared --disable-devel
make -j${DEP_SPOT_JOBS}
make install-exec
EOF
		export MSYSTEM=UCRT64
		export CHERE_INVOKING=1
		"${DEP_SPOT_MSYS_ROOT}/usr/bin/bash.exe" --login "$build_script" \
			|| { echo "dep-spot: MSYS2 build failed" >&2; rm -rf "$work"; return 1; }
	else
		( cd "$src" && env -u CPPFLAGS -u LDFLAGS \
			CC="$DEP_SPOT_CC" CXX="$DEP_SPOT_CXX" \
			./configure --prefix="$staging_prefix" --disable-python \
				--disable-shared --disable-devel ) \
			> "${work}/configure.log" 2>&1 \
			|| { echo "dep-spot: configure failed" >&2; tail -20 "${work}/configure.log" >&2; rm -rf "$work"; return 1; }
		( cd "$src" && make -j "$DEP_SPOT_JOBS" ) \
			> "${work}/build.log" 2>&1 \
			|| { echo "dep-spot: build failed" >&2; tail -20 "${work}/build.log" >&2; rm -rf "$work"; return 1; }
		( cd "$src" && make install-exec ) \
			> "${work}/install.log" 2>&1 \
			|| { echo "dep-spot: install failed" >&2; tail -20 "${work}/install.log" >&2; rm -rf "$work"; return 1; }
	fi

	local exe="ltlsynt"
	[ "$DEP_SPOT_TARGET" = "win-msvc-x64" ] && exe="ltlsynt.exe"
	if [ ! -f "${staging_prefix}/bin/${exe}" ]; then
		echo "dep-spot: ${staging_prefix}/bin/${exe} missing" >&2
		rm -rf "$work"
		return 1
	fi

	if [ "$DEP_SPOT_BUILDER" = msys2 ]; then
		# CreateProcess loads DLLs from the executable's directory; the MSVC
		# test process has no UCRT64 on PATH the way an MSYS shell does.
		local dll
		for dll in libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll; do
			if [ -f "${DEP_SPOT_MSYS_ROOT}/ucrt64/bin/${dll}" ]; then
				cp -f "${DEP_SPOT_MSYS_ROOT}/ucrt64/bin/${dll}" \
					"${staging_prefix}/bin/"
			fi
		done
		if [ -x "${DEP_SPOT_MSYS_ROOT}/ucrt64/bin/strip.exe" ]; then
			"${DEP_SPOT_MSYS_ROOT}/ucrt64/bin/strip.exe" \
				"${staging_prefix}/bin/"*.exe
		fi
	fi

	# The configure prefix is baked into the tools; rewrite every baked build
	# path to an equal-length placeholder so none is published.
	if ! python3 - "$staging_prefix" "$staging" "$work" "$src" <<'PY'
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
	rm -rf "$work"
	return 0
}

dep_entry "$@"

SPOT_VERSION="$(dep_var SPOT_VERSION 2.16)"
SPOT_URL="$(dep_var SPOT_URL \
	"https://www.lre.epita.fr/dload/spot/spot-${SPOT_VERSION}.tar.gz")"

case "${DEP_TARGET:-native}" in
	native|darwin-arm64|darwin-x86_64|win-msvc-x64) ;;
	*)
		echo "dep-spot: unsupported target '${DEP_TARGET}'" >&2
		exit 2
		;;
esac
dep_require_target_host dep-spot "${DEP_TARGET:-native}"

mode="$(dep_var TAU_DEP_MODE producer)"
case "$mode" in
	producer|consumer) ;;
	*)
		echo "dep-spot: -DTAU_DEP_MODE must be producer or consumer, got '${mode}'" >&2
		exit 2
		;;
esac

DEP_SPOT_JOBS="$(dep_jobs)"
DEP_SPOT_TARGET="${DEP_TARGET:-native}"
DEP_SPOT_SHA256="$(dep_var SPOT_SHA256 "${SPOT_SHA256}")"
DEP_SPOT_CC="$(dep_var TAU_DEP_CC "")"
DEP_SPOT_CXX="$(dep_var TAU_DEP_CXX "")"
if [ -z "$DEP_SPOT_CC" ] || [ -z "$DEP_SPOT_CXX" ]; then
	echo "dep-spot: no compiler; pass -DTAU_DEP_CC and -DTAU_DEP_CXX" >&2
	exit 2
fi

DEP_SPOT_BUILDER="preset"
DEP_SPOT_BUILDER_CXX="$DEP_SPOT_CXX"
if [ "$DEP_SPOT_TARGET" = "win-msvc-x64" ]; then
	DEP_SPOT_BUILDER="msys2"
	if ! DEP_SPOT_MSYS_ROOT="$(_dep_spot_msys_root)"; then
		echo "dep-spot: MSYS2 UCRT64 g++ not found; install MSYS2 and" >&2
		echo "  pacman -S mingw-w64-ucrt-x86_64-gcc make" >&2
		exit 2
	fi
	DEP_SPOT_BUILDER_CXX="${DEP_SPOT_MSYS_ROOT}/ucrt64/bin/g++.exe"
fi

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
