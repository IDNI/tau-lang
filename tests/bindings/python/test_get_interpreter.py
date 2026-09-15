#!/usr/bin/env python3
"""
Test script checking that the interpreter can be created.
"""

import tau_loader as tau

def main():
	# Make the interpreter for a given specification as a string
	r = tau.get_interpreter("o[t] = i[t].")
	assert r, f"Failed to create interpreter: {r.report.errors}"
	i = r.value

	print("Test passed!")

if __name__ == "__main__":
	main()
