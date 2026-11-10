#!/bin/bash

# Build the tau CLI and the nanobind module of the current source, then run
# the tau-overload benchmarks (tests/benchmark/testnet/run_overload.py) on
# them, natively. overload-docker.sh in the same folder runs them in Docker,
# the way the Benchmarks workflow does.
#
#   ./dev bench-overload [<PRESET>] [<CMAKE_OPTIONS>] [-- <RUN_OVERLOAD_ARGS>]
#
#   ./dev bench-overload -DTAU_BUILD_JOBS=10
#   ./dev bench-overload -- --only dispatch-blowup --cap 60
#   TAU_OVERLOAD_NO_BUILD=1 ./dev bench-overload -- --out build/overload-b
#
# The preset defaults to release-all; it must build the CLI and the binding.
# TAU_PYTHON names the interpreter the module is built for and run with
# (default <TAU_SHARED_PREFIX>/py312/bin/python3, made by ./dev
# dep-python-venv); it needs nanobind. TAU_OVERLOAD_NO_BUILD=1 skips the build
# and runs on the preset's existing build folder. Arguments after `--` go to
# run_overload.py; --out defaults to <build>/overload-results.
# The exit status is the one of run_overload.py.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PARSER_ROOT="${REPO_ROOT}/external/parser"
DEV_ROOT="${PARSER_ROOT}"
source "${PARSER_ROOT}/scripts/devrc"

cd "${REPO_ROOT}"

DEFAULT_PRESET="release-all"

PYTHON="${TAU_PYTHON:-${TAU_SHARED_PREFIX:-${HOME}/.tau}/py312/bin/python3}"
if ! "${PYTHON}" -c 'import nanobind' > /dev/null 2>&1; then
	echo "Error: ${PYTHON} is not an interpreter with nanobind." >&2
	echo "Run ./dev dep-python-venv, or set TAU_PYTHON." >&2
	exit 1
fi
export TAU_PYTHON="${PYTHON}"

# A bare non-flag token before `--` is the preset.
PRESET_GIVEN=""
for arg in "$@"; do
	[[ $arg == -- ]] && break
	[[ $arg == -* ]] && continue
	PRESET_GIVEN="$arg"
	break
done

PRESET_ARGS=(--keep-cache -DPython_EXECUTABLE="${PYTHON}")
[[ " $* " == *" --target "* ]] || PRESET_ARGS+=(--target tau,tau_nanobind)
[[ -z ${PRESET_GIVEN} ]] && PRESET_ARGS+=("${DEFAULT_PRESET}")
PRESET_ARGS+=("$@")

if [[ ${TAU_OVERLOAD_NO_BUILD:-} == 1 ]]; then
	# parse the arguments only, to know the preset and the program arguments
	dev_entry "${PRESET_ARGS[@]}"
	[[ -z $PRESET ]] && PRESET="${PRESET_GIVEN:-${DEFAULT_PRESET}}"
else
	preset_entry "${PRESET_ARGS[@]}"
fi

BUILD_DIR="${REPO_ROOT}/$(preset_binary_dir "${PRESET}")"
TAU_BIN="${BUILD_DIR}/tau"
MODULE_DIR="${BUILD_DIR}/bindings/python/nanobind"
if [ ! -x "${TAU_BIN}" ]; then
	echo "Error: no tau CLI at ${TAU_BIN}; preset '${PRESET}' must build it." >&2
	exit 1
fi
if ! compgen -G "${MODULE_DIR}/tau*.so" > /dev/null; then
	echo "Error: no tau extension module in ${MODULE_DIR}; preset '${PRESET}'" >&2
	echo "must build the Python binding." >&2
	exit 1
fi

# normalize_args keeps the `--` itself at the head of DEV_PROGRAM
RUN_ARGS=(${DEV_PROGRAM[@]+"${DEV_PROGRAM[@]}"})
[[ ${RUN_ARGS[0]:-} == -- ]] && RUN_ARGS=("${RUN_ARGS[@]:1}")
[[ " ${RUN_ARGS[*]-} " == *" --out "* ]] || RUN_ARGS+=(--out "${BUILD_DIR}/overload-results")

echo "tau-overload on $(git log -1 --format='%h %s') with ${TAU_BIN}"
status=0
"${PYTHON}" "${REPO_ROOT}/tests/benchmark/testnet/run_overload.py" \
	--tau "${TAU_BIN}" --module-dir "${MODULE_DIR}" --python "${PYTHON}" \
	${RUN_ARGS[@]+"${RUN_ARGS[@]}"} || status=$?
echo "run_overload.py exit status: ${status}"
exit "${status}"
