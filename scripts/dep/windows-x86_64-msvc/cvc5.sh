#!/bin/bash
# The cvc5 package for windows-x86_64-msvc.
# scripts/dep/common/cvc5.sh holds the shared recipe.
#
# cvc5 does not build with cl.exe, so the clang-cl of the Visual Studio install
# builds it for both MSVC presets. The id records that builder and its own
# flags, not the preset compiler, so the cl and clang-cl presets share one
# package. Both sides use the MSVC ABI, the VS STL and the /MD runtime.
#
# cvc5 downloads neither CaDiCaL nor GMP here. CaDiCaL builds from the pinned
# closure archive with cmake/cvc5-msvc/cadical.cmake, and GMP comes from vcpkg
# at a pinned commit, because gmpxx must have the MSVC ABI. GMP is a DLL, never
# a static library in cvc5.dll, and it ships in bin beside cvc5.dll.
#
# The package holds the cvc5 library only. The parser library of cvc5 does not
# link as a DLL of its own here: it reads data and functions of cvc5.dll that
# cvc5 exports to no other DLL. tau uses neither it nor the cvc5 binary, which
# needs it, so neither is built or installed.

set -u

DEP_FILE_TARGET=windows-x86_64-msvc
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

CVC5_MSVC_VCPKG_REPO="https://github.com/microsoft/vcpkg.git"
# The vcpkg commit whose gmp port is 6.3.0, port-version 5.
CVC5_MSVC_VCPKG_COMMIT="e06564091c10e0b042900800c9fd48aad7f00643"
CVC5_MSVC_GMP_PORT="gmp:x64-windows"
CVC5_MSVC_GMP_VERSION="6.3.0#5"
# cvc5 keys its Windows code on the MinGW macro __WIN32__. The flags hold no
# path: the compat header and the unistd.h and getopt.h shims reach clang-cl
# through CL, and their hashes are id fields.
CVC5_MSVC_FLAGS="-D__WIN32__"

# No system GMP serves this target: the prebuild step installs the vcpkg one.
_dep_cvc5_gmp_header() {
	return 0
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

# The clang-cl of the Visual Studio install, not the first one on PATH: a
# standalone LLVM changes with the runner image, and the VS copy matches the
# STL headers that cl.exe uses.
_dep_cvc5_vs_clang_cl() {
	local vswhere vs
	vswhere="/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
	[ -x "$vswhere" ] || vswhere="$(command -v vswhere.exe 2>/dev/null || true)"
	[ -n "$vswhere" ] || return 1
	vs="$("$vswhere" -latest -products '*' \
		-requires Microsoft.VisualStudio.Component.VC.Llvm.Clang \
		-property installationPath 2>/dev/null | tr -d '\r' | head -n 1)"
	[ -n "$vs" ] || return 1
	vs="$(cygpath -m "$vs")"
	[ -f "${vs}/VC/Tools/Llvm/x64/bin/clang-cl.exe" ] || return 1
	printf '%s' "${vs}/VC/Tools/Llvm/x64/bin/clang-cl.exe"
}

_dep_cvc5_target_setup() {
	local _py _clang_cl
	if [ -n "$CVC5_CMAKE_PREFIX" ]; then
		echo "dep-cvc5: windows-x86_64-msvc builds its own CaDiCaL and GMP; remove -DCVC5_CMAKE_PREFIX" >&2
		exit 2
	fi
	if ! _clang_cl="$(_dep_cvc5_vs_clang_cl)"; then
		echo "dep-cvc5: no Visual Studio clang-cl found; install the VS component Microsoft.VisualStudio.Component.VC.Llvm.Clang" >&2
		exit 2
	fi
	CVC5_MSVC_DIR="${DEV_ROOT}/cmake/cvc5-msvc"
	DEP_CVC5_CC="$_clang_cl"
	DEP_CVC5_CXX="$_clang_cl"
	DEP_CVC5_CFLAGS="$CVC5_MSVC_FLAGS"
	DEP_CVC5_CXXFLAGS="$CVC5_MSVC_FLAGS"
	DEP_CVC5_GMP_SOURCE="vcpkg"
	DEP_CVC5_GMP_VERSION="$CVC5_MSVC_GMP_VERSION"
	DEP_CVC5_GENERATOR="Ninja"
	_DEP_CVC5_BUILD_TARGETS=(cvc5)
	# clang-cl takes no flag-encoded path map; the install-time rewrite covers it.
	DEP_CVC5_PREFIX_MAP=OFF
	# clang-cl reaches CMake only as CC/CXX: configure.sh splits a -D value at
	# spaces, and clang-cl sits under "Program Files".
	# The try_run version checks of cvc5 link the Release CaDiCaL and GMP, so
	# they build in Release too.
	_DEP_CVC5_TARGET_ARGS=(--ninja production -DCMAKE_TRY_COMPILE_CONFIGURATION=Release)
	_py="$(_dep_cvc5_python || true)"
	if [ -n "$_py" ]; then
		_DEP_CVC5_TARGET_ARGS+=("-DPython_EXECUTABLE=$_py"
			"-DPython3_EXECUTABLE=$_py")
	fi
	_DEP_CVC5_COMPILER_ENV=(CC="$DEP_CVC5_CC" CXX="$DEP_CVC5_CXX")
}

# The builder, the vcpkg pin and the files of cmake/cvc5-msvc/ change the
# package, so each is an id field.
_dep_cvc5_target_fields() {
	local compat unistd getopt cadical
	compat="$(dep_sha256 "${CVC5_MSVC_DIR}/compat.h")" || return 1
	unistd="$(dep_sha256 "${CVC5_MSVC_DIR}/include/unistd.h")" || return 1
	getopt="$(dep_sha256 "${CVC5_MSVC_DIR}/include/getopt.h")" || return 1
	cadical="$(dep_sha256 "${CVC5_MSVC_DIR}/cadical.cmake")" || return 1
	printf '%s\n' \
		"builder=clang-cl" \
		"vcpkg_repo=${CVC5_MSVC_VCPKG_REPO}" \
		"vcpkg_commit=${CVC5_MSVC_VCPKG_COMMIT}" \
		"gmp_port=${CVC5_MSVC_GMP_PORT}" \
		"cvc5_targets=cvc5" \
		"msvc_compat_h_hash=${compat}" \
		"msvc_unistd_h_hash=${unistd}" \
		"msvc_getopt_h_hash=${getopt}" \
		"msvc_cadical_cmake_hash=${cadical}"
}

# A unique folder with a short path for the vcpkg tree and the CaDiCaL build.
# Under the staging folder, their build paths pass the 260 characters cl.exe
# accepts.
_dep_cvc5_msvc_short_dir() {
	local base="${TMPDIR:-/tmp}"
	if [ -n "${RUNNER_TEMP:-}" ]; then
		base="$(cygpath -u "$RUNNER_TEMP")" || return 1
	fi
	mktemp -d "${base}/cvc5.XXXXXX"
}

# Overwrite <root> in every file under <dir> with '@' of the same length. The
# install step masks the staging paths the same way, but <root> is outside them.
_dep_cvc5_msvc_mask_path() {
	local root="$1" dir="$2"
	python3 - "$dir" "$root" "$(cygpath -w "$root")" "$(cygpath -m "$root")" <<'PY'
import os
import sys
top, *roots = sys.argv[1:]
olds = sorted({r.encode() for r in roots if r}, key=len, reverse=True)
for dirpath, _dirs, files in os.walk(top):
	for name in files:
		path = os.path.join(dirpath, name)
		with open(path, 'rb') as fh:
			data = fh.read()
		new = data
		for old in olds:
			new = new.replace(old, b'@' * len(old))
		if new != data:
			with open(path, 'wb') as fh:
				fh.write(new)
PY
}

# GMP from vcpkg into <deps>, with the vcpkg checkout in <vcpkg>. The vcpkg
# build uses cl.exe of the MSVC shell, so it runs before CL is set for clang-cl.
_dep_cvc5_msvc_gmp() {
	local vcpkg="$1" deps="$2" installed lib
	# Git for Windows refuses a path over 260 characters without core.longpaths.
	git init -q "$vcpkg" \
		&& git -C "$vcpkg" config core.longpaths true \
		&& git -C "$vcpkg" remote add origin "$CVC5_MSVC_VCPKG_REPO" \
		&& git -C "$vcpkg" fetch -q --depth 1 origin "$CVC5_MSVC_VCPKG_COMMIT" \
		&& git -C "$vcpkg" checkout -q FETCH_HEAD \
		|| { echo "dep-cvc5: cannot fetch vcpkg ${CVC5_MSVC_VCPKG_COMMIT}" >&2; return 1; }
	( cd "$vcpkg" && cmd.exe //c "$(cygpath -w "${vcpkg}/bootstrap-vcpkg.bat")" -disableMetrics ) \
		|| { echo "dep-cvc5: vcpkg bootstrap failed" >&2; return 1; }
	# The runner's own VCPKG_ROOT must not redirect this pinned checkout, and
	# Git Bash must not rewrite the port:triplet argument as a path list.
	( cd "$vcpkg" && env -u VCPKG_ROOT VCPKG_DISABLE_METRICS=1 MSYS2_ARG_CONV_EXCL='*' \
		./vcpkg.exe install "$CVC5_MSVC_GMP_PORT" ) \
		|| { echo "dep-cvc5: vcpkg install ${CVC5_MSVC_GMP_PORT} failed" >&2; return 1; }
	installed="${vcpkg}/installed/${CVC5_MSVC_GMP_PORT#*:}"
	mkdir -p "${deps}/include" "${deps}/lib" "${deps}/bin" || return 1
	cp "${installed}"/include/gmp*.h "${deps}/include/" \
		|| { echo "dep-cvc5: no GMP headers under ${installed}/include" >&2; return 1; }
	cp "${installed}"/lib/*.lib "${deps}/lib/" \
		|| { echo "dep-cvc5: no GMP libraries under ${installed}/lib" >&2; return 1; }
	cp "${installed}"/bin/*gmp*.dll "${deps}/bin/" \
		|| { echo "dep-cvc5: no GMP DLL under ${installed}/bin" >&2; return 1; }
	# The GMP source license files, else the vcpkg copyright file that joins them.
	local licenses=("${vcpkg}"/buildtrees/gmp/src/*/COPYING*)
	[ -f "${licenses[0]}" ] || licenses=("${installed}/share/gmp/copyright")
	mkdir -p "${deps}/share/licenses/gmp" \
		&& cp "${licenses[@]}" "${deps}/share/licenses/gmp/" \
		|| { echo "dep-cvc5: no GMP license file under ${vcpkg}" >&2; return 1; }
	# cvc5's FindGMP asks for gmp and gmpxx, which MSVC finds as gmp.lib and
	# gmpxx.lib only.
	for lib in gmp gmpxx; do
		if [ ! -f "${deps}/lib/${lib}.lib" ] && [ -f "${deps}/lib/lib${lib}.lib" ]; then
			cp "${deps}/lib/lib${lib}.lib" "${deps}/lib/${lib}.lib" || return 1
		fi
		if [ ! -f "${deps}/lib/${lib}.lib" ]; then
			echo "dep-cvc5: vcpkg gave no ${lib}.lib" >&2
			return 1
		fi
	done
}

# CaDiCaL from the pinned closure archive into <deps>, built in <cadical_build>.
# The archive stays under the cvc5 build tree, where the closure check reads it.
_dep_cvc5_msvc_cadical() {
	local work="$1" build="$2" deps="$3" cadical_build="$4"
	local entry name version url sha archive got src
	for entry in "${CVC5_EXPECTED_CLOSURE[@]}"; do
		IFS='|' read -r name version url sha <<< "$entry"
		[ "$name" = CaDiCaL ] && break
	done
	if [ "$name" != CaDiCaL ]; then
		echo "dep-cvc5: no CaDiCaL entry in the expected closure" >&2
		return 1
	fi
	mkdir -p "${build}/msvc-archives" "${work}/msvc-src" || return 1
	archive="${build}/msvc-archives/$(_dep_cvc5_url_archive "$url")"
	curl -fsSL "$url" -o "$archive" \
		|| { echo "dep-cvc5: CaDiCaL download failed" >&2; return 1; }
	got="$(dep_sha256 "$archive")" || return 1
	if [ "$got" != "$sha" ]; then
		echo "dep-cvc5: CaDiCaL archive hashes ${got}, expected ${sha}" >&2
		return 1
	fi
	tar -xzf - -C "${work}/msvc-src" < "$archive" \
		|| { echo "dep-cvc5: CaDiCaL extract failed" >&2; return 1; }
	src="${work}/msvc-src/cadical-${version}"
	[ -d "$src" ] || { echo "dep-cvc5: extracted tree not found: '${src}'" >&2; return 1; }
	cp "${CVC5_MSVC_DIR}/cadical.cmake" "${src}/CMakeLists.txt" || return 1
	env -u CPPFLAGS -u CXXFLAGS -u CFLAGS -u LDFLAGS -u CL \
		"$DEP_CVC5_CMAKE" -S "$src" -B "$cadical_build" -G Ninja \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_CXX_COMPILER="$DEP_CVC5_CXX" \
		-DCVC5_MSVC_DIR="$(cygpath -m "$CVC5_MSVC_DIR")" \
		-DCMAKE_INSTALL_PREFIX="$(cygpath -m "$deps")" \
		|| { echo "dep-cvc5: CaDiCaL configure failed" >&2
			_dep_cvc5_print_logs "$cadical_build" >&2; return 1; }
	env -u CPPFLAGS -u CXXFLAGS -u CFLAGS -u LDFLAGS -u CL \
		"$DEP_CVC5_CMAKE" --build "$cadical_build" -- -j "$CVC5_JOBS" \
		|| { echo "dep-cvc5: CaDiCaL build failed" >&2
			_dep_cvc5_print_logs "$cadical_build" >&2; return 1; }
	env -u CPPFLAGS -u CXXFLAGS -u CFLAGS -u LDFLAGS -u CL \
		"$DEP_CVC5_CMAKE" --install "$cadical_build" \
		|| { echo "dep-cvc5: CaDiCaL install failed" >&2; return 1; }
}

# CMAKE_PREFIX_PATH and CL carry the checkout and staging paths, so they travel
# in the environment of the configure and the build, not in the id.
_dep_cvc5_target_prebuild() {
	local work="$1" build="$2" deps="${1}/msvc-deps" short rc exit_trap outer=""
	short="$(_dep_cvc5_msvc_short_dir)" \
		|| { echo "dep-cvc5: cannot create a short build folder" >&2; return 1; }
	# The trap holds the path itself, as the local is gone when the trap runs,
	# and it keeps the command of an outer EXIT trap.
	exit_trap="$(trap -p EXIT)"
	[ -n "$exit_trap" ] && outer="$(eval "set -- $exit_trap"; printf '%s' "$3")"
	trap "rm -rf $(printf '%q' "$short")${outer:+; $outer}" EXIT
	_dep_cvc5_msvc_gmp "${short}/vcpkg" "$deps" \
		&& _dep_cvc5_msvc_cadical "$work" "$build" "$deps" "${short}/cadical" \
		&& { _dep_cvc5_msvc_mask_path "$short" "$deps" \
			|| { echo "dep-cvc5: cannot mask ${short} in ${deps}" >&2; false; }; }
	rc=$?
	rm -rf "$short"
	eval "${exit_trap:-trap - EXIT}"
	[ "$rc" -eq 0 ] || return 1
	export CMAKE_PREFIX_PATH
	CMAKE_PREFIX_PATH="$(cygpath -m "$deps")"
	# clang-cl reads extra options from CL. The compat header goes into every
	# file, because ssize_t reaches files that do not include unistd.h.
	export CL
	CL="-FI\"$(cygpath -m "${CVC5_MSVC_DIR}/compat.h")\" -I\"$(cygpath -m "${CVC5_MSVC_DIR}/include")\""
	export MSYS2_ENV_CONV_EXCL="${MSYS2_ENV_CONV_EXCL:+${MSYS2_ENV_CONV_EXCL};}CL;CMAKE_PREFIX_PATH"
}

# The install rules of the top folder and of src: the headers, the library and
# the package files. CMAKE_INSTALL_LOCAL_ONLY leaves out the rules of their
# subfolders, which install the parser library and the binary.
#
# The prefix is given again here: cvc5's configure.sh takes one that starts
# with a drive letter for a relative path and puts its own folder before it.
_dep_cvc5_target_install() {
	local build="$1" prefix dir
	prefix="$(cygpath -m "$2")" || return 1
	for dir in "$build" "${build}/src"; do
		"$DEP_CVC5_CMAKE" "-DCMAKE_INSTALL_PREFIX=${prefix}" \
			-DCMAKE_INSTALL_LOCAL_ONLY=ON \
			-P "$(cygpath -m "${dir}/cmake_install.cmake")" || return 1
	done
	_dep_cvc5_msvc_drop_parser_target "${prefix}/lib/cmake/cvc5"
}

# The exported target files of <dir> without cvc5::cvc5parser. find_package
# refuses a package whose target names a library that is not installed.
_dep_cvc5_msvc_drop_parser_target() {
	python3 - "$1" <<'PY'
import glob
import os
import re
import sys
target = 'cvc5::cvc5parser'
files = sorted(glob.glob(os.path.join(sys.argv[1], 'cvc5Targets*.cmake')))
if len(files) < 2:
	sys.exit('dep-cvc5: no exported target files under ' + sys.argv[1])
dropped = 0
for path in files:
	with open(path, newline='') as fh:
		text = fh.read()
	eol = '\r\n' if '\r\n' in text else '\n'
	# A block of the file is the lines between two empty ones: the parser
	# target fills whole blocks, and is one name in the list of all targets.
	blocks = []
	for block in text.split(eol + eol):
		block = re.sub(r'(foreach\(_cmake_expected_target IN ITEMS[^)\r\n]*?) '
			+ target + r'\b', r'\1', block)
		if target in block:
			dropped += 1
			continue
		blocks.append(block)
	text = (eol + eol).join(blocks)
	if 'cvc5parser' in text:
		sys.exit('dep-cvc5: the parser target is still named in ' + path)
	with open(path, 'w', newline='') as fh:
		fh.write(text)
if dropped < 3:
	sys.exit('dep-cvc5: expected the parser target in 3 blocks of the '
		'exported target files, dropped %d' % dropped)
PY
}

_dep_cvc5_target_gmp_licenses() {
	local dir="$1" work="$2"
	mkdir -p "$dir" && cp "${work}/msvc-deps/share/licenses/gmp/"* "$dir/"
}

# The GMP DLLs go beside cvc5.dll, where configure copies every DLL from.
_dep_cvc5_target_postinstall() {
	local prefix="$1" work="$2"
	mkdir -p "${prefix}/bin" || return 1
	cp "${work}/msvc-deps/bin/"*gmp*.dll "${prefix}/bin/" \
		|| { echo "dep-cvc5: no GMP DLL under ${work}/msvc-deps/bin" >&2; return 1; }
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/cvc5.sh"
