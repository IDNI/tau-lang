#!/bin/bash

# Thin wrapper around cmake/tau-compile.cmake: `./dev compile <dir> [options]`
# builds an artifact directory `tau gen` emitted and copies the executable,
# through the same script `tau compile` runs. The first positional is the
# artifact directory; --preset, -D and -G are the build-selection options
# `./dev preset` takes, parsed by the shared argument loop.
#
# Without --preset the build is native and uses <source>/build/release/sdk.
# With --preset it uses the platform SDK, <source>/build/<platform>/sdk, then
# the box an install prefix carries at lib/tau/sdk/<platform>/lib/cmake/Tau.
# TAU_SDK_DIR names the SDK in both cases.

source "$(dirname "${BASH_SOURCE[0]}")/env"

usage() {
	echo "usage: ./dev compile <artifact-dir> [--preset <name>]" >&2
	echo "                     [-D NAME=VALUE]... [-G <generator>]" >&2
	echo "" >&2
	echo "  --preset  target platform or tau preset name; without it the build" >&2
	echo "            is native and uses build/release/sdk" >&2
	echo "  -D        cmake cache variable for the emitted project's configure" >&2
	echo "  -G        cmake generator for the emitted project's configure" >&2
	echo "  TAU_SDK_DIR selects the SDK; it defaults to build/release/sdk, or" >&2
	echo "              to build/<platform>/sdk with --preset." >&2
}

# --preset is not part of the shared parser, so pull it out before it runs.
preset=""
args=()
while [[ $# -gt 0 ]]; do
	case "$1" in
		--preset)
			if [[ $# -lt 2 ]]; then
				echo "Missing value for --preset" >&2
				usage
				exit 2
			fi
			preset="$2"; shift 2
			;;
		--preset=*) preset="${1#--preset=}"; shift ;;
		*) args+=("$1"); shift ;;
	esac
done

# The shared loop preset.sh uses decides -D, -G and the positionals.
normalize_args "${args[@]}"
if [[ ${#DEV_POSITIONAL[@]} -lt 1 ]]; then
	usage
	exit 2
fi
if [[ ${#DEV_POSITIONAL[@]} -gt 1 ]]; then
	echo "Unknown compile argument: ${DEV_POSITIONAL[1]}" >&2
	exit 2
fi
artifact_dir="${DEV_POSITIONAL[0]}"
[[ "$artifact_dir" = /* ]] || artifact_dir="$(pwd)/${artifact_dir}"

# The platform a --preset name builds in, read from the map every SDK carries
# (cmake/tau-platforms.cmake, written by tau_write_platform_map). Prefer the
# SDK TAU_SDK_DIR names; otherwise any SDK built in the tree serves, since the
# map is the same in each. Without a map the name is the platform, so a tree
# that has no SDK at all still names the one to build.
platform=""
if [[ -n "$preset" ]]; then
	map_file=""
	if [[ -n "${TAU_SDK_DIR:-}" && -f "${TAU_SDK_DIR}/cmake/tau-platforms.cmake" ]]; then
		map_file="${TAU_SDK_DIR}/cmake/tau-platforms.cmake"
	else
		for candidate in "${REPO_ROOT}"/build/*/sdk/cmake/tau-platforms.cmake; do
			[[ -f "$candidate" ]] && { map_file="$candidate"; break; }
		done
	fi
	if [[ -n "$map_file" ]]; then
		entry="$(grep -F "\"${preset}|" "$map_file" | head -n1)"
		if [[ -n "$entry" ]]; then
			platform="${entry#*\"${preset}|}"
			platform="${platform%%\"*}"
		else
			platform="$preset"
		fi
	else
		# No SDK carries the map: ./dev preset's own mapping still names the
		# folder of a preset, and a plain name is a platform itself.
		build_dir="$(preset_binary_dir "$preset")"
		if [[ -n "$build_dir" && "$build_dir" != "build" ]]; then
			platform="${build_dir#build/}"
		else
			platform="$preset"
		fi
	fi
fi

# TAU_SDK_DIR names the SDK the script runs from. A native build defaults to
# the source tree's release SDK; a preset build to the platform's own.
sdk_suffix=""
case "$platform" in
	*-w64)  sdk_suffix="-windows-x86_64-mingw" ;;
	*wasm*) sdk_suffix="-wasm32-emscripten" ;;
	*-arm64) sdk_suffix="-linux-arm64" ;;
esac
if [[ -n "${TAU_SDK_DIR:-}" ]]; then
	script_sdk="${TAU_SDK_DIR}"
	if [[ ! -f "${script_sdk}/cmake/tau-compile.cmake" ]]; then
		echo "tau SDK not found at ${script_sdk}; set TAU_SDK_DIR to a directory holding TauConfig.cmake" >&2
		exit 1
	fi
elif [[ -z "$preset" ]]; then
	script_sdk="${REPO_ROOT}/build/release/sdk"
	if [[ ! -f "${script_sdk}/cmake/tau-compile.cmake" ]]; then
		echo "no native SDK at build/release/sdk; build it with ./dev preset release or set TAU_SDK_DIR" >&2
		exit 1
	fi
else
	script_sdk="${REPO_ROOT}/build/${platform}/sdk"
	if [[ ! -f "${script_sdk}/cmake/tau-compile.cmake" ]]; then
		# An installed box: <prefix>/lib/tau/sdk/<platform>/lib/cmake/Tau,
		# the layout the packages use. The tau on PATH names its own prefix,
		# then the two system prefixes. lib*/ covers lib, lib64 and a
		# multiarch lib/<triplet>, the forms resolve_sdk_dir probes.
		prefixes=()
		if command -v tau >/dev/null 2>&1; then
			prefixes+=("$(dirname "$(command -v tau)")/..")
		fi
		prefixes+=("/usr/local" "/usr")
		for prefix in "${prefixes[@]}"; do
			for candidate in \
					"${prefix}"/lib*/tau/sdk/"${platform}"/lib/cmake/Tau \
					"${prefix}"/lib*/*/tau/sdk/"${platform}"/lib/cmake/Tau; do
				if [[ -f "${candidate}/cmake/tau-compile.cmake" ]]; then
					script_sdk="${candidate}"
					break 2
				fi
			done
		done
	fi
	if [[ ! -f "${script_sdk}/cmake/tau-compile.cmake" ]]; then
		echo "no SDK for ${platform}; install tau-sdk${sdk_suffix} or build it with ./dev preset ${platform}" >&2
		exit 1
	fi
fi

# TAU_EXTRA_ARGS reaches the script as one cmake list.
extra=("${DEV_CMAKE[@]}")
if [[ -n "$GENERATOR_EXPLICIT" ]]; then
	extra+=("-G" "$GENERATOR")
fi
tauextra=""
for a in "${extra[@]}"; do
	[[ -n "$tauextra" ]] && tauextra+=";"
	tauextra+="$a"
done

extra_args=()
[[ -n "$tauextra" ]] && extra_args+=("-DTAU_EXTRA_ARGS=${tauextra}")
if [[ -z "$preset" ]]; then
	extra_args+=("-DTAU_NATIVE=ON")
else
	extra_args+=("-DTAU_PRESET=${platform}")
fi
# The resolved box is the script host and the target SDK; the artifact
# presets name no SDK of their own.
sdk_args=("-DTAU_SDK_DIR=${script_sdk}")

exec cmake \
	"-DTAU_ARTIFACT_DIR=${artifact_dir}" \
	"${sdk_args[@]}" \
	"${extra_args[@]}" \
	-P "${script_sdk}/cmake/tau-compile.cmake"
