#!/bin/bash
# The Spot CLI package for windows-x86_64-msvc.
# scripts/dep/common/spot.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=windows-x86_64-msvc
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

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

_dep_spot_target_setup() {
	DEP_SPOT_BUILDER="msys2"
	DEP_SPOT_MSYS_ROOT="$(_dep_spot_msys_root)" || DEP_SPOT_MSYS_ROOT=""
	if [ -n "$DEP_SPOT_MSYS_ROOT" ]; then
		DEP_SPOT_BUILDER_CXX="${DEP_SPOT_MSYS_ROOT}/ucrt64/bin/g++.exe"
	fi
	DEP_SPOT_EXE="ltlsynt.exe"
}

# A lookup must work without the toolchain, so the id names it and asks no g++.
_dep_spot_target_compiler_fields() {
	printf '%s\n' \
		"compiler_id=GNU" \
		"compiler_version=msys2-ucrt64" \
		"target_triple=x86_64-w64-mingw32"
}

# Git Bash rewrites `/c` and `/d` on cmd/bash command lines as
# filesystem paths; keep the MSYS2 invocation in one login shell and
# let it set UCRT64 up itself.
# CPPFLAGS=-w drops the warnings of the Spot sources and keeps the
# optimization flags that configure picks.
_dep_spot_target_build() {
	local src="$1" staging_prefix="$2"
	local build_script
	if [ -z "$DEP_SPOT_MSYS_ROOT" ]; then
		echo "dep-spot: MSYS2 UCRT64 g++ not found; install MSYS2 and" >&2
		echo "  pacman -S mingw-w64-ucrt-x86_64-gcc make" >&2
		return 1
	fi
	build_script="$(dirname "$src")/build.sh"
	cat > "$build_script" <<EOF
#!/bin/bash
set -euo pipefail
export PATH=/ucrt64/bin:/usr/bin:\$PATH
cd "${src}"
./configure --prefix="${staging_prefix}" --disable-python --disable-shared --disable-devel CPPFLAGS=-w
make -j${DEP_SPOT_JOBS}
make install-exec
EOF
	export MSYSTEM=UCRT64
	export CHERE_INVOKING=1
	"${DEP_SPOT_MSYS_ROOT}/usr/bin/bash.exe" --login "$build_script" \
		|| { echo "dep-spot: MSYS2 build failed" >&2; return 1; }
}

# CreateProcess loads DLLs from the executable's directory; the MSVC
# test process has no UCRT64 on PATH the way an MSYS shell does.
_dep_spot_target_finish() {
	local staging_prefix="$1" dll
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
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/spot.sh"
