#!/usr/bin/env python3
"""
The ctypes library initializes the engine itself: every algebra of the pack
parses, and the TAU_* variables are read, a bad value being an error.

Needs TAU_LTL_LIB naming libtau_ctypes and tau_lang.py on PYTHONPATH.
TAU_TEST_PACK lists the algebras of the build, comma separated.
"""

import os
import shutil
import subprocess
import sys

import tau_lang as tau

PACK = os.environ.get("TAU_TEST_PACK", "qlt,bv").split(",")

def child(code, **env):
	return subprocess.run([sys.executable, "-c",
		"import tau_lang as tau\n" + code],
		env=dict(os.environ, **env), capture_output=True, text=True)

def test_qlt_spec():
	assert tau.decide("G (o1[t]:qlt = {1/2}:qlt).") == tau.Verdict.REALIZABLE, \
		tau.last_error()
	handle = tau.synthesize("G (o3[t]:qlt = {1/2}:qlt).")
	out = tau.mealy_step(handle, {})
	assert "1/2" in out["outputs"]["o3"], out
	tau.mealy_free(handle)

def test_bv_spec():
	assert tau.decide("always o2[t]:bv[8] = i2[t]:bv[8].") \
		== tau.Verdict.REALIZABLE, tau.last_error()

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
	if "qlt" in PACK:
		test_qlt_spec()
	if "bv" in PACK:
		test_bv_spec()
	test_environment_is_read()
	test_a_bad_variable_is_an_error()
	if "qlt" not in PACK or "bv" not in PACK:
		print("skipped: the typed specs need qlt and bv in the pack")
		sys.exit(77)
	print("Test passed!")

if __name__ == "__main__":
	main()
