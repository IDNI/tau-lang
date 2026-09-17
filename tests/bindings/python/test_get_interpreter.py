#!/usr/bin/env python3
"""
Test script checking that the interpreter can be created, in both shapes the
module offers: the plain one (the value, None on failure) and the `_result`
twin carrying the diagnostics report.
"""

import tau_loader as tau

def main():
	# Make the interpreter for a given specification as a string
	i = tau.get_interpreter("o[t] = i[t].")
	assert i is not None, "Failed to create interpreter"

	assert tau.get_interpreter("this is not a spec ((") is None

	r = tau.get_interpreter_result("o[t] = i[t].")
	assert r, f"Failed to create interpreter: {r.report.errors}"
	assert r.value is not None

	bad = tau.get_interpreter_result("this is not a spec ((")
	assert not bad and bad.value is None
	assert bad.report.has_error and bad.report.errors

	print("Test passed!")

if __name__ == "__main__":
	main()
