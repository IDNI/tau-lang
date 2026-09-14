f"""
Loads the tau module from a build directory.

Usage:
import tau_loader as tau

The module directory is resolved in this order.

1. If the TAU_PYTHON_MODULE_DIR environment variable is set, its value is
   used directly and no arguments are parsed.
2. Otherwise, if the first argument names a configure preset in the repo's
   CMakePresets.json, that preset's binaryDir is resolved (following its
   inherits chain the way CMake does) and the module directory is
   <binaryDir>/bindings/python/nanobind.
3. Otherwise, if the first argument names an existing directory, that
   directory is used, or its bindings/python/nanobind subdirectory when
   the directory itself holds no tau*.so or tau*.pyd module file.
4. With no arguments, the preset defaults to release.
5. Anything else raises FileNotFoundError naming what was tried.

"""

import sys
import os
import json
import glob

def _has_module(directory):
	return bool(glob.glob(os.path.join(directory, "tau*.so"))
		or glob.glob(os.path.join(directory, "tau*.pyd")))

def _repo_root():
	script_dir = os.path.dirname(os.path.abspath(__file__))
	return os.path.abspath(os.path.join(script_dir, '..', '..', '..'))

def _resolve_binary_dir(presets_by_name, name, source_dir):
	preset = presets_by_name.get(name)
	if preset is None:
		return None
	if "binaryDir" in preset:
		return preset["binaryDir"].replace("${sourceDir}", source_dir)
	inherits = preset.get("inherits")
	if not inherits:
		return None
	if isinstance(inherits, str):
		inherits = [inherits]
	# depth-first, first definition wins, matching CMake's own resolution
	for parent in inherits:
		found = _resolve_binary_dir(presets_by_name, parent, source_dir)
		if found is not None:
			return found
	return None

def _resolve_preset_module_dir(repo_root, name):
	presets_path = os.path.join(repo_root, "CMakePresets.json")
	if not os.path.isfile(presets_path):
		return None
	with open(presets_path) as f:
		data = json.load(f)
	presets_by_name = {p["name"]: p for p in data.get("configurePresets", [])}
	if name not in presets_by_name:
		return None
	binary_dir = _resolve_binary_dir(presets_by_name, name, repo_root)
	if binary_dir is None:
		return None
	return os.path.join(binary_dir, "bindings", "python", "nanobind")

def add_build_dir_to_path():
	env_dir = os.environ.get("TAU_PYTHON_MODULE_DIR")
	if env_dir:
		build_dir = os.path.abspath(env_dir)
		if os.path.exists(build_dir):
			sys.path.insert(0, build_dir)
			return "env", build_dir
		raise FileNotFoundError(f"Module directory {build_dir} not found")

	args = sys.argv[1:]
	repo_root = _repo_root()

	if not args:
		preset = "release"
		build_dir = _resolve_preset_module_dir(repo_root, preset)
		if build_dir and _has_module(build_dir):
			sys.path.insert(0, build_dir)
			return preset, build_dir
		raise FileNotFoundError(f"Preset {preset} module directory not found: {build_dir}")

	arg = args[0]

	preset_build_dir = _resolve_preset_module_dir(repo_root, arg)
	if preset_build_dir is not None:
		if _has_module(preset_build_dir):
			sys.path.insert(0, preset_build_dir)
			return arg, preset_build_dir
		raise FileNotFoundError(f"Preset {arg} module directory not found: {preset_build_dir}")

	if os.path.isdir(arg):
		build_dir = os.path.abspath(arg)
		if not _has_module(build_dir):
			nested = os.path.join(build_dir, 'bindings', 'python', 'nanobind')
			if os.path.isdir(nested):
				build_dir = nested
		if os.path.exists(build_dir):
			sys.path.insert(0, build_dir)
			return arg, build_dir
		raise FileNotFoundError(f"Build directory {build_dir} not found")

	raise FileNotFoundError(f"{arg} is not a known preset or an existing directory")

preset, build_dir = add_build_dir_to_path()

print(f"Running: {os.path.basename(sys.argv[0])}")
print(f"Build preset: {preset}")
print(f"Module build directory: {build_dir}")

# Import tau and make all its exports available at module level
import tau
from tau import *
