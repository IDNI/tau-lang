#!/usr/bin/env python3
"""
The ctypes library initializes the engine itself: every algebra of the pack
parses, and the TAU_* variables are read, a bad value being an error.

Needs TAU_LTL_LIB naming libtau_ctypes and tau_lang.py on PYTHONPATH.
"""

import os
import shutil
import subprocess
import sys

import tau_lang as tau

def child(code, **env):
	return subprocess.run([sys.executable, "-c",
		"import tau_lang as tau\n" + code],
		env=dict(os.environ, **env), capture_output=True, text=True)

def test_typed_specs():
	assert tau.decide("G (o1[t]:qlt = {1/2}:qlt).") == tau.Verdict.REALIZABLE, \
		tau.last_error()
	assert tau.decide("always o2[t]:bv[8] = i2[t]:bv[8].") \
		== tau.Verdict.REALIZABLE, tau.last_error()
	handle = tau.synthesize("G (o3[t]:qlt = {1/2}:qlt).")
	out = tau.mealy_step(handle, {})
	assert "1/2" in out["outputs"]["o3"], out
	tau.mealy_free(handle)

def test_environment_is_read():
	r = child("print(tau.get_limit('max_fixpoint_steps'))",
		TAU_MAX_FIXPOINT_STEPS="77")
	assert r.stdout.strip() == "77", r

def test_a_bad_variable_is_an_error():
	r = child("v = tau.decide('always o1[t] = i1[t].')\n"
		"print(v == tau.Verdict.INTERNAL_ERR)\n"
		"print(tau.last_error())", TAU_MAX_FIXPOINT_STEPS="many")
	assert r.stdout.startswith("True\n"), r
	assert "init" in r.stdout, r

def main():
	if not (shutil.which("ltlsynt") or os.environ.get("TAU_SPOT_BIN")):
		print("skipped: ltlsynt not available")
		sys.exit(77)
	test_typed_specs()
	test_environment_is_read()
	test_a_bad_variable_is_an_error()
	print("Test passed!")

if __name__ == "__main__":
	main()
