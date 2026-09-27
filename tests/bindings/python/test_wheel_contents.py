#!/usr/bin/env python3
"""Verify a built tau nanobind wheel without rebuilding it.

Usage: test_wheel_contents.py <wheel>

Registered as the opt-in ctest `binding-python-wheel-check` when
TAU_WHEEL_PATH names a built wheel. It checks the wheel tag against this
interpreter, the module and its bundled cvc5 runtime, the module's relative
search path, and that no file in the wheel carries an absolute home path.
"""

import os
import re
import subprocess
import sys
import sysconfig
import tempfile
import zipfile

ABS = re.compile(
	rb"(?:/home/|/Users/)[A-Za-z0-9_./+-]{3,}"
	rb"|[A-Za-z]:\\Users\\[A-Za-z0-9_.\\+-]{3,}")


def absolute_paths(data):
	return set(ABS.findall(data))


def main(argv):
	if len(argv) != 2:
		print("usage: test_wheel_contents.py <wheel>", file=sys.stderr)
		return 2
	wheel = argv[1]
	if not os.path.isfile(wheel):
		print(f"wheel not found: {wheel}", file=sys.stderr)
		return 2

	cp = "cp%d%d" % sys.version_info[:2]
	plat = sysconfig.get_platform().replace("-", "_").replace(".", "_")
	arch = sysconfig.get_platform().split("-")[-1]
	name = os.path.basename(wheel)
	tag = f"{cp}-{cp}-{plat}"
	# auditwheel and delocate retag the wheel with the build host's floor, so
	# accept the manylinux or macosx family for this arch as well.
	family = re.compile(
		rf"{re.escape(cp)}-{re.escape(cp)}-(?:manylinux[0-9_]*|macosx_[0-9_]+)"
		rf"{re.escape(arch)}\.whl$")
	if tag not in name and not family.search(name):
		print(f"wheel tag mismatch: {name} is neither {tag} nor a repaired tag",
			file=sys.stderr)
		return 1

	z = zipfile.ZipFile(wheel)
	names = z.namelist()
	module = [n for n in names
		if re.match(r"tau/tau\.(?:cpython-\d+.*\.so|cp\d+.*\.pyd)$", n)]
	if not module:
		print(f"no tau module in wheel: {names}", file=sys.stderr)
		return 1
	if sys.platform == "win32":
		# delvewheel rewrites the imports and bundles the DLLs into .libs.
		if not [n for n in names if re.match(r"tau/\.libs/.*\.dll$", n)]:
			print(f"no bundled runtime under tau/.libs: {names}", file=sys.stderr)
			return 1
	elif sys.platform == "darwin":
		if not [n for n in names if re.match(r"tau/tau_libs/libcvc5.*\.dylib$", n)]:
			print(f"missing bundled runtime in tau/tau_libs: {names}", file=sys.stderr)
			return 1
	else:
		for lib in ("tau/tau_libs/libcvc5.so.1", "tau/tau_libs/libpoly.so.0",
				"tau/tau_libs/libpolyxx.so.0"):
			if lib not in names:
				print(f"missing bundled runtime {lib}", file=sys.stderr)
				return 1

	with tempfile.TemporaryDirectory() as tmp:
		z.extract(module[0], tmp)
		mp = os.path.join(tmp, module[0])
		# A Windows DLL has no search path; delvewheel resolves it instead.
		if sys.platform != "win32":
			if sys.platform == "darwin":
				otool = subprocess.run(["otool", "-l", mp],
					capture_output=True, text=True, check=True).stdout
				rpaths = re.findall(r"^\s*path (\S+)", otool, re.M)
				if "@loader_path/tau_libs" not in rpaths:
					print(f"module RPATHs are not relative: {rpaths}",
						file=sys.stderr)
					return 1
			else:
				readelf = subprocess.run(["readelf", "-d", mp],
					capture_output=True, text=True, check=True).stdout
				m = re.search(r"RUNPATH.*\[(.*)\]", readelf)
				if not m:
					print("module has no RUNPATH", file=sys.stderr)
					return 1
				if m.group(1) != "$ORIGIN/tau_libs":
					print(f"module RUNPATH is not relative: {m.group(1)}",
						file=sys.stderr)
					return 1
		module_abs = absolute_paths(open(mp, "rb").read())
		if module_abs:
			print(f"module carries {len(module_abs)} absolute paths", file=sys.stderr)
			return 1

	dep_abs = 0
	for n in names:
		if n.endswith((".so", ".pyd", ".dylib", ".dll")) or ".so." in n:
			dep_abs += len(absolute_paths(z.read(n)))
	print(f"wheel-ok tag={tag} module={module[0]} "
		f"module_absolute=0 bundled_absolute={dep_abs}")
	# A wheel carrying an absolute store, build, or home path is not shippable.
	return 0 if dep_abs == 0 else 1


if __name__ == "__main__":
	sys.exit(main(sys.argv))
