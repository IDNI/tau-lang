#!/bin/bash
# The Spot CLI package for windows-arm64-msvc.
# scripts/dep/common/spot.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=windows-arm64-msvc
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# The MSYS2 CLANGARM64 toolchain that builds the Windows host tool.
_dep_spot_msys_root() {
	local d
	for d in \
		"/c/msys64" \
		"/c/tools/msys64" \
		"${HOME:-}/msys64" \
		"/c/Program Files/msys64"
	do
		if [ -x "${d}/clangarm64/bin/clang++.exe" ]; then
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
		DEP_SPOT_BUILDER_CXX="${DEP_SPOT_MSYS_ROOT}/clangarm64/bin/clang++.exe"
	fi
	DEP_SPOT_EXE="ltlsynt.exe"
}

# A lookup must work without the toolchain, so the id names it and asks no clang++.
_dep_spot_target_compiler_fields() {
	printf '%s\n' \
		"compiler_id=Clang" \
		"compiler_version=msys2-clangarm64" \
		"target_triple=aarch64-w64-mingw32"
}

# Git Bash rewrites `/c` and `/d` on cmd/bash command lines as
# filesystem paths; keep the MSYS2 invocation in one login shell and
# let it set CLANGARM64 up itself.
# CPPFLAGS=-w drops the warnings of the Spot sources and keeps the
# optimization flags that configure picks.
# clang refuses the narrowing of a size to the 32-bit Windows unsigned long
# in Spot.
_dep_spot_target_build() {
	local src="$1" staging_prefix="$2"
	local build_script
	if [ -z "$DEP_SPOT_MSYS_ROOT" ]; then
		echo "dep-spot: MSYS2 CLANGARM64 clang++ not found; install MSYS2 and" >&2
		echo "  pacman -S --noconfirm --needed mingw-w64-clang-aarch64-toolchain make diffutils" >&2
		return 1
	fi
	# clang refuses a dllexport template with a local type, and a static build exports nothing.
	local common_hh="${src}/spot/misc/common.hh"
	if ! grep -qx '  #define SPOT_DLL' "$common_hh"; then
		echo "dep-spot: common.hh has no SPOT_DLL define" >&2
		return 1
	fi
	sed -i 's|^  #define SPOT_DLL$|  // no SPOT_DLL: the static build exports nothing|' "$common_hh" \
		&& ! grep -q '^  #define SPOT_DLL$' "$common_hh" \
		|| { echo "dep-spot: cannot drop the SPOT_DLL define" >&2; return 1; }
	build_script="$(dirname "$src")/build.sh"
	cat > "$build_script" <<EOF
#!/bin/bash
set -euo pipefail
export PATH=/clangarm64/bin:/usr/bin:\$PATH
cd "${src}"
./configure --prefix="${staging_prefix}" --disable-python --disable-shared --disable-devel CPPFLAGS="-w -Wno-c++11-narrowing"
make -j${DEP_SPOT_JOBS}
make install-exec
EOF
	export MSYSTEM=CLANGARM64
	export CHERE_INVOKING=1
	"${DEP_SPOT_MSYS_ROOT}/usr/bin/bash.exe" --login "$build_script" \
		|| { echo "dep-spot: MSYS2 build failed" >&2; return 1; }
}

# The runtime DLLs ship with the tools, so their license texts ship too.
# _dep_spot_runtime_licenses <staging prefix> <env dir> <msystem> <dll>...
_dep_spot_runtime_licenses() {
	local staging_prefix="$1" env_dir="$2" msystem="$3"
	shift 3
	local work script list dll path rel found
	work="$(dirname "$staging_prefix")/work"
	script="${work}/licenses.sh"
	list="${work}/licenses.txt"
	mkdir -p "$work"
	cat > "$script" <<'EOF'
#!/bin/bash
set -uo pipefail
out="$1" env_dir="$2"
shift 2
: > "$out"
for dll in "$@"; do
	owner="$(pacman -Qoq "/${env_dir}/bin/${dll}")" || continue
	pacman -Qlq "$owner" | grep '/share/licenses/' | grep -v '/$' \
		| sed "s|^|${dll} |" >> "$out"
done
EOF
	MSYSTEM="$msystem" CHERE_INVOKING=1 \
		"${DEP_SPOT_MSYS_ROOT}/usr/bin/bash.exe" --login "$script" "$list" "$env_dir" "$@" \
		|| { echo "dep-spot: pacman license lookup failed" >&2; return 1; }
	for dll in "$@"; do
		found=0
		while read -r path; do
			rel="${path#*/share/licenses/}"
			mkdir -p "$(dirname "${staging_prefix}/share/licenses/${rel}")"
			cp -f "${DEP_SPOT_MSYS_ROOT}${path}" "${staging_prefix}/share/licenses/${rel}" \
				|| { echo "dep-spot: cannot copy ${path}" >&2; return 1; }
			found=1
		done < <(sed -n "s|^${dll} ||p" "$list")
		if [ "$found" = 0 ]; then
			echo "dep-spot: no license files for ${dll}" >&2
			return 1
		fi
	done
}

# CreateProcess loads DLLs from the executable's directory; the MSVC
# test process has no CLANGARM64 on PATH the way an MSYS shell does.
_dep_spot_target_finish() {
	local staging_prefix="$1" dll strip copied=()
	for dll in libc++.dll libunwind.dll libwinpthread-1.dll; do
		if [ -f "${DEP_SPOT_MSYS_ROOT}/clangarm64/bin/${dll}" ]; then
			cp -f "${DEP_SPOT_MSYS_ROOT}/clangarm64/bin/${dll}" \
				"${staging_prefix}/bin/"
			copied+=("$dll")
		fi
	done
	_dep_spot_runtime_licenses "$staging_prefix" clangarm64 CLANGARM64 ${copied[@]+"${copied[@]}"} \
		|| return 1
	strip="${DEP_SPOT_MSYS_ROOT}/clangarm64/bin/strip.exe"
	if [ ! -x "$strip" ]; then
		strip="${DEP_SPOT_MSYS_ROOT}/clangarm64/bin/llvm-strip.exe"
	fi
	if [ -x "$strip" ]; then
		"$strip" "${staging_prefix}/bin/"*.exe
	fi
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/spot.sh"
