#!/usr/bin/env python3
"""
Test script checking that the interpreter can be created, and that a failed
parse reports through the returned result instead of a bare value.
"""

import tau_loader as tau

def main():
	# Make the interpreter for a given specification as a string
	r = tau.get_interpreter("o[t] = i[t].")
	assert r, f"Failed to create interpreter: {r.report.errors}"
	assert r.value is not None

	bad = tau.get_interpreter("this is not a spec ((")
	assert not bad and bad.value is None
	assert bad.report.has_error and bad.report.errors

	print("Test passed!")

if __name__ == "__main__":
	main()
