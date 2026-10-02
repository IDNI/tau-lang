#!/bin/bash
# Create the Python 3.12.14 venv the nanobind binding and the wheel jobs build
# against, with the packages they need installed into it.
#
#   ./dev dep-python-venv                            # the host interpreter
#   ./dev dep-python-venv -DTAU_PYTHON_ARCH=aarch64  # a target interpreter
#
# uv fetches the interpreter and the venv lands in <TAU_SHARED_PREFIX>/py312
# (a target arch gets its own <TAU_SHARED_PREFIX>/py312-<arch>). The pins are
# installed with the venv's own pip, so no package reaches a system
# interpreter. The wheel repair tool is the platform's own: auditwheel on
# Linux, delocate on macOS, delvewheel on Windows.
#
# Progress goes to stderr and the interpreter path is the last line on stdout.
# A CI job does not capture it: the script appends TAU_PYTHON to GITHUB_ENV
# and, for a host venv, the venv's bin directory to GITHUB_PATH (the way
# dep-oras.sh does), so every later step builds with the venv interpreter and
# its repair tool. The interpreter is named bin/python3 on Linux and
# macOS, Scripts/python.exe on Windows, and TAU_PYTHON carries the native form
# so the native CMake on Windows can read it.
#
# A target arch needs binfmt for that arch before this runs: uv refuses a
# foreign interpreter, so the venv is made by the fetched interpreter itself.
# A target arch is Linux only.
set -euo pipefail

source "$(dirname "${BASH_SOURCE[0]}")/env"

dep_entry "$@"

PYTHON_VERSION="3.12.14"
SETUPTOOLS_VERSION="${SETUPTOOLS_VERSION:-84.0.0}"
WHEEL_VERSION="${WHEEL_VERSION:-0.48.0}"
NANOBIND_VERSION="${NANOBIND_VERSION:-3.0.1}"
AUDITWHEEL_VERSION="${AUDITWHEEL_VERSION:-6.8.2}"
PATCHELF_VERSION="${PATCHELF_VERSION:-0.19.1.0}"
DELOCATE_VERSION="${DELOCATE_VERSION:-0.13.0}"
DELVEWHEEL_VERSION="${DELVEWHEEL_VERSION:-1.13.1}"
UV_VERSION="0.12.13"
TARGET_ARCH="$(dep_var TAU_PYTHON_ARCH)"

case "$(uname -s)" in
	Linux) HOST_OS=linux ;;
	Darwin) HOST_OS=macos ;;
	MINGW*|MSYS*|CYGWIN*) HOST_OS=windows ;;
	*) echo "dep-python-venv: unsupported host $(uname -s)" >&2; exit 2 ;;
esac
case "$(uname -m)" in
	x86_64|amd64) HOST_ARCH=x86_64 ;;
	aarch64|arm64) HOST_ARCH=aarch64 ;;
	*) HOST_ARCH="$(uname -m)" ;;
esac

case "${HOST_OS}-${HOST_ARCH}" in
	linux-x86_64)    UV_ASSET="uv-x86_64-unknown-linux-gnu.tar.gz" ;;
	linux-aarch64)   UV_ASSET="uv-aarch64-unknown-linux-gnu.tar.gz" ;;
	macos-x86_64)    UV_ASSET="uv-x86_64-apple-darwin.tar.gz" ;;
	macos-aarch64)   UV_ASSET="uv-aarch64-apple-darwin.tar.gz" ;;
	windows-x86_64)  UV_ASSET="uv-x86_64-pc-windows-msvc.zip" ;;
	*) UV_ASSET="" ;;
esac
# From the .sha256 files of the uv release. A version bump needs new digests,
# so a changed or unknown asset fails the checksum instead of running.
case "${UV_VERSION} ${UV_ASSET}" in
	"0.12.13 uv-x86_64-unknown-linux-gnu.tar.gz")  UV_SHA="745765a3b6e360ad76743599ae5c42e9278c7edf8bbff9fc76d05bf2623a04dd" ;;
	"0.12.13 uv-aarch64-unknown-linux-gnu.tar.gz") UV_SHA="2eaa5d94f5db7b3a1a092156b9420459e42ab0217d917fe74a876309cef9b5e9" ;;
	"0.12.13 uv-x86_64-apple-darwin.tar.gz")       UV_SHA="5e287ef61cb6a9b61b3a83fef124fd143e400468a7dac794230147a810e17119" ;;
	"0.12.13 uv-aarch64-apple-darwin.tar.gz")      UV_SHA="7e6ddb9316acc00f2296c82ff4d99977870ee34b2f0ddcae9444d714db9364ed" ;;
	"0.12.13 uv-x86_64-pc-windows-msvc.zip")       UV_SHA="a86c9dc7bad9b03f388583b7187c05fe9951c2e0d392217e8fd43d97787f6ec2" ;;
	*) UV_SHA="" ;;
esac

# A native path for the tools that are not MSYS: uv writes the interpreter
# where UV_PYTHON_INSTALL_DIR points, and TAU_PYTHON reaches CMake.
native_path() {
	if [ "$HOST_OS" = windows ] && command -v cygpath > /dev/null 2>&1; then
		cygpath -m "$1"
	else
		printf '%s' "$1"
	fi
}

if [ "$HOST_OS" = windows ]; then
	VENV_PYTHON="Scripts/python.exe"
	BASE_PYTHON="python.exe"
	REPAIR="delvewheel==${DELVEWHEEL_VERSION}"
else
	VENV_PYTHON="bin/python3"
	BASE_PYTHON="bin/python3"
	if [ "$HOST_OS" = macos ]; then
		REPAIR="delocate==${DELOCATE_VERSION}"
	else
		# auditwheel runs the patchelf program and installs none itself.
		REPAIR="auditwheel==${AUDITWHEEL_VERSION} patchelf==${PATCHELF_VERSION}"
	fi
fi

# A native prefix would put a drive-letter colon inside the colon-separated
# PATH below, so the shell side keeps the MSYS form. cmake -E echo can end
# its line with CRLF on Windows, and a trailing CR would reach a path.
prefix="$(dep_shared_prefix)"
prefix="${prefix%$'\r'}"
if [ -n "$prefix" ] && [ "$HOST_OS" = windows ] && command -v cygpath > /dev/null 2>&1; then
	prefix="$(cygpath -u "$prefix")"
fi
uv_bin_dir="${prefix}/uv/bin"
uv_python_dir="${prefix}/uv/python"
# Keep the interpreter uv fetches under the shared prefix, so the venv and the
# base it points at are one tree a job can cache or drop together.
export UV_PYTHON_INSTALL_DIR="$(native_path "$uv_python_dir")"

if [ -n "${TARGET_ARCH}" ]; then
	if [ "$HOST_OS" != linux ]; then
		echo "dep-python-venv: a target arch is Linux only" >&2
		exit 2
	fi
	venv="${prefix}/py312-${TARGET_ARCH}"
	request="cpython-${PYTHON_VERSION}-linux-${TARGET_ARCH}-gnu"
	base_name="cpython-${PYTHON_VERSION}-linux-${TARGET_ARCH}-gnu"
else
	venv="${prefix}/py312"
	request="${PYTHON_VERSION}"
	case "$HOST_OS" in
		linux) base_name="cpython-${PYTHON_VERSION}-linux-${HOST_ARCH}-gnu" ;;
		*)     base_name="cpython-${PYTHON_VERSION}-${HOST_OS}-${HOST_ARCH}-none" ;;
	esac
fi

packages="setuptools==${SETUPTOOLS_VERSION} wheel==${WHEEL_VERSION} nanobind==${NANOBIND_VERSION} ${REPAIR}"

stamp="${venv}/.tau-packages"
if [ -x "${venv}/${VENV_PYTHON}" ] && [ "$(cat "${stamp}" 2>/dev/null)" = "${packages}" ]; then
	echo "dep-python-venv: ${venv} already has ${packages}" >&2
else
	if ! command -v uv > /dev/null 2>&1; then
		uv_stamp="${prefix}/uv/.version"
		uv_expected="${UV_VERSION} ${UV_ASSET} ${UV_SHA}"
		if { [ ! -x "${uv_bin_dir}/uv" ] && [ ! -x "${uv_bin_dir}/uv.exe" ]; } \
			|| [ "$(cat "${uv_stamp}" 2>/dev/null)" != "${uv_expected}" ]; then
			if [ -z "${UV_SHA}" ]; then
				echo "dep-python-venv: no pinned uv digest for ${HOST_OS}-${HOST_ARCH}" >&2
				exit 2
			fi
			echo "dep-python-venv: installing uv ${UV_VERSION} into ${uv_bin_dir}" >&2
			uv_work="$(mktemp -d "${TMPDIR:-/tmp}/tau-uv.XXXXXX")"
			# shellcheck disable=SC2064
			trap "rm -rf '${uv_work}'" EXIT
			uv_archive="${uv_work}/${UV_ASSET}"
			curl -fsSL --retry 3 -o "${uv_archive}" \
				"https://github.com/astral-sh/uv/releases/download/${UV_VERSION}/${UV_ASSET}" \
				|| { echo "dep-python-venv: cannot download ${UV_ASSET}" >&2; exit 1; }
			uv_actual="$(dep_sha256 "${uv_archive}")"
			if [ "${uv_actual}" != "${UV_SHA}" ]; then
				echo "dep-python-venv: ${UV_ASSET} hashes ${uv_actual}, expected ${UV_SHA}" >&2
				exit 1
			fi
			(cd "${uv_work}" && "${CMAKE:-cmake}" -E tar xf "${uv_archive}") \
				|| { echo "dep-python-venv: cannot extract ${UV_ASSET}" >&2; exit 1; }
			# The .tar.gz holds its binaries in a folder named after the
			# asset, and the .zip holds them at its root.
			mkdir -p "${uv_bin_dir}"
			uv_found=0
			for f in "${uv_work}"/uv "${uv_work}"/uv.exe "${uv_work}"/uv-*/uv; do
				[ -f "$f" ] || continue
				mv "$f" "${uv_bin_dir}/"
				uv_found=1
			done
			if [ "${uv_found}" != 1 ]; then
				echo "dep-python-venv: ${UV_ASSET} holds no uv binary" >&2
				exit 1
			fi
			chmod +x "${uv_bin_dir}"/uv* 2>/dev/null || true
			printf '%s' "${uv_expected}" > "${uv_stamp}"
		fi
		PATH="${uv_bin_dir}:${PATH}"
		export PATH
	fi

	echo "dep-python-venv: fetching ${request}" >&2
	uv python install "${request}" >&2

	base="${uv_python_dir}/${base_name}"
	if [ ! -x "${base}/${BASE_PYTHON}" ]; then
		echo "dep-python-venv: no ${request} under ${uv_python_dir}" >&2
		exit 1
	fi
	# A wrong-arch interpreter fails here with the host's exec error, which is
	# the binfmt registration the target mode needs.
	"${base}/${BASE_PYTHON}" -m venv "$(native_path "${venv}")" >&2 || {
		echo "dep-python-venv: cannot run ${base}/${BASE_PYTHON}; a target arch needs binfmt" >&2
		exit 1
	}

	echo "dep-python-venv: installing ${packages}" >&2
	"${venv}/${VENV_PYTHON}" -m pip install --upgrade pip >&2
	# pack-wheel.sh runs pip wheel --no-build-isolation, so a setuptools that
	# can build a wheel is a build requirement and not an optional extra.
	# shellcheck disable=SC2086
	"${venv}/${VENV_PYTHON}" -m pip install ${packages} >&2
	printf '%s' "${packages}" > "${stamp}"
fi

interpreter="$(native_path "${venv}/${VENV_PYTHON}")"
if [ -n "${GITHUB_ENV:-}" ] && [ -w "${GITHUB_ENV}" ]; then
	printf 'TAU_PYTHON=%s\n' "${interpreter}" >> "${GITHUB_ENV}"
fi
# The wheel repair tools install beside the interpreter, and the binding
# finds them with find_program, so later steps need that directory on PATH.
# A target venv stays off PATH: its python3 runs only under qemu.
if [ -z "${TARGET_ARCH}" ] && [ -n "${GITHUB_PATH:-}" ] && [ -w "${GITHUB_PATH}" ]; then
	printf '%s\n' "$(dirname "${interpreter}")" >> "${GITHUB_PATH}"
fi
echo "${interpreter}"
