#!/bin/bash
# Build a self-contained nanobind wheel from an already-built tau module.
#
#   pack-wheel.sh <module-dir> <cvc5-prefix> <out-dir> [version]
#
# The module directory holds the module installed by the tau build. Its install
# rpath is relative to the module. The wheel ships the module plus the cvc5
# runtime it links under tau/tau_libs, so the installed wheel needs no store
# path. setuptools is the backend, invoked with --no-build-isolation, so no
# build backend is fetched. The bdist_wheel subclass sets the tags from this
# interpreter. The platform bundler then repairs the wheel: auditwheel retags
# Linux manylinux, delocate retags macOS, and delvewheel bundles the Windows
# DLLs. On Linux auditwheel excludes libcurl and the system libgmp, so it never
# bundles them with their LGPL closure (GnuTLS, nettle, libidn2, ...). A GMP
# that the cvc5 package holds is bundled with cvc5 by the walk below.
#
# TAU_WHEEL_LICENSES is a |-separated list of license files and directories.
# Each file, and the content of each directory, goes to tau/licenses, beside
# the cvc5 package licenses, so the wheel carries the license of every library
# it holds.
set -eu

if [ "$#" -lt 3 ]; then
	echo "usage: pack-wheel.sh <module-dir> <cvc5-prefix> <out-dir> [version]" >&2
	exit 2
fi
module_dir="$1"
cvc5_prefix="$2"
out_dir="$3"
version="${4:-0.1.0}"
python="${TAU_WHEEL_PYTHON:-$(command -v python3)}"

# The wheel writer and auditwheel stamp each zip entry with this time, so a
# fixed value gives byte-identical wheels. CI passes the commit time.
export SOURCE_DATE_EPOCH="${SOURCE_DATE_EPOCH:-315532800}"
platform="${TAU_WHEEL_PLATFORM:?TAU_WHEEL_PLATFORM must name the target platform}"

# The extension suffix differs per platform. The repair tool below is chosen
# by the same name.
case "${platform}" in
	linux|macos) module_suffix=so ;;
	windows) module_suffix=pyd ;;
	*)
		echo "build-wheel: unknown platform '${platform}'" >&2
		exit 2 ;;
esac

module="$(ls "$module_dir"/tau*."${module_suffix}" 2>/dev/null | head -n 1)"
if [ -z "$module" ]; then
	echo "build-wheel: no tau*.${module_suffix} in '${module_dir}'" >&2
	exit 2
fi
if [ ! -d "${cvc5_prefix}/lib" ]; then
	echo "build-wheel: no cvc5 lib dir at '${cvc5_prefix}/lib'" >&2
	exit 2
fi

stage="${out_dir}/stage"
pkgdir="${stage}/tau"
rm -rf "${stage}"
mkdir -p "${pkgdir}/tau_libs" "${out_dir}/wheel"
# The count checks below need the wheel of this run alone.
rm -f "${out_dir}/wheel"/tau_nanobind-*.whl
cp "${module}" "${pkgdir}/"

# Copy the cvc5 runtime closure the module needs. Walk the dynamic
# dependencies, keep every soname the cvc5 prefix holds, and recurse into it.
# A system libgmp is not in the cvc5 prefix and is left to the system. Windows has no such walk: delvewheel reads the PE import table
# and bundles what it finds.
needed() {
	case "${platform}" in
		macos)
			otool -L "$1" 2>/dev/null | awk 'NR > 1 { print $1 }' ;;
		linux)
			readelf -d "$1" 2>/dev/null \
				| awk '/NEEDED/ { sub(/.*\[/, ""); sub(/\].*/, ""); print }' ;;
	esac
}
queue="${pkgdir}/$(basename "${module}")"
seen=""
while [ -n "${queue}" ]; do
	cur="${queue%% *}"
	[ "${cur}" = "${queue}" ] && queue="" || queue="${queue#* }"
	for soname in $(needed "${cur}"); do
		soname="$(basename "${soname}")"
		case " ${seen} " in *" ${soname} "*) continue ;; esac
		seen="${seen} ${soname}"
		src="${cvc5_prefix}/lib/${soname}"
		if [ -f "${src}" ] || [ -L "${src}" ]; then
			cp -L "${src}" "${pkgdir}/tau_libs/${soname}"
			queue="${queue} ${pkgdir}/tau_libs/${soname}"
		fi
	done
done

# delocate resolves @rpath per library, and the cvc5 package gives its
# libraries no usable LC_RPATH. Each one finds its siblings beside itself;
# the edit breaks the ad-hoc signature, which arm64 requires.
if [ "${platform}" = macos ]; then
	for lib in "${pkgdir}"/tau_libs/*.dylib; do
		[ -f "${lib}" ] || continue
		if ! otool -l "${lib}" | grep -qE '^ +path @loader_path \(offset'; then
			chmod u+w "${lib}"
			install_name_tool -add_rpath @loader_path "${lib}"
			codesign --force --sign - "${lib}"
		fi
	done
fi

# The licenses of the cvc5 package (cvc5, CaDiCaL, SymFPU, GMP) and of every
# path in TAU_WHEEL_LICENSES.
if [ ! -d "${cvc5_prefix}/share/licenses" ]; then
	echo "build-wheel: no license dir at '${cvc5_prefix}/share/licenses'" >&2
	exit 2
fi
mkdir -p "${pkgdir}/licenses"
cp -R "${cvc5_prefix}/share/licenses/." "${pkgdir}/licenses/"
IFS='|' read -r -a licenses <<< "${TAU_WHEEL_LICENSES:-}"
for lic in ${licenses[@]+"${licenses[@]}"}; do
	if [ -d "${lic}" ]; then
		cp -R "${lic}/." "${pkgdir}/licenses/"
	elif [ -f "${lic}" ]; then
		cp "${lic}" "${pkgdir}/licenses/"
	else
		echo "build-wheel: no license file or dir at '${lic}'" >&2
		exit 2
	fi
done

cat > "${pkgdir}/__init__.py" <<'EOF'
# The extension is the build's nanobind module, shipped as tau.tau. Re-export
# its public names so `import tau` exposes them, matching the build-tree module.
from .tau import *  # noqa: F401,F403
EOF

cat > "${stage}/pyproject.toml" <<'EOF'
[build-system]
requires = ["setuptools>=64"]
build-backend = "setuptools.build_meta"
EOF

cat > "${stage}/setup.py" <<EOF
import sys
import sysconfig

from setuptools import setup

try:
	from setuptools.command.bdist_wheel import bdist_wheel as _bdist_wheel
except ImportError:
	from wheel.bdist_wheel import bdist_wheel as _bdist_wheel


class bdist_wheel(_bdist_wheel):
	"""Tag the wheel for the interpreter that built the extension.

	A prebuilt extension carries no ext_modules for setuptools to derive the
	tags from, so the wheel would default to py3-none-any. The module is built
	against this interpreter's CPython ABI on this platform, so the tag comes
	from sys and sysconfig.
	"""

	def get_tag(self):
		python, abi, plat = super().get_tag()
		cp = "cp%d%d" % (sys.version_info[0], sys.version_info[1])
		if python in ("py3", "py2.py3"):
			python = cp
		if abi == "none":
			abi = cp
		if plat == "any":
			plat = sysconfig.get_platform().replace("-", "_").replace(".", "_")
		return python, abi, plat


setup(
	name="tau-nanobind",
	version="${version}",
	description="Python bindings for the Tau Language Framework LTL(ABA) API.",
	python_requires=">=3.9",
	packages=["tau"],
	package_data={"tau": ["*.so", "*.pyd", "tau_libs/*", "licenses/*",
		"licenses/*/*"]},
	cmdclass={"bdist_wheel": bdist_wheel},
)
EOF

"${python}" -m pip wheel --no-build-isolation --no-deps \
	-w "${out_dir}/wheel" "${stage}"

# Repair the wheel with the platform bundler. auditwheel and delocate retag the
# platform tag. delvewheel bundles the DLLs the module imports.
shopt -s nullglob
built=("${out_dir}/wheel"/tau_nanobind-*.whl)
if [ "${#built[@]}" -ne 1 ]; then
	echo "build-wheel: expected one built wheel, found ${#built[@]}" >&2
	exit 1
fi
rm -rf "${out_dir}/repaired"
case "${platform}" in
	linux)
		"${TAU_WHEEL_REPAIR_PYTHON:?}" -m auditwheel repair --exclude libgmp.so.10 \
			--exclude libcurl.so.4 \
			-w "${out_dir}/repaired" "${built[0]}" ;;
	macos)
		"${TAU_WHEEL_REPAIR_PROGRAM:?}" -w "${out_dir}/repaired" "${built[0]}" ;;
	windows)
		"${TAU_WHEEL_REPAIR_PROGRAM:?}" repair --add-path "${cvc5_prefix}/bin" \
			-w "${out_dir}/repaired" "${built[0]}" ;;
esac
repaired=("${out_dir}/repaired"/tau_nanobind-*.whl)
if [ "${#repaired[@]}" -ne 1 ]; then
	echo "build-wheel: the repair tool produced ${#repaired[@]} wheels" >&2
	exit 1
fi
# delvewheel keeps the file name, so the built wheel goes first.
rm -f "${built[0]}"
mv "${repaired[0]}" "${out_dir}/wheel/"
