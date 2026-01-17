f"""
Loads a tau module from build directories

Usage:
import tau_loader as tau

Each script using this module accepts build type as a first argument and toolchain as a second argument.
It adds the proper build directory path to the Python path.

Lower case build types (debug, release, relwithdebinfo) have build dir under build or build/$BUILD_TYPE directories
Capitalized build types (Release, Debug, RelWithDebInfo) have build dir build-$BUILD_TYPE

"""

import sys
import os

def add_build_dir_to_path():
	"""Parse command line arguments for build type"""
	preset = "Release"
	toolchain = "gcc"
	old = True

	args = sys.argv[1:]
	if args:
		i = 0
		if args[i] in ("debug", "release", "relwithdebinfo"):
			preset = args[i]
			old = False
			i += 1
		else:
			if args[i] in ("Release", "Debug", "RelWithDebInfo"):
				preset = args[i]
				old = True
				i += 1
		i += 1
		if i < len(args) and args[i] in ("gcc", "clang"):
			toolchain = args[i]
			i += 1

	# Add the build directory to Python path to use local build
	script_dir = os.path.dirname(os.path.abspath(__file__))
	build_dir = os.path.join(script_dir, '..', '..', '..')
	if old:
		build_dir = os.path.join(build_dir, f'build-{preset}')
	else:
		build_dir = os.path.join(build_dir, 'build')
		if preset != "release":
			build_dir = os.path.join(build_dir, preset)
		if toolchain != "gcc":
			build_dir = os.path.join(build_dir, toolchain)
	build_dir = os.path.join(build_dir, 'bindings', 'python', 'nanobind')
	build_dir = os.path.abspath(build_dir)

	if os.path.exists(build_dir):
		sys.path.insert(0, build_dir)
	else:
		raise FileNotFoundError(f"Build directory {build_dir} not found")

	return preset, build_dir

preset, build_dir = add_build_dir_to_path()

print(f"Running: {os.path.basename(sys.argv[0])}")
print(f"Build preset: {preset}")
print(f"Module build directory: {build_dir}")

# Import tau and make all its exports available at module level
import tau
from tau import *
