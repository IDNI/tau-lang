#!/bin/bash

# This helper script clones tau-testnet and runs its pytest suite against the
# Python binding built from the current tau-lang code, to check that tau-lang
# changes do not break tau-testnet.
#
# It is the downstream mirror of the parser's scripts/test-with-tau.sh: that
# one compiles tau-lang against the current parser, this one runs tau-testnet
# against the current tau-lang. The shape differs because tau-testnet is a
# Python project -- it does not vendor tau-lang, it imports the `tau` nanobind
# extension module (see its tau_native.py), so the coupling is the built
# module rather than a source tree copied into a submodule.
#
# This script accepts an optional CMake preset. It defaults to
# release-binding-python; the others that build the binding are
# release-binding-python-tests, debug-binding-python-tests and
# devel-binding-python-tests. A preset without the binding has nothing for
# tau-testnet to import and is rejected after the build.
#
# -D options are forwarded to the configure step. Arguments after `--` are
# forwarded to pytest instead of the default invocation:
#
#   ./dev test-with-tau-testnet
#   ./dev test-with-tau-testnet devel-binding-python-tests
#   ./dev test-with-tau-testnet -- -k test_consensus_time
#
# TAU_TESTNET_DIR points at an existing tau-testnet checkout instead of
# cloning one into ./tau-testnet; TAU_TESTNET_REPO overrides the clone URL.
# TAU_TESTNET_PYTHON selects the interpreter the venv is built from (default
# python3) -- tau-testnet pins exact dependency versions, so the newest
# interpreter on the box is not always one they publish wheels for.
#
# The configure keeps the cache of the preset's build directory, so a
# binding preset over an existing build of the same directory reuses its
# objects and its configured pack.
#
# Coverage note: tau-testnet's conftest.py sets TAU_FORCE_TEST=1, so most of
# the suite runs against its deterministic test validator and never reaches
# the engine. The files that do construct a real TauInterface -- and so load
# and exercise this build of the binding -- are tests/test_consensus_time.py
# and tests/test_web_wallet_examples.py. tests/test_tau_native_direct.py
# substitutes a fake module and only covers the Python glue.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PARSER_ROOT="${REPO_ROOT}/external/parser"
DEV_ROOT="${PARSER_ROOT}"
source "${PARSER_ROOT}/scripts/devrc"

cd "${REPO_ROOT}"

TESTNET_REPO="${TAU_TESTNET_REPO:-https://github.com/IDNI/tau-testnet}"
TESTNET_DIR="${TAU_TESTNET_DIR:-${REPO_ROOT}/tau-testnet}"
DEFAULT_PRESET="release-binding-python"

# preset_entry defaults to the plain `release` preset, which does not build the
# binding, so substitute our own default when no preset was named. A bare
# non-flag token before `--` is the preset; everything after `--` is pytest's.
PRESET_GIVEN=""
past_dd=false
for arg in "$@"; do
	$past_dd && continue
	if [[ $arg == -- ]]; then past_dd=true; continue; fi
	[[ $arg == -* ]] && continue
	PRESET_GIVEN="$arg"
	break
done

# clone tau-testnet
if [ ! -d "${TESTNET_DIR}" ]; then
	echo "Cloning tau-testnet into ${TESTNET_DIR}"
	git clone "${TESTNET_REPO}" "${TESTNET_DIR}"
else
	echo "Using tau-testnet at ${TESTNET_DIR}"
fi

# tau_native.load_tau_module() looks for a build under <parent>/tau-lang
# *before* falling back to PYTHONPATH. When TAU_TESTNET_DIR points at a
# checkout that sits beside an unrelated tau-lang clone, that sibling wins and
# the suite silently exercises someone else's build -- the one failure mode
# that would make this whole script meaningless. Refuse instead.
SIBLING="$(cd "${TESTNET_DIR}/.." && pwd)/tau-lang"
if [ "${SIBLING}" != "${REPO_ROOT}" ]; then
	# kept in step with tau_native.load_tau_module()'s candidate_paths
	for rel in build/bindings/python/nanobind \
			build/release/bindings/python/nanobind \
			build/Release/bindings/python/nanobind \
			build-Release/bindings/python/nanobind \
			build/devel/bindings/python/nanobind \
			build/relwithdebinfo/bindings/python/nanobind \
			build/debug/bindings/python/nanobind \
			build/Debug/bindings/python/nanobind \
			build-Debug/bindings/python/nanobind; do
		if [ -d "${SIBLING}/${rel}" ]; then
			echo "Error: ${SIBLING}/${rel} exists and tau_native.py" >&2
			echo "prefers it over PYTHONPATH, so the suite would test" >&2
			echo "that build instead of ${REPO_ROOT}." >&2
			echo "Move or clean it, or unset TAU_TESTNET_DIR to clone" >&2
			echo "into ${REPO_ROOT}/tau-testnet." >&2
			exit 1
		fi
	done
fi

# The extension module is built against the interpreter CMake finds, and
# imported by the interpreter running pytest; a mismatch shows up as a bare
# ImportError. Create the venv first and pin the build to it so there is only
# one interpreter in play.
VENV_DIR="${TESTNET_DIR}/venv"
if [ ! -x "${VENV_DIR}/bin/python3" ]; then
	echo "Creating venv in ${VENV_DIR}"
	"${TAU_TESTNET_PYTHON:-python3}" -m venv "${VENV_DIR}"
fi
VENV_PYTHON="${VENV_DIR}/bin/python3"

echo "Installing tau-testnet requirements"
"${VENV_PYTHON}" -m pip install --upgrade pip setuptools wheel
if ! "${VENV_PYTHON}" -m pip install -r "${TESTNET_DIR}/requirements.txt"; then
	echo >&2
	echo "Error: could not install tau-testnet's requirements with" >&2
	echo "  $("${VENV_PYTHON}" --version 2>&1)" >&2
	# The interpreter hint only applies past 3.12; on 3.12 the pip error
	# above is the whole story.
	minor="$("${VENV_PYTHON}" -c 'import sys; print(sys.version_info.minor)')"
	if [ "${minor}" -gt 12 ]; then
		echo >&2
		echo "requirements.txt pins exact versions, and several of those pins" >&2
		echo "publish no wheel past cp312 (blake3==0.4.1, coincurve via libp2p)." >&2
		echo "On a newer interpreter pip builds them from source instead, which" >&2
		echo "needs their build toolchains and hits their build bugs -- neither" >&2
		echo "of which says anything about tau-lang. tau-testnet's own" >&2
		echo "Dockerfile.standalone runs on ubuntu:24.04 (Python 3.12) for this" >&2
		echo "reason. Point the venv at an interpreter the pins have wheels for:" >&2
		echo "  rm -rf ${VENV_DIR}" >&2
		echo "  TAU_TESTNET_PYTHON=python3.12 ./dev test-with-tau-testnet" >&2
		echo "If no such interpreter is installed, 'uv python install 3.12'" >&2
		echo "fetches one without root; pass 'uv python find 3.12' above." >&2
	fi
	exit 1
fi
# not in requirements.txt: needed to configure the binding, not to run it
"${VENV_PYTHON}" -m pip install nanobind

# configure and build the preset; sets PRESET and TAU_BUILD_JOBS.
# --keep-cache adds the binding to whatever the build directory holds
# already instead of reconfiguring it from scratch.
PRESET_ARGS=(--keep-cache -DPython_EXECUTABLE="${VENV_PYTHON}")
[[ -z ${PRESET_GIVEN} ]] && PRESET_ARGS+=("${DEFAULT_PRESET}")
PRESET_ARGS+=("$@")
preset_entry "${PRESET_ARGS[@]}"

MODULE_DIR="${REPO_ROOT}/$(preset_binary_dir "${PRESET}")/bindings/python/nanobind"
if ! compgen -G "${MODULE_DIR}/tau*.so" > /dev/null; then
	echo "Error: no tau extension module in ${MODULE_DIR}" >&2
	echo "Preset '${PRESET}' does not build the Python binding. Use one that" >&2
	echo "does: ${DEFAULT_PRESET}, release-binding-python-tests," >&2
	echo "debug-binding-python-tests or devel-binding-python-tests." >&2
	exit 1
fi

# Much of the suite loads data/genesis.json, which is not committed at that
# path -- the network profile under networks/ has to be copied into place
# first, the same step tau-testnet's README gives for running a node. Without
# it ~116 tests fail on a missing genesis artifact, which buries any real
# signal about tau-lang.
mkdir -p "${TESTNET_DIR}/data"
if [ ! -f "${TESTNET_DIR}/data/genesis.json" ]; then
	echo "Seeding data/genesis.json from networks/tau-testnet-v2"
	cp "${TESTNET_DIR}/networks/tau-testnet-v2/genesis.json" \
		"${TESTNET_DIR}/data/genesis.json"
fi

# normalize_args keeps the `--` itself at the head of DEV_PROGRAM
PYTEST_ARGS=("${DEV_PROGRAM[@]}")
[[ ${PYTEST_ARGS[0]:-} == -- ]] && PYTEST_ARGS=("${PYTEST_ARGS[@]:1}")
# README's recommended invocation: the suite is Trio-based, and pytest.ini
# keeps pytest-asyncio in strict mode rather than letting it claim every
# async test
[[ ${#PYTEST_ARGS[@]} -eq 0 ]] && PYTEST_ARGS=(-p no:asyncio)

# run the tau-testnet suite against it
echo "Running tau-testnet tests with ${MODULE_DIR} on PYTHONPATH"
cd "${TESTNET_DIR}"
PYTHONPATH="${MODULE_DIR}${PYTHONPATH:+:${PYTHONPATH}}" \
	"${VENV_PYTHON}" -m pytest "${PYTEST_ARGS[@]}"
