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

# bootstrap.bat + b2 in one cmd session, so both inherit the vcvars
# environment. Git Bash rewrites `/c`, `/d` and `--`-switches on a
# cmd.exe command line as filesystem paths; keeping them in a .bat
# lets cmd parse them itself. vcvars64.bat is a local fallback only:
# CI gets the MSVC shell from an action, so cl is already on PATH and
# a missing vcvars is not fatal.
_dep_boost_target_build() {
	local work="$1" staging_prefix="$2"
	local _boost_win _prefix_win _build_win _vcvars _v _a
	_boost_win="$(cygpath -w "$work")"
	_prefix_win="$(cygpath -w "$staging_prefix")"
	_build_win="$(cygpath -w "${work}/bin.v2")"
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
		printf 'cd /d "%s"\r\n' "$_boost_win"
		printf 'if errorlevel 1 exit /b 1\r\n'
		printf 'call bootstrap.bat --with-libraries=log\r\n'
		printf 'if errorlevel 1 exit /b 1\r\n'
		printf 'b2.exe --user-config=./user-config.jam'
		printf ' --prefix="%s" --build-dir="%s"' \
			"$_prefix_win" "$_build_win"
		printf ' --with-log --layout=system -j%s' "$DEP_BOOST_JOBS"
		for _a in "${_DEP_BOOST_B2_ARGS[@]}"; do
			printf ' %s' "$_a"
		done
		printf ' install\r\n'
	} > "$_bat"
	cmd.exe //c "$(cygpath -w "$_bat")" \
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
