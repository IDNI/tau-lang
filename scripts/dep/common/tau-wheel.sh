#!/bin/bash
# Publish the tau nanobind wheel as a LOCAL store package. Each
# scripts/dep/<target>/tau-wheel.sh sources this file for its own target.
#
#   ./dev dep-tau-wheel \
#     -DTAU_WHEEL_FILE=<built wheel> -DTAU_WHEEL_SHA256=<digest> \
#     -DTAU_WHEEL_NANOBIND_VERSION=<version> \
#     -DTAU_WHEEL_PARSER_SDK_ID=<id> -DTAU_WHEEL_CVC5_ID=<id> \
#     -DTAU_WHEEL_BOOST_ID=<id>
#
# The producer does not build the wheel. It publishes the exact artifact a
# consumer installs and fails unless its digest matches the tested one, so the
# entry holds the byte-identical wheel the downstream suite ran. Building the
# wheel is a separate step; a later publish never rebuilds.
#
# The id hashes only build-affecting inputs: the Tau commit and tree hash, the
# parser SDK / cvc5 / Boost package ids, the interpreter version and ABI, the
# nanobind version, the recipe, and the toolchain. Writer and scanner hashes are
# not inputs and are not recorded.
set -u

DEV_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
source "${DEV_ROOT}/scripts/devrc"

DEP_RECIPE_COMMON="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# Content hash of the Tau working tree. The provenance helpers and the wheel
# recipe files are excluded: each is hashed on its own or cannot change the wheel bytes.
# Tracked files only, so an untracked build artifact cannot move the id.
_dep_tau_wheel_tree_hash() {
	local src="$1" value digest
	local exclude='^(external/parser|scripts/dep/[^/]+/tau-wheel\.sh)$'
	if command -v sha256sum > /dev/null 2>&1; then
		digest="sha256sum"
	elif command -v shasum > /dev/null 2>&1; then
		digest="shasum -a 256"
	else
		echo "dep-tau-wheel: neither sha256sum nor shasum is available" >&2
		return 1
	fi
	# BSD tools have no -z/--zero; NUL records become newline records for the
	# sort and back for xargs, which is exact for every path without a newline.
	value="$(cd "$src" && git ls-files -z --cached \
		| tr '\0' '\n' | grep -vE "$exclude" | LC_ALL=C sort \
		| tr '\n' '\0' | xargs -0 -n 100 $digest | dep_sha256_stdin)"
	[ -n "$value" ] || { echo "dep-tau-wheel: Tau tree hash is empty" >&2; return 1; }
	printf '%s' "$value"
}

_dep_tau_wheel_field_block() {
	local recipe_hash recipe_common_hash tree_hash compiler_id
	local build_helper publish_helper manifest store
	local build_hash publish_hash manifest_hash store_hash
	build_helper="${__devrc_dir}/dep-build"
	publish_helper="${__devrc_dir}/devrc"
	manifest="${__devrc_dir}/../cmake/tau-manifest.cmake"
	store="${__devrc_dir}/../cmake/tau-store.cmake"
	recipe_hash="$(dep_sha256 "$DEP_RECIPE")" || return 1
	recipe_common_hash="$(dep_sha256 "$DEP_RECIPE_COMMON")" || return 1
	tree_hash="$(_dep_tau_wheel_tree_hash "$TAU_WHEEL_TREE")" || return 1
	build_hash="$(dep_sha256 "$build_helper")" || return 1
	publish_hash="$(dep_sha256 "$publish_helper")" || return 1
	manifest_hash="$(dep_sha256 "$manifest")" || return 1
	store_hash="$(dep_sha256 "$store")" || return 1
	case "$(dep_compiler_id "$TAU_WHEEL_CXX")" in
		GNU) compiler_id=GNU ;;
		Clang) compiler_id=Clang ;;
		AppleClang) compiler_id=AppleClang ;;
		MSVC) compiler_id=MSVC ;;
		*) compiler_id=unknown ;;
	esac
	printf '%s\n' \
		"dep=tau-wheel" \
		"target=${DEP_TARGET:-$(dep_host_target)}" \
		"tau_commit=${TAU_WHEEL_COMMIT}" \
		"tau_tree_hash=${tree_hash}" \
		"parser_sdk_package_id=${TAU_WHEEL_PARSER_SDK_ID}" \
		"cvc5_package_id=${TAU_WHEEL_CVC5_ID}" \
		"boost_package_id=${TAU_WHEEL_BOOST_ID}" \
		"python_version=${TAU_WHEEL_PY_VERSION}" \
		"python_abi=${TAU_WHEEL_PY_ABI}" \
		"nanobind_version=${TAU_WHEEL_NANOBIND_VERSION}" \
		"recipe_hash=${recipe_hash}" \
		"recipe_common_hash=${recipe_common_hash}" \
		"helper_build_hash=${build_hash}" \
		"provenance.publish_helper_hash=${publish_hash}" \
		"provenance.manifest_writer_hash=${manifest_hash}" \
		"provenance.store_writer_hash=${store_hash}" \
		"compiler_id=${compiler_id}" \
		"compiler_version=$(dep_compiler_version "$TAU_WHEEL_CXX")" \
		"target_triple=$(dep_compiler_triple "$TAU_WHEEL_CXX" "${DEP_TARGET:-$(dep_host_target)}")" \
		"os=$(uname -s)" \
		"arch=$(uname -m)"
}

# Publish the given wheel. The digest is asserted before the manifest is
# written, so a mismatch never reaches the store.
_dep_tau_wheel_producer() {
	local staging_prefix="$1" wheel="$2" expected="$3"
	local name got
	if [ ! -f "$wheel" ]; then
		echo "dep-tau-wheel: wheel not found: '$wheel'" >&2
		return 1
	fi
	name="$(basename "$wheel")"
	cp "$wheel" "$staging_prefix/$name" || return 1
	got="$(dep_sha256 "$staging_prefix/$name")" || return 1
	if [ "$got" != "$expected" ]; then
		echo "dep-tau-wheel: published wheel digest ${got} does not match the tested ${expected}" >&2
		rm -f "$staging_prefix/$name"
		return 1
	fi
	if [ -f "$DEV_ROOT/LICENSE.md" ]; then
		cp "$DEV_ROOT/LICENSE.md" "$staging_prefix/LICENSE.md" || return 1
	fi
	return 0
}

dep_entry "$@"

dep_require_file_target tau-wheel

mode="$(dep_var TAU_DEP_MODE producer)"
case "$mode" in
	producer|consumer) ;;
	*) echo "dep-tau-wheel: -DTAU_DEP_MODE must be producer or consumer" >&2; exit 2 ;;
esac

TAU_WHEEL_FILE="$(dep_var TAU_WHEEL_FILE "")"
TAU_WHEEL_SHA256="$(dep_var TAU_WHEEL_SHA256 "")"
TAU_WHEEL_NANOBIND_VERSION="$(dep_var TAU_WHEEL_NANOBIND_VERSION "")"
TAU_WHEEL_PARSER_SDK_ID="$(dep_var TAU_WHEEL_PARSER_SDK_ID "")"
TAU_WHEEL_CVC5_ID="$(dep_var TAU_WHEEL_CVC5_ID "")"
TAU_WHEEL_BOOST_ID="$(dep_var TAU_WHEEL_BOOST_ID "")"
TAU_WHEEL_TREE="$(dep_var TAU_WHEEL_TREE "$DEV_ROOT")"
TAU_WHEEL_COMMIT="$(dep_var TAU_WHEEL_COMMIT "$(git -C "$TAU_WHEEL_TREE" rev-parse HEAD 2>/dev/null)")"

[ -n "$TAU_WHEEL_FILE" ] || { echo "dep-tau-wheel: -DTAU_WHEEL_FILE is required" >&2; exit 2; }
[ -n "$TAU_WHEEL_SHA256" ] || { echo "dep-tau-wheel: -DTAU_WHEEL_SHA256 is required" >&2; exit 2; }
[ -n "$TAU_WHEEL_NANOBIND_VERSION" ] || { echo "dep-tau-wheel: -DTAU_WHEEL_NANOBIND_VERSION is required" >&2; exit 2; }
[ -n "$TAU_WHEEL_PARSER_SDK_ID" ] || { echo "dep-tau-wheel: -DTAU_WHEEL_PARSER_SDK_ID is required" >&2; exit 2; }
[ -n "$TAU_WHEEL_CVC5_ID" ] || { echo "dep-tau-wheel: -DTAU_WHEEL_CVC5_ID is required" >&2; exit 2; }
[ -n "$TAU_WHEEL_BOOST_ID" ] || { echo "dep-tau-wheel: -DTAU_WHEEL_BOOST_ID is required" >&2; exit 2; }

# The interpreter version and ABI come from the wheel tag, so the id follows the
# artifact the wheel actually is.
TAU_WHEEL_BASENAME="$(basename "$TAU_WHEEL_FILE")"
TAU_WHEEL_PY_VERSION="$(printf '%s' "$TAU_WHEEL_BASENAME" \
	| sed -nE 's/.*-cp([0-9]+)-.*/\1/p')"
TAU_WHEEL_PY_ABI="$(printf '%s' "$TAU_WHEEL_BASENAME" \
	| sed -nE 's/.*-(cp[0-9]+)-.*/\1/p')"
[ -n "$TAU_WHEEL_PY_VERSION" ] || { echo "dep-tau-wheel: cannot read the wheel tag" >&2; exit 2; }

TAU_WHEEL_CXX="$(dep_var TAU_DEP_CXX "")"
[ -n "$TAU_WHEEL_CXX" ] || { echo "dep-tau-wheel: no compiler; pass -DTAU_DEP_CXX" >&2; exit 2; }

block="$(_dep_tau_wheel_field_block)" || {
	echo "dep-tau-wheel: cannot build the identity field block" >&2
	exit 1
}

out=""
if ! dep_ensure out "$mode" tau-wheel "$block" _dep_tau_wheel_producer \
		"$TAU_WHEEL_FILE" "$TAU_WHEEL_SHA256"; then
	echo "dep-tau-wheel: ${mode} failed" >&2
	exit 1
fi

echo "dep-tau-wheel: tau-wheel package prefix: ${out}"
