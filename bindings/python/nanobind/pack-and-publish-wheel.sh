#!/bin/bash
# The wheel producer needs the built module, so packing and publishing run at
# build time. The producer command arrives from CMake's tau-deps helper with the
# compiler and flags already set, so nothing here picks a compiler.
set -euo pipefail

if [ "$#" -lt 8 ]; then
	echo "usage: pack-and-publish-wheel.sh <module-dir> <cvc5-prefix> <out-dir> <version> <nanobind-version> <parser-sdk-id> <cvc5-id> <boost-id> <producer-command...>" >&2
	exit 2
fi

module_dir="$1"; cvc5_prefix="$2"; out_dir="$3"; version="$4"
nanobind_version="$5"; parser_sdk_id="$6"; cvc5_id="$7"; boost_id="$8"
shift 8

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
bash "${here}/pack-wheel.sh" "$module_dir" "$cvc5_prefix" "$out_dir" "$version"

shopt -s nullglob
wheels=("${out_dir}"/wheel/tau_nanobind-*.whl)
if [ "${#wheels[@]}" -ne 1 ]; then
	echo "pack-and-publish-wheel: expected one wheel in ${out_dir}/wheel, found ${#wheels[@]}" >&2
	exit 1
fi
wheel="${wheels[0]}"
# BSD hosts have no sha256sum. shasum -a 256 is its macOS twin.
if command -v sha256sum > /dev/null 2>&1; then
	sha="$(sha256sum "$wheel" | awk '{print $1}')"
else
	sha="$(shasum -a 256 "$wheel" | awk '{print $1}')"
fi

exec_out="$("$@" \
	-DTAU_WHEEL_FILE="$wheel" \
	-DTAU_WHEEL_SHA256="$sha" \
	-DTAU_WHEEL_NANOBIND_VERSION="$nanobind_version" \
	-DTAU_WHEEL_PARSER_SDK_ID="$parser_sdk_id" \
	-DTAU_WHEEL_CVC5_ID="$cvc5_id" \
	-DTAU_WHEEL_BOOST_ID="$boost_id")"
printf '%s\n' "$exec_out"
# Record the published entry as used, so store eviction keeps it by last use.
prefix="$(printf '%s\n' "$exec_out" | sed -n 's/.*package prefix: //p' | tail -n 1)"
if [ -n "$prefix" ] && [ -d "$prefix" ]; then
	date +%s > "$(dirname "$prefix")/.last-used"
fi
