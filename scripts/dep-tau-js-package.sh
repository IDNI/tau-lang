#!/bin/bash
# Publish the assembled @idni/tau-lang npm package as a LOCAL store package.
#
#   ./dev dep-tau-js-package \
#     -DTAU_JS_PACKAGE_DIR=<wasm build dir> \
#     -DTAU_JS_PACKAGE_VERSION=<version> \
#     -DTAU_JS_PARSER_SDK_ID=<id> -DTAU_JS_FTXUI_ID=<id>
#
# The producer does not build the wasm library. It publishes the package.json
# "files" list from the build directory the wasm preset wrote, so the
# entry holds the bytes a consumer would install. There is no npm publish
# anywhere: no registry and no token, only the same content-addressed store
# every other package uses.
#
# The id hashes only build-affecting inputs: the Tau commit and tree hash, the
# parser SDK and FTXUI package ids, the package version, the recipe, and the
# toolchain. Writer and scanner hashes are not inputs and are not recorded.
set -u

DEV_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${DEV_ROOT}/scripts/devrc"

TAU_JS_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# Content hash of the Tau working tree. The provenance helpers and this recipe
# are excluded: each is hashed on its own or cannot change the package bytes.
_dep_tau_js_tree_hash() {
	local src="$1" value digest
	local exclude='^(external/parser|scripts/dep-tau-js-package\.sh)$'
	if command -v sha256sum > /dev/null 2>&1; then
		digest="sha256sum"
	else
		digest="shasum -a 256"
	fi
	value="$(cd "$src" && git ls-files -z --cached --others --exclude-standard \
		| grep -zvE "$exclude" \
		| LC_ALL=C sort -z | xargs -0 -r $digest | dep_sha256_stdin)"
	[ -n "$value" ] || { echo "dep-tau-js: Tau tree hash is empty" >&2; return 1; }
	printf '%s' "$value"
}

_dep_tau_js_field_block() {
	local recipe_hash tree_hash compiler_id threads
	local build_helper publish_helper manifest store
	local build_hash publish_hash manifest_hash store_hash
	# The library's threading follows the preset, and the pthread variant is a
	# different package, so it must be part of the id.
	threads="OFF"
	case " ${TAU_JS_CXXFLAGS:-} " in *" -pthread "*) threads="ON" ;; esac
	build_helper="${__devrc_dir}/dep-build"
	publish_helper="${__devrc_dir}/devrc"
	manifest="${__devrc_dir}/../cmake/tau-manifest.cmake"
	store="${__devrc_dir}/../cmake/tau-store.cmake"
	recipe_hash="$(dep_sha256 "$TAU_JS_RECIPE")" || return 1
	tree_hash="$(_dep_tau_js_tree_hash "$TAU_JS_TREE")" || return 1
	build_hash="$(dep_sha256 "$build_helper")" || return 1
	publish_hash="$(dep_sha256 "$publish_helper")" || return 1
	manifest_hash="$(dep_sha256 "$manifest")" || return 1
	store_hash="$(dep_sha256 "$store")" || return 1
	case "$("$TAU_JS_EMCC" --version 2>&1 | head -n 1)" in
		*GCC*|*g++*) compiler_id=GNU ;;
		*[Cc]lang*) compiler_id=Clang ;;
		*) compiler_id=unknown ;;
	esac
	printf '%s\n' \
		"dep=tau-js" \
		"tau_commit=${TAU_JS_COMMIT}" \
		"tau_tree_hash=${tree_hash}" \
		"parser_sdk_package_id=${TAU_JS_PARSER_SDK_ID}" \
		"ftxui_package_id=${TAU_JS_FTXUI_ID}" \
		"package_version=${TAU_JS_PACKAGE_VERSION}" \
		"module_format=js+esm" \
		"threads=${threads}" \
		"exceptions=wasm-exceptions" \
		"recipe_hash=${recipe_hash}" \
		"helper_build_hash=${build_hash}" \
		"provenance.publish_helper_hash=${publish_hash}" \
		"provenance.manifest_writer_hash=${manifest_hash}" \
		"provenance.store_writer_hash=${store_hash}" \
		"compiler_id=${compiler_id}" \
		"compiler_version=$("$TAU_JS_EMCC" --version | head -n 1)" \
		"target_triple=wasm32" \
		"os=$(uname -s)" \
		"arch=$(uname -m)"
}

# Publish exactly the files package.json's "files" list names, plus the
# manifest. A partial set must never reach the store, so every listed file is
# required before anything is copied.
_dep_tau_js_producer() {
	local staging_prefix="$1" package_dir="$2"
	local rel
	local files="package.json tau.js tau.wasm tau.esm.mjs tau.esm.wasm tau.node.js tau.node.mjs LICENSE.md tests/parity.js tests/native.js"
	if [ ! -d "$package_dir" ]; then
		echo "dep-tau-js: package directory not found: '$package_dir'" >&2
		return 1
	fi
	for rel in $files; do
		if [ ! -f "$package_dir/$rel" ]; then
			echo "dep-tau-js: package is missing $rel in '$package_dir'" >&2
			return 1
		fi
	done
	for rel in $files; do
		mkdir -p "$(dirname "$staging_prefix/$rel")" || return 1
		cp "$package_dir/$rel" "$staging_prefix/$rel" || return 1
	done
	return 0
}

dep_entry "$@"

mode="$(dep_var TAU_DEP_MODE producer)"
case "$mode" in
	producer|consumer) ;;
	*) echo "dep-tau-js: -DTAU_DEP_MODE must be producer or consumer" >&2; exit 2 ;;
esac

TAU_JS_PACKAGE_DIR="$(dep_var TAU_JS_PACKAGE_DIR "")"
TAU_JS_PACKAGE_VERSION="$(dep_var TAU_JS_PACKAGE_VERSION "")"
TAU_JS_PARSER_SDK_ID="$(dep_var TAU_JS_PARSER_SDK_ID "")"
TAU_JS_FTXUI_ID="$(dep_var TAU_JS_FTXUI_ID "")"
TAU_JS_TREE="$(dep_var TAU_JS_TREE "$DEV_ROOT")"
TAU_JS_COMMIT="$(dep_var TAU_JS_COMMIT "$(git -C "$TAU_JS_TREE" rev-parse HEAD 2>/dev/null)")"

[ -n "$TAU_JS_PACKAGE_DIR" ] || { echo "dep-tau-js: -DTAU_JS_PACKAGE_DIR is required" >&2; exit 2; }
[ -n "$TAU_JS_PACKAGE_VERSION" ] || { echo "dep-tau-js: -DTAU_JS_PACKAGE_VERSION is required" >&2; exit 2; }
[ -n "$TAU_JS_PARSER_SDK_ID" ] || { echo "dep-tau-js: -DTAU_JS_PARSER_SDK_ID is required" >&2; exit 2; }
[ -n "$TAU_JS_FTXUI_ID" ] || { echo "dep-tau-js: -DTAU_JS_FTXUI_ID is required" >&2; exit 2; }

TAU_JS_EMCC="$(dep_var TAU_DEP_CXX "")"
TAU_JS_CXXFLAGS="$(dep_var TAU_DEP_CXXFLAGS "")"
[ -n "$TAU_JS_EMCC" ] || { echo "dep-tau-js: no compiler; pass -DTAU_DEP_CXX" >&2; exit 2; }

block="$(_dep_tau_js_field_block)" || {
	echo "dep-tau-js: cannot build the identity field block" >&2
	exit 1
}

out=""
if ! dep_ensure out "$mode" tau-js "$block" _dep_tau_js_producer \
		"$TAU_JS_PACKAGE_DIR"; then
	echo "dep-tau-js: ${mode} failed" >&2
	exit 1
fi

# Record the entry as used, so store eviction keeps it by last use.
if [ -n "$out" ] && [ -d "$out" ]; then
	date +%s > "$(dirname "$out")/.last-used"
fi

echo "dep-tau-js: package prefix: ${out}"
