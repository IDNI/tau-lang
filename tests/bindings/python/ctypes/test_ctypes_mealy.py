#!/usr/bin/env python3
"""
A synthesized machine through the ctypes step interface: the loop over
mealy_input_vars, mealy_step and mealy_state, the names mealy_output_vars
lists, a stream named state, escapes in a value, and reset.

Needs TAU_LTL_LIB naming libtau_ctypes and tau_lang.py on PYTHONPATH.
"""

import os
import shutil
import sys

import tau_lang as tau

def test_step_loop():
	handle = tau.synthesize("always o1[t] = i1[t].")
	assert tau.mealy_output_vars(handle) == ["o1"]
	assert tau.mealy_state(handle) == 0
	for step, value in enumerate(("T", "F", "T"), start=1):
		assert tau.mealy_input_vars(handle) == ["i1"]
		out = tau.mealy_step(handle, {"i1": value + "."})
		assert out == {"outputs": {"o1": value}, "state": step}, out
		assert tau.mealy_state(handle) == step
	tau.mealy_free(handle)

def test_internal_outputs_are_not_listed():
	handle = tau.synthesize("G (F (o1[t] = 1)) && G (F (o1[t] = 0)).")
	assert tau.mealy_output_vars(handle) == ["o1"]
	assert set(tau.mealy_step(handle, {})["outputs"]) == {"o1"}
	tau.mealy_free(handle)

def test_output_named_state():
	handle = tau.synthesize(
		"state : tau := out console. always state[t] = i1[t].")
	out = tau.mealy_step(handle, {"i1": "T."})
	assert out == {"outputs": {"state": "T"}, "state": 1}, out
	tau.mealy_free(handle)

def test_escapes_reach_the_engine():
	handle = tau.synthesize("always o1[t] = i1[t].")
	# The parse error quotes the value, so the raw last error shows what
	# reached the engine of each escape json.dumps writes.
	for value, escape, first_byte in (("é", b"u00e9", b"\xc3"),
			("\U0001F600", b"ud83d", b"\xf0")):
		try:
			tau.mealy_step(handle, {"i1": value})
		except tau.StepError:
			raw = tau._load().tau_lang_last_error()
			assert escape not in raw.lower() and first_byte in raw, raw
		else:
			raise AssertionError("a bad value stepped")
	tau.mealy_free(handle)

def test_malformed_input_is_refused():
	lib = tau._load()
	handle = tau.synthesize("always o1[t] = i1[t].")
	for text in (b'{"i1": "\\uD800"}', b'{"i1": "\\u12"}', b'{"i1": "\\q"}'):
		assert lib.tau_lang_mealy_step(handle, text) is None, text
		assert tau.last_error() == "malformed input JSON", tau.last_error()
	tau.mealy_free(handle)

def test_reset_releases_every_machine():
	handle = tau.synthesize("always o1[t] = i1[t].")
	assert tau.reset() > 0
	try:
		tau.mealy_step(handle, {"i1": "T."})
	except tau.StepError as e:
		assert "invalid handle" in str(e), str(e)
	else:
		raise AssertionError("a handle outlived the reset")
	handle = tau.synthesize("always o1[t] = i1[t].")
	assert tau.mealy_step(handle, {"i1": "T."})["outputs"] == {"o1": "T"}
	tau.mealy_free(handle)

def main():
	if not (shutil.which("ltlsynt") or os.environ.get("TAU_SPOT_BIN")):
		print("skipped: ltlsynt not available")
		sys.exit(77)
	test_step_loop()
	test_internal_outputs_are_not_listed()
	test_output_named_state()
	test_escapes_reach_the_engine()
	test_malformed_input_is_refused()
	test_reset_releases_every_machine()
	print("Test passed!")

if __name__ == "__main__":
	main()
