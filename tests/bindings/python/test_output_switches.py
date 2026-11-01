#!/usr/bin/env python3
"""
The output switches of the CLI, as module functions: the severity silences the
engine's own lines, a word it does not know is refused, and highlighting and
charvar reach what the engine prints and parses.
"""

import os
import subprocess
import sys

import tau_loader as tau

CHILD = """
import tau_loader as tau
tau.set_severity(%r)
assert tau.get_interpreter("o1[t]:sbf = 0.")
"""

def lines_of_a_run(level):
	env = dict(os.environ, PYTHONPATH=os.pathsep.join(sys.path))
	return subprocess.run([sys.executable, "-c", CHILD % level], env=env,
		capture_output=True, text=True, check=True).stdout

def test_severity():
	assert "Temporal normalization" in lines_of_a_run("info")
	assert "Temporal normalization" not in lines_of_a_run("error")

def test_a_bad_severity_is_refused():
	r = tau.set_severity("loud")
	assert not r
	assert r.report.has_error and "loud" in r.report.errors[0], r.report.errors
	assert tau.set_severity("error").value == "error"

def test_highlighting():
	spec = "o1[t]:sbf = 0 && o2[t]:sbf = 0."
	tau.set_highlighting(True)
	coloured = tau.get_interpreter(spec).value.current_spec()
	tau.set_highlighting(False)
	plain = tau.get_interpreter(spec).value.current_spec()
	assert "\x1b[" in coloured, coloured
	assert "\x1b[" not in plain, plain

def test_charvar():
	spec = "x = 1 && y = 1 && xy = 0."
	assert tau.sat(spec).value is False, "xy reads as x & y"
	tau.set_charvar(False)
	assert tau.sat(spec).value is True, "xy reads as one name"
	tau.set_charvar(True)

def test_indenting():
	spec = "always o1[t]:sbf = 0 && o2[t]:sbf = 1."
	tau.set_indenting(True)
	indented = tau.get_interpreter(spec).value.current_spec()
	tau.set_indenting(False)
	flat = tau.get_interpreter(spec).value.current_spec()
	assert indented != flat, flat

def main():
	test_severity()
	test_a_bad_severity_is_refused()
	test_highlighting()
	test_charvar()
	test_indenting()
	print("Test passed!")

if __name__ == "__main__":
	main()
