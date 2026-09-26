#!/bin/bash
# Build and cache the native Linux Boost package in the LOCAL store.
#
#   ./dev dep-boost-package -DTAU_DEP_CC=clang -DTAU_DEP_CXX=clang++ \
#       -DTAU_BUILD_JOBS=5
#   ./dev dep-boost-package -DTAU_DEP_MODE=consumer -DTAU_BUILD_JOBS=5
#
# The source is pinned to one immutable commit (Boost 1.86.0).
#
# Only the native, cross-toolchain, macOS and MSVC tuples are produced; any
# other target or host is rejected. The wasm variants have their own threading
# and exception-encoding inputs, which this identity records explicitly.

set -u

DEV_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${DEV_ROOT}/scripts/devrc"

DEP_BOOST_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
BOOST_DEFAULT_REPO="https://github.com/boostorg/boost.git"
BOOST_DEFAULT_COMMIT="65c1319bb92fe7a9a4abd588eff5818d9c2bccf9"
BOOST_TOOLSET="gcc"
BOOST_LINK_MODE="static,shared"
BOOST_THREADING="multi"
# Native has no wasm exception encoding; the field exists so the wasm variants
# can reuse this schema. The legacy .tau-eh-abi stamp covers only this option.
BOOST_EXCEPTION_ENCODING="none"
BOOST_CXX_STANDARD="17"

# The depinst.py log closure of the pinned commit. Each entry is
# "<submodule path>@<commit>". The producer resolves the closure in its own work
# tree, fails if a required submodule is missing or dirty, and fails if the
# resolved set differs from this. This is the Boost analogue of the cvc5 closure
# table; the recorded value is the verified one.
BOOST_EXPECTED_CLOSURE=(
	"libs/algorithm@32c5a6327cfdca5d41ce0f1d8849b811886daa2f"
	"libs/align@5ad7df63cd792fbdb801d600b93cad1a432f0151"
	"libs/any@aee53090c04bd0a263480fae25fddddcef54ca82"
	"libs/array@23f6b27c0d9916b9932baac898ae3009817a9153"
	"libs/asio@c28d453674dd2071fdc8cce5ffabcb54c910f466"
	"libs/assert@242fdfb1373127331cb229379b50b1dba0f53e49"
	"libs/atomic@fbdb5f44d4ace030d8a8836cc4f3738bcc1c6d72"
	"libs/bind@97776ec7ba6217ecdfb8fd777dec85117de6f188"
	"libs/chrono@ee0d6d543a37d9b7243682549e9ae359eb89daa9"
	"libs/concept_check@37c9bddf0bdefaaae0ca5852c1a153d9fc43f278"
	"libs/config@2bd6f0dfc52ff1fb1bd502361f4d0c750ce51d93"
	"libs/container@e08a064896048daa306b82d7525e5b524051fb42"
	"libs/container_hash@6d214eb776456bf17fbee20780a034a23438084f"
	"libs/context@1bde50e400547e29336afe7ea0cd693d8c884fb6"
	"libs/conversion@9f285ef0c43c101e49b37bf5e6085e8d635887dc"
	"libs/core@622476533c60b979d0b6a406d601c88219092cdb"
	"libs/coroutine@1e1347c0b1910b9310ec1719edad8b0bf2fd03c8"
	"libs/date_time@85e637cb325208c2af9af791c3a1948b4888c6cd"
	"libs/describe@50719b212349f3d1268285c586331584d3dbfeb5"
	"libs/detail@804767983fb6f77f8e0fd975f23d1facee6f224b"
	"libs/dynamic_bitset@8e20aa1462bf6dcadc338835df529a6d568431b1"
	"libs/endian@900fe18a117daa9ce023529b006f122ad4116749"
	"libs/exception@ece51fe0f7b8273ffc3fa626c3d1442f7af24ef9"
	"libs/filesystem@41a990ef14760d487c8c3b234f5e3e59178140df"
	"libs/function@1204b7d20091c1ac65fb02a1d5991a82098e4f77"
	"libs/function_types@895335874d67987ada0d8bf6ca1725e70642ed49"
	"libs/functional@6a573e4b8333ee63ee62ce95558c3667348db233"
	"libs/fusion@53da4e0b2c8cd4969b09fa646d314ac0c414c1ff"
	"libs/headers@5c0228ed90ff32a54ba0b015ddae2b170062dcb1"
	"libs/integer@c37a3402beff5da04790d054bd7292920b433046"
	"libs/interprocess@9fc4ee7dd7277d86c691921007856aca99c3c1ac"
	"libs/intrusive@b9b83bd9e691cde4f116d8a0bd4e09b91e70ce4f"
	"libs/io@342e4c6d10d586058818daa84201a2d301357a53"
	"libs/iterator@4914e1f40abf6ba1e6e2ad196cdac557b0783cf3"
	"libs/lexical_cast@ff2f0496b02f8b186b737b35693b446041aa545d"
	"libs/log@9a906e2f540680ff35a20207f9a1a9a3b889f52b"
	"libs/move@70c57e804f6c287c0ee3ab39c2ef1bb9ec9c02e5"
	"libs/mp11@d0f3481b210a3baf49ce16d7b595ba5f4bee8ce6"
	"libs/mpl@b37b709cbdb6b2c285fb808dab985aa005786351"
	"libs/multi_index@d702a48363997f65fb717346f6694e0703902f8a"
	"libs/numeric/conversion@50a1eae942effb0a9b90724323ef8f2a67e7984a"
	"libs/optional@c60db27762ff9cc16529e069c3c15f2fa898f994"
	"libs/parameter@ea493c7cd7703be5ccaac01bcb6ccc781dd03b91"
	"libs/phoenix@8913607a3788cb82d48ed461ea59c919b7bad3df"
	"libs/pool@ec7da07ed13e0c61e50d945b574a12ae7ec83cf4"
	"libs/predef@9aca7f5b609a731106a6d70e8dca9a4196dca968"
	"libs/preprocessor@c4ea7e40d365ba28faecef8917d5c3f1e0121bf9"
	"libs/property_tree@8080ecd14f2d952a4bb7ae869fc0f54f54f5a31f"
	"libs/proto@7f924934689b940f3a72212ab0f714ec6fd6e34b"
	"libs/random@29ac88879985418c6284587485d0ad8c36e33cbc"
	"libs/range@2bb6b636796f7b008196888613f51f5bb347c77d"
	"libs/ratio@d5b33caa7d564be9be6d962b18659b7741d764ac"
	"libs/regex@4cbcd3078e6ae10d05124379623a1bf03fcb9350"
	"libs/scope@db963eab9cd9244edde0488423effbcb8200ea62"
	"libs/serialization@a20c4d97c37e5f437c8ba78f296830edb79cff9e"
	"libs/smart_ptr@1650077eee2c62d05d9deeff06a75c058648101a"
	"libs/spirit@43112c3f055fa109f20b8e8e6e50d0a016da742a"
	"libs/static_assert@ba72d3340f3dc6e773868107f35902292f84b07e"
	"libs/system@5d5ecb74a7eabbc7968a6a7d8dcf17c291649105"
	"libs/test@4161c7be9806c09c2fea50c0215bd8bd7ff4997e"
	"libs/thread@aec18d337f41d8e3081ee65f5cf3b5090179ab0e"
	"libs/throw_exception@6ff2e5dca53fc50bef54ef3eeaaed83d35cbc061"
	"libs/tokenizer@90106f155bd72b62aaca0d9ad826f4132030dba0"
	"libs/tuple@b67941dd7d03536a854b96f001954792311ab515"
	"libs/type_index@e37bc99e85e85bcac420ac1d4c1a8a5bca280d47"
	"libs/type_traits@cc6fc3daa8e20c4a70b563d67ceb4c34d8c1c4d7"
	"libs/typeof@4bc9de322cd44373435540d4e6c8f207892fc9a0"
	"libs/unordered@d61719a196a6022d9dc68f5118c16c881bb5a0d4"
	"libs/utility@31de9279d826c8a6ae6126c0c4a8a32ea9f0edff"
	"libs/variant@b33ff4f811a27d4dc4a4c8a840310523de4df4f8"
	"libs/variant2@f9bdafd3ca0f5025012f60a405b559888513a9be"
	"libs/winapi@c5fb9c86e8b9ea460fc6d7255d15d69d4d2705ae"
	"libs/xpressive@4679fbd23f962bfa78d44acf5fa48f6f790642c0"
	"tools/boost_install@3f67136ec1e7855f29c40db8949e763ff73c840f"
	"tools/boostdep@8f9cb2ef559fbe760f6522ddb8f693c02b83c032"
	"tools/build@4a52d8c06635435b64e31a56eaf7ca5dc912a71d"
	"tools/cmake@ae2e6a647187246d6009f80b56ba4c2c8f3a008c"
)

_dep_boost_compiler_id() {
	dep_compiler_id "$DEP_BOOST_CXX"
}

# Print the resolved closure of a work tree as sorted "<path>@<commit>" lines.
_dep_boost_closure_actual() {
	local work="$1"
	( cd "$work" && git submodule status --recursive 2>/dev/null \
		| grep -v '^-' | awk '{print $2"@"$1}' | LC_ALL=C sort )
}

_dep_boost_field_block() {
	local build_helper publish_helper manifest store
	local recipe_hash build_hash publish_hash manifest_hash store_hash
	local closure=""
	local entry
	build_helper="${__devrc_dir}/dep-build"
	publish_helper="${__devrc_dir}/devrc"
	manifest="${__devrc_dir}/../cmake/tau-manifest.cmake"
	store="${__devrc_dir}/../cmake/tau-store.cmake"
	recipe_hash="$(dep_sha256 "$DEP_BOOST_RECIPE")" || return 1
	build_hash="$(dep_sha256 "$build_helper")" || return 1
	publish_hash="$(dep_sha256 "$publish_helper")" || return 1
	manifest_hash="$(dep_sha256 "$manifest")" || return 1
	store_hash="$(dep_sha256 "$store")" || return 1
	for entry in "${BOOST_EXPECTED_CLOSURE[@]}"; do
		closure="${closure}${closure:+;}${entry}"
	done
	printf '%s\n' \
		"dep=boost" \
		"target=${DEP_BOOST_TARGET:-native}" \
		"repo=${BOOST_REPO}" \
		"commit=${BOOST_COMMIT}" \
		"recipe_hash=${recipe_hash}" \
		"helper_build_hash=${build_hash}" \
		"provenance.publish_helper_hash=${publish_hash}" \
		"provenance.manifest_writer_hash=${manifest_hash}" \
		"provenance.store_writer_hash=${store_hash}" \
		"submodule_closure=${closure}" \
		"b2_args=${_DEP_BOOST_B2_ARGS[*]}" \
		"toolset=${BOOST_TOOLSET}" \
		"link_mode=${BOOST_LINK_MODE}" \
		"threading=${BOOST_THREADING}" \
		"exception_encoding=${BOOST_EXCEPTION_ENCODING}" \
		"cxx_standard=${BOOST_CXX_STANDARD}" \
		"cflags=${DEP_BOOST_CFLAGS}" \
		"cxxflags=${DEP_BOOST_CXXFLAGS}" \
		"compiler_id=$(_dep_boost_compiler_id)" \
		"compiler_version=$(dep_compiler_version "$DEP_BOOST_CXX")" \
		"target_triple=$(dep_compiler_triple "$DEP_BOOST_CXX" "${DEP_BOOST_TARGET:-native}")" \
		"os=$(uname -s)" \
		"arch=$(uname -m)" \
		"os_release_hash=$(dep_os_release_hash)" \
		"libc_version=$(dep_libc_version)" \
		"build_type=Release" \
		"pic=ON" \
		"lto=OFF" \
		"sanitizer=OFF"
}

# Verify the work tree's closure against the expected one. Fails on a missing or
# extra submodule, a commit mismatch, or a dirty treated submodule.
_dep_boost_verify_closure() {
	local work="$1" entry commit path actual expected
	expected="$(printf '%s\n' "${BOOST_EXPECTED_CLOSURE[@]}" | LC_ALL=C sort)"
	actual="$(_dep_boost_closure_actual "$work")" || return 1
	if [ "$actual" != "$expected" ]; then
		echo "dep-boost: resolved submodule closure differs from the expected closure" >&2
		diff <(printf '%s\n' "$expected") <(printf '%s\n' "$actual") >&2 || true
		return 1
	fi
	for entry in "${BOOST_EXPECTED_CLOSURE[@]}"; do
		path="${entry%@*}"
		if [ ! -e "${work}/${path}/.git" ] && [ ! -e "${work}/${path}/.gitdir" ]; then
			# .git may be a file; existence of the dir is enough
			[ -d "${work}/${path}" ] || { echo "dep-boost: missing submodule ${path}" >&2; return 1; }
		fi
		if [ -n "$(git -C "${work}/${path}" status --porcelain 2>/dev/null)" ]; then
			echo "dep-boost: submodule ${path} has a dirty working tree" >&2
			return 1
		fi
	done
	return 0
}

# Producer callback. $1 is the staging prefix. Source, b2, bootstrap output and
# bin.v2 live in <staging>/work, outside prefix/, and are removed on success.
_dep_boost_producer() {
	local staging_prefix="$1"
	local staging work cxx
	cxx="$DEP_BOOST_CXX"
	staging="$(dirname "$staging_prefix")"
	work="${staging}/work"
	rm -rf "$work"
	mkdir -p "$work" "$staging_prefix"
	git init -q "$work" || { rm -rf "$work"; return 1; }
	git -C "$work" remote add origin "$BOOST_REPO" || { rm -rf "$work"; return 1; }
	git -C "$work" fetch -q --depth 1 origin "$BOOST_COMMIT" \
		|| { rm -rf "$work"; return 1; }
	git -C "$work" checkout -q FETCH_HEAD || { rm -rf "$work"; return 1; }
	local head
	head="$(git -C "$work" rev-parse HEAD)"
	if [ "$head" != "$BOOST_COMMIT" ]; then
		echo "dep-boost: checkout is ${head}, expected ${BOOST_COMMIT}" >&2
		rm -rf "$work"
		return 1
	fi
	# Resolve the depinst closure inside the work tree, never a shared source.
	git -C "$work" submodule update --init tools/boostdep tools/build tools/cmake libs/log \
		|| { echo "dep-boost: cannot init boostdep/build/cmake/log" >&2; rm -rf "$work"; return 1; }
	( cd "$work" && python3 tools/boostdep/depinst/depinst.py log ) \
		|| { echo "dep-boost: depinst.py log failed" >&2; rm -rf "$work"; return 1; }
	_dep_boost_verify_closure "$work" || { rm -rf "$work"; return 1; }
	echo "dep-boost: resolved closure has $(printf '%s\n' "${BOOST_EXPECTED_CLOSURE[@]}" | wc -l) submodules" >&2
	# Bootstrap with an explicit toolset. The compiler is pinned through a
	# user-config so the recorded toolset is not the host default.
	local b2_bin="${work}/b2"
	if [ "$DEP_BOOST_TARGET" = "w64" ]; then
		cat > "${work}/user-config.jam" <<EOF
using gcc : mingw64 : ${cxx}
        :
        <rc>x86_64-w64-mingw32-windres
        <archiver>x86_64-w64-mingw32-ar
;
EOF
	elif [ "$DEP_BOOST_TARGET" = "win-msvc-x64" ]; then
		# b2's msvc toolset finds cl through the developer environment the
		# runner set up; naming the compiler path here would freeze a version.
		cat > "${work}/user-config.jam" <<EOF
using msvc ;
EOF
	elif [ "$DEP_BOOST_TARGET" = "wasm" ]; then
		EMSCRIPTEN_DIR="${TAU_SHARED_PREFIX}/emsdk/upstream/emscripten"
		cat > "${work}/user-config.jam" <<EOF
using emscripten : : ${EMSCRIPTEN_DIR}/em++ ;
EOF
	else
		cat > "${work}/user-config.jam" <<EOF
using ${BOOST_TOOLSET} : : ${cxx} ;
EOF
	fi
	if [ "$DEP_BOOST_TARGET" = "win-msvc-x64" ]; then
		# bootstrap.bat + b2 in one cmd session, so both inherit the vcvars
		# environment. Git Bash rewrites `/c`, `/d` and `--`-switches on a
		# cmd.exe command line as filesystem paths; keeping them in a .bat
		# lets cmd parse them itself. vcvars64.bat is a local fallback only:
		# CI gets the MSVC shell from an action, so cl is already on PATH and
		# a missing vcvars is not fatal.
		local _boost_win _prefix_win _build_win _vcvars _v
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
		b2_bin="${work}/b2.exe"
		cmd.exe //c "$(cygpath -w "$_bat")" \
			|| { echo "dep-boost: MSVC bootstrap+b2 failed" >&2; rm -rf "$work"; return 1; }
	else
		if [ "$(dep_host_os)" = windows ]; then
			# bootstrap.bat has no --with-toolset; the toolset arrives
			# through the user-config passed to b2 below.
			b2_bin="${work}/b2.exe"
			( cd "$work" && cmd //c bootstrap.bat --with-libraries=log \
				> "${work}/bootstrap.log" 2>&1 ) \
				|| { echo "dep-boost: bootstrap failed" >&2; tail -20 "${work}/bootstrap.log" >&2; rm -rf "$work"; return 1; }
		else
			( cd "$work" && ./bootstrap.sh --with-toolset=${BOOST_TOOLSET} --with-libraries=log \
				> "${work}/bootstrap.log" 2>&1 ) \
				|| { echo "dep-boost: bootstrap failed" >&2; tail -20 "${work}/bootstrap.log" >&2; rm -rf "$work"; return 1; }
		fi
		local b2_version
		b2_version="$("$b2_bin" --version 2>&1 | head -n 1)"
		echo "dep-boost: ${b2_version} (bootstrapped with toolset ${BOOST_TOOLSET})" >&2
		( cd "$work" && "$b2_bin" --user-config=./user-config.jam \
			--prefix="$staging_prefix" --build-dir="${work}/bin.v2" \
			"${_DEP_BOOST_B2_ARGS[@]}" -j "$DEP_BOOST_JOBS" install ) \
			|| { echo "dep-boost: b2 build failed" >&2; rm -rf "$work"; return 1; }
	fi
	# Boost's generated CMake configs bake the staging prefix into an
	# if(EXISTS ...) block. Derive the prefix from the config file's own location
	# instead, so a relocated package resolves itself. Every config sits at
	# <prefix>/lib/cmake/<dir>/, so ../../.. is the prefix.
	while IFS= read -r -d '' cfg; do
		dep_sed_inplace "$cfg" \
			"s|${staging_prefix}|\${CMAKE_CURRENT_LIST_DIR}/../../..|g"
	done < <(find "$staging_prefix/lib/cmake" -type f -name '*.cmake' -print0 2>/dev/null)
	if [ -f "${work}/LICENSE_1_0.txt" ]; then
		mkdir -p "$staging_prefix/share/licenses/boost"
		cp "${work}/LICENSE_1_0.txt" "$staging_prefix/share/licenses/boost/LICENSE_1_0.txt"
	else
		echo "dep-boost: no LICENSE_1_0.txt in the boost source" >&2
		rm -rf "$work"
		return 1
	fi
	rm -rf "$work"
	return 0
}

dep_entry "$@"

case "${DEP_TARGET:-native}" in
	native|w64|wasm|darwin-arm64|darwin-x86_64|win-msvc-x64) ;;
	*)
		echo "dep-boost: unknown target '${DEP_TARGET}'" >&2
		exit 2
		;;
esac
dep_require_target_host dep-boost "${DEP_TARGET:-native}"

mode="$(dep_var TAU_DEP_MODE producer)"
case "$mode" in
	producer|consumer) ;;
	*)
		echo "dep-boost: -DTAU_DEP_MODE must be producer or consumer, got '${mode}'" >&2
		exit 2
		;;
esac

BOOST_REPO="$(dep_var BOOST_REPO "$BOOST_DEFAULT_REPO")"
BOOST_COMMIT="$(dep_var BOOST_COMMIT "$BOOST_DEFAULT_COMMIT")"
case "$BOOST_COMMIT" in
	*[!0-9a-f]*|"")
		echo "dep-boost: BOOST_COMMIT must be 40 lowercase hex characters, got '${BOOST_COMMIT}'" >&2
		exit 2
		;;
esac
if [ "${#BOOST_COMMIT}" -ne 40 ]; then
	echo "dep-boost: BOOST_COMMIT must be 40 lowercase hex characters, got '${BOOST_COMMIT}'" >&2
	exit 2
fi
DEP_BOOST_JOBS="$(dep_jobs)"

DEP_BOOST_CC="$(dep_var TAU_DEP_CC "")"
DEP_BOOST_CXX="$(dep_var TAU_DEP_CXX "")"
if [ -z "$DEP_BOOST_CC" ] || [ -z "$DEP_BOOST_CXX" ]; then
	echo "dep-boost: no compiler; pass -DTAU_DEP_CC and -DTAU_DEP_CXX" >&2
	exit 2
fi
DEP_BOOST_CFLAGS="$(dep_var TAU_DEP_CFLAGS "")"
DEP_BOOST_CXXFLAGS="$(dep_var TAU_DEP_CXXFLAGS "")"
case "$(dep_compiler_id "$DEP_BOOST_CXX")" in
	GNU) BOOST_TOOLSET="gcc" ;;
	Clang|AppleClang) BOOST_TOOLSET="clang" ;;
	MSVC) BOOST_TOOLSET="msvc" ;;
	*) echo "dep-boost: unknown compiler '$DEP_BOOST_CXX'" >&2; exit 2 ;;
esac
DEP_BOOST_TARGET="${DEP_TARGET:-native}"
DEP_BOOST_TARGET_OS="linux"
DEP_BOOST_B2_TOOLSET="${BOOST_TOOLSET}"
DEP_BOOST_B2_LINK="${BOOST_LINK_MODE}"
DEP_BOOST_B2_THREADING="${BOOST_THREADING}"
DEP_BOOST_B2_ADDRESS_MODEL="64"
DEP_BOOST_B2_PIC=" -fPIC"
case "$DEP_BOOST_TARGET" in
	w64)
		DEP_BOOST_TARGET_OS="windows"
		;;
	darwin-arm64|darwin-x86_64)
		# b2 names the macOS variant clang-darwin from the toolset plus this.
		DEP_BOOST_TARGET_OS="darwin"
		# A dylib's install_name would bake the staging prefix and fail the
		# path audit; Tau links Boost statically anyway.
		BOOST_LINK_MODE="static"
		DEP_BOOST_B2_LINK="static"
		;;
	win-msvc-x64)
		DEP_BOOST_TARGET_OS="windows"
		DEP_BOOST_B2_PIC=""
		# --layout=system names the static and the shared library identically,
		# so install static only (Tau links Boost statically).
		BOOST_LINK_MODE="static"
		DEP_BOOST_B2_LINK="static"
		;;
	wasm)
		# em++ rejects the -m64 that address-model=64 becomes, and needs no PIC.
		DEP_BOOST_TARGET_OS=""
		DEP_BOOST_B2_TOOLSET="emscripten"
		DEP_BOOST_B2_LINK="static"
		DEP_BOOST_B2_ADDRESS_MODEL="32"
		DEP_BOOST_B2_PIC=""
		case " $DEP_BOOST_CXXFLAGS " in
			*" -pthread "*) DEP_BOOST_B2_THREADING="multi" ;;
			*) DEP_BOOST_B2_THREADING="single" ;;
		esac
		;;
esac

_DEP_BOOST_B2_ARGS=(
	"toolset=${DEP_BOOST_B2_TOOLSET}"
	"variant=release"
	"address-model=${DEP_BOOST_B2_ADDRESS_MODEL}"
	"threading=${DEP_BOOST_B2_THREADING}"
	"link=${DEP_BOOST_B2_LINK}"
	"cflags=${DEP_BOOST_CFLAGS}${DEP_BOOST_B2_PIC}"
	"cxxflags=${DEP_BOOST_CXXFLAGS}${DEP_BOOST_B2_PIC}"
)
if [ -n "$DEP_BOOST_TARGET_OS" ]; then
	_DEP_BOOST_B2_ARGS+=("target-os=${DEP_BOOST_TARGET_OS}")
fi
if [ "$DEP_BOOST_TARGET" = "wasm" ] || [ "$DEP_BOOST_TARGET" = "win-msvc-x64" ]; then
	_DEP_BOOST_B2_ARGS+=("define=BOOST_LOG_WITHOUT_SYSLOG")
fi

block="$(_dep_boost_field_block)" || {
	echo "dep-boost: cannot build the identity field block" >&2
	exit 1
}

out=""
if ! dep_ensure out "$mode" boost "$block" _dep_boost_producer; then
	echo "dep-boost: ${mode} failed" >&2
	exit 1
fi

echo "dep-boost: package prefix: ${out}"
