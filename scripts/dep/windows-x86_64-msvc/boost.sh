#!/bin/bash
# The Boost package for windows-x86_64-msvc.
# scripts/dep/common/boost.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=windows-x86_64-msvc
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# b2's msvc toolset finds cl through the developer environment the
# runner set up; naming the compiler path here would freeze a version.
_dep_boost_user_config() {
	cat > "${1}/user-config.jam" <<EOF
using msvc ;
EOF
}

# Overwrite each <folder> in every file under <prefix> with '@' of the same
# length: MSVC has no flag that maps a path, and each library keeps the path
# of its objects and sources. A folder is masked under each of its spellings,
# with either slash and with its long and its 8.3 name.
_dep_boost_msvc_mask_paths() {
	local prefix="$1" dir spellings=()
	shift
	for dir in "$@"; do
		spellings+=("$dir" "$(cygpath -w "$dir")" "$(cygpath -m "$dir")"
			"$(cygpath -wl "$dir")" "$(cygpath -ml "$dir")")
	done
	python3 - "$prefix" "${spellings[@]}" <<'PY'
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

# bootstrap.bat + b2 in one cmd session, so both inherit the vcvars
# environment. Git Bash rewrites `/c`, `/d` and `--`-switches on a
# cmd.exe command line as filesystem paths; keeping them in a .bat
# lets cmd parse them itself. vcvars64.bat is a local fallback only:
# CI gets the MSVC shell from an action, so cl is already on PATH and
# a missing vcvars is not fatal.
#
# b2 builds in a folder with a short path: under the staging folder, the
# paths of its objects and of the cmake files it writes pass the 260
# characters cl.exe and cmd accept.
_dep_boost_target_build() {
	local work="$1" staging_prefix="$2"
	local _boost_win _prefix_win _build_win _tmp_win _vcvars _v _a _short _rc
	local _base="${TMPDIR:-/tmp}"
	if [ -n "${RUNNER_TEMP:-}" ]; then
		_base="$(cygpath -u "$RUNNER_TEMP")" || return 1
	fi
	_short="$(mktemp -d "${_base}/boost.XXXXXX")" \
		|| { echo "dep-boost: cannot create a short build folder" >&2; return 1; }
	_boost_win="$(cygpath -w "$work")"
	_prefix_win="$(cygpath -w "$staging_prefix")"
	_build_win="$(cygpath -w "$_short")"
	mkdir -p "${_short}/tmp" || return 1
	_tmp_win="$(cygpath -w "${_short}/tmp")"
	_vcvars=""
	for _v in \
		"${VSINSTALLDIR:-}/VC/Auxiliary/Build/vcvars64.bat" \
		"/c/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Auxiliary/Build/vcvars64.bat" \
		"/c/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" \
		"/c/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvars64.bat"
	do
		[ -f "$_v" ] || continue
		_vcvars="$(cygpath -w "$_v" 2>/dev/null || echo "$_v")"
		break
	done
	local _bat="${work}/_tau_msvc_build.bat"
	{
		printf '@echo off\r\n'
		if [ -n "$_vcvars" ]; then
			echo "dep-boost: using vcvars $_vcvars" >&2
			printf 'call "%s"\r\n' "$_vcvars"
			printf 'if errorlevel 1 exit /b 1\r\n'
		fi
		# The librarian names a member it converts after a file in TEMP,
		# so TEMP is under the short folder, whose path is masked.
		printf 'set "TEMP=%s"\r\nset "TMP=%s"\r\n' "$_tmp_win" "$_tmp_win"
		printf 'cd /d "%s"\r\n' "$_boost_win"
		printf 'if errorlevel 1 exit /b 1\r\n'
		printf 'call bootstrap.bat --with-libraries=log\r\n'
		printf 'if errorlevel 1 exit /b 1\r\n'
		printf 'b2.exe --user-config=./user-config.jam'
		printf ' --prefix="%s" --build-dir="%s"' \
			"$_prefix_win" "$_build_win"
		printf ' --with-log --layout=system -j%s' "$DEP_BOOST_JOBS"
		# An argument with a space, as a cxxflags list, stays one argument
		# of b2 only between quotes.
		for _a in "${_DEP_BOOST_B2_ARGS[@]}"; do
			case "$_a" in
				*[[:space:]]*) printf ' "%s"' "$_a" ;;
				*) printf ' %s' "$_a" ;;
			esac
		done
		printf ' install\r\n'
	} > "$_bat"
	cmd.exe //c "$(cygpath -w "$_bat")"
	_rc=$?
	if [ "$_rc" -eq 0 ]; then
		_dep_boost_msvc_mask_paths "$staging_prefix" "$_short" \
				"$(dirname "$staging_prefix")" \
			|| { echo "dep-boost: cannot mask the build paths" >&2; _rc=1; }
	fi
	rm -rf "$_short"
	[ "$_rc" -eq 0 ] \
		|| { echo "dep-boost: MSVC bootstrap+b2 failed" >&2; return 1; }
}

_dep_boost_target_setup() {
	DEP_BOOST_TARGET_OS="windows"
	DEP_BOOST_B2_PIC=""
	# --layout=system names the static and the shared library identically,
	# so install static only (Tau links Boost statically).
	BOOST_LINK_MODE="static"
	DEP_BOOST_B2_LINK="static"
	DEP_BOOST_B2_DEFINE="BOOST_LOG_WITHOUT_SYSLOG"
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/boost.sh"
